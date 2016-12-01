#include <hyscan-data-schema.h>
#include <string.h>
#include <stdio.h>

void print_node (HyScanDataSchema     *schema,
                 HyScanDataSchemaNode *nodes)
{
  guint i;

  for (i = 0; i < nodes->n_nodes; i++)
    print_node (schema, nodes->nodes[i]);

  for (i = 0; i < nodes->n_keys; i++)
    {
      const gchar *access = "";

      if (nodes->keys[i]->access == HYSCAN_DATA_SCHEMA_ACCESS_READONLY)
        access = "(READONLY)";
      else if (nodes->keys[i]->access == HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY)
        access = "(WRITEONLY)";

      if (nodes->keys[i]->description != NULL)
        printf ("%s (%s) %s\n", nodes->keys[i]->id, nodes->keys[i]->description, access);
      else
        printf ("%s %s\n", nodes->keys[i]->id, access);

      switch (nodes->keys[i]->type)
        {
        case HYSCAN_DATA_SCHEMA_KEY_BOOLEAN:
          {
            GVariant *default_value;
            default_value = hyscan_data_schema_key_get_default (schema, nodes->keys[i]->id);
            printf ("     Default value: %s\n", g_variant_get_boolean (default_value) ? "TRUE" : "FALSE");
            g_variant_unref (default_value);
          }
          break;

        case HYSCAN_DATA_SCHEMA_KEY_INTEGER:
          {
            GVariant *default_value;
            GVariant *minimum_value;
            GVariant *maximum_value;
            GVariant *value_step;
            default_value = hyscan_data_schema_key_get_default (schema, nodes->keys[i]->id);
            minimum_value = hyscan_data_schema_key_get_minimum (schema, nodes->keys[i]->id);
            maximum_value = hyscan_data_schema_key_get_maximum (schema, nodes->keys[i]->id);
            value_step = hyscan_data_schema_key_get_step (schema, nodes->keys[i]->id);
            printf ("     Default value: %" G_GINT64_FORMAT, g_variant_get_int64 (default_value));
            if (nodes->keys[i]->access != HYSCAN_DATA_SCHEMA_ACCESS_READONLY)
              {
                if (g_variant_get_int64 (minimum_value) != G_MININT64)
                  printf (", minimum value %" G_GINT64_FORMAT, g_variant_get_int64 (minimum_value));

                if (g_variant_get_int64 (maximum_value) != G_MAXINT64)
                  printf (", maximum value %" G_GINT64_FORMAT, g_variant_get_int64 (maximum_value));

                if (g_variant_get_int64 (value_step) != 1)
                  printf (", step %" G_GINT64_FORMAT, g_variant_get_int64 (value_step));
              }

            printf ("\n");

            g_variant_unref (default_value);
            g_variant_unref (minimum_value);
            g_variant_unref (maximum_value);
            g_variant_unref (value_step);
          }
          break;

        case HYSCAN_DATA_SCHEMA_KEY_DOUBLE:
          {
            GVariant *default_value;
            GVariant *minimum_value;
            GVariant *maximum_value;
            GVariant *value_step;
            default_value = hyscan_data_schema_key_get_default (schema, nodes->keys[i]->id);
            minimum_value = hyscan_data_schema_key_get_minimum (schema, nodes->keys[i]->id);
            maximum_value = hyscan_data_schema_key_get_maximum (schema, nodes->keys[i]->id);
            value_step = hyscan_data_schema_key_get_step (schema, nodes->keys[i]->id);
            printf ("     Default value: %.03lf", g_variant_get_double (default_value));
            if (nodes->keys[i]->access != HYSCAN_DATA_SCHEMA_ACCESS_READONLY)
              {
                if (g_variant_get_double (minimum_value) != -G_MAXDOUBLE)
                  printf (", minimum value %.03lf", g_variant_get_double (minimum_value));

                if (g_variant_get_double (maximum_value) != G_MAXDOUBLE)
                  printf (", maximum value %.03lf", g_variant_get_double (maximum_value));

                if (g_variant_get_double (value_step) != 1.0)
                  printf (", step %.03lf", g_variant_get_double (value_step));
              }

            printf ("\n");

            g_variant_unref (default_value);
            g_variant_unref (minimum_value);
            g_variant_unref (maximum_value);
            g_variant_unref (value_step);
          }
          break;

        case HYSCAN_DATA_SCHEMA_KEY_STRING:
          {
            GVariant *default_value;
            default_value = hyscan_data_schema_key_get_default (schema, nodes->keys[i]->id);
            printf ("     Default value: %s\n",
                    default_value != NULL ?
                      g_variant_get_string (default_value, NULL) : "'no default value'");
            if (default_value != NULL)
              g_variant_unref (default_value);
          }
          break;

        case HYSCAN_DATA_SCHEMA_KEY_ENUM:
          {
            const gchar *enum_id;
            HyScanDataSchemaEnumValue **values;
            GVariant *default_value;
            gint j;

            enum_id = hyscan_data_schema_key_get_enum_id (schema, nodes->keys[i]->id);
            values = hyscan_data_schema_key_get_enum_values (schema, enum_id);
            default_value = hyscan_data_schema_key_get_default (schema, nodes->keys[i]->id);

            for (j = 0; values != NULL && values[j] != NULL; j++)
              {
                if ((nodes->keys[i]->access == HYSCAN_DATA_SCHEMA_ACCESS_READONLY) &&
                    (values[j]->value != g_variant_get_int64 (default_value)))
                  continue;

                if (values[j]->value == g_variant_get_int64 (default_value))
                  printf ("     * ");
                else
                  printf ("       ");

                if (values[j]->description != NULL)
                  printf ("%s (%s)\n", values[j]->name, values[j]->description);
                else
                  printf ("%s\n", values[j]->name);
              }

            g_variant_unref (default_value);
            hyscan_data_schema_free_enum_values (values);
          }
          break;

        default:
          break;
        }
    }
}

int
main (int argc, char **argv)
{
  HyScanDataSchema *schema;
  HyScanDataSchemaNode *nodes;

  if (argc != 3)
    {
      g_message ("Usage: data-schema-list <schema-file> <schema-id>");
      return -1;
    }

  schema = hyscan_data_schema_new_from_file (argv[1], argv[2]);

  nodes = hyscan_data_schema_list_nodes (schema);
  if (nodes == NULL)
    g_error ("can't list nodes");

  print_node (schema, nodes);

  hyscan_data_schema_free_nodes (nodes);

  g_object_unref (schema);

  return 0;
}
