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
  PROP_O,
  PROP_SCHEMA_DATA,
  PROP_SCHEMA_ID
};

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
  HyScanDataSchemaKeyType      type;                   /* Тип параметра. */
  GVariant                    *value;                  /* Значение параметра. */
  GVariantClass                value_type;             /* Тип значения параметра. */
  guint32                      mod_count;              /* Счётчик изменений значений параметра. */
  HyScanDataSchemaKeyAccess    access;                 /* Атрибут доступа к параметру. */
} HyScanDataBoxParam;

struct _HyScanDataBoxPrivate
{
  gchar                       *schema_data;            /* Описание схемы в XML. */
  gchar                       *schema_id;              /* Идентификатор схемы. */
  HyScanDataSchema            *schema;                 /* Схема параметров. */

  gchar                      **keys_list;              /* Список параметров. */
  GHashTable                  *params;                 /* Значения параметров. */

  guint32                      mod_count;              /* Глобальный счётчик изменений значений параметра. */

  GRWLock                      lock;                   /* Блокировка. */
};

static void    hyscan_data_box_interface_init          (HyScanParamInterface  *iface);
static void    hyscan_data_box_set_property            (GObject               *object,
                                                        guint                  prop_id,
                                                        const GValue          *value,
                                                        GParamSpec            *pspec);
static void    hyscan_data_box_object_constructed      (GObject               *object);
static void    hyscan_data_box_object_finalize         (GObject               *object);

static void    hyscan_data_box_param_free              (gpointer               param);

static gboolean hyscan_data_box_signal_accumulator     (GSignalInvocationHint *ihint,
                                                        GValue                *return_accu,
                                                        const GValue          *handler_return,
                                                        gpointer               data);

static guint   hyscan_data_box_signals[SIGNAL_LAST] = { 0 };

G_DEFINE_TYPE_WITH_CODE (HyScanDataBox, hyscan_data_box, G_TYPE_OBJECT,
                         G_ADD_PRIVATE (HyScanDataBox)
                         G_IMPLEMENT_INTERFACE (HYSCAN_TYPE_PARAM, hyscan_data_box_interface_init));


static void hyscan_data_box_class_init( HyScanDataBoxClass *klass )
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = hyscan_data_box_set_property;

  object_class->constructed = hyscan_data_box_object_constructed;
  object_class->finalize = hyscan_data_box_object_finalize;

  g_object_class_install_property (object_class, PROP_SCHEMA_DATA,
    g_param_spec_string ("schema-data", "SchemaData", "Schema data", NULL,
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_property (object_class, PROP_SCHEMA_ID,
    g_param_spec_string ("schema-id", "SchemaID", "Schema id", NULL,
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

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
hyscan_data_box_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  HyScanDataBox *data_box = HYSCAN_DATA_BOX (object);
  HyScanDataBoxPrivate *priv = data_box->priv;

  switch (prop_id)
    {
    case PROP_SCHEMA_DATA:
      priv->schema_data = g_value_dup_string (value);
      break;

    case PROP_SCHEMA_ID:
      priv->schema_id = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hyscan_data_box_object_constructed (GObject *object)
{
  HyScanDataBox *data_box = HYSCAN_DATA_BOX (object);
  HyScanDataBoxPrivate *priv = data_box->priv;

  gint i;

  G_OBJECT_CLASS (hyscan_data_box_parent_class)->constructed (object);

  g_rw_lock_init (&priv->lock);

  /* Схема параметров. */
  priv->schema = hyscan_data_schema_new_from_string (priv->schema_data, priv->schema_id);

  /* Таблица со значениями параметров. */
  priv->params = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, hyscan_data_box_param_free);

  /* Список параметров. */
  priv->keys_list = hyscan_data_schema_list_keys (priv->schema);
  if (priv->keys_list == NULL)
    return;

  for (i = 0; priv->keys_list[i] != NULL; i++)
    {
      HyScanDataBoxParam *param = g_slice_new0 (HyScanDataBoxParam);

      param->type = hyscan_data_schema_key_get_type (priv->schema, priv->keys_list[i]);
      param->id = g_quark_from_string (priv->keys_list[i]);
      param->access = hyscan_data_schema_key_get_access (priv->schema, priv->keys_list[i]);

      switch (param->type)
        {
        case HYSCAN_DATA_SCHEMA_KEY_BOOLEAN:
          param->value_type = G_VARIANT_CLASS_BOOLEAN;
          break;

        case HYSCAN_DATA_SCHEMA_KEY_INTEGER:
          param->value_type = G_VARIANT_CLASS_INT64;
          break;

        case HYSCAN_DATA_SCHEMA_KEY_DOUBLE:
          param->value_type = G_VARIANT_CLASS_DOUBLE;
          break;

        case HYSCAN_DATA_SCHEMA_KEY_STRING:
          param->value_type = G_VARIANT_CLASS_STRING;
          break;

        case HYSCAN_DATA_SCHEMA_KEY_ENUM:
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

  g_object_unref (priv->schema);
  g_free (priv->schema_data);
  g_free (priv->schema_id);

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

/* Функция возвращает строку с текущими значениями параметров. */
gchar *
hyscan_data_box_serialize (HyScanDataBox *data_box)
{
  HyScanDataBoxPrivate *priv;
  GVariantDict *dict;
  GVariant *vdict;

  gboolean has_params = FALSE;
  gchar *sparams = NULL;
  gsize i;

  g_return_val_if_fail (HYSCAN_IS_DATA_BOX (data_box), NULL);

  priv = data_box->priv;

  if (priv->keys_list == NULL)
    return NULL;

  g_rw_lock_reader_lock (&priv->lock);

  dict = g_variant_dict_new (NULL);

  /* Проверяем все параметры на предмет установки значения
   * отличного от значения по умолчанию. */
  for (i = 0; priv->keys_list[i] != NULL; i++)
    {
      HyScanDataBoxParam *param;
      GVariant *value;

      param = g_hash_table_lookup (priv->params, priv->keys_list[i]);
      if ((param == NULL) || (param->access == HYSCAN_DATA_SCHEMA_ACCESS_READONLY))
        continue;

      value = param->value;
      if (value == NULL)
        continue;

      /* Добавляем изменённые значения в массив. */
      g_variant_dict_insert_value (dict, priv->keys_list[i], value);
      has_params = TRUE;
    }

  if (has_params)
    {
      vdict = g_variant_dict_end (dict);
      sparams = g_variant_print (vdict, TRUE);
      g_variant_unref (vdict);
    }

  g_variant_dict_unref (dict);

  g_rw_lock_reader_unlock (&priv->lock);

  return sparams;
}

/* Функция устанавливает значения параметров. */
gboolean
hyscan_data_box_deserialize (HyScanDataBox *data_box,
                             const gchar   *svalues)
{
  GVariant *vdict;
  gboolean status;

  gchar **names;
  GVariant **values;

  gsize n_params;
  gsize i;

  g_return_val_if_fail (HYSCAN_IS_DATA_BOX (data_box), FALSE);

  /* Загружаем значения из строки. */
  vdict = g_variant_parse (NULL, svalues, NULL, NULL, NULL);
  if (vdict == NULL)
    return FALSE;

  n_params = g_variant_n_children (vdict);
  names = g_new0 (gchar*, n_params + 1);
  values = g_new0 (GVariant*, n_params + 1);

  /* Массивы имён параметров и их значения. */
  for (i = 0; i < n_params; i++)
    {
      GVariant *param;
      GVariant *value;
      gchar *key;

      param = g_variant_get_child_value (vdict, i);
      g_variant_get (param, "{sv}", &key, &value);

      names[i] = key;
      values[i] = value;

      g_variant_unref (param);
    }

  g_variant_unref (vdict);

  /* Устанавливаем новые значения. */
  status = hyscan_param_set (HYSCAN_PARAM (data_box), (const gchar* const*)names, values);
  for (i = 0; i < n_params; i++)
    {
      g_free (names[i]);
      g_variant_unref (values[i]);
    }

  g_free (names);
  g_free (values);

  return status;
}

/* Функция возвращает схему параметров. */
static HyScanDataSchema *
hyscan_data_box_schema (HyScanParam *param)
{
  HyScanDataBox *data_box = HYSCAN_DATA_BOX (param);
  HyScanDataBoxPrivate *priv = data_box->priv;

  return g_object_ref (priv->schema);
}

/* Функция устанавливает значения параметров. */
static gboolean
hyscan_data_box_set (HyScanParam         *param,
                     const gchar *const  *names,
                     GVariant           **values)
{
  HyScanDataBox *data_box = HYSCAN_DATA_BOX (param);
  HyScanDataBoxPrivate *priv = data_box->priv;
  HyScanDataSchema *schema = priv->schema;
  gboolean status = FALSE;
  gboolean cancel = FALSE;
  guint8 *defaults;
  guint n_names;
  guint i;

  if (names == NULL || values == NULL)
    return FALSE;

  n_names = g_strv_length ((gchar **)names);
  defaults = g_malloc0 (n_names);

  /* Проверяем параметы. */
  for (i = 0; i < n_names; i++)
    {
      HyScanDataBoxParam *param;

      param = g_hash_table_lookup (priv->params, names[i]);
      if (param == NULL)
        goto exit;

      /* Параметр только для чтения. */
      if (param->access == HYSCAN_DATA_SCHEMA_ACCESS_READONLY)
        goto exit;

      /* Установка значения по умолчанию. */
      if (values[i] == NULL)
        {
          values[i] = hyscan_data_schema_key_get_default (schema, names[i]);
          defaults[i] = TRUE;
          continue;
        }

      /* Ошибка в типе нового значения параметра. */
      if (param->value_type != g_variant_classify (values[i]))
        goto exit;

      /* Недопустимое значение параметра. */
      if (!hyscan_data_schema_key_check (schema, names[i], values[i]))
        goto exit;
    }

  /* Сигнал перед изменением параметров. */
  g_signal_emit (data_box, hyscan_data_box_signals[SIGNAL_SET], 0, names, values, &cancel);

  for (i = 0; i < n_names; i++)
    if (defaults[i])
      g_clear_pointer (&values[i], g_variant_unref);

  if (cancel)
    goto exit;

  g_rw_lock_writer_lock (&priv->lock);

  /* Изменяем параметы. */
  for (i = 0; i < n_names; i++)
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

  g_rw_lock_writer_unlock (&priv->lock);

exit:
  g_free (defaults);
  return status;
}

/* Функция считывает значения параметров. */
static gboolean
hyscan_data_box_get (HyScanParam         *param,
                     const gchar *const  *names,
                     GVariant           **values)
{
  HyScanDataBox *data_box = HYSCAN_DATA_BOX (param);
  HyScanDataBoxPrivate *priv = data_box->priv;
  HyScanDataSchema *schema = priv->schema;
  guint n_names;
  guint i;

  if (names == NULL || values == NULL)
    return FALSE;

  n_names = g_strv_length ((gchar **)names);

  /* Проверяем параметы. */
  for (i = 0; i < n_names; i++)
    {
      HyScanDataBoxParam *param;

      param = g_hash_table_lookup (priv->params, names[i]);
      if (param == NULL)
        return FALSE;

      /* Параметр только для записи. */
      if (param->access == HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY)
        return FALSE;
    }

  g_rw_lock_reader_lock (&priv->lock);

  /* Считываем параметы. */
  for (i = 0; i < n_names; i++)
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

static void
hyscan_data_box_interface_init (HyScanParamInterface *iface)
{
  iface->schema = hyscan_data_box_schema;
  iface->set = hyscan_data_box_set;
  iface->get = hyscan_data_box_get;
}
