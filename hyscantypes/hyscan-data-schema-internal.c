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

/* Функция записывает в область памяти data значение типа gboolean. */
void
hyscan_data_schema_value_set_boolean (gpointer data,
                                      gboolean value)
{
  *(gboolean*)data = value;
}

/* Функция записывает в область памяти data значение типа gint64. */
void
hyscan_data_schema_value_set_integer (gpointer data,
                                      gint64   value)
{
  *(gint64*)data = value;
}

/* Функция записывает в область памяти data значение типа gdouble. */
void
hyscan_data_schema_value_set_double (gpointer data,
                                     gdouble  value)
{
  *(gdouble*)data = value;
}

/* Функция записывает в область памяти data указатель на копию строки. */
void
hyscan_data_schema_value_set_string (gpointer     data,
                                     const gchar *value)
{
  gpointer *p = data;
  g_free (*p);
  *p = g_strdup (value);
}

/* Функция считывает из области памяти data значение типа gboolean. */
gboolean
hyscan_data_schema_value_get_boolean (gconstpointer data)
{
  return *(gboolean*)data;
}

/* Функция считывает из области памяти data значение типа gint64. */
gint64
hyscan_data_schema_value_get_integer (gconstpointer data)
{
  return *(gint64*)data;
}

/* Функция считывает из области памяти data значение типа gdouble. */
gdouble
hyscan_data_schema_value_get_double (gconstpointer data)
{
  return *(gdouble*)data;
}

/* Функция считывает из области памяти data указатель на строку. */
const gchar *
hyscan_data_schema_value_get_string (gconstpointer data)
{
  gconstpointer const *p = data;
  return *p;
}

/* Функция проверяет имя на предмет допустимости. */
gboolean
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

/* Функция проверяет идентификатор на предмет допустимости. */
gboolean
hyscan_data_schema_validate_id (const gchar *id)
{
  gchar **pathv;
  guint n_pathv;
  guint i;

  pathv = g_strsplit (id, "/", -1);
  n_pathv = g_strv_length (pathv);
  if (n_pathv < 2)
    return FALSE;

  for (i = 1; i < n_pathv - 1; i++)
    if (!hyscan_data_schema_validate_name (pathv[i]))
      {
        g_strfreev (pathv);
        return FALSE;
      }

  g_strfreev (pathv);
  return TRUE;
}


/* Функция проверяет значение перечисляемого типа на допустимость. */
gboolean
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

/* Функция создаёт новый узел с параметрами. */
HyScanDataSchemaNode *
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
HyScanDataSchemaParam *
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
void
hyscan_data_schema_insert_param (HyScanDataSchemaNode *node,
                                 const gchar          *id,
                                 const gchar          *name,
                                 const gchar          *description,
                                 HyScanDataSchemaType  type,
                                 gboolean              readonly)
{
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
      node->nodes[node->n_nodes] = hyscan_data_schema_new_node (cur_path);
      node->n_nodes += 1;
      node->nodes[node->n_nodes] = NULL;
      node = node->nodes[j];
      g_free (cur_path);
    }

  g_strfreev (pathv);

  /* Новый параметр. */
  node->params = g_realloc (node->params, (node->n_params + 2) * sizeof (HyScanDataSchemaKey*));
  node->params[node->n_params] = hyscan_data_schema_new_param (id, name, description, type, readonly);
  node->n_params += 1;
  node->params[node->n_params] = NULL;
}

/* Функция освобождает память занятую структурой со значениями типа enum. */
void
hyscan_data_schema_free_enum (HyScanDataSchemaEnum *values)
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

/* Функция освобождает память занятую структурой с параметром. */
void
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
