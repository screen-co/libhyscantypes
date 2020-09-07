/* hyscan-data-schema-builder.c
 *
 * Copyright 2016-2019 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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

/**
 * SECTION: hyscan-data-schema-builder
 * @Short_description: класс генерации схем данных HyScan
 * @Title: HyScanDataSchemaBuilder
 *
 * Класс предназначен для автоматизированного создания схем данных. Подробное
 * описание схем данных приведено в разделе #HyScanDataSchema.
 *
 * Создание объекта производится функциями #hyscan_data_schema_builder_new,
 * #hyscan_data_schema_builder_new_with_version,
 * #hyscan_data_schema_builder_new_with_gettext и
 * #hyscan_data_schema_builder_new_full.
 *
 * Название и описание узла можно задать с помощью функции
 * #hyscan_data_schema_builder_node_set_name.
 *
 * Списки возможных значений параметров типа ENUM создаются с помощью функций
 * #hyscan_data_schema_builder_enum_create и #hyscan_data_schema_builder_enum_value_create.
 *
 * Параметры создаются с помощью следующих функций:
 *
 * - #hyscan_data_schema_builder_key_boolean_create - для параметров типа BOOLEAN;
 * - #hyscan_data_schema_builder_key_integer_create - для параметров типа INTEGER;
 * - #hyscan_data_schema_builder_key_double_create - для параметров типа DOUBLE;
 * - #hyscan_data_schema_builder_key_string_create - для параметров типа STRING;
 * - #hyscan_data_schema_builder_key_enum_create - для параметров типа ENUM.
 *
 * Для любого параметра можно задать рекомендуемый вид отображения функцией
 * #hyscan_data_schema_builder_key_set_view и атрибут доступа функцией
 * #hyscan_data_schema_builder_key_set_access.
 *
 * Для параметров типа INTEGER и DOUBLE можно задать диапазон допустимых значений и
 * рекомендуемый шаг изменения значений. Для этих целей используются функции
 * #hyscan_data_schema_builder_key_integer_range и #hyscan_data_schema_builder_key_double_range.
 *
 * Функция #hyscan_data_schema_builder_schema_join может использоваться для добавления
 * параметров их другой схемы в создаваемую.
 *
 * Для получения XML описания схемы и её характеристик, после определения
 * параметров, можно использовать функции #hyscan_data_schema_builder_get_data,
 * #hyscan_data_schema_builder_get_id, #hyscan_data_schema_builder_get_version и
 * #hyscan_data_schema_builder_get_text_domain.
 *
 * Для создания схемы данных предназначена функция
 * #hyscan_data_schema_builder_get_schema.
 *
 * Данный класс не является потокобезопасным.
 */

#include "hyscan-data-schema-builder.h"
#include "hyscan-data-schema-internal.h"

#include <gio/gio.h>
#include <string.h>

enum
{
  PROP_O,
  PROP_SCHEMA_ID,
  PROP_SCHEMA_VERSION,
  PROP_GETTEXT_DOMAIN
};

struct _HyScanDataSchemaBuilderPrivate
{
  gchar                       *schema_id;              /* Идентификатор схемы. */
  gint64                       schema_version;         /* Версия схемы. */
  gchar                       *gettext_domain;         /* Домен gettext. */

  GHashTable                  *keys;                   /* Список параметров. */
  HyScanDataSchemaNode        *nodes;                  /* Список узлов. */
  GHashTable                  *enums;                  /* Список значений перечисляемых типов. */
};

static void      hyscan_data_schema_builder_set_property         (GObject                     *object,
                                                                  guint                        prop_id,
                                                                  const GValue                *value,
                                                                  GParamSpec                  *pspec);
static void      hyscan_data_schema_builder_object_constructed   (GObject                     *object);
static void      hyscan_data_schema_builder_object_finalize      (GObject                     *object);

static void      hyscan_data_schema_builder_dump_enum            (GOutputStream               *ostream,
                                                                  const gchar                 *enum_id,
                                                                  GList                       *values);
static void      hyscan_data_schema_builder_dump_node            (GOutputStream               *ostream,
                                                                  GHashTable                  *keys,
                                                                  HyScanDataSchemaNode        *node,
                                                                  guint                        level);
static void      hyscan_data_schema_builder_dump_key             (GOutputStream               *ostream,
                                                                  HyScanDataSchemaInternalKey *ikey,
                                                                  gint                         level);

static gboolean  hyscan_data_schema_builder_join_node            (HyScanDataSchemaBuilder     *builder,
                                                                  const gchar                 *dst_root,
                                                                  HyScanDataSchema            *schema,
                                                                  const HyScanDataSchemaNode  *node);

static gboolean  hyscan_data_schema_builder_join_key             (HyScanDataSchemaBuilder     *builder,
                                                                  const gchar                 *dst_root,
                                                                  HyScanDataSchema            *schema,
                                                                  const HyScanDataSchemaKey   *key);

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

  g_object_class_install_property (object_class, PROP_SCHEMA_VERSION,
    g_param_spec_int64 ("schema-version", "SchemaVersion", "Data schema version",
                        0, G_MAXINT64, 0,
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

    case PROP_SCHEMA_VERSION:
      priv->schema_version = g_value_get_int64 (value);
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

  /* Список узлов. */
  priv->nodes = hyscan_data_schema_node_new ("/", NULL, NULL, NULL, NULL);

  /* Таблица enum типов и их значений. */
  priv->enums = g_hash_table_new_full (g_str_hash,
                                       g_str_equal,
                                       g_free,
                                       (GDestroyNotify)hyscan_data_schema_internal_enum_free);
}

static void
hyscan_data_schema_builder_object_finalize (GObject *object)
{
  HyScanDataSchemaBuilder *data_schema_builder = HYSCAN_DATA_SCHEMA_BUILDER (object);
  HyScanDataSchemaBuilderPrivate *priv = data_schema_builder->priv;

  g_hash_table_unref (priv->enums);
  hyscan_data_schema_node_free (priv->nodes);
  g_hash_table_unref (priv->keys);

  g_free (priv->schema_id);
  g_free (priv->gettext_domain);

  G_OBJECT_CLASS (hyscan_data_schema_builder_parent_class)->finalize (object);
}

/* Функция формирует XML описание enum значений. */
static void
hyscan_data_schema_builder_dump_enum (GOutputStream *ostream,
                                      const gchar   *enum_id,
                                      GList         *values)
{
  g_output_stream_printf (ostream, NULL, NULL, NULL,
                          "  <enum id=\"%s\">\n",
                          enum_id);

  while (values != NULL)
    {
      HyScanDataSchemaEnumValue *value = values->data;

      if (value->description == NULL)
        {
          g_output_stream_printf (ostream, NULL, NULL, NULL,
                                  "    <value id=\"%s\" name=\"%s\" value=\"%" G_GINT64_FORMAT "\"/>\n",
                                  value->id, value->name, value->value);
        }
      else
        {
          g_output_stream_printf (ostream, NULL, NULL, NULL,
                                  "    <value id=\"%s\" name=\"%s\" value=\"%" G_GINT64_FORMAT "\">\n",
                                  value->id, value->name, value->value);

          g_output_stream_printf (ostream, NULL, NULL, NULL,
                                  "      <description>%s</description>\n",
                                  value->description);

          g_output_stream_printf (ostream, NULL, NULL, NULL,
                                  "    </value>\n");
        }

      values = values->next;
    }

  g_output_stream_printf (ostream, NULL, NULL, NULL,
                          "  </enum>\n");
}

/* Функция формирует XML описание узла с параметрами. */
static void
hyscan_data_schema_builder_dump_node (GOutputStream        *ostream,
                                      GHashTable           *keys,
                                      HyScanDataSchemaNode *node,
                                      guint                 level)
{
  gchar *indent;
  gchar **pathv;
  GList *list;

  indent = g_malloc0 (2 + 2 * level + 1);
  memset (indent, ' ', 2 + 2 * level);

  pathv = g_strsplit (node->path, "/", -1);
  if (g_strv_length (pathv) < level)
    return;

  if (level > 0)
    {
      g_output_stream_printf (ostream, NULL, NULL, NULL,
                              "%s<node id=\"%s\"",
                              indent, pathv[level]);

      if (node->name != NULL)
        g_output_stream_printf (ostream, NULL, NULL, NULL, " name=\"%s\"", node->name);

      g_output_stream_printf (ostream, NULL, NULL, NULL, ">\n");

      if (node->description != NULL)
        {
          g_output_stream_printf (ostream, NULL, NULL, NULL,
                                  "%s  <description>%s</description>\n",
                                  indent, node->description);
        }
    }

  /* Рекурсивное описание всех узлов. */
  list = node->nodes;
  while (list != NULL)
    {
      hyscan_data_schema_builder_dump_node (ostream, keys, list->data, level + 1);
      list = list->next;
    }

  /* Описание всех параметров. */
  list = node->keys;
  while (list != NULL)
    {
      HyScanDataSchemaInternalKey *ikey;
      ikey = g_hash_table_lookup (keys, ((HyScanDataSchemaKey*)(list->data))->id);
      hyscan_data_schema_builder_dump_key (ostream, ikey, level);
      list = list->next;
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
                                     HyScanDataSchemaInternalKey *ikey,
                                     gint                         level)
{
  gboolean short_view = TRUE;
  gchar *indent;
  gchar **pathv;
  gchar *type;

  indent = g_malloc0 (4 + 2 * level + 1);
  memset (indent, ' ', 4 + 2 * level);

  pathv = g_strsplit (ikey->id, "/", -1);

  switch (ikey->type)
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
  if (ikey->type != HYSCAN_DATA_SCHEMA_KEY_ENUM)
    {
      g_output_stream_printf (ostream, NULL, NULL, NULL,
                              "%s<key id=\"%s\" name=\"%s\" type=\"%s\"",
                              indent, pathv[level+1], ikey->name, type);
    }
  else
    {
      g_output_stream_printf (ostream, NULL, NULL, NULL,
                              "%s<key id=\"%s\" name=\"%s\" enum=\"%s\"",
                              indent, pathv[level+1], ikey->name, ikey->enum_id);
    }

  /* Рекомендуемый вид отображения. */
  if (ikey->view == HYSCAN_DATA_SCHEMA_VIEW_BIN)
    g_output_stream_printf (ostream, NULL, NULL, NULL, " view=\"bin\"");
  else if (ikey->view == HYSCAN_DATA_SCHEMA_VIEW_DEC)
    g_output_stream_printf (ostream, NULL, NULL, NULL, " view=\"dec\"");
  else if (ikey->view == HYSCAN_DATA_SCHEMA_VIEW_HEX)
    g_output_stream_printf (ostream, NULL, NULL, NULL, " view=\"hex\"");
  else if (ikey->view == HYSCAN_DATA_SCHEMA_VIEW_DATE)
    g_output_stream_printf (ostream, NULL, NULL, NULL, " view=\"date\"");
  else if (ikey->view == HYSCAN_DATA_SCHEMA_VIEW_TIME)
    g_output_stream_printf (ostream, NULL, NULL, NULL, " view=\"time\"");
  else if (ikey->view == HYSCAN_DATA_SCHEMA_VIEW_DATE_TIME)
    g_output_stream_printf (ostream, NULL, NULL, NULL, " view=\"datetime\"");
  else if (ikey->view == HYSCAN_DATA_SCHEMA_VIEW_RGB)
    g_output_stream_printf (ostream, NULL, NULL, NULL, " view=\"rgb\"");
  else if (ikey->view == HYSCAN_DATA_SCHEMA_VIEW_RGBA)
    g_output_stream_printf (ostream, NULL, NULL, NULL, " view=\"rgba\"");
  else if (ikey->view == HYSCAN_DATA_SCHEMA_VIEW_FILE)
    g_output_stream_printf (ostream, NULL, NULL, NULL, " view=\"file\"");
  else if (ikey->view == HYSCAN_DATA_SCHEMA_VIEW_DIR)
    g_output_stream_printf (ostream, NULL, NULL, NULL, " view=\"dir\"");
  else if (ikey->view == HYSCAN_DATA_SCHEMA_VIEW_SCHEMA)
    g_output_stream_printf (ostream, NULL, NULL, NULL, " view=\"schema\"");

  /* Атрибуты доступа к параметру. */
  if (ikey->access != HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT)
    {
      g_output_stream_printf (ostream, NULL, NULL, NULL, " access=\"");

      if (ikey->access & HYSCAN_DATA_SCHEMA_ACCESS_READ)
        g_output_stream_printf (ostream, NULL, NULL, NULL, "r");
      if (ikey->access & HYSCAN_DATA_SCHEMA_ACCESS_WRITE)
        g_output_stream_printf (ostream, NULL, NULL, NULL, "w");
      if (ikey->access & HYSCAN_DATA_SCHEMA_ACCESS_HIDDEN)
        g_output_stream_printf (ostream, NULL, NULL, NULL, "h");

      g_output_stream_printf (ostream, NULL, NULL, NULL, "\"");
    }

  /* Описание параметра. */
  if (ikey->description != NULL)
    {
      short_view = FALSE;
      g_output_stream_printf (ostream, NULL, NULL, NULL, ">\n");
      g_output_stream_printf (ostream, NULL, NULL, NULL,
                              "%s  <description>%s</description>\n",
                              indent, ikey->description);
    }

  /* Дипазон допустимых значений и значение по умолчанию. */
  switch (ikey->type)
    {
    case HYSCAN_DATA_SCHEMA_KEY_BOOLEAN:
      {
        gboolean default_value = g_variant_get_boolean (ikey->default_value);

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
        gint64 default_value = 0;
        gint64 minimum_value = G_MININT64;
        gint64 maximum_value = G_MAXINT64;
        gint64 value_step = 1;

        if (ikey->default_value != NULL)
          default_value = g_variant_get_int64 (ikey->default_value);
        if (ikey->minimum_value != NULL)
          minimum_value = g_variant_get_int64 (ikey->minimum_value);
        if (ikey->maximum_value != NULL)
          maximum_value = g_variant_get_int64 (ikey->maximum_value);
        if (ikey->value_step != NULL)
          value_step = g_variant_get_int64 (ikey->value_step);

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

        if ((minimum_value != G_MININT64) || (maximum_value != G_MAXINT64) || (value_step != 1))
          {
            if (short_view)
              {
                short_view = FALSE;
                g_output_stream_printf (ostream, NULL, NULL, NULL, ">\n");
              }

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
        gdouble default_value = 0.0;
        gdouble minimum_value = -G_MAXDOUBLE;
        gdouble maximum_value = G_MAXDOUBLE;
        gdouble value_step = 1.0;

        if (ikey->default_value != NULL)
          default_value = g_variant_get_double (ikey->default_value);
        if (ikey->minimum_value != NULL)
          minimum_value = g_variant_get_double (ikey->minimum_value);
        if (ikey->maximum_value != NULL)
          maximum_value = g_variant_get_double (ikey->maximum_value);
        if (ikey->value_step != NULL)
          value_step = g_variant_get_double (ikey->value_step);

        if (default_value != 0.0)
          {
            gchar value_str [G_ASCII_DTOSTR_BUF_SIZE];

            if (short_view)
              {
                short_view = FALSE;
                g_output_stream_printf (ostream, NULL, NULL, NULL, ">\n");
              }

            g_ascii_dtostr (value_str, G_ASCII_DTOSTR_BUF_SIZE, default_value);
            g_output_stream_printf (ostream, NULL, NULL, NULL,
                                    "%s  <default>%s</default>\n",
                                    indent, value_str);
          }

        if ((minimum_value != -G_MAXDOUBLE) || (maximum_value != G_MAXDOUBLE) || (value_step != 1.0))
          {
            gchar value_str [G_ASCII_DTOSTR_BUF_SIZE];

            if (short_view)
              {
                short_view = FALSE;
                g_output_stream_printf (ostream, NULL, NULL, NULL, ">\n");
              }

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
        if (ikey->default_value != NULL)
          {
            if (short_view)
              {
                short_view = FALSE;
                g_output_stream_printf (ostream, NULL, NULL, NULL, ">\n");
              }

            g_output_stream_printf (ostream, NULL, NULL, NULL,
                                    "%s  <default>%s</default>\n",
                                    indent, g_variant_get_string (ikey->default_value, NULL));
          }
      }
      break;

    case HYSCAN_DATA_SCHEMA_KEY_ENUM:
      {
        gint64 default_value = g_variant_get_int64 (ikey->default_value);

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

/* Функция добавляет описание узла и параметров в текущий путь. */
static gboolean
hyscan_data_schema_builder_join_node (HyScanDataSchemaBuilder    *builder,
                                      const gchar                *dst_root,
                                      HyScanDataSchema           *schema,
                                      const HyScanDataSchemaNode *node)
{
  GList *keys = node->keys;
  GList *nodes = node->nodes;
  gboolean status = TRUE;

  /* Параметры узла. */
  if (((node->name != NULL) || (node->description != NULL)) &&
      !hyscan_data_schema_builder_node_set_name (builder, dst_root, node->name, node->description))
    {
      return FALSE;
    }

  /* Добавление параметров. */
  while (status && (keys != NULL))
    {
      status = hyscan_data_schema_builder_join_key (builder, dst_root, schema, keys->data);

      keys = g_list_next (keys);
    }

  /* Добавление дочерних узлов. */
  while (status && (nodes != NULL))
    {
      const gchar *id;
      gchar *dst_path;

      node = nodes->data;

      /* Идентификатор узла - крайний компонент пути. */
      id = node->path + strlen (node->path);
      while (id >= node->path)
        {
          if (*id == '/')
            {
              id++;
              break;
            }
          id--;
        }

      /* Целевой путь. Если идентификатор узла пустой, это корневой узел. */
      dst_path = hyscan_data_schema_internal_make_path (dst_root, id);
      status = hyscan_data_schema_builder_join_node (builder, dst_path, schema, nodes->data);
      g_free (dst_path);

      nodes = g_list_next (nodes);
    }

  return status;
}

/* Функция добавляет параметр в текущий путь. */
static gboolean
hyscan_data_schema_builder_join_key (HyScanDataSchemaBuilder   *builder,
                                     const gchar               *dst_root,
                                     HyScanDataSchema          *schema,
                                     const HyScanDataSchemaKey *key)
{
  GVariant *default_value;
  GVariant *minimum_value;
  GVariant *maximum_value;
  GVariant *value_step;
  const gchar *id;
  gchar *key_id;

  gboolean status = TRUE;

  /* Идентификатор параметра - крайний компонент пути. */
  id = key->id + strlen (key->id);
  while (id >= key->id)
    {
      if (*id == '/')
        {
          id++;
          break;
        }
      id--;
    }

  /* Свойства параметра. */
  key_id = hyscan_data_schema_internal_make_path (dst_root, id);
  default_value = hyscan_data_schema_key_get_default (schema, key->id);
  minimum_value = hyscan_data_schema_key_get_minimum (schema, key->id);
  maximum_value = hyscan_data_schema_key_get_maximum (schema, key->id);
  value_step = hyscan_data_schema_key_get_step (schema, key->id);

  switch (key->type)
    {
    case HYSCAN_DATA_SCHEMA_KEY_BOOLEAN:
      {
        status = hyscan_data_schema_builder_key_boolean_create (builder, key_id,
                                                                key->name, key->description,
                                                                g_variant_get_boolean (default_value));
      }
      break;

    case HYSCAN_DATA_SCHEMA_KEY_INTEGER:
      {
        status = hyscan_data_schema_builder_key_integer_create (builder, key_id,
                                                                key->name, key->description,
                                                                g_variant_get_int64 (default_value));
        if (status)
          {
            status = hyscan_data_schema_builder_key_integer_range (builder, key_id,
                                                                   g_variant_get_int64 (minimum_value),
                                                                   g_variant_get_int64 (maximum_value),
                                                                   g_variant_get_int64 (value_step));
          }
      }
      break;

    case HYSCAN_DATA_SCHEMA_KEY_DOUBLE:
      {
        status = hyscan_data_schema_builder_key_double_create  (builder, key_id,
                                                                key->name, key->description,
                                                                g_variant_get_double (default_value));
        if (status)
          {
            status = hyscan_data_schema_builder_key_double_range  (builder, key_id,
                                                                   g_variant_get_double (minimum_value),
                                                                   g_variant_get_double (maximum_value),
                                                                   g_variant_get_double (value_step));
          }
      }
      break;

    case HYSCAN_DATA_SCHEMA_KEY_STRING:
      {
        status = hyscan_data_schema_builder_key_string_create (builder, key_id, key->name, key->description,
                                                               (default_value == NULL) ?
                                                               NULL : g_variant_get_string (default_value, NULL));

      }
      break;

    case HYSCAN_DATA_SCHEMA_KEY_ENUM:
      {
        const gchar *enum_id;

        /* Создаём список вариантов возможных значений для ENUM параметра,
         * если он еще не был создан. */
        enum_id = hyscan_data_schema_key_get_enum_id (schema, key->id);
        if (hyscan_data_schema_builder_enum_create (builder, enum_id))
          {
            GList *values, *cur_value;

            values = hyscan_data_schema_enum_get_values (schema, enum_id);
            for (cur_value = values; cur_value != NULL; cur_value = cur_value->next)
              {
                HyScanDataSchemaEnumValue *value = cur_value->data;
                status = hyscan_data_schema_builder_enum_value_create (builder, enum_id,
                                                                       value->value,
                                                                       value->id,
                                                                       value->name,
                                                                       value->description);

                if (!status)
                  break;
              }
            g_list_free (values);
          }
        else
          {
            status = TRUE;
          }

        if (status)
          {
            status = hyscan_data_schema_builder_key_enum_create (builder, key_id,
                                                                 key->name, key->description, enum_id,
                                                                 g_variant_get_int64 (default_value));
          }
      }
      break;

    default:
      break;
    }

  if (status)
    status = hyscan_data_schema_builder_key_set_view (builder, key_id, key->view);
  if (status)
    status = hyscan_data_schema_builder_key_set_access (builder, key_id, key->access);

  g_clear_pointer (&default_value, g_variant_unref);
  g_clear_pointer (&minimum_value, g_variant_unref);
  g_clear_pointer (&maximum_value, g_variant_unref);
  g_clear_pointer (&value_step, g_variant_unref);

  g_free (key_id);

  return status;
}

/**
 * hyscan_data_schema_builder_new:
 * @schema_id: идентификатор создаваемой схемы
 *
 * Функция создаёт новый объект #HyScanDataSchemaBuilder.
 *
 * Returns: #HyScanDataSchemaBuilder. Для удаления #g_object_unref.
 */
HyScanDataSchemaBuilder *
hyscan_data_schema_builder_new (const gchar *schema_id)
{
  return hyscan_data_schema_builder_new_full (schema_id, 0, NULL);
}

/**
 * hyscan_data_schema_builder_new_with_version:
 * @schema_id: идентификатор создаваемой схемы
 * @schema_version: версия создаваемой схемы
 *
 * Функция создаёт новый объект #HyScanDataSchemaBuilder с возможностью перевода.
 *
 * Returns: #HyScanDataSchemaBuilder. Для удаления #g_object_unref.
 */
HyScanDataSchemaBuilder *
hyscan_data_schema_builder_new_with_version (const gchar *schema_id,
                                             gint64       schema_version)
{
  return hyscan_data_schema_builder_new_full (schema_id, schema_version, NULL);
}

/**
 * hyscan_data_schema_builder_new_with_gettext:
 * @schema_id: идентификатор создаваемой схемы
 * @gettext_domain: название домена перевода gettext
 *
 * Функция создаёт новый объект #HyScanDataSchemaBuilder с возможностью перевода.
 *
 * Returns: #HyScanDataSchemaBuilder. Для удаления #g_object_unref.
 */
HyScanDataSchemaBuilder *
hyscan_data_schema_builder_new_with_gettext (const gchar *schema_id,
                                             const gchar *gettext_domain)
{
  return hyscan_data_schema_builder_new_full (schema_id, 0, gettext_domain);
}


/**
 * hyscan_data_schema_builder_new_with_gettext:
 * @schema_id: идентификатор создаваемой схемы
 * @schema_version: версия создаваемой схемы
 * @gettext_domain: название домена перевода gettext
 *
 * Функция создаёт новый объект #HyScanDataSchemaBuilder с возможностью перевода.
 *
 * Returns: #HyScanDataSchemaBuilder. Для удаления #g_object_unref.
 */
HyScanDataSchemaBuilder *
hyscan_data_schema_builder_new_full (const gchar *schema_id,
                                     gint64       schema_version,
                                     const gchar *gettext_domain)
{
  return g_object_new (HYSCAN_TYPE_DATA_SCHEMA_BUILDER,
                       "schema-id", schema_id,
                       "schema-version", schema_version,
                       "gettext-domain", gettext_domain,
                       NULL);
}

/**
 * hyscan_data_schema_builder_get_schema:
 * @builder: указатель на #HyScanDataSchemaBuilder
 *
 * Функция создаёт схему данных.
 *
 * Returns: (transfer full): #HyScanDataSchema. Для удаления #g_object_unref.
 */
HyScanDataSchema *
hyscan_data_schema_builder_get_schema (HyScanDataSchemaBuilder *builder)
{
  HyScanDataSchema *schema;
  gchar *data;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), NULL);

  data = hyscan_data_schema_builder_get_data (builder);
  schema = hyscan_data_schema_new_from_string (data, builder->priv->schema_id);
  g_free (data);

  return schema;
}

/**
 * hyscan_data_schema_builder_get_data:
 * @builder: указатель на #HyScanDataSchemaBuilder
 *
 * Функция создаёт XML описание схемы данных.
 *
 * Returns: Описание схемы данных. Для удаления #g_free.
 */
gchar *
hyscan_data_schema_builder_get_data (HyScanDataSchemaBuilder *builder)
{
  HyScanDataSchemaBuilderPrivate *priv;

  GHashTableIter iter;
  gpointer key, value;

  GOutputStream *ostream;
  gchar zero = 0;
  gchar *data;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), NULL);

  priv = builder->priv;

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
    hyscan_data_schema_builder_dump_enum (ostream, key, value);

  /* Описания параметров. */
  if (priv->nodes->name != NULL)
    {
      g_output_stream_printf (ostream, NULL, NULL, NULL,
                              "  <schema id=\"%s\" version=\"%" G_GINT64_FORMAT "\" name=\"%s\">\n",
                              priv->schema_id, priv->schema_version, priv->nodes->name);
    }
  else
    {
      g_output_stream_printf (ostream, NULL, NULL, NULL,
                              "  <schema id=\"%s\" version=\"%" G_GINT64_FORMAT "\" >\n",
                              priv->schema_id, priv->schema_version);
    }

  if (priv->nodes->description != NULL)
    {
      g_output_stream_printf (ostream, NULL, NULL, NULL,
                              "    <description>%s</description>\n",
                              priv->nodes->description);
    }

  hyscan_data_schema_builder_dump_node (ostream, priv->keys, priv->nodes, 0);

  g_output_stream_printf (ostream, NULL, NULL, NULL,
                          "  </schema>\n");

  g_output_stream_printf (ostream, NULL, NULL, NULL,
                          "</schemalist>\n");

  g_output_stream_write (ostream, &zero, sizeof (zero), NULL, NULL);

  g_output_stream_close (ostream, NULL, NULL);
  data = g_memory_output_stream_steal_data (G_MEMORY_OUTPUT_STREAM (ostream));
  g_object_unref (ostream);

  return data;
}

/**
 * hyscan_data_schema_builder_get_id:
 * @builder: указатель на #HyScanDataSchemaBuilder
 *
 * Функция возвращает идентификатор схемы данных.
 *
 * Returns: Идентификатор схемы данных.
 */
const gchar *
hyscan_data_schema_builder_get_id (HyScanDataSchemaBuilder *builder)
{
  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), NULL);

  return builder->priv->schema_id;
}

/**
 * hyscan_data_schema_builder_get_version:
 * @builder: указатель на #HyScanDataSchemaBuilder
 *
 * Функция возвращает версию схемы данных.
 *
 * Returns: Версия схемы данных.
 */
gint64
hyscan_data_schema_builder_get_version (HyScanDataSchemaBuilder *builder)
{
  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), -1);

  return builder->priv->schema_version;
}

/**
 * hyscan_data_schema_builder_get_text_domain:
 * @builder: указатель на #HyScanDataSchemaBuilder
 *
 * Функция возвращает название домена перевода gettext.
 *
 * Returns: Название домена перевода gettext.
 */
const gchar *
hyscan_data_schema_builder_get_text_domain (HyScanDataSchemaBuilder *builder)
{
  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), NULL);

  return builder->priv->gettext_domain;
}

/**
 * hyscan_data_schema_builder_enum_create:
 * @builder: указатель на #HyScanDataSchemaBuilder
 * @enum_id: идентификатор списка
 *
 * Функция создаёт новый список допустимых значений параметра ENUM.
 *
 * Returns: %TRUE - если список создан, иначе %FALSE.
 */
gboolean
hyscan_data_schema_builder_enum_create (HyScanDataSchemaBuilder *builder,
                                        const gchar             *enum_id)
{
  HyScanDataSchemaBuilderPrivate *priv;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  priv = builder->priv;

  if (!hyscan_data_schema_internal_validate_name (enum_id))
    return FALSE;

  if (g_hash_table_contains (priv->enums, enum_id))
    return FALSE;

  return g_hash_table_insert (priv->enums, g_strdup (enum_id), NULL);
}

/**
 * hyscan_data_schema_builder_enum_value_create:
 * @builder: указатель на #HyScanDataSchemaBuilder
 * @enum_id: идентификатор списка
 * @value: численное значение параметра
 * @value_id: идентификатор значения параметра
 * @name: название значения параметра
 * @description: (nullable): описание значения параметра
 *
 * Функция создаёт новое значение параметра в списке возможных значений.
 *
 * Returns: %TRUE - если новое значение создано, иначе %FALSE.
 */
gboolean
hyscan_data_schema_builder_enum_value_create (HyScanDataSchemaBuilder *builder,
                                              const gchar             *enum_id,
                                              gint64                   value,
                                              const gchar             *value_id,
                                              const gchar             *name,
                                              const gchar             *description)
{
  HyScanDataSchemaBuilderPrivate *priv;
  HyScanDataSchemaEnumValue *enum_value;
  gboolean replace_values = TRUE;
  GList *values, *cur_values;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  priv = builder->priv;

  if (!g_hash_table_contains (priv->enums, enum_id))
    return FALSE;

  /* Проверяем что такого значения ещё нет. */
  values = g_hash_table_lookup (priv->enums, enum_id);
  for (cur_values = values; cur_values != NULL; cur_values = cur_values->next)
    {
      replace_values = FALSE;
      enum_value = cur_values->data;
      if (g_strcmp0 (enum_value->id, value_id) == 0)
        return FALSE;
    }

  /* Добавляем новое значение. */
  enum_value = hyscan_data_schema_enum_value_new (value, value_id, name, description);
  values = g_list_append (values, enum_value);

  /* Если до этого список значений был пустой, запоминаем новый. */
  if (replace_values)
    g_hash_table_insert (priv->enums, g_strdup (enum_id), values);

  return TRUE;
}

/**
 * hyscan_data_schema_builder_node_set_name:
 * @builder: указатель на #HyScanDataSchemaBuilder
 * @path: путь до узла
 * @name: название узла
 * @description: описание узла
 *
 * Функция устанавливает название и описание узла. Если узел не существует,
 * он будет создан.
 *
 * Returns: %TRUE - если название и описание узла установлено, иначе %FALSE.
 */
gboolean
hyscan_data_schema_builder_node_set_name (HyScanDataSchemaBuilder *builder,
                                          const gchar             *path,
                                          const gchar             *name,
                                          const gchar             *description)
{
  HyScanDataSchemaBuilderPrivate *priv;
  HyScanDataSchemaNode *node;
  gchar *pathn;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  priv = builder->priv;

  pathn = hyscan_data_schema_internal_normalize_id (path);
  if (pathn == NULL)
    return FALSE;

  node = hyscan_data_schema_internal_insert_node (priv->nodes, pathn);

  g_free ((gchar*)node->name);
  g_free ((gchar*)node->description);

  node->name = g_strdup (name);
  node->description = g_strdup (description);

  g_free (pathn);

  return TRUE;
}

/**
 * hyscan_data_schema_builder_key_boolean_create:
 * @builder: указатель на #HyScanDataSchemaBuilder
 * @key_id: идентификатор параметра
 * @name: название параметра
 * @description: (nullable): описание параметра
 * @default_value: значение по умолчанию
 *
 * Функция создаёт новый параметр типа BOOLEAN.
 *
 * Returns: %TRUE если новый параметр создан, иначе %FALSE.
 */
gboolean
hyscan_data_schema_builder_key_boolean_create (HyScanDataSchemaBuilder *builder,
                                               const gchar             *key_id,
                                               const gchar             *name,
                                               const gchar             *description,
                                               gboolean                 default_value)
{
  HyScanDataSchemaBuilderPrivate *priv;
  HyScanDataSchemaInternalKey *ikey;
  HyScanDataSchemaKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  priv = builder->priv;

  if (g_hash_table_contains (priv->keys, key_id))
    return FALSE;

  if (!hyscan_data_schema_internal_validate_id (key_id))
    return FALSE;

  ikey = hyscan_data_schema_internal_key_new (key_id, name, description);
  ikey->type = HYSCAN_DATA_SCHEMA_KEY_BOOLEAN;
  ikey->default_value = g_variant_new_boolean (default_value);

  key = hyscan_data_schema_key_new (ikey->id, ikey->name, ikey->description,
                                    ikey->type, ikey->view, ikey->access);
  hyscan_data_schema_internal_node_insert_key (priv->nodes, key);

  return g_hash_table_insert (priv->keys, ikey->id, ikey);
}

/**
 * hyscan_data_schema_builder_key_integer_create:
 * @builder: указатель на #HyScanDataSchemaBuilder
 * @key_id: идентификатор параметра
 * @name: название параметра
 * @description: (nullable): описание параметра
 * @default_value: значение по умолчанию
 *
 * Функция создаёт новый параметр типа INTEGER.
 *
 * Returns: %TRUE если новый параметр создан, иначе %FALSE.
 */
gboolean
hyscan_data_schema_builder_key_integer_create (HyScanDataSchemaBuilder *builder,
                                               const gchar             *key_id,
                                               const gchar             *name,
                                               const gchar             *description,
                                               gint64                   default_value)
{
  HyScanDataSchemaBuilderPrivate *priv;
  HyScanDataSchemaInternalKey *ikey;
  HyScanDataSchemaKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  priv = builder->priv;

  if (g_hash_table_contains (priv->keys, key_id))
    return FALSE;

  if (!hyscan_data_schema_internal_validate_id (key_id))
    return FALSE;

  ikey = hyscan_data_schema_internal_key_new (key_id, name, description);
  ikey->type = HYSCAN_DATA_SCHEMA_KEY_INTEGER;
  ikey->default_value = g_variant_new_int64 (default_value);

  key = hyscan_data_schema_key_new (ikey->id, ikey->name, ikey->description,
                                    ikey->type, ikey->view, ikey->access);
  hyscan_data_schema_internal_node_insert_key (priv->nodes, key);

  return g_hash_table_insert (priv->keys, ikey->id, ikey);
}

/**
 * hyscan_data_schema_builder_key_double_create:
 * @builder: указатель на #HyScanDataSchemaBuilder
 * @key_id: идентификатор параметра
 * @name: название параметра
 * @description: (nullable): описание параметра
 * @default_value: значение по умолчанию
 *
 * Функция создаёт новый параметр типа DOUBLE.
 *
 * Returns: %TRUE если новый параметр создан, иначе %FALSE.
 */
gboolean
hyscan_data_schema_builder_key_double_create (HyScanDataSchemaBuilder *builder,
                                              const gchar             *key_id,
                                              const gchar             *name,
                                              const gchar             *description,
                                              gdouble                  default_value)
{
  HyScanDataSchemaBuilderPrivate *priv;
  HyScanDataSchemaInternalKey *ikey;
  HyScanDataSchemaKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  priv = builder->priv;

  if (g_hash_table_contains (priv->keys, key_id))
    return FALSE;

  if (!hyscan_data_schema_internal_validate_id (key_id))
    return FALSE;

  ikey = hyscan_data_schema_internal_key_new (key_id, name, description);
  ikey->type = HYSCAN_DATA_SCHEMA_KEY_DOUBLE;
  ikey->default_value = g_variant_new_double (default_value);

  key = hyscan_data_schema_key_new (ikey->id, ikey->name, ikey->description,
                                    ikey->type, ikey->view, ikey->access);
  hyscan_data_schema_internal_node_insert_key (priv->nodes, key);

  return g_hash_table_insert (priv->keys, ikey->id, ikey);
}

/**
 * hyscan_data_schema_builder_key_string_create:
 * @builder: указатель на #HyScanDataSchemaBuilder
 * @key_id: идентификатор параметра
 * @name: название параметра
 * @description: (nullable): описание параметра
 * @default_value: значение по умолчанию
 *
 * Функция создаёт новый параметр типа STRING.
 *
 * Returns: %TRUE если новый параметр создан, иначе %FALSE.
 */
gboolean
hyscan_data_schema_builder_key_string_create (HyScanDataSchemaBuilder *builder,
                                              const gchar             *key_id,
                                              const gchar             *name,
                                              const gchar             *description,
                                              const gchar             *default_value)
{
  HyScanDataSchemaBuilderPrivate *priv;
  HyScanDataSchemaInternalKey *ikey;
  HyScanDataSchemaKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  priv = builder->priv;

  if (g_hash_table_contains (priv->keys, key_id))
    return FALSE;

  if (!hyscan_data_schema_internal_validate_id (key_id))
    return FALSE;

  ikey = hyscan_data_schema_internal_key_new (key_id, name, description);
  ikey->type = HYSCAN_DATA_SCHEMA_KEY_STRING;
  if (default_value != NULL)
    ikey->default_value = g_variant_new_string (default_value);

  key = hyscan_data_schema_key_new (ikey->id, ikey->name, ikey->description,
                                    ikey->type, ikey->view, ikey->access);
  hyscan_data_schema_internal_node_insert_key (priv->nodes, key);

  return g_hash_table_insert (priv->keys, ikey->id, ikey);
}

/**
 * hyscan_data_schema_builder_key_enum_create:
 * @builder: указатель на #HyScanDataSchemaBuilder
 * @key_id: идентификатор параметра
 * @name: название параметра
 * @description: (nullable): описание параметра
 * @enum_id: идентификатор списка возможных значений параметра
 * @default_value: значение по умолчанию
 *
 * Функция создаёт новый параметр типа ENUM.
 *
 * Returns: %TRUE если новый параметр создан, иначе %FALSE.
 */
gboolean
hyscan_data_schema_builder_key_enum_create (HyScanDataSchemaBuilder *builder,
                                            const gchar             *key_id,
                                            const gchar             *name,
                                            const gchar             *description,
                                            const gchar             *enum_id,
                                            gint64                   default_value)
{
  HyScanDataSchemaBuilderPrivate *priv;
  HyScanDataSchemaInternalKey *ikey;
  HyScanDataSchemaKey *key;
  GList *values;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  priv = builder->priv;

  if (g_hash_table_contains (priv->keys, key_id))
    return FALSE;

  if (!hyscan_data_schema_internal_validate_id (key_id))
    return FALSE;

  values = g_hash_table_lookup (priv->enums, enum_id);
  if (values == NULL)
    return FALSE;

  if (!hyscan_data_schema_internal_enum_check (values, default_value))
    {
      g_warning ("HyScanDataBuilder: default value out of range in key '%s'", key_id);
      return FALSE;
    }

  ikey = hyscan_data_schema_internal_key_new (key_id, name, description);
  ikey->type = HYSCAN_DATA_SCHEMA_KEY_ENUM;
  ikey->enum_id = g_strdup (enum_id);
  ikey->default_value = g_variant_new_int64 (default_value);

  key = hyscan_data_schema_key_new (ikey->id, ikey->name, ikey->description,
                                    ikey->type, ikey->view, ikey->access);
  hyscan_data_schema_internal_node_insert_key (priv->nodes, key);

  return g_hash_table_insert (priv->keys, ikey->id, ikey);
}

/**
 * hyscan_data_schema_builder_key_set_view:
 * @builder: указатель на #HyScanDataSchemaBuilder
 * @key_id: идентификатор параметра
 * @view: рекомендуемый вид отображения параметра #HyScanDataSchemaViewType
 *
 * Функция задаёт рекомендуемый вид отображения параметра.
 *
 * Returns: %TRUE если вид отображения установлен, иначе %FALSE.
 *
 */
gboolean
hyscan_data_schema_builder_key_set_view (HyScanDataSchemaBuilder  *builder,
                                         const gchar              *key_id,
                                         HyScanDataSchemaViewType  view)
{
  HyScanDataSchemaInternalKey *ikey;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  ikey = g_hash_table_lookup (builder->priv->keys, key_id);
  if (ikey == NULL)
    return FALSE;

  ikey->view = view;

  return TRUE;
}

/**
 * hyscan_data_schema_builder_key_set_access:
 * @builder: указатель на #HyScanDataSchemaBuilder
 * @key_id: идентификатор параметра
 * @access: атрибут доступа к параметру #HyScanDataSchemaKeyAccess
 *
 * Функция задаёт атрибут доступа к параметру.
 *
 * Returns: %TRUE если атрибут доступа установлен, иначе %FALSE.
 */
gboolean
hyscan_data_schema_builder_key_set_access (HyScanDataSchemaBuilder   *builder,
                                           const gchar               *key_id,
                                           HyScanDataSchemaKeyAccess  access)
{
  HyScanDataSchemaInternalKey *ikey;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  ikey = g_hash_table_lookup (builder->priv->keys, key_id);
  if (ikey == NULL)
    return FALSE;

  ikey->access = access;

  return TRUE;
}

/**
 * hyscan_data_schema_builder_key_integer_range:
 * @builder: указатель на #HyScanDataSchemaBuilder
 * @key_id: идентификатор параметра
 * @minimum_value: минимально возможное значение параметра
 * @maximum_value: максимально возможное значение параметра
 * @value_step: рекомендуемый шаг изменения значения параметра
 *
 * Функция задаёт диапазон допустимых значений и рекомендуемый шаг изменения
 * значения параметра типа INTEGER.
 *
 * Returns: %TRUE если значения установлены, иначе %FALSE.
 */
gboolean
hyscan_data_schema_builder_key_integer_range (HyScanDataSchemaBuilder *builder,
                                              const gchar             *key_id,
                                              gint64                   minimum_value,
                                              gint64                   maximum_value,
                                              gint64                   value_step)
{
  HyScanDataSchemaInternalKey *ikey;
  gint64 default_value;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  ikey = g_hash_table_lookup (builder->priv->keys, key_id);
  if (ikey == NULL)
    return FALSE;

  if (ikey->type != HYSCAN_DATA_SCHEMA_KEY_INTEGER)
    return FALSE;

  default_value = g_variant_get_int64 (ikey->default_value);
  if ((minimum_value > maximum_value) ||
      (default_value < minimum_value) ||
      (default_value > maximum_value))
    {
      g_warning ("HyScanDataBuilder: default value out of range in key '%s'", key_id);
      return FALSE;
    }

  ikey->minimum_value = g_variant_new_int64 (minimum_value);
  ikey->maximum_value = g_variant_new_int64 (maximum_value);
  ikey->value_step = g_variant_new_int64 (value_step);

  return TRUE;
}

/**
 * hyscan_data_schema_builder_key_double_range:
 * @builder: указатель на #HyScanDataSchemaBuilder
 * @key_id: идентификатор параметра
 * @minimum_value: минимально возможное значение параметра
 * @maximum_value: максимально возможное значение параметра
 * @value_step: рекомендуемый шаг изменения значения параметра
 *
 * Функция задаёт диапазон допустимых значений и рекомендуемый шаг изменения
 * значения параметра типа DOUBLE.
 *
 * Returns: %TRUE если значения установлены, иначе %FALSE.
 */
gboolean
hyscan_data_schema_builder_key_double_range (HyScanDataSchemaBuilder *builder,
                                             const gchar             *key_id,
                                             gdouble                  minimum_value,
                                             gdouble                  maximum_value,
                                             gdouble                  value_step)
{
  HyScanDataSchemaInternalKey *ikey;
  gdouble default_value;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA_BUILDER (builder), FALSE);

  ikey = g_hash_table_lookup (builder->priv->keys, key_id);
  if (ikey == NULL)
    return FALSE;

  if (ikey->type != HYSCAN_DATA_SCHEMA_KEY_DOUBLE)
    return FALSE;

  default_value = g_variant_get_double (ikey->default_value);
  if ((minimum_value > maximum_value) ||
      (default_value < minimum_value) ||
      (default_value > maximum_value))
    {
      g_warning ("HyScanDataBuilder: default value out of range in key '%s'", key_id);
      return FALSE;
    }

  ikey->minimum_value = g_variant_new_double (minimum_value);
  ikey->maximum_value = g_variant_new_double (maximum_value);
  ikey->value_step = g_variant_new_double (value_step);

  return TRUE;
}

/**
 * hyscan_data_schema_builder_schema_join:
 * @builder: указатель на #HyScanDataSchemaBuilder
 * @dst_root: префикс пути для новых параметров из другой схемы
 * @schema: схема #HyScanDataSchema с добавляемыми параметрами
 * @src_root: исходный путь в схеме
 *
 * Функция добавляет содержимое другой схемы. Параметры из пути src_root схемы
 * добавляются в путь dst_root. Например если src_root = "/src/root", а
 * dst_root = "/dst/root", параметр схемы "/src/root/param" будет создан с
 * идентификатором "/dst/root/param".
 *
 * Returns: %TRUE если новые параметры добавлены, иначе %FALSE.
 */
gboolean
hyscan_data_schema_builder_schema_join (HyScanDataSchemaBuilder *builder,
                                        const gchar             *dst_root,
                                        HyScanDataSchema        *schema,
                                        const gchar             *src_root)
{
  const HyScanDataSchemaNode *node;
  gchar *dst_path;
  gchar *src_path;

  gboolean status = FALSE;

  /* Нормализация путей. */
  dst_path = hyscan_data_schema_internal_normalize_id (dst_root);
  src_path = hyscan_data_schema_internal_normalize_id (src_root);

  /* Поиск начального узла исходной схемы. */
  node = hyscan_data_schema_list_nodes (schema);
  if (node != NULL)
    {
      GList *nodes = node->nodes;

      while (g_strcmp0 (src_path, node->path) != 0)
        {
          if (nodes != NULL)
            {
              node = nodes->data;
              if (hyscan_data_schema_internal_is_path (src_path, node->path))
                nodes = node->nodes;
              else
                nodes = g_list_next (nodes);
            }
          else
            {
              node = NULL;
              break;
            }
        }
    }

  /* Добавление параметров в схему. */
  if (node != NULL)
    status = hyscan_data_schema_builder_join_node (builder, dst_path, schema, node);

  g_free (dst_path);
  g_free (src_path);

  return status;
}
