/**
 * \file hyscan-data-schema.c
 *
 * \brief Исходный файл класса схемы данных.
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2016
 * \license Проприетарная лицензия ООО "Экран"
 *
 */

#include "hyscan-data-schema.h"
#include "hyscan-data-schema-overrides.h"

#include <gio/gio.h>
#include <glib/gi18n.h>
#include <libxml/parser.h>
#include <string.h>

enum
{
  PROP_O,
  PROP_SCHEMA_DATA,
  PROP_SCHEMA_ID,
  PROP_OVERRIDES_DATA,
};

/* Варианты значений перечисляемого типа. */
typedef struct
{
  gchar                       *id;                     /* Идентификатор значений перечисляемого типа. */

  HyScanDataSchemaEnumValue  **values;                 /* Варианты значений для перечисляемого типа. */
} HyScanDataSchemaEnum;

/* Данные параметра. */
typedef struct
{
  gchar                       *id;                     /* Идентификатор параметра. */

  HyScanDataSchemaType         type;                   /* Тип параметра. */
  gchar                       *name;                   /* Название параметра. */
  gchar                       *description;            /* Описание параметра. */
  gboolean                     readonly;               /* Параметр доступен только для чтения. */

  HyScanDataSchemaEnum        *enum_values;            /* Варианты значений для перечисляемого типа. */
  gint64                       default_value;          /* Значение по умолчанию. */
  gint64                       minimum_value;          /* Минимально возможное значение. */
  gint64                       maximum_value;          /* Маскимально возможное значение. */
  gint64                       value_step;             /* Шаг изменения значения. */
} HyScanDataSchemaKey;

struct _HyScanDataSchemaPrivate
{
  gchar                       *schema_data;            /* Описание схемы в формате XML. */
  gchar                       *schema_id;              /* Идентификатор используемой схемы. */
  gchar                       *overrides_data;         /* Переопределения схемы в формате INI. */
  gchar                       *gettext_domain;         /* Название домена переводов. */

  HyScanDataSchemaOverrides   *overrides;              /* Переопределения схемы. */

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

static void            hyscan_data_schema_value_set_boolean    (gpointer                    data,
                                                                gboolean                    value);
static void            hyscan_data_schema_value_set_integer    (gpointer                    data,
                                                                gint64                      value);
static void            hyscan_data_schema_value_set_double     (gpointer                    data,
                                                                gdouble                     value);
static void            hyscan_data_schema_value_set_string     (gpointer                    data,
                                                                const gchar                *value);

static gboolean        hyscan_data_schema_value_get_boolean    (gconstpointer               data);
static gint64          hyscan_data_schema_value_get_integer    (gconstpointer               data);
static gdouble         hyscan_data_schema_value_get_double     (gconstpointer               data);
static const gchar    *hyscan_data_schema_value_get_string     (gconstpointer               data);

static gboolean        hyscan_data_schema_validate_name        (const gchar                *name);

static gint            hyscan_data_schema_compare_keys         (HyScanDataSchemaKey        *key1,
                                                                HyScanDataSchemaKey        *key2,
                                                                gpointer                    user_data);

static void            hyscan_data_schema_free_enum            (HyScanDataSchemaEnum       *values);
static HyScanDataSchemaEnumValue
                      *hyscan_data_schema_parse_enum_value     (xmlNodePtr                  node,
                                                                const gchar                *gettext_domain);
static HyScanDataSchemaEnum
                      *hyscan_data_schema_parse_enum_values    (xmlNodePtr                  node,
                                                                const gchar                *gettext_domain);
static HyScanDataSchemaEnum
               *hyscan_data_schema_parse_overrided_enum_values (HyScanDataSchemaOverrides  *overrides,
                                                                const gchar                *enum_id);
static gboolean        hyscan_data_schema_check_enum           (HyScanDataSchemaEnum       *enums,
                                                                gint64                      value);

static void            hyscan_data_schema_free_key             (HyScanDataSchemaKey        *key);
static HyScanDataSchemaKey *hyscan_data_schema_parse_key       (xmlNodePtr                  node,
                                                                const gchar                *path,
                                                                GHashTable                 *enums,
                                                                HyScanDataSchemaOverrides  *overrides,
                                                                const gchar                *gettext_domain);
static gboolean        hyscan_data_schema_parse_node           (xmlNodePtr                  node,
                                                                const gchar                *schema_path,
                                                                const gchar                *path,
                                                                GHashTable                 *keys,
                                                                GHashTable                 *enums,
                                                                HyScanDataSchemaOverrides  *overrides,
                                                                const gchar                *gettext_domain);
static gboolean        hyscan_data_schema_parse_schema         (xmlDocPtr                   doc,
                                                                const gchar                *schema_path,
                                                                const gchar                *schema,
                                                                const gchar                *path,
                                                                GHashTable                 *keys,
                                                                GHashTable                 *enums,
                                                                HyScanDataSchemaOverrides  *overrides,
                                                                const gchar                *gettext_domain);

static HyScanDataSchemaNode *hyscan_data_schema_new_node       (const gchar                *path);
static HyScanDataSchemaParam *hyscan_data_schema_new_param     (const gchar                *id,
                                                                const gchar                *name,
                                                                const gchar                *description,
                                                                HyScanDataSchemaType        type,
                                                                gboolean                    readonly);
static void            hyscan_data_schema_insert_param         (HyScanDataSchemaNode       *node,
                                                                const gchar                *id,
                                                                const gchar                *name,
                                                                const gchar                *description,
                                                                HyScanDataSchemaType        type,
                                                                gboolean                    readonly);

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

  g_object_class_install_property (object_class, PROP_OVERRIDES_DATA,
    g_param_spec_string ("overrides-data", "OverridesData", "Overrides data", NULL,
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

    case PROP_OVERRIDES_DATA:
      priv->overrides_data = g_value_dup_string (value);
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
                                       (GDestroyNotify)hyscan_data_schema_free_key);

  /* Таблица enum типов и их значений. */
  priv->enums = g_hash_table_new_full (g_str_hash,
                                       g_str_equal,
                                       NULL,
                                       (GDestroyNotify)hyscan_data_schema_free_enum);

  /* Разбор описания схемы. */
  if (priv->schema_data == NULL)
    return;
  doc = xmlParseMemory (priv->schema_data, strlen (priv->schema_data));
  if (doc == NULL)
    return;

  /* Переопределения схемы. */
  if (priv->overrides_data != NULL)
    priv->overrides = hyscan_data_schema_overrides_new_from_data (priv->overrides_data);
  else
    priv->overrides = hyscan_data_schema_overrides_new ();

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
          HyScanDataSchemaEnum *values;

          /* Загружаем в первую очередь переопределённые значения. */
          values = hyscan_data_schema_parse_overrided_enum_values (priv->overrides, (gchar*)id);
          if (values == NULL)
            values = hyscan_data_schema_parse_enum_values (node, priv->gettext_domain);

          g_hash_table_insert (priv->enums, values->id, values);
        }
      else
        {
          g_warning ("HyScanDataSchema: duplicated enum type %s", id);
        }

      xmlFree (id);
    }

  /* Обработка описания схемы. */
  status = hyscan_data_schema_parse_schema (doc,
                                            "/",
                                            priv->schema_id,
                                            "/",
                                            priv->keys,
                                            priv->enums,
                                            priv->overrides,
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
      HyScanDataSchemaKey *key;
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
  g_object_unref (priv->overrides);
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
  g_free (priv->overrides_data);
  g_free (priv->gettext_domain);

  G_OBJECT_CLASS (hyscan_data_schema_parent_class)->finalize (object);
}

/* Функция записывает в область памяти data значение типа gboolean. */
static void
hyscan_data_schema_value_set_boolean (gpointer data,
                                      gboolean value)
{
  *(gboolean*)data = value;
}

/* Функция записывает в область памяти data значение типа gint64. */
static void
hyscan_data_schema_value_set_integer (gpointer data,
                                      gint64   value)
{
  *(gint64*)data = value;
}

/* Функция записывает в область памяти data значение типа gdouble. */
static void
hyscan_data_schema_value_set_double (gpointer data,
                                     gdouble  value)
{
  *(gdouble*)data = value;
}

/* Функция записывает в область памяти data указатель на копию строки. */
static void
hyscan_data_schema_value_set_string (gpointer     data,
                                     const gchar *value)
{
  gpointer *p = data;
  g_free (*p);
  *p = g_strdup (value);
}

/* Функция считывает из области памяти data значение типа gboolean. */
static gboolean
hyscan_data_schema_value_get_boolean (gconstpointer data)
{
  return *(gboolean*)data;
}

/* Функция считывает из области памяти data значение типа gint64. */
static gint64
hyscan_data_schema_value_get_integer (gconstpointer data)
{
  return *(gint64*)data;
}

/* Функция считывает из области памяти data значение типа gdouble. */
static gdouble
hyscan_data_schema_value_get_double (gconstpointer data)
{
  return *(gdouble*)data;
}

/* Функция считывает из области памяти data указатель на строку. */
static const gchar *
hyscan_data_schema_value_get_string (gconstpointer data)
{
  gconstpointer const *p = data;
  return *p;
}

/* Функция проверяет имя на предмет допустимости. */
static gboolean
hyscan_data_schema_validate_name (const gchar *name)
{
  gint i;

  for (i = 0; name[i] != '\0'; i++)
    {
      gboolean match = FALSE;

      if (name[i] >= 'a' && name[i] <= 'z')
        match = TRUE;
      if (name[i] >= 'A' && name[i] <= 'Z')
        match = TRUE;
      if (name[i] >= '0' && name[i] <= '9')
        match = TRUE;
      if (name[i] == '-' || name[i] == '.')
        match = TRUE;

      if (!match)
        return FALSE;
    }

  return TRUE;
}

/* Функция сравнения двух параметров по идентификаторам (для сортировки списка). */
static gint
hyscan_data_schema_compare_keys (HyScanDataSchemaKey *key1,
                                 HyScanDataSchemaKey *key2,
                                 gpointer             user_data)
{
  return g_strcmp0 (key1->id, key2->id);
}

/* Функция освобождает память занятую структурой со значениями типа enum. */
static void
hyscan_data_schema_free_enum (HyScanDataSchemaEnum *values)
{
  gint i;

  for (i = 0; values->values[i] != NULL; i++)
    {
      g_free (values->values[i]->name);
      g_free (values->values[i]->description);
      g_free (values->values[i]);
    }

  g_free (values->values);
  g_free (values->id);
  g_free (values);
}

/* Функция обрабатывает описание одного варианта значения для типа enum. */
static HyScanDataSchemaEnumValue *
hyscan_data_schema_parse_enum_value (xmlNodePtr   node,
                                     const gchar *gettext_domain)
{
  HyScanDataSchemaEnumValue *enum_value = NULL;

  xmlChar *name = NULL;
  xmlChar *value = NULL;
  xmlChar *description = NULL;

  /* Проверяем XML элемент. */
  if (node->type != XML_ELEMENT_NODE)
    return NULL;
  if (g_ascii_strcasecmp ((gchar*)node->name, "value") != 0)
    return NULL;

  /* Атрибуты name и value. */
  name = xmlGetProp (node, (xmlChar *)"name");
  value = xmlGetProp (node, (xmlChar *)"value");
  if (name == NULL || value == NULL)
    goto exit;

  /* Поле description, может отсутствовать. */
  for (node = node->children; node != NULL; node = node->next)
    if (node->type == XML_ELEMENT_NODE)
      if (g_ascii_strcasecmp ((gchar*)node->name, "description") == 0)
        {
          description = xmlNodeGetContent (node);
          break;
        }

  enum_value = g_new0 (HyScanDataSchemaEnumValue, 1);
  enum_value->name = g_strdup ((gchar*)name);
  enum_value->value = g_ascii_strtoll ((const gchar *)value, NULL, 10);
  enum_value->description = g_strdup (g_dgettext (gettext_domain, (const gchar *)description));

exit:
  xmlFree (name);
  xmlFree (value);
  xmlFree (description);

  return enum_value;
}

/* Функция обрабатывает описание значений типа enum. */
static HyScanDataSchemaEnum *
hyscan_data_schema_parse_enum_values (xmlNodePtr   node,
                                      const gchar *gettext_domain)
{
  xmlNodePtr curnode;
  xmlChar *id;

  HyScanDataSchemaEnum *values;
  gint n_values;
  gint i;

  /* Число вариантов значений. */
  for (n_values = 0, curnode = node->children; curnode != NULL; curnode = curnode->next)
    {
      if (curnode->type != XML_ELEMENT_NODE)
        continue;
      if (g_ascii_strcasecmp ((gchar*)curnode->name, "value") != 0)
        continue;
      n_values += 1;
    }

  if (n_values == 0)
    return NULL;

  id = xmlGetProp (node, (xmlChar *)"id");
  if (id == NULL)
    {
      g_warning ("HyScanDataSchema: unknown enum id");
      return NULL;
    }

  values = g_new0 (HyScanDataSchemaEnum, 1);
  values->id = g_strdup ((const gchar *)id);
  values->values = g_malloc ((n_values + 1) * sizeof (HyScanDataSchemaEnumValue *));

  xmlFree (id);

  /* Разбор всех вариантов значений. */
  for (i = 0, curnode = node->children; curnode != NULL; curnode = curnode->next)
    {
      if (curnode->type != XML_ELEMENT_NODE)
        continue;
      if (g_ascii_strcasecmp ((gchar*)curnode->name, "value") != 0)
        continue;
      values->values[i] = hyscan_data_schema_parse_enum_value (curnode, gettext_domain);
      if (values->values[i] == NULL)
        {
          g_warning ("HyScanDataSchema: unknown value in enum '%s'", values->id);
          break;
        }
      i++;
    }
  values->values[i] = NULL;

  return values;
}

/* Функция обрабатывает переопределения значений типа enum. */
static HyScanDataSchemaEnum *
hyscan_data_schema_parse_overrided_enum_values (HyScanDataSchemaOverrides *overrides,
                                                const gchar               *enum_id)
{
  HyScanDataSchemaEnumValue **enum_values;
  HyScanDataSchemaEnum *values;

  if (overrides == NULL)
    return NULL;

  enum_values = hyscan_data_schema_overrides_get_enum_values (overrides, enum_id);
  if (enum_values == NULL)
    return NULL;

  values = g_new0 (HyScanDataSchemaEnum, 1);
  values->id = g_strdup (enum_id);
  values->values = enum_values;

  return values;
}

/* Функция проверяет значение перечисляемого типа на допустимость. */
static gboolean
hyscan_data_schema_check_enum (HyScanDataSchemaEnum *enums,
                               gint64                value)
{
  guint i;

  for (i = 0; enums->values[i] != NULL; i++)
    {
      if (enums->values[i]->value == value)
        return TRUE;
    }

  return FALSE;
}

/* Функция освобождает память занятую структурой с параметром. */
static void
hyscan_data_schema_free_key (HyScanDataSchemaKey *key)
{
  g_free (key->id);
  g_free (key->name);
  g_free (key->description);

  if (key->type == HYSCAN_DATA_SCHEMA_TYPE_STRING)
    {
      hyscan_data_schema_value_set_string (&key->default_value, NULL);
      hyscan_data_schema_value_set_string (&key->minimum_value, NULL);
      hyscan_data_schema_value_set_string (&key->maximum_value, NULL);
      hyscan_data_schema_value_set_string (&key->value_step, NULL);
    }

  g_free (key);
}

/* Функция обрабатывает описание параметра. */
static HyScanDataSchemaKey *
hyscan_data_schema_parse_key (xmlNodePtr                 node,
                              const gchar               *path,
                              GHashTable                *enums,
                              HyScanDataSchemaOverrides *overrides,
                              const gchar               *gettext_domain)
{
  HyScanDataSchemaKey *key = NULL;

  HyScanDataSchemaType key_data_type = HYSCAN_DATA_SCHEMA_TYPE_INVALID;
  HyScanDataSchemaEnum *enum_values = NULL;

  xmlNodePtr curnode;
  xmlChar *idx = NULL;
  xmlChar *namex = NULL;
  xmlChar *typex = NULL;
  xmlChar *enumx = NULL;
  xmlChar *flagsx = NULL;
  xmlChar *descriptionx = NULL;
  xmlChar *default_valuex = NULL;
  xmlChar *minimum_valuex = NULL;
  xmlChar *maximum_valuex = NULL;
  xmlChar *value_stepx = NULL;

  /* Атрибуты id и name. */
  idx = xmlGetProp (node, (xmlChar *)"id");
  namex = xmlGetProp (node, (xmlChar *)"name");
  if (idx == NULL || namex == NULL)
    {
      g_warning ("HyScanDataSchema: unknown key id or name in node '%s'", path);
      goto exit;
    }

  if (!hyscan_data_schema_validate_name ((gchar*)idx))
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

  /* Атрибут flags. */
  flagsx = xmlGetProp (node, (xmlChar *)"flags");

  /* Проверка типа данных. */
  if (typex != NULL)
    {
      if (g_ascii_strcasecmp ((const gchar *)typex, "boolean") == 0)
        key_data_type = HYSCAN_DATA_SCHEMA_TYPE_BOOLEAN;
      else if (g_ascii_strcasecmp ((const gchar *)typex, "integer") == 0)
        key_data_type = HYSCAN_DATA_SCHEMA_TYPE_INTEGER;
      else if (g_ascii_strcasecmp ((const gchar *)typex, "double") == 0)
        key_data_type = HYSCAN_DATA_SCHEMA_TYPE_DOUBLE;
      else if (g_ascii_strcasecmp ((const gchar *)typex, "string") == 0)
        key_data_type = HYSCAN_DATA_SCHEMA_TYPE_STRING;
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
      key_data_type = HYSCAN_DATA_SCHEMA_TYPE_ENUM;
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
  key = g_new0 (HyScanDataSchemaKey, 1);
  key->id = g_strdup_printf ("%s%s",path, (const gchar *)idx);
  key->name = g_strdup (g_dgettext (gettext_domain, (const gchar *)namex));
  key->description = g_strdup (g_dgettext (gettext_domain, (const gchar *)descriptionx));
  key->type = key_data_type;
  key->enum_values = enum_values;
  key->readonly = FALSE;

  /* Дополнительные флаги. */
  if (flagsx != NULL)
    {
      if (g_strcmp0 ((const gchar *)flagsx,"readonly") == 0)
        key->readonly = TRUE;
    }

  switch (key->type)
    {
    case HYSCAN_DATA_SCHEMA_TYPE_BOOLEAN:
      {
        gboolean default_value = FALSE;
        gboolean readonly;
        gboolean status;

        status = hyscan_data_schema_overrides_get_boolean (overrides,
                                                           key->id,
                                                           &default_value,
                                                           &readonly);
        if (status)
          {
            key->readonly = readonly;
          }
        else
          {
            if (default_valuex != NULL)
              {
                if (g_ascii_strcasecmp ((const gchar *)default_valuex, "1") == 0 ||
                    g_ascii_strcasecmp ((const gchar *)default_valuex, "true") == 0)
                  {
                    default_value = TRUE;
                  }
              }
          }

        hyscan_data_schema_value_set_boolean (&key->default_value, default_value);
        hyscan_data_schema_value_set_boolean (&key->minimum_value, FALSE);
        hyscan_data_schema_value_set_boolean (&key->maximum_value, TRUE);
        hyscan_data_schema_value_set_boolean (&key->value_step, FALSE);
      }
      break;

    case HYSCAN_DATA_SCHEMA_TYPE_INTEGER:
      {
        gint64 default_value = 0;
        gint64 minimum_value = G_MININT64;
        gint64 maximum_value = G_MAXINT64;
        gint64 value_step = 1;
        gboolean readonly;
        gboolean status;

        status = hyscan_data_schema_overrides_get_integer (overrides,
                                                           key->id,
                                                           &default_value,
                                                           &minimum_value,
                                                           &maximum_value,
                                                           &value_step,
                                                           &readonly);
        if (status)
          {
            key->readonly = readonly;
          }
        else
          {
            if (default_valuex != NULL)
              default_value = g_ascii_strtoll ((const gchar *)default_valuex, NULL, 10);
            if (minimum_valuex != NULL)
              minimum_value = g_ascii_strtoll ((const gchar *)minimum_valuex, NULL, 10);
            if (maximum_valuex != NULL)
              maximum_value = g_ascii_strtoll ((const gchar *)maximum_valuex, NULL, 10);
            if (value_stepx != NULL)
              value_step = g_ascii_strtoll ((const gchar *)value_stepx, NULL, 10);
          }

        if ((minimum_value > maximum_value) ||
            (default_value < minimum_value) ||
            (default_value > maximum_value))
          {
            g_warning ("HyScanDataSchema: default value out of range in key '%s'", key->id);
            hyscan_data_schema_free_key (key);
            key = NULL;
            goto exit;
          }

        hyscan_data_schema_value_set_integer (&key->default_value, default_value);
        hyscan_data_schema_value_set_integer (&key->minimum_value, minimum_value);
        hyscan_data_schema_value_set_integer (&key->maximum_value, maximum_value);
        hyscan_data_schema_value_set_integer (&key->value_step, value_step);
      }
      break;

    case HYSCAN_DATA_SCHEMA_TYPE_DOUBLE:
      {
        gdouble default_value = 0.0;
        gdouble minimum_value = -G_MAXDOUBLE;
        gdouble maximum_value = G_MAXDOUBLE;
        gdouble value_step = 1.0;
        gboolean readonly;
        gboolean status;

        status = hyscan_data_schema_overrides_get_double (overrides,
                                                          key->id,
                                                          &default_value,
                                                          &minimum_value,
                                                          &maximum_value,
                                                          &value_step,
                                                          &readonly);

        if (status)
          {
            key->readonly = readonly;
          }
        else
          {
            if (default_valuex != NULL)
              default_value = g_ascii_strtod ((const gchar *)default_valuex, NULL);
            if (minimum_valuex != NULL)
              minimum_value = g_ascii_strtod ((const gchar *)minimum_valuex, NULL);
            if (maximum_valuex != NULL)
              maximum_value = g_ascii_strtod ((const gchar *)maximum_valuex, NULL);
            if (value_stepx != NULL)
              value_step = g_ascii_strtod ((const gchar *)value_stepx, NULL);
          }

        if ((minimum_value > maximum_value) ||
            (default_value < minimum_value) ||
            (default_value > maximum_value))
          {
            g_warning ("HyScanDataSchema: default value out of range in key '%s'", key->id);
            hyscan_data_schema_free_key (key);
            key = NULL;
            goto exit;
          }

        hyscan_data_schema_value_set_double (&key->default_value, default_value);
        hyscan_data_schema_value_set_double (&key->minimum_value, minimum_value);
        hyscan_data_schema_value_set_double (&key->maximum_value, maximum_value);
        hyscan_data_schema_value_set_double (&key->value_step, value_step);
      }
      break;

    case HYSCAN_DATA_SCHEMA_TYPE_STRING:
      {
        gchar *default_value;
        gboolean readonly;
        gboolean status;

        status = hyscan_data_schema_overrides_get_string (overrides,
                                                          key->id,
                                                          &default_value,
                                                          &readonly);

        if (status)
          {
            key->readonly = readonly;
            hyscan_data_schema_value_set_string (&key->default_value, default_value);
            g_free (default_value);
          }
        else
          {
            hyscan_data_schema_value_set_string (&key->default_value, (const gchar *)default_valuex);
            hyscan_data_schema_value_set_string (&key->minimum_value, NULL);
            hyscan_data_schema_value_set_string (&key->maximum_value, NULL);
            hyscan_data_schema_value_set_string (&key->value_step, NULL);
          }

      }
      break;

    case HYSCAN_DATA_SCHEMA_TYPE_ENUM:
      {
        gint64 default_value = 123;
        gboolean readonly;
        gboolean status;

        status = hyscan_data_schema_overrides_get_enum (overrides,
                                                        key->id,
                                                        &default_value,
                                                        &readonly);

        if (status)
          {
            key->readonly = readonly;
          }
        else
          {
            if (default_valuex != NULL)
              default_value = g_ascii_strtoll ((const gchar *)default_valuex, NULL, 10);
          }

        if (!hyscan_data_schema_check_enum (key->enum_values, default_value))
          {
            g_warning ("HyScanDataSchema: default value out of range in key '%s'", key->id);
            hyscan_data_schema_free_key (key);
            key = NULL;
            goto exit;
          }

        hyscan_data_schema_value_set_integer (&key->default_value, default_value);
        hyscan_data_schema_value_set_integer (&key->minimum_value, 0);
        hyscan_data_schema_value_set_integer (&key->maximum_value, 0);
        hyscan_data_schema_value_set_integer (&key->value_step, 0);
      }
      break;

    default:
      break;
    }

exit:
  xmlFree (idx);
  xmlFree (namex);
  xmlFree (typex);
  xmlFree (enumx);
  xmlFree (flagsx);
  xmlFree (descriptionx);
  xmlFree (default_valuex);
  xmlFree (minimum_valuex);
  xmlFree (maximum_valuex);
  xmlFree (value_stepx);

  return key;
}

/* Функция обрабатывает описание ветки схемы. */
static gboolean
hyscan_data_schema_parse_node (xmlNodePtr                 node,
                               const gchar               *schema_path,
                               const gchar               *path,
                               GHashTable                *keys,
                               GHashTable                *enums,
                               HyScanDataSchemaOverrides *overrides,
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

      if (!hyscan_data_schema_validate_name ((gchar*)id))
        {
          g_warning ("HyScanDataSchema: incorrect node name '%s' in node '%s'", id, path);
          return FALSE;
        }

      if (g_ascii_strcasecmp ((gchar*)node->name, "key") == 0)
        {
          HyScanDataSchemaKey *key = hyscan_data_schema_parse_key (node,
                                                                   path,
                                                                   enums,
                                                                   overrides,
                                                                   gettext_domain);
          if (key == NULL)
            {
              status = FALSE;
            }
          else
            {
              if (g_hash_table_contains (keys, key->id))
                {
                  g_warning ("HyScanDataSchema: duplicated key '%s'", key->id);
                  status = FALSE;
                }
              else
                {
                g_hash_table_insert (keys, key->id, key);
                }
            }
        }

      if (g_ascii_strcasecmp ((gchar*)node->name, "node") == 0)
        {
          gchar *cpath = g_strdup_printf ("%s%s/", path, id);
          if (schema != NULL)
            {
              status = hyscan_data_schema_parse_schema (node->doc,
                                                        schema_path,
                                                        (gchar*)schema,
                                                        cpath,
                                                        keys,
                                                        enums,
                                                        overrides,
                                                        gettext_domain);
            }
          else
            {
              status = hyscan_data_schema_parse_node (node->children,
                                                      schema_path,
                                                      cpath,
                                                      keys,
                                                      enums,
                                                      overrides,
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
                                 HyScanDataSchemaOverrides *overrides,
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
                                              cpath,
                                              path,
                                              keys,
                                              enums,
                                              overrides,
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

/* Функция создаёт новый узел с параметрами. */
static HyScanDataSchemaNode *
hyscan_data_schema_new_node (const gchar *path)
{
  HyScanDataSchemaNode *node;

  node = g_new0 (HyScanDataSchemaNode, 1);
  node->path = g_strdup (path);
  node->nodes = g_malloc0 (sizeof (HyScanDataSchemaNode*));
  node->params = g_malloc0 (sizeof (HyScanDataSchemaParam*));

  return node;
}

/* Функция создаёт новую структуру с описанием параметра. */
static HyScanDataSchemaParam *
hyscan_data_schema_new_param (const gchar          *id,
                              const gchar          *name,
                              const gchar          *description,
                              HyScanDataSchemaType  type,
                              gboolean              readonly)
{
  HyScanDataSchemaParam *param;

  param = g_new (HyScanDataSchemaParam, 1);
  param->id = g_strdup (id);
  param->name = g_strdup (name);
  param->description = g_strdup (description);
  param->type = type;
  param->readonly = readonly;

  return param;
}

/* Функция добавляет новый параметр в список. */
static void
hyscan_data_schema_insert_param (HyScanDataSchemaNode *node,
                                 const gchar          *id,
                                 const gchar          *name,
                                 const gchar          *description,
                                 HyScanDataSchemaType  type,
                                 gboolean              readonly)
{
  gchar **pathv;
  guint i, j;

  pathv = g_strsplit (id, "/", -1);

  /* Поиск узла для параметра. */
  for (i = 1; i < g_strv_length (pathv) - 1; i++)
    {
      gboolean has_node = FALSE;
      gchar *cur_path;

      /* Сверяем путь для текущего узла с идентификатором параметра. */
      for (j = 0; node->nodes != NULL && node->nodes[j] != NULL; j++)
        {
          /* Если совпадают, мы на правильном пути:) */
          if (g_str_has_prefix (id, node->nodes[j]->path))
            {
              node = node->nodes[j];
              has_node = TRUE;
              break;
            }
        }

      /* Если узел для текущего пути найден, переходим к следующему компоненту пути. */
      if (has_node)
        continue;

      /* Или добавляем новый узел. */
      cur_path = g_strdup_printf ("%s/%s", node->path, pathv[i]);
      node->nodes = g_realloc (node->nodes, (node->n_nodes + 2) * sizeof (HyScanDataSchemaNode*));
      node->nodes[node->n_nodes] = hyscan_data_schema_new_node (cur_path);
      node->n_nodes += 1;
      node->nodes[node->n_nodes] = NULL;
      node = node->nodes[j];
      g_free (cur_path);
    }

  g_strfreev (pathv);

  /* Новый параметр. */
  node->params = g_realloc (node->params, (node->n_params + 2) * sizeof (HyScanDataSchemaParam*));
  node->params[node->n_params] = hyscan_data_schema_new_param (id, name, description, type, readonly);
  node->n_params += 1;
  node->params[node->n_params] = NULL;
}

/* Функция создаёт новый объект HyScanDataSchema. */
HyScanDataSchema *
hyscan_data_schema_new_from_string (const gchar *schema_data,
                                    const gchar *schema_id,
                                    const gchar *overrides_data)
{
  return g_object_new (HYSCAN_TYPE_DATA_SCHEMA,
                       "schema-data", schema_data,
                       "schema-id", schema_id,
                       "overrides-data", overrides_data,
                       NULL);
}

/* Функция создаёт новый объект HyScanDataSchema. */
HyScanDataSchema *
hyscan_data_schema_new_from_file (const gchar *schema_path,
                                  const gchar *schema_id,
                                  const gchar *overrides_data)
{
  gchar *schema_data;
  gpointer schema;

  if (!g_file_get_contents (schema_path, &schema_data, NULL, NULL))
    return NULL;

  schema = g_object_new (HYSCAN_TYPE_DATA_SCHEMA,
                         "schema-data", schema_data,
                         "schema-id", schema_id,
                         "overrides-data", overrides_data,
                         NULL);
  g_free (schema_data);

  return schema;
}

/* Функция создаёт новый объект HyScanDataSchema. */
HyScanDataSchema *
hyscan_data_schema_new_from_resource (const gchar *schema_resource,
                                      const gchar *schema_id,
                                      const gchar *overrides_data)
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
                         "overrides-data", overrides_data,
                         NULL);
  g_bytes_unref (resource);

  return schema;
}

/* Функция возвращает описание схемы данных в фомате XML. */
gchar *
hyscan_data_schema_get_data (HyScanDataSchema *schema)
{
  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), NULL);

  return g_strdup (schema->priv->schema_data);
}

/* Функция возвращает идентификатор используемой схемы данных. */
gchar *
hyscan_data_schema_get_id (HyScanDataSchema *schema)
{
  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), NULL);

  return g_strdup (schema->priv->schema_id);
}

/* Функция возвращает переопределения используемой схемы данных. */
gchar *
hyscan_data_schema_get_overrides (HyScanDataSchema *schema)
{
  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), NULL);

  return g_strdup (schema->priv->overrides_data);
}

/* Функция возвращает список параметров определённых в схеме. */
gchar **
hyscan_data_schema_list_keys (HyScanDataSchema *schema)
{
  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), NULL);

  return g_strdupv (schema->priv->keys_list);
}

/* Функция возвращает иеархический список узлов и параметров определённых в схеме. */
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

  node = hyscan_data_schema_new_node ("");
  for (i = 0; priv->keys_list[i] != NULL; i++)
    {
      HyScanDataSchemaKey *key = g_hash_table_lookup (priv->keys, priv->keys_list[i]);
      hyscan_data_schema_insert_param (node, priv->keys_list[i], key->name, key->description, key->type, key->readonly);
    }

  return node;
}

/* Функция проверяет существование параметра в схеме. */
gboolean
hyscan_data_schema_has_key (HyScanDataSchema *schema,
                            const gchar      *key_id)
{
  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), FALSE);

  return g_hash_table_contains (schema->priv->keys, key_id);
}

/* Функция возвращает тип параметра. */
HyScanDataSchemaType
hyscan_data_schema_key_get_type (HyScanDataSchema *schema,
                                 const gchar      *key_id)
{
  HyScanDataSchemaKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), HYSCAN_DATA_SCHEMA_TYPE_INVALID);

  key = g_hash_table_lookup (schema->priv->keys, key_id);
  if (key == NULL)
    return HYSCAN_DATA_SCHEMA_TYPE_INVALID;

  return key->type;
}

/* Функция возвращает название параметра. */
const gchar *
hyscan_data_schema_key_get_name (HyScanDataSchema *schema,
                                 const gchar      *key_id)
{
  HyScanDataSchemaKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), NULL);

  key = g_hash_table_lookup (schema->priv->keys, key_id);
  if (key == NULL)
    return NULL;

  return key->name;
}

/* Функция возвращает описание параметра. */
const gchar *
hyscan_data_schema_key_get_description (HyScanDataSchema *schema,
                                        const gchar      *key_id)
{
  HyScanDataSchemaKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), NULL);

  key = g_hash_table_lookup (schema->priv->keys, key_id);
  if (key == NULL)
    return NULL;

  return key->description;
}

/* Функция возвращает доступность параметра на запись. */
gboolean
hyscan_data_schema_key_is_readonly (HyScanDataSchema *schema,
                                    const gchar      *key_id)
{
  HyScanDataSchemaKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), TRUE);

  key = g_hash_table_lookup (schema->priv->keys, key_id);
  if (key == NULL)
    return TRUE;

  return key->readonly;
}

/* Функция возвращает значение по умолчанию для параметра с типом BOOLEAN. */
gboolean
hyscan_data_schema_key_get_default_boolean (HyScanDataSchema *schema,
                                            const gchar      *key_id,
                                            gboolean         *value)
{
  HyScanDataSchemaKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), FALSE);

  key = g_hash_table_lookup (schema->priv->keys, key_id);
  if (key == NULL || key->type != HYSCAN_DATA_SCHEMA_TYPE_BOOLEAN)
    return FALSE;

  *value = hyscan_data_schema_value_get_boolean (&key->default_value);

  return TRUE;
}

/* Функция возвращает значение по умолчанию для параметра с типом INTEGER. */
gboolean
hyscan_data_schema_key_get_default_integer (HyScanDataSchema *schema,
                                            const gchar      *key_id,
                                            gint64           *value)
{
  HyScanDataSchemaKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), FALSE);

  key = g_hash_table_lookup (schema->priv->keys, key_id);
  if (key == NULL || key->type != HYSCAN_DATA_SCHEMA_TYPE_INTEGER)
    return FALSE;

  *value = hyscan_data_schema_value_get_integer (&key->default_value);

  return TRUE;
}

/* Функция возвращает значение по умолчанию для параметра с типом DOUBLE. */
gboolean
hyscan_data_schema_key_get_default_double (HyScanDataSchema *schema,
                                           const gchar      *key_id,
                                           gdouble          *value)
{
  HyScanDataSchemaKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), FALSE);

  key = g_hash_table_lookup (schema->priv->keys, key_id);
  if (key == NULL || key->type != HYSCAN_DATA_SCHEMA_TYPE_DOUBLE)
    return FALSE;

  *value = hyscan_data_schema_value_get_double (&key->default_value);

  return TRUE;
}

/* Функция возвращает значение по умолчанию для параметра с типом STRING. */
const gchar *
hyscan_data_schema_key_get_default_string (HyScanDataSchema *schema,
                                            const gchar      *key_id)
{
  HyScanDataSchemaKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), NULL);

  key = g_hash_table_lookup (schema->priv->keys, key_id);
  if (key == NULL || key->type != HYSCAN_DATA_SCHEMA_TYPE_STRING)
    return NULL;

  return hyscan_data_schema_value_get_string (&key->default_value);
}

/* Функция возвращает значение по умолчанию для параметра с типом ENUM. */
gboolean
hyscan_data_schema_key_get_default_enum (HyScanDataSchema *schema,
                                         const gchar      *key_id,
                                         gint64           *value)
{
  HyScanDataSchemaKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), FALSE);

  key = g_hash_table_lookup (schema->priv->keys, key_id);
  if (key == NULL || key->type != HYSCAN_DATA_SCHEMA_TYPE_ENUM)
    return FALSE;

  *value = hyscan_data_schema_value_get_integer (&key->default_value);

  return TRUE;
}

/* Функция возвращает диапазон допустимых значений для параметра с типом INTEGER. */
gboolean
hyscan_data_schema_key_get_integer_range (HyScanDataSchema      *schema,
                                          const gchar           *key_id,
                                          gint64                *minimum,
                                          gint64                *maximum)
{
  HyScanDataSchemaKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), FALSE);

  key = g_hash_table_lookup (schema->priv->keys, key_id);
  if (key == NULL || key->type != HYSCAN_DATA_SCHEMA_TYPE_INTEGER)
    return FALSE;

  *minimum = hyscan_data_schema_value_get_integer (&key->minimum_value);
  *maximum = hyscan_data_schema_value_get_integer (&key->maximum_value);

  return TRUE;
}

/* Функция возвращает диапазон допустимых значений для параметра с типом DOUBLE. */
gboolean
hyscan_data_schema_key_get_double_range (HyScanDataSchema      *schema,
                                         const gchar           *key_id,
                                         gdouble               *minimum,
                                         gdouble               *maximum)
{
  HyScanDataSchemaKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), FALSE);

  key = g_hash_table_lookup (schema->priv->keys, key_id);
  if (key == NULL || key->type != HYSCAN_DATA_SCHEMA_TYPE_DOUBLE)
    return FALSE;

  *minimum = hyscan_data_schema_value_get_double (&key->minimum_value);
  *maximum = hyscan_data_schema_value_get_double (&key->maximum_value);

  return TRUE;
}

/* Функция возвращает варианты допустимых значений для параметра с типом ENUM. */
HyScanDataSchemaEnumValue **
hyscan_data_schema_key_get_enum_values (HyScanDataSchema      *schema,
                                        const gchar           *key_id)
{
  HyScanDataSchemaEnumValue **values;
  HyScanDataSchemaKey *key;
  gint n_values;
  gint i;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), NULL);

  key = g_hash_table_lookup (schema->priv->keys, key_id);
  if (key == NULL || key->type != HYSCAN_DATA_SCHEMA_TYPE_ENUM)
    return NULL;

  for (i = 0, n_values = 0; key->enum_values->values[i] != NULL; i++)
    n_values += 1;

  values = g_malloc0 (sizeof (HyScanDataSchemaEnumValue*) * (n_values + 1));
  for (i = 0; i < n_values; i++)
    {
      values[i] = g_new0 (HyScanDataSchemaEnumValue, 1);
      values[i]->value = key->enum_values->values[i]->value;
      values[i]->name = g_strdup (key->enum_values->values[i]->name);
      values[i]->description = g_strdup (key->enum_values->values[i]->description);
    }

  return values;
}

/* Функция возвращает рекомендуемый шаг изменения значения для параметра с типом INTEGER. */
gboolean
hyscan_data_schema_key_get_integer_step (HyScanDataSchema *schema,
                                         const gchar      *key_id,
                                         gint64           *value)
{
  HyScanDataSchemaKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), FALSE);

  key = g_hash_table_lookup (schema->priv->keys, key_id);
  if (key == NULL || key->type != HYSCAN_DATA_SCHEMA_TYPE_INTEGER)
    return FALSE;

  *value = hyscan_data_schema_value_get_integer (&key->value_step);

  return TRUE;
}

/* Функция возвращает рекомендуемый шаг изменения значения для параметра с типом DOUBLE. */
gboolean
hyscan_data_schema_key_get_double_step (HyScanDataSchema *schema,
                                        const gchar      *key_id,
                                        gdouble          *value)
{
  HyScanDataSchemaKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), FALSE);

  key = g_hash_table_lookup (schema->priv->keys, key_id);
  if (key == NULL || key->type != HYSCAN_DATA_SCHEMA_TYPE_DOUBLE)
    return FALSE;

  *value = hyscan_data_schema_value_get_double (&key->value_step);

  return TRUE;
}

/* Функция проверяет значение параметра типа INTEGER на предмет нахождения в допустимом диапазоне. */
gboolean
hyscan_data_schema_key_check_integer (HyScanDataSchema *schema,
                                      const gchar      *key_id,
                                      gint64            value)
{
  HyScanDataSchemaKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), FALSE);

  key = g_hash_table_lookup (schema->priv->keys, key_id);
  if (key == NULL || key->type != HYSCAN_DATA_SCHEMA_TYPE_INTEGER)
    return FALSE;

  if (key->readonly &&
      value != hyscan_data_schema_value_get_integer (&key->default_value))
    {
      return FALSE;
    }

  if (value < hyscan_data_schema_value_get_integer (&key->minimum_value))
    return FALSE;
  if (value > hyscan_data_schema_value_get_integer (&key->maximum_value))
    return FALSE;

  return TRUE;
}

/* Функция проверяет значение параметра типа DOUBLE на предмет нахождения в допустимом диапазоне. */
gboolean
hyscan_data_schema_key_check_double (HyScanDataSchema *schema,
                                     const gchar      *key_id,
                                     gdouble           value)
{
  HyScanDataSchemaKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), FALSE);

  key = g_hash_table_lookup (schema->priv->keys, key_id);
  if (key == NULL || key->type != HYSCAN_DATA_SCHEMA_TYPE_DOUBLE)
    return FALSE;

  if (key->readonly &&
      value != hyscan_data_schema_value_get_double (&key->default_value))
    {
      return FALSE;
    }

  if (value < hyscan_data_schema_value_get_double (&key->minimum_value))
    return FALSE;
  if (value > hyscan_data_schema_value_get_double (&key->maximum_value))
    return FALSE;

  return TRUE;
}

/* Функция проверяет значение параметра типа ENUM на предмет нахождения в допустимом диапазоне. */
gboolean
hyscan_data_schema_key_check_enum (HyScanDataSchema *schema,
                                   const gchar      *key_id,
                                   gint64            value)
{
  HyScanDataSchemaKey *key;

  g_return_val_if_fail (HYSCAN_IS_DATA_SCHEMA (schema), FALSE);

  key = g_hash_table_lookup (schema->priv->keys, key_id);
  if (key == NULL || key->type != HYSCAN_DATA_SCHEMA_TYPE_ENUM)
    return FALSE;

  if (key->readonly &&
      value != hyscan_data_schema_value_get_integer (&key->default_value))
    {
      return FALSE;
    }

  return hyscan_data_schema_check_enum (key->enum_values, value);
}

/* Функция освобождает память занятую списком узлов и параметров. */
void
hyscan_data_schema_free_nodes (HyScanDataSchemaNode *nodes)
{
  gint i;

  for (i = 0; i < nodes->n_nodes; i++)
    hyscan_data_schema_free_nodes (nodes->nodes[i]);

  for (i = 0; i < nodes->n_params; i++)
    {
      g_free (nodes->params[i]->id);
      g_free (nodes->params[i]->name);
      g_free (nodes->params[i]->description);
      g_free (nodes->params[i]);
    }

  g_free (nodes->path);

  g_free (nodes->nodes);
  g_free (nodes->params);
  g_free (nodes);
}

/* Функция освобождает память занятую списком вариантов допустимых значений для параметра с типом ENUM. */
void
hyscan_data_schema_free_enum_values (HyScanDataSchemaEnumValue **values)
{
  gint i;

  for (i = 0; values[i] != NULL; i++)
    {
      g_free (values[i]->name);
      g_free (values[i]->description);
      g_free (values[i]);
    }

  g_free (values);
}
