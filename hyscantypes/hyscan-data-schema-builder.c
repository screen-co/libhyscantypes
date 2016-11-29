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
                                       (GDestroyNotify)hyscan_data_schema_internal_key_free);

  /* Таблица enum типов и их значений. */
  priv->enums = g_hash_table_new_full (g_str_hash,
                                       g_str_equal,
                                       NULL,
                                       (GDestroyNotify)hyscan_data_schema_internal_enum_free);
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
  guint i;

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
  gboolean short_view = TRUE;
  gchar *indent;
  gchar **pathv;
  gchar *type;

  indent = g_malloc0 (4 + 2 * level + 1);
  memset (indent, ' ', 4 + 2 * level);

  pathv = g_strsplit (key->id, "/", -1);

  switch (key->type)
    {
    case HYSCAN_DATA_SCHEMA_KEY_BOOLEAN:
      type = "boolean";
      break;

    case HYSCAN_DATA_SCHEMA_KEY_INTEGER:
      type = "integer";
      break;

    case HYSCAN_DATA_SCHEMA_KEY_DOUBLE:
      type = "double";
      break;

    case HYSCAN_DATA_SCHEMA_KEY_STRING:
      type = "string";
      break;

    case HYSCAN_DATA_SCHEMA_KEY_ENUM:
      type = "enum";
      break;

    default:
      type = NULL;
      break;
    }

  if (type == NULL)
    return;

  /* Заголовок параметра. */
  if (key->type != HYSCAN_DATA_SCHEMA_KEY_ENUM)
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

  /* Рекомендуемый вид отображения. */
  if (key->view == HYSCAN_DATA_SCHEMA_VIEW_BIN)
    g_output_stream_printf (ostream, NULL, NULL, NULL, " view=\"bin\"");
  else if (key->view == HYSCAN_DATA_SCHEMA_VIEW_DEC)
    g_output_stream_printf (ostream, NULL, NULL, NULL, " view=\"dec\"");
  else if (key->view == HYSCAN_DATA_SCHEMA_VIEW_HEX)
    g_output_stream_printf (ostream, NULL, NULL, NULL, " view=\"hex\"");
  else if (key->view == HYSCAN_DATA_SCHEMA_VIEW_DATE)
    g_output_stream_printf (ostream, NULL, NULL, NULL, " view=\"date\"");
  else if (key->view == HYSCAN_DATA_SCHEMA_VIEW_SCHEMA)
    g_output_stream_printf (ostream, NULL, NULL, NULL, " view=\"schema\"");

  /* Атрибуты доступа к параметру. */
  if (key->access == HYSCAN_DATA_SCHEMA_ACCESS_READONLY)
    g_output_stream_printf (ostream, NULL, NULL, NULL, " access=\"readonly\"");
  else if (key->access == HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY)
    g_output_stream_printf (ostream, NULL, NULL, NULL, " access=\"writeonly\"");

  /* Описание параметра. */
  if (key->description != NULL)
    {
      short_view = FALSE;
      g_output_stream_printf (ostream, NULL, NULL, NULL, ">\n");
      g_output_stream_printf (ostream, NULL, NULL, NULL,
                              "%s  <description>%s</description>\n",
                              indent, key->description);
    }

  /* Дипазон допустимых значений и значение по умолчанию. */
  switch (key->type)
    {
    case HYSCAN_DATA_SCHEMA_KEY_BOOLEAN:
      {
        gboolean default_value = g_variant_get_boolean (key->default_value);

        if (default_value)
          {
            if (short_view)
              {
                short_view = FALSE;
                g_output_stream_printf (ostream, NULL, NULL, NULL, ">\n");
              }
            g_output_stream_printf (ostream, NULL, NULL, NULL,
                                    "%s  <default>true</default>\n",
                                    indent);
          }
      }
      break;

    case HYSCAN_DATA_SCHEMA_KEY_INTEGER:
      {
        gint64 default_value = g_variant_get_int64 (key->default_value);

        if (short_view && ((default_value != 0) || (key->value_step != NULL)))
          {
            short_view = FALSE;
            g_output_stream_printf (ostream, NULL, NULL, NULL, ">\n");
          }

        if (default_value != 0)
          {
            g_output_stream_printf (ostream, NULL, NULL, NULL,
                                    "%s  <default>%" G_GINT64_FORMAT "</default>\n",
                                    indent, default_value);
          }

        if (key->value_step != NULL)
          {
            gint64 minimum_value = g_variant_get_int64 (key->minimum_value);
            gint64 maximum_value = g_variant_get_int64 (key->maximum_value);
            gint64 value_step = g_variant_get_int64 (key->value_step);

            g_output_stream_printf (ostream, NULL, NULL, NULL, "%s  <range", indent);

            if (minimum_value != G_MININT64)
              {
                g_output_stream_printf (ostream, NULL, NULL, NULL,
                                        " min=\"%" G_GINT64_FORMAT "\"", minimum_value);
              }

            if (maximum_value != G_MAXINT64)
              {
                g_output_stream_printf (ostream, NULL, NULL, NULL,
                                        " max=\"%" G_GINT64_FORMAT "\"", maximum_value);
              }

            if (value_step != 1)
              {
                g_output_stream_printf (ostream, NULL, NULL, NULL,
                                        " step=\"%" G_GINT64_FORMAT "\"", value_step);
              }

            g_output_stream_printf (ostream, NULL, NULL, NULL, "/>\n");
          }
      }
      break;

    case HYSCAN_DATA_SCHEMA_KEY_DOUBLE:
      {
        gdouble default_value = g_variant_get_double (key->default_value);

        if (short_view && ((default_value != 0.0) || (key->value_step != NULL)))
          {
            short_view = FALSE;
            g_output_stream_printf (ostream, NULL, NULL, NULL, ">\n");
          }

        if (default_value != 0.0)
          {
            gchar value_str [G_ASCII_DTOSTR_BUF_SIZE];

            g_ascii_dtostr (value_str, G_ASCII_DTOSTR_BUF_SIZE, default_value);
            g_output_stream_printf (ostream, NULL, NULL, NULL,
                                    "%s  <default>%s</default>\n",
                                    indent, value_str);
          }

        if (key->value_step != NULL)
          {
            gdouble minimum_value = g_variant_get_double (key->minimum_value);
            gdouble maximum_value = g_variant_get_double (key->maximum_value);
            gdouble value_step = g_variant_get_double (key->value_step);
            gchar value_str [G_ASCII_DTOSTR_BUF_SIZE];

            g_output_stream_printf (ostream, NULL, NULL, NULL, "%s  <range", indent);

            if (minimum_value != -G_MAXDOUBLE)
              {
                g_ascii_dtostr (value_str, G_ASCII_DTOSTR_BUF_SIZE, minimum_value);
                g_output_stream_printf (ostream, NULL, NULL, NULL, " min=\"%s\"", value_str);
              }

            if (maximum_value != G_MAXDOUBLE)
              {
                g_ascii_dtostr (value_str, G_ASCII_DTOSTR_BUF_SIZE, maximum_value);
                g_output_stream_printf (ostream, NULL, NULL, NULL, " max=\"%s\"", value_str);
              }

            if (value_step != 1.0)
              {
                g_ascii_dtostr (value_str, G_ASCII_DTOSTR_BUF_SIZE, value_step);
                g_output_stream_printf (ostream, NULL, NULL, NULL, " step=\"%s\"", value_str);
              }

            g_output_stream_printf (ostream, NULL, NULL, NULL, "/>\n");
          }
      }
      break;

    case HYSCAN_DATA_SCHEMA_KEY_STRING:
      {
        if (key->default_value != NULL)
          {
            if (short_view)
              {
                short_view = FALSE;
                g_output_stream_printf (ostream, NULL, NULL, NULL, ">\n");
              }
            g_output_stream_printf (ostream, NULL, NULL, NULL,
                                    "%s  <default>%s</default>\n",
                                    indent, g_variant_get_string (key->default_value, NULL));
          }
      }
      break;

    case HYSCAN_DATA_SCHEMA_KEY_ENUM:
      {
        gint64 default_value = g_variant_get_int64 (key->default_value);

        if (default_value != 0)
          {
            if (short_view)
              {
                short_view = FALSE;
                g_output_stream_printf (ostream, NULL, NULL, NULL, ">\n");
              }
            g_output_stream_printf (ostream, NULL, NULL, NULL,
                                    "%s  <default>%" G_GINT64_FORMAT "</default>\n",
                                    indent, default_value);
          }
      }
      break;

    default:
      break;
    }

  if (short_view)
    g_output_stream_printf (ostream, NULL, NULL, NULL, "/>\n");
  else
    g_output_stream_printf (ostream, NULL, NULL, NULL, "%s</key>\n", indent);

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
  nodes = hyscan_data_schema_internal_node_new ("");
  g_hash_table_iter_init (&iter, priv->keys);
  while (g_hash_table_iter_next (&iter, &key, NULL))
    {
      gchar *key_id = key;
      HyScanDataSchemaInternalKey *key = g_hash_table_lookup (priv->keys, key_id);

      hyscan_data_schema_internal_node_insert_key (nodes, key->id, key->name, key->description,
                                                   key->type, key->view, key->access);
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

  if (!hyscan_data_schema_internal_validate_name (enum_id))
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
                                               gboolean                 default_value)
{
  HyScanDataSchemaInternalKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  if (g_hash_table_contains (builder->priv->keys, key_id))
    return FALSE;

  if (!hyscan_data_schema_internal_validate_id (key_id))
    return FALSE;

  key = g_new0 (HyScanDataSchemaInternalKey, 1);
  key->id = g_strdup (key_id);
  key->name = g_strdup (name);
  key->description = g_strdup (description);
  key->type = HYSCAN_DATA_SCHEMA_KEY_BOOLEAN;
  key->default_value = g_variant_new_boolean (default_value);

  return g_hash_table_insert (builder->priv->keys, key->id, key);
}

/* Функция создаёт новый параметр типа INTEGER. */
gboolean
hyscan_data_schema_builder_key_integer_create (HyScanDataSchemaBuilder *builder,
                                               const gchar             *key_id,
                                               const gchar             *name,
                                               const gchar             *description,
                                               gint64                   default_value)
{
  HyScanDataSchemaInternalKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  if (g_hash_table_contains (builder->priv->keys, key_id))
    return FALSE;

  if (!hyscan_data_schema_internal_validate_id (key_id))
    return FALSE;

  key = g_new0 (HyScanDataSchemaInternalKey, 1);
  key->id = g_strdup (key_id);
  key->name = g_strdup (name);
  key->description = g_strdup (description);
  key->type = HYSCAN_DATA_SCHEMA_KEY_INTEGER;
  key->default_value = g_variant_new_int64 (default_value);

  return g_hash_table_insert (builder->priv->keys, key->id, key);
}

/* Функция создаёт новый параметр типа DOUBLE. */
gboolean
hyscan_data_schema_builder_key_double_create (HyScanDataSchemaBuilder *builder,
                                              const gchar             *key_id,
                                              const gchar             *name,
                                              const gchar             *description,
                                              gdouble                  default_value)
{
  HyScanDataSchemaInternalKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  if (g_hash_table_contains (builder->priv->keys, key_id))
    return FALSE;

  if (!hyscan_data_schema_internal_validate_id (key_id))
    return FALSE;

  key = g_new0 (HyScanDataSchemaInternalKey, 1);
  key->id = g_strdup (key_id);
  key->name = g_strdup (name);
  key->description = g_strdup (description);
  key->type = HYSCAN_DATA_SCHEMA_KEY_DOUBLE;
  key->default_value = g_variant_new_double (default_value);

  return g_hash_table_insert (builder->priv->keys, key->id, key);
}

/* Функция создаёт новый параметр типа STRING. */
gboolean
hyscan_data_schema_builder_key_string_create (HyScanDataSchemaBuilder *builder,
                                              const gchar             *key_id,
                                              const gchar             *name,
                                              const gchar             *description,
                                              const gchar             *default_value)
{
  HyScanDataSchemaInternalKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  if (g_hash_table_contains (builder->priv->keys, key_id))
    return FALSE;

  if (!hyscan_data_schema_internal_validate_id (key_id))
    return FALSE;

  key = g_new0 (HyScanDataSchemaInternalKey, 1);
  key->id = g_strdup (key_id);
  key->name = g_strdup (name);
  key->description = g_strdup (description);
  key->type = HYSCAN_DATA_SCHEMA_KEY_STRING;
  if (default_value != NULL)
    key->default_value = g_variant_new_string (default_value);

  return g_hash_table_insert (builder->priv->keys, key->id, key);
}

/* Функция создаёт новый параметр типа ENUM. */
gboolean
hyscan_data_schema_builder_key_enum_create (HyScanDataSchemaBuilder *builder,
                                            const gchar             *key_id,
                                            const gchar             *name,
                                            const gchar             *description,
                                            const gchar             *enum_id,
                                            gint64                   default_value)
{
  HyScanDataSchemaInternalKey *key;
  HyScanDataSchemaEnum *values;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  if (g_hash_table_contains (builder->priv->keys, key_id))
    return FALSE;

  if (!hyscan_data_schema_internal_validate_id (key_id))
    return FALSE;

  values = g_hash_table_lookup (builder->priv->enums, enum_id);
  if (values == NULL)
    return FALSE;

  if (!hyscan_data_schema_internal_enum_check (values, default_value))
    {
      g_warning ("HyScanDataBuilder: default value out of range in key '%s'", key_id);
      return FALSE;
    }

  key = g_new0 (HyScanDataSchemaInternalKey, 1);
  key->id = g_strdup (key_id);
  key->name = g_strdup (name);
  key->description = g_strdup (description);
  key->type = HYSCAN_DATA_SCHEMA_KEY_ENUM;
  key->enum_values = values;
  key->default_value = g_variant_new_int64 (default_value);

  return g_hash_table_insert (builder->priv->keys, key->id, key);
}

/* Функция задаёт рекомендуемый вид отображения параметра. */
gboolean
hyscan_data_schema_builder_key_set_view (HyScanDataSchemaBuilder  *builder,
                                         const gchar              *key_id,
                                         HyScanDataSchemaViewType  view)
{
  HyScanDataSchemaInternalKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  key = g_hash_table_lookup (builder->priv->keys, key_id);
  if (key == NULL)
    return FALSE;

  key->view = view;

  return TRUE;
}

/* Функция задаёт атрибут доступа к параметру. */
gboolean
hyscan_data_schema_builder_key_set_access (HyScanDataSchemaBuilder   *builder,
                                           const gchar               *key_id,
                                           HyScanDataSchemaKeyAccess  access)
{
  HyScanDataSchemaInternalKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  key = g_hash_table_lookup (builder->priv->keys, key_id);
  if (key == NULL)
    return FALSE;

  key->access = access;

  return TRUE;
}

/* Функция задаёт диапазон допустимых значений и рекомендуемый шаг изменения значения параметра. */
gboolean
hyscan_data_schema_builder_key_integer_range (HyScanDataSchemaBuilder *builder,
                                              const gchar             *key_id,
                                              gint64                   minimum_value,
                                              gint64                   maximum_value,
                                              gint64                   value_step)
{
  HyScanDataSchemaInternalKey *key;
  gint64 default_value;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  key = g_hash_table_lookup (builder->priv->keys, key_id);
  if (key == NULL)
    return FALSE;

  if (key->type != HYSCAN_DATA_SCHEMA_KEY_INTEGER)
    return FALSE;

  default_value = g_variant_get_int64 (key->default_value);
  if ((minimum_value > maximum_value) ||
      (default_value < minimum_value) ||
      (default_value > maximum_value))
    {
      g_warning ("HyScanDataBuilder: default value out of range in key '%s'", key_id);
      return FALSE;
    }

  key->minimum_value = g_variant_new_int64 (minimum_value);
  key->maximum_value = g_variant_new_int64 (maximum_value);
  key->value_step = g_variant_new_int64 (value_step);

  return TRUE;
}

/* Функция задаёт диапазон допустимых значений и рекомендуемый шаг изменения значения параметра. */
gboolean
hyscan_data_schema_builder_key_double_range (HyScanDataSchemaBuilder *builder,
                                             const gchar             *key_id,
                                             gdouble                  minimum_value,
                                             gdouble                  maximum_value,
                                             gdouble                  value_step)
{
  HyScanDataSchemaInternalKey *key;
  gdouble default_value;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  key = g_hash_table_lookup (builder->priv->keys, key_id);
  if (key == NULL)
    return FALSE;

  if (key->type != HYSCAN_DATA_SCHEMA_KEY_DOUBLE)
    return FALSE;

  default_value = g_variant_get_double (key->default_value);
  if ((minimum_value > maximum_value) ||
      (default_value < minimum_value) ||
      (default_value > maximum_value))
    {
      g_warning ("HyScanDataBuilder: default value out of range in key '%s'", key_id);
      return FALSE;
    }

  key->minimum_value = g_variant_new_double (minimum_value);
  key->maximum_value = g_variant_new_double (maximum_value);
  key->value_step = g_variant_new_double (value_step);

  return TRUE;
}
