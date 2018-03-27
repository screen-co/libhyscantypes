/* data-box-test.c
 *
 * Copyright 2017 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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
 * Contact the Screen LLC in this case - info@screen-co.ru
 */

/* HyScanTypes имеет двойную лицензию.
 *
 * Во-первых, вы можете распространять HyScanTypes на условиях Стандартной
 * Общественной Лицензии GNU версии 3, либо по любой более поздней версии
 * лицензии (по вашему выбору). Полные положения лицензии GNU приведены в
 * <http://www.gnu.org/licenses/>.
 *
 * Во-вторых, этот программный код можно использовать по коммерческой
 * лицензии. Для этого свяжитесь с ООО Экран - info@screen-co.ru.
 */

#include <gio/gio.h>
#include <hyscan-data-box.h>
#include <libxml/parser.h>

gchar *test_schema_create (const gchar *schema_id);

HyScanParamList *list;
const gchar *cur_name;
GVariant *cur_value;

static volatile guint32 mod_counter = 0;

gboolean
set_cb (HyScanDataBox   *data,
        HyScanParamList *list)
{
  const gchar * const *names;
  GVariant *value;

  if (cur_value == NULL)
    return TRUE;

  names = hyscan_param_list_params (list);
  if (names == NULL)
    {
      g_warning ("set_cb: empty list");
      return FALSE;
    }

  if (g_strcmp0 (cur_name, names[0]) != 0)
    {
      g_warning ("set_cb: incorrect name '%s', should be '%s'", names[0], cur_name);
      return FALSE;
    }

  if (names[1] != NULL)
    {
      g_warning ("set_cb: wrong number of parameters");
      return FALSE;
    }

  value = hyscan_param_list_get (list, names[0]);
  if (value == NULL)
    return TRUE;

  if (g_variant_compare (cur_value, value) != 0)
    {
      g_warning ("set_cb: value mismatch");
      return FALSE;
    }

  g_variant_unref (value);

  return TRUE;
}

void
changed_cb (HyScanDataBox *data,
            const gchar   *name)
{
  mod_counter += 1;
}

/* Функция проверки параметра типа BOOLEAN. */
void
check_boolean (HyScanDataBox    *data,
               HyScanDataSchema *schema,
               const gchar      *name)
{
  HyScanParam *param;
  HyScanDataSchemaKeyAccess access;
  guint32 local_mod_counter;
  GVariant *default_value;

  cur_name = name;
  param = HYSCAN_PARAM (data);
  local_mod_counter = mod_counter;

  hyscan_param_list_clear (list);
  hyscan_param_list_add (list, name);

  access = hyscan_data_schema_key_get_access (schema, name);
  if (access == HYSCAN_DATA_SCHEMA_ACCESS_READONLY)
    {
      cur_value = NULL;

      if (hyscan_param_set (param, list))
        g_error ("%s: can set readonly value", name);

      return;
    }
  if (access == HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY)
    {
      if (hyscan_param_get (param, list))
        g_error ("%s: can get writeonly value", name);

      return;
    }

  default_value = hyscan_data_schema_key_get_default (schema, name);
  if (default_value == NULL)
    g_error ("%s: can't get default value", name);

  cur_value = g_variant_new_boolean (TRUE);

  hyscan_param_list_set_boolean (list, name, TRUE);
  if (!hyscan_param_set (param, list))
    g_error ("%s: can't set value", name);

  g_variant_unref (cur_value);

  if (!hyscan_param_get (param, list))
    g_error ("%s: can't get value", name);

  if (!hyscan_param_list_get_boolean (list, name))
    g_error ("%s: TRUE value mismatch", name);

  cur_value = g_variant_new_boolean (FALSE);

  hyscan_param_list_set_boolean (list, name, FALSE);
  if (!hyscan_param_set (param, list))
    g_error ("%s: can't set value", name);

  g_variant_unref (cur_value);

  if (!hyscan_param_get (param, list))
    g_error ("%s: can't get value", name);

  if (hyscan_param_list_get_boolean (list, name))
    g_error ("%s: FALSE value mismatch", name);

  cur_value = NULL;

  hyscan_param_list_set (list, name, NULL);
  if (!hyscan_param_set (param, list))
    g_error ("%s: can't clear value", name);

  if (!hyscan_param_get (param, list))
    g_error ("%s: can't get value", name);

  if (hyscan_param_list_get_boolean (list, name) != g_variant_get_boolean (default_value))
    g_error ("%s: default value mismatch", name);

  local_mod_counter = mod_counter - local_mod_counter;
  if (local_mod_counter != hyscan_data_box_get_mod_count (data, name))
    g_error ("%s: modification counter error", name);

  g_variant_unref (default_value);

  hyscan_param_list_set_boolean (list, name, g_random_int_range (0, 2));
  if (!hyscan_param_set (param, list))
      g_error ("%s: can't set value", name);

  hyscan_param_list_set (list, name, g_variant_new_byte (0));
  if (hyscan_param_set (param, list))
      g_error ("%s: can set incorrect value", name);
}

/* Функция проверки параметра типа INTEGER. */
void
check_integer (HyScanDataBox    *data,
               HyScanDataSchema *schema,
               const gchar      *name)
{
  HyScanParam *param;
  HyScanDataSchemaKeyAccess access;
  guint32 local_mod_counter;
  GVariant *default_value;
  GVariant *minimum_value;
  GVariant *maximum_value;
  GVariant *value_step;
  gint64 min, max, step;
  gint64 value;
  gint64 i;

  cur_name = name;
  param = HYSCAN_PARAM (data);
  local_mod_counter = mod_counter;

  hyscan_param_list_clear (list);
  hyscan_param_list_add (list, name);

  access = hyscan_data_schema_key_get_access (schema, name);
  if (access == HYSCAN_DATA_SCHEMA_ACCESS_READONLY)
    {
      cur_value = NULL;

      if (hyscan_param_set (param, list))
        g_error ("%s: can set read only value", name);

      return;
    }
  if (access == HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY)
    {
      if (hyscan_param_get (param, list))
        g_error ("%s: can get writeonly value", name);

      return;
    }

  default_value = hyscan_data_schema_key_get_default (schema, name);
  if (default_value == NULL)
    g_error ("%s: can't get default value", name);

  minimum_value = hyscan_data_schema_key_get_minimum (schema, name);
  if (minimum_value == NULL)
    g_error ("%s: can't get minimum value", name);

  maximum_value = hyscan_data_schema_key_get_maximum (schema, name);
  if (maximum_value == NULL)
    g_error ("%s: can't get maximum value", name);

  value_step = hyscan_data_schema_key_get_step (schema, name);
  if (value_step == NULL)
    g_error ("%s: can't get value step", name);

  min = g_variant_get_int64 (minimum_value);
  max = g_variant_get_int64 (maximum_value);
  step = g_variant_get_int64 (value_step);
  for (i = min; i <= max; i += step)
    {
      cur_value = g_variant_new_int64 (i);

      hyscan_param_list_set_integer (list, name, i);
      if (!hyscan_param_set (param, list))
        g_error ("%s: can't set value %"G_GINT64_FORMAT, name, i);

      g_variant_unref (cur_value);

      if (!hyscan_param_get (param, list))
        g_error ("%s: can't get value", name);

      value = hyscan_param_list_get_integer (list, name);
      if (value != i)
        g_error ("%s: value mismatch (%"G_GINT64_FORMAT" != %"G_GINT64_FORMAT")", name, value, i);
    }

  cur_value = NULL;

  hyscan_param_list_set (list, name, NULL);
  if (!hyscan_param_set (param, list))
    g_error ("%s: can't clear value", name);

  if (!hyscan_param_get (param, list))
    g_error ("%s: can't get value", name);

  value = hyscan_param_list_get_integer (list, name);
  if (value != g_variant_get_int64 (default_value))
    g_error ("%s: value mismatch (%"G_GINT64_FORMAT" != %"G_GINT64_FORMAT")", name, value, g_variant_get_int64 (default_value));

  local_mod_counter = mod_counter - local_mod_counter;
  if (local_mod_counter != hyscan_data_box_get_mod_count (data, name))
    g_error ("%s: modification counter error", name);

  g_variant_unref (default_value);
  g_variant_unref (minimum_value);
  g_variant_unref (maximum_value);
  g_variant_unref (value_step);

  hyscan_param_list_set_integer (list, name, g_random_int_range (min, max + 1));
  if (!hyscan_param_set (param, list))
      g_error ("%s: can't set value", name);

  hyscan_param_list_set (list, name, g_variant_new_byte (0));
  if (hyscan_param_set (param, list))
      g_error ("%s: can set incorrect value", name);
}

/* Функция проверки параметра типа DOUBLE. */
void
check_double (HyScanDataBox    *data,
              HyScanDataSchema *schema,
              const gchar      *name)
{
  HyScanParam *param;
  HyScanDataSchemaKeyAccess access;
  guint32 local_mod_counter;
  GVariant *default_value;
  GVariant *minimum_value;
  GVariant *maximum_value;
  GVariant *value_step;
  gdouble min, max, step;
  gdouble value;
  gdouble i;

  cur_name = name;
  param = HYSCAN_PARAM (data);
  local_mod_counter = mod_counter;

  hyscan_param_list_clear (list);
  hyscan_param_list_add (list, name);

  access = hyscan_data_schema_key_get_access (schema, name);
  if (access == HYSCAN_DATA_SCHEMA_ACCESS_READONLY)
    {
      cur_value = NULL;

      if (hyscan_param_set (param, list))
        g_error ("%s: can set read only value", name);

      return;
    }
  if (access == HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY)
    {
      if (hyscan_param_get (param, list))
        g_error ("%s: can get writeonly value", name);

      return;
    }

  default_value = hyscan_data_schema_key_get_default (schema, name);
  if (default_value == NULL)
    g_error ("%s: can't get default value", name);

  minimum_value = hyscan_data_schema_key_get_minimum (schema, name);
  if (minimum_value == NULL)
    g_error ("%s: can't get minimum value", name);

  maximum_value = hyscan_data_schema_key_get_maximum (schema, name);
  if (maximum_value == NULL)
    g_error ("%s: can't get maximum value", name);

  value_step = hyscan_data_schema_key_get_step (schema, name);
  if (value_step == NULL)
    g_error ("%s: can't get value step", name);

  min = g_variant_get_double (minimum_value);
  max = g_variant_get_double (maximum_value);
  step = g_variant_get_double (value_step);
  for (i = min; i <= max; i += step)
    {
      cur_value = g_variant_new_double (i);

      hyscan_param_list_set_double (list, name, i);
      if (!hyscan_param_set (param, list))
        g_error ("%s: can't set value %.3lf", name, step);

      g_variant_unref (cur_value);

      if (!hyscan_param_get (param, list))
        g_error ("%s: can't get value", name);

      value = hyscan_param_list_get_double (list, name);
      if (value != i)
        g_error ("%s: value mismatch (%.3lf != %.3lf)", name, value, i);
    }

  cur_value = NULL;

  hyscan_param_list_set (list, name, NULL);
  if (!hyscan_param_set (param, list))
    g_error ("%s: can't clear value", name);

  if (!hyscan_param_get (param, list))
    g_error ("%s: can't get value", name);

  value = hyscan_param_list_get_double (list, name);
  if (value != g_variant_get_double (default_value))
    g_error ("%s: value mismatch (%.3lf != %.3lf)", name, value, g_variant_get_double (default_value));

  local_mod_counter = mod_counter - local_mod_counter;
  if (local_mod_counter != hyscan_data_box_get_mod_count (data, name))
    g_error ("%s: modification counter error", name);

  g_variant_unref (default_value);
  g_variant_unref (minimum_value);
  g_variant_unref (maximum_value);
  g_variant_unref (value_step);

  hyscan_param_list_set_double (list, name, g_random_double_range (min, max));
  if (!hyscan_param_set (param, list))
      g_error ("%s: can't set value", name);

  hyscan_param_list_set (list, name, g_variant_new_byte (0));
  if (hyscan_param_set (param, list))
      g_error ("%s: can set incorrect value", name);
}

/* Функция проверки параметра типа STRING. */
void
check_string (HyScanDataBox    *data,
              HyScanDataSchema *schema,
              const gchar      *name)
{
  HyScanParam *param;
  HyScanDataSchemaKeyAccess access;
  guint32 local_mod_counter;
  GVariant *default_value;
  gchar *value1;
  const gchar *value2;
  gint32 i;

  cur_name = name;
  param = HYSCAN_PARAM (data);
  local_mod_counter = mod_counter;

  hyscan_param_list_clear (list);
  hyscan_param_list_add (list, name);

  access = hyscan_data_schema_key_get_access (schema, name);
  if (access == HYSCAN_DATA_SCHEMA_ACCESS_READONLY)
    {
      cur_value = NULL;

      if (hyscan_param_set (param, list))
        g_error ("%s: can set read only value", name);

      return;
    }
  if (access == HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY)
    {
      if (hyscan_param_get (param, list))
        g_error ("%s: can get writeonly value", name);

      return;
    }

  default_value = hyscan_data_schema_key_get_default (schema, name);
  if (g_pattern_match_simple ("*null*", name))
    {
      if (default_value != NULL)
        g_error ("%s: can't get default value", name);
    }
  else
    {
      if (default_value == NULL)
        g_error ("%s: default value not NULL", name);
    }

  for (i = 0; i <= 1000; i++)
    {
      value1 = g_strdup_printf ("Test string %d", i);

      cur_value = g_variant_new_string (value1);

      hyscan_param_list_set_string (list, name, value1);
      if (!hyscan_param_set (param, list))
        g_error ("%s: can't set value %s", name, value1);

      g_variant_unref (cur_value);

      hyscan_param_get (param, list);
      value2 = hyscan_param_list_get_string (list, name);
      if (g_strcmp0 (value1, value2) != 0)
        g_error ("%s: value mismatch ('%s' != '%s')", name, value1, value2);

      g_free (value1);
    }

  cur_value = NULL;

  hyscan_param_list_set (list, name, NULL);
  if (!hyscan_param_set (param, list))
    g_error ("%s: can't clear value", name);

  if (!g_pattern_match_simple ("*null*", name))
    {
      hyscan_param_get (param, list);
      value2 = hyscan_param_list_get_string (list, name);
      if (g_strcmp0 (g_variant_get_string (default_value, NULL), value2) != 0)
        g_error ("%s: value mismatch ('%s' != '%s')", name, value1, value2);
    }

  local_mod_counter = mod_counter - local_mod_counter;
  if (local_mod_counter != hyscan_data_box_get_mod_count (data, name))
    g_error ("%s: modification counter error", name);

  g_clear_pointer (&default_value, g_variant_unref);

  value1 = g_strdup_printf ("string %d", g_random_int ());
  hyscan_param_list_set_string (list, name, value1);
  if (!hyscan_param_set (param, list))
      g_error ("%s: can't set value", name);
  g_free (value1);

  hyscan_param_list_set (list, name, g_variant_new_byte (0));
  if (hyscan_param_set (param, list))
      g_error ("%s: can set incorrect value", name);
}

/* Функция проверки параметра типа ENUM. */
void
check_enum (HyScanDataBox    *data,
            HyScanDataSchema *schema,
            const gchar      *name)
{
  HyScanParam *param;
  HyScanDataSchemaKeyAccess access;
  guint32 local_mod_counter;
  const gchar *enum_id;
  GList *values, *values_list;
  GVariant *default_value;
  gint64 value;

  cur_name = name;
  param = HYSCAN_PARAM (data);
  local_mod_counter = mod_counter;

  hyscan_param_list_clear (list);
  hyscan_param_list_add (list, name);

  access = hyscan_data_schema_key_get_access (schema, name);
  if (access == HYSCAN_DATA_SCHEMA_ACCESS_READONLY)
    {
      cur_value = NULL;

      if (hyscan_param_set (param, list))
        g_error ("%s: can set read only value", name);

      return;
    }
  if (access == HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY)
    {
      if (hyscan_param_get (param, list))
        g_error ("%s: can get writeonly value", name);

      return;
    }

  default_value = hyscan_data_schema_key_get_default (schema, name);
  if (default_value == NULL)
    g_error ("%s: can't get default value", name);

  enum_id = hyscan_data_schema_key_get_enum_id (schema, name);
  values_list = values = hyscan_data_schema_get_enum_values (schema, enum_id);
  if (values == NULL)
    g_error ("%s: can't get enum values", name);

  while (values != NULL)
    {
      HyScanDataSchemaEnumValue *enum_value = values->data;

      cur_value = g_variant_new_int64 (enum_value->value);

      hyscan_param_list_set_enum (list, name, enum_value->value);
      if (!hyscan_param_set (param, list))
        g_error ("%s: can't set value %"G_GINT64_FORMAT, name, enum_value->value);

      g_variant_unref (cur_value);

      if (!hyscan_param_get (param, list))
        g_error ("%s: can't get value", name);

      value = hyscan_param_list_get_enum (list, name);
      if (value != enum_value->value)
        g_error ("%s: value mismatch (%"G_GINT64_FORMAT" != %"G_GINT64_FORMAT")", name, value, enum_value->value);

      values = values->next;
    }

  g_list_free (values_list);

  cur_value = NULL;

  hyscan_param_list_set (list, name, NULL);
  if (!hyscan_param_set (param, list))
    g_error ("%s: can't clear value", name);

  if (!hyscan_param_get (param, list))
    g_error ("%s: can't get value", name);

  value = hyscan_param_list_get_enum (list, name);
  if (value != g_variant_get_int64 (default_value))
    g_error ("%s: value mismatch (%"G_GINT64_FORMAT" != %"G_GINT64_FORMAT")", name, value, g_variant_get_int64 (default_value));

  local_mod_counter = mod_counter - local_mod_counter;
  if (local_mod_counter != hyscan_data_box_get_mod_count (data, name))
    g_error ("%s: modification counter error", name);

  g_variant_unref (default_value);

  hyscan_param_list_set (list, name, g_variant_new_byte (0));
  if (hyscan_param_set (param, list))
      g_error ("%s: can set incorrect value", name);
}

/* Функция сравнивает значения параметров двух объектов HyScanDataBox. */
void
compare_values (HyScanDataBox *data,
                HyScanDataBox *data2)
{
  HyScanDataSchema *schema;
  HyScanParamList *list1;
  HyScanParamList *list2;
  gchar **keys_list;
  gsize i;

  schema = hyscan_param_schema (HYSCAN_PARAM (data));
  keys_list = hyscan_data_schema_list_keys (schema);
  if (keys_list == NULL)
    g_error ("compare: empty schema");

  list1 = hyscan_param_list_new ();
  list2 = hyscan_param_list_new ();

  for (i = 0; keys_list[i] != NULL; i++)
    {
      HyScanDataSchemaKeyAccess access;
      GVariant *value1;
      GVariant *value2;

      access = hyscan_data_schema_key_get_access (schema, keys_list[i]);
      if (access == HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY)
        continue;

      hyscan_param_list_clear (list1);
      hyscan_param_list_clear (list2);

      hyscan_param_list_add (list1, keys_list[i]);
      hyscan_param_list_add (list2, keys_list[i]);

      if (!hyscan_param_get (HYSCAN_PARAM (data), list1))
        g_error ("compare: can't get values");

      if (!hyscan_param_get (HYSCAN_PARAM (data2), list2))
        g_error ("compare: can't get values2");

      value1 = hyscan_param_list_get (list1, keys_list[i]);
      value2 = hyscan_param_list_get (list2, keys_list[i]);

      if ((value1 == NULL) && (value2 == NULL))
        continue;

      if (g_variant_compare (value1, value2) != 0)
        g_error ("compare: %s mismatch", keys_list[i]);

      g_variant_unref (value1);
      g_variant_unref (value2);
    }

  g_strfreev (keys_list);
  g_object_unref (schema);
  g_object_unref (list1);
  g_object_unref (list2);
}

int
main (int    argc,
      char **argv)
{
  gchar *schema_file = NULL;
  gboolean silent = FALSE;

  HyScanDataBox *data;
  HyScanDataBox *data2;
  HyScanDataSchema *schema;
  gchar *schema_data;
  gchar **keys_list;
  gchar *sparams;
  gsize i;

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

  list = hyscan_param_list_new ();
  schema_data = test_schema_create ("test");

  if (schema_file != NULL)
    g_file_set_contents (schema_file, schema_data, -1, NULL);

  schema = hyscan_data_schema_new_from_string (schema_data, "test");
  data = hyscan_data_box_new_from_schema (schema);

  keys_list = hyscan_data_schema_list_keys (schema);
  if (keys_list == NULL)
    g_error ("empty schema");

  g_signal_connect (data, "set", G_CALLBACK (set_cb), NULL);
  g_signal_connect (data, "changed", G_CALLBACK (changed_cb), NULL);

  for (i = 0; keys_list[i] != NULL; i++)
    {
      HyScanDataSchemaKeyType type = hyscan_data_schema_key_get_value_type (schema, keys_list[i]);

      if (!silent)
        g_message ("check key: %s", keys_list[i]);

      switch (type)
        {
        case HYSCAN_DATA_SCHEMA_KEY_BOOLEAN:
          check_boolean (data, schema, keys_list[i]);
          break;

        case HYSCAN_DATA_SCHEMA_KEY_INTEGER:
          check_integer (data, schema, keys_list[i]);
          break;

        case HYSCAN_DATA_SCHEMA_KEY_DOUBLE:
          check_double (data, schema, keys_list[i]);
          break;

        case HYSCAN_DATA_SCHEMA_KEY_STRING:
          check_string (data, schema, keys_list[i]);
          break;

        case HYSCAN_DATA_SCHEMA_KEY_ENUM:
          check_enum (data, schema, keys_list[i]);
          break;

        default:
          break;
        }
    }

  g_strfreev (keys_list);

  if (mod_counter != hyscan_data_box_get_mod_count (data, NULL))
    g_error ("modification counter error");

  sparams = hyscan_data_box_serialize (data);
  if (sparams == NULL)
    g_error ("can't serialize values");

  data2 = hyscan_data_box_new_from_string (schema_data, "test");
  if (!hyscan_data_box_deserialize (data2, sparams))
    g_error ("can't deserialize values");

  compare_values (data, data2);

  g_object_unref (schema);
  g_object_unref (data);
  g_object_unref (data2);
  g_object_unref (list);
  g_free (schema_data);
  g_free (sparams);

  xmlCleanupParser ();

  g_message ("All done");

  return 0;
}
