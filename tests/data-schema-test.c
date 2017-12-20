#include <gio/gio.h>
#include <hyscan-data-schema-builder.h>
#include <libxml/parser.h>

gchar *test_schema_create (const gchar *schema_id);

/* Функция проверки параметра типа BOOLEAN. */
void
check_boolean (HyScanDataSchema *schema,
               const gchar      *key_id)
{
  const gchar *name;
  const gchar *description;
  HyScanDataSchemaKeyAccess access;

  gchar *check_name;
  gchar *check_description;

  GVariant *default_value;

  if (!g_str_has_prefix (key_id, "/test/"))
    g_error ("%s: name error", key_id);

  if (hyscan_data_schema_key_get_value_type (schema, key_id) != HYSCAN_DATA_SCHEMA_KEY_BOOLEAN)
    g_error ("%s: type error", key_id);

  name = hyscan_data_schema_key_get_name (schema, key_id);
  description = hyscan_data_schema_key_get_description (schema, key_id);
  if (name == NULL || description == NULL)
    g_error ("%s: name or description error", key_id);

  if (hyscan_data_schema_key_get_view (schema, key_id) != HYSCAN_DATA_SCHEMA_VIEW_BIN)
    g_error ("%s: view error", key_id);

  access = hyscan_data_schema_key_get_access (schema, key_id);
  if (g_str_has_prefix (key_id, "/readonly") && (access != HYSCAN_DATA_SCHEMA_ACCESS_READONLY))
    g_error ("%s: access error", key_id);
  if (g_str_has_prefix (key_id, "/writeonly") && (access != HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY))
    g_error ("%s: access error", key_id);

  default_value = hyscan_data_schema_key_get_default (schema, key_id);
  if (default_value == NULL)
    g_error ("%s: default value error", key_id);

  check_name = g_strdup_printf ("%s", g_variant_get_boolean (default_value) ? "True" : "False");
  check_description = g_strdup_printf ("%s value", g_variant_get_boolean (default_value) ? "True" : "False");

  if (g_strcmp0 (name, check_name) != 0)
    g_error ("%s: name mismatch ('%s' != '%s')", key_id, name, check_name);
  if (g_strcmp0 (description, check_description) != 0)
    g_error ("%s: description mismatch ('%s' != '%s')", key_id, description, check_description);

  g_free (check_name);
  g_free (check_description);

  g_variant_unref (default_value);
}

/* Функция проверки параметра типа INTEGER. */
void
check_integer (HyScanDataSchema *schema,
               const gchar      *key_id)
{
  const gchar *name;
  const gchar *description;
  HyScanDataSchemaKeyAccess access;

  gchar *check_name;
  gchar *check_description;

  GVariant *default_value;
  GVariant *minimum_value;
  GVariant *maximum_value;
  GVariant *value_step;

  if (!g_str_has_prefix (key_id, "/test/"))
    g_error ("%s: name error", key_id);

  if (hyscan_data_schema_key_get_value_type (schema, key_id) != HYSCAN_DATA_SCHEMA_KEY_INTEGER)
    g_error ("%s: type error", key_id);

  name = hyscan_data_schema_key_get_name (schema, key_id);
  description = hyscan_data_schema_key_get_description (schema, key_id);
  if (name == NULL || description == NULL)
    g_error ("%s: name or description error", key_id);

  if (hyscan_data_schema_key_get_view (schema, key_id) != HYSCAN_DATA_SCHEMA_VIEW_HEX)
    g_error ("%s: view error", key_id);

  access = hyscan_data_schema_key_get_access (schema, key_id);
  if (g_str_has_prefix (key_id, "/readonly") && (access != HYSCAN_DATA_SCHEMA_ACCESS_READONLY))
    g_error ("%s: access error", key_id);
  if (g_str_has_prefix (key_id, "/writeonly") && (access != HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY))
    g_error ("%s: access error", key_id);

  default_value = hyscan_data_schema_key_get_default (schema, key_id);
  if (default_value == NULL)
    g_error ("%s: default value error", key_id);

  minimum_value = hyscan_data_schema_key_get_minimum (schema, key_id);
  if (minimum_value == NULL)
    g_error ("%s: minimum value error", key_id);

  maximum_value = hyscan_data_schema_key_get_maximum (schema, key_id);
  if (maximum_value == NULL)
    g_error ("%s: maximum value error", key_id);

  value_step = hyscan_data_schema_key_get_step (schema, key_id);
  if (value_step == NULL)
    g_error ("%s: value step error", key_id);

  check_name = g_strdup_printf ("Integer %" G_GINT64_FORMAT, g_variant_get_int64 (default_value));
  check_description = g_strdup_printf ("Integer %" G_GINT64_FORMAT " value", g_variant_get_int64 (default_value));

  if (g_strcmp0 (name, check_name) != 0)
    g_error ("%s: name mismatch ('%s' != '%s')", key_id, name, check_name);
  if (g_strcmp0 (description, check_description) != 0)
    g_error ("%s: description mismatch ('%s' != '%s')", key_id, description, check_description);

  g_free (check_name);
  g_free (check_description);

  if (g_variant_get_int64 (default_value) != g_variant_get_int64 (minimum_value))
    g_error ("%s: minimum value error", key_id);

  if (g_variant_get_int64 (default_value) != g_variant_get_int64 (maximum_value) / 1000)
    g_error ("%s: maximum value error", key_id);

  if (g_variant_get_int64 (default_value) != g_variant_get_int64 (value_step))
    g_error ("%s: value step error", key_id);

  g_variant_unref (default_value);
  g_variant_unref (minimum_value);
  g_variant_unref (maximum_value);
  g_variant_unref (value_step);
}

/* Функция проверки параметра типа DOUBLE. */
void
check_double (HyScanDataSchema *schema,
              const gchar      *key_id)
{
  const gchar *name;
  const gchar *description;
  HyScanDataSchemaKeyAccess access;

  gchar *check_name;
  gchar *check_description;

  GVariant *default_value;
  GVariant *minimum_value;
  GVariant *maximum_value;
  GVariant *value_step;

  if (!g_str_has_prefix (key_id, "/test/"))
    g_error ("%s: name error", key_id);

  if (hyscan_data_schema_key_get_value_type (schema, key_id) != HYSCAN_DATA_SCHEMA_KEY_DOUBLE)
    g_error ("%s: type error", key_id);

  name = hyscan_data_schema_key_get_name (schema, key_id);
  description = hyscan_data_schema_key_get_description (schema, key_id);
  if (name == NULL || description == NULL)
    g_error ("%s: name or description error", key_id);

  if (hyscan_data_schema_key_get_view (schema, key_id) != HYSCAN_DATA_SCHEMA_VIEW_DEC)
    g_error ("%s: view error", key_id);

  access = hyscan_data_schema_key_get_access (schema, key_id);
  if (g_str_has_prefix (key_id, "/readonly") && (access != HYSCAN_DATA_SCHEMA_ACCESS_READONLY))
    g_error ("%s: access error", key_id);
  if (g_str_has_prefix (key_id, "/writeonly") && (access != HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY))
    g_error ("%s: access error", key_id);

  default_value = hyscan_data_schema_key_get_default (schema, key_id);
  if (default_value == NULL)
    g_error ("%s: default value error", key_id);

  minimum_value = hyscan_data_schema_key_get_minimum (schema, key_id);
  if (minimum_value == NULL)
    g_error ("%s: minimum value error", key_id);

  maximum_value = hyscan_data_schema_key_get_maximum (schema, key_id);
  if (maximum_value == NULL)
    g_error ("%s: maximum value error", key_id);

  value_step = hyscan_data_schema_key_get_step (schema, key_id);
  if (value_step == NULL)
    g_error ("%s: value step error", key_id);

  check_name = g_strdup_printf ("Double %" G_GINT64_FORMAT, (gint64)g_variant_get_double (default_value));
  check_description = g_strdup_printf ("Double %" G_GINT64_FORMAT " value", (gint64)g_variant_get_double (default_value));

  if (g_strcmp0 (name, check_name) != 0)
    g_error ("%s: name mismatch ('%s' != '%s')", key_id, name, check_name);
  if (g_strcmp0 (description, check_description) != 0)
    g_error ("%s: description mismatch ('%s' != '%s')", key_id, description, check_description);

  g_free (check_name);
  g_free (check_description);

  if (g_variant_get_double (default_value) != g_variant_get_double (minimum_value))
    g_error ("%s: minimum value error", key_id);

  if (g_variant_get_double (default_value) != g_variant_get_double (maximum_value) / 1000)
    g_error ("%s: maximum value error", key_id);

  if (g_variant_get_double (default_value) != g_variant_get_double (value_step))
    g_error ("%s: value step error", key_id);

  g_variant_unref (default_value);
  g_variant_unref (minimum_value);
  g_variant_unref (maximum_value);
  g_variant_unref (value_step);
}

/* Функция проверки параметра типа STRING. */
void
check_string (HyScanDataSchema *schema,
              const gchar      *key_id)
{
  const gchar *name;
  const gchar *description;
  HyScanDataSchemaKeyAccess access;

  gchar *check_name;
  gchar *check_description;

  GVariant *default_value;

  if (!g_str_has_prefix (key_id, "/test/"))
    g_error ("%s: name error", key_id);

  if (hyscan_data_schema_key_get_value_type (schema, key_id) != HYSCAN_DATA_SCHEMA_KEY_STRING)
    g_error ("%s: type error", key_id);

  name = hyscan_data_schema_key_get_name (schema, key_id);
  description = hyscan_data_schema_key_get_description (schema, key_id);
  if (name == NULL || description == NULL)
    g_error ("%s: name or description error", key_id);

  if (hyscan_data_schema_key_get_view (schema, key_id) != HYSCAN_DATA_SCHEMA_VIEW_SCHEMA)
    g_error ("%s: view error", key_id);

  access = hyscan_data_schema_key_get_access (schema, key_id);
  if (g_str_has_prefix (key_id, "/readonly") && (access != HYSCAN_DATA_SCHEMA_ACCESS_READONLY))
    g_error ("%s: access error", key_id);
  if (g_str_has_prefix (key_id, "/writeonly") && (access != HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY))
    g_error ("%s: access error", key_id);

  default_value = hyscan_data_schema_key_get_default (schema, key_id);
  if (g_pattern_match_simple ("*null*", key_id))
    {
      if (default_value != NULL)
        g_error ("%s: default value error", key_id);
    }
  else
    {
      if (default_value == NULL)
        g_error ("%s: default value error", key_id);

      check_name = g_strdup_printf ("%s", g_variant_get_string (default_value, NULL));
      check_description = g_strdup_printf ("%s value", g_variant_get_string (default_value, NULL));

      if (g_strcmp0 (name, check_name) != 0)
        g_error ("%s: name mismatch ('%s' != '%s')", key_id, name, check_name);
      if (g_strcmp0 (description, check_description) != 0)
        g_error ("%s: description mismatch ('%s' != '%s')", key_id, description, check_description);

      g_free (check_name);
      g_free (check_description);

      g_variant_unref (default_value);
    }
}

/* Функция проверки параметра типа ENUM. */
void
check_enum (HyScanDataSchema *schema,
            const gchar      *key_id)
{
  const gchar *name;
  const gchar *description;
  HyScanDataSchemaKeyAccess access;

  gchar *check_name;
  gchar *check_description;

  GVariant *default_value;
  gint64 i;

  if (!g_str_has_prefix (key_id, "/test/"))
    g_error ("%s: name error", key_id);

  if (hyscan_data_schema_key_get_value_type (schema, key_id) != HYSCAN_DATA_SCHEMA_KEY_ENUM)
    g_error ("%s: type error", key_id);

  name = hyscan_data_schema_key_get_name (schema, key_id);
  description = hyscan_data_schema_key_get_description (schema, key_id);
  if (name == NULL || description == NULL)
    g_error ("%s: name or description error", key_id);

  if (hyscan_data_schema_key_get_view (schema, key_id) != HYSCAN_DATA_SCHEMA_VIEW_DATE)
    g_error ("%s: view error", key_id);

  access = hyscan_data_schema_key_get_access (schema, key_id);
  if (g_str_has_prefix (key_id, "/readonly") && (access != HYSCAN_DATA_SCHEMA_ACCESS_READONLY))
    g_error ("%s: access error", key_id);
  if (g_str_has_prefix (key_id, "/writeonly") && (access != HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY))
    g_error ("%s: access error", key_id);

  default_value = hyscan_data_schema_key_get_default (schema, key_id);
  if (default_value == NULL)
    g_error ("%s: default value error", key_id);

  check_name = g_strdup_printf ("Enum %" G_GINT64_FORMAT, g_variant_get_int64 (default_value));
  check_description = g_strdup_printf ("Enum %" G_GINT64_FORMAT " value", g_variant_get_int64 (default_value));

  if (g_strcmp0 (name, check_name) != 0)
    g_error ("%s: name mismatch ('%s' != '%s')", key_id, name, check_name);
  if (g_strcmp0 (description, check_description) != 0)
    g_error ("%s: description mismatch ('%s' != '%s')", key_id, description, check_description);

  g_free (check_name);
  g_free (check_description);

  for (i = g_variant_get_int64 (default_value); i > 0; i--)
    {
      GVariant *value = g_variant_new_int64 (i);
      if (!hyscan_data_schema_key_check (schema, key_id, value))
        g_error ("%s: range error", key_id);
      g_variant_unref (value);
    }

  g_variant_unref (default_value);
}

int
main (int argc, char **argv)
{
  HyScanDataSchemaBuilder *builder;
  HyScanDataSchema *schema;
  gchar *schema_data;
  gchar **keys_list;
  guint i;

  schema_data = test_schema_create ("orig");

  schema = hyscan_data_schema_new_from_string (schema_data, "orig");
  g_free (schema_data);

  builder = hyscan_data_schema_builder_new ("test");
  hyscan_data_schema_builder_schema_join (builder, "/", schema, "/orig");
  schema_data = hyscan_data_schema_builder_get_data (builder);
  g_object_unref (builder);
  g_object_unref (schema);

  schema = hyscan_data_schema_new_from_string (schema_data, "test");
  g_free (schema_data);

  keys_list = hyscan_data_schema_list_keys (schema);
  if (keys_list == NULL)
    g_error ("empty schema");

  for (i = 0; keys_list[i] != NULL; i++)
    {
      HyScanDataSchemaKeyType type = hyscan_data_schema_key_get_value_type (schema, keys_list[i]);

      g_message ("check key: %s", keys_list[i]);

      switch (type)
        {
        case HYSCAN_DATA_SCHEMA_KEY_BOOLEAN:
          check_boolean (schema, keys_list[i]);
          break;

        case HYSCAN_DATA_SCHEMA_KEY_INTEGER:
          check_integer (schema, keys_list[i]);
          break;

        case HYSCAN_DATA_SCHEMA_KEY_DOUBLE:
          check_double (schema, keys_list[i]);
          break;

        case HYSCAN_DATA_SCHEMA_KEY_STRING:
          check_string (schema, keys_list[i]);
          break;

        case HYSCAN_DATA_SCHEMA_KEY_ENUM:
          check_enum (schema, keys_list[i]);
          break;

        default:
          break;
        }
    }

  g_strfreev (keys_list);

  g_object_unref (schema);

  xmlCleanupParser ();

  g_message ("All done");

  return 0;
}
