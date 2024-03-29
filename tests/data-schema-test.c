/* data-schema-test.c
 *
 * Copyright 2017-2019 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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

#include <gio/gio.h>
#include <hyscan-data-schema-builder.h>

#define SCHEMA_ID_STAGE1        "orig"
#define SCHEMA_ID_STAGE2        "test"
#define SCHEMA_VERSION_STAGE1   123456
#define SCHEMA_VERSION_STAGE2   123456
#define SCHEMA_NAME             "Test schema"
#define SCHEMA_DESCRIPTION      "Test schema description"

gchar *test_schema_create (const gchar *schema_id);

/* Функция ищет параметр в списке. */
HyScanDataSchemaKey *
find_node (const HyScanDataSchemaNode *node,
           const gchar                *key_id)
{
  GList *keys = node->keys;
  GList *nodes = node->nodes;

  while (keys)
    {
      HyScanDataSchemaKey *key = keys->data;

      if (g_strcmp0 (key->id, key_id) == 0)
        return key;

      keys = g_list_next (keys);
    }

  while (nodes)
    {
      HyScanDataSchemaKey *key = find_node (nodes->data, key_id);

      if ((key != NULL) && (g_strcmp0 (key->id, key_id) == 0))
        return key;

      nodes = g_list_next (nodes);
    }

  return NULL;
}

/* Функция рекурсивно проверяет описание узлов. */
void
check_node (HyScanDataSchema           *schema,
            const HyScanDataSchemaNode *nodes,
            gboolean                    silent)
{
  gchar *description = NULL;
  gchar *name = NULL;

  if (!silent)
    g_message ("check node: %s", nodes->path);

  if (g_strcmp0 (nodes->path, "/") == 0)
    {
      name = g_strdup (SCHEMA_NAME);
      description = g_strdup (SCHEMA_DESCRIPTION);
    }
  else
    {
      gchar **pathv = g_strsplit (nodes->path, "/", -1);

      if (g_strv_length (pathv) > 1)
        {
          guint i;
          for (i = 0; pathv[i + 1] != NULL; i++);
          name = g_strdup (pathv[i]);
          name[0] = g_ascii_toupper (name[0]);
          description = g_strdup_printf ("%s description", name);
        }

      g_strfreev (pathv);
    }

  if ((g_strcmp0 (nodes->name, name) != 0) ||
      (g_strcmp0 (nodes->description, description) != 0))
    {
      g_message ("%s: %s (%s)", nodes->path, nodes->name, nodes->description);
      g_error ("%s: node description error", nodes->path);
    }

  g_free (description);
  g_free (name);


  /* Проверка дочерних узлов. */
  if (nodes->nodes != NULL)
    {
      GList *cur_node  = nodes->nodes;
      while (cur_node != NULL)
        {
          check_node (schema, cur_node->data, silent);

          cur_node = g_list_next (cur_node);
        }
    }
}

/* Функция проверки описания параметра. */
void
check_key (HyScanDataSchema           *schema,
           const HyScanDataSchemaNode *nodes,
           const gchar                *key_id)
{
  HyScanDataSchemaKey *key;

  const gchar *name;
  const gchar *description;
  HyScanDataSchemaKeyAccess access;

  key = find_node (nodes, key_id);
  if (key == NULL)
    g_error ("%s: unknown key", key_id);

  name = hyscan_data_schema_key_get_name (schema, key_id);
  description = hyscan_data_schema_key_get_description (schema, key_id);
  access = hyscan_data_schema_key_get_access (schema, key_id);

  if (g_strcmp0 (key->id, key_id) != 0)
    g_error ("%s: node id error", key_id);

  if (g_strcmp0 (key->name, name) != 0)
    g_error ("%s: node name error", key_id);

  if (g_strcmp0 (key->description, description) != 0)
    g_error ("%s: node description error", key_id);

  if (key->type != hyscan_data_schema_key_get_value_type (schema, key_id))
    g_error ("%s: node type error", key_id);

  if (key->view != hyscan_data_schema_key_get_view (schema, key_id))
    g_error ("%s: node view error", key_id);

  if (key->access != hyscan_data_schema_key_get_access (schema, key_id))
    g_error ("%s: node access error", key_id);

  if (g_pattern_match_simple ("*readonly*", key_id) && (access & HYSCAN_DATA_SCHEMA_ACCESS_WRITE))
    g_error ("%s: access error", key_id);

  if (g_pattern_match_simple ("*writeonly*", key_id) && (access & HYSCAN_DATA_SCHEMA_ACCESS_READ))
    g_error ("%s: access error", key_id);

  if (g_pattern_match_simple ("*hidden*", key_id) && !(access & HYSCAN_DATA_SCHEMA_ACCESS_HIDDEN))
    g_error ("%s: access error", key_id);
}

/* Функция проверки параметра типа BOOLEAN. */
void
check_boolean (HyScanDataSchema *schema,
               const gchar      *key_id)
{
  const gchar *name;
  const gchar *description;

  gchar *check_name;
  gchar *check_description;

  GVariant *default_value1;
  gboolean default_value2;

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

  default_value1 = hyscan_data_schema_key_get_default (schema, key_id);
  if (default_value1 == NULL)
    g_error ("%s: default value1 error", key_id);

  if (!hyscan_data_schema_key_get_boolean (schema, key_id, &default_value2))
    g_error ("%s: value2 error", key_id);

  if (g_variant_get_boolean (default_value1) != default_value2)
    g_error ("%s: default value error", key_id);

  check_name = g_strdup_printf ("%s", g_variant_get_boolean (default_value1) ? "True" : "False");
  check_description = g_strdup_printf ("%s value", g_variant_get_boolean (default_value1) ? "True" : "False");

  if (g_strcmp0 (name, check_name) != 0)
    g_error ("%s: name mismatch ('%s' != '%s')", key_id, name, check_name);
  if (g_strcmp0 (description, check_description) != 0)
    g_error ("%s: description mismatch ('%s' != '%s')", key_id, description, check_description);

  g_free (check_name);
  g_free (check_description);

  g_variant_unref (default_value1);
}

/* Функция проверки параметра типа INTEGER. */
void
check_integer (HyScanDataSchema *schema,
               const gchar      *key_id)
{
  const gchar *name;
  const gchar *description;

  gchar *check_name;
  gchar *check_description;

  GVariant *default_value1;
  GVariant *minimum_value1;
  GVariant *maximum_value1;
  GVariant *value_step1;

  gint64 default_value2;
  gint64 minimum_value2;
  gint64 maximum_value2;
  gint64 value_step2;

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

  default_value1 = hyscan_data_schema_key_get_default (schema, key_id);
  if (default_value1 == NULL)
    g_error ("%s: default value1 error", key_id);

  minimum_value1 = hyscan_data_schema_key_get_minimum (schema, key_id);
  if (minimum_value1 == NULL)
    g_error ("%s: minimum value1 error", key_id);

  maximum_value1 = hyscan_data_schema_key_get_maximum (schema, key_id);
  if (maximum_value1 == NULL)
    g_error ("%s: maximum value1 error", key_id);

  value_step1 = hyscan_data_schema_key_get_step (schema, key_id);
  if (value_step1 == NULL)
    g_error ("%s: value step1 error", key_id);

  if (!hyscan_data_schema_key_get_integer (schema, key_id,
                                           &minimum_value2, &maximum_value2,
                                           &default_value2, &value_step2))
    {
      g_error ("%s: value2 error", key_id);
    }

  check_name = g_strdup_printf ("Integer %" G_GINT64_FORMAT, g_variant_get_int64 (default_value1));
  check_description = g_strdup_printf ("Integer %" G_GINT64_FORMAT " value", g_variant_get_int64 (default_value1));

  if (g_strcmp0 (name, check_name) != 0)
    g_error ("%s: name mismatch ('%s' != '%s')", key_id, name, check_name);
  if (g_strcmp0 (description, check_description) != 0)
    g_error ("%s: description mismatch ('%s' != '%s')", key_id, description, check_description);

  g_free (check_name);
  g_free (check_description);

  if (g_variant_get_int64 (default_value1) != default_value2)
    g_error ("%s: default value error", key_id);

  if ((g_variant_get_int64 (default_value1) != g_variant_get_int64 (minimum_value1)) ||
      (g_variant_get_int64 (minimum_value1) != minimum_value2))
    {
      g_error ("%s: minimum value error", key_id);
    }

  if ((g_variant_get_int64 (default_value1) != g_variant_get_int64 (maximum_value1) / 1000) ||
      (g_variant_get_int64 (maximum_value1) != maximum_value2))
    {
      g_error ("%s: maximum value error", key_id);
    }

  if ((g_variant_get_int64 (default_value1) != g_variant_get_int64 (value_step1)) ||
      (g_variant_get_int64 (value_step1) != value_step2))
    {
      g_error ("%s: value step error", key_id);
    }

  g_variant_unref (default_value1);
  g_variant_unref (minimum_value1);
  g_variant_unref (maximum_value1);
  g_variant_unref (value_step1);
}

/* Функция проверки параметра типа DOUBLE. */
void
check_double (HyScanDataSchema *schema,
              const gchar      *key_id)
{
  const gchar *name;
  const gchar *description;

  gchar *check_name;
  gchar *check_description;

  GVariant *default_value1;
  GVariant *minimum_value1;
  GVariant *maximum_value1;
  GVariant *value_step1;

  gdouble default_value2;
  gdouble minimum_value2;
  gdouble maximum_value2;
  gdouble value_step2;

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

  default_value1 = hyscan_data_schema_key_get_default (schema, key_id);
  if (default_value1 == NULL)
    g_error ("%s: default value1 error", key_id);

  minimum_value1 = hyscan_data_schema_key_get_minimum (schema, key_id);
  if (minimum_value1 == NULL)
    g_error ("%s: minimum value1 error", key_id);

  maximum_value1 = hyscan_data_schema_key_get_maximum (schema, key_id);
  if (maximum_value1 == NULL)
    g_error ("%s: maximum value1 error", key_id);

  value_step1 = hyscan_data_schema_key_get_step (schema, key_id);
  if (value_step1 == NULL)
    g_error ("%s: value step1 error", key_id);


  if (!hyscan_data_schema_key_get_double (schema, key_id,
                                           &minimum_value2, &maximum_value2,
                                           &default_value2, &value_step2))
    {
      g_error ("%s: value2 error", key_id);
    }

  check_name = g_strdup_printf ("Double %" G_GINT64_FORMAT, (gint64)g_variant_get_double (default_value1));
  check_description = g_strdup_printf ("Double %" G_GINT64_FORMAT " value", (gint64)g_variant_get_double (default_value1));

  if (g_strcmp0 (name, check_name) != 0)
    g_error ("%s: name mismatch ('%s' != '%s')", key_id, name, check_name);
  if (g_strcmp0 (description, check_description) != 0)
    g_error ("%s: description mismatch ('%s' != '%s')", key_id, description, check_description);

  g_free (check_name);
  g_free (check_description);

  if (g_variant_get_double (default_value1) != default_value2)
    g_error ("%s: default value error", key_id);

  if ((g_variant_get_double (default_value1) != g_variant_get_double (minimum_value1)) ||
      (g_variant_get_double (minimum_value1) != minimum_value2))
    {
      g_error ("%s: minimum value error", key_id);
    }

  if ((g_variant_get_double (default_value1) != g_variant_get_double (maximum_value1) / 1000) ||
      (g_variant_get_double (maximum_value1) != maximum_value2))
    {
      g_error ("%s: maximum value error", key_id);
    }

  if ((g_variant_get_double (default_value1) != g_variant_get_double (value_step1)) ||
      (g_variant_get_double (value_step1) != value_step2))
    {
      g_error ("%s: value step error", key_id);
    }

  g_variant_unref (default_value1);
  g_variant_unref (minimum_value1);
  g_variant_unref (maximum_value1);
  g_variant_unref (value_step1);
}

/* Функция проверки параметра типа STRING. */
void
check_string (HyScanDataSchema *schema,
              const gchar      *key_id)
{
  const gchar *name;
  const gchar *description;

  gchar *check_name;
  gchar *check_description;

  GVariant *default_value1;
  const gchar *default_value2;

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

  default_value1 = hyscan_data_schema_key_get_default (schema, key_id);
  default_value2 = hyscan_data_schema_key_get_string (schema, key_id);

  if (g_pattern_match_simple ("*null*", key_id))
    {
      if ((default_value1 != NULL) || (default_value2 != NULL))
        g_error ("%s: default value error", key_id);
    }
  else
    {
      if ((default_value1 == NULL) || (default_value2 == NULL))
        g_error ("%s: default value error", key_id);

      check_name = g_strdup_printf ("%s", g_variant_get_string (default_value1, NULL));
      check_description = g_strdup_printf ("%s value", g_variant_get_string (default_value1, NULL));

      if (g_strcmp0 (g_variant_get_string (default_value1, NULL), default_value2) != 0)
        g_error ("%s: default value mismatch", key_id);
      if (g_strcmp0 (name, check_name) != 0)
        g_error ("%s: name mismatch ('%s' != '%s')", key_id, name, check_name);
      if (g_strcmp0 (description, check_description) != 0)
        g_error ("%s: description mismatch ('%s' != '%s')", key_id, description, check_description);

      g_free (check_name);
      g_free (check_description);

      g_variant_unref (default_value1);
    }
}

/* Функция проверки параметра типа ENUM. */
void
check_enum (HyScanDataSchema *schema,
            const gchar      *key_id)
{
  const gchar *name;
  const gchar *description;
  const gchar *enum_id;
  GList *enum_values;

  gchar *check_name;
  gchar *check_description;

  GVariant *default_value1;
  gint64 default_value2;
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

  default_value1 = hyscan_data_schema_key_get_default (schema, key_id);
  if (default_value1 == NULL)
    g_error ("%s: default value1 error", key_id);

  if (!hyscan_data_schema_key_get_enum (schema, key_id, &default_value2))
    g_error ("%s: value2 error", key_id);

  if (g_variant_get_int64 (default_value1) != default_value2)
    g_error ("%s: default value error", key_id);

  enum_id = hyscan_data_schema_key_get_enum_id (schema, key_id);
  if (enum_id == NULL)
    g_error ("%s: couldn't get enum id", key_id);

  enum_values = hyscan_data_schema_enum_get_values (schema, enum_id);
  if (enum_values == NULL)
    g_error ("%s: couldn't get enum values", key_id);

  g_clear_pointer (&enum_values, g_list_free);

  check_name = g_strdup_printf ("Enum %" G_GINT64_FORMAT, g_variant_get_int64 (default_value1));
  check_description = g_strdup_printf ("Enum %" G_GINT64_FORMAT " value", g_variant_get_int64 (default_value1));

  if (g_strcmp0 (name, check_name) != 0)
    g_error ("%s: name mismatch ('%s' != '%s')", key_id, name, check_name);
  if (g_strcmp0 (description, check_description) != 0)
    g_error ("%s: description mismatch ('%s' != '%s')", key_id, description, check_description);

  g_free (check_name);
  g_free (check_description);

  for (i = g_variant_get_int64 (default_value1); i > 0; i--)
    {
      gchar *value_id = g_strdup_printf ("value-%d", (gint)i);
      const HyScanDataSchemaEnumValue *enum_value;
      GVariant *value;

      enum_value = hyscan_data_schema_enum_find_by_id (schema, enum_id, value_id);
      if (enum_value == NULL || enum_value->value != i)
        g_error ("%s: enum by id value error", key_id);
      g_free (value_id);

      enum_value = hyscan_data_schema_enum_find_by_value (schema, enum_id, i);
      if (enum_value == NULL || enum_value->value != i)
        g_error ("%s: enum by value error", key_id);

      value = g_variant_new_int64 (i);
      if (!hyscan_data_schema_key_check (schema, key_id, value))
        g_error ("%s: range error", key_id);
      g_variant_unref (value);
    }

  g_variant_unref (default_value1);
}

int
main (int    argc,
      char **argv)
{
  gchar *schema_file = NULL;
  gboolean silent = FALSE;

  HyScanDataSchemaBuilder *builder;
  HyScanDataSchema *schema;
  gchar *schema_data;
  const gchar * const *keys_list;
  const HyScanDataSchemaNode *nodes;
  guint i;

  /* Разбор командной строки. */
  {
    gchar **args;
    GError *error = NULL;
    GOptionContext *context;
    GOptionEntry entries[] =
      {
        { "dump-schema", 'd', 0, G_OPTION_ARG_STRING, &schema_file, "Dump test schema to file", NULL },
        { "silent", 's', 0, G_OPTION_ARG_NONE, &silent, "Don't show messages", NULL },
        { NULL }
      };

#ifdef G_OS_WIN32
    args = g_win32_get_command_line ();
#else
    args = g_strdupv (argv);
#endif

    context = g_option_context_new ("");
    g_option_context_set_help_enabled (context, TRUE);
    g_option_context_add_main_entries (context, entries, NULL);
    g_option_context_set_ignore_unknown_options (context, FALSE);
    if (!g_option_context_parse_strv (context, &args, &error))
      {
        g_print ("%s\n", error->message);
        return -1;
      }

    g_option_context_free (context);
    g_strfreev (args);
  }

  schema_data = test_schema_create ("orig");

  schema = hyscan_data_schema_new_from_string (schema_data, "orig");
  g_free (schema_data);

  builder = hyscan_data_schema_builder_new_with_version (SCHEMA_ID_STAGE1, SCHEMA_VERSION_STAGE1);
  hyscan_data_schema_builder_schema_join (builder, "/orig", schema, "/orig");
  hyscan_data_schema_builder_node_set_name (builder, "/orig", SCHEMA_NAME, SCHEMA_DESCRIPTION);
  schema_data = hyscan_data_schema_builder_get_data (builder);
  g_object_unref (builder);
  g_object_unref (schema);

  schema = hyscan_data_schema_new_from_string (schema_data, SCHEMA_ID_STAGE1);
  g_assert (SCHEMA_VERSION_STAGE1 == hyscan_data_schema_get_version (schema));
  g_free (schema_data);

  builder = hyscan_data_schema_builder_new_with_version (SCHEMA_ID_STAGE2, SCHEMA_VERSION_STAGE2);
  hyscan_data_schema_builder_schema_join (builder, "/", schema, "/orig");
  schema_data = hyscan_data_schema_builder_get_data (builder);
  g_object_unref (builder);
  g_object_unref (schema);

  if (schema_file != NULL)
    g_file_set_contents (schema_file, schema_data, -1, NULL);

  schema = hyscan_data_schema_new_from_string (schema_data, SCHEMA_ID_STAGE2);
  g_assert (SCHEMA_VERSION_STAGE2 == hyscan_data_schema_get_version (schema));
  g_free (schema_data);

  keys_list = hyscan_data_schema_list_keys (schema);
  nodes = hyscan_data_schema_list_nodes (schema);
  if ((keys_list == NULL) || (nodes == NULL))
    g_error ("empty schema");

  if ((g_strcmp0 (nodes->name, SCHEMA_NAME) != 0) ||
      (g_strcmp0 (nodes->description, SCHEMA_DESCRIPTION) != 0) ||
      (g_strcmp0 (hyscan_data_schema_node_get_name (schema, "/"), SCHEMA_NAME) != 0) ||
      (g_strcmp0 (hyscan_data_schema_node_get_description (schema, "/"), SCHEMA_DESCRIPTION) != 0))
    {
      g_error ("schema name/description error");
    }

  check_node (schema, nodes, silent);

  for (i = 0; keys_list[i] != NULL; i++)
    {
      HyScanDataSchemaKeyType type = hyscan_data_schema_key_get_value_type (schema, keys_list[i]);

      if (!silent)
        g_message ("check key: %s", keys_list[i]);

      check_key (schema, nodes, keys_list[i]);

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

  g_object_unref (schema);

  g_message ("All done");

  return 0;
}
