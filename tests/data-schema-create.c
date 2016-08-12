#include <hyscan-data-schema-builder.h>

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
test_schema_create_boolean (HyScanDataSchemaBuilder *builder,
                            const gchar             *path,
                            gboolean                 readonly,
                            gboolean                 value)
{
  gchar *key_id = g_strdup_printf ("%s/%s", path, value ? "true" : "false");
  gchar *name = g_strdup_printf ("%s", value ? "True" : "False");
  gchar *description = g_strdup_printf ("%s value", value ? "True" : "False");

  hyscan_data_schema_builder_key_boolean_create (builder, key_id, name, description, readonly, value);

  g_free (key_id);
  g_free (name);
  g_free (description);
}

static void
test_schema_create_integer (HyScanDataSchemaBuilder *builder,
                            const gchar             *path,
                            gboolean                 readonly,
                            gint                     value)
{
  gchar *key_id = g_strdup_printf ("%s/integer%d", path, value);
  gchar *name = g_strdup_printf ("Integer %d", value);
  gchar *description = g_strdup_printf ("Integer %d value", value);

  hyscan_data_schema_builder_key_integer_create (builder, key_id, name, description, readonly,
                                                 value, value, value * 1000, value);

  g_free (key_id);
  g_free (name);
  g_free (description);
}

static void
test_schema_create_double (HyScanDataSchemaBuilder *builder,
                           const gchar             *path,
                           gboolean                 readonly,
                           gint                     value)
{
  gchar *key_id = g_strdup_printf ("%s/double%d", path, value);
  gchar *name = g_strdup_printf ("Double %d", value);
  gchar *description = g_strdup_printf ("Double %d value", value);

  hyscan_data_schema_builder_key_double_create (builder, key_id, name, description, readonly,
                                                value, value, value * 1000.0, value);

  g_free (key_id);
  g_free (name);
  g_free (description);
}

static void
test_schema_create_string (HyScanDataSchemaBuilder *builder,
                           const gchar             *path,
                           gboolean                 readonly,
                           gint                     value)
{
  gchar *key_id = g_strdup_printf ("%s/string%d", path, value);
  gchar *name = g_strdup_printf ("String %d", value);
  gchar *description = g_strdup_printf ("String %d value", value);

  hyscan_data_schema_builder_key_string_create (builder, key_id, name, description, readonly, name);

  g_free (key_id);
  g_free (name);
  g_free (description);
}

static void
test_schema_create_null_string (HyScanDataSchemaBuilder *builder,
                                const gchar             *path,
                                gboolean                 readonly,
                                gint                     value)
{
  gchar *key_id = g_strdup_printf ("%s/null%d", path, value);
  gchar *name = g_strdup_printf ("NULL string %d", value);
  gchar *description = g_strdup_printf ("NULL string %d value", value);

  hyscan_data_schema_builder_key_string_create (builder, key_id, name, description, readonly, NULL);

  g_free (key_id);
  g_free (name);
  g_free (description);
}

static void
test_schema_create_enum (HyScanDataSchemaBuilder *builder,
                         const gchar             *path,
                         gboolean                 readonly,
                         gint                     value)
{
  gchar *key_id = g_strdup_printf ("%s/enum%d", path, value);
  gchar *name = g_strdup_printf ("Enum %d", value);
  gchar *description = g_strdup_printf ("Enum %d value", value);
  gchar *enum_id = g_strdup_printf ("enum%d", value);

  hyscan_data_schema_builder_key_enum_create (builder, key_id, name, description, readonly, enum_id, value);

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
  gint i;

  test_schema_create_boolean (builder, path, FALSE, FALSE);
  test_schema_create_integer (builder, path, FALSE, 2);
  test_schema_create_double (builder, path, FALSE, 2);
  test_schema_create_string (builder, path, FALSE, 2);
  test_schema_create_null_string (builder, path, FALSE, 2);
  test_schema_create_enum (builder, path, FALSE, 2);

  node_path = g_strdup_printf ("%s/booleans", path);
  test_schema_create_boolean (builder, node_path, FALSE, TRUE);
  test_schema_create_boolean (builder, node_path, FALSE, FALSE);
  g_free (node_path);

  for (i = 1; i <= 5; i++)
    {
      node_path = g_strdup_printf ("%s/integers", path);
      test_schema_create_integer (builder, node_path, FALSE, i);
      g_free (node_path);

      node_path = g_strdup_printf ("%s/doubles", path);
      test_schema_create_double (builder, node_path, FALSE, i);
      g_free (node_path);

      node_path = g_strdup_printf ("%s/strings", path);
      test_schema_create_string (builder, node_path, FALSE, i);
      g_free (node_path);

      node_path = g_strdup_printf ("%s/strings", path);
      test_schema_create_null_string (builder, node_path, FALSE, i);
      g_free (node_path);

      node_path = g_strdup_printf ("%s/enums", path);
      test_schema_create_enum (builder, node_path, FALSE, i);
      g_free (node_path);
    }
}

gchar *
test_schema_create (const gchar *schema_id)
{
  gchar *data;
  HyScanDataSchemaBuilder *builder;

  builder = hyscan_data_schema_builder_new (schema_id);

  test_schema_create_enums_values (builder);

  test_schema_create_boolean (builder, "", FALSE, TRUE);
  test_schema_create_integer (builder, "", FALSE, 1);
  test_schema_create_double (builder, "", FALSE, 1);
  test_schema_create_string (builder, "", FALSE, 1);
  test_schema_create_null_string (builder, "", FALSE, 1);
  test_schema_create_enum (builder, "", FALSE, 1);

  test_schema_create_boolean (builder, "/readonly", TRUE, FALSE);
  test_schema_create_integer (builder, "/readonly", TRUE, 2);
  test_schema_create_double (builder, "/readonly", TRUE, 2);
  test_schema_create_string (builder, "/readonly", TRUE, 2);
  test_schema_create_null_string (builder, "/readonly", TRUE, 2);
  test_schema_create_enum (builder, "/readonly", TRUE, 2);

  test_schema_create_complex (builder, "/complex1");
  test_schema_create_complex (builder, "/complex2");
  test_schema_create_complex (builder, "/complex3");
  test_schema_create_complex (builder, "/complex4");
  test_schema_create_complex (builder, "/complex5");

  data = hyscan_data_schema_builder_get_data (builder);

  g_object_unref (builder);

  return data;
}
