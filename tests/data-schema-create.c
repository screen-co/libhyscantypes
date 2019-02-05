/* data-schema-create.c
 *
 * Copyright 2017-2018 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
 *
 * This file is part of HyScanTypes.
 *
 * HyScanTypes is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HyScanTypes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Alternatively, you can license this code under a commercial license.
 * Contact the Screen LLC in this case - <info@screen-co.ru>.
 */

/* HyScanTypes имеет двойную лицензию.
 *
 * Во-первых, вы можете распространять HyScanTypes на условиях Стандартной
 * Общественной Лицензии GNU версии 3, либо по любой более поздней версии
 * лицензии (по вашему выбору). Полные положения лицензии GNU приведены в
 * <http://www.gnu.org/licenses/>.
 *
 * Во-вторых, этот программный код можно использовать по коммерческой
 * лицензии. Для этого свяжитесь с ООО Экран - <info@screen-co.ru>.
 */

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
  hyscan_data_schema_builder_enum_value_create (builder, "enum1", 1, "value-1", "Value1", "Value 1");

  hyscan_data_schema_builder_enum_create (builder, "enum2");
  hyscan_data_schema_builder_enum_value_create (builder, "enum2", 1, "value-1", "Value1", "Value 1");
  hyscan_data_schema_builder_enum_value_create (builder, "enum2", 2, "value-2", "Value2", "Value 2");

  hyscan_data_schema_builder_enum_create (builder, "enum3");
  hyscan_data_schema_builder_enum_value_create (builder, "enum3", 1, "value-1", "Value1", "Value 1");
  hyscan_data_schema_builder_enum_value_create (builder, "enum3", 2, "value-2", "Value2", "Value 2");
  hyscan_data_schema_builder_enum_value_create (builder, "enum3", 3, "value-3", "Value3", "Value 3");

  hyscan_data_schema_builder_enum_create (builder, "enum4");
  hyscan_data_schema_builder_enum_value_create (builder, "enum4", 1, "value-1", "Value1", "Value 1");
  hyscan_data_schema_builder_enum_value_create (builder, "enum4", 2, "value-2", "Value2", "Value 2");
  hyscan_data_schema_builder_enum_value_create (builder, "enum4", 3, "value-3", "Value3", "Value 3");
  hyscan_data_schema_builder_enum_value_create (builder, "enum4", 4, "value-4", "Value4", "Value 4");

  hyscan_data_schema_builder_enum_create (builder, "enum5");
  hyscan_data_schema_builder_enum_value_create (builder, "enum5", 1, "value-1", "Value1", "Value 1");
  hyscan_data_schema_builder_enum_value_create (builder, "enum5", 2, "value-2", "Value2", "Value 2");
  hyscan_data_schema_builder_enum_value_create (builder, "enum5", 3, "value-3", "Value3", "Value 3");
  hyscan_data_schema_builder_enum_value_create (builder, "enum5", 4, "value-4", "Value4", "Value 4");
  hyscan_data_schema_builder_enum_value_create (builder, "enum5", 5, "value-5", "Value5", "Value 5");
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
  if (access != HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT)
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
  if (access != HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT)
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
  if (access != HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT)
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
  if (access != HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT)
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
  if (access != HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT)
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
  if (access != HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT)
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
  gchar *data;
  HyScanDataSchema *schema;
  HyScanDataSchemaBuilder *builder;

  builder = hyscan_data_schema_builder_new (schema_id);

  test_schema_create_enums_values (builder);

  test_schema_create_boolean     (builder, "/builder/test", TRUE, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
  test_schema_create_integer     (builder, "/builder/test", 1, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
  test_schema_create_double      (builder, "/builder/test", 1, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
  test_schema_create_string      (builder, "/builder/test", 1, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
  test_schema_create_null_string (builder, "/builder/test", 1, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
  test_schema_create_enum        (builder, "/builder/test", 1, HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT);
  test_schema_set_node_description (builder, "/builder/test");

  test_schema_create_boolean     (builder, "/builder/test/hidden", FALSE,
                                  HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT | HYSCAN_DATA_SCHEMA_ACCESS_HIDDEN);
  test_schema_create_integer     (builder, "/builder/test/hidden", 2,
                                  HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT | HYSCAN_DATA_SCHEMA_ACCESS_HIDDEN);
  test_schema_create_double      (builder, "/builder/test/hidden", 2,
                                  HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT | HYSCAN_DATA_SCHEMA_ACCESS_HIDDEN);
  test_schema_create_string      (builder, "/builder/test/hidden", 2,
                                  HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT | HYSCAN_DATA_SCHEMA_ACCESS_HIDDEN);
  test_schema_create_null_string (builder, "/builder/test/hidden", 2,
                                  HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT | HYSCAN_DATA_SCHEMA_ACCESS_HIDDEN);
  test_schema_create_enum        (builder, "/builder/test/hidden", 2,
                                  HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT | HYSCAN_DATA_SCHEMA_ACCESS_HIDDEN);
  test_schema_set_node_description (builder, "/builder/test/hidden");

  test_schema_create_boolean     (builder, "/builder/test/readonly", FALSE, HYSCAN_DATA_SCHEMA_ACCESS_READ);
  test_schema_create_integer     (builder, "/builder/test/readonly", 2, HYSCAN_DATA_SCHEMA_ACCESS_READ);
  test_schema_create_double      (builder, "/builder/test/readonly", 2, HYSCAN_DATA_SCHEMA_ACCESS_READ);
  test_schema_create_string      (builder, "/builder/test/readonly", 2, HYSCAN_DATA_SCHEMA_ACCESS_READ);
  test_schema_create_null_string (builder, "/builder/test/readonly", 2, HYSCAN_DATA_SCHEMA_ACCESS_READ);
  test_schema_create_enum        (builder, "/builder/test/readonly", 2, HYSCAN_DATA_SCHEMA_ACCESS_READ);
  test_schema_set_node_description (builder, "/builder/test/readonly");

  test_schema_create_boolean     (builder, "/builder/test/writeonly", FALSE, HYSCAN_DATA_SCHEMA_ACCESS_WRITE);
  test_schema_create_integer     (builder, "/builder/test/writeonly", 2, HYSCAN_DATA_SCHEMA_ACCESS_WRITE);
  test_schema_create_double      (builder, "/builder/test/writeonly", 2, HYSCAN_DATA_SCHEMA_ACCESS_WRITE);
  test_schema_create_string      (builder, "/builder/test/writeonly", 2, HYSCAN_DATA_SCHEMA_ACCESS_WRITE);
  test_schema_create_null_string (builder, "/builder/test/writeonly", 2, HYSCAN_DATA_SCHEMA_ACCESS_WRITE);
  test_schema_create_enum        (builder, "/builder/test/writeonly", 2, HYSCAN_DATA_SCHEMA_ACCESS_WRITE);
  test_schema_set_node_description (builder, "/builder/test/writeonly");

  test_schema_create_complex (builder, "/builder/test/complex1");
  test_schema_create_complex (builder, "/builder/test/complex2");
  test_schema_create_complex (builder, "/builder/test/complex3");
  test_schema_create_complex (builder, "/builder/test/complex4");
  test_schema_create_complex (builder, "/builder/test/complex5");

  schema = hyscan_data_schema_builder_get_schema (builder);
  g_object_unref (builder);

  builder = hyscan_data_schema_builder_new (schema_id);
  hyscan_data_schema_builder_schema_join (builder, "/orig", schema, "/builder");
  data = hyscan_data_schema_builder_get_data (builder);
  g_object_unref (builder);
  g_object_unref (schema);

  return data;
}
