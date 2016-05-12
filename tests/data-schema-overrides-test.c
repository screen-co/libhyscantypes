#include <hyscan-data-schema-overrides.h>

typedef struct
{
  const gchar         *key_id;
  gboolean             default_value;
  gboolean             readonly;
} BooleanOverrides;

typedef struct
{
  const gchar         *key_id;
  gint64               default_value;
  gint64               minimum_value;
  gint64               maximum_value;
  gint64               value_step;
  gboolean             readonly;
} IntegerOverrides;

typedef struct
{
  const gchar         *key_id;
  gdouble              default_value;
  gdouble              minimum_value;
  gdouble              maximum_value;
  gdouble              value_step;
  gboolean             readonly;
} DoubleOverrides;

typedef struct
{
  const gchar         *key_id;
  const gchar         *default_value;
  gboolean             readonly;
} StringOverrides;

typedef struct
{
  const gchar         *key_id;
  gint64               default_value;
  gboolean             readonly;
} EnumOverrides;


static BooleanOverrides BooleanOverridesList[] =
{
  { "/boolean/key1", TRUE, TRUE },
  { "/boolean/key2", TRUE, FALSE },
  { "/boolean/key3", FALSE, TRUE },
  { "/boolean/key4", FALSE, FALSE },
  { NULL, FALSE, FALSE }
};

static IntegerOverrides IntegerOverridesList[] =
{
  { "/integer/key1", 1, -100, 100, 1, TRUE },
  { "/integer/key2", 2, -200, 200, 2, FALSE },
  { "/integer/key3", 3, -300, 300, 3, TRUE },
  { "/integer/key4", 4, -400, 400, 4, FALSE },
  { "/integer/key5", 5, -500, 500, 5, TRUE },
  { NULL, 0, 0, 0, 0, FALSE }
};

static DoubleOverrides DoubleOverridesList[] =
{
  { "/double/key1", 1.0, -100.0, 100.0, 0.1, FALSE },
  { "/double/key2", 2.0, -200.0, 200.0, 0.2, TRUE },
  { "/double/key3", 3.0, -300.0, 300.0, 0.3, FALSE },
  { "/double/key4", 4.0, -400.0, 400.0, 0.4, TRUE },
  { "/double/key5", 5.0, -500.0, 500.0, 0.5, FALSE },
  { NULL, 0.0, 0.0, 0.0, 0.0, FALSE }
};

static StringOverrides StringOverridesList[] =
{
  { "/string/key1", "value1", FALSE },
  { "/string/key2", "value2", TRUE },
  { "/string/key3", "value3", FALSE },
  { "/string/key4", "value4", TRUE },
  { "/string/key5", "value5", FALSE },
  { NULL, NULL, FALSE }
};

static EnumOverrides EnumOverridesList[] =
{
  { "/enum/key1", 1, FALSE },
  { "/enum/key2", 2, TRUE },
  { "/enum/key3", 3, FALSE },
  { "/enum/key4", 4, TRUE },
  { "/enum/key5", 5, FALSE },
  { NULL, 0, FALSE }
};

HyScanDataSchemaEnumValue EnumValueOverrides1 = { 1, "VALUE1", "Value 1" };
HyScanDataSchemaEnumValue EnumValueOverrides2 = { 2, "VALUE2", "Value 2" };
HyScanDataSchemaEnumValue EnumValueOverrides3 = { 3, "VALUE3", "Value 3" };
HyScanDataSchemaEnumValue EnumValueOverrides4 = { 4, "VALUE4", "Value 4" };
HyScanDataSchemaEnumValue EnumValueOverrides5 = { 5, "VALUE5", "Value 5" };

static const HyScanDataSchemaEnumValue * const EnumValueOverridesList[] =
{
  &EnumValueOverrides1,
  &EnumValueOverrides2,
  &EnumValueOverrides3,
  &EnumValueOverrides4,
  &EnumValueOverrides5,
  NULL
};

int
main (int    argc,
      char **argv)
{
  HyScanDataSchemaOverrides *overrides;
  HyScanDataSchemaEnumValue **enum_values;
  gboolean status;
  gchar *data;
  gint i;

  overrides = hyscan_data_schema_overrides_new ();

  for (i = 0; BooleanOverridesList[i].key_id != NULL; i++)
    {
      status = hyscan_data_schema_overrides_set_boolean (overrides,
                                                         BooleanOverridesList[i].key_id,
                                                         BooleanOverridesList[i].default_value,
                                                         BooleanOverridesList[i].readonly);

      if (!status)
        g_error ("%s: can't set overrides", BooleanOverridesList[i].key_id);
    }

  for (i = 0; IntegerOverridesList[i].key_id != NULL; i++)
    {
      status = hyscan_data_schema_overrides_set_integer (overrides,
                                                         IntegerOverridesList[i].key_id,
                                                         IntegerOverridesList[i].default_value,
                                                         IntegerOverridesList[i].minimum_value,
                                                         IntegerOverridesList[i].maximum_value,
                                                         IntegerOverridesList[i].value_step,
                                                         IntegerOverridesList[i].readonly);

      if (!status)
        g_error ("%s: can't set overrides", IntegerOverridesList[i].key_id);
    }

  for (i = 0; DoubleOverridesList[i].key_id != NULL; i++)
    {
      status = hyscan_data_schema_overrides_set_double (overrides,
                                                        DoubleOverridesList[i].key_id,
                                                        DoubleOverridesList[i].default_value,
                                                        DoubleOverridesList[i].minimum_value,
                                                        DoubleOverridesList[i].maximum_value,
                                                        DoubleOverridesList[i].value_step,
                                                        DoubleOverridesList[i].readonly);

      if (!status)
        g_error ("%s: can't set overrides", DoubleOverridesList[i].key_id);
    }

  for (i = 0; StringOverridesList[i].key_id != NULL; i++)
    {
      status = hyscan_data_schema_overrides_set_string (overrides,
                                                        StringOverridesList[i].key_id,
                                                        StringOverridesList[i].default_value,
                                                        StringOverridesList[i].readonly);

      if (!status)
        g_error ("%s: can't set overrides", StringOverridesList[i].key_id);
    }

  for (i = 0; EnumOverridesList[i].key_id != NULL; i++)
    {
      status = hyscan_data_schema_overrides_set_enum (overrides,
                                                      EnumOverridesList[i].key_id,
                                                      EnumOverridesList[i].default_value,
                                                      EnumOverridesList[i].readonly);

      if (!status)
        g_error ("%s: can't set overrides", EnumOverridesList[i].key_id);
    }

  hyscan_data_schema_overrides_set_enum_values (overrides, "enum", EnumValueOverridesList);

  data = hyscan_data_schema_overrides_get_data (overrides);
  if (data == NULL)
    g_error ("can't get schema override data");

  g_object_unref (overrides);

  overrides = hyscan_data_schema_overrides_new_from_data (data);

  g_free (data);

  for (i = 0; BooleanOverridesList[i].key_id != NULL; i++)
    {
      gboolean default_value;
      gboolean readonly;

      status = hyscan_data_schema_overrides_get_boolean (overrides,
                                                         BooleanOverridesList[i].key_id,
                                                         &default_value,
                                                         &readonly);

      if (!status)
        g_error ("%s: can't get overrides", BooleanOverridesList[i].key_id);

      if (BooleanOverridesList[i].default_value != default_value)
        g_error ("%s: default value mismatch", BooleanOverridesList[i].key_id);

      if (BooleanOverridesList[i].readonly != readonly)
        g_error ("%s: readonly parameter mismatch", BooleanOverridesList[i].key_id);
    }

  for (i = 0; IntegerOverridesList[i].key_id != NULL; i++)
    {
      gint64 default_value;
      gint64 minimum_value;
      gint64 maximum_value;
      gint64 value_step;
      gboolean readonly;

      status = hyscan_data_schema_overrides_get_integer (overrides,
                                                         IntegerOverridesList[i].key_id,
                                                         &default_value,
                                                         &minimum_value,
                                                         &maximum_value,
                                                         &value_step,
                                                         &readonly);

      if (!status)
        g_error ("%s: can't get overrides", IntegerOverridesList[i].key_id);

      if (IntegerOverridesList[i].default_value != default_value)
        g_error ("%s: default value mismatch", IntegerOverridesList[i].key_id);

      if (IntegerOverridesList[i].minimum_value != minimum_value)
        g_error ("%s: minimum value mismatch", IntegerOverridesList[i].key_id);

      if (IntegerOverridesList[i].maximum_value != maximum_value)
        g_error ("%s: maximum value mismatch", IntegerOverridesList[i].key_id);

      if (IntegerOverridesList[i].value_step != value_step)
        g_error ("%s: value step mismatch", IntegerOverridesList[i].key_id);

      if (IntegerOverridesList[i].readonly != readonly)
        g_error ("%s: readonly parameter mismatch", IntegerOverridesList[i].key_id);
    }

  for (i = 0; DoubleOverridesList[i].key_id != NULL; i++)
    {
      gdouble default_value;
      gdouble minimum_value;
      gdouble maximum_value;
      gdouble value_step;
      gboolean readonly;

      status = hyscan_data_schema_overrides_get_double (overrides,
                                                        DoubleOverridesList[i].key_id,
                                                        &default_value,
                                                        &minimum_value,
                                                        &maximum_value,
                                                        &value_step,
                                                        &readonly);

      if (!status)
        g_error ("%s: can't get overrides", DoubleOverridesList[i].key_id);

      if (DoubleOverridesList[i].default_value != default_value)
        g_error ("%s: default value mismatch", DoubleOverridesList[i].key_id);

      if (DoubleOverridesList[i].minimum_value != minimum_value)
        g_error ("%s: minimum value mismatch", DoubleOverridesList[i].key_id);

      if (DoubleOverridesList[i].maximum_value != maximum_value)
        g_error ("%s: maximum value mismatch", DoubleOverridesList[i].key_id);

      if (DoubleOverridesList[i].value_step != value_step)
        g_error ("%s: value step mismatch", DoubleOverridesList[i].key_id);

      if (DoubleOverridesList[i].readonly != readonly)
        g_error ("%s: readonly parameter mismatch", DoubleOverridesList[i].key_id);
    }

  for (i = 0; StringOverridesList[i].key_id != NULL; i++)
    {
      gchar *default_value;
      gboolean readonly;

      status = hyscan_data_schema_overrides_get_string (overrides,
                                                        StringOverridesList[i].key_id,
                                                        &default_value,
                                                        &readonly);

      if (!status)
        g_error ("%s: can't get overrides", StringOverridesList[i].key_id);

      if (g_strcmp0 (StringOverridesList[i].default_value, default_value) != 0)
        g_error ("%s: default value mismatch", StringOverridesList[i].key_id);

      if (StringOverridesList[i].readonly != readonly)
        g_error ("%s: readonly parameter mismatch", StringOverridesList[i].key_id);

      g_free (default_value);
    }

  for (i = 0; EnumOverridesList[i].key_id != NULL; i++)
    {
      gint64 default_value;
      gboolean readonly;

      status = hyscan_data_schema_overrides_get_enum (overrides,
                                                      EnumOverridesList[i].key_id,
                                                      &default_value,
                                                      &readonly);

      if (!status)
        g_error ("%s: can't get overrides", EnumOverridesList[i].key_id);

      if (EnumOverridesList[i].default_value != default_value)
        g_error ("%s: default value mismatch", EnumOverridesList[i].key_id);

      if (EnumOverridesList[i].readonly != readonly)
        g_error ("%s: readonly parameter mismatch", EnumOverridesList[i].key_id);
    }

  enum_values = hyscan_data_schema_overrides_get_enum_values (overrides, "enum");
  if (enum_values == NULL)
    g_error ("can't get enum values overrides");

  for (i = 0; EnumValueOverridesList[i] != NULL; i++)
    {
      if (EnumValueOverridesList[i]->value != enum_values[i]->value)
        g_error ("%d: enum value mismatch", i);

      if (g_strcmp0 (EnumValueOverridesList[i]->name, enum_values[i]->name) != 0)
        g_error ("%d: enum name mismatch", i);

      if (g_strcmp0 (EnumValueOverridesList[i]->description, enum_values[i]->description) != 0)
        g_error ("%d: enum description mismatch", i);
    }

  hyscan_data_schema_free_enum_values (enum_values);

  g_object_unref (overrides);

  return 0;
}
