/* hyscan-param-merge.c
 *
 * Copyright 2020 Screen LLC, Alexey Sakhnov <alexsakhnov@gmail.com>
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
 * SECTION: hyscan-param-merge
 * @Short_description: класс объединения параметров с похожей схемой данных
 * @Title: HyScanParamMerge
 *
 * Класс реализует интерфейс #HyScanParam и позволяет установить одинаковые значения параметров
 * в несколько обработчиков данных #HyScanParam, имеющих одинаковую или похожую схему данных.
 *
 * Класс формирует схему, являющуюся пересечением параметров схем переданных в него
 * обработчиков, таким образом гарантируя валидность устанавливаемых и считываемых
 * значений для каждого обработчика.
 *
 * При вызове функции hyscan_param_set() класс установливает в обработчики значения
 * всех полученных параметров, за исключением тех, для которых был установлен флаг
 * "хранить значение неизменным". Для управления этим флагом используйте функции
 * hyscan_param_merge_set_keep() и hyscan_param_merge_get_keep().
 *
 * При первоначальном связывании всех обработчиков, те параметры, которые имеют
 * различные значения в разных обработчика, автоматически помечаются флагом
 * "хранить значение неизменным".
 *
 */

#include "hyscan-param-merge.h"
#include "hyscan-data-schema-builder.h"

struct _HyScanParamMergePrivate
{
  HyScanDataSchemaBuilder   *builder;   /* Генератор схемы. */
  HyScanDataSchema          *schema;    /* Схема. */
  GList                     *params;    /* Список дочерних объектов HyScanParam. */
  GList                     *schemas;   /* Схемы HyScanDataSchema дочерних объектов. */
  GHashTable                *keep;      /* Таблица ключей, для которых надо хранить значение неизменным. */
};

static void     hyscan_param_merge_interface_init       (HyScanParamInterface       *iface);
static void     hyscan_param_merge_object_constructed   (GObject                    *object);
static void     hyscan_param_merge_object_finalize      (GObject                    *object);
static gboolean hyscan_param_merge_add_key_boolean      (HyScanParamMerge           *merge,
                                                         const gchar                *key_id);
static gboolean hyscan_param_merge_add_key_integer      (HyScanParamMerge           *merge,
                                                         const gchar                *key_id);
static gboolean hyscan_param_merge_add_key_double       (HyScanParamMerge           *merge,
                                                         const gchar                *key_id);
static gboolean hyscan_param_merge_add_key_string       (HyScanParamMerge           *merge,
                                                         const gchar                *key_id);
static gboolean hyscan_param_merge_add_key_enum         (HyScanParamMerge           *merge,
                                                         const gchar                *key_id);
static void     hyscan_param_merge_bind_keep            (HyScanParamMerge           *merge);
static void     hyscan_param_merge_node_to_table        (const HyScanDataSchemaNode *node,
                                                         GHashTable                 *keys);
static gboolean hyscan_param_merge_enum_map             (HyScanDataSchema           *schema,
                                                         HyScanParam                *child,
                                                         const gchar                *key_id,
                                                         gint64                      value,
                                                         gint64                     *child_value);

G_DEFINE_TYPE_WITH_CODE (HyScanParamMerge, hyscan_param_merge, G_TYPE_OBJECT,
                         G_ADD_PRIVATE (HyScanParamMerge)
                         G_IMPLEMENT_INTERFACE (HYSCAN_TYPE_PARAM, hyscan_param_merge_interface_init))

static void
hyscan_param_merge_class_init (HyScanParamMergeClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = hyscan_param_merge_object_constructed;
  object_class->finalize = hyscan_param_merge_object_finalize;
}

static void
hyscan_param_merge_init (HyScanParamMerge *param_merge)
{
  param_merge->priv = hyscan_param_merge_get_instance_private (param_merge);
}

static void
hyscan_param_merge_object_constructed (GObject *object)
{
  HyScanParamMerge *param_merge = HYSCAN_PARAM_MERGE (object);
  HyScanParamMergePrivate *priv = param_merge->priv;

  G_OBJECT_CLASS (hyscan_param_merge_parent_class)->constructed (object);

  priv->keep = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
  priv->builder = hyscan_data_schema_builder_new ("merge");
}

static void
hyscan_param_merge_object_finalize (GObject *object)
{
  HyScanParamMerge *param_merge = HYSCAN_PARAM_MERGE (object);
  HyScanParamMergePrivate *priv = param_merge->priv;

  g_clear_object (&priv->builder);
  g_clear_object (&priv->schema);
  g_hash_table_destroy (priv->keep);
  g_list_free_full (priv->params, g_object_unref);
  g_list_free_full (priv->schemas, g_object_unref);

  G_OBJECT_CLASS (hyscan_param_merge_parent_class)->finalize (object);
}

static gboolean
hyscan_param_merge_add_key_boolean (HyScanParamMerge *merge,
                                    const gchar      *key_id)
{
  HyScanParamMergePrivate *priv = merge->priv;
  HyScanDataSchema *base = priv->schemas->data;
  const gchar *name, *description;
  GVariant *default_value;

  name = hyscan_data_schema_key_get_name (base, key_id);
  description = hyscan_data_schema_key_get_description (base, key_id);
  default_value = hyscan_data_schema_key_get_default (base, key_id);
  hyscan_data_schema_builder_key_boolean_create (priv->builder, key_id, name, description,
                                                 g_variant_get_boolean (default_value));
  g_variant_unref (default_value);

  return TRUE;
}

static gboolean
hyscan_param_merge_add_key_integer (HyScanParamMerge *merge,
                                    const gchar      *key_id)
{
  HyScanParamMergePrivate *priv = merge->priv;
  HyScanDataSchema *base = priv->schemas->data;
  GList *schema_l;
  const gchar *name, *description;
  gint64 min_value, max_value, default_value, step;

  name = hyscan_data_schema_key_get_name (base, key_id);
  description = hyscan_data_schema_key_get_description (base, key_id);

  hyscan_data_schema_key_get_integer (base, key_id, &min_value, &max_value, &default_value, &step);
  for (schema_l = priv->schemas->next; schema_l != NULL; schema_l = schema_l->next)
    {
      HyScanDataSchema *other_schema = schema_l->data;
      gint64 min_value2, max_value2, step2;

      hyscan_data_schema_key_get_integer (other_schema, key_id, &min_value2, &max_value2, NULL, &step2);
      min_value = MAX (min_value, min_value2);
      max_value = MIN (max_value, max_value2);
      step = MAX (step, step2);
    }

  if (min_value > max_value)
    return FALSE;

  default_value = CLAMP (default_value, min_value, max_value);
  hyscan_data_schema_builder_key_integer_create (priv->builder, key_id, name, description, default_value);
  hyscan_data_schema_builder_key_integer_range (priv->builder, key_id, min_value, max_value, step);

  return TRUE;
}

static gboolean
hyscan_param_merge_add_key_double (HyScanParamMerge *merge,
                                    const gchar      *key_id)
{
  HyScanParamMergePrivate *priv = merge->priv;
  HyScanDataSchema *base = priv->schemas->data;
  GList *schema_l;
  const gchar *name, *description;
  gdouble default_value;
  gdouble min_value, max_value, step;

  name = hyscan_data_schema_key_get_name (base, key_id);
  description = hyscan_data_schema_key_get_description (base, key_id);

  hyscan_data_schema_key_get_double (base, key_id, &min_value, &max_value, &default_value, &step);
  for (schema_l = priv->schemas->next; schema_l != NULL; schema_l = schema_l->next)
    {
      HyScanDataSchema *other_schema = schema_l->data;
      gdouble min_value2, max_value2, step2;

      hyscan_data_schema_key_get_double (other_schema, key_id, &min_value2, &max_value2, NULL, &step2);
      min_value = MAX (min_value, min_value2);
      max_value = MIN (max_value, max_value2);
      step = MAX (step, step2);
    }

  if (min_value > max_value)
    return FALSE;

  default_value = CLAMP (default_value, min_value, max_value);
  hyscan_data_schema_builder_key_double_create (priv->builder, key_id, name, description, default_value);
  hyscan_data_schema_builder_key_double_range (priv->builder, key_id, min_value, max_value, step);

  return TRUE;
}

static gboolean
hyscan_param_merge_add_key_string (HyScanParamMerge *merge,
                                    const gchar      *key_id)
{
  HyScanParamMergePrivate *priv = merge->priv;
  HyScanDataSchema *base = priv->schemas->data;
  const gchar *name, *description;
  const gchar *default_value;

  default_value = hyscan_data_schema_key_get_string (base, key_id);
  name = hyscan_data_schema_key_get_name (base, key_id);
  description = hyscan_data_schema_key_get_description (base, key_id);
  hyscan_data_schema_builder_key_string_create (priv->builder, key_id, name, description, default_value);

  return TRUE;
}

static gboolean
hyscan_param_merge_add_key_enum (HyScanParamMerge *merge,
                                const gchar       *key_id)
{
  HyScanParamMergePrivate *priv = merge->priv;
  HyScanDataSchema *base = priv->schemas->data;
  const gchar *name, *description, *enum_id;
  gint64 default_value;
  GList *enum_values;
  GList *schema_l;
  GList *enum_l, *enum_next;
  gboolean default_value_valid;

  hyscan_data_schema_key_get_enum (base, key_id, &default_value);
  name = hyscan_data_schema_key_get_name (base, key_id);
  description = hyscan_data_schema_key_get_description (base, key_id);
  enum_id = hyscan_data_schema_key_get_enum_id (base, key_id);

  /* Отбираем значения enum, чтобы они были доступны во всех обработчиках. */
  enum_values = hyscan_data_schema_enum_get_values (base, enum_id);
  for (schema_l = priv->schemas->next; schema_l != NULL; schema_l = schema_l->next)
    {
      HyScanDataSchema *other_schema = schema_l->data;
      const gchar *other_enum_id;

      other_enum_id = hyscan_data_schema_key_get_enum_id (other_schema, key_id);
      for (enum_l = enum_values; enum_l != NULL; enum_l = enum_next)
        {
          HyScanDataSchemaEnumValue *enum_value = enum_l->data;
          enum_next = enum_l->next;

          if (NULL != hyscan_data_schema_enum_find_by_id (other_schema, other_enum_id, enum_value->id))
            continue;

          enum_values = g_list_delete_link (enum_values, enum_l);
        }
    }

  /* На выходе фильтра не осталось ни одного варианта enum. */
  if (enum_values == NULL)
    return FALSE;

  /* Создаём новое перечисление из того, что осталось. И, заодно, проверяем, что дефолтное значение там присутствует. */
  default_value_valid = FALSE;
  hyscan_data_schema_builder_enum_create (priv->builder, enum_id);
  for (enum_l = enum_values; enum_l != NULL; enum_l = enum_l->next)
    {
      HyScanDataSchemaEnumValue *enum_value = enum_l->data;
      hyscan_data_schema_builder_enum_value_create (priv->builder, enum_id,
                                                    enum_value->value,
                                                    enum_value->id,
                                                    enum_value->name,
                                                    enum_value->description);

      if (default_value == enum_value->value)
        default_value_valid = TRUE;
    }

  if (!default_value_valid)
    default_value = ((HyScanDataSchemaEnumValue *) enum_values->data)->value;

  hyscan_data_schema_builder_key_enum_create (priv->builder, key_id, name, description, enum_id, default_value);

  g_list_free (enum_values);

  return TRUE;
}

/* Функция для удаления GVariant. */
static void
hyscan_param_merge_variant_free (gpointer value)
{
  g_clear_pointer (&value, g_variant_unref);
}

/* Определяет параметры, для которых значения в разных обработчиках различаются,
 * и устанавливает для них флаг "хранить значение неизменным". */
static void
hyscan_param_merge_bind_keep (HyScanParamMerge *merge)
{
  HyScanParamMergePrivate *priv = merge->priv;
  const gchar *const *keys;
  gint i;

  GList *link;
  GList *plists = NULL;
  GHashTable *ref_values;
  GHashTableIter iter;
  HyScanParam *ref_param;
  HyScanDataSchema *ref_schema;
  HyScanParamList *plist;

  g_hash_table_remove_all (priv->keep);

  keys = hyscan_data_schema_list_keys (priv->schema);
  if (keys == NULL)
    return;

  if (priv->params == NULL)
    return;

  /* Список общих ключей всех параметров. */
  plist = hyscan_param_list_new ();
  for (i = 0; keys[i] != NULL; i++)
    hyscan_param_list_add (plist, keys[i]);

  /* Формируем таблицу референсных значений. */
  ref_param = priv->params->data;
  ref_schema = hyscan_param_schema (ref_param);
  hyscan_param_get (ref_param, plist);
  ref_values = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, hyscan_param_merge_variant_free);
  for (i = 0; keys[i] != NULL; i++)
    g_hash_table_insert (ref_values, (gpointer) keys[i], hyscan_param_list_get (plist, keys[i]));

  /* Получаем значения всех остальных параметров и сравниваем с референсными. */
  for (link = priv->params->next; link != NULL; link = link->next)
    {
      HyScanParam *param = link->data;
      GVariant *ref_value;
      gchar *key;

      /* Удаляем из таблицы обших ключей те, у которых отличаются значения в разных обработчиках. */
      hyscan_param_get (param, plist);
      g_hash_table_iter_init (&iter, ref_values);
      while (g_hash_table_iter_next (&iter, (gpointer *) &key, (gpointer *) &ref_value))
        {
          gboolean equal;

          if (hyscan_data_schema_key_get_value_type (ref_schema, key) == HYSCAN_DATA_SCHEMA_KEY_ENUM)
            {
              gint64 value, ref_mapped;

              value = hyscan_param_list_get_enum (plist, key);
              if (hyscan_param_merge_enum_map (ref_schema, param, key, g_variant_get_int64 (ref_value), &ref_mapped))
                equal = (value == ref_mapped);
              else
                equal = FALSE;
            }
          else
            {
              GVariant *value;

              value = hyscan_param_list_get (plist, key);
              if (ref_value != NULL)
                {
                  equal = (value != NULL) && g_variant_equal (value, ref_value);
                  g_variant_unref (value);
                }
              else
                {
                  equal = (value == NULL);
                }
            }

          if (!equal)
            g_hash_table_iter_remove (&iter);
        }

      if (g_hash_table_size (ref_values) == 0)
        break;
    }

  /* Заполняем таблицу ключей, значения которых надо хранить неизменными. */
  for (i = 0; keys[i] != NULL; i++)
    {
      if (!g_hash_table_contains (ref_values, keys[i]))
        g_hash_table_add (priv->keep, g_strdup (keys[i]));
    }

  g_hash_table_destroy (ref_values);
  g_list_free_full (plists, g_object_unref);
  g_object_unref (ref_schema);
  g_object_unref (plist);
}

static void
hyscan_param_merge_node_to_table (const HyScanDataSchemaNode *node,
                                  GHashTable                 *keys)
{
  GList *link;

  for (link = node->nodes; link != NULL; link = link->next)
    hyscan_param_merge_node_to_table (link->data, keys);

  for (link = node->keys; link != NULL; link = link->next)
    {
      HyScanDataSchemaKey *key = link->data;

      g_hash_table_insert (keys, (gpointer) key->id, key);
    }
}

/* Обходит все узлы node и составляет схему, оставляя только ключи из keys. */
static void
hyscan_param_merge_schema_build (HyScanParamMerge           *merge,
                                 const HyScanDataSchemaNode *node,
                                 GHashTable                 *keys)
{
  HyScanParamMergePrivate *priv = merge->priv;
  GList *link;

  for (link = node->nodes; link != NULL; link = link->next)
    hyscan_param_merge_node_to_table (link->data, keys);

  for (link = node->keys; link != NULL; link = link->next)
    {
      HyScanDataSchemaKey *key = link->data;

      gboolean added;

      if (!g_hash_table_contains (keys, key->id))
        continue;

      switch (key->type)
        {
          case HYSCAN_DATA_SCHEMA_KEY_BOOLEAN:
            added = hyscan_param_merge_add_key_boolean (merge, key->id);
            break;

          case HYSCAN_DATA_SCHEMA_KEY_INTEGER:
            added = hyscan_param_merge_add_key_integer (merge, key->id);
            break;

          case HYSCAN_DATA_SCHEMA_KEY_DOUBLE:
            added = hyscan_param_merge_add_key_double (merge, key->id);
            break;

          case HYSCAN_DATA_SCHEMA_KEY_STRING:
            added = hyscan_param_merge_add_key_string (merge, key->id);
            break;

          case HYSCAN_DATA_SCHEMA_KEY_ENUM:
            added = hyscan_param_merge_add_key_enum (merge, key->id);
            break;

          case HYSCAN_DATA_SCHEMA_KEY_INVALID:
          default:
            added = FALSE;
        }

      if (!added)
        continue;

      hyscan_data_schema_builder_key_set_view (priv->builder, key->id, key->view);
      hyscan_data_schema_builder_key_set_access (priv->builder, key->id, key->access);
    }
}

/* Находит соответствующее числовое значения для перечисления. */
static gboolean
hyscan_param_merge_enum_map (HyScanDataSchema *schema,
                             HyScanParam      *child,
                             const gchar      *key_id,
                             gint64            value,
                             gint64           *child_value)
{
  HyScanDataSchema *child_schema = hyscan_param_schema (child);

  const gchar *enum_id;
  const HyScanDataSchemaEnumValue *parent_enum, *child_enum;

  enum_id = hyscan_data_schema_key_get_enum_id (schema, key_id);

  parent_enum = hyscan_data_schema_enum_find_by_value (schema, enum_id, value);
  child_enum = hyscan_data_schema_enum_find_by_id (child_schema, enum_id, parent_enum->id);

  g_object_unref (child_schema);

  if (child_enum == NULL)
    return FALSE;

  *child_value = child_enum->value;

  return TRUE;
}

static HyScanDataSchema *
hyscan_param_merge_schema (HyScanParam *param)
{
  HyScanParamMerge *merge = HYSCAN_PARAM_MERGE (param);
  HyScanParamMergePrivate *priv = merge->priv;

  return priv->schema != NULL ? g_object_ref (priv->schema) : NULL;
}

static gboolean
hyscan_param_merge_set (HyScanParam     *param,
                        HyScanParamList *list)
{
  HyScanParamMerge *merge = HYSCAN_PARAM_MERGE (param);
  HyScanParamMergePrivate *priv = merge->priv;
  HyScanParamList *filtered;
  GList *link;

  GList *enum_keys = NULL;
  const gchar *const *param_keys;
  gint i;

  gboolean result = TRUE;

  if (priv->schema == NULL || priv->params == NULL)
    return FALSE;
  
  param_keys = hyscan_param_list_params (list);
  if (param_keys == NULL)
    return TRUE;

  /* Убираем те ключи, значения которых надо сохранить без изменений. */
  filtered = hyscan_param_list_new ();
  for (i = 0; param_keys[i] != NULL; i++)
    {
      GVariant *value;

      if (g_hash_table_contains (priv->keep, param_keys[i]))
        continue;
      
      value = hyscan_param_list_get (list, param_keys[i]);
      hyscan_param_list_set (filtered, param_keys[i], value);
      g_variant_unref (value);
      
      /* Формируем список перечислений. */
      if (hyscan_data_schema_key_get_value_type (priv->schema, param_keys[i]) == HYSCAN_DATA_SCHEMA_KEY_ENUM)
        enum_keys = g_list_append (enum_keys, (gpointer) param_keys[i]);
    }

  /* Устанавливаем значения всем дочерним параметрам. */
  for (link = priv->params; link != NULL; link = link->next)
    {
      HyScanParam *child = link->data;
      GList *link_enum;

      /* Определяем соответствие чилсенных значений перечислений в двух схемах. */
      for (link_enum = enum_keys; link_enum != NULL; link_enum = link_enum->next)
        {
          const gchar *key_id = link_enum->data;
          gint64 value, child_value;

          value = hyscan_param_list_get_enum (list, key_id);
          if (!hyscan_param_merge_enum_map (priv->schema, child, key_id, value, &child_value))
            goto exit;

          hyscan_param_list_set_enum (filtered, key_id, child_value);
        }

      if (!(result = hyscan_param_set (child, filtered)))
        goto exit;
    }

exit:
  g_list_free (enum_keys);
  g_object_unref (filtered);

  return result;
}

static gboolean
hyscan_param_merge_get (HyScanParam     *param,
                        HyScanParamList *list)
{
  HyScanParamMerge *merge = HYSCAN_PARAM_MERGE (param);
  HyScanParamMergePrivate *priv = merge->priv;
  HyScanParam *base;

  if (priv->schema == NULL || priv->params == NULL)
    return FALSE;

  /* Делегируем получение значений первому параметру. */
  base = priv->params->data;

  return hyscan_param_get (base, list);
}

static void
hyscan_param_merge_interface_init (HyScanParamInterface *iface)
{
  iface->set = hyscan_param_merge_set;
  iface->get = hyscan_param_merge_get;
  iface->schema = hyscan_param_merge_schema;
}

/**
 * hyscan_param_merge_new:
 *
 * Создаёт новый объект #HyScanParamMerge.
 *
 * Returns: (transfer full): указатель на новый объект #HyScanParamMerge, для удаления g_object_unref()
 */
HyScanParamMerge *
hyscan_param_merge_new (void)
{
  return g_object_new (HYSCAN_TYPE_PARAM_MERGE, NULL);
}

/**
 * hyscan_param_merge_add:
 * @merge: указатель на #HyScanParamMerge
 * @param: указатель на #HyScanParam
 *
 * Добавляет параметр @param в список для слияния.
 *
 * Returns: %TRUE в случае успеха, иначе %FALSE
 */
gboolean
hyscan_param_merge_add (HyScanParamMerge *merge,
                         HyScanParam     *param)
{
  HyScanParamMergePrivate *priv;

  g_return_val_if_fail (HYSCAN_IS_PARAM_MERGE (merge), FALSE);

  priv = merge->priv;

  if (priv->builder == NULL)
    return FALSE;

  priv->params = g_list_append (priv->params, g_object_ref (param));
  priv->schemas = g_list_append (priv->schemas, hyscan_param_schema (param));

  return TRUE;
}

/**
 * hyscan_param_merge_bind:
 * @merge: указатель на #HyScanParamMerge
 *
 * Завершает добавление дочерних параметров. После вызова этой функции будет
 * сформирована общая схема для всех ранее добавленных параметров, и объект
 * @merge может быть использован в качества #HyScanParam.
 *
 * Returns: %TRUE в случае успеха, иначе %FALSE.
 */
gboolean
hyscan_param_merge_bind (HyScanParamMerge *merge)
{
  HyScanParamMergePrivate *priv;

  GList *link;
  HyScanDataSchema *base;

  GHashTable *schema_keys;
  GHashTableIter iter;
  HyScanDataSchemaKey *schema_key;
  gchar *key_id;

  g_return_val_if_fail (HYSCAN_IS_PARAM_MERGE (merge), FALSE);

  priv = merge->priv;

  if (priv->builder == NULL || priv->params == NULL)
    return FALSE;

  /* Берём ключи и определяем их типы по схеме первого параметра. */
  schema_keys = g_hash_table_new (g_str_hash, g_str_equal);
  base = priv->schemas->data;
  hyscan_param_merge_node_to_table (hyscan_data_schema_list_nodes (base), schema_keys);

  /* Отбираем только те ключи, где типы данных и параметры отображения совпадают во всех остальных схемах. */
  for (link = priv->schemas->next; link != NULL; link = link->next)
    {
      HyScanDataSchema *other_schema = link->data;

      g_hash_table_iter_init (&iter, schema_keys);
      while (g_hash_table_iter_next (&iter, (gpointer *) &key_id, (gpointer) &schema_key))
        {
          if (schema_key->type != hyscan_data_schema_key_get_value_type (other_schema, key_id) ||
              schema_key->access != hyscan_data_schema_key_get_access (other_schema, key_id) ||
              schema_key->view != hyscan_data_schema_key_get_view (other_schema, key_id))
            {
              g_hash_table_iter_remove (&iter);
            }
        }
    }

  /* По оставшимся ключам формируем схему. */
  hyscan_param_merge_schema_build (merge, hyscan_data_schema_list_nodes (base), schema_keys);

  priv->schema = hyscan_data_schema_builder_get_schema (priv->builder);

  hyscan_param_merge_bind_keep (merge);

  g_hash_table_destroy (schema_keys);

  return TRUE;
}

/**
 * hyscan_param_merge_get_keep:
 * @merge: указатель на #HyScanParamMerge
 * @key_id: идентификатор параметра
 *
 * Считывает признак того, надо ли хранить для каждого из дочерних объектов #HyScanParam
 * значение параметра @key_id неизменным.
 *
 * Returns: %TRUE, если каждый внутренний объект #HyScanParam хранит своё значение параметра неизменным;
 *   %FALSE - если всем #HyScanParam будет установлено одинаковое значение параметра.
 */
gboolean
hyscan_param_merge_get_keep (HyScanParamMerge *merge,
                             const gchar      *key_id)
{
  HyScanParamMergePrivate *priv;

  g_return_val_if_fail (HYSCAN_IS_PARAM_MERGE (merge), FALSE);

  priv = merge->priv;

  return g_hash_table_contains (priv->keep, key_id);
}

/**
 * hyscan_param_merge_set_keep:
 * @merge:
 * @key_id: идентификатор параметра
 * @keep: сохранить значение без изменений
 *
 * Устанавливает, надо ли в каждом из дочерних элементов сохранить значение параметра @key_id неизменным.
 *
 * Если @keep = %FALSE, то при вызове hyscan_param_set() для всех внутренних объектов
 * #HyScanParam будет установлено одинаковое значение. Если @keep = %TRUE, то значение
 * параметра для каждого объекта останется неизменным.
 */
void
hyscan_param_merge_set_keep (HyScanParamMerge *merge,
                             const gchar      *key_id,
                             gboolean          keep)
{
  HyScanParamMergePrivate *priv;

  g_return_if_fail (HYSCAN_IS_PARAM_MERGE (merge));

  priv = merge->priv;

  if (keep)
    g_hash_table_add (priv->keep, g_strdup (key_id));
  else
    g_hash_table_remove (priv->keep, key_id);
}
