#include <hyscan-param-merge.h>
#include <hyscan-param-controller.h>
#include <hyscan-data-schema-builder.h>

#define GENDER_ENUM    "gender"
#define GENDER_UNKNOWN "unknown"
#define GENDER_MALE    "male"
#define GENDER_FEMALE  "female"

enum
{
  SCHEMA1_UNKNOWN,
  SCHEMA1_MALE,
  SCHEMA1_FEMALE,
};

enum
{
  SCHEMA2_MALE,
  SCHEMA2_FEMALE,
};

typedef struct
{
  GString   *name;
  gint64     age;
  gboolean   accept;
  gint64     gender;
} Person;

static HyScanDataSchema *
build_schema1 (void)
{
  HyScanDataSchemaBuilder *builder;
  HyScanDataSchema *schema;

  builder = hyscan_data_schema_builder_new ("schema1");

  hyscan_data_schema_builder_key_string_create (builder, "/name", "Name", NULL, "Bill");
  hyscan_data_schema_builder_key_integer_create (builder, "/age", "Age", NULL, 64);
  hyscan_data_schema_builder_key_integer_range (builder, "/age", 20, 120, 1);
  hyscan_data_schema_builder_key_boolean_create (builder, "/accept", "Accept", NULL, TRUE);

  hyscan_data_schema_builder_enum_create (builder, GENDER_ENUM);
  hyscan_data_schema_builder_enum_value_create (builder, GENDER_ENUM, SCHEMA1_UNKNOWN, GENDER_UNKNOWN, "Unknown", NULL);
  hyscan_data_schema_builder_enum_value_create (builder, GENDER_ENUM, SCHEMA1_MALE, GENDER_MALE, "Male", NULL);
  hyscan_data_schema_builder_enum_value_create (builder, GENDER_ENUM, SCHEMA1_FEMALE, GENDER_FEMALE, "Female", NULL);

  hyscan_data_schema_builder_key_enum_create (builder, "/gender", "Gender", NULL, GENDER_ENUM, 0);

  schema = hyscan_data_schema_builder_get_schema (builder);

  g_object_unref (builder);

  return schema;
}

static HyScanDataSchema *
build_schema2 (void)
{
  HyScanDataSchemaBuilder *builder;
  HyScanDataSchema *schema;

  builder = hyscan_data_schema_builder_new ("schema2");

  hyscan_data_schema_builder_key_string_create (builder, "/name", "Name", NULL, "Steve");
  hyscan_data_schema_builder_key_integer_create (builder, "/age", "Age", NULL, 64);
  hyscan_data_schema_builder_key_integer_range (builder, "/age", 0, 100, 1);

  hyscan_data_schema_builder_enum_create (builder, GENDER_ENUM);
  hyscan_data_schema_builder_enum_value_create (builder, GENDER_ENUM, SCHEMA2_MALE, GENDER_MALE, "Male", NULL);
  hyscan_data_schema_builder_enum_value_create (builder, GENDER_ENUM, SCHEMA2_FEMALE, GENDER_FEMALE, "Female", NULL);

  hyscan_data_schema_builder_key_enum_create (builder, "/gender", "Gender", NULL, GENDER_ENUM, 0);

  schema = hyscan_data_schema_builder_get_schema (builder);

  g_object_unref (builder);

  return schema;
}

static Person *
person_new (const gchar *name,
            gint64       age,
            gboolean     accept,
            gint64       gender)
{
  Person *person = g_new0 (Person, 1);
  person->name = g_string_new (name);
  person->age = age;
  person->accept = accept;
  person->gender = gender;

  return person;
}

static void
person_free (Person *person)
{
  g_string_free (person->name, TRUE);
  g_free (person);
}

int
main (int    argc,
      char **argv)
{
  HyScanParamMerge *merge;
  HyScanParamController *param1, *param2;
  HyScanDataSchema *schema;
  Person *person1, *person2;

  param1 = hyscan_param_controller_new (NULL);
  person1 = person_new ("Steve", 64, TRUE, SCHEMA1_MALE);
  schema = build_schema1 ();
  hyscan_param_controller_set_schema (param1, schema);
  hyscan_param_controller_add_string (param1, "/name", person1->name);
  hyscan_param_controller_add_boolean (param1, "/accept", &person1->accept);
  hyscan_param_controller_add_integer (param1, "/age", &person1->age);
  hyscan_param_controller_add_enum (param1, "/gender", &person1->gender);
  g_object_unref (schema);

  param2 = hyscan_param_controller_new (NULL);
  person2 = person_new ("Bill", 64, TRUE, SCHEMA2_MALE);
  schema = build_schema2 ();
  hyscan_param_controller_set_schema (param2, schema);
  hyscan_param_controller_add_string (param2, "/name", person2->name);
  hyscan_param_controller_add_boolean (param2, "/accept", &person2->accept);
  hyscan_param_controller_add_integer (param2, "/age", &person2->age);
  hyscan_param_controller_add_enum (param2, "/gender", &person2->gender);
  g_object_unref (schema);

  merge = hyscan_param_merge_new();
  hyscan_param_merge_add (merge, HYSCAN_PARAM (param1));
  hyscan_param_merge_add (merge, HYSCAN_PARAM (param2));
  g_assert_true (hyscan_param_merge_bind (merge));
  g_object_unref (param1);
  g_object_unref (param2);

  /* Проверяем, что допустимые значения ужались. */
  {
    schema = hyscan_param_schema (HYSCAN_PARAM (merge));
    gint64 min_value, max_value;
    hyscan_data_schema_key_get_integer (schema, "/age", &min_value, &max_value, NULL, NULL);
    g_assert_cmpint (min_value, ==, 20);
    g_assert_cmpint (max_value, ==, 100);
    g_object_unref (schema);
  }

  /* Проверяем, что перечисление уменьшилось до общих значений. */
  {
    GList *values;

    schema = hyscan_param_schema (HYSCAN_PARAM (merge));
    values = hyscan_data_schema_enum_get_values (schema, GENDER_ENUM);
    g_assert_cmpint (2, ==, g_list_length (values));
    g_assert_nonnull (hyscan_data_schema_enum_find_by_id (schema, GENDER_ENUM, GENDER_MALE));
    g_assert_nonnull (hyscan_data_schema_enum_find_by_id (schema, GENDER_ENUM, GENDER_FEMALE));
    g_list_free (values);
    g_object_unref (schema);
  }

  /* Проверяем, что отстуствуюшие в одной из схем ключи не добавились. */
  {
    schema = hyscan_param_schema (HYSCAN_PARAM (merge));
    g_assert_false (hyscan_data_schema_has_key (schema, "/accept"));
    g_object_unref (schema);
  }

  /* Проверяем, что перечисления устанавливаются корректно, согласно схемам обработчиков. */
  g_assert_false (hyscan_param_merge_get_keep (merge, "/gender"));
  {
    HyScanParamList *list;
    const HyScanDataSchemaEnumValue *female;

    schema = hyscan_param_schema (HYSCAN_PARAM (merge));

    /* Находим численное значение для id = GENDER_FEMALE. Какое оно будет - это зависит от реализации. */
    list = hyscan_param_list_new ();
    female = hyscan_data_schema_enum_find_by_id (schema, GENDER_ENUM, GENDER_FEMALE);
    hyscan_param_list_set_enum (list, "/gender", female->value);

    g_assert_cmpint (person1->gender, ==, SCHEMA1_MALE);
    g_assert_cmpint (person2->gender, ==, SCHEMA2_MALE);

    hyscan_param_set (HYSCAN_PARAM (merge), list);

    g_assert_cmpint (person1->gender, ==, SCHEMA1_FEMALE);
    g_assert_cmpint (person2->gender, ==, SCHEMA2_FEMALE);

    g_object_unref (list);
    g_object_unref (schema);
  }

  /* Проверяем, что параметр установился в оба обработчика. */
  {
    HyScanParamList *list;
    list = hyscan_param_list_new ();
    hyscan_param_list_set_integer (list, "/age", 42);
    hyscan_param_set (HYSCAN_PARAM (merge), list);
    g_object_unref (list);
    g_assert_cmpint (person1->age, ==, 42);
    g_assert_cmpint (person2->age, ==, 42);
  }

  /* Проверяем, что значение параметра сохранилось без изменений. */
  g_assert_true (hyscan_param_merge_get_keep (merge, "/name"));
  {
    HyScanParamList *list;
    list = hyscan_param_list_new ();
    hyscan_param_list_set_string (list, "/name", "Elon");
    hyscan_param_set (HYSCAN_PARAM (merge), list);
    g_object_unref (list);
    g_assert_cmpstr (person1->name->str, !=, "Elon");
    g_assert_cmpstr (person2->name->str, !=, "Elon");
  }

  /* Теперь значение должно поменяться. */
  hyscan_param_merge_set_keep (merge, "/name", FALSE);
  {
    HyScanParamList *list;
    list = hyscan_param_list_new ();
    hyscan_param_list_set_string (list, "/name", "Linus");
    hyscan_param_set (HYSCAN_PARAM (merge), list);
    g_object_unref (list);
    g_assert_cmpstr (person1->name->str, ==, "Linus");
    g_assert_cmpstr (person2->name->str, ==, "Linus");
  }

  g_message ("Test has been done successfully");

  person_free (person1);
  person_free (person2);
  g_object_unref (merge);

  return 0;
}
