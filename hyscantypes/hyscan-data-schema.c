/* hyscan-data-schema.c
 *
 * Copyright 2016-2017 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
 *
 * This file is part of HyScanTypes.
 *
 * HyScanTypes is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HyScan is distributed in the hope that it will be useful,
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
 * Во первых, вы можете распространять HyScanTypes на условиях Стандартной
 * Общественной Лицензии GNU версии 3, либо по любой более поздней версии
 * лицензии (по вашему выбору). Полные положения лицензии GNU приведены в
 * <http://www.gnu.org/licenses/>.
 *
 * Во вторых, этот программный код можно использовать по коммерческой
 * лицензии. Для этого свяжитесь с ООО Экран - info@screen-co.ru.
 */

/**
 * SECTION: hyscan-data-schema
 * @Short_description: класс схем данных HyScan
 * @Title: HyScanDataSchema
 *
 * Класс предназначен для описания модели данных. Под моделью данных подразумевается
 * список параметров имеющих следующие атрибуты:
 *
 * - уникальный идентификатор параметра;
 * - название параметра;
 * - описание параметра;
 * - тип параметра;
 * - вид отображения;
 * - значение по умолчанию;
 * - минимальное и максимальное значения параметра;
 * - рекомендуемый шаг изменения параметра;
 * - варианты значений параметра для перечисляемых типов.
 *
 * Схема данных приводится в формате XML. Описание схемы имеет следующий вид:
 *
 * [
 * <schemalist gettext-domain="domain">
 *
 *   <enum id="sex">
 *     <value name="unknown" value="0">
 *       <description>Unknown</description>
 *     </value>
 *     <value name="male" value="1">
 *       <description>Male</description>
 *     </value>
 *     <value name="female" value="2">
 *       <description>Female</description>
 *     </value>
 *   </enum>
 *
 *   <schema id="name">
 *     <key id="first" name="First name" type="string"/>
 *     <key id="middle" name="Middle name" type="string"/>
 *     <key id="Last" name="Last name" type="string"/>
 *   </schema>
 *
 *   <schema id="person">
 *     <node id="name" schema="name">
 *     <key id="sex" name="Sex" enum="sex"/>
 *       <description>Gender identity</description>
 *       <default>0</default>
 *     </key>
 *     <key id="birthdate" name="Birth date" type="integer" view="date" access="readonly"/>
 *     <key id="weight" name="Weight" type="double">
 *       <description>Weight in kilogramms</description>
 *       <range min="0" max="1000" step="0.1"/>
 *     </key>
 *   </schema>
 *
 * </schemalist>
 * ]
 *
 * Все данные с описанием схем хранятся в теге &lt;schemalist&gt;. Опциональным
 * атрибутом этого тега является "gettext-domain" - имя домена с переводами для
 * имён и описаний параметров схем. При загрузке схемы классом #HyScanDataSchema,
 * будет использован этот домен для поиска переводов.
 *
 * Описание может хранить несколько схем одновременно. Для определения схемы
 * используется тег &lt;schema&gt;. Тег содержит обязательный атрибут "id" -
 * идентификатор схемы. По этому идентификатору схема загружается классом
 * #HyScanDataSchema или может использоваться в составе другой схемы.
 *
 * Включение одной схемы в другую осуществляется тегом &lt;node&gt;. Обязательными
 * атрибутами, в этом случае, являются:
 *
 * - id - идентификатор ветки;
 * - schema - идентификатор дочерней схемы.
 *
 * Тэг &lt;node&gt; также может использоваться для создания вложенных структур.
 * В этом случае атрибут "schema" не указывается, а необходимые параметры
 * объявляются в этом теге. Класс #HyScanDataSchema поддерживает неограниченную
 * вложенность.
 *
 * Параметры в схеме определяются тегом &lt;key&gt;. Обязательными атрибутами
 * этого тега являются:
 *
 * - id - идентификатор параметра;
 * - name - название параметра (может содержать перевод);
 * - type - тип параметра.
 *
 * В схеме данных поддерживаются параметры следующих типов (#HyScanDataSchemaKeyType):
 *
 * - "boolean" - #HYSCAN_DATA_SCHEMA_KEY_BOOLEAN;
 * - "integer" - #HYSCAN_DATA_SCHEMA_KEY_INTEGER;
 * - "double" - #HYSCAN_DATA_SCHEMA_KEY_DOUBLE;
 * - "string" - #HYSCAN_DATA_SCHEMA_KEY_STRING;
 * - "enum" - #HYSCAN_DATA_SCHEMA_KEY_ENUM.
 *
 * Дополнительные атрибуты:
 *
 * - view - рекомендуемый вид отображения;
 * - access - атрибуты доступа к параметру.
 *
 * Поддерживаются следующие виды отображения значения параметра (#HyScanDataSchemaViewType):
 *
 * - "bin" - #HYSCAN_DATA_SCHEMA_VIEW_BIN;
 * - "dec" - #HYSCAN_DATA_SCHEMA_VIEW_DEC;
 * - "hex" - #HYSCAN_DATA_SCHEMA_VIEW_HEX;
 * - "date" - #HYSCAN_DATA_SCHEMA_VIEW_DATE;
 * - "time" - #HYSCAN_DATA_SCHEMA_VIEW_TIME;
 * - "datetime" - #HYSCAN_DATA_SCHEMA_VIEW_DATE_TIME;
 * - "schema" - #HYSCAN_DATA_SCHEMA_VIEW_SCHEMA.
 *
 * Возможны следующие атрибуты доступа к параметру (#HyScanDataSchemaKeyAccess):
 *
 * - "readonly" - #HYSCAN_DATA_SCHEMA_ACCESS_READONLY;
 * - "writeonly" - #HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY.
 *
 * Тэг &lt;key&gt; может содержать вложенные тэги:
 *
 * - description - строка с описанием параметра (может содержать перевод);
 * - default - значение параметра по умолчанию;
 * - range - диапазон допустимых значений и рекомендуемый шаг изменения.
 *
 * Тэг &lt;range&gt; применим только для типов данных "integer" и "double" и
 * может содержать следующие атрибуты:
 *
 * - min - минимальное значение параметра;
 * - max - максимальное значение параметра;
 * - step - рекомендуемый шаг изменения параметра.
 *
 * Если для типа "string" не задано значение по умолчанию, в качестве такового
 * будет использован NULL.
 *
 * При определении параметра перечисляемого типа, вместо атрибута "type"
 * используется "enum", в котором указывается идентификатор группы вариантов
 * значений.
 *
 * Группы вариантов значение определяется тегом &lt;enum&gt;. Тег содержит
 * обязательный атрибут "id" - идентификатор группы значений.
 *
 * Значения в группе определяются тэгом &lt;value&gt;. Обязательными атрибутами
 * этого тега являются:
 *
 * - name - название значения (может содержать перевод);
 * - value - численный идентификатор значения.
 *
 * Тэг &lt;value&gt; может содержать вложенный тег &lt;description&gt; с описанием
 * значения. Описание значения может содержать перевод.
 *
 * Функции этого класса предназначены, в первую очередь, для классов реализующих
 * доступ к данным или для программ исследующих струкутуру данных. Разработчик
 * должен знать схему данных с которой он работает.
 *
 * Создание объекта со схемой данных осуществляется функциями #hyscan_data_schema_new_from_string,
 * #hyscan_data_schema_new_from_file и #hyscan_data_schema_new_from_resource.
 *
 * Получить описание загруженной схемы данных можно с помощью функции #hyscan_data_schema_get_data,
 * а её идентификатор #hyscan_data_schema_get_id.
 *
 * Список всех параметров схемы можно получить с помощью функции #hyscan_data_schema_list_keys.
 * Проверить существование параметра в схеме можно функцией #hyscan_data_schema_has_key.
 *
 * Структурированный список параметров и их описаний можно получить функцией
 * #hyscan_data_schema_list_nodes. Функция возвращает указатель на вновь созданую
 * структуру #HyScanDataSchemaNode.
 *
 * Функция #hyscan_data_schema_key_get_access предназначена для получения атрибутов
 * доступа к параметру.
 *
 * Следующая группа функций предназначена для получения атрибутов параметров:
 *
 * - #hyscan_data_schema_key_get_value_type - возвращает тип параметра;
 * - #hyscan_data_schema_key_get_name - возвращает название параметра;
 * - #hyscan_data_schema_key_get_description - возвращает описание параметра;
 * - #hyscan_data_schema_key_get_default - возвращает значение параметра по умолчанию;
 * - #hyscan_data_schema_key_get_minimum - Функция возвращает минимальное значение параметра;
 * - #hyscan_data_schema_key_get_maximum - Функция возвращает максимальное значение параметра;
 * - #hyscan_data_schema_key_get_step - возвращает рекомендуемый шаг изменения значения параметра.
 *
 * Проверить значение параметра на предмет нахождения в допустимом диапазоне
 * можно функцией #hyscan_data_schema_key_check.
 *
 * Варианты допустимых значений для параметров с типом ENUM можно получить с
 * помощью функции #hyscan_data_schema_key_get_enum_values. Предварительно
 * необходимо получить идентификатор списка значений для ENUM параметра с
 * помощью функции #hyscan_data_schema_key_get_enum_id.
 *
 * Значения параметров определяются с помощью GVariant. Используются следующие
 * типы GVariant:
 *
 * - #HYSCAN_DATA_SCHEMA_KEY_BOOLEAN - G_VARIANT_CLASS_BOOLEAN;
 * - #HYSCAN_DATA_SCHEMA_KEY_INTEGER - G_VARIANT_CLASS_INT64;
 * - #HYSCAN_DATA_SCHEMA_KEY_DOUBLE - G_VARIANT_CLASS_DOUBLE;
 * - #HYSCAN_DATA_SCHEMA_KEY_STRING - G_VARIANT_CLASS_STRING;
 * - #HYSCAN_DATA_SCHEMA_KEY_ENUM - G_VARIANT_CLASS_INT64.
 */

#include "hyscan-data-schema-internal.h"

#include <gio/gio.h>
#include <glib/gi18n.h>
#include <libxml/parser.h>
#include <string.h>

enum
{
  PROP_O,
  PROP_SCHEMA_DATA,
  PROP_SCHEMA_ID
};

struct _HyScanDataSchemaPrivate
{
  gchar                       *schema_data;            /* Описание схемы в формате XML. */
  gchar                       *schema_id;              /* Идентификатор используемой схемы. */
  gchar                       *gettext_domain;         /* Название домена переводов. */

  gchar                      **keys_list;              /* Список имён параметров. */

  GHashTable                  *keys;                   /* Список параметров. */
  GHashTable                  *enums;                  /* Список значений перечисляемых типов. */
};

static void            hyscan_data_schema_set_property         (GObject                    *object,
                                                                guint                       prop_id,
                                                                const GValue               *value,
                                                                GParamSpec                 *pspec);
static void            hyscan_data_schema_object_constructed   (GObject                    *object);
static void            hyscan_data_schema_object_finalize      (GObject                    *object);

static gint            hyscan_data_schema_compare_keys         (HyScanDataSchemaInternalKey *ikey1,
                                                                HyScanDataSchemaInternalKey *ikey2,
                                                                gpointer                     user_data);

static GList *         hyscan_data_schema_parse_enum_values    (xmlNodePtr                  node,
                                                                const gchar                *gettext_domain);

static HyScanDataSchemaInternalKey *
                       hyscan_data_schema_parse_key            (xmlNodePtr                  node,
                                                                const gchar                *path,
                                                                GHashTable                 *enums,
                                                                const gchar                *gettext_domain);
static gboolean        hyscan_data_schema_parse_node           (xmlNodePtr                  node,
                                                                const gchar                *schema_path,
                                                                const gchar                *path,
                                                                GHashTable                 *keys,
                                                                GHashTable                 *enums,
                                                                const gchar                *gettext_domain);
static gboolean        hyscan_data_schema_parse_schema         (xmlDocPtr                   doc,
                                                                const gchar                *schema_path,
                                                                const gchar                *schema,
                                                                const gchar                *path,
                                                                GHashTable                 *keys,
                                                                GHashTable                 *enums,
                                                                const gchar                *gettext_domain);

G_DEFINE_BOXED_TYPE (HyScanDataSchemaEnumValue, hyscan_data_schema_enum_value,
                     hyscan_data_schema_enum_value_copy, hyscan_data_schema_enum_value_free)

G_DEFINE_BOXED_TYPE (HyScanDataSchemaNode, hyscan_data_schema_node,
                     hyscan_data_schema_node_copy, hyscan_data_schema_node_free)

G_DEFINE_BOXED_TYPE (HyScanDataSchemaKey, hyscan_data_schema_key,
                     hyscan_data_schema_key_copy, hyscan_data_schema_key_free)

G_DEFINE_TYPE_WITH_PRIVATE (HyScanDataSchema, hyscan_data_schema, G_TYPE_OBJECT);

static void hyscan_data_schema_class_init( HyScanDataSchemaClass *klass )
{
  GObjectClass *object_class = G_OBJECT_CLASS( klass );

  object_class->set_property = hyscan_data_schema_set_property;

  object_class->constructed = hyscan_data_schema_object_constructed;
  object_class->finalize = hyscan_data_schema_object_finalize;

  g_object_class_install_property (object_class, PROP_SCHEMA_DATA,
    g_param_spec_string ("schema-data", "SchemaData", "Schema data", NULL,
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_property (object_class, PROP_SCHEMA_ID,
    g_param_spec_string ("schema-id", "SchemaID", "Schema id", NULL,
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

static void
hyscan_data_schema_init (HyScanDataSchema *schema)
{
  schema->priv = hyscan_data_schema_get_instance_private (schema);
}

static void
hyscan_data_schema_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  HyScanDataSchema *schema = HYSCAN_DATA_SCHEMA (object);
  HyScanDataSchemaPrivate *priv = schema->priv;

  switch (prop_id)
    {
    case PROP_SCHEMA_DATA:
      priv->schema_data = g_value_dup_string (value);
      break;

    case PROP_SCHEMA_ID:
      priv->schema_id = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hyscan_data_schema_object_constructed (GObject *object)
{
  HyScanDataSchema *schema = HYSCAN_DATA_SCHEMA (object);
  HyScanDataSchemaPrivate *priv = schema->priv;

  xmlDocPtr doc;
  xmlNodePtr node;
  xmlChar *gettext_domain;

  gboolean status;
  guint n_keys;

  /* Таблица параметров. */
  priv->keys =  g_hash_table_new_full (g_str_hash,
                                       g_str_equal,
                                       NULL,
                                       (GDestroyNotify)hyscan_data_schema_internal_key_free);

  /* Таблица enum типов и их значений. */
  priv->enums = g_hash_table_new_full (g_str_hash,
                                       g_str_equal,
                                       g_free,
                                       (GDestroyNotify)hyscan_data_schema_internal_enum_free);

  /* Разбор описания схемы. */
  if (priv->schema_data == NULL)
    return;
  doc = xmlParseMemory (priv->schema_data, strlen (priv->schema_data));
  if (doc == NULL)
    return;

  /* Поиск корневого узла schemalist. */
  for (node = xmlDocGetRootElement (doc); node != NULL; node = node->next)
    if (node->type == XML_ELEMENT_NODE)
      if (g_ascii_strcasecmp ((gchar*)node->name, "schemalist") == 0)
        break;

  if (node == NULL)
    goto exit;

  /* Название домена переводов. */
  gettext_domain = xmlGetProp (node, (xmlChar *)"gettext-domain");
  if (gettext_domain != NULL)
    priv->gettext_domain = g_strdup ((gchar*)gettext_domain);
  xmlFree (gettext_domain);

  /* Обработка enum типов. */
  for (node = node->children; node != NULL; node = node->next)
    {
      xmlChar *id;

      if (node->type != XML_ELEMENT_NODE)
        continue;
      if (g_ascii_strcasecmp ((gchar*)node->name, "enum") != 0)
        continue;

      id = xmlGetProp (node, (xmlChar *)"id");
      if (id == NULL)
        {
          g_warning ("HyScanDataSchema: unknown enum id");
          goto exit;
        }

      if (g_hash_table_lookup (priv->enums, id) == NULL)
        {
          xmlChar *enum_id;
          GList *values;

          enum_id = xmlGetProp (node, (xmlChar *)"id");
          values = hyscan_data_schema_parse_enum_values (node, priv->gettext_domain);

          if ((enum_id != NULL) && (values != NULL))
            g_hash_table_insert (priv->enums, g_strdup ((const gchar*)enum_id), values);

          xmlFree (enum_id);
        }
      else
        {
          g_warning ("HyScanDataSchema: duplicated enum type %s", id);
        }

      xmlFree (id);
    }

  /* Обработка описания схемы. */
  status = hyscan_data_schema_parse_schema (doc,
                                            "/", priv->schema_id, "/",
                                            priv->keys, priv->enums,
                                            priv->gettext_domain);
  if (!status)
    {
      g_hash_table_remove_all (priv->keys);
      g_hash_table_remove_all (priv->enums);
    }

  /* Список имён параметров. */
  n_keys = g_hash_table_size (priv->keys);
  if (n_keys > 0)
    {
      HyScanDataSchemaInternalKey *key;
      GHashTableIter iter;
      guint i;

      priv->keys_list = g_malloc ((n_keys + 1) * sizeof (gchar*));
      g_hash_table_iter_init (&iter, priv->keys);
      for (i = 0; g_hash_table_iter_next (&iter, NULL, (gpointer*)&key); i++)
        priv->keys_list[i] = key->id;
      priv->keys_list[i] = NULL;

      g_qsort_with_data (priv->keys_list, n_keys, sizeof (gchar*),
                         (GCompareDataFunc)hyscan_data_schema_compare_keys,
                         NULL);
    }

exit:
  xmlFreeDoc (doc);
}

static void
hyscan_data_schema_object_finalize (GObject *object)
{
  HyScanDataSchema *schema = HYSCAN_DATA_SCHEMA (object);
  HyScanDataSchemaPrivate *priv = schema->priv;

  g_hash_table_unref (priv->keys);
  g_hash_table_unref (priv->enums);

  g_free (priv->keys_list);

  g_free (priv->schema_data);
  g_free (priv->schema_id);
  g_free (priv->gettext_domain);

  G_OBJECT_CLASS (hyscan_data_schema_parent_class)->finalize (object);
}

/* Функция сравнения двух параметров по идентификаторам (для сортировки списка). */
gint
hyscan_data_schema_compare_keys (HyScanDataSchemaInternalKey *ikey1,
                                 HyScanDataSchemaInternalKey *ikey2,
                                 gpointer                     user_data)
{
  return g_strcmp0 (ikey1->id, ikey2->id);
}

/* Функция обрабатывает описание значений типа enum. */
GList *
hyscan_data_schema_parse_enum_values (xmlNodePtr   node,
                                      const gchar *gettext_domain)
{
  GList *values = NULL;

  /* Разбор всех вариантов значений. */
  for (node = node->children; node != NULL; node = node->next)
    {
      xmlNodePtr sub_node;
      xmlChar *name = NULL;
      xmlChar *value = NULL;
      xmlChar *description = NULL;
      HyScanDataSchemaEnumValue *enum_value;

      /* Проверяем что XML элемент содержит описание значения. */
      if (node->type != XML_ELEMENT_NODE)
        continue;
      if (g_ascii_strcasecmp ((gchar*)node->name, "value") != 0)
        continue;

      /* Атрибуты name и value. */
      name = xmlGetProp (node, (xmlChar *)"name");
      value = xmlGetProp (node, (xmlChar *)"value");
      if (name == NULL || value == NULL)
        continue;

      /* Поле description, может отсутствовать. */
      for (sub_node = node->children; sub_node != NULL; sub_node = sub_node->next)
        if (sub_node->type == XML_ELEMENT_NODE)
          if (g_ascii_strcasecmp ((gchar*)sub_node->name, "description") == 0)
            {
              description = xmlNodeGetContent (sub_node);
              break;
            }
      enum_value = hyscan_data_schema_enum_value_new (g_ascii_strtoll ((const gchar *)value, NULL, 10),
                                                      (const gchar*)name,
                                                      g_dgettext (gettext_domain, (const gchar *)description));

      values = g_list_prepend (values, enum_value);

      xmlFree (name);
      xmlFree (value);
      xmlFree (description);
    }

  return g_list_reverse (values);
}

/* Функция обрабатывает описание параметра. */
static HyScanDataSchemaInternalKey *
hyscan_data_schema_parse_key (xmlNodePtr   node,
                              const gchar *path,
                              GHashTable  *enums,
                              const gchar *gettext_domain)
{
  HyScanDataSchemaInternalKey *ikey = NULL;

  HyScanDataSchemaKeyType key_type = HYSCAN_DATA_SCHEMA_KEY_INVALID;
  HyScanDataSchemaViewType key_view = HYSCAN_DATA_SCHEMA_VIEW_DEFAULT;
  HyScanDataSchemaKeyAccess key_access = HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT;
  GList *enum_values = NULL;

  xmlNodePtr curnode;
  xmlChar *idx = NULL;
  xmlChar *namex = NULL;
  xmlChar *typex = NULL;
  xmlChar *viewx = NULL;
  xmlChar *enumx = NULL;
  xmlChar *accessx = NULL;
  xmlChar *descriptionx = NULL;
  xmlChar *default_valuex = NULL;
  xmlChar *minimum_valuex = NULL;
  xmlChar *maximum_valuex = NULL;
  xmlChar *value_stepx = NULL;
  gchar *key_id;

  /* Атрибуты id и name. */
  idx = xmlGetProp (node, (xmlChar *)"id");
  namex = xmlGetProp (node, (xmlChar *)"name");
  if (idx == NULL || namex == NULL)
    {
      g_warning ("HyScanDataSchema: unknown key id or name in node '%s'", path);
      goto exit;
    }

  if (!hyscan_data_schema_internal_validate_name ((gchar*)idx))
    {
      g_warning ("HyScanDataSchema: incorrect key name '%s' in node '%s'", idx, path);
      goto exit;
    }

  /* Атрибуты type или enum. */
  typex = xmlGetProp (node, (xmlChar *)"type");
  enumx = xmlGetProp (node, (xmlChar *)"enum");
  if ((typex == NULL && enumx == NULL) || (typex != NULL && enumx != NULL))
    {
      g_warning ("HyScanDataSchema: '%s%s' unknown key type", path, idx);
      goto exit;
    }

  /* Вид отображения. */
  viewx = xmlGetProp (node, (xmlChar *)"view");
  if (viewx != NULL)
    {
      if (g_ascii_strcasecmp ((const gchar *)viewx, "bin") == 0)
        key_view = HYSCAN_DATA_SCHEMA_VIEW_BIN;
      else if (g_ascii_strcasecmp ((const gchar *)viewx, "dec") == 0)
        key_view = HYSCAN_DATA_SCHEMA_VIEW_DEC;
      else if (g_ascii_strcasecmp ((const gchar *)viewx, "hex") == 0)
        key_view = HYSCAN_DATA_SCHEMA_VIEW_HEX;
      else if (g_ascii_strcasecmp ((const gchar *)viewx, "date") == 0)
        key_view = HYSCAN_DATA_SCHEMA_VIEW_DATE;
      else if (g_ascii_strcasecmp ((const gchar *)viewx, "time") == 0)
        key_view = HYSCAN_DATA_SCHEMA_VIEW_TIME;
      else if (g_ascii_strcasecmp ((const gchar *)viewx, "datetime") == 0)
        key_view = HYSCAN_DATA_SCHEMA_VIEW_DATE_TIME;
      else if (g_ascii_strcasecmp ((const gchar *)viewx, "schema") == 0)
        key_view = HYSCAN_DATA_SCHEMA_VIEW_SCHEMA;

      xmlFree (viewx);
    }

  /* Атрибуты параметра. */
  accessx = xmlGetProp (node, (xmlChar *)"access");
  if (accessx != NULL)
    {
      if (g_ascii_strcasecmp ((const gchar *)accessx, "readonly") == 0)
        key_access = HYSCAN_DATA_SCHEMA_ACCESS_READONLY;
      else if (g_ascii_strcasecmp ((const gchar *)accessx, "writeonly") == 0)
        key_access = HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY;

      xmlFree (accessx);
    }

  /* Проверка типа данных. */
  if (typex != NULL)
    {
      if (g_ascii_strcasecmp ((const gchar *)typex, "boolean") == 0)
        key_type = HYSCAN_DATA_SCHEMA_KEY_BOOLEAN;
      else if (g_ascii_strcasecmp ((const gchar *)typex, "integer") == 0)
        key_type = HYSCAN_DATA_SCHEMA_KEY_INTEGER;
      else if (g_ascii_strcasecmp ((const gchar *)typex, "double") == 0)
        key_type = HYSCAN_DATA_SCHEMA_KEY_DOUBLE;
      else if (g_ascii_strcasecmp ((const gchar *)typex, "string") == 0)
        key_type = HYSCAN_DATA_SCHEMA_KEY_STRING;
      else
        {
          g_warning ("HyScanDataSchema: unknown type '%s' in key '%s%s'", typex, path, idx);
          goto exit;
        }
    }
  else
    {
      enum_values = g_hash_table_lookup (enums, enumx);
      if (enum_values == NULL)
        {
          g_warning ("HyScanDataSchema: incorrect enum id '%s' in key '%s%s'", enumx, path, idx);
          goto exit;
        }
      key_type = HYSCAN_DATA_SCHEMA_KEY_ENUM;
    }

  /* Поле description. */
  for (curnode = node->children; curnode != NULL; curnode = curnode->next)
    if (curnode->type == XML_ELEMENT_NODE)
      if (g_ascii_strcasecmp ((gchar*)curnode->name, "description") == 0)
        {
          descriptionx = xmlNodeGetContent (curnode);
          break;
        }

  /* Поле default. */
  for (curnode = node->children; curnode != NULL; curnode = curnode->next)
    if (curnode->type == XML_ELEMENT_NODE)
      if (g_ascii_strcasecmp ((gchar*)curnode->name, "default") == 0)
        {
          default_valuex = xmlNodeGetContent (curnode);
          break;
        }

  /* Поле range. */
  for (curnode = node->children; curnode != NULL; curnode = curnode->next)
    if (curnode->type == XML_ELEMENT_NODE)
      if (g_ascii_strcasecmp ((gchar*)curnode->name, "range") == 0)
        {
          minimum_valuex = xmlGetProp (curnode, (xmlChar *)"min");
          maximum_valuex = xmlGetProp (curnode, (xmlChar *)"max");
          value_stepx = xmlGetProp (curnode, (xmlChar *)"step");
          break;
        }

  /* Описание параметра. */
  key_id= g_strdup_printf ("%s%s", path, (const gchar *)idx);
  ikey = hyscan_data_schema_internal_key_new (key_id,
                                              (const gchar *)namex,
                                              (const gchar *)descriptionx);
  g_free (key_id);

  ikey->type = key_type;
  ikey->view = key_view;
  ikey->access = key_access;

  switch (ikey->type)
    {
    case HYSCAN_DATA_SCHEMA_KEY_BOOLEAN:
      {
        gboolean default_value = FALSE;

        if (default_valuex != NULL)
          if (g_ascii_strcasecmp ((const gchar *)default_valuex, "true") == 0)
            default_value = TRUE;

        ikey->value_type = G_VARIANT_CLASS_BOOLEAN;
        ikey->default_value = g_variant_new_boolean (default_value);
      }
      break;

    case HYSCAN_DATA_SCHEMA_KEY_INTEGER:
      {
        gint64 default_value = 0;
        gint64 minimum_value = G_MININT64;
        gint64 maximum_value = G_MAXINT64;
        gint64 value_step = 1;

        if (default_valuex != NULL)
          default_value = g_ascii_strtoll ((const gchar *)default_valuex, NULL, 10);
        if (minimum_valuex != NULL)
          minimum_value = g_ascii_strtoll ((const gchar *)minimum_valuex, NULL, 10);
        if (maximum_valuex != NULL)
          maximum_value = g_ascii_strtoll ((const gchar *)maximum_valuex, NULL, 10);
        if (value_stepx != NULL)
          value_step = g_ascii_strtoll ((const gchar *)value_stepx, NULL, 10);

        if ((minimum_value > maximum_value) ||
            (default_value < minimum_value) ||
            (default_value > maximum_value))
          {
            g_warning ("HyScanDataSchema: default value out of range in key '%s'", ikey->id);
            hyscan_data_schema_internal_key_free (ikey);
            ikey = NULL;
            goto exit;
          }

        ikey->value_type = G_VARIANT_CLASS_INT64;
        ikey->default_value = g_variant_new_int64 (default_value);
        ikey->minimum_value = g_variant_new_int64 (minimum_value);
        ikey->maximum_value = g_variant_new_int64 (maximum_value);
        ikey->value_step = g_variant_new_int64 (value_step);
      }
      break;

    case HYSCAN_DATA_SCHEMA_KEY_DOUBLE:
      {
        gdouble default_value = 0.0;
        gdouble minimum_value = -G_MAXDOUBLE;
        gdouble maximum_value = G_MAXDOUBLE;
        gdouble value_step = 1.0;

        if (default_valuex != NULL)
          default_value = g_ascii_strtod ((const gchar *)default_valuex, NULL);
        if (minimum_valuex != NULL)
          minimum_value = g_ascii_strtod ((const gchar *)minimum_valuex, NULL);
        if (maximum_valuex != NULL)
          maximum_value = g_ascii_strtod ((const gchar *)maximum_valuex, NULL);
        if (value_stepx != NULL)
          value_step = g_ascii_strtod ((const gchar *)value_stepx, NULL);

        if ((minimum_value > maximum_value) ||
            (default_value < minimum_value) ||
            (default_value > maximum_value))
          {
            g_warning ("HyScanDataSchema: default value out of range in key '%s'", ikey->id);
            hyscan_data_schema_internal_key_free (ikey);
            ikey = NULL;
            goto exit;
          }

        ikey->value_type = G_VARIANT_CLASS_DOUBLE;
        ikey->default_value = g_variant_new_double (default_value);
        ikey->minimum_value = g_variant_new_double (minimum_value);
        ikey->maximum_value = g_variant_new_double (maximum_value);
        ikey->value_step = g_variant_new_double (value_step);
      }
      break;

    case HYSCAN_DATA_SCHEMA_KEY_STRING:
      {
        ikey->value_type = G_VARIANT_CLASS_STRING;

        if (default_valuex != NULL)
          ikey->default_value = g_variant_new_string ((const gchar *)default_valuex);
      }
      break;

    case HYSCAN_DATA_SCHEMA_KEY_ENUM:
      {
        gint64 default_value = 0;

        if (default_valuex != NULL)
          default_value = g_ascii_strtoll ((const gchar *)default_valuex, NULL, 10);

        if (!hyscan_data_schema_internal_enum_check (enum_values, default_value))
          {
            g_warning ("HyScanDataSchema: default value out of range in key '%s'", ikey->id);
            hyscan_data_schema_internal_key_free (ikey);
            ikey = NULL;
            goto exit;
          }

        ikey->enum_id = g_strdup ((const gchar*)enumx);
        ikey->value_type = G_VARIANT_CLASS_INT64;
        ikey->default_value = g_variant_new_int64 (default_value);
      }
      break;

    default:
      break;
    }

  if (ikey->default_value != NULL)
    g_variant_ref_sink (ikey->default_value);
  if (ikey->minimum_value != NULL)
    g_variant_ref_sink (ikey->minimum_value);
  if (ikey->maximum_value != NULL)
    g_variant_ref_sink (ikey->maximum_value);
  if (ikey->value_step != NULL)
    g_variant_ref_sink (ikey->value_step);

exit:
  xmlFree (idx);
  xmlFree (namex);
  xmlFree (typex);
  xmlFree (enumx);
  xmlFree (descriptionx);
  xmlFree (default_valuex);
  xmlFree (minimum_valuex);
  xmlFree (maximum_valuex);
  xmlFree (value_stepx);

  return ikey;
}

/* Функция обрабатывает описание ветки схемы. */
static gboolean
hyscan_data_schema_parse_node (xmlNodePtr                 node,
                               const gchar               *schema_path,
                               const gchar               *path,
                               GHashTable                *keys,
                               GHashTable                *enums,
                               const gchar               *gettext_domain)
{
  for (; node != NULL; node = node->next)
    {
      gboolean status = TRUE;

      xmlChar *id = xmlGetProp (node, (xmlChar *)"id");
      xmlChar *schema = xmlGetProp (node, (xmlChar *)"schema");

      if (node->type != XML_ELEMENT_NODE)
        continue;

      if (id == NULL)
        {
          g_warning ("HyScanDataSchema: unknown element id in '%s'", path);
          return FALSE;
        }

      if (!hyscan_data_schema_internal_validate_name ((gchar*)id))
        {
          g_warning ("HyScanDataSchema: incorrect node name '%s' in node '%s'", id, path);
          return FALSE;
        }

      if (g_ascii_strcasecmp ((gchar*)node->name, "key") == 0)
        {
          HyScanDataSchemaInternalKey *ikey = hyscan_data_schema_parse_key (node, path,
                                                                            enums, gettext_domain);
          if (ikey == NULL)
            {
              status = FALSE;
            }
          else
            {
              if (g_hash_table_contains (keys, ikey->id))
                {
                  g_warning ("HyScanDataSchema: duplicated key '%s'", ikey->id);
                  status = FALSE;
                }
              else
                {
                g_hash_table_insert (keys, ikey->id, ikey);
                }
            }
        }

      if (g_ascii_strcasecmp ((gchar*)node->name, "node") == 0)
        {
          gchar *cpath = g_strdup_printf ("%s%s/", path, id);
          if (schema != NULL)
            {
              status = hyscan_data_schema_parse_schema (node->doc,
                                                        schema_path, (gchar*)schema, cpath,
                                                        keys, enums,
                                                        gettext_domain);
            }
          else
            {
              status = hyscan_data_schema_parse_node (node->children,
                                                      schema_path, cpath,
                                                      keys, enums,
                                                      gettext_domain);
            }
          g_free (cpath);
        }

      xmlFree (schema);
      xmlFree (id);

      if (!status)
        return FALSE;
    }

  return TRUE;
}

/* Функция обрабатывает описание схемы. */
static gboolean
hyscan_data_schema_parse_schema (xmlDocPtr                  doc,
                                 const gchar               *schema_path,
                                 const gchar               *schema,
                                 const gchar               *path,
                                 GHashTable                *keys,
                                 GHashTable                *enums,
                                 const gchar               *gettext_domain)
{
  xmlNodePtr node;
  gchar **schemas;
  gint i;

  schemas = g_strsplit (schema_path, "/", 0);
  for (i = 0; schemas != NULL && schemas[i] != NULL; i++)
    if (g_strcmp0 (schemas[i], schema) == 0)
      {
        g_warning ("HyScanDataSchema: node '%s' loops back to schema '%s'", path, schema);
        g_strfreev (schemas);
        return FALSE;
      }
  g_strfreev (schemas);

  for (node = doc->children; node != NULL; node = node->next)
    if (node->type == XML_ELEMENT_NODE)
      if (g_ascii_strcasecmp ((gchar*)node->name, "schemalist") == 0)
        break;

  if (node == NULL)
    return FALSE;

  for (node = node->children; node != NULL; node = node->next)
    {
      xmlChar *id;
      gchar *cpath;
      gboolean status;

      if (node->type != XML_ELEMENT_NODE)
        continue;
      if (g_ascii_strcasecmp ((gchar*)node->name, "schema") != 0)
        continue;

      id = xmlGetProp (node, (xmlChar *)"id");
      if (g_ascii_strcasecmp ((gchar*)id, schema) != 0)
        {
          xmlFree (id);
          continue;
        }
      xmlFree (id);

      cpath = g_strdup_printf ("%s%s/", schema_path, (gchar*)schema);
      status = hyscan_data_schema_parse_node (node->children,
                                              cpath, path,
                                              keys, enums,
                                              gettext_domain);
      if (!status)
        {
          g_free (cpath);
          return FALSE;
        }
      g_free (cpath);

      return TRUE;
    }

  g_warning ("HyScanDataSchema: unknown schema '%s' in '%s'", schema, path);

  return FALSE;
}

/**
 * hyscan_data_schema_new_from_string:
 * @schema_data: строка с описанием схемы в формате XML
 * @schema_id: идентификатор загружаемой схемы
 *
 * Функция создаёт новый объект #HyScanDataSchema из описания схемы в виде
 * строки с XML данными.
 *
 * Returns: #HyScanDataSchema. Для удаления #g_object_unref.
 */
HyScanDataSchema *
hyscan_data_schema_new_from_string (const gchar *schema_data,
                                    const gchar *schema_id)
{
  return g_object_new (HYSCAN_TYPE_DATA_SCHEMA,
                       "schema-data", schema_data,
                       "schema-id", schema_id,
                       NULL);
}

/**
 * hyscan_data_schema_new_from_file:
 * @schema_path: путь к XML файлу с описанием схемы
 * @schema_id: идентификатор загружаемой схемы
 *
 * Функция создаёт новый объект #HyScanDataSchema из описания схемы в XML файле.
 *
 * Returns: #HyScanDataSchema. Для удаления #g_object_unref.
 */
HyScanDataSchema *
hyscan_data_schema_new_from_file (const gchar *schema_path,
                                  const gchar *schema_id)
{
  gchar *schema_data;
  gpointer schema;

  if (!g_file_get_contents (schema_path, &schema_data, NULL, NULL))
    return NULL;

  schema = g_object_new (HYSCAN_TYPE_DATA_SCHEMA,
                         "schema-data", schema_data,
                         "schema-id", schema_id,
                         NULL);
  g_free (schema_data);

  return schema;
}

/**
 * hyscan_data_schema_new_from_resource:
 * @schema_resource: путь к ресурсу GResource
 * @schema_id: идентификатор загружаемой схемы
 *
 * Функция создаёт новый объект #HyScanDataSchema из описания схемы в виде
 * строки с XML данными загружаемой из ресурсов.
 *
 * Returns: #HyScanDataSchema. Для удаления #g_object_unref.
 */
HyScanDataSchema *
hyscan_data_schema_new_from_resource (const gchar *schema_resource,
                                      const gchar *schema_id)
{
  const gchar *schema_data;
  GBytes *resource;
  gpointer schema;

  resource = g_resources_lookup_data (schema_resource, 0, NULL);
  if (resource == NULL)
    return NULL;

  schema_data = g_bytes_get_data (resource, NULL);
  schema = g_object_new (HYSCAN_TYPE_DATA_SCHEMA,
                         "schema-data", schema_data,
                         "schema-id", schema_id,
                         NULL);
  g_bytes_unref (resource);

  return schema;
}

/**
 * hyscan_data_schema_get_data:
 * @schema: указатель на #HyScanDataSchema
 *
 * Функция возвращает описание загруженной схемы данных в виде строки с XML данными.
 *
 * Returns: (transfer none): Описание загруженной схемы данных.
 */
const gchar *
hyscan_data_schema_get_data (HyScanDataSchema *schema)
{
  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), NULL);

  return schema->priv->schema_data;
}

/**
 * hyscan_data_schema_get_id:
 * @schema: указатель на #HyScanDataSchema
 *
 * Функция возвращает идентификатор загруженной схемы данных.
 *
 * Returns: (transfer none): Идентификатор загруженной схемы данных.
 */
const gchar *
hyscan_data_schema_get_id (HyScanDataSchema *schema)
{
  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), NULL);

  return schema->priv->schema_id;
}

/**
 * hyscan_data_schema_list_keys:
 * @schema: указатель на #HyScanDataSchema
 *
 * Функция возвращает список параметров определённых в схеме.
 *
 * Returns: (transfer full): Список параметров в схеме. Для удаления #g_strfreev.
 */
gchar **
hyscan_data_schema_list_keys (HyScanDataSchema *schema)
{
  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), NULL);

  return g_strdupv (schema->priv->keys_list);
}

/**
 * hyscan_data_schema_list_nodes:
 * @schema: указатель на #HyScanDataSchema
 *
 * Функция возвращает иеархический список узлов и параметров определённых в схеме.
 *
 * Returns: (transfer full): Список параметров в схеме или NULL.
 * Для удаления #hyscan_data_schema_node_free.
 */
HyScanDataSchemaNode *
hyscan_data_schema_list_nodes (HyScanDataSchema *schema)
{
  HyScanDataSchemaPrivate *priv;
  HyScanDataSchemaNode *node;
  guint i;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), NULL);

  priv = schema->priv;

  if (priv->keys_list == NULL)
    return NULL;

  node = hyscan_data_schema_node_new ("", NULL, NULL);
  for (i = 0; priv->keys_list[i] != NULL; i++)
    {
      HyScanDataSchemaInternalKey *ikey;
      HyScanDataSchemaKey *key;

      ikey = g_hash_table_lookup (priv->keys, priv->keys_list[i]);
      key = hyscan_data_schema_key_new (ikey->id, ikey->name, ikey->description,
                                        ikey->type, ikey->view, ikey->access);

      hyscan_data_schema_internal_node_insert_key (node, key);
    }

  return node;
}

/**
 * hyscan_data_schema_has_key:
 * @schema: указатель на #HyScanDataSchema
 * @key_id: идентификатор параметра
 *
 * Функция проверяет существование параметра в схеме.
 *
 * Returns: %TRUE - если параметр присутствует в схеме, иначе %FALSE.
 */
gboolean
hyscan_data_schema_has_key (HyScanDataSchema *schema,
                            const gchar      *key_id)
{
  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), FALSE);

  return g_hash_table_contains (schema->priv->keys, key_id);
}

/**
 * hyscan_data_schema_key_get_name:
 * @schema: указатель на #HyScanDataSchema
 * @key_id: идентификатор параметра
 *
 * Функция возвращает название параметра.
 *
 * Returns: Название параметра или NULL.
 */
const gchar *
hyscan_data_schema_key_get_name (HyScanDataSchema *schema,
                                 const gchar      *key_id)
{
  HyScanDataSchemaInternalKey *ikey;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), NULL);

  ikey = g_hash_table_lookup (schema->priv->keys, key_id);
  if (ikey == NULL)
    return NULL;

  return ikey->name;
}

/**
 * hyscan_data_schema_key_get_description:
 * @schema: указатель на #HyScanDataSchema
 * @key_id: идентификатор параметра
 *
 * Функция возвращает описание параметра.
 *
 * Returns: Описание параметра или NULL.
 */
const gchar *
hyscan_data_schema_key_get_description (HyScanDataSchema *schema,
                                        const gchar      *key_id)
{
  HyScanDataSchemaInternalKey *ikey;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), NULL);

  ikey = g_hash_table_lookup (schema->priv->keys, key_id);
  if (ikey == NULL)
    return NULL;

  return ikey->description;
}

/**
 * hyscan_data_schema_key_get_value_type:
 * @schema: указатель на #HyScanDataSchema
 * @key_id: идентификатор параметра
 *
 * Функция возвращает тип параметра.
 *
 * Returns: Тип параметра.
 */
HyScanDataSchemaKeyType
hyscan_data_schema_key_get_value_type (HyScanDataSchema *schema,
                                       const gchar      *key_id)
{
  HyScanDataSchemaInternalKey *ikey;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), HYSCAN_DATA_SCHEMA_KEY_INVALID);

  ikey = g_hash_table_lookup (schema->priv->keys, key_id);
  if (ikey == NULL)
    return HYSCAN_DATA_SCHEMA_KEY_INVALID;

  return ikey->type;
}

/**
 * hyscan_data_schema_key_get_view:
 * @schema: указатель на #HyScanDataSchema
 * @key_id: идентификатор параметра
 *
 * Функция возвращает рекомендуемый вид отображения параметра.
 *
 * Returns: Рекомендуемый вид отображения параметра.
 */
HyScanDataSchemaViewType
hyscan_data_schema_key_get_view (HyScanDataSchema *schema,
                                 const gchar      *key_id)
{
  HyScanDataSchemaInternalKey *ikey;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), HYSCAN_DATA_SCHEMA_VIEW_DEFAULT);

  ikey = g_hash_table_lookup (schema->priv->keys, key_id);
  if (ikey == NULL)
    return HYSCAN_DATA_SCHEMA_VIEW_DEFAULT;

  return ikey->view;
}

/**
 * hyscan_data_schema_key_get_access:
 * @schema: указатель на #HyScanDataSchema
 * @key_id: идентификатор параметра
 *
 * Функция возвращает атрибуты доступа к параметру.
 *
 * Returns: Атрибуты доступа к параметру.
 */
HyScanDataSchemaKeyAccess
hyscan_data_schema_key_get_access (HyScanDataSchema *schema,
                                   const gchar      *key_id)
{
  HyScanDataSchemaInternalKey *ikey;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), TRUE);

  ikey = g_hash_table_lookup (schema->priv->keys, key_id);
  if (ikey == NULL)
    return TRUE;

  return ikey->access;
}

/**
 * hyscan_data_schema_key_get_enum_id:
 * @schema: указатель на #HyScanDataSchema
 * @key_id: идентификатор параметра
 *
 * Функция возвращает идентификатор списка допустимых значений для параметра с типом ENUM.
 *
 * Returns: Идентификатор списка значений или NULL.
 */
const gchar *
hyscan_data_schema_key_get_enum_id (HyScanDataSchema *schema,
                                    const gchar      *key_id)
{
  HyScanDataSchemaInternalKey *ikey;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), NULL);

  ikey = g_hash_table_lookup (schema->priv->keys, key_id);
  if (ikey == NULL || ikey->type != HYSCAN_DATA_SCHEMA_KEY_ENUM)
    return NULL;

  return ikey->enum_id;
}

/**
 * hyscan_data_schema_key_get_default:
 * @schema: указатель на #HyScanDataSchema
 * @key_id: идентификатор параметра
 *
 * Функция возвращает значение параметра по умолчанию.
 *
 * Returns: Значение параметра по умолчанию или NULL. Для удаления #g_variant_unref.
 */
GVariant *
hyscan_data_schema_key_get_default (HyScanDataSchema *schema,
                                    const gchar      *key_id)
{
  HyScanDataSchemaInternalKey *ikey;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), FALSE);

  ikey = g_hash_table_lookup (schema->priv->keys, key_id);
  if (ikey == NULL || ikey->default_value == NULL)
    return NULL;

  return g_variant_ref (ikey->default_value);
}

/**
 * hyscan_data_schema_key_get_minimum:
 * @schema: указатель на #HyScanDataSchema
 * @key_id: идентификатор параметра
 *
 * Функция возвращает минимальное значение параметра для типов INTEGER и DOUBLE.
 *
 * Returns: Минимальное значение параметра или NULL. Для удаления #g_variant_unref.
 */
GVariant *
hyscan_data_schema_key_get_minimum (HyScanDataSchema *schema,
                                    const gchar      *key_id)
{
  HyScanDataSchemaInternalKey *ikey;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), FALSE);

  ikey = g_hash_table_lookup (schema->priv->keys, key_id);
  if (ikey == NULL || ikey->minimum_value == NULL)
    return NULL;

  return g_variant_ref (ikey->minimum_value);
}

/**
 * hyscan_data_schema_key_get_maximum:
 * @schema: указатель на #HyScanDataSchema
 * @key_id: идентификатор параметра
 *
 * Функция возвращает максимальное значение параметра для типов INTEGER и DOUBLE.
 *
 * Returns: Максимальное значение параметра или NULL. Для удаления #g_variant_unref.
 */
GVariant *
hyscan_data_schema_key_get_maximum (HyScanDataSchema *schema,
                                    const gchar      *key_id)
{
  HyScanDataSchemaInternalKey *ikey;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), FALSE);

  ikey = g_hash_table_lookup (schema->priv->keys, key_id);
  if (ikey == NULL || ikey->maximum_value == NULL)
    return NULL;

  return g_variant_ref (ikey->maximum_value);
}

/**
 * hyscan_data_schema_key_get_step:
 * @schema: указатель на #HyScanDataSchema
 * @key_id: идентификатор параметра
 *
 * Функция возвращает рекомендуемый шаг изменения значения параметра для типов INTEGER и DOUBLE.
 *
 * Returns: Рекомендуемый шаг изменения значения параметра или NULL. Для удаления #g_variant_unref.
 */
GVariant *
hyscan_data_schema_key_get_step (HyScanDataSchema *schema,
                                 const gchar      *key_id)
{
  HyScanDataSchemaInternalKey *ikey;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), FALSE);

  ikey = g_hash_table_lookup (schema->priv->keys, key_id);
  if (ikey == NULL || ikey->value_step == NULL)
    return NULL;

  return g_variant_ref (ikey->value_step);
}

/**
 * hyscan_data_schema_key_check:
 * @schema: указатель на #HyScanDataSchema
 * @key_id: идентификатор параметра
 * @value: значение параметра
 *
 * Функция проверяет значение параметра на предмет нахождения в допустимом диапазоне.
 *
 * Returns: %TRUE - если значение находится в допустимых пределах, иначе %FALSE.
 */
gboolean
hyscan_data_schema_key_check (HyScanDataSchema *schema,
                              const gchar      *key_id,
                              GVariant         *value)
{
  HyScanDataSchemaInternalKey *ikey;
  GList *enum_values;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), FALSE);

  ikey = g_hash_table_lookup (schema->priv->keys, key_id);
  if (ikey == NULL)
    return FALSE;

  if (value != NULL && g_variant_classify (value) != ikey->value_type)
    return FALSE;

  switch (ikey->type)
    {
    case HYSCAN_DATA_SCHEMA_KEY_INTEGER:
      if (g_variant_get_int64 (value) < g_variant_get_int64 (ikey->minimum_value))
        return FALSE;
      if (g_variant_get_int64 (value) > g_variant_get_int64 (ikey->maximum_value))
        return FALSE;
      return TRUE;

    case HYSCAN_DATA_SCHEMA_KEY_DOUBLE:
      if (g_variant_get_double (value) < g_variant_get_double (ikey->minimum_value))
        return FALSE;
      if (g_variant_get_double (value) > g_variant_get_double (ikey->maximum_value))
        return FALSE;
      return TRUE;

    case HYSCAN_DATA_SCHEMA_KEY_ENUM:
      enum_values = g_hash_table_lookup (schema->priv->enums, ikey->enum_id);
      return hyscan_data_schema_internal_enum_check (enum_values, g_variant_get_int64 (value));

    default:
      break;
    }

  return TRUE;
}

/**
 * hyscan_data_schema_get_enum_values:
 * @schema: указатель на #HyScanDataSchema
 * @enum_id: идентификатор списка значений для ENUM параметра
 *
 * Функция возвращает варианты допустимых значений для указанного
 * идентификатора списка значений.
 *
 * Returns: (transfer container) (element-type HyScanDataSchemaEnumValue):
 *          Список допустимых значений параметра или NULL.
 */
GList *
hyscan_data_schema_get_enum_values (HyScanDataSchema *schema,
                                    const gchar      *enum_id)
{
  GList *enum_values;
  GList *new_enum_values;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), NULL);

  enum_values = g_hash_table_lookup (schema->priv->enums, enum_id);
  if (enum_values == NULL)
    return NULL;

  new_enum_values = NULL;
  while (enum_values != NULL)
    {
      new_enum_values = g_list_prepend (new_enum_values, enum_values->data);
      enum_values = enum_values->next;
    }

  return g_list_reverse (new_enum_values);
}

/**
 * hyscan_data_schema_enum_value_new:
 * @value: численное значение параметра
 * @name: название значения параметра
 * @description: описание значения параметра
 *
 * Функция создаёт новую структуру #HyScanDataSchemaEnumValue и заполняет её поля.
 *
 * Returns: (transfer full): Новая структура #HyScanDataSchemaEnumValue.
 * Для удаления #hyscan_data_schema_enum_value_free.
 */
HyScanDataSchemaEnumValue *
hyscan_data_schema_enum_value_new (gint64       value,
                                   const gchar *name,
                                   const gchar *description)
{
  HyScanDataSchemaEnumValue new_value;

  new_value.value = value;
  new_value.name = name;
  new_value.description = description;

  return hyscan_data_schema_enum_value_copy (&new_value);
}

/**
 * hyscan_data_schema_enum_value_copy:
 * @value: структура #HyScanDataSchemaEnumValue для копирования
 *
 * Функция создаёт копию структуры #HyScanDataSchemaEnumValue.
 *
 * Returns: (transfer full): Новая структура #HyScanDataSchemaEnumValue.
 * Для удаления #hyscan_data_schema_enum_value_free.
 */
HyScanDataSchemaEnumValue *
hyscan_data_schema_enum_value_copy (HyScanDataSchemaEnumValue *value)
{
  HyScanDataSchemaEnumValue *new_value;

  new_value = g_slice_new (HyScanDataSchemaEnumValue);
  new_value->value = value->value;
  new_value->name = g_strdup (value->name);
  new_value->description = g_strdup (value->description);

  return new_value;
}

/**
 * hyscan_data_schema_enum_value_free:
 * @value: структура #HyScanDataSchemaEnumValue для удаления
 *
 * Функция удаляет структуру #HyScanDataSchemaEnumValue.
 */
void
hyscan_data_schema_enum_value_free (HyScanDataSchemaEnumValue *value)
{
  if (value == NULL)
    return;

  g_free ((gchar*)value->name);
  g_free ((gchar*)value->description);

  g_slice_free (HyScanDataSchemaEnumValue, value);
}

/**
 * hyscan_data_schema_node_new:
 * @path: путь до узла
 * @nodes: (element-type HyScanDataSchemaNode) (transfer none): дочерние узлы
 * @keys: (element-type HyScanDataSchemaKey) (transfer none): параметры
 *
 * Функция создаёт новую структуру #HyScanDataSchemaNode и заполняет её поля.
 *
 * Returns: (transfer full): Новая структура #HyScanDataSchemaNode.
 * Для удаления #hyscan_data_schema_node_free.
 */
HyScanDataSchemaNode *
hyscan_data_schema_node_new (const gchar *path,
                             GList       *nodes,
                             GList       *keys)
{
  HyScanDataSchemaNode new_node;

  new_node.path = path;
  new_node.nodes = nodes;
  new_node.keys = keys;

  return hyscan_data_schema_node_copy (&new_node);
}

/**
 * hyscan_data_schema_node_copy:
 * @node: структура #HyScanDataSchemaNode для копирования
 *
 * Функция создаёт копию структуры #HyScanDataSchemaNode.
 *
 * Returns: (transfer full): Новая структура #HyScanDataSchemaNode.
 * Для удаления #hyscan_data_schema_node_free.
 */
HyScanDataSchemaNode *
hyscan_data_schema_node_copy (HyScanDataSchemaNode *node)
{
  HyScanDataSchemaNode *new_node;
  GList *nodes, *new_nodes;
  GList *keys, *new_keys;

  new_node = g_slice_new (HyScanDataSchemaNode);
  new_node->path = g_strdup (node->path);

  nodes = node->nodes;
  new_nodes = NULL;
  while (nodes != NULL)
    {
      new_nodes = g_list_prepend (new_nodes, hyscan_data_schema_node_copy (nodes->data));
      nodes = nodes->next;
    }
  new_node->nodes = g_list_reverse (new_nodes);

  keys = node->keys;
  new_keys = NULL;
  while (keys != NULL)
    {
      new_keys = g_list_prepend (new_keys, hyscan_data_schema_key_copy (keys->data));
      keys = keys->next;
    }
  new_node->keys = g_list_reverse (new_keys);

  return new_node;
}

/**
 * hyscan_data_schema_node_free:
 * @node: структура #HyScanDataSchemaNode для удаления
 *
 * Функция удаляет структуру #HyScanDataSchemaNode.
 */
void
hyscan_data_schema_node_free (HyScanDataSchemaNode *node)
{
  if (node == NULL)
    return;

  g_list_free_full (node->nodes, (GDestroyNotify)hyscan_data_schema_node_free);
  g_list_free_full (node->keys, (GDestroyNotify)hyscan_data_schema_key_free);

  g_free ((gchar*)node->path);

  g_slice_free (HyScanDataSchemaNode, node);
}

/**
 * hyscan_data_schema_key_new:
 * @id: идентификатор параметра
 * @name: название параметра
 * @description: описание параметра
 * @type: тип параметра
 * @view: рекомендуемый вид отображения параметра
 * @access: атрибуты доступа к параметру
 *
 * Функция создаёт новую структуру #HyScanDataSchemaKey и заполняет её поля.
 *
 * Returns: (transfer full): Новая структура #HyScanDataSchemaKey.
 * Для удаления #hyscan_data_schema_key_free.
 */
HyScanDataSchemaKey *
hyscan_data_schema_key_new (const gchar               *id,
                            const gchar               *name,
                            const gchar               *description,
                            HyScanDataSchemaKeyType    type,
                            HyScanDataSchemaViewType   view,
                            HyScanDataSchemaKeyAccess  access)
{
  HyScanDataSchemaKey new_key;

  new_key.id = id;
  new_key.name = name;
  new_key.description = description;
  new_key.type = type;
  new_key.view = view;
  new_key.access = access;

  return hyscan_data_schema_key_copy (&new_key);
}

/**
 * hyscan_data_schema_key_copy:
 * @key: структура #HyScanDataSchemaKey для копирования
 *
 * Функция создаёт копию структуры #HyScanDataSchemaKey.
 *
 * Returns: (transfer full): Новая структура #HyScanDataSchemaKey.
 * Для удаления #hyscan_data_schema_key_free.
 */
HyScanDataSchemaKey *
hyscan_data_schema_key_copy (HyScanDataSchemaKey *key)
{
  HyScanDataSchemaKey *new_key;

  new_key = g_slice_new (HyScanDataSchemaKey);
  new_key->id = g_strdup (key->id);
  new_key->name = g_strdup (key->name);
  new_key->description = g_strdup (key->description);
  new_key->type = key->type;
  new_key->view = key->view;
  new_key->access = key->access;

  return new_key;
}

/**
 * hyscan_data_schema_key_free:
 * @key: структура #HyScanDataSchemaKey для удаления
 *
 * Функция удаляет структуру #HyScanDataSchemaKey.
 */
void
hyscan_data_schema_key_free (HyScanDataSchemaKey *key)
{
  if (key == NULL)
    return;

  g_free ((gchar*)key->id);
  g_free ((gchar*)key->name);
  g_free ((gchar*)key->description);

  g_slice_free (HyScanDataSchemaKey, key);
}
