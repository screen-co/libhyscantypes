#include <hyscan-data-box.h>

static volatile guint32 mod_counter = 0;

void changed_cb (HyScanDataBox *data, const gchar *name)
{
  mod_counter += 1;
}

/* Функция проверки параметра типа BOOLEAN. */
void check_boolean (HyScanDataBox *data, const gchar *name)
{
  HyScanDataSchema *schema;
  guint32 local_mod_counter;
  gboolean default_value;
  gboolean value;

  schema = hyscan_data_box_get_schema (data);
  local_mod_counter = mod_counter;

  if (!hyscan_data_schema_key_get_default_boolean (schema, name, &default_value))
    g_error ("%s: can't get default value", name);

  if (!hyscan_data_box_set_boolean (data, name, TRUE))
    g_error ("%s: can't set value", name);

  if (!hyscan_data_box_get_boolean (data, name, &value))
    g_error ("%s: can't get value", name);

  if (!value)
    g_error ("%s: TRUE value mismatch", name);

  if (!hyscan_data_box_set_boolean (data, name, FALSE))
    g_error ("%s: can't set value", name);

  if (!hyscan_data_box_get_boolean (data, name, &value))
    g_error ("%s: can't get value", name);

  if (value)
    g_error ("%s: FALSE value mismatch", name);

  if (!hyscan_data_box_set (data, name, HYSCAN_DATA_SCHEMA_TYPE_BOOLEAN, NULL, 0))
    g_error ("%s: can't clear value", name);

  if (!hyscan_data_box_get_boolean (data, name, &value))
    g_error ("%s: can't get value", name);

  if (value != default_value)
    g_error ("%s: default value mismatch", name);

  local_mod_counter = mod_counter - local_mod_counter;
  if (local_mod_counter != hyscan_data_box_get_mod_count (data, name))
    g_error ("%s: modification counter error", name);
}

/* Функция проверки параметра типа INTEGER. */
void check_integer (HyScanDataBox *data, const gchar *name)
{
  HyScanDataSchema *schema;
  guint32 local_mod_counter;
  gint64 default_value;
  gint64 minimum_value;
  gint64 maximum_value;
  gint64 value;
  gint64 step;
  gint64 i;

  schema = hyscan_data_box_get_schema (data);
  local_mod_counter = mod_counter;

  if (!hyscan_data_schema_key_get_default_integer (schema, name, &default_value))
    g_error ("%s: can't get default value", name);

  if (!hyscan_data_schema_key_get_integer_range (schema, name, &minimum_value, &maximum_value))
    g_error ("%s: can't get values range", name);

  if (!hyscan_data_schema_key_get_integer_step (schema, name, &step))
    g_error ("%s: can't get value step", name);

  for (i = minimum_value; i <= maximum_value; i += step)
    {
      if (!hyscan_data_box_set_integer (data, name, i))
        g_error ("%s: can't set value %"G_GINT64_FORMAT, name, i);

      if (!hyscan_data_box_get_integer (data, name, &value))
        g_error ("%s: can't get value", name);

      if (value != i)
        g_error ("%s: value mismatch (%"G_GINT64_FORMAT" != %"G_GINT64_FORMAT")", name, value, i);
    }

  if (!hyscan_data_box_set (data, name, HYSCAN_DATA_SCHEMA_TYPE_INTEGER, NULL, 0))
    g_error ("%s: can't clear value", name);

  if (!hyscan_data_box_get_integer (data, name, &value))
    g_error ("%s: can't get value", name);

  if (value != default_value)
    g_error ("%s: value mismatch (%"G_GINT64_FORMAT" != %"G_GINT64_FORMAT")", name, value, default_value);

  local_mod_counter = mod_counter - local_mod_counter;
  if (local_mod_counter != hyscan_data_box_get_mod_count (data, name))
    g_error ("%s: modification counter error", name);
}

/* Функция проверки параметра типа DOUBLE. */
void check_double (HyScanDataBox *data, const gchar *name)
{
  HyScanDataSchema *schema;
  guint32 local_mod_counter;
  gdouble default_value;
  gdouble minimum_value;
  gdouble maximum_value;
  gdouble value;
  gdouble step;
  gdouble i;

  schema = hyscan_data_box_get_schema (data);
  local_mod_counter = mod_counter;

  if (!hyscan_data_schema_key_get_default_double (schema, name, &default_value))
    g_error ("%s: can't get default value", name);

  if (!hyscan_data_schema_key_get_double_range (schema, name, &minimum_value, &maximum_value))
    g_error ("%s: can't get values range", name);

  if (!hyscan_data_schema_key_get_double_step (schema, name, &step))
    g_error ("%s: can't get value step", name);

  for (i = minimum_value; i <= maximum_value; i += step)
    {
      if (!hyscan_data_box_set_double (data, name, i))
        g_error ("%s: can't set value %.3lf", name, step);

      if (!hyscan_data_box_get_double (data, name, &value))
        g_error ("%s: can't get value", name);

      if (value != i)
        g_error ("%s: value mismatch (%.3lf != %.3lf)", name, value, i);
    }

  if (!hyscan_data_box_set (data, name, HYSCAN_DATA_SCHEMA_TYPE_DOUBLE, NULL, 0))
    g_error ("%s: can't clear value", name);

  if (!hyscan_data_box_get_double (data, name, &value))
    g_error ("%s: can't get value", name);

  if (value != default_value)
    g_error ("%s: value mismatch (%.3lf != %.3lf)", name, value, default_value);

  local_mod_counter = mod_counter - local_mod_counter;
  if (local_mod_counter != hyscan_data_box_get_mod_count (data, name))
    g_error ("%s: modification counter error", name);
}

/* Функция проверки параметра типа STRING. */
void check_string (HyScanDataBox *data, const gchar *name)
{
  HyScanDataSchema *schema;
  guint32 local_mod_counter;
  const gchar *default_value;
  gchar *value1;
  gchar *value2;
  gint32 i;

  schema = hyscan_data_box_get_schema (data);
  local_mod_counter = mod_counter;

  default_value = hyscan_data_schema_key_get_default_string (schema, name);
  if (default_value == NULL)
    g_error ("%s: can't get default value", name);

  for (i = 0; i <= 1000; i++)
    {
      value1 = g_strdup_printf ("Test string %d", i);
      if (!hyscan_data_box_set_string (data, name, value1))
        g_error ("%s: can't set value %s", name, value1);

      value2 = hyscan_data_box_get_string (data, name);
      if (g_strcmp0 (value1, value2) != 0)
        g_error ("%s: value mismatch ('%s' != '%s')", name, value1, value2);

      g_free (value1);
      g_free (value2);
    }

  if (!hyscan_data_box_set (data, name, HYSCAN_DATA_SCHEMA_TYPE_STRING, NULL, 0))
    g_error ("%s: can't clear value", name);

  value2 = hyscan_data_box_get_string (data, name);
  if (g_strcmp0 (default_value, value2) != 0)
    g_error ("%s: value mismatch ('%s' != '%s')", name, value1, value2);
  g_free (value2);

  local_mod_counter = mod_counter - local_mod_counter;
  if (local_mod_counter != hyscan_data_box_get_mod_count (data, name))
    g_error ("%s: modification counter error", name);
}

/* Функция проверки параметра типа ENUM. */
void check_enum (HyScanDataBox *data, const gchar *name)
{
  HyScanDataSchema *schema;
  guint32 local_mod_counter;
  const HyScanDataSchemaEnumValue* const *values;
  gint64 default_value;
  gint64 value;
  gint i;

  schema = hyscan_data_box_get_schema (data);
  local_mod_counter = mod_counter;

  if (!hyscan_data_schema_key_get_default_enum (schema, name, &default_value))
    g_error ("%s: can't get default value", name);

  values = hyscan_data_schema_key_get_enum_values (schema, name);
  if (values == NULL)
    g_error ("%s: can't get default value", name);

  for (i = 0; values[i] != NULL; i++)
    {
      if (!hyscan_data_box_set_enum (data, name, values[i]->value))
        g_error ("%s: can't set value %"G_GINT64_FORMAT, name, values[i]->value);

      if (!hyscan_data_box_get_enum (data, name, &value))
        g_error ("%s: can't get value", name);

      if (value != values[i]->value)
        g_error ("%s: value mismatch (%"G_GINT64_FORMAT" != %"G_GINT64_FORMAT")", name, value, values[i]->value);
    }

  if (!hyscan_data_box_set (data, name, HYSCAN_DATA_SCHEMA_TYPE_ENUM, NULL, 0))
    g_error ("%s: can't clear value", name);

  if (!hyscan_data_box_get_enum (data, name, &value))
    g_error ("%s: can't get value", name);

  if (value != default_value)
    g_error ("%s: value mismatch (%"G_GINT64_FORMAT" != %"G_GINT64_FORMAT")", name, value, default_value);

  local_mod_counter = mod_counter - local_mod_counter;
  if (local_mod_counter != hyscan_data_box_get_mod_count (data, name))
    g_error ("%s: modification counter error", name);
}

int
main (int    argc,
      char **argv)
{
  HyScanDataBox *data;
  HyScanDataSchema *schema;
  const gchar * const *keys_list;
  guint i;

  if (argv[1] == NULL)
    g_error ("unknown schema file");

  data = hyscan_data_box_new_from_xml_file (argv[1], "test");
  schema = hyscan_data_box_get_schema (data);
  keys_list = hyscan_data_schema_list_keys (schema);
  if (keys_list == NULL)
    g_error ("empty schema");

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

  if (mod_counter != hyscan_data_box_get_mod_count (data, NULL))
    g_error ("modification counter error");

  g_object_unref (data);

  return 0;
}
