/* hyscan-data-schema-internal.c
 *
 * Copyright 2016-2018 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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

#include "hyscan-data-schema-internal.h"
#include <string.h>

/* Функция проверяет имя на предмет допустимости. */
gboolean
hyscan_data_schema_internal_validate_name (const gchar *name)
{
  guint i;

  for (i = 0; name[i] != '\0'; i++)
    {
      gboolean match = FALSE;

      if ((name[i] >= 'a') && (name[i] <= 'z'))
        match = TRUE;
      else if ((name[i] >= 'A') && (name[i] <= 'Z'))
        match = TRUE;
      else if ((name[i] >= '0') && (name[i] <= '9'))
        match = TRUE;
      else if ((name[i] == '-') || (name[i] == '_') || (name[i] == '.'))
        match = TRUE;

      if (!match)
        return FALSE;
    }

  return (i > 0) ? TRUE : FALSE;
}

/* Функция проверяет идентификатор на предмет допустимости. */
gboolean
hyscan_data_schema_internal_validate_id (const gchar *id)
{
  gchar **pathv;
  guint i;

  if (g_strcmp0 (id, "/") == 0)
    return TRUE;

  pathv = g_strsplit (id, "/", -1);
  if (g_strv_length (pathv) < 2)
    return FALSE;

  for (i = 1; pathv[i] != NULL; i++)
    if (!hyscan_data_schema_internal_validate_name (pathv[i]))
      {
        g_strfreev (pathv);
        return FALSE;
      }

  g_strfreev (pathv);
  return TRUE;
}

/* Функция нормализует путь к узлу или параметру. */
gchar *
hyscan_data_schema_internal_normalize_id (const gchar *id)
{
  gchar *idn;
  gsize len;

  /* Путь должен начинаться с символа '/'. */
  if (*id != '/')
    return NULL;

  /* Специальный случай корневого узла. */
  if (g_strcmp0 (id, "/") == 0)
    return g_strdup ("/");

  /* Путь не может быть короче двух символов. */
  len = strlen (id);
  if (len < 2)
    return NULL;

  /* В конце не должно быть символа '/'. */
  idn = g_strdup (id);
  if (idn[len - 1] == '/')
    idn[len - 1] = 0;

  if (!hyscan_data_schema_internal_validate_id (idn))
    g_clear_pointer (&idn, g_free);

  return idn;
}

/* Функция объёдиняет два пути в один. */
gchar *
hyscan_data_schema_internal_make_path (const gchar *path1,
                                       const gchar *path2)
{
  const gchar *separator = (*path2 == '/') ? "" : "/";

  if (g_strcmp0 (path1, "/") == 0)
    return g_strdup_printf ("%s%s", separator, path2);

  return g_strdup_printf ("%s%s%s", path1, separator, path2);
}

/* Функция определяет является ли путь dst_path частью пути src_path. */
gboolean
hyscan_data_schema_internal_is_path (const gchar *src_path,
                                     const gchar *dst_path)
{
  gsize len = strlen (dst_path);

  if (g_strcmp0 (dst_path, "/") == 0)
    return TRUE;

  if (g_str_has_prefix (src_path, dst_path) &&
      ((src_path[len] == '/') || (src_path[len] == 0)))
    {
      return TRUE;
    }

  return FALSE;
}

/* Функция проверяет значение перечисляемого типа на допустимость. */
gboolean
hyscan_data_schema_internal_enum_check (GList  *values,
                                        gint64  value)
{
  while (values != NULL)
    {
      if (((HyScanDataSchemaEnumValue*)values->data)->value == value)
        return TRUE;

      values = values->next;
    }

  return FALSE;
}

/* Функция освобождает память занятую списком со значениями типа enum. */
void
hyscan_data_schema_internal_enum_free (GList *values)
{
  g_list_free_full (values, (GDestroyNotify)hyscan_data_schema_enum_value_free);
}

/* Функция создаёт новую структуру HyScanDataSchemaInternalNode. */
HyScanDataSchemaInternalNode *
hyscan_data_schema_internal_node_new (const gchar *path,
                                      const gchar *name,
                                      const gchar *description)
{
  HyScanDataSchemaInternalNode *new_node = g_slice_new0 (HyScanDataSchemaInternalNode);

  new_node->path = g_strdup (path);
  new_node->name = g_strdup (name);
  new_node->description = g_strdup (description);

  return new_node;
}

/* Функция освобождает память занятую структурой с узлом. */
void
hyscan_data_schema_internal_node_free (HyScanDataSchemaInternalNode *node)
{
  g_free (node->path);
  g_free (node->name);
  g_free (node->description);

  g_slice_free (HyScanDataSchemaInternalNode, node);
}

/* Функция создаёт новую структуру HyScanDataSchemaInternalKey. */
HyScanDataSchemaInternalKey *
hyscan_data_schema_internal_key_new (const gchar *id,
                                     const gchar *name,
                                     const gchar *description)
{
  HyScanDataSchemaInternalKey *new_key = g_slice_new0 (HyScanDataSchemaInternalKey);

  new_key->id = g_strdup (id);
  new_key->name = g_strdup (name);
  new_key->description = g_strdup (description);
  new_key->access = HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT;

  return new_key;
}

/* Функция освобождает память занятую структурой с параметром. */
void
hyscan_data_schema_internal_key_free (HyScanDataSchemaInternalKey *key)
{
  g_free (key->id);
  g_free (key->name);
  g_free (key->description);
  g_free (key->enum_id);

  g_clear_pointer (&key->default_value, g_variant_unref);
  g_clear_pointer (&key->minimum_value, g_variant_unref);
  g_clear_pointer (&key->maximum_value, g_variant_unref);
  g_clear_pointer (&key->value_step, g_variant_unref);

  g_slice_free (HyScanDataSchemaInternalKey, key);
}

/* Функция добавляет новый узел в список и возвращает указатель на него. */
HyScanDataSchemaNode *
hyscan_data_schema_internal_insert_node (HyScanDataSchemaNode *node,
                                         const gchar          *path)
{
  HyScanDataSchemaNode *up_node = node;
  GList *nodes = node->nodes;
  gchar **pathv = g_strsplit (path, "/", -1);
  guint n_pathv = 1;

  /* Цикл по всем узлам, до тех пор пока не найдём нужный. */
  while (g_strcmp0 (path, node->path) != 0)
    {
      /* Цикл по дочерним узлам текущего узла. */
      if (nodes != NULL)
        {
          node = nodes->data;

          /* Если мы на нужном пути, проваливаемся далее. */
          if (hyscan_data_schema_internal_is_path (path, node->path))
            {
              n_pathv += 1;
              up_node = node;
              nodes = node->nodes;
            }

          /* Смотрим следующий узел на текущем уровне. */
          else
            {
              nodes = g_list_next (nodes);

              /* Если все узлы закончились, а нужный не найден - создаём его. */
              if (nodes == NULL)
                node = up_node;
            }
        }

      /* Просмотрели все узлы текущего уровня и не нашли нужный. */
      else
        {
          HyScanDataSchemaNode *new_node;
          gchar *new_path;

          new_path = hyscan_data_schema_internal_make_path (node->path, pathv[n_pathv]);
          new_node= hyscan_data_schema_node_new (new_path, NULL, NULL, NULL, NULL);
          g_free (new_path);

          node->nodes = g_list_append (node->nodes, new_node);
          node = new_node;
          n_pathv += 1;
        }
    }

  g_strfreev (pathv);

  return node;
}

/* Функция добавляет новый параметр в список. */
void
hyscan_data_schema_internal_node_insert_key (HyScanDataSchemaNode *node,
                                             HyScanDataSchemaKey  *key)
{
  gchar *path;
  gsize i;

  /* Обрезаем последний компонент пути. */
  path = g_strdup (key->id);
  for (i = strlen (path); (path[i] != '/') && (i > 0); i--)
    path[i] = 0;
  if (i > 0)
    path[i] = 0;

  /* Добавляем новый параметр. */
  node = hyscan_data_schema_internal_insert_node (node, path);
  node->keys = g_list_append (node->keys, key);

  g_free (path);
}
