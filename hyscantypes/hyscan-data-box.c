/*
 * \file hyscan-data-box.h
 *
 * \brief Заголовочный файл класса работы с параметрами.
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2016
 * \license Проприетарная лицензия ООО "Экран"
 *
 */

#include "hyscan-data-box.h"
#include "hyscan-types-marshallers.h"
#include <gio/gio.h>
#include <string.h>

enum
{
  SIGNAL_SET,
  SIGNAL_CHANGED,
  SIGNAL_LAST
};

/* Значение параметра. */
typedef struct
{
  GQuark                       id;                     /* Идентификатор названия параметра. */
  HyScanDataSchemaType         type;                   /* Тип параметра. */
  GVariant                    *value;                  /* Значение параметра. */
  GVariantClass                value_type;             /* Тип значения параметра. */
  guint32                      mod_count;              /* Счётчик изменений значений параметра. */
  gboolean                     readonly;               /* Параметр доступен только для чтения. */
} HyScanDataBoxParam;

struct _HyScanDataBoxPrivate
{
  gchar                      **keys_list;              /* Список параметров. */
  GHashTable                  *params;                 /* Значения параметров. */

  guint32                      mod_count;              /* Глобальный счётчик изменений значений параметра. */

  GRWLock                      lock;                   /* Блокировка. */
};

static void    hyscan_data_box_object_constructed      (GObject               *object);
static void    hyscan_data_box_object_finalize         (GObject               *object);

static void    hyscan_data_box_param_free              (gpointer               param);

static gboolean hyscan_data_box_signal_accumulator     (GSignalInvocationHint *ihint,
                                                        GValue                *return_accu,
                                                        const GValue          *handler_return,
                                                        gpointer               data);

static guint   hyscan_data_box_signals[SIGNAL_LAST] = { 0 };

G_DEFINE_TYPE_WITH_PRIVATE (HyScanDataBox, hyscan_data_box, HYSCAN_TYPE_DATA_SCHEMA);

static void hyscan_data_box_class_init( HyScanDataBoxClass *klass )
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = hyscan_data_box_object_constructed;
  object_class->finalize = hyscan_data_box_object_finalize;

  hyscan_data_box_signals[SIGNAL_SET] =
    g_signal_new ("set", HYSCAN_TYPE_DATA_BOX, G_SIGNAL_RUN_LAST, 0,
                  hyscan_data_box_signal_accumulator, NULL,
                  g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER,
                  G_TYPE_BOOLEAN,
                  2, G_TYPE_POINTER, G_TYPE_POINTER);

  hyscan_data_box_signals[SIGNAL_CHANGED] =
    g_signal_new ("changed", HYSCAN_TYPE_DATA_BOX, G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED, 0,
                  NULL, NULL,
                  g_cclosure_marshal_VOID__STRING, G_TYPE_NONE, 1, G_TYPE_STRING);
}

static void
hyscan_data_box_init (HyScanDataBox *data_box)
{
  data_box->priv = hyscan_data_box_get_instance_private (data_box);
}

static void
hyscan_data_box_object_constructed (GObject *object)
{
  HyScanDataBox *data_box = HYSCAN_DATA_BOX (object);
  HyScanDataBoxPrivate *priv = data_box->priv;
  HyScanDataSchema *schema = HYSCAN_DATA_SCHEMA (object);

  gint i;

  G_OBJECT_CLASS (hyscan_data_box_parent_class)->constructed (object);

  g_rw_lock_init (&priv->lock);

  /* Таблица со значениями параметров. */
  priv->params = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, hyscan_data_box_param_free);

  /* Список параметров. */
  priv->keys_list = hyscan_data_schema_list_keys (schema);
  if (priv->keys_list == NULL)
    return;

  for (i = 0; priv->keys_list[i] != NULL; i++)
    {
      HyScanDataBoxParam *param = g_slice_new0 (HyScanDataBoxParam);

      param->type = hyscan_data_schema_key_get_type (schema, priv->keys_list[i]);
      param->id = g_quark_from_string (priv->keys_list[i]);
      param->readonly = hyscan_data_schema_key_is_readonly (schema, priv->keys_list[i]);

      switch (param->type)
        {
        case HYSCAN_DATA_SCHEMA_TYPE_BOOLEAN:
          param->value_type = G_VARIANT_CLASS_BOOLEAN;
          break;

        case HYSCAN_DATA_SCHEMA_TYPE_INTEGER:
          param->value_type = G_VARIANT_CLASS_INT64;
          break;

        case HYSCAN_DATA_SCHEMA_TYPE_DOUBLE:
          param->value_type = G_VARIANT_CLASS_DOUBLE;
          break;

        case HYSCAN_DATA_SCHEMA_TYPE_STRING:
          param->value_type = G_VARIANT_CLASS_STRING;
          break;

        case HYSCAN_DATA_SCHEMA_TYPE_ENUM:
          param->value_type = G_VARIANT_CLASS_INT64;
          break;

        default:
          param->value_type = 0;
          break;
        }

      g_hash_table_insert (priv->params, (gpointer)priv->keys_list[i], param);
    }
}

static void
hyscan_data_box_object_finalize (GObject *object)
{
  HyScanDataBox *data_box = HYSCAN_DATA_BOX (object);
  HyScanDataBoxPrivate *priv = data_box->priv;

  g_rw_lock_clear (&priv->lock);
  g_hash_table_unref (priv->params);
  g_strfreev (priv->keys_list);

  G_OBJECT_CLASS (hyscan_data_box_parent_class)->finalize (object);
}

/* Функция освобождает память занятую структурой с параметром. */
static void
hyscan_data_box_param_free (gpointer data)
{
  HyScanDataBoxParam *param = data;

  g_clear_pointer (&param->value, g_variant_unref);
  g_slice_free (HyScanDataBoxParam, param);
}

/* Функция аккумулирует ответы всех callback'ов сигнала set.
 * Здесь действует обратная логика, если какой-либо из callback'ов
 * вернёт FALSE, аккумулятор вернёт TRUE. Это будет сигналом
 * прекратить обработку запроса установки параметров. */
static gboolean
hyscan_data_box_signal_accumulator (GSignalInvocationHint *ihint,
                                    GValue                *return_accu,
                                    const GValue          *handler_return,
                                    gpointer               data)
{
  if (!g_value_get_boolean (handler_return))
    {
      g_value_set_boolean (return_accu, TRUE);
      return FALSE;
    }

  return TRUE;
}

/* Функция создаёт новый объект HyScanDataBox. */
HyScanDataBox *
hyscan_data_box_new_from_string (const gchar  *schema_data,
                                 const gchar  *schema_id)
{
  return g_object_new (HYSCAN_TYPE_DATA_BOX,
                       "schema-data", schema_data,
                       "schema-id", schema_id,
                       NULL);
}

/* Функция создаёт новый объект HyScanDataBox. */
HyScanDataBox *
hyscan_data_box_new_from_file (const gchar *schema_path,
                               const gchar *schema_id)
{
  gchar *schema_data;
  gpointer data_box;

  if (!g_file_get_contents (schema_path, &schema_data, NULL, NULL))
    return NULL;

  data_box = g_object_new (HYSCAN_TYPE_DATA_BOX,
                           "schema-data", schema_data,
                           "schema-id", schema_id,
                           NULL);
  g_free (schema_data);

  return data_box;
}

/* Функция создаёт новый объект HyScanDataBox. */
HyScanDataBox *
hyscan_data_box_new_from_resource (const gchar *schema_resource,
                                   const gchar *schema_id)
{
  const gchar *schema_data;
  GBytes *resource;
  gpointer data_box;

  resource = g_resources_lookup_data (schema_resource, 0, NULL);
  if (resource == NULL)
    return NULL;

  schema_data = g_bytes_get_data (resource, NULL);
  data_box = g_object_new (HYSCAN_TYPE_DATA_BOX,
                           "schema-data", schema_data,
                           "schema-id", schema_id,
                           NULL);
  g_bytes_unref (resource);

  return data_box;
}

/* Функция возвращает значение счётчика изменений параметра. */
guint32
hyscan_data_box_get_mod_count (HyScanDataBox *data_box,
                               const gchar   *name)
{
  HyScanDataBoxParam *param;

  g_return_val_if_fail (HYSCAN_IS_DATA_BOX (data_box), 0);

  if (name == NULL)
    return data_box->priv->mod_count;

  param = g_hash_table_lookup (data_box->priv->params, name);
  if (param != NULL)
    return param->mod_count;

  return 0;
}

gboolean
hyscan_data_box_set (HyScanDataBox      *data_box,
                     const gchar *const *names,
                     GVariant          **values)
{
  HyScanDataBoxPrivate *priv;
  HyScanDataSchema *schema;
  gboolean status = FALSE;
  gboolean cancel = FALSE;
  gint i;

  g_return_val_if_fail (HYSCAN_IS_DATA_BOX (data_box), FALSE);

  priv = data_box->priv;
  schema = HYSCAN_DATA_SCHEMA (data_box);

  /* Проверяем параметы. */
  for (i = 0; names[i] != NULL; i++)
    {
      HyScanDataBoxParam *param;

      param = g_hash_table_lookup (priv->params, names[i]);
      if (param == NULL)
        return FALSE;

      /* Параметр только для чтения. */
      if (param->readonly)
        return FALSE;

      /* Установка значения по умолчанию. */
      if (values[i] == NULL)
        continue;

      /* Ошибка в типе нового значения параметра. */
      if (param->value_type != g_variant_classify (values[i]))
        return FALSE;

      /* Недопустимое значение параметра. */
      if (!hyscan_data_schema_key_check (schema, names[i], values[i]))
        return FALSE;
    }

  g_rw_lock_writer_lock (&priv->lock);

  /* Сигнал перед изменением параметров. */
  g_signal_emit (data_box, hyscan_data_box_signals[SIGNAL_SET], 0, names, values, &cancel);
  if (cancel)
    goto exit;

  /* Изменяем параметы. */
  for (i = 0; names[i] != NULL; i++)
    {
      HyScanDataBoxParam *param;

      param = g_hash_table_lookup (priv->params, names[i]);

      /* Устанавливаем новое значение. */
      g_clear_pointer (&param->value, g_variant_unref);
      if (values[i] != NULL)
        param->value = g_variant_ref_sink (values[i]);
      param->mod_count += 1;

      /* Сигнал об изменении параметра. */
      g_signal_emit (data_box, hyscan_data_box_signals[SIGNAL_CHANGED], param->id, names[i]);
    }

  priv->mod_count += 1;
  status = TRUE;

exit:
  g_rw_lock_writer_unlock (&priv->lock);

  return status;
}


gboolean
hyscan_data_box_get (HyScanDataBox      *data_box,
                     const gchar *const *names,
                     GVariant          **values)
{
  HyScanDataBoxPrivate *priv;
  HyScanDataSchema *schema;
  gint i;

  g_return_val_if_fail (HYSCAN_IS_DATA_BOX (data_box), FALSE);

  priv = data_box->priv;
  schema = HYSCAN_DATA_SCHEMA (data_box);

  if (names == NULL || values == NULL)
    return FALSE;

  /* Проверяем параметы. */
  for (i = 0; names[i] != NULL; i++)
    if (!g_hash_table_contains (priv->params, names[i]))
      return FALSE;

  g_rw_lock_reader_lock (&priv->lock);

  /* Считываем параметы. */
  for (i = 0; names[i] != NULL; i++)
    {
      HyScanDataBoxParam *param = g_hash_table_lookup (priv->params, names[i]);

      /* Считываем значение параметра. */
      if (param->value != NULL)
        values[i] = g_variant_ref (param->value);
      else
        values[i] = hyscan_data_schema_key_get_default (schema, names[i]);
    }

  g_rw_lock_reader_unlock (&priv->lock);

  return TRUE;
}

gboolean
hyscan_data_box_set_default (HyScanDataBox *data_box,
                             const gchar   *name)
{
  const gchar *names[2];
  GVariant *values[1];

  names[0] = name;
  names[1] = NULL;

  values[0] = NULL;

  return hyscan_data_box_set (data_box, names, values);
}

/* Функция устанавливает значение параметра типа BOOLEAN. */
gboolean
hyscan_data_box_set_boolean (HyScanDataBox *data_box,
                             const gchar   *name,
                             gboolean       value)
{
  const gchar *names[2];
  GVariant *values[1];

  names[0] = name;
  names[1] = NULL;

  values[0] = g_variant_new_boolean (value);

  if (hyscan_data_box_set (data_box, names, values))
    return TRUE;

  g_variant_unref (values[0]);

  return FALSE;
}

/* Функция устанавливает значение параметра типа INTEGER. */
gboolean
hyscan_data_box_set_integer (HyScanDataBox *data_box,
                             const gchar   *name,
                             gint64         value)
{
  const gchar *names[2];
  GVariant *values[1];

  names[0] = name;
  names[1] = NULL;

  values[0] = g_variant_new_int64 (value);

  if (hyscan_data_box_set (data_box, names, values))
    return TRUE;

  g_variant_unref (values[0]);

  return FALSE;
}

/* Функция устанавливает значение параметра типа DOUBLE. */
gboolean
hyscan_data_box_set_double (HyScanDataBox *data_box,
                            const gchar   *name,
                            gdouble        value)
{
  const gchar *names[2];
  GVariant *values[1];

  names[0] = name;
  names[1] = NULL;

  values[0] = g_variant_new_double (value);

  if (hyscan_data_box_set (data_box, names, values))
    return TRUE;

  g_variant_unref (values[0]);

  return FALSE;
}

/* Функция устанавливает значение параметра типа STRING. */
gboolean
hyscan_data_box_set_string (HyScanDataBox *data_box,
                            const gchar   *name,
                            const gchar   *value)
{
  const gchar *names[2];
  GVariant *values[1];

  names[0] = name;
  names[1] = NULL;

  values[0] = g_variant_new_string (value);

  if (hyscan_data_box_set (data_box, names, values))
    return TRUE;

  g_variant_unref (values[0]);

  return FALSE;
}

/* Функция устанавливает значение параметра типа ENUM. */
gboolean
hyscan_data_box_set_enum (HyScanDataBox *data_box,
                          const gchar   *name,
                          gint64         value)
{
  const gchar *names[2];
  GVariant *values[1];

  names[0] = name;
  names[1] = NULL;

  values[0] = g_variant_new_int64 (value);

  if (hyscan_data_box_set (data_box, names, values))
    return TRUE;

  g_variant_unref (values[0]);

  return FALSE;
}

/* Функция считывает значение параметра типа BOOLEAN. */
gboolean
hyscan_data_box_get_boolean (HyScanDataBox *data_box,
                             const gchar   *name,
                             gboolean      *value)
{
  const gchar *names[2];
  GVariant *values[1];

  names[0] = name;
  names[1] = NULL;

  if (!hyscan_data_box_get (data_box, names, values))
    return FALSE;

  *value = g_variant_get_boolean (values[0]);
  g_variant_unref (values[0]);

  return TRUE;
}

/* Функция считывает значение параметра типа INTEGER. */
gboolean
hyscan_data_box_get_integer (HyScanDataBox *data_box,
                             const gchar   *name,
                             gint64        *value)
{
  const gchar *names[2];
  GVariant *values[1];

  names[0] = name;
  names[1] = NULL;

  if (!hyscan_data_box_get (data_box, names, values))
    return FALSE;

  *value = g_variant_get_int64 (values[0]);
  g_variant_unref (values[0]);

  return TRUE;
}

/* Функция считывает значение параметра типа DOUBLE. */
gboolean
hyscan_data_box_get_double (HyScanDataBox *data_box,
                            const gchar   *name,
                            gdouble       *value)
{
  const gchar *names[2];
  GVariant *values[1];

  names[0] = name;
  names[1] = NULL;

  if (!hyscan_data_box_get (data_box, names, values))
    return FALSE;

  *value = g_variant_get_double (values[0]);
  g_variant_unref (values[0]);

  return TRUE;
}

/* Функция считывает значение параметра типа STRING. */
gchar *
hyscan_data_box_get_string (HyScanDataBox *data_box,
                            const gchar   *name)
{
  const gchar *names[2];
  GVariant *values[1];
  gchar *value;

  names[0] = name;
  names[1] = NULL;

  if (!hyscan_data_box_get (data_box, names, values))
    return NULL;

  if (values[0] == NULL)
    return NULL;

  value = g_variant_dup_string (values[0], NULL);
  g_variant_unref (values[0]);

  return value;
}

/* Функция считывает значение параметра типа ENUM. */
gboolean
hyscan_data_box_get_enum (HyScanDataBox *data_box,
                          const gchar   *name,
                          gint64        *value)
{
  const gchar *names[2];
  GVariant *values[1];

  names[0] = name;
  names[1] = NULL;

  if (!hyscan_data_box_get (data_box, names, values))
    return FALSE;

  *value = g_variant_get_int64 (values[0]);
  g_variant_unref (values[0]);

  return TRUE;
}
