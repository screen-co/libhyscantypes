#include <gio/gio.h>
#include <hyscan-data-box.h>

gchar *test_schema_create (const gchar *schema_id);

const gchar *cur_name;
GVariant *cur_value;

static volatile guint32 mod_counter = 0;

gboolean
set_cb (HyScanDataBox       *data,
        const gchar *const  *names,
        GVariant           **values)
{
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

  if (cur_value == NULL && values[0] == NULL)
    return TRUE;

  if (g_variant_compare (cur_value, values[0]) != 0)
    {
      g_warning ("set_cb: value mismatch");
      return FALSE;
    }

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
check_boolean (HyScanDataBox *data,
               const gchar   *name)
{
  HyScanDataSchema *schema;
  guint32 local_mod_counter;
  GVariant *default_value;
  gboolean value;

  cur_name = name;
  schema = HYSCAN_DATA_SCHEMA (data);
  local_mod_counter = mod_counter;

  if (hyscan_data_schema_key_is_readonly (schema, name))
    {
      cur_value = NULL;

      if (hyscan_data_box_set_default (data, name))
        g_error ("%s: can set read only value", name);

      return;
    }

  default_value = hyscan_data_schema_key_get_default (schema, name);
  if (default_value == NULL)
    g_error ("%s: can't get default value", name);

  cur_value = g_variant_new_boolean (TRUE);

  if (!hyscan_data_box_set_boolean (data, name, TRUE))
    g_error ("%s: can't set value", name);

  g_variant_unref (cur_value);

  if (!hyscan_data_box_get_boolean (data, name, &value))
    g_error ("%s: can't get value", name);

  if (!value)
    g_error ("%s: TRUE value mismatch", name);

  cur_value = g_variant_new_boolean (FALSE);

  if (!hyscan_data_box_set_boolean (data, name, FALSE))
    g_error ("%s: can't set value", name);

  g_variant_unref (cur_value);

  if (!hyscan_data_box_get_boolean (data, name, &value))
    g_error ("%s: can't get value", name);

  if (value)
    g_error ("%s: FALSE value mismatch", name);

  cur_value = NULL;

  if (!hyscan_data_box_set_default (data, name))
    g_error ("%s: can't clear value", name);

  if (!hyscan_data_box_get_boolean (data, name, &value))
    g_error ("%s: can't get value", name);

  if (value != g_variant_get_boolean (default_value))
    g_error ("%s: default value mismatch", name);

  local_mod_counter = mod_counter - local_mod_counter;
  if (local_mod_counter != hyscan_data_box_get_mod_count (data, name))
    g_error ("%s: modification counter error", name);

  g_variant_unref (default_value);
}

/* Функция проверки параметра типа INTEGER. */
void
check_integer (HyScanDataBox *data,
               const gchar   *name)
{
  HyScanDataSchema *schema;
  guint32 local_mod_counter;
  GVariant *default_value;
  GVariant *minimum_value;
  GVariant *maximum_value;
  GVariant *value_step;
  gint64 min, max, step;
  gint64 value;
  gint64 i;

  cur_name = name;
  schema = HYSCAN_DATA_SCHEMA (data);
  local_mod_counter = mod_counter;

  if (hyscan_data_schema_key_is_readonly (schema, name))
    {
      cur_value = NULL;

      if (hyscan_data_box_set_default (data, name))
        g_error ("%s: can set read only value", name);

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

      if (!hyscan_data_box_set_integer (data, name, i))
        g_error ("%s: can't set value %"G_GINT64_FORMAT, name, i);

      g_variant_unref (cur_value);

      if (!hyscan_data_box_get_integer (data, name, &value))
        g_error ("%s: can't get value", name);

      if (value != i)
        g_error ("%s: value mismatch (%"G_GINT64_FORMAT" != %"G_GINT64_FORMAT")", name, value, i);
    }

  cur_value = NULL;

  if (!hyscan_data_box_set_default (data, name))
    g_error ("%s: can't clear value", name);

  if (!hyscan_data_box_get_integer (data, name, &value))
    g_error ("%s: can't get value", name);

  if (value != g_variant_get_int64 (default_value))
    g_error ("%s: value mismatch (%"G_GINT64_FORMAT" != %"G_GINT64_FORMAT")", name, value, g_variant_get_int64 (default_value));

  local_mod_counter = mod_counter - local_mod_counter;
  if (local_mod_counter != hyscan_data_box_get_mod_count (data, name))
    g_error ("%s: modification counter error", name);

  g_variant_unref (default_value);
  g_variant_unref (minimum_value);
  g_variant_unref (maximum_value);
  g_variant_unref (value_step);
}

/* Функция проверки параметра типа DOUBLE. */
void
check_double (HyScanDataBox *data,
              const gchar   *name)
{
  HyScanDataSchema *schema;
  guint32 local_mod_counter;
  GVariant *default_value;
  GVariant *minimum_value;
  GVariant *maximum_value;
  GVariant *value_step;
  gdouble min, max, step;
  gdouble value;
  gdouble i;

  cur_name = name;
  schema = HYSCAN_DATA_SCHEMA (data);
  local_mod_counter = mod_counter;

  if (hyscan_data_schema_key_is_readonly (schema, name))
    {
      cur_value = NULL;

      if (hyscan_data_box_set_default (data, name))
        g_error ("%s: can set read only value", name);

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

      if (!hyscan_data_box_set_double (data, name, i))
        g_error ("%s: can't set value %.3lf", name, step);

      g_variant_unref (cur_value);

      if (!hyscan_data_box_get_double (data, name, &value))
        g_error ("%s: can't get value", name);

      if (value != i)
        g_error ("%s: value mismatch (%.3lf != %.3lf)", name, value, i);
    }

  cur_value = NULL;

  if (!hyscan_data_box_set_default (data, name))
    g_error ("%s: can't clear value", name);

  if (!hyscan_data_box_get_double (data, name, &value))
    g_error ("%s: can't get value", name);

  if (value != g_variant_get_double (default_value))
    g_error ("%s: value mismatch (%.3lf != %.3lf)", name, value, g_variant_get_double (default_value));

  local_mod_counter = mod_counter - local_mod_counter;
  if (local_mod_counter != hyscan_data_box_get_mod_count (data, name))
    g_error ("%s: modification counter error", name);

  g_variant_unref (default_value);
  g_variant_unref (minimum_value);
  g_variant_unref (maximum_value);
  g_variant_unref (value_step);
}

/* Функция проверки параметра типа STRING. */
void
check_string (HyScanDataBox *data,
              const gchar   *name)
{
  HyScanDataSchema *schema;
  guint32 local_mod_counter;
  GVariant *default_value;
  gchar *value1;
  gchar *value2;
  gint32 i;

  cur_name = name;
  schema = HYSCAN_DATA_SCHEMA (data);
  local_mod_counter = mod_counter;

  if (hyscan_data_schema_key_is_readonly (schema, name))
    {
      cur_value = NULL;

      if (hyscan_data_box_set_default (data, name))
        g_error ("%s: can set read only value", name);

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

      if (!hyscan_data_box_set_string (data, name, value1))
        g_error ("%s: can't set value %s", name, value1);

      g_variant_unref (cur_value);

      value2 = hyscan_data_box_get_string (data, name);
      if (g_strcmp0 (value1, value2) != 0)
        g_error ("%s: value mismatch ('%s' != '%s')", name, value1, value2);

      g_free (value1);
      g_free (value2);
    }

  cur_value = NULL;

  if (!hyscan_data_box_set_default (data, name))
    g_error ("%s: can't clear value", name);

  if (!g_pattern_match_simple ("*null*", name))
    {
      value2 = hyscan_data_box_get_string (data, name);
      if (g_strcmp0 (g_variant_get_string (default_value, NULL), value2) != 0)
        g_error ("%s: value mismatch ('%s' != '%s')", name, value1, value2);
      g_free (value2);
    }

  local_mod_counter = mod_counter - local_mod_counter;
  if (local_mod_counter != hyscan_data_box_get_mod_count (data, name))
    g_error ("%s: modification counter error", name);

  g_clear_pointer (&default_value, g_variant_unref);
}

/* Функция проверки параметра типа ENUM. */
void
check_enum (HyScanDataBox *data,
            const gchar   *name)
{
  HyScanDataSchema *schema;
  guint32 local_mod_counter;
  HyScanDataSchemaEnumValue **values;
  GVariant *default_value;
  gint64 value;
  gint i;

  cur_name = name;
  schema = HYSCAN_DATA_SCHEMA (data);
  local_mod_counter = mod_counter;

  if (hyscan_data_schema_key_is_readonly (schema, name))
    {
      cur_value = NULL;

      if (hyscan_data_box_set_default (data, name))
        g_error ("%s: can set read only value", name);

      return;
    }

  default_value = hyscan_data_schema_key_get_default (schema, name);
  if (default_value == NULL)
    g_error ("%s: can't get default value", name);

  values = hyscan_data_schema_key_get_enum_values (schema, name);
  if (values == NULL)
    g_error ("%s: can't get default value", name);

  for (i = 0; values[i] != NULL; i++)
    {
      cur_value = g_variant_new_int64 (values[i]->value);

      if (!hyscan_data_box_set_enum (data, name, values[i]->value))
        g_error ("%s: can't set value %"G_GINT64_FORMAT, name, values[i]->value);

      g_variant_unref (cur_value);

      if (!hyscan_data_box_get_enum (data, name, &value))
        g_error ("%s: can't get value", name);

      if (value != values[i]->value)
        g_error ("%s: value mismatch (%"G_GINT64_FORMAT" != %"G_GINT64_FORMAT")", name, value, values[i]->value);
    }

  cur_value = NULL;

  if (!hyscan_data_box_set_default (data, name))
    g_error ("%s: can't clear value", name);

  if (!hyscan_data_box_get_enum (data, name, &value))
    g_error ("%s: can't get value", name);

  if (value != g_variant_get_int64 (default_value))
    g_error ("%s: value mismatch (%"G_GINT64_FORMAT" != %"G_GINT64_FORMAT")", name, value, g_variant_get_int64 (default_value));

  local_mod_counter = mod_counter - local_mod_counter;
  if (local_mod_counter != hyscan_data_box_get_mod_count (data, name))
    g_error ("%s: modification counter error", name);

  hyscan_data_schema_free_enum_values (values);

  g_variant_unref (default_value);
}

int
main (int    argc,
      char **argv)
{
  HyScanDataBox *data;
  HyScanDataSchema *schema;
  gchar *schema_data;
  gchar **keys_list;
  guint i;

  schema_data = test_schema_create ("test");
  data = hyscan_data_box_new_from_string (schema_data, "test");
  schema = HYSCAN_DATA_SCHEMA (data);

  keys_list = hyscan_data_schema_list_keys (schema);
  if (keys_list == NULL)
    g_error ("empty schema");

  g_signal_connect (data, "set", G_CALLBACK (set_cb), NULL);
  g_signal_connect (data, "changed", G_CALLBACK (changed_cb), NULL);

  for (i = 0; keys_list[i] != NULL; i++)
    {
      HyScanDataSchemaType type = hyscan_data_schema_key_get_type (schema, keys_list[i]);

      switch (type)
        {
        case HYSCAN_DATA_SCHEMA_TYPE_BOOLEAN:
          check_boolean (data, keys_list[i]);
          break;

        case HYSCAN_DATA_SCHEMA_TYPE_INTEGER:
          check_integer (data, keys_list[i]);
          break;

        case HYSCAN_DATA_SCHEMA_TYPE_DOUBLE:
          check_double (data, keys_list[i]);
          break;

        case HYSCAN_DATA_SCHEMA_TYPE_STRING:
          check_string (data, keys_list[i]);
          break;

        case HYSCAN_DATA_SCHEMA_TYPE_ENUM:
          check_enum (data, keys_list[i]);
          break;

        default:
          break;
        }
    }

  g_strfreev (keys_list);

  if (mod_counter != hyscan_data_box_get_mod_count (data, NULL))
    g_error ("modification counter error");

  g_object_unref (data);
  g_free (schema_data);

  return 0;
}
