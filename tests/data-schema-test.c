#include <gio/gio.h>
#include <hyscan-data-schema.h>

HyScanDataSchema *test_schema_create (const gchar *schema_id);

/* Функция проверки параметра типа BOOLEAN. */
void check_boolean (HyScanDataSchema *schema, const gchar *key_id)
{
  const gchar *name;
  const gchar *description;
  gchar *check_name;
  gchar *check_description;
  gboolean default_value;

  if (hyscan_data_schema_key_get_type (schema, key_id) != HYSCAN_DATA_SCHEMA_TYPE_BOOLEAN)
    g_error ("%s: type error", key_id);

  name = hyscan_data_schema_key_get_name (schema, key_id);
  description = hyscan_data_schema_key_get_description (schema, key_id);
  if (name == NULL || description == NULL)
    g_error ("%s: name or description error", key_id);

  if (!hyscan_data_schema_key_get_default_boolean (schema, key_id, &default_value))
    g_error ("%s: default value error", key_id);

  check_name = g_strdup_printf ("%s", default_value ? "True" : "False");
  check_description = g_strdup_printf ("%s value", default_value ? "True" : "False");

  if (g_strcmp0 (name, check_name) != 0)
    g_error ("%s: name mismatch ('%s' != '%s')", key_id, name, check_name);
  if (g_strcmp0 (description, check_description) != 0)
    g_error ("%s: description mismatch ('%s' != '%s')", key_id, description, check_description);

  g_free (check_name);
  g_free (check_description);
}

/* Функция проверки параметра типа INTEGER. */
void check_integer (HyScanDataSchema *schema, const gchar *key_id)
{
  const gchar *name;
  const gchar *description;

  gchar *check_name;
  gchar *check_description;

  gint64 default_value;
  gint64 minimum_value;
  gint64 maximum_value;
  gint64 value_step;

  if (hyscan_data_schema_key_get_type (schema, key_id) != HYSCAN_DATA_SCHEMA_TYPE_INTEGER)
    g_error ("%s: type error", key_id);

  name = hyscan_data_schema_key_get_name (schema, key_id);
  description = hyscan_data_schema_key_get_description (schema, key_id);
  if (name == NULL || description == NULL)
    g_error ("%s: name or description error", key_id);

  if (!hyscan_data_schema_key_get_default_integer (schema, key_id, &default_value))
    g_error ("%s: default value error", key_id);
  if (!hyscan_data_schema_key_get_integer_range (schema, key_id, &minimum_value, &maximum_value))
    g_error ("%s: range values error", key_id);
  if (!hyscan_data_schema_key_get_integer_step (schema, key_id, &value_step))
    g_error ("%s: value step error", key_id);

  check_name = g_strdup_printf ("Integer %" G_GINT64_FORMAT, default_value);
  check_description = g_strdup_printf ("Integer %" G_GINT64_FORMAT " value", default_value);

  if (g_strcmp0 (name, check_name) != 0)
    g_error ("%s: name mismatch ('%s' != '%s')", key_id, name, check_name);
  if (g_strcmp0 (description, check_description) != 0)
    g_error ("%s: description mismatch ('%s' != '%s')", key_id, description, check_description);

  g_free (check_name);
  g_free (check_description);

  if (default_value != minimum_value)
    g_error ("%s: minimum value error", key_id);

  if (default_value != maximum_value / 1000)
    g_error ("%s: maximum value error", key_id);

  if (default_value != value_step)
    g_error ("%s: value step error", key_id);
}

/* Функция проверки параметра типа DOUBLE. */
void check_double (HyScanDataSchema *schema, const gchar *key_id)
{
  const gchar *name;
  const gchar *description;

  gchar *check_name;
  gchar *check_description;

  gdouble default_value;
  gdouble minimum_value;
  gdouble maximum_value;
  gdouble value_step;

  if (hyscan_data_schema_key_get_type (schema, key_id) != HYSCAN_DATA_SCHEMA_TYPE_DOUBLE)
    g_error ("%s: type error", key_id);

  name = hyscan_data_schema_key_get_name (schema, key_id);
  description = hyscan_data_schema_key_get_description (schema, key_id);
  if (name == NULL || description == NULL)
    g_error ("%s: name or description error", key_id);

  if (!hyscan_data_schema_key_get_default_double (schema, key_id, &default_value))
    g_error ("%s: default value error", key_id);
  if (!hyscan_data_schema_key_get_double_range (schema, key_id, &minimum_value, &maximum_value))
    g_error ("%s: range values error", key_id);
  if (!hyscan_data_schema_key_get_double_step (schema, key_id, &value_step))
    g_error ("%s: value step error", key_id);

  check_name = g_strdup_printf ("Double %" G_GINT64_FORMAT, (gint64)default_value);
  check_description = g_strdup_printf ("Double %" G_GINT64_FORMAT " value", (gint64)default_value);

  if (g_strcmp0 (name, check_name) != 0)
    g_error ("%s: name mismatch ('%s' != '%s')", key_id, name, check_name);
  if (g_strcmp0 (description, check_description) != 0)
    g_error ("%s: description mismatch ('%s' != '%s')", key_id, description, check_description);

  g_free (check_name);
  g_free (check_description);

  if (default_value != minimum_value)
    g_error ("%s: minimum value error", key_id);

  if (default_value != maximum_value / 1000)
    g_error ("%s: maximum value error", key_id);

  if (default_value != value_step)
    g_error ("%s: value step error", key_id);
}

/* Функция проверки параметра типа STRING. */
void check_string (HyScanDataSchema *schema, const gchar *key_id)
{
  const gchar *name;
  const gchar *description;
  gchar *check_name;
  gchar *check_description;
  const gchar *default_value;

  if (hyscan_data_schema_key_get_type (schema, key_id) != HYSCAN_DATA_SCHEMA_TYPE_STRING)
    g_error ("%s: type error", key_id);

  name = hyscan_data_schema_key_get_name (schema, key_id);
  description = hyscan_data_schema_key_get_description (schema, key_id);
  if (name == NULL || description == NULL)
    g_error ("%s: name or description error", key_id);

  default_value = hyscan_data_schema_key_get_default_string (schema, key_id);
  if (default_value == NULL)
    g_error ("%s: default value error", key_id);

  check_name = g_strdup_printf ("%s", default_value);
  check_description = g_strdup_printf ("%s value", default_value);

  if (g_strcmp0 (name, check_name) != 0)
    g_error ("%s: name mismatch ('%s' != '%s')", key_id, name, check_name);
  if (g_strcmp0 (description, check_description) != 0)
    g_error ("%s: description mismatch ('%s' != '%s')", key_id, description, check_description);

  g_free (check_name);
  g_free (check_description);
}

/* Функция проверки параметра типа ENUM. */
void check_enum (HyScanDataSchema *schema, const gchar *key_id)
{
  const gchar *name;
  const gchar *description;
  gchar *check_name;
  gchar *check_description;
  gint64 default_value;
  gint64 i;

  if (hyscan_data_schema_key_get_type (schema, key_id) != HYSCAN_DATA_SCHEMA_TYPE_ENUM)
    g_error ("%s: type error", key_id);

  name = hyscan_data_schema_key_get_name (schema, key_id);
  description = hyscan_data_schema_key_get_description (schema, key_id);
  if (name == NULL || description == NULL)
    g_error ("%s: name or description error", key_id);

  if (!hyscan_data_schema_key_get_default_enum (schema, key_id, &default_value))
    g_error ("%s: default value error", key_id);

  check_name = g_strdup_printf ("Enum %" G_GINT64_FORMAT, default_value);
  check_description = g_strdup_printf ("Enum %" G_GINT64_FORMAT " value", default_value);

  if (g_strcmp0 (name, check_name) != 0)
    g_error ("%s: name mismatch ('%s' != '%s')", key_id, name, check_name);
  if (g_strcmp0 (description, check_description) != 0)
    g_error ("%s: description mismatch ('%s' != '%s')", key_id, description, check_description);

  g_free (check_name);
  g_free (check_description);

  if (hyscan_data_schema_key_is_readonly (schema, key_id))
    return;

  for (i = default_value; i > 0; i--)
    {
      if (!hyscan_data_schema_key_check_enum (schema, key_id, i))
        g_error ("%s: range error", key_id);
    }
}

int
main (int argc, char **argv)
{
  HyScanDataSchema *schema;
  gchar **keys_list;
  guint i;

  schema = test_schema_create ("test");

  keys_list = hyscan_data_schema_list_keys (schema);
  if (keys_list == NULL)
    g_error ("empty schema");

  for (i = 0; keys_list[i] != NULL; i++)
    {
      HyScanDataSchemaType type = hyscan_data_schema_key_get_type (schema, keys_list[i]);

      switch (type)
        {
        case HYSCAN_DATA_SCHEMA_TYPE_BOOLEAN:
          check_boolean (schema, keys_list[i]);
          break;

        case HYSCAN_DATA_SCHEMA_TYPE_INTEGER:
          check_integer (schema, keys_list[i]);
          break;

        case HYSCAN_DATA_SCHEMA_TYPE_DOUBLE:
          check_double (schema, keys_list[i]);
          break;

        case HYSCAN_DATA_SCHEMA_TYPE_STRING:
          check_string (schema, keys_list[i]);
          break;

        case HYSCAN_DATA_SCHEMA_TYPE_ENUM:
          check_enum (schema, keys_list[i]);
          break;

        default:
          break;
        }
    }

  g_strfreev (keys_list);

  g_object_unref (schema);

  return 0;
}
