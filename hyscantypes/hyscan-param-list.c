/**
 * \file hyscan-param-list.c
 *
 * \brief Исходный файл класса для чтения/записи значений параметров.
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2017
 * \license Проприетарная лицензия ООО "Экран"
 *
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

HyScanParamList *
hyscan_param_list_new (void)
{
  return g_object_new (HYSCAN_TYPE_PARAM_LIST, NULL);
}

const gchar * const *
hyscan_param_list_params (HyScanParamList *list)
{
  g_return_val_if_fail (HYSCAN_IS_PARAM_LIST (list), NULL);

  return (const gchar **) list->priv->names;
}

gboolean
hyscan_param_list_contains (HyScanParamList *list,
                            const gchar     *name)
{
  g_return_val_if_fail (HYSCAN_IS_PARAM_LIST (list), FALSE);

  return g_hash_table_contains (list->priv->params, name);
}

void
hyscan_param_list_clear (HyScanParamList *list)
{
  g_return_if_fail (HYSCAN_IS_PARAM_LIST (list));

  g_hash_table_remove_all (list->priv->params);

  hyscan_param_list_update_names (list->priv);
}

void
hyscan_param_list_add (HyScanParamList *list,
                       const gchar     *name)
{
  g_return_if_fail (HYSCAN_IS_PARAM_LIST (list));

  if (g_hash_table_insert (list->priv->params, g_strdup (name), NULL))
    hyscan_param_list_update_names (list->priv);
}

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

void
hyscan_param_list_set_boolean (HyScanParamList *list,
                               const gchar     *name,
                               gboolean         value)
{
  hyscan_param_list_set (list, name, g_variant_new_boolean (value));
}

void
hyscan_param_list_set_integer (HyScanParamList *list,
                               const gchar     *name,
                               gint64           value)
{
  hyscan_param_list_set (list, name, g_variant_new_int64 (value));
}

void
hyscan_param_list_set_double (HyScanParamList *list,
                              const gchar     *name,
                              gdouble          value)
{
  hyscan_param_list_set (list, name, g_variant_new_double (value));
}

void
hyscan_param_list_set_string (HyScanParamList *list,
                              const gchar     *name,
                              const gchar     *value)
{
  hyscan_param_list_set (list, name, g_variant_new_string (value));
}

void
hyscan_param_list_set_enum (HyScanParamList *list,
                            const gchar     *name,
                            gint64           value)
{
  hyscan_param_list_set_integer (list, name, value);
}

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

gint64
hyscan_param_list_get_enum (HyScanParamList *list,
                            const gchar     *name)
{
  return hyscan_param_list_get_integer (list, name);
}

