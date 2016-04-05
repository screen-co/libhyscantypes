/**
 * \file hyscan-data-schema-overrides.c
 *
 * \brief Исходный файл класса переопределений схемы данных.
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2016
 * \license Проприетарная лицензия ООО "Экран"
 *
 */

#include "hyscan-data-schema-overrides.h"

enum
{
  PROP_O,
  PROP_DATA
};

struct _HyScanDataSchemaOverridesPrivate
{
  gchar                       *data;

  GKeyFile                    *overrides;
};

static void    hyscan_data_schema_overrides_set_property       (GObject               *object,
                                                                guint                  prop_id,
                                                                const GValue          *value,
                                                                GParamSpec            *pspec);
static void    hyscan_data_schema_overrides_object_constructed (GObject               *object);
static void    hyscan_data_schema_overrides_object_finalize    (GObject               *object);

G_DEFINE_TYPE_WITH_PRIVATE (HyScanDataSchemaOverrides, hyscan_data_schema_overrides, G_TYPE_OBJECT)

static void
hyscan_data_schema_overrides_class_init (HyScanDataSchemaOverridesClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = hyscan_data_schema_overrides_set_property;

  object_class->constructed = hyscan_data_schema_overrides_object_constructed;
  object_class->finalize = hyscan_data_schema_overrides_object_finalize;

  g_object_class_install_property (object_class, PROP_DATA,
    g_param_spec_string ("data", "Data", "Overrides data", NULL,
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

static void
hyscan_data_schema_overrides_init (HyScanDataSchemaOverrides *data_schema_overrides)
{
  data_schema_overrides->priv = hyscan_data_schema_overrides_get_instance_private (data_schema_overrides);
}

static void
hyscan_data_schema_overrides_set_property (GObject      *object,
                                          guint         prop_id,
                                          const GValue *value,
                                          GParamSpec   *pspec)
{
  HyScanDataSchemaOverrides *data_schema_overrides = HYSCAN_DATA_SCHEMA_OVERRIDES (object);
  HyScanDataSchemaOverridesPrivate *priv = data_schema_overrides->priv;

  switch (prop_id)
    {
    case PROP_DATA:
      priv->data = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hyscan_data_schema_overrides_object_constructed (GObject *object)
{
  HyScanDataSchemaOverrides *data_schema_overrides = HYSCAN_DATA_SCHEMA_OVERRIDES (object);
  HyScanDataSchemaOverridesPrivate *priv = data_schema_overrides->priv;

  priv->overrides = g_key_file_new ();

  if (priv->data != NULL)
    {
      GError *error = NULL;

      g_key_file_load_from_data (priv->overrides, priv->data, -1, G_KEY_FILE_NONE, &error);
      if (error != NULL)
        {
          g_warning ("HyScanDataSchemaOverrides: %s", error->message);
          g_error_free (error);
        }
    }
}

static void
hyscan_data_schema_overrides_object_finalize (GObject *object)
{
  HyScanDataSchemaOverrides *data_schema_overrides = HYSCAN_DATA_SCHEMA_OVERRIDES (object);
  HyScanDataSchemaOverridesPrivate *priv = data_schema_overrides->priv;

  g_key_file_unref (priv->overrides);
  g_clear_pointer (&priv->data, g_free);

  G_OBJECT_CLASS (hyscan_data_schema_overrides_parent_class)->finalize (object);
}

/* Функция создаёт объект HyScanDataSchemaOverrides. */
HyScanDataSchemaOverrides *
hyscan_data_schema_overrides_new (void)
{
  return g_object_new (HYSCAN_TYPE_DATA_SCHEMA_OVERRIDES, NULL);
}

/* Функция создаёт объект HyScanDataSchemaOverrides. */
HyScanDataSchemaOverrides *
hyscan_data_schema_overrides_new_from_data (const gchar *data)
{
  return g_object_new (HYSCAN_TYPE_DATA_SCHEMA_OVERRIDES,
                       "data", data,
                       NULL);
}

/* Функция создаёт объект HyScanDataSchemaOverrides. */
HyScanDataSchemaOverrides *
hyscan_data_schema_overrides_new_from_file (const gchar *path)
{
  gpointer object;
  gchar *data;

  if (!g_file_get_contents (path, &data, NULL, NULL))
    return NULL;

  object =  g_object_new (HYSCAN_TYPE_DATA_SCHEMA_OVERRIDES,
                         "data", data,
                         NULL);
  g_free (data);

  return object;
}

/* Функция возвращает переопределения схемы данных в фомате INI. */
gchar *
hyscan_data_schema_overrides_get_data (HyScanDataSchemaOverrides *overrides)
{
  HyScanDataSchemaOverridesPrivate *priv;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_OVERRIDES (overrides), NULL);
  priv = overrides->priv;

  if (priv->data == NULL)
    priv->data = g_key_file_to_data (priv->overrides, NULL, NULL);

  return g_strdup (priv->data);
}

/* Функция задаёт новые значения для параметра типа BOOLEAN. */
gboolean
hyscan_data_schema_overrides_set_boolean (HyScanDataSchemaOverrides *overrides,
                                          const gchar               *key_id,
                                          gboolean                   default_value,
                                          gboolean                   readonly)
{
  HyScanDataSchemaOverridesPrivate *priv;
  gchar *key_type;
  gint type_match;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_OVERRIDES (overrides), FALSE);
  priv = overrides->priv;

  key_type = g_key_file_get_string (priv->overrides, key_id, "type", NULL);
  type_match = g_strcmp0 (key_type, "boolean");
  g_free (key_type);

  if (type_match != 0 && key_type != NULL)
    return FALSE;

  g_key_file_set_string (priv->overrides, key_id, "type", "boolean");
  g_key_file_set_boolean (priv->overrides, key_id, "default", default_value);
  g_key_file_set_boolean (priv->overrides, key_id, "readonly", readonly);

  g_clear_pointer (&priv->data, g_free);

  return TRUE;
}

/* Функция возвращает значения для параметра типа BOOLEAN. */
gboolean
hyscan_data_schema_overrides_get_boolean (HyScanDataSchemaOverrides *overrides,
                                          const gchar               *key_id,
                                          gboolean                  *default_value,
                                          gboolean                  *readonly)
{
  HyScanDataSchemaOverridesPrivate *priv;
  gchar *key_type;
  gint type_match;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_OVERRIDES (overrides), FALSE);
  priv = overrides->priv;

  key_type = g_key_file_get_string (priv->overrides, key_id, "type", NULL);
  type_match = g_strcmp0 (key_type, "boolean");
  g_free (key_type);

  if (type_match != 0)
    return FALSE;

  if (default_value != NULL)
    *default_value = g_key_file_get_boolean (priv->overrides, key_id, "default", NULL);

  if (readonly != NULL)
    *readonly = g_key_file_get_boolean (priv->overrides, key_id, "readonly", NULL);

  return TRUE;
}

/* Функция задаёт новые значения для параметра типа INTEGER. */
gboolean
hyscan_data_schema_overrides_set_integer (HyScanDataSchemaOverrides *overrides,
                                          const gchar               *key_id,
                                          gint64                     default_value,
                                          gint64                     minimum_value,
                                          gint64                     maximum_value,
                                          gint64                     value_step,
                                          gboolean                   readonly)
{
  HyScanDataSchemaOverridesPrivate *priv;
  gchar *key_type;
  gint type_match;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_OVERRIDES (overrides), FALSE);
  priv = overrides->priv;

  key_type = g_key_file_get_string (priv->overrides, key_id, "type", NULL);
  type_match = g_strcmp0 (key_type, "integer");
  g_free (key_type);

  if (type_match != 0 && key_type != NULL)
    return FALSE;

  if ((minimum_value > maximum_value) ||
      (default_value < minimum_value) ||
      (default_value > maximum_value))
    return FALSE;

  g_key_file_set_string (priv->overrides, key_id, "type", "integer");
  g_key_file_set_int64 (priv->overrides, key_id, "default", default_value);
  g_key_file_set_int64 (priv->overrides, key_id, "min", minimum_value);
  g_key_file_set_int64 (priv->overrides, key_id, "max", maximum_value);
  g_key_file_set_int64 (priv->overrides, key_id, "step", value_step);
  g_key_file_set_boolean (priv->overrides, key_id, "readonly", readonly);

  g_clear_pointer (&priv->data, g_free);

  return TRUE;
}

/* Функция возвращает значения для параметра типа INTEGER. */
gboolean
hyscan_data_schema_overrides_get_integer (HyScanDataSchemaOverrides *overrides,
                                          const gchar               *key_id,
                                          gint64                    *default_value,
                                          gint64                    *minimum_value,
                                          gint64                    *maximum_value,
                                          gint64                    *value_step,
                                          gboolean                  *readonly)
{
  HyScanDataSchemaOverridesPrivate *priv;
  gchar *key_type;
  gint type_match;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_OVERRIDES (overrides), FALSE);
  priv = overrides->priv;

  key_type = g_key_file_get_string (priv->overrides, key_id, "type", NULL);
  type_match = g_strcmp0 (key_type, "integer");
  g_free (key_type);

  if (type_match != 0)
    return FALSE;

  if (default_value != NULL)
    *default_value = g_key_file_get_int64 (priv->overrides, key_id, "default", NULL);

  if (minimum_value != NULL)
    *minimum_value = g_key_file_get_int64 (priv->overrides, key_id, "min", NULL);

  if (maximum_value != NULL)
    *maximum_value = g_key_file_get_int64 (priv->overrides, key_id, "max", NULL);

  if (value_step != NULL)
    *value_step = g_key_file_get_int64 (priv->overrides, key_id, "step", NULL);

  if (readonly != NULL)
    *readonly = g_key_file_get_boolean (priv->overrides, key_id, "readonly", NULL);

  return TRUE;
}

/* Функция задаёт новые значения для параметра типа DOUBLE. */
gboolean
hyscan_data_schema_overrides_set_double (HyScanDataSchemaOverrides *overrides,
                                         const gchar               *key_id,
                                         gdouble                    default_value,
                                         gdouble                    minimum_value,
                                         gdouble                    maximum_value,
                                         gdouble                    value_step,
                                         gboolean                   readonly)
{
  HyScanDataSchemaOverridesPrivate *priv;
  gchar *key_type;
  gint type_match;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_OVERRIDES (overrides), FALSE);
  priv = overrides->priv;

  key_type = g_key_file_get_string (priv->overrides, key_id, "type", NULL);
  type_match = g_strcmp0 (key_type, "double");
  g_free (key_type);

  if (type_match != 0 && key_type != NULL)
    return FALSE;

  if ((minimum_value > maximum_value) ||
      (default_value < minimum_value) ||
      (default_value > maximum_value))
    return FALSE;

  g_key_file_set_string (priv->overrides, key_id, "type", "double");
  g_key_file_set_double (priv->overrides, key_id, "default", default_value);
  g_key_file_set_double (priv->overrides, key_id, "min", minimum_value);
  g_key_file_set_double (priv->overrides, key_id, "max", maximum_value);
  g_key_file_set_double (priv->overrides, key_id, "step", value_step);
  g_key_file_set_boolean (priv->overrides, key_id, "readonly", readonly);

  g_clear_pointer (&priv->data, g_free);

  return TRUE;
}

/* Функция возвращает значения для параметра типа DOUBLE. */
gboolean
hyscan_data_schema_overrides_get_double (HyScanDataSchemaOverrides *overrides,
                                         const gchar               *key_id,
                                         gdouble                   *default_value,
                                         gdouble                   *minimum_value,
                                         gdouble                   *maximum_value,
                                         gdouble                   *value_step,
                                         gboolean                  *readonly)
{
  HyScanDataSchemaOverridesPrivate *priv;
  gchar *key_type;
  gint type_match;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_OVERRIDES (overrides), FALSE);
  priv = overrides->priv;

  key_type = g_key_file_get_string (priv->overrides, key_id, "type", NULL);
  type_match = g_strcmp0 (key_type, "double");
  g_free (key_type);

  if (type_match != 0)
    return FALSE;

  if (default_value != NULL)
    *default_value = g_key_file_get_double (priv->overrides, key_id, "default", NULL);

  if (minimum_value != NULL)
    *minimum_value = g_key_file_get_double (priv->overrides, key_id, "min", NULL);

  if (maximum_value != NULL)
    *maximum_value = g_key_file_get_double (priv->overrides, key_id, "max", NULL);

  if (value_step != NULL)
    *value_step = g_key_file_get_double (priv->overrides, key_id, "step", NULL);

  if (readonly != NULL)
    *readonly = g_key_file_get_boolean (priv->overrides, key_id, "readonly", NULL);

  return TRUE;
}

/* Функция задаёт новые значения для параметра типа STRING. */
gboolean
hyscan_data_schema_overrides_set_string (HyScanDataSchemaOverrides *overrides,
                                         const gchar               *key_id,
                                         const gchar               *default_value,
                                         gboolean                   readonly)
{
  HyScanDataSchemaOverridesPrivate *priv;
  gchar *key_type;
  gint type_match;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_OVERRIDES (overrides), FALSE);
  priv = overrides->priv;

  key_type = g_key_file_get_string (priv->overrides, key_id, "type", NULL);
  type_match = g_strcmp0 (key_type, "string");
  g_free (key_type);

  if (type_match != 0 && key_type != NULL)
    return FALSE;

  g_key_file_set_string (priv->overrides, key_id, "type", "string");
  g_key_file_set_string (priv->overrides, key_id, "default", default_value);
  g_key_file_set_boolean (priv->overrides, key_id, "readonly", readonly);

  g_clear_pointer (&priv->data, g_free);

  return TRUE;
}

/* Функция возвращает значения для параметра типа STRING. */
gboolean
hyscan_data_schema_overrides_get_string (HyScanDataSchemaOverrides *overrides,
                                         const gchar               *key_id,
                                         gchar                    **default_value,
                                         gboolean                  *readonly)
{
  HyScanDataSchemaOverridesPrivate *priv;
  gchar *key_type;
  gint type_match;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_OVERRIDES (overrides), FALSE);
  priv = overrides->priv;

  key_type = g_key_file_get_string (priv->overrides, key_id, "type", NULL);
  type_match = g_strcmp0 (key_type, "string");
  g_free (key_type);

  if (type_match != 0)
    return FALSE;

  if (default_value != NULL)
    *default_value = g_key_file_get_string (priv->overrides, key_id, "default", NULL);

  if (readonly != NULL)
    *readonly = g_key_file_get_boolean (priv->overrides, key_id, "readonly", NULL);

  return TRUE;
}

/* Функция задаёт новые значения для параметра типа ENUM. */
gboolean
hyscan_data_schema_overrides_set_enum (HyScanDataSchemaOverrides *overrides,
                                       const gchar               *key_id,
                                       gint64                     default_value,
                                       gboolean                   readonly)
{
  HyScanDataSchemaOverridesPrivate *priv;
  gchar *key_type;
  gint type_match;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_OVERRIDES (overrides), FALSE);
  priv = overrides->priv;

  key_type = g_key_file_get_string (priv->overrides, key_id, "type", NULL);
  type_match = g_strcmp0 (key_type, "enum");
  g_free (key_type);

  if (type_match != 0 && key_type != NULL)
    return FALSE;

  g_key_file_set_string (priv->overrides, key_id, "type", "enum");
  g_key_file_set_int64 (priv->overrides, key_id, "default", default_value);
  g_key_file_set_boolean (priv->overrides, key_id, "readonly", readonly);

  g_clear_pointer (&priv->data, g_free);

  return TRUE;
}

/* Функция возвращает значения для параметра типа ENUM. */
gboolean
hyscan_data_schema_overrides_get_enum (HyScanDataSchemaOverrides *overrides,
                                       const gchar               *key_id,
                                       gint64                    *default_value,
                                       gboolean                  *readonly)
{
  HyScanDataSchemaOverridesPrivate *priv;
  gchar *key_type;
  gint type_match;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_OVERRIDES (overrides), FALSE);
  priv = overrides->priv;

  key_type = g_key_file_get_string (priv->overrides, key_id, "type", NULL);
  type_match = g_strcmp0 (key_type, "enum");
  g_free (key_type);

  if (type_match != 0)
    return FALSE;

  if (default_value != NULL)
    *default_value = g_key_file_get_int64 (priv->overrides, key_id, "default", NULL);

  if (readonly != NULL)
    *readonly = g_key_file_get_boolean (priv->overrides, key_id, "readonly", NULL);

  return TRUE;
}

/* Функция задаёт новые варианты допустимых значений для параметра с типом ENUM. */
gboolean
hyscan_data_schema_overrides_set_enum_values (HyScanDataSchemaOverrides *overrides,
                                              const gchar               *enum_id,
                                              const HyScanDataSchemaEnumValue* const *values)
{
  HyScanDataSchemaOverridesPrivate *priv;
  gchar *key_type;
  gint type_match;
  const gchar *enum_value[4];
  gint i;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_OVERRIDES (overrides), FALSE);
  priv = overrides->priv;

  key_type = g_key_file_get_string (priv->overrides, enum_id, "type", NULL);
  type_match = g_strcmp0 (key_type, "enum-values");
  g_free (key_type);

  if (type_match != 0 && key_type != NULL)
    return FALSE;

  g_key_file_remove_group (priv->overrides, enum_id, NULL);

  g_key_file_set_string (priv->overrides, enum_id, "type", "enum-values");

  for (i = 0; values[i] != NULL; i++)
    {
      gchar *value_id;
      gchar *value;

      value_id = g_strdup_printf ("value%d", i);
      value = g_strdup_printf ("%" G_GINT64_FORMAT, values[i]->value);

      enum_value[0] = value;
      enum_value[1] = values[i]->name;
      enum_value[2] = values[i]->description;
      enum_value[3] = NULL;

      g_key_file_set_string_list (priv->overrides, enum_id, value_id, enum_value, 3);

      g_free (value_id);
      g_free (value);
    }

  return TRUE;
}

/* Функция возвращает список вариантов допустимых значений для параметра с типом ENUM. */
HyScanDataSchemaEnumValue **
hyscan_data_schema_overrides_get_enum_values (HyScanDataSchemaOverrides *overrides,
                                              const gchar               *enum_id)
{
  HyScanDataSchemaOverridesPrivate *priv;
  gchar *key_type;
  gint type_match;
  gchar **values;
  HyScanDataSchemaEnumValue **enum_values;
  gint n_values;
  gint i;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_OVERRIDES (overrides), NULL);
  priv = overrides->priv;

  key_type = g_key_file_get_string (priv->overrides, enum_id, "type", NULL);
  type_match = g_strcmp0 (key_type, "enum-values");
  g_free (key_type);

  if (type_match != 0)
    return NULL;

  values = g_key_file_get_keys (priv->overrides, enum_id, NULL, NULL);
  if (values == NULL)
    return NULL;

  for (i = 0, n_values = 0; values[i] != NULL; i++)
    {
      if (g_str_has_prefix (values[i], "value"))
        n_values += 1;
    }

  enum_values = g_malloc0 (sizeof (HyScanDataSchemaEnumValue*) * (n_values + 1));

  for (i = 0, n_values = 0; values[i] != NULL; i++)
    {
      gchar **value;

      if (!g_str_has_prefix (values[i], "value"))
        continue;

      value = g_key_file_get_string_list (priv->overrides, enum_id, values[i], NULL, NULL);
      if (value == NULL || g_strv_length (value) != 3)
        {
          g_strfreev (value);
          break;
        }

      enum_values[n_values] = g_new(HyScanDataSchemaEnumValue, 1);
      enum_values[n_values]->value = g_ascii_strtoll (value[0], NULL, 10);
      enum_values[n_values]->name = g_strdup (value[1]);
      enum_values[n_values]->description = g_strdup (value[2]);

      n_values += 1;

      g_strfreev (value);
    }

  g_strfreev (values);

  return enum_values;
}
