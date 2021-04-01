/* hyscan-param-proxy.c
 *
 * Copyright 2019 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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
 * SECTION: hyscan-param-proxy
 * @Short_description: класс для объединения параметров
 * @Title: HyScanParamProxy
 *
 * Класс реализует интерфейс #HyScanParam и обеспечивает объединение и доступ
 * к параметрам находящимся в других классах, также реализующих интерфейс
 * #HyScanParam.
 *
 * Перед началом использования необходимо зарегистрировать обработчики
 * параметров с помощью функции #hyscan_param_proxy_add. После регистрации
 * всех обработчиков необходимо вызвать функцию #hyscan_param_proxy_bind.
 * После этого, добавлять новые обработчики будет нельзя.
 */


#include "hyscan-param-proxy.h"
#include "hyscan-data-schema-builder.h"
#include "hyscan-data-schema-internal.h"

#include <string.h>

enum
{
  PROP_O,
  PROP_SCHEMA_ID,
  PROP_SCHEMA_VERSION
};

typedef struct
{
  HyScanParam                 *backend;        /* Бэкенд для параметра. */
  GHashTable                  *keys;           /* Таблица соответствия параметров для бэкенда. */
} HyScanParamProxyBackend;

typedef struct
{
  HyScanParam                 *backend;        /* Бэкенд для параметра. */
  const gchar                 *key;            /* Оригинальное название параметра. */
} HyScanParamProxyKey;

struct _HyScanParamProxyPrivate
{
  gchar                       *schema_id;      /* Идентификатор общей схемы. */
  gint64                       schema_version; /* Версия общей схемы. */

  GList                       *backends;       /* Список бэкендов. */
  GHashTable                  *keys;           /* Список параметров. */
  HyScanParamList             *list;           /* Значения параметров. */

  HyScanDataSchemaBuilder     *builder;        /* Генератор схемы параметров. */
  HyScanDataSchema            *schema;         /* Схема параметров. */
};

static void    hyscan_param_proxy_interface_init       (HyScanParamInterface  *iface);
static void    hyscan_param_proxy_set_property         (GObject               *object,
                                                        guint                  prop_id,
                                                        const GValue          *value,
                                                        GParamSpec            *pspec);
static void    hyscan_param_proxy_object_constructed   (GObject               *object);
static void    hyscan_param_proxy_object_finalize      (GObject               *object);
static void    hyscan_param_proxy_backend_free         (gpointer               data);
static void    hyscan_param_proxy_key_free             (gpointer               data);

G_DEFINE_TYPE_WITH_CODE (HyScanParamProxy, hyscan_param_proxy, G_TYPE_OBJECT,
                         G_ADD_PRIVATE (HyScanParamProxy)
                         G_IMPLEMENT_INTERFACE (HYSCAN_TYPE_PARAM, hyscan_param_proxy_interface_init))

static void
hyscan_param_proxy_class_init (HyScanParamProxyClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = hyscan_param_proxy_set_property;

  object_class->constructed = hyscan_param_proxy_object_constructed;
  object_class->finalize = hyscan_param_proxy_object_finalize;

  g_object_class_install_property (object_class, PROP_SCHEMA_ID,
    g_param_spec_string ("schema-id", "ID", "Schema ID", NULL,
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_property (object_class, PROP_SCHEMA_VERSION,
    g_param_spec_int64 ("schema-version", "Version", "Schema version", G_MININT64, G_MAXINT64, 0,
                        G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

static void
hyscan_param_proxy_init (HyScanParamProxy *proxy)
{
  proxy->priv = hyscan_param_proxy_get_instance_private (proxy);
}

static void
hyscan_param_proxy_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  HyScanParamProxy *param_proxy = HYSCAN_PARAM_PROXY (object);
  HyScanParamProxyPrivate *priv = param_proxy->priv;

  switch (prop_id)
    {
    case PROP_SCHEMA_ID:
      priv->schema_id = g_value_dup_string (value);
      break;

    case PROP_SCHEMA_VERSION:
      priv->schema_version = g_value_get_int64 (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hyscan_param_proxy_object_constructed (GObject *object)
{
  HyScanParamProxy *proxy = HYSCAN_PARAM_PROXY (object);
  HyScanParamProxyPrivate *priv = proxy->priv;

  priv->keys = g_hash_table_new_full (g_str_hash, g_str_equal,
                                      g_free, hyscan_param_proxy_key_free);

  priv->list = hyscan_param_list_new ();

  if (priv->schema_id == NULL)
    priv->schema_id = g_strdup ("proxy");

  priv->builder = hyscan_data_schema_builder_new_full (priv->schema_id, priv->schema_version, NULL);
}

static void
hyscan_param_proxy_object_finalize (GObject *object)
{
  HyScanParamProxy *proxy = HYSCAN_PARAM_PROXY (object);
  HyScanParamProxyPrivate *priv = proxy->priv;

  g_list_free_full (priv->backends, hyscan_param_proxy_backend_free);
  g_hash_table_unref (priv->keys);

  g_clear_object (&priv->builder);
  g_clear_object (&priv->schema);
  g_clear_object (&priv->list);

  g_free (priv->schema_id);

  G_OBJECT_CLASS (hyscan_param_proxy_parent_class)->finalize (object);
}

/* Функция освобождает память занятую структурой HyScanParamProxyBackend. */
static void
hyscan_param_proxy_backend_free (gpointer data)
{
  HyScanParamProxyBackend *backend = data;

  g_object_unref (backend->backend);
  g_hash_table_unref (backend->keys);

  g_slice_free (HyScanParamProxyBackend, data);
}

/* Функция освобождает память занятую структурой HyScanParamProxyKey. */
static void
hyscan_param_proxy_key_free (gpointer data)
{
  g_slice_free (HyScanParamProxyKey, data);
}

/**
 * hyscan_param_proxy_new:
 *
 * Функция создаёт новый объект #HyScanParamProxy.
 *
 * Returns: #HyScanParamProxy. Для удаления #g_object_unref.
 */
HyScanParamProxy *
hyscan_param_proxy_new (void)
{
  return hyscan_param_proxy_new_full ("proxy", 0);
}

/**
 * hyscan_param_proxy_new:
 * @schema_id: идентификатор схемы объекта
 * @schema_version: версия схемы объекта
 *
 * Функция создаёт новый объект #HyScanParamProxy.
 *
 * Returns: #HyScanParamProxy. Для удаления #g_object_unref.
 */
HyScanParamProxy *
hyscan_param_proxy_new_full (const gchar *schema_id,
                             gint64       schema_version)
{
  return g_object_new (HYSCAN_TYPE_PARAM_PROXY,
                       "schema-id", schema_id,
                       "schema-version", schema_version,
                       NULL);
}

/**
 * hyscan_param_proxy_add:
 * @proxy: указатель на #HyScanParamProxy
 * @prefix: префикс пути для добавляемых параметров
 * @param: обработчик для добавляемых параметров
 * @root: путь к параметрам
 *
 * Функция регистрирует новые параметры и их обработчик.
 *
 * Returns: %TRUE если регистрация прошла успешно, иначе %FALSE.
 */
gboolean
hyscan_param_proxy_add (HyScanParamProxy *proxy,
                        const gchar      *prefix,
                        HyScanParam      *param,
                        const gchar      *root)
{
  HyScanParamProxyPrivate *priv;

  GList *backends;
  HyScanParamProxyBackend *backend;
  HyScanDataSchema *schema;
  const gchar * const *keys;

  gchar *prefix_path;
  gchar *root_path;
  gsize root_len;
  guint i;

  g_return_val_if_fail (HYSCAN_IS_PARAM_PROXY (proxy), FALSE);

  priv = proxy->priv;

  /* Добавление бэкендов завершено. */
  if (priv->builder == NULL)
    return FALSE;

  /* Определяем параметры которые необходимо проксировать. */
  schema = hyscan_param_schema (param);
  keys = hyscan_data_schema_list_keys (schema);
  if (keys == NULL)
    {
      g_object_unref (schema);
      return FALSE;
    }

  if (!hyscan_data_schema_builder_schema_join (priv->builder, prefix, schema, root))
    {
      g_clear_object (&priv->builder);
      return FALSE;
    }

  /* Нормализация путей. */
  prefix_path = hyscan_data_schema_internal_normalize_id (prefix);
  root_path = hyscan_data_schema_internal_normalize_id (root);
  root_len = strlen (root_path);

  backend = NULL;
  backends = priv->backends;
  while (backends != NULL)
    {
      backend = backends->data;
      if (backend->backend == param)
        break;

      backends = g_list_next (backends);
      backend = NULL;
    }

  if (backend == NULL)
    {
      backend = g_slice_new0 (HyScanParamProxyBackend);
      backend->backend = g_object_ref (param);
      backend->keys = g_hash_table_new (g_str_hash, g_str_equal);
      priv->backends = g_list_prepend (priv->backends, backend);
    }

  for (i = 0; keys[i] != NULL; i++)
    {
      HyScanParamProxyKey *proxy_key;
      gchar *proxy_key_path;

      if (!hyscan_data_schema_internal_is_path (keys[i], root_path))
        continue;

      proxy_key_path = hyscan_data_schema_internal_make_path (prefix_path, keys[i] + root_len);
      proxy_key = g_slice_new0 (HyScanParamProxyKey);
      proxy_key->backend = param;
      proxy_key->key = keys[i];

      g_hash_table_insert (priv->keys, proxy_key_path, proxy_key);
      g_hash_table_insert (backend->keys, (gchar*)keys[i], proxy_key_path);
    }

  g_object_unref (schema);

  g_free (prefix_path);
  g_free (root_path);

  return TRUE;
}

/**
 * hyscan_param_proxy_node_set_name:
 * @proxy: указатель на #HyScanParamProxy
 * @path: путь до узла
 * @name: название узла
 * @description: описание узла
 *
 * Функция устанавливает название и описание узла. Если узел не существует,
 * он будет создан.
 *
 * Returns: %TRUE - если название и описание узла установлено, иначе %FALSE.
 */
gboolean
hyscan_param_proxy_node_set_name (HyScanParamProxy *proxy,
                                  const gchar      *path,
                                  const gchar      *name,
                                  const gchar      *description)
{
  HyScanParamProxyPrivate *priv;

  g_return_val_if_fail (HYSCAN_IS_PARAM_PROXY (proxy), FALSE);

  priv = proxy->priv;

  if (priv->builder == NULL)
    return FALSE;

  return hyscan_data_schema_builder_node_set_name (priv->builder, path, name, description);
}

/**
 * hyscan_param_proxy_bind:
 * @proxy: указатель на #HyScanParamProxy
 *
 * Функция заершает конфигурирование и переводит объект в рабочее состояние.
 * После вызова этой функции регистрировать новые параметры будет нельзя.
 *
 * Returns: %TRUE если конфигурирование завершено успешно, иначе %FALSE.
 */
gboolean
hyscan_param_proxy_bind (HyScanParamProxy *proxy)
{
  HyScanParamProxyPrivate *priv;

  g_return_val_if_fail (HYSCAN_IS_PARAM_PROXY (proxy), FALSE);

  priv = proxy->priv;

  if (priv->builder == NULL)
    return FALSE;

  priv->schema = hyscan_data_schema_builder_get_schema (priv->builder);

  g_clear_object (&priv->builder);

  return TRUE;
}

static HyScanDataSchema *
hyscan_param_proxy_schema (HyScanParam *param)
{
  HyScanParamProxy *proxy = HYSCAN_PARAM_PROXY (param);
  HyScanParamProxyPrivate *priv = proxy->priv;

  if (priv->schema == NULL)
    return NULL;

  return g_object_ref (priv->schema);
}

static gboolean
hyscan_param_proxy_set (HyScanParam     *param,
                        HyScanParamList *list)
{
  HyScanParamProxy *proxy = HYSCAN_PARAM_PROXY (param);
  HyScanParamProxyPrivate *priv = proxy->priv;

  const gchar * const *keys;
  GList *backends;
  guint i;

  if (priv->schema == NULL)
    return FALSE;

  /* Список параметров. */
  keys = hyscan_param_list_params (list);
  if (keys == NULL)
    return FALSE;

  /* Проверяем что нам известны все параметры. */
  for (i = 0; keys[i] != NULL; i++)
    {
      if (!g_hash_table_contains (priv->keys, keys[i]))
        return FALSE;
    }

  /* Просматриваем все бэкенды. */
  backends = priv->backends;
  while (backends != NULL)
    {
      HyScanParamProxyBackend *backend = backends->data;
      gboolean is_params = FALSE;

      /* Ищем какие параметры относятся к этому бэкенду. */
      hyscan_param_list_clear (priv->list);
      for (i = 0; keys[i] != NULL; i++)
        {
          HyScanParamProxyKey *proxy_key = g_hash_table_lookup (priv->keys, keys[i]);

          if (backend->backend == proxy_key->backend)
            {
              GVariant *value = hyscan_param_list_get (list, keys[i]);
              hyscan_param_list_set (priv->list, proxy_key->key, value);
              g_clear_pointer (&value, g_variant_unref);
              is_params = TRUE;
            }
        }

      /* Передаём параметры в бэкенд. */
      if (is_params)
        {
          if (!hyscan_param_set (backend->backend, priv->list))
            return FALSE;
        }

      backends = g_list_next (backends);
    }

  return TRUE;
}

static gboolean
hyscan_param_proxy_get (HyScanParam     *param,
                        HyScanParamList *list)
{
  HyScanParamProxy *proxy = HYSCAN_PARAM_PROXY (param);
  HyScanParamProxyPrivate *priv = proxy->priv;

  const gchar * const *keys_out;
  const gchar * const *keys_in;
  GList *backends;
  guint i;

  if (priv->schema == NULL)
    return FALSE;

  /* Список параметров. */
  keys_in = hyscan_param_list_params (list);
  if (keys_in == NULL)
    return FALSE;

  /* Проверяем что нам известны все параметры. */
  for (i = 0; keys_in[i] != NULL; i++)
    {
      if (!g_hash_table_contains (priv->keys, keys_in[i]))
        return FALSE;
    }

  /* Просматриваем все бэкенды. */
  backends = priv->backends;
  while (backends != NULL)
    {
      HyScanParamProxyBackend *backend = backends->data;
      gboolean is_params = FALSE;

      /* Ищем какие параметры относятся к этому бэкенду. */
      hyscan_param_list_clear (priv->list);
      for (i = 0; keys_in[i] != NULL; i++)
        {
          HyScanParamProxyKey *proxy_key = g_hash_table_lookup (priv->keys, keys_in[i]);

          if (backend->backend == proxy_key->backend)
            {
              hyscan_param_list_add (priv->list, proxy_key->key);
              is_params = TRUE;
            }
        }

      /* Запрашиваем параметры из бэкенда. */
      if (is_params)
        {
          if (!hyscan_param_get (backend->backend, priv->list))
            return FALSE;

          keys_out = hyscan_param_list_params (priv->list);
          for (i = 0; keys_out[i] != NULL; i++)
            {
              const gchar *proxy_key_path = g_hash_table_lookup (backend->keys, keys_out[i]);
              GVariant *value = hyscan_param_list_get (priv->list, keys_out[i]);

              hyscan_param_list_set (list, proxy_key_path, value);
              g_clear_pointer (&value, g_variant_unref);
            }
        }

      backends = g_list_next (backends);
    }

  return TRUE;
}

static void
hyscan_param_proxy_interface_init (HyScanParamInterface *iface)
{
  iface->schema = hyscan_param_proxy_schema;
  iface->set = hyscan_param_proxy_set;
  iface->get = hyscan_param_proxy_get;
}
