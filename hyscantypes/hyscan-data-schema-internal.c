/*
 * \file hyscan-data-schema-internal.c
 *
 * \brief Исходный файл внутренних функций для работы со схемами.
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2016
 * \license Проприетарная лицензия ООО "Экран"
 *
 */

#include "hyscan-data-schema-internal.h"

/* Функция проверяет имя на предмет допустимости. */
gboolean
hyscan_data_schema_internal_validate_name (const gchar *name)
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

/* Функция проверяет идентификатор на предмет допустимости. */
gboolean
hyscan_data_schema_internal_validate_id (const gchar *id)
{
  gchar **pathv;
  guint n_pathv;
  guint i;

  pathv = g_strsplit (id, "/", -1);
  n_pathv = g_strv_length (pathv);
  if (n_pathv < 2)
    return FALSE;

  for (i = 1; i < n_pathv - 1; i++)
    if (!hyscan_data_schema_internal_validate_name (pathv[i]))
      {
        g_strfreev (pathv);
        return FALSE;
      }

  g_strfreev (pathv);
  return TRUE;
}

/* Функция создаёт новый узел с параметрами. */
HyScanDataSchemaNode *
hyscan_data_schema_internal_node_new (const gchar *path)
{
  HyScanDataSchemaNode *node;

  node = g_new0 (HyScanDataSchemaNode, 1);
  node->path = g_strdup (path);

  return node;
}

/* Функция добавляет новый параметр в список. */
void
hyscan_data_schema_internal_node_insert_key (HyScanDataSchemaNode *node,
                                             const gchar          *id,
                                             const gchar          *name,
                                             const gchar          *description,
                                             HyScanDataSchemaType  type,
                                             gboolean              readonly)
{
  HyScanDataSchemaKey *key;

  gchar **pathv;
  guint n_pathv;
  guint i, j;

  /* Поиск узла для параметра. */
  pathv = g_strsplit (id, "/", -1);
  n_pathv = g_strv_length (pathv);
  for (i = 1; i < n_pathv - 1; i++)
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
      node->nodes[node->n_nodes] = hyscan_data_schema_internal_node_new (cur_path);
      node->n_nodes += 1;
      node->nodes[node->n_nodes] = NULL;
      node = node->nodes[j];
      g_free (cur_path);
    }

  g_strfreev (pathv);

  /* Новый параметр. */
  key = g_new (HyScanDataSchemaKey, 1);
  key->id = g_strdup (id);
  key->name = g_strdup (name);
  key->description = g_strdup (description);
  key->type = type;
  key->readonly = readonly;

  node->keys = g_realloc (node->keys, (node->n_keys + 2) * sizeof (HyScanDataSchemaInternalKey*));
  node->keys[node->n_keys] = key;
  node->n_keys += 1;
  node->keys[node->n_keys] = NULL;
}

/* Функция освобождает память занятую структурой с параметром. */
void
hyscan_data_schema_internal_key_free (HyScanDataSchemaInternalKey *key)
{
  g_free (key->id);
  g_free (key->name);
  g_free (key->description);

  g_clear_pointer (&key->default_value, g_variant_unref);
  g_clear_pointer (&key->minimum_value, g_variant_unref);
  g_clear_pointer (&key->maximum_value, g_variant_unref);
  g_clear_pointer (&key->value_step, g_variant_unref);

  g_free (key);
}

/* Функция освобождает память занятую структурой со значениями типа enum. */
void
hyscan_data_schema_internal_enum_free (HyScanDataSchemaEnum *values)
{
  gint i;

  for (i = 0; i < values->n_values; i++)
    {
      g_free (values->values[i]->name);
      g_free (values->values[i]->description);
      g_free (values->values[i]);
    }

  g_free (values->values);
  g_free (values->id);
  g_free (values);
}

/* Функция проверяет значение перечисляемого типа на допустимость. */
gboolean
hyscan_data_schema_internal_enum_check (HyScanDataSchemaEnum *enums,
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
