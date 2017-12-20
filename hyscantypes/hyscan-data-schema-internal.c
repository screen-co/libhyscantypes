/* hyscan-data-schema-internal.c
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
      if ((name[i] >= 'A') && (name[i] <= 'Z'))
        match = TRUE;
      if ((name[i] >= '0') && (name[i] <= '9'))
        match = TRUE;
      if ((name[i] == '-') || (name[i] == '_') || (name[i] == '.'))
        match = TRUE;

      if (!match)
        return FALSE;
    }

  return TRUE;
}

/* Функция проверяет идентификатор на предмет допустимости. */
gboolean
hyscan_data_schema_internal_validate_id (const gchar *id)
{
  gchar **pathv;
  guint i;

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

/* Функция добавляет новый параметр в список. */
void
hyscan_data_schema_internal_node_insert_key (HyScanDataSchemaNode *node,
                                             HyScanDataSchemaKey  *key)
{
  gchar **pathv;
  guint n_pathv;
  guint i;

  /* Поиск узла для параметра. */
  pathv = g_strsplit (key->id, "/", -1);
  n_pathv = g_strv_length (pathv);
  for (i = 1; i < n_pathv - 1; i++)
    {
      GList *nodes = node->nodes;
      gboolean has_node = FALSE;

      /* Сверяем путь для текущего узла с идентификатором параметра. */
      while (nodes != NULL)
        {
          HyScanDataSchemaNode *cur_node = nodes->data;
          const gchar *path = cur_node->path;
          guint id_len = strlen (key->id);
          guint path_len = strlen (path);

          /* Если совпадают, мы на правильном пути:) */
          if (g_str_has_prefix (key->id, path) && (id_len > path_len) && (key->id[path_len] == '/'))
            {
              node = nodes->data;
              has_node = TRUE;
              break;
            }

          nodes = nodes->next;
        }

      /* Если узел не найден, добавляем его. */
      if (!has_node)
        {
          HyScanDataSchemaNode *new_node;
          gchar *cur_path;

          cur_path = g_strdup_printf ("%s/%s", node->path, pathv[i]);
          new_node= hyscan_data_schema_node_new (cur_path, NULL, NULL);
          g_free (cur_path);

          node->nodes = g_list_append (node->nodes, new_node);
          node = new_node;
        }
    }

  g_strfreev (pathv);

  /* Добавляем новый параметр. */
  node->keys = g_list_append (node->keys, key);
}
