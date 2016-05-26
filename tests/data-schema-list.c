#include <hyscan-data-schema.h>
#include <string.h>
#include <stdio.h>

void print_node (HyScanDataSchema     *schema,
                 HyScanDataSchemaNode *nodes)
{
  gint i;

  for (i = 0; i < nodes->n_nodes; i++)
    print_node (schema, nodes->nodes[i]);

  if (strlen (nodes->path) == 0)
    printf ("Node: '/'\n");
  else
    printf ("Node: '%s'\n", nodes->path);

  for (i = 0; i < nodes->n_params; i++)
    {
      printf (" * %s (%s) %s\n", nodes->params[i]->name,
                                 nodes->params[i]->description,
                                 nodes->params[i]->readonly ? "(READONLY)" : "");

      switch (nodes->params[i]->type)
        {
        case HYSCAN_DATA_SCHEMA_TYPE_BOOLEAN:
          {
            gboolean default_value;
            hyscan_data_schema_key_get_default_boolean (schema, nodes->params[i]->id, &default_value);
            printf ("     Default value: %s\n", default_value ? "TRUE" : "FALSE");
          }
          break;

        case HYSCAN_DATA_SCHEMA_TYPE_INTEGER:
          {
            gint64 default_value;
            gint64 minimum_value;
            gint64 maximum_value;
            gint64 value_step;
            hyscan_data_schema_key_get_default_integer (schema, nodes->params[i]->id, &default_value);
            hyscan_data_schema_key_get_integer_range (schema, nodes->params[i]->id, &minimum_value, &maximum_value);
            hyscan_data_schema_key_get_integer_step (schema, nodes->params[i]->id, &value_step);
            printf ("     Default value: %" G_GINT64_FORMAT, default_value);
            if (!nodes->params[i]->readonly)
              {
                printf (", minimum value %" G_GINT64_FORMAT", maximum value %" G_GINT64_FORMAT, minimum_value, maximum_value);
                printf (", step %" G_GINT64_FORMAT "\n", value_step);
              }
            else
              {
                printf ("\n");
              }
          }
          break;

        case HYSCAN_DATA_SCHEMA_TYPE_DOUBLE:
          {
            gdouble default_value;
            gdouble minimum_value;
            gdouble maximum_value;
            gdouble value_step;
            hyscan_data_schema_key_get_default_double (schema, nodes->params[i]->id, &default_value);
            hyscan_data_schema_key_get_double_range (schema, nodes->params[i]->id, &minimum_value, &maximum_value);
            hyscan_data_schema_key_get_double_step (schema, nodes->params[i]->id, &value_step);
            printf ("     Default value: %.03lf", default_value);
            if (!nodes->params[i]->readonly)
              {
                printf (", minimum value %.03lf, maximum value %.03lf", minimum_value, maximum_value);
                printf (", step %.03lf\n", value_step);
              }
            else
              {
                printf ("\n");
              }
          }
          break;

        case HYSCAN_DATA_SCHEMA_TYPE_STRING:
          {
            const gchar *default_value;
            default_value = hyscan_data_schema_key_get_default_string (schema, nodes->params[i]->id);
            printf ("     Default value: %s\n", default_value != NULL ? default_value : "'no default value'");
          }
          break;

        case HYSCAN_DATA_SCHEMA_TYPE_ENUM:
          {
            HyScanDataSchemaEnumValue **values;
            gint64 default_value;
            gint j;

            values = hyscan_data_schema_key_get_enum_values (schema, nodes->params[i]->id);
            hyscan_data_schema_key_get_default_enum (schema, nodes->params[i]->id, &default_value);

            for (j = 0; values != NULL && values[j] != NULL; j++)
              {
                if (nodes->params[i]->readonly &&
                    values[j]->value != default_value)
                  continue;

                if (values[j]->value == default_value)
                  printf ("     * ");
                else
                  printf ("       ");

                printf ("%s (%s)\n", values[j]->name, values[j]->description);
              }

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
