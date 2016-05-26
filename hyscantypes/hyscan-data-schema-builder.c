/*
 * \file hyscan-data-schema-builder.c
 *
 * \brief Исходный файл класса генерации схемы данных.
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2016
 * \license Проприетарная лицензия ООО "Экран"
 *
 */

#include "hyscan-data-schema-builder.h"
#include "hyscan-data-schema-internal.h"

#include <gio/gio.h>
#include <string.h>

enum
{
  PROP_O,
  PROP_SCHEMA_ID,
  PROP_GETTEXT_DOMAIN
};

struct _HyScanDataSchemaBuilderPrivate
{
  gchar                       *schema_id;              /* Идентификатор схемы. */
  gchar                       *gettext_domain;         /* Домен gettext. */

  GHashTable                  *keys;                   /* Список параметров. */
  GHashTable                  *enums;                  /* Список значений перечисляемых типов. */
};

static void            hyscan_data_schema_builder_set_property         (GObject               *object,
                                                                        guint                  prop_id,
                                                                        const GValue          *value,
                                                                        GParamSpec            *pspec);
static void            hyscan_data_schema_builder_object_constructed   (GObject               *object);
static void            hyscan_data_schema_builder_object_finalize      (GObject               *object);

static void            hyscan_data_schema_builder_dump_enum            (GOutputStream         *ostream,
                                                                        HyScanDataSchemaEnum  *values);
static void            hyscan_data_schema_builder_dump_node            (GOutputStream         *ostream,
                                                                        GHashTable            *keys,
                                                                        HyScanDataSchemaNode  *node,
                                                                        gint                   level);
static void            hyscan_data_schema_builder_dump_key             (GOutputStream         *ostream,
                                                                        HyScanDataSchemaInternalKey *key,
                                                                        gint                   level);

G_DEFINE_TYPE_WITH_PRIVATE (HyScanDataSchemaBuilder, hyscan_data_schema_builder, G_TYPE_OBJECT)

static void
hyscan_data_schema_builder_class_init (HyScanDataSchemaBuilderClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = hyscan_data_schema_builder_set_property;

  object_class->constructed = hyscan_data_schema_builder_object_constructed;
  object_class->finalize = hyscan_data_schema_builder_object_finalize;

  g_object_class_install_property (object_class, PROP_SCHEMA_ID,
    g_param_spec_string ("schema-id", "SchemaID", "Data schema id", NULL,
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_property (object_class, PROP_GETTEXT_DOMAIN,
    g_param_spec_string ("gettext-domain", "GettextDomain", "Gettext domain", NULL,
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

static void
hyscan_data_schema_builder_init (HyScanDataSchemaBuilder *data_schema_builder)
{
  data_schema_builder->priv = hyscan_data_schema_builder_get_instance_private (data_schema_builder);
}

static void
hyscan_data_schema_builder_set_property (GObject      *object,
                                         guint         prop_id,
                                         const GValue *value,
                                         GParamSpec   *pspec)
{
  HyScanDataSchemaBuilder *data_schema_builder = HYSCAN_DATA_SCHEMA_BUILDER (object);
  HyScanDataSchemaBuilderPrivate *priv = data_schema_builder->priv;

  switch (prop_id)
    {
    case PROP_SCHEMA_ID:
      priv->schema_id = g_value_dup_string (value);
      break;

    case PROP_GETTEXT_DOMAIN:
      priv->gettext_domain = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hyscan_data_schema_builder_object_constructed (GObject *object)
{
  HyScanDataSchemaBuilder *data_schema_builder = HYSCAN_DATA_SCHEMA_BUILDER (object);
  HyScanDataSchemaBuilderPrivate *priv = data_schema_builder->priv;

  /* Таблица параметров. */
  priv->keys =  g_hash_table_new_full (g_str_hash,
                                       g_str_equal,
                                       NULL,
                                       (GDestroyNotify)hyscan_data_schema_free_key);

  /* Таблица enum типов и их значений. */
  priv->enums = g_hash_table_new_full (g_str_hash,
                                       g_str_equal,
                                       NULL,
                                       (GDestroyNotify)hyscan_data_schema_free_enum);
}

static void
hyscan_data_schema_builder_object_finalize (GObject *object)
{
  HyScanDataSchemaBuilder *data_schema_builder = HYSCAN_DATA_SCHEMA_BUILDER (object);
  HyScanDataSchemaBuilderPrivate *priv = data_schema_builder->priv;

  g_hash_table_unref (priv->enums);
  g_hash_table_unref (priv->keys);

  g_free (priv->schema_id);
  g_free (priv->gettext_domain);

  G_OBJECT_CLASS (hyscan_data_schema_builder_parent_class)->finalize (object);
}

/* Функция формирует XML описание enum значений. */
static void
hyscan_data_schema_builder_dump_enum (GOutputStream        *ostream,
                                      HyScanDataSchemaEnum *values)
{
  gint i;

  g_output_stream_printf (ostream, NULL, NULL, NULL,
                          "  <enum id=\"%s\">\n",
                          values->id);

  for (i = 0; i < values->n_values; i++)
    {
      if (values->values[i]->description == NULL)
        {
          g_output_stream_printf (ostream, NULL, NULL, NULL,
                                  "    <value name=\"%s\" value=\"%" G_GINT64_FORMAT "\"/>\n",
                                  values->values[i]->name, values->values[i]->value);
        }
      else
        {
          g_output_stream_printf (ostream, NULL, NULL, NULL,
                                  "    <value name=\"%s\" value=\"%" G_GINT64_FORMAT "\">\n",
                                  values->values[i]->name, values->values[i]->value);

          g_output_stream_printf (ostream, NULL, NULL, NULL,
                                  "      <description>%s</description>\n",
                                  values->values[i]->description);

          g_output_stream_printf (ostream, NULL, NULL, NULL,
                                  "    </value>\n");
        }
    }

  g_output_stream_printf (ostream, NULL, NULL, NULL,
                          "  </enum>\n");
}

/* Функция формирует XML описание узла с параметрами. */
static void
hyscan_data_schema_builder_dump_node (GOutputStream        *ostream,
                                      GHashTable           *keys,
                                      HyScanDataSchemaNode *node,
                                      gint                  level)
{
  gchar *indent;
  gchar **pathv;
  gint i;

  indent = g_malloc0 (2 + 2 * level + 1);
  memset (indent, ' ', 2 + 2 * level);

  pathv = g_strsplit (node->path, "/", -1);

  if (level > 0)
    {
      g_output_stream_printf (ostream, NULL, NULL, NULL,
                              "%s<node id=\"%s\">\n",
                              indent, pathv[level]);
    }

  /* Рекурсивное описание всех узлов. */
  for (i = 0; i < node->n_nodes; i++)
    hyscan_data_schema_builder_dump_node (ostream, keys, node->nodes[i], level + 1);

  /* Описание всех параметров. */
  for (i = 0; i < node->n_keys; i++)
    {
      HyScanDataSchemaInternalKey *key = g_hash_table_lookup (keys, node->keys[i]->id);
      hyscan_data_schema_builder_dump_key (ostream, key, level);
    }

  if (level > 0)
    {
      g_output_stream_printf (ostream, NULL, NULL, NULL,
                              "%s</node>\n",
                              indent);
    }

  g_strfreev (pathv);
  g_free (indent);
}

/* Функция формирует XML описание параметра. */
static void
hyscan_data_schema_builder_dump_key (GOutputStream               *ostream,
                                     HyScanDataSchemaInternalKey *key,
                                     gint                         level)
{
  gchar *indent;
  gchar **pathv;
  gchar *type;

  indent = g_malloc0 (4 + 2 * level + 1);
  memset (indent, ' ', 4 + 2 * level);

  pathv = g_strsplit (key->id, "/", -1);

  switch (key->type)
    {
    case HYSCAN_DATA_SCHEMA_TYPE_BOOLEAN:
      type = "boolean";
      break;

    case HYSCAN_DATA_SCHEMA_TYPE_INTEGER:
      type = "integer";
      break;

    case HYSCAN_DATA_SCHEMA_TYPE_DOUBLE:
      type = "double";
      break;

    case HYSCAN_DATA_SCHEMA_TYPE_STRING:
      type = "string";
      break;

    case HYSCAN_DATA_SCHEMA_TYPE_ENUM:
      type = "enum";
      break;

    default:
      type = NULL;
      break;
    }

  if (type == NULL)
    return;

  /* Заголовок параметра. */
  if (key->type != HYSCAN_DATA_SCHEMA_TYPE_ENUM)
    {
      g_output_stream_printf (ostream, NULL, NULL, NULL,
                              "%s<key id=\"%s\" name=\"%s\" type=\"%s\"",
                              indent, pathv[level+1], key->name, type);
    }
  else
    {
      g_output_stream_printf (ostream, NULL, NULL, NULL,
                              "%s<key id=\"%s\" name=\"%s\" enum=\"%s\"",
                              indent, pathv[level+1], key->name, key->enum_values->id);
    }

  /* Флаги параметра. */
  if (key->readonly)
    {
      g_output_stream_printf (ostream, NULL, NULL, NULL,
                              " flags=\"readonly\">\n");
    }
  else
    {
      g_output_stream_printf (ostream, NULL, NULL, NULL,
                              ">\n");
    }

  /* Описание параметра. */
  if (key->description != NULL)
    {
      g_output_stream_printf (ostream, NULL, NULL, NULL,
                              "%s  <description>%s</description>\n",
                              indent, key->description);
    }

  /* Дипазон допустимых значений и значение по умолчанию. */
  switch (key->type)
    {
    case HYSCAN_DATA_SCHEMA_TYPE_BOOLEAN:
      {
        gboolean default_value = hyscan_data_schema_value_get_boolean (&key->default_value);

        if (default_value)
          {
            g_output_stream_printf (ostream, NULL, NULL, NULL,
                                    "%s  <default>true</default>\n",
                                    indent);
          }
      }
      break;

    case HYSCAN_DATA_SCHEMA_TYPE_INTEGER:
      {
        gint64 default_value = hyscan_data_schema_value_get_integer (&key->default_value);
        gint64 minimum_value = hyscan_data_schema_value_get_integer (&key->minimum_value);
        gint64 maximum_value = hyscan_data_schema_value_get_integer (&key->maximum_value);
        gint64 value_step = hyscan_data_schema_value_get_integer (&key->value_step);

        if (default_value != 0)
          {
            g_output_stream_printf (ostream, NULL, NULL, NULL,
                                    "%s  <default>%" G_GINT64_FORMAT "</default>\n",
                                    indent, default_value);
          }

        if (minimum_value != G_MININT64 || maximum_value != G_MAXINT64 || value_step != 1)
          {
            g_output_stream_printf (ostream, NULL, NULL, NULL,
                                    "%s  <range min=\"%" G_GINT64_FORMAT "\" "
                                    "max=\"%" G_GINT64_FORMAT "\" "
                                    "step=\"%" G_GINT64_FORMAT "\"/>\n",
                                    indent, minimum_value, maximum_value, value_step);
          }
      }
      break;

    case HYSCAN_DATA_SCHEMA_TYPE_DOUBLE:
      {
        gdouble default_value = hyscan_data_schema_value_get_double (&key->default_value);
        gdouble minimum_value = hyscan_data_schema_value_get_double (&key->minimum_value);
        gdouble maximum_value = hyscan_data_schema_value_get_double (&key->maximum_value);
        gdouble value_step = hyscan_data_schema_value_get_double (&key->value_step);

        if (default_value != 0)
          {
            g_output_stream_printf (ostream, NULL, NULL, NULL,
                                    "%s  <default>%f</default>\n",
                                    indent, default_value);
          }

        if (minimum_value != -G_MAXDOUBLE || maximum_value != G_MAXDOUBLE || value_step != 1.0)
          {
            g_output_stream_printf (ostream, NULL, NULL, NULL,
                                    "%s  <range min=\"%f\" "
                                    "max=\"%f\" "
                                    "step=\"%f\"/>\n",
                                    indent, minimum_value, maximum_value, value_step);
          }
      }
      break;

    case HYSCAN_DATA_SCHEMA_TYPE_STRING:
      {
        const gchar *default_value = hyscan_data_schema_value_get_string (&key->default_value);

        if (default_value != NULL)
          {
            g_output_stream_printf (ostream, NULL, NULL, NULL,
                                    "%s  <default>%s</default>\n",
                                    indent, default_value);
          }
      }
      break;

    case HYSCAN_DATA_SCHEMA_TYPE_ENUM:
      {
        gint64 default_value = hyscan_data_schema_value_get_integer (&key->default_value);

        if (default_value != 0)
          {
            g_output_stream_printf (ostream, NULL, NULL, NULL,
                                    "%s  <default>%" G_GINT64_FORMAT "</default>\n",
                                    indent, default_value);
          }
      }
      break;

    default:
      break;
    }

  g_output_stream_printf (ostream, NULL, NULL, NULL,
                          "%s</key>\n",
                          indent);

  g_strfreev (pathv);
  g_free (indent);
}

/* Функция создаёт новый объект HyScanDataSchemaBuilder. */
HyScanDataSchemaBuilder *
hyscan_data_schema_builder_new (const gchar *schema_id)
{
  return g_object_new (HYSCAN_TYPE_DATA_SCHEMA_BUILDER,
                       "schema-id", schema_id,
                       NULL);
}

/* Функция создаёт новый объект HyScanDataSchemaBuilder с возможностью перевода. */
HyScanDataSchemaBuilder *
hyscan_data_schema_builder_new_with_gettext (const gchar *schema_id,
                                             const gchar *gettext_domain)
{
  return g_object_new (HYSCAN_TYPE_DATA_SCHEMA_BUILDER,
                       "schema-id", schema_id,
                       "gettext-domain", gettext_domain,
                       NULL);
}

/* Функция создаёт XML описание схемы данных. */
gchar *
hyscan_data_schema_builder_get_data (HyScanDataSchemaBuilder *builder)
{
  HyScanDataSchemaBuilderPrivate *priv;

  GHashTableIter iter;
  gpointer key, value;

  HyScanDataSchemaNode *nodes;

  GOutputStream *ostream;
  gchar *data;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), NULL);

  priv = builder->priv;

  /* Структура параметров. */
  nodes = hyscan_data_schema_new_node ("");
  g_hash_table_iter_init (&iter, priv->keys);
  while (g_hash_table_iter_next (&iter, &key, NULL))
    {
      gchar *key_id = key;
      HyScanDataSchemaInternalKey *key = g_hash_table_lookup (priv->keys, key_id);
      hyscan_data_schema_insert_param (nodes, key->id, key->name, key->description, key->type, key->readonly);
    }

  /* Виртуальный поток вывода данных в память. */
  ostream = g_memory_output_stream_new_resizable ();

  /* Заголовок XML данных. */
  g_output_stream_printf (ostream, NULL, NULL, NULL,
                          "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

  if (priv->gettext_domain == NULL)
    {
      g_output_stream_printf (ostream, NULL, NULL, NULL,
                              "<schemalist>\n");
    }
  else
    {
      g_output_stream_printf (ostream, NULL, NULL, NULL,
                              "<schemalist gettext-domain=\"%s\">\n",
                              priv->gettext_domain);
    }

  /* Enum значения. */
  g_hash_table_iter_init (&iter, priv->enums);
  while (g_hash_table_iter_next (&iter, &key, &value))
    hyscan_data_schema_builder_dump_enum (ostream, value);

  /* Описания параметров. */
  g_output_stream_printf (ostream, NULL, NULL, NULL,
                          "  <schema id=\"%s\">\n",
                          priv->schema_id);

  hyscan_data_schema_builder_dump_node (ostream, priv->keys, nodes, 0);

  g_output_stream_printf (ostream, NULL, NULL, NULL,
                          "  </schema>\n");

  g_output_stream_printf (ostream, NULL, NULL, NULL,
                          "</schemalist>\n");

  g_output_stream_close (ostream, NULL, NULL);
  data = g_memory_output_stream_steal_data (G_MEMORY_OUTPUT_STREAM (ostream));
  g_object_unref (ostream);

  hyscan_data_schema_free_nodes (nodes);

  return data;
}

/* Функция создаёт новый список допустимых значений параметра ENUM */
gboolean
hyscan_data_schema_builder_enum_create (HyScanDataSchemaBuilder *builder,
                                        const gchar             *enum_id)
{
  HyScanDataSchemaEnum *values;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  if (!hyscan_data_schema_validate_name (enum_id))
    return FALSE;

  if (g_hash_table_contains (builder->priv->enums, enum_id))
    return FALSE;

  values = g_new0 (HyScanDataSchemaEnum, 1);
  values->id = g_strdup ((const gchar *)enum_id);
  values->values = NULL;
  values->n_values = 0;

  return g_hash_table_insert (builder->priv->enums, values->id, values);
}

/* Функция создаёт новое значение параметра в списке возможных значений. */
gboolean
hyscan_data_schema_builder_enum_value_create (HyScanDataSchemaBuilder *builder,
                                              const gchar             *enum_id,
                                              gint64                   value,
                                              const gchar             *name,
                                              const gchar             *description)
{
  HyScanDataSchemaEnum *values;
  HyScanDataSchemaEnumValue *enum_value;
  gint i;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  values = g_hash_table_lookup (builder->priv->enums, enum_id);
  if (values == NULL)
    return FALSE;

  for (i = 0; i < values->n_values; i++)
    if (values->values[i]->value == value)
      return FALSE;

  values->values = g_realloc (values->values, (values->n_values + 2) * sizeof (HyScanDataSchemaEnumValue*));

  enum_value = g_new0 (HyScanDataSchemaEnumValue, 1);
  enum_value->value = value;
  enum_value->name = g_strdup (name);
  enum_value->description = g_strdup (description);

  values->values[values->n_values] = enum_value;
  values->values[values->n_values + 1] = NULL;

  values->n_values += 1;

  return TRUE;
}

/* Функция создаёт новый параметр типа BOOLEAN. */
gboolean
hyscan_data_schema_builder_key_boolean_create (HyScanDataSchemaBuilder *builder,
                                               const gchar             *key_id,
                                               const gchar             *name,
                                               const gchar             *description,
                                               gboolean                 readonly,
                                               gboolean                 default_value)
{
  HyScanDataSchemaInternalKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  if (g_hash_table_contains (builder->priv->keys, key_id))
    return FALSE;

  if (!hyscan_data_schema_validate_id (key_id))
    return FALSE;

  key = g_new0 (HyScanDataSchemaInternalKey, 1);
  key->id = g_strdup (key_id);
  key->name = g_strdup (name);
  key->description = g_strdup (description);
  key->type = HYSCAN_DATA_SCHEMA_TYPE_BOOLEAN;
  key->enum_values = NULL;
  key->readonly = readonly;

  hyscan_data_schema_value_set_boolean (&key->default_value, default_value);
  hyscan_data_schema_value_set_boolean (&key->minimum_value, FALSE);
  hyscan_data_schema_value_set_boolean (&key->maximum_value, TRUE);
  hyscan_data_schema_value_set_boolean (&key->value_step, FALSE);

  return g_hash_table_insert (builder->priv->keys, key->id, key);
}

/* Функция создаёт новый параметр типа INTEGER. */
gboolean
hyscan_data_schema_builder_key_integer_create (HyScanDataSchemaBuilder *builder,
                                               const gchar             *key_id,
                                               const gchar             *name,
                                               const gchar             *description,
                                               gboolean                 readonly,
                                               gint64                   default_value,
                                               gint64                   minimum_value,
                                               gint64                   maximum_value,
                                               gint64                   value_step)
{
  HyScanDataSchemaInternalKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  if (g_hash_table_contains (builder->priv->keys, key_id))
    return FALSE;

  if (!hyscan_data_schema_validate_id (key_id))
    return FALSE;

  if ((minimum_value > maximum_value) ||
      (default_value < minimum_value) ||
      (default_value > maximum_value))
    {
      g_warning ("HyScanDataBuilder: default value out of range in key '%s'", key_id);
      return FALSE;
    }

  key = g_new0 (HyScanDataSchemaInternalKey, 1);
  key->id = g_strdup (key_id);
  key->name = g_strdup (name);
  key->description = g_strdup (description);
  key->type = HYSCAN_DATA_SCHEMA_TYPE_INTEGER;
  key->enum_values = NULL;
  key->readonly = readonly;

  hyscan_data_schema_value_set_integer (&key->default_value, default_value);
  hyscan_data_schema_value_set_integer (&key->minimum_value, minimum_value);
  hyscan_data_schema_value_set_integer (&key->maximum_value, maximum_value);
  hyscan_data_schema_value_set_integer (&key->value_step, value_step);

  return g_hash_table_insert (builder->priv->keys, key->id, key);
}

/* Функция создаёт новый параметр типа DOUBLE. */
gboolean
hyscan_data_schema_builder_key_double_create (HyScanDataSchemaBuilder *builder,
                                              const gchar             *key_id,
                                              const gchar             *name,
                                              const gchar             *description,
                                              gboolean                 readonly,
                                              gdouble                  default_value,
                                              gdouble                  minimum_value,
                                              gdouble                  maximum_value,
                                              gdouble                  value_step)
{
  HyScanDataSchemaInternalKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  if (g_hash_table_contains (builder->priv->keys, key_id))
    return FALSE;

  if (!hyscan_data_schema_validate_id (key_id))
    return FALSE;

  if ((minimum_value > maximum_value) ||
      (default_value < minimum_value) ||
      (default_value > maximum_value))
    {
      g_warning ("HyScanDataBuilder: default value out of range in key '%s'", key_id);
      return FALSE;
    }

  key = g_new0 (HyScanDataSchemaInternalKey, 1);
  key->id = g_strdup (key_id);
  key->name = g_strdup (name);
  key->description = g_strdup (description);
  key->type = HYSCAN_DATA_SCHEMA_TYPE_DOUBLE;
  key->enum_values = NULL;
  key->readonly = readonly;

  hyscan_data_schema_value_set_double (&key->default_value, default_value);
  hyscan_data_schema_value_set_double (&key->minimum_value, minimum_value);
  hyscan_data_schema_value_set_double (&key->maximum_value, maximum_value);
  hyscan_data_schema_value_set_double (&key->value_step, value_step);

  return g_hash_table_insert (builder->priv->keys, key->id, key);
}

/* Функция создаёт новый параметр типа STRING. */
gboolean
hyscan_data_schema_builder_key_string_create (HyScanDataSchemaBuilder *builder,
                                              const gchar             *key_id,
                                              const gchar             *name,
                                              const gchar             *description,
                                              gboolean                 readonly,
                                              const gchar             *default_value)
{
  HyScanDataSchemaInternalKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  if (g_hash_table_contains (builder->priv->keys, key_id))
    return FALSE;

  if (!hyscan_data_schema_validate_id (key_id))
    return FALSE;

  key = g_new0 (HyScanDataSchemaInternalKey, 1);
  key->id = g_strdup (key_id);
  key->name = g_strdup (name);
  key->description = g_strdup (description);
  key->type = HYSCAN_DATA_SCHEMA_TYPE_STRING;
  key->enum_values = NULL;
  key->readonly = readonly;

  hyscan_data_schema_value_set_string (&key->default_value, default_value);
  hyscan_data_schema_value_set_string (&key->minimum_value, NULL);
  hyscan_data_schema_value_set_string (&key->maximum_value, NULL);
  hyscan_data_schema_value_set_string (&key->value_step, NULL);

  return g_hash_table_insert (builder->priv->keys, key->id, key);
}

/* Функция создаёт новый параметр типа ENUM. */
gboolean
hyscan_data_schema_builder_key_enum_create (HyScanDataSchemaBuilder *builder,
                                            const gchar             *key_id,
                                            const gchar             *name,
                                            const gchar             *description,
                                            gboolean                 readonly,
                                            const gchar             *enum_id,
                                            gint64                   default_value)
{
  HyScanDataSchemaInternalKey *key;
  HyScanDataSchemaEnum *values;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  if (g_hash_table_contains (builder->priv->keys, key_id))
    return FALSE;

  if (!hyscan_data_schema_validate_id (key_id))
    return FALSE;

  values = g_hash_table_lookup (builder->priv->enums, enum_id);
  if (values == NULL)
    return FALSE;

  if (!hyscan_data_schema_check_enum (values, default_value))
    {
      g_warning ("HyScanDataBuilder: default value out of range in key '%s'", key_id);
      return FALSE;
    }

  key = g_new0 (HyScanDataSchemaInternalKey, 1);
  key->id = g_strdup (key_id);
  key->name = g_strdup (name);
  key->description = g_strdup (description);
  key->type = HYSCAN_DATA_SCHEMA_TYPE_ENUM;
  key->enum_values = values;
  key->readonly = readonly;

  hyscan_data_schema_value_set_integer (&key->default_value, default_value);
  hyscan_data_schema_value_set_integer (&key->minimum_value, 0);
  hyscan_data_schema_value_set_integer (&key->maximum_value, 0);
  hyscan_data_schema_value_set_integer (&key->value_step, 0);

  return g_hash_table_insert (builder->priv->keys, key->id, key);
}
