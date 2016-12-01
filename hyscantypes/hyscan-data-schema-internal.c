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
#include <string.h>

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
hyscan_data_schema_internal_node_insert_key (HyScanDataSchemaNode     *node,
                                             const gchar              *id,
                                             const gchar              *name,
                                             const gchar              *description,
                                             HyScanDataSchemaKeyType   type,
                                             HyScanDataSchemaViewType  view,
                                             HyScanDataSchemaKeyAccess access)
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
          if (g_str_has_prefix (id, node->nodes[j]->path) &&
              (id[strlen (node->nodes[j]->path)] == '/'))
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
  key->view = view;
  key->access = access;

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

/* Функция добавляет схему или её часть в HyScanDataSchemaBuilder по указанному пути. */
gboolean
hyscan_data_schema_internal_builder_join_schema (HyScanDataSchemaBuilder *builder,
                                                 const gchar             *dst_root,
                                                 HyScanDataSchema        *schema,
                                                 const gchar             *src_root)
{
  gchar **keys;
  GHashTable *enums;
  gboolean status = FALSE;
  guint src_offset;
  guint i, j;

  /* Идентификаторов списков ENUM значений. */
  enums = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);

  /* Список всех параметров схемы. */
  keys = hyscan_data_schema_list_keys (schema);
  if (keys == NULL)
    goto exit;

  /* Смещение до имени параметра относительно исходного пути. */
  src_offset = strlen (src_root);

  for (i = 0; keys[i] != NULL; i++)
    {
      gchar *key_id;
      const gchar *name;
      const gchar *description;
      HyScanDataSchemaKeyType type;
      HyScanDataSchemaViewType view;
      HyScanDataSchemaKeyAccess access;
      GVariant *default_value;
      GVariant *minimum_value;
      GVariant *maximum_value;
      GVariant *value_step;

      /* Обрабатываем только параметры из нужной ветки. */
      if (!g_str_has_prefix (keys[i], src_root))
        continue;

      /* Свойства параметра. */
      key_id = g_strdup_printf ("%s%s", dst_root, keys[i] + src_offset);
      name = hyscan_data_schema_key_get_name (schema, keys[i]);
      description = hyscan_data_schema_key_get_description (schema, keys[i]);
      type = hyscan_data_schema_key_get_type (schema, keys[i]);
      view = hyscan_data_schema_key_get_view (schema, keys[i]);
      access = hyscan_data_schema_key_get_access (schema, keys[i]);
      default_value = hyscan_data_schema_key_get_default (schema, keys[i]);
      minimum_value = hyscan_data_schema_key_get_minimum (schema, keys[i]);
      maximum_value = hyscan_data_schema_key_get_maximum (schema, keys[i]);
      value_step = hyscan_data_schema_key_get_step (schema, keys[i]);

      switch (type)
        {
        case HYSCAN_DATA_SCHEMA_KEY_BOOLEAN:
          {
            status = hyscan_data_schema_builder_key_boolean_create (builder, key_id, name, description,
                                                                    g_variant_get_boolean (default_value));
          }
          break;

        case HYSCAN_DATA_SCHEMA_KEY_INTEGER:
          {
            status = hyscan_data_schema_builder_key_integer_create (builder, key_id, name, description,
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
            hyscan_data_schema_builder_key_double_create (builder, key_id, name, description,
                                                          g_variant_get_double (default_value));
            if (status)
              {
                status = hyscan_data_schema_builder_key_double_range (builder, key_id,
                                                                      g_variant_get_double (minimum_value),
                                                                      g_variant_get_double (maximum_value),
                                                                      g_variant_get_double (value_step));
              }
          }
          break;

        case HYSCAN_DATA_SCHEMA_KEY_STRING:
          {
            hyscan_data_schema_builder_key_string_create (builder, key_id, name, description,
                                                          (default_value == NULL) ? NULL :
                                                          g_variant_get_string (default_value, NULL));

          }
          break;

        case HYSCAN_DATA_SCHEMA_KEY_ENUM:
          {
            const gchar *enum_id;

            /* Создаём список вариантов возможных значений дя ENUM параметра,
             * если он еще не был создан. */
            enum_id = hyscan_data_schema_key_get_enum_id (schema, keys[i]);
            if (!g_hash_table_contains (enums, enum_id))
              {
                HyScanDataSchemaEnumValue **values;

                hyscan_data_schema_builder_enum_create (builder, enum_id);
                values = hyscan_data_schema_key_get_enum_values (schema, enum_id);
                for (j = 0; values != NULL && values[j] != NULL; j++)
                  {
                    status = hyscan_data_schema_builder_enum_value_create (builder, enum_id,
                                                                           values[j]->value,
                                                                           values[j]->name,
                                                                           values[j]->description);
                    if (!status)
                      break;
                  }
                hyscan_data_schema_free_enum_values (values);

                g_hash_table_insert (enums, g_strdup (enum_id), NULL);
              }

            if (status)
              {
                status = hyscan_data_schema_builder_key_enum_create (builder, key_id, name, description, enum_id,
                                                                     g_variant_get_int64 (default_value));
              }
          }
          break;

        default:
          break;
        }

      hyscan_data_schema_builder_key_set_view (builder, key_id, view);
      hyscan_data_schema_builder_key_set_access (builder, key_id, access);

      g_clear_pointer (&default_value, g_variant_unref);
      g_clear_pointer (&minimum_value, g_variant_unref);
      g_clear_pointer (&maximum_value, g_variant_unref);
      g_clear_pointer (&value_step, g_variant_unref);
      g_free (key_id);

      if (!status)
        break;
    }

exit:
  g_hash_table_unref (enums);
  g_strfreev (keys);

  return status;
}
