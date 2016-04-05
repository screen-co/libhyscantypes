/**
 * \file hyscan-data-box.h
 *
 * \brief Заголовочный файл класса работы с параметрами.
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2016
 * \license Проприетарная лицензия ООО "Экран"
 *
 */

#include "hyscan-data-box.h"
#include <string.h>

enum
{
  PROP_O,
  PROP_SCHEMA
};

enum
{
  SIGNAL_CHANGED,
  SIGNAL_LAST
};

/* Значение параметра. */
typedef struct
{
  gboolean                     used;                   /* Признак установки значения параметра. */
  HyScanDataSchemaType         type;                   /* Тип параметра. */
  gint64                       value;                  /* Значение параметра. */
  guint32                      mod_count;              /* Счётчик изменений значений параметра. */
  GQuark                       name_id;                /* Идентификатор названия параметра. */
  gboolean                     readonly;               /* Параметр доступен только для чтения. */
} HyScanDataBoxParam;

struct _HyScanDataBoxPrivate
{
  HyScanDataSchema            *schema;                 /* Описание схемы. */

  gchar                      **keys_list;              /* Список параметров. */
  GHashTable                  *params;                 /* Значения параметров. */

  guint32                      mod_count;              /* Глобальный счётчик изменений значений параметра. */

  GRWLock                      lock;                   /* Блокировка. */
};

static void    hyscan_data_box_set_property            (GObject               *object,
                                                        guint                  prop_id,
                                                        const GValue          *value,
                                                        GParamSpec            *pspec);
static void    hyscan_data_box_object_constructed      (GObject               *object);
static void    hyscan_data_box_object_finalize         (GObject               *object);

static gint32  hyscan_data_box_get_type_size           (HyScanDataSchemaType   type);

static void    hyscan_data_box_param_free              (gpointer               param);

static guint   hyscan_data_box_signals[SIGNAL_LAST] = { 0 };

G_DEFINE_TYPE_WITH_PRIVATE (HyScanDataBox, hyscan_data_box, G_TYPE_OBJECT);

static void hyscan_data_box_class_init( HyScanDataBoxClass *klass )
{
  GObjectClass *object_class = G_OBJECT_CLASS( klass );

  object_class->set_property = hyscan_data_box_set_property;

  object_class->constructed = hyscan_data_box_object_constructed;
  object_class->finalize = hyscan_data_box_object_finalize;

  g_object_class_install_property (object_class, PROP_SCHEMA,
    g_param_spec_object ("schema", "Schema", "Schema object", HYSCAN_TYPE_DATA_SCHEMA,
                      G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

  hyscan_data_box_signals[SIGNAL_CHANGED] =
    g_signal_new( "changed", HYSCAN_TYPE_DATA_BOX, G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED, 0, NULL, NULL,
                  g_cclosure_marshal_VOID__STRING, G_TYPE_NONE, 1, G_TYPE_STRING );
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
    case PROP_SCHEMA:
      priv->schema = g_value_dup_object (value);
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

  g_rw_lock_init (&priv->lock);

  /* Таблица со значениями параметров. */
  priv->params = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, hyscan_data_box_param_free);

  /* Схема данных. */
  if (priv->schema == NULL)
    return;

  /* Список параметров. */
  priv->keys_list = hyscan_data_schema_list_keys (priv->schema);
  if (priv->keys_list == NULL)
    return;

  for (i = 0; priv->keys_list[i] != NULL; i++)
    {
      HyScanDataBoxParam *param = g_slice_new0 (HyScanDataBoxParam);

      param->type = hyscan_data_schema_key_get_type (priv->schema, priv->keys_list[i]);
      param->name_id = g_quark_from_string (priv->keys_list[i]);
      param->readonly = hyscan_data_schema_key_is_readonly (priv->schema, priv->keys_list[i]);
      g_hash_table_insert (priv->params, (gpointer)priv->keys_list[i], param);
    }
}

static void
hyscan_data_box_object_finalize (GObject *object)
{
  HyScanDataBox *data_box = HYSCAN_DATA_BOX (object);
  HyScanDataBoxPrivate *priv = data_box->priv;

  g_object_unref (priv->schema);
  g_hash_table_unref (priv->params);
  g_rw_lock_clear (&priv->lock);

  g_strfreev (priv->keys_list);

  G_OBJECT_CLASS (hyscan_data_box_parent_class)->finalize (object);
}

/* Функция возвращает размер одного элемента данных. */
static gint32
hyscan_data_box_get_type_size (HyScanDataSchemaType type)
{
  switch (type)
    {
    case HYSCAN_DATA_SCHEMA_TYPE_BOOLEAN:
      return sizeof (gboolean);

    case HYSCAN_DATA_SCHEMA_TYPE_INTEGER:
      return sizeof (gint64);

    case HYSCAN_DATA_SCHEMA_TYPE_DOUBLE:
      return sizeof (gdouble);

    case HYSCAN_DATA_SCHEMA_TYPE_STRING:
      return sizeof (gchar);

    case HYSCAN_DATA_SCHEMA_TYPE_ENUM:
      return sizeof (gint64);

    default:
      break;
    }

  return 0;
}

/* Функция освобождает память занятую структурой с параметром. */
static void
hyscan_data_box_param_free (gpointer data)
{
  HyScanDataBoxParam *param = data;

  if (param->type == HYSCAN_DATA_SCHEMA_TYPE_STRING)
    g_free (*(gpointer*)(&param->value));

  g_slice_free (HyScanDataBoxParam, param);
}

/* Функция создаёт новый объект HyScanDataBox. */
HyScanDataBox *
hyscan_data_box_new_from_schema (HyScanDataSchema *schema)
{
  return g_object_new (HYSCAN_TYPE_DATA_BOX, "schema", schema, NULL);
}

/* Функция создаёт новый объект HyScanDataBox. */
HyScanDataBox *
hyscan_data_box_new_from_file (const gchar *schema_path,
                               const gchar *schema_id,
                               const gchar *overrides_data)
{
  HyScanDataBox *data_box;
  HyScanDataSchema *schema;

  schema = hyscan_data_schema_new_from_file (schema_path, schema_id, overrides_data);
  data_box = g_object_new (HYSCAN_TYPE_DATA_BOX, "schema", schema, NULL);
  g_clear_object (&schema);

  return data_box;
}

/* Функция создаёт новый объект HyScanDataBox. */
HyScanDataBox *
hyscan_data_box_new_from_resource (const gchar *schema_resource,
                                   const gchar *schema_id,
                                   const gchar *overrides_data)
{
  HyScanDataBox *data_box;
  HyScanDataSchema *schema;

  schema = hyscan_data_schema_new_from_resource (schema_resource, schema_id, overrides_data);
  data_box = g_object_new (HYSCAN_TYPE_DATA_BOX, "schema", schema, NULL);
  g_clear_object (&schema);

  return data_box;
}

/* Функция возвращает указатель на объект HyScanDataSchema. */
HyScanDataSchema *
hyscan_data_box_get_schema (HyScanDataBox *data_box)
{
  g_return_val_if_fail (HYSCAN_IS_DATA_BOX (data_box), NULL);

  return data_box->priv->schema;
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

/* Функция устанавливает значение параметра. */
gboolean
hyscan_data_box_set (HyScanDataBox        *data_box,
                     const gchar          *name,
                     HyScanDataSchemaType  type,
                     gconstpointer         value,
                     gint32                size)
{
  HyScanDataBoxPrivate *priv;
  HyScanDataBoxParam *param;
  gboolean status = FALSE;
  GQuark name_id = 0;

  g_return_val_if_fail (HYSCAN_IS_DATA_BOX (data_box), FALSE);

  priv = data_box->priv;

  param = g_hash_table_lookup (priv->params, name);
  if (param == NULL || param->type != type || param->readonly)
    return FALSE;

  g_rw_lock_writer_lock (&priv->lock);

  name_id = param->name_id;

  /* Сброс значения по умолчанию. */
  if (size == 0)
    {
      if (param->type == HYSCAN_DATA_SCHEMA_TYPE_STRING)
        g_free (*(gpointer*)(&param->value));
      param->value = 0;
      param->used = FALSE;
      param->mod_count += 1;
      priv->mod_count += 1;
      status = TRUE;
      goto exit;
    }

  /* Проверка размера значения параметра. */
  if (type != HYSCAN_DATA_SCHEMA_TYPE_STRING)
    {
      if (size != hyscan_data_box_get_type_size(type))
        goto exit;
    }
  else
    {
      if (size != strlen ((gchar*)value) + 1)
        goto exit;
    }

  /* Изменяем значение параметра. */
  switch (type)
    {
    case HYSCAN_DATA_SCHEMA_TYPE_BOOLEAN:
      *(gint64*)(&param->value) = *(gboolean*)value ? TRUE : FALSE;
      break;

    case HYSCAN_DATA_SCHEMA_TYPE_INTEGER:
      if (!hyscan_data_schema_key_check_integer (priv->schema, name, *(gint64*)value))
        goto exit;
      *(gint64*)(&param->value) = *(gint64*)value;
      break;

    case HYSCAN_DATA_SCHEMA_TYPE_DOUBLE:
      if (!hyscan_data_schema_key_check_double (priv->schema, name, *(gdouble*)value))
        goto exit;
      *(gdouble*)(&param->value) = *(gdouble*)value;
      break;

    case HYSCAN_DATA_SCHEMA_TYPE_STRING:
      g_free (*(gpointer*)(&param->value));
      param->value = (gint64)g_strdup (value);
      break;

    case HYSCAN_DATA_SCHEMA_TYPE_ENUM:
      if (!hyscan_data_schema_key_check_enum (priv->schema, name, *(gint64*)value))
        goto exit;
      *(gint64*)(&param->value) = *(gint64*)value;
      break;

    default:
      goto exit;
    }

  param->used = TRUE;
  param->mod_count += 1;
  priv->mod_count += 1;
  status = TRUE;

exit:
  g_rw_lock_writer_unlock (&priv->lock);

  if (status)
    g_signal_emit (data_box, hyscan_data_box_signals[SIGNAL_CHANGED], name_id, name);

  return status;
}

/* Функция считывает значение параметра. */
gboolean
hyscan_data_box_get (HyScanDataBox         *data_box,
                     const gchar           *name,
                     HyScanDataSchemaType   type,
                     gpointer               buffer,
                     gint32                *buffer_size)
{
  HyScanDataBoxPrivate *priv;
  HyScanDataBoxParam *param;
  gboolean status = FALSE;

  g_return_val_if_fail (HYSCAN_IS_DATA_BOX (data_box), FALSE);

  priv = data_box->priv;

  param = g_hash_table_lookup (priv->params, name);
  if (param == NULL || param->type != type)
    return FALSE;

  g_rw_lock_reader_lock (&priv->lock);

  /* Определение размера значения параметра. */
  if (buffer == NULL)
    {
      if (type != HYSCAN_DATA_SCHEMA_TYPE_STRING)
        *buffer_size = hyscan_data_box_get_type_size (param->type);
      else if (param->used)
        *buffer_size = strlen (*(gpointer*)(&param->value)) + 1;
      else
        *buffer_size = strlen (hyscan_data_schema_key_get_default_string (priv->schema, name)) + 1;

      status = TRUE;
      goto exit;
    }

  /* Проверка размера значения параметра. */
  if (type != HYSCAN_DATA_SCHEMA_TYPE_STRING)
    {
      if (*buffer_size != hyscan_data_box_get_type_size(type))
        goto exit;
    }

  status = TRUE;

  switch (type)
    {
    case HYSCAN_DATA_SCHEMA_TYPE_BOOLEAN:
      if (param->used)
        *(gboolean*)buffer = *(gint64*)&param->value;
      else
        status = hyscan_data_schema_key_get_default_boolean (priv->schema, name, buffer);
      break;

    case HYSCAN_DATA_SCHEMA_TYPE_INTEGER:
      if (param->used)
        *(gint64*)buffer = *(gint64*)&param->value;
      else
        status = hyscan_data_schema_key_get_default_integer (priv->schema, name, buffer);
      break;

    case HYSCAN_DATA_SCHEMA_TYPE_DOUBLE:
      if (param->used)
        *(gdouble*)buffer = *(gdouble*)&param->value;
      else
        status = hyscan_data_schema_key_get_default_double (priv->schema, name, buffer);
      break;

    case HYSCAN_DATA_SCHEMA_TYPE_STRING:
      {
        const gchar *str_value;

        if (param->used)
          str_value = *(gpointer*)&param->value;
        else
          str_value = hyscan_data_schema_key_get_default_string (priv->schema, name);

        if (str_value == NULL)
          *buffer_size = 0;
        else
          *buffer_size = g_snprintf (buffer, *buffer_size, "%s", str_value);
      }
      break;

    case HYSCAN_DATA_SCHEMA_TYPE_ENUM:
      if (param->used)
        *(gint64*)buffer = *(gint64*)&param->value;
      else
        status = hyscan_data_schema_key_get_default_enum (priv->schema, name, buffer);
      break;

    default:
      status = FALSE;
      goto exit;
    }

exit:
  g_rw_lock_reader_unlock (&priv->lock);

  return status;
}

/* Функция устанавливает значение параметра типа BOOLEAN. */
gboolean
hyscan_data_box_set_boolean (HyScanDataBox *data_box,
                             const gchar   *name,
                             gboolean       value)
{
  HyScanDataSchemaType type = HYSCAN_DATA_SCHEMA_TYPE_BOOLEAN;
  guint size = hyscan_data_box_get_type_size(type);
  return hyscan_data_box_set (data_box, name, type, &value, size);
}

/* Функция устанавливает значение параметра типа INTEGER. */
gboolean
hyscan_data_box_set_integer (HyScanDataBox *data_box,
                             const gchar   *name,
                             gint64         value)
{
  HyScanDataSchemaType type = HYSCAN_DATA_SCHEMA_TYPE_INTEGER;
  guint size = hyscan_data_box_get_type_size(type);
  return hyscan_data_box_set (data_box, name, type, &value, size);
}

/* Функция устанавливает значение параметра типа DOUBLE. */
gboolean
hyscan_data_box_set_double (HyScanDataBox *data_box,
                            const gchar   *name,
                            gdouble        value)
{
  HyScanDataSchemaType type = HYSCAN_DATA_SCHEMA_TYPE_DOUBLE;
  guint size = hyscan_data_box_get_type_size(type);
  return hyscan_data_box_set (data_box, name, type, &value, size);
}

/* Функция устанавливает значение параметра типа STRING. */
gboolean
hyscan_data_box_set_string (HyScanDataBox *data_box,
                            const gchar   *name,
                            const gchar   *value)
{
  HyScanDataSchemaType type = HYSCAN_DATA_SCHEMA_TYPE_STRING;
  gint32 size = strlen (value) + 1;
  return hyscan_data_box_set (data_box, name, type, value, size);
}

/* Функция устанавливает значение параметра типа ENUM. */
gboolean
hyscan_data_box_set_enum (HyScanDataBox *data_box,
                          const gchar   *name,
                          gint64         value)
{
  HyScanDataSchemaType type = HYSCAN_DATA_SCHEMA_TYPE_ENUM;
  guint size = hyscan_data_box_get_type_size(type);
  return hyscan_data_box_set (data_box, name, type, &value, size);
}

/* Функция считывает значение параметра типа BOOLEAN. */
gboolean
hyscan_data_box_get_boolean (HyScanDataBox *data_box,
                             const gchar   *name,
                             gboolean      *value)
{
  HyScanDataSchemaType type = HYSCAN_DATA_SCHEMA_TYPE_BOOLEAN;
  gint32 size = hyscan_data_box_get_type_size(type);
  return hyscan_data_box_get (data_box, name, type, value, &size);
}

/* Функция считывает значение параметра типа INTEGER. */
gboolean
hyscan_data_box_get_integer (HyScanDataBox *data_box,
                             const gchar   *name,
                             gint64        *value)
{
  HyScanDataSchemaType type = HYSCAN_DATA_SCHEMA_TYPE_INTEGER;
  gint32 size = hyscan_data_box_get_type_size(type);
  return hyscan_data_box_get (data_box, name, type, value, &size);
}

/* Функция считывает значение параметра типа DOUBLE. */
gboolean
hyscan_data_box_get_double (HyScanDataBox *data_box,
                            const gchar   *name,
                            gdouble       *value)
{
  HyScanDataSchemaType type = HYSCAN_DATA_SCHEMA_TYPE_DOUBLE;
  gint32 size = hyscan_data_box_get_type_size(type);
  return hyscan_data_box_get (data_box, name, type, value, &size);
}

/* Функция считывает значение параметра типа STRING. */
gchar *
hyscan_data_box_get_string (HyScanDataBox *data_box,
                            const gchar   *name)
{
  HyScanDataSchemaType type = HYSCAN_DATA_SCHEMA_TYPE_STRING;
  gchar *value;
  gint32 size;

  if (!hyscan_data_box_get (data_box, name, type, NULL, &size))
    return NULL;

  if (size <= 0)
    return NULL;

  size = (size / 128) + 1;
  size = (size * 128);
  value = g_malloc (size);

  if (!hyscan_data_box_get (data_box, name, type, value, &size))
    return NULL;

  return value;
}

/* Функция считывает значение параметра типа ENUM. */
gboolean
hyscan_data_box_get_enum (HyScanDataBox *data_box,
                          const gchar   *name,
                          gint64        *value)
{
  HyScanDataSchemaType type = HYSCAN_DATA_SCHEMA_TYPE_ENUM;
  gint32 size = hyscan_data_box_get_type_size(type);
  return hyscan_data_box_get (data_box, name, type, value, &size);
}
