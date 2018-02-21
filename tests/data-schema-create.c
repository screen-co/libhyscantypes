#include <hyscan-data-schema-builder.h>

static void
test_schema_set_node_description (HyScanDataSchemaBuilder *builder,
                                  const gchar             *path)
{
  gchar **pathv;

  pathv = g_strsplit (path, "/", -1);
  if (g_strv_length (pathv) > 1)
    {
      gchar *description;
      gchar *name;
      guint i;

      for (i = 0; pathv[i + 1] != NULL; i++);
      name = g_strdup (pathv[i]);
      name[0] = g_ascii_toupper (name[0]);
      description = g_strdup_printf ("%s description", name);

      hyscan_data_schema_builder_node_set_name (builder, path, name, description);

      g_free (description);
      g_free (name);
    }

  g_strfreev (pathv);
}

static void
test_schema_create_enums_values (HyScanDataSchemaBuilder *builder)
{
  hyscan_data_schema_builder_enum_create (builder, "enum1");
  hyscan_data_schema_builder_enum_value_create (builder, "enum1", 1, "Value1", "Value 1");

  hyscan_data_schema_builder_enum_create (builder, "enum2");
  hyscan_data_schema_builder_enum_value_create (builder, "enum2", 1, "Value1", "Value 1");
  hyscan_data_schema_builder_enum_value_create (builder, "enum2", 2, "Value2", "Value 2");

  hyscan_data_schema_builder_enum_create (builder, "enum3");
  hyscan_data_schema_builder_enum_value_create (builder, "enum3", 1, "Value1", "Value 1");
  hyscan_data_schema_builder_enum_value_create (builder, "enum3", 2, "Value2", "Value 2");
  hyscan_data_schema_builder_enum_value_create (builder, "enum3", 3, "Value3", "Value 3");

  hyscan_data_schema_builder_enum_create (builder, "enum4");
  hyscan_data_schema_builder_enum_value_create (builder, "enum4", 1, "Value1", "Value 1");
  hyscan_data_schema_builder_enum_value_create (builder, "enum4", 2, "Value2", "Value 2");
  hyscan_data_schema_builder_enum_value_create (builder, "enum4", 3, "Value3", "Value 3");
  hyscan_data_schema_builder_enum_value_create (builder, "enum4", 4, "Value4", "Value 4");

  hyscan_data_schema_builder_enum_create (builder, "enum5");
  hyscan_data_schema_builder_enum_value_create (builder, "enum5", 1, "Value1", "Value 1");
  hyscan_data_schema_builder_enum_value_create (builder, "enum5", 2, "Value2", "Value 2");
  hyscan_data_schema_builder_enum_value_create (builder, "enum5", 3, "Value3", "Value 3");
  hyscan_data_schema_builder_enum_value_create (builder, "enum5", 4, "Value4", "Value 4");
  hyscan_data_schema_builder_enum_value_create (builder, "enum5", 5, "Value5", "Value 5");
}

static void
test_schema_create_boolean (HyScanDataSchemaBuilder   *builder,
                            const gchar               *path,
                            gboolean                   value,
                            HyScanDataSchemaKeyAccess  access)
{
  gchar *key_id = g_strdup_printf ("%s/%s", path, value ? "true" : "false");
  gchar *name = g_strdup_printf ("%s", value ? "True" : "False");
  gchar *description = g_strdup_printf ("%s value", value ? "True" : "False");

  hyscan_data_schema_builder_key_boolean_create (builder, key_id, name, description, value);
  hyscan_data_schema_builder_key_set_view (builder, key_id, HYSCAN_DATA_SCHEMA_VIEW_BIN);
  hyscan_data_schema_builder_key_set_access (builder, key_id, access);

  g_free (key_id);
  g_free (name);
  g_free (description);
}

static void
test_schema_create_integer (HyScanDataSchemaBuilder   *builder,
                            const gchar               *path,
                            gint                       value,
                            HyScanDataSchemaKeyAccess  access)
{
  gchar *key_id = g_strdup_printf ("%s/integer%d", path, value);
  gchar *name = g_strdup_printf ("Integer %d", value);
  gchar *description = g_strdup_printf ("Integer %d value", value);

  hyscan_data_schema_builder_key_integer_create (builder, key_id, name, description, value);
  hyscan_data_schema_builder_key_set_view (builder, key_id, HYSCAN_DATA_SCHEMA_VIEW_HEX);
  hyscan_data_schema_builder_key_set_access (builder, key_id, access);
  hyscan_data_schema_builder_key_integer_range (builder, key_id, value, 1000 * value, value);

  g_free (key_id);
  g_free (name);
  g_free (description);
}

static void
test_schema_create_double (HyScanDataSchemaBuilder   *builder,
                           const gchar               *path,
                           gint                       value,
                           HyScanDataSchemaKeyAccess  access)
{
  gchar *key_id = g_strdup_printf ("%s/double%d", path, value);
  gchar *name = g_strdup_printf ("Double %d", value);
  gchar *description = g_strdup_printf ("Double %d value", value);

  hyscan_data_schema_builder_key_double_create (builder, key_id, name, description, value);
  hyscan_data_schema_builder_key_set_view (builder, key_id, HYSCAN_DATA_SCHEMA_VIEW_DEC);
  hyscan_data_schema_builder_key_set_access (builder, key_id, access);
  hyscan_data_schema_builder_key_double_range (builder, key_id, value, 1000.0 * value, value);

  g_free (key_id);
  g_free (name);
  g_free (description);
}

static void
test_schema_create_string (HyScanDataSchemaBuilder   *builder,
                           const gchar               *path,
                           gint                       value,
                           HyScanDataSchemaKeyAccess  access)
{
  gchar *key_id = g_strdup_printf ("%s/string%d", path, value);
  gchar *name = g_strdup_printf ("String %d", value);
  gchar *description = g_strdup_printf ("String %d value", value);

  hyscan_data_schema_builder_key_string_create (builder, key_id, name, description, name);
  hyscan_data_schema_builder_key_set_view (builder, key_id, HYSCAN_DATA_SCHEMA_VIEW_SCHEMA);
  hyscan_data_schema_builder_key_set_access (builder, key_id, access);

  g_free (key_id);
  g_free (name);
  g_free (description);
}

static void
test_schema_create_null_string (HyScanDataSchemaBuilder   *builder,
                                const gchar               *path,
                                gint                       value,
                                HyScanDataSchemaKeyAccess  access)
{
  gchar *key_id = g_strdup_printf ("%s/null%d", path, value);
  gchar *name = g_strdup_printf ("NULL string %d", value);
  gchar *description = g_strdup_printf ("NULL string %d value", value);

  hyscan_data_schema_builder_key_string_create (builder, key_id, name, description, NULL);
  hyscan_data_schema_builder_key_set_view (builder, key_id, HYSCAN_DATA_SCHEMA_VIEW_SCHEMA);
  hyscan_data_schema_builder_key_set_access (builder, key_id, access);

  g_free (key_id);
  g_free (name);
  g_free (description);
}

static void
test_schema_create_enum (HyScanDataSchemaBuilder   *builder,
                         const gchar               *path,
                         gint                       value,
                         HyScanDataSchemaKeyAccess  access)
{
  gchar *key_id = g_strdup_printf ("%s/enum%d", path, value);
  gchar *name = g_strdup_printf ("Enum %d", value);
  gchar *description = g_strdup_printf ("Enum %d value", value);
  gchar *enum_id = g_strdup_printf ("enum%d", value);

  hyscan_data_schema_builder_key_enum_create (builder, key_id, name, description, enum_id, value);
  hyscan_data_schema_builder_key_set_view (builder, key_id, HYSCAN_DATA_SCHEMA_VIEW_DATE);
  hyscan_data_schema_builder_key_set_access (builder, key_id, access);

  g_free (key_id);
  g_free (name);
  g_free (description);
  g_free (enum_id);
}

static void
test_schema_create_complex (HyScanDataSchemaBuilder *builder,
                            const gchar             *path)
{
  gchar *node_path;
  guint i;

  test_schema_create_boolean (builder, path, FALSE, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
  test_schema_create_integer (builder, path, 2, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
  test_schema_create_double (builder, path, 2, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
  test_schema_create_string (builder, path, 2, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
  test_schema_create_null_string (builder, path, 2, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
  test_schema_create_enum (builder, path, 2, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);

  node_path = g_strdup_printf ("%s/booleans", path);
  test_schema_create_boolean (builder, node_path, TRUE, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
  test_schema_create_boolean (builder, node_path, FALSE, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
  test_schema_set_node_description (builder, node_path);
  g_free (node_path);

  for (i = 1; i <= 5; i++)
    {
      node_path = g_strdup_printf ("%s/integers", path);
      test_schema_create_integer (builder, node_path, i, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
      test_schema_set_node_description (builder, node_path);
      g_free (node_path);

      node_path = g_strdup_printf ("%s/doubles", path);
      test_schema_create_double (builder, node_path, i, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
      test_schema_set_node_description (builder, node_path);
      g_free (node_path);

      node_path = g_strdup_printf ("%s/strings", path);
      test_schema_create_string (builder, node_path, i, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
      test_schema_set_node_description (builder, node_path);
      g_free (node_path);

      node_path = g_strdup_printf ("%s/strings", path);
      test_schema_create_null_string (builder, node_path, i, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
      test_schema_set_node_description (builder, node_path);
      g_free (node_path);

      node_path = g_strdup_printf ("%s/enums", path);
      test_schema_create_enum (builder, node_path, i, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
      test_schema_set_node_description (builder, node_path);
      g_free (node_path);
    }

  test_schema_set_node_description (builder, path);
}

gchar *
test_schema_create (const gchar *schema_id)
{
  gchar *id;
  gchar *data;
  HyScanDataSchema *schema;
  HyScanDataSchemaBuilder *builder;

  builder = hyscan_data_schema_builder_new (schema_id);

  test_schema_create_enums_values (builder);

  test_schema_create_boolean (builder, "/builder/test", TRUE, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
  test_schema_create_integer (builder, "/builder/test", 1, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
  test_schema_create_double (builder, "/builder/test", 1, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
  test_schema_create_string (builder, "/builder/test", 1, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
  test_schema_create_null_string (builder, "/builder/test", 1, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
  test_schema_create_enum (builder, "/builder/test", 1, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
  test_schema_set_node_description (builder, "/builder/test");

  test_schema_create_boolean (builder, "/builder/test/readonly", FALSE, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  test_schema_create_integer (builder, "/builder/test/readonly", 2, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  test_schema_create_double (builder, "/builder/test/readonly", 2, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  test_schema_create_string (builder, "/builder/test/readonly", 2, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  test_schema_create_null_string (builder, "/builder/test/readonly", 2, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  test_schema_create_enum (builder, "/builder/test/readonly", 2, HYSCAN_DATA_SCHEMA_ACCESS_READONLY);
  test_schema_set_node_description (builder, "/builder/test/readonly");

  test_schema_create_boolean (builder, "/builder/test/writeonly", FALSE, HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY);
  test_schema_create_integer (builder, "/builder/test/writeonly", 2, HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY);
  test_schema_create_double (builder, "/builder/test/writeonly", 2, HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY);
  test_schema_create_string (builder, "/builder/test/writeonly", 2, HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY);
  test_schema_create_null_string (builder, "/builder/test/writeonly", 2, HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY);
  test_schema_create_enum (builder, "/builder/test/writeonly", 2, HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY);
  test_schema_set_node_description (builder, "/builder/test/writeonly");

  test_schema_create_complex (builder, "/builder/test/complex1");
  test_schema_create_complex (builder, "/builder/test/complex2");
  test_schema_create_complex (builder, "/builder/test/complex3");
  test_schema_create_complex (builder, "/builder/test/complex4");
  test_schema_create_complex (builder, "/builder/test/complex5");

  data = hyscan_data_schema_builder_get_data (builder);
  id = hyscan_data_schema_builder_get_id (builder);
  schema = hyscan_data_schema_new_from_string (data, id);
  g_object_unref (builder);
  g_free (data);
  g_free (id);

  builder = hyscan_data_schema_builder_new (schema_id);
  hyscan_data_schema_builder_schema_join (builder, "/orig", schema, "/builder");
  data = hyscan_data_schema_builder_get_data (builder);
  g_object_unref (builder);
  g_object_unref (schema);

  return data;
}
