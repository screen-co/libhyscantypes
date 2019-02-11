/* hyscan-param-list.c
 *
 * Copyright 2017-2018 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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
 * SECTION: hyscan-param-list
 * @Short_description: класс для чтения/записи значений параметров
 * @Title: HyScanParamList
 *
 * Класс предназначен для чтения/записи параметров через интерфейс #HyScanParam.
 *
 * Создание объекта класса осуществляется функцией #hyscan_param_list_new. Класс
 * содержит список имён параметров и при необходимости их значений.
 *
 * Добавление параметров в список осуществляется функциями:
 *
 * - #hyscan_param_list_add - добавляет параметр без значения;
 * - #hyscan_param_list_set - добавляет параметр со значением в виде #GVariant;
 * - #hyscan_param_list_set_boolean - добавляет #HYSCAN_DATA_SCHEMA_KEY_BOOLEAN параметр со значением;
 * - #hyscan_param_list_set_integer - добавляет #HYSCAN_DATA_SCHEMA_KEY_INTEGER параметр со значением;
 * - #hyscan_param_list_set_double - добавляет #HYSCAN_DATA_SCHEMA_KEY_DOUBLE параметр со значением;
 * - #hyscan_param_list_set_string - добавляет #HYSCAN_DATA_SCHEMA_KEY_STRING параметр со значением;
 * - #hyscan_param_list_set_enum - добавляет #HYSCAN_DATA_SCHEMA_KEY_ENUM параметр со значением;
 *
 * Добавление параметров без значения может использоваться для считывания
 * значений #hyscan_param_get и для установки значений по умолчанию #hyscan_param_set.
 *
 * Считывание значений параметров из списка осуществляется функциями:
 *
 * - #hyscan_param_list_get - считывает значение параметра в виде GVariant;
 * - #hyscan_param_list_get_boolean - считывает значение HYSCAN_DATA_SCHEMA_KEY_BOOLEAN параметра;
 * - #hyscan_param_list_get_integer - считывает значение HYSCAN_DATA_SCHEMA_KEY_INTEGER параметра;
 * - #hyscan_param_list_get_double - считывает значение HYSCAN_DATA_SCHEMA_KEY_DOUBLE параметра;
 * - #hyscan_param_list_get_string - считывает значение HYSCAN_DATA_SCHEMA_KEY_STRING параметра;
 * - #hyscan_param_list_dup_string - считывает значение HYSCAN_DATA_SCHEMA_KEY_STRING параметра;
 * - #hyscan_param_list_get_enum - считывает значение HYSCAN_DATA_SCHEMA_KEY_ENUM параметра.
 *
 * Текущий список имён параметров можно получить функцией #hyscan_param_list_params.
 *
 * Проверить параметр на присутствие в списке можно функцией #hyscan_param_list_contains.
 *
 * Очистить список параметров можно функцией #hyscan_param_list_clear.
 *
 * Добавить ключи из одного списка параметров в другой можно функцией #hyscan_param_list_update
 *
 * Данный класс не является потокобезопасным.
 */

#include "hyscan-param-list.h"

struct _HyScanParamListPrivate
{
  GHashTable                  *params;                 /* Список параметров. */
  gchar                      **names;                  /* Список имён параметров. */
};

static void    hyscan_param_list_object_finalize       (GObject                 *object);

static void    hyscan_param_list_free_param            (gpointer                 data);

static void    hyscan_param_list_update_names          (HyScanParamListPrivate  *priv);

G_DEFINE_TYPE_WITH_PRIVATE (HyScanParamList, hyscan_param_list, G_TYPE_OBJECT)

static void
hyscan_param_list_class_init (HyScanParamListClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = hyscan_param_list_object_finalize;
}

static void
hyscan_param_list_init (HyScanParamList *list)
{
  list->priv = hyscan_param_list_get_instance_private (list);

  list->priv->params = g_hash_table_new_full (g_str_hash, g_str_equal,
                                              g_free, hyscan_param_list_free_param);
}

static void
hyscan_param_list_object_finalize (GObject *object)
{
  HyScanParamList *list = HYSCAN_PARAM_LIST (object);

  g_hash_table_unref(list->priv->params);
  g_strfreev (list->priv->names);

  G_OBJECT_CLASS (hyscan_param_list_parent_class)->finalize (object);
}

static void
hyscan_param_list_free_param (gpointer data)
{
  g_clear_pointer (&data, g_variant_unref);
}

/* Функция обновляет внутренний список имён параметров. */
static void
hyscan_param_list_update_names (HyScanParamListPrivate *priv)
{
  GHashTableIter iter;
  gpointer key, value;
  guint n_names;
  guint i = 0;

  g_clear_pointer (&priv->names, g_strfreev);
  n_names = g_hash_table_size (priv->params);
  if (n_names == 0)
    return;

  priv->names = g_new0 (gchar*, n_names + 1);
  g_hash_table_iter_init (&iter, priv->params);
  while (g_hash_table_iter_next (&iter, &key, &value))
    priv->names[i++] = g_strdup (key);
}

/**
 * hyscan_param_list_new:
 *
 * Функция создаёт новый объект #HyScanParamList.
 *
 * Returns: #HyScanParamList. Для удаления #g_object_unref.
 */
HyScanParamList *
hyscan_param_list_new (void)
{
  return g_object_new (HYSCAN_TYPE_PARAM_LIST, NULL);
}

/**
 * hyscan_param_list_update:
 * @list: указатель на #HyScanParamList приемник
 * @orig: указатель на #HyScanParamList источник
 *
 * Функция копирует параметры из одного #HyScanParamList в другой. Если
 * какие-то параметры присутствуют в обеих списках, они будут перезаписаны.
 * Если какие-то параметры присутствуют только в приемнике, то они удалены
 * не будут
 */
void
hyscan_param_list_update (HyScanParamList *list,
                          HyScanParamList *orig)
{
  const gchar * const * params;
  GVariant * value;

  g_return_if_fail (HYSCAN_IS_PARAM_LIST (list));
  g_return_if_fail (HYSCAN_IS_PARAM_LIST (orig));

  params = hyscan_param_list_params (orig);

  if (params == NULL)
    return;

  for (; *params != NULL; ++params)
    {
      value = hyscan_param_list_get (orig, *params);
      hyscan_param_list_set (list, *params, value);
      g_variant_unref (value);
    }
}

/**
 * hyscan_param_list_params:
 * @list: указатель на #HyScanParamList
 *
 * Функция возвращает текущий список имён параметров. Память, занимаемая
 * списком, принадлежит объекту и не должна изменяться пользователем.
 * Список имён действителен только до изменения параметров.
 *
 * Returns: (transfer none): Текущий список имён параметров.
 */
const gchar * const *
hyscan_param_list_params (HyScanParamList *list)
{
  g_return_val_if_fail (HYSCAN_IS_PARAM_LIST (list), NULL);

  return (const gchar **) list->priv->names;
}

/**
 * hyscan_param_list_contains:
 * @list: указатель на #HyScanParamList
 * @name: название параметра
 *
 * Функция проверяет список на присутствие в нём параметр с указанным именем.
 *
 * Returns: %TRUE если параметр есть в списке, иначе %FALSE.
 */
gboolean
hyscan_param_list_contains (HyScanParamList *list,
                            const gchar     *name)
{
  g_return_val_if_fail (HYSCAN_IS_PARAM_LIST (list), FALSE);

  return g_hash_table_contains (list->priv->params, name);
}

/**
 * hyscan_param_list_clear:
 * @list: указатель на #HyScanParamList
 *
 * Функция очищает текущий список имён параметров.
 */
void
hyscan_param_list_clear (HyScanParamList *list)
{
  g_return_if_fail (HYSCAN_IS_PARAM_LIST (list));

  g_hash_table_remove_all (list->priv->params);

  hyscan_param_list_update_names (list->priv);
}

/**
 * hyscan_param_list_add:
 * @list: указатель на #HyScanParamList
 * @name: название параметра
 *
 * Функция добавляет параметр без значения в список. Если параметр
 * с таким именем уже существует, он будет заменён на параметр без значения.
 */
void
hyscan_param_list_add (HyScanParamList *list,
                       const gchar     *name)
{
  g_return_if_fail (HYSCAN_IS_PARAM_LIST (list));

  if (g_hash_table_insert (list->priv->params, g_strdup (name), NULL))
    hyscan_param_list_update_names (list->priv);
}

/**
 * hyscan_param_list_set:
 * @list: указатель на #HyScanParamList
 * @name: название параметра
 * @value: значение параметра или NULL
 *
 * Функция добавляет параметр в список со значением в виде #GVariant.
 * Если параметр с таким именем уже существует, он будет заменён.
 */
void
hyscan_param_list_set (HyScanParamList *list,
                       const gchar     *name,
                       GVariant        *value)
{
  g_return_if_fail (HYSCAN_IS_PARAM_LIST (list));

  if (value != NULL)
    g_variant_ref_sink (value);

  if (g_hash_table_insert (list->priv->params, g_strdup (name), value))
    hyscan_param_list_update_names (list->priv);
}

/**
 * hyscan_param_list_set_boolean:
 * @list: указатель на #HyScanParamList
 * @name: название параметра
 * @value: значение параметра
 *
 * Функция добавляет #HYSCAN_DATA_SCHEMA_KEY_BOOLEAN параметр в список.
 * Если параметр с таким именем уже существует, он будет заменён.
 */
void
hyscan_param_list_set_boolean (HyScanParamList *list,
                               const gchar     *name,
                               gboolean         value)
{
  hyscan_param_list_set (list, name, g_variant_new_boolean (value));
}

/**
 * hyscan_param_list_set_integer:
 * @list: указатель на #HyScanParamList
 * @name: название параметра
 * @value: значение параметра
 *
 * Функция добавляет #HYSCAN_DATA_SCHEMA_KEY_INTEGER параметр в список.
 * Если параметр с таким именем уже существует, он будет заменён.
 */
void
hyscan_param_list_set_integer (HyScanParamList *list,
                               const gchar     *name,
                               gint64           value)
{
  hyscan_param_list_set (list, name, g_variant_new_int64 (value));
}

/**
 * hyscan_param_list_set_double:
 * @list: указатель на #HyScanParamList
 * @name: название параметра
 * @value: значение параметра
 *
 * Функция добавляет #HYSCAN_DATA_SCHEMA_KEY_DOUBLE параметр в список.
 * Если параметр с таким именем уже существует, он будет заменён.
 */
void
hyscan_param_list_set_double (HyScanParamList *list,
                              const gchar     *name,
                              gdouble          value)
{
  hyscan_param_list_set (list, name, g_variant_new_double (value));
}

/**
 * hyscan_param_list_set_string:
 * @list: указатель на #HyScanParamList
 * @name: название параметра
 * @value: значение параметра
 *
 * Функция добавляет #HYSCAN_DATA_SCHEMA_KEY_STRING параметр в список.
 * Если параметр с таким именем уже существует, он будет заменён.
 */
void
hyscan_param_list_set_string (HyScanParamList *list,
                              const gchar     *name,
                              const gchar     *value)
{
  hyscan_param_list_set (list, name, (value != NULL) ? g_variant_new_string (value) : NULL);
}

/**
 * hyscan_param_list_set_enum:
 * @list: указатель на #HyScanParamList
 * @name: название параметра
 * @value: значение параметра
 *
 * Функция добавляет #HYSCAN_DATA_SCHEMA_KEY_ENUM параметр в список.
 * Если параметр с таким именем уже существует, он будет заменён.
 */
void
hyscan_param_list_set_enum (HyScanParamList *list,
                            const gchar     *name,
                            gint64           value)
{
  hyscan_param_list_set_integer (list, name, value);
}

/**
 * hyscan_param_list_get:
 * @list: указатель на #HyScanParamList
 * @name: название параметра
 *
 * Функция считывает значение параметра из списка в виде GVariant.
 *
 * Returns: (transfer full): Значение параметра или NULL.
 * Для удаления #g_variant_unref.
 */
GVariant *
hyscan_param_list_get (HyScanParamList *list,
                       const gchar     *name)
{
  GVariant *value;

  g_return_val_if_fail (HYSCAN_IS_PARAM_LIST (list), NULL);

  value = g_hash_table_lookup (list->priv->params, name);
  if (value != NULL)
    g_variant_ref_sink (value);

  return value;
}

/**
 * hyscan_param_list_get_boolean:
 * @list: указатель на #HyScanParamList
 * @name: название параметра
 *
 * Функция считывает значение #HYSCAN_DATA_SCHEMA_KEY_BOOLEAN параметра из списка.
 *
 * Returns: Значение параметра или FALSE.
 */
gboolean
hyscan_param_list_get_boolean (HyScanParamList *list,
                               const gchar     *name)
{
  GVariant *value;

  g_return_val_if_fail (HYSCAN_IS_PARAM_LIST (list), FALSE);

  value = g_hash_table_lookup (list->priv->params, name);
  if ((value == NULL) || (g_variant_classify (value) != G_VARIANT_CLASS_BOOLEAN))
    return FALSE;

  return g_variant_get_boolean (value);
}

/**
 * hyscan_param_list_get_integer:
 * @list: указатель на #HyScanParamList
 * @name: название параметра
 *
 * Функция считывает значение #HYSCAN_DATA_SCHEMA_KEY_INTEGER параметра из списка.
 *
 * Returns: Значение параметра или 0.
 */
gint64
hyscan_param_list_get_integer (HyScanParamList *list,
                               const gchar     *name)
{
  GVariant *value;

  g_return_val_if_fail (HYSCAN_IS_PARAM_LIST (list), 0);

  value = g_hash_table_lookup (list->priv->params, name);
  if ((value == NULL) || (g_variant_classify (value) != G_VARIANT_CLASS_INT64))
    return 0;

  return g_variant_get_int64 (value);
}

/**
 * hyscan_param_list_get_double:
 * @list: указатель на #HyScanParamList
 * @name: название параметра
 *
 * Функция считывает значение #HYSCAN_DATA_SCHEMA_KEY_DOUBLE параметра из списка.
 *
 * Returns: Значение параметра или 0.0.
 */
gdouble
hyscan_param_list_get_double (HyScanParamList *list,
                              const gchar     *name)
{
  GVariant *value;

  g_return_val_if_fail (HYSCAN_IS_PARAM_LIST (list), 0.0);

  value = g_hash_table_lookup (list->priv->params, name);
  if ((value == NULL) || (g_variant_classify (value) != G_VARIANT_CLASS_DOUBLE))
    return 0.0;

  return g_variant_get_double (value);
}

/**
 * hyscan_param_list_get_string:
 * @list: указатель на #HyScanParamList
 * @name: название параметра
 *
 * Функция считывает значение #HYSCAN_DATA_SCHEMA_KEY_STRING параметра из списка.
 *
 * Returns: Значение параметра или NULL.
 */
const gchar *
hyscan_param_list_get_string (HyScanParamList *list,
                              const gchar     *name)
{
  GVariant *value;

  g_return_val_if_fail (HYSCAN_IS_PARAM_LIST (list), NULL);

  value = g_hash_table_lookup (list->priv->params, name);
  if ((value == NULL) || (g_variant_classify (value) != G_VARIANT_CLASS_STRING))
    return NULL;

  return g_variant_get_string (value, NULL);
}

/**
 * hyscan_param_list_dup_string:
 * @list: указатель на #HyScanParamList
 * @name: название параметра
 *
 * Функция считывает значение #HYSCAN_DATA_SCHEMA_KEY_STRING параметра из списка
 * и возвращает его копию.
 *
 * Returns: Значение параметра или NULL. Для удаления #g_free.
 */
gchar *
hyscan_param_list_dup_string (HyScanParamList *list,
                              const gchar     *name)
{
  return g_strdup (hyscan_param_list_get_string (list, name));
}

/**
 * hyscan_param_list_get_enum:
 * @list: указатель на #HyScanParamList
 * @name: название параметра
 *
 * Функция считывает значение #HYSCAN_DATA_SCHEMA_KEY_ENUM параметра из списка.
 *
 * Returns: Значение параметра или 0.
 */
gint64
hyscan_param_list_get_enum (HyScanParamList *list,
                            const gchar     *name)
{
  return hyscan_param_list_get_integer (list, name);
}

