/* hyscan-param-controller.c
 *
 * Copyright 2018 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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
 * SECTION: hyscan-param-controller
 * @Short_description: класс параметров с пользовательским управлением
 * @Title: HyScanParamController
 *
 * Класс предоставляет реализацию интерфейса #HyScanParam для параметров,
 * определённых схемой #HyScanDataSchema. Для чтения/записи параметра
 * пользователь должен указать специальные функции #hyscan_param_controller_set
 * и #hyscan_param_controller_get. Кроме этого имеется возможность связать
 * параметр с одиночной переменной или объектом GString для строковых
 * параметров.
 */

#include "hyscan-param-controller.h"

enum
{
  PROP_O,
  PROP_LOCK
};

/* Стуктура с описанием действия над параметром. */
typedef struct
{
  hyscan_param_controller_set  set_fn;         /* Функция установки значения параметра. */
  hyscan_param_controller_get  get_fn;         /* Функция чтния значения параметра. */
  gpointer                     user_data;      /* Пользовательские данные. */
} HyScanParamControllerExec;

struct _HyScanParamControllerPrivate
{
  HyScanDataSchema            *schema;         /* Схема данных. */
  GHashTable                  *params;         /* Список обрабатываемых параметров. */
  GMutex                      *lock;           /* Блокировка доступа к параметрам. */
};

static void    hyscan_param_controller_interface_init        (HyScanParamInterface *iface);
static void    hyscan_param_controller_set_property          (GObject                *object,
                                                              guint                   prop_id,
                                                              const GValue           *value,
                                                              GParamSpec             *pspec);
static void    hyscan_param_controller_object_constructed    (GObject                *object);
static void    hyscan_param_controller_object_finalize       (GObject                *object);

static void    hyscan_param_controller_free_exec             (gpointer                data);

G_DEFINE_TYPE_WITH_CODE (HyScanParamController, hyscan_param_controller, G_TYPE_OBJECT,
                         G_ADD_PRIVATE (HyScanParamController)
                         G_IMPLEMENT_INTERFACE (HYSCAN_TYPE_PARAM, hyscan_param_controller_interface_init))

static void
hyscan_param_controller_class_init (HyScanParamControllerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = hyscan_param_controller_set_property;

  object_class->constructed = hyscan_param_controller_object_constructed;
  object_class->finalize = hyscan_param_controller_object_finalize;

  g_object_class_install_property (object_class, PROP_LOCK,
    g_param_spec_pointer ("lock", "Lock", "Lock",
                          G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

static void
hyscan_param_controller_init (HyScanParamController *controller)
{
  controller->priv = hyscan_param_controller_get_instance_private (controller);
}

/* Функции установки/чтения параметра типа BOOLEAN. */
static gboolean
hyscan_param_controller_set_boolean (const gchar *name,
                                     GVariant    *value,
                                     gpointer     data)
{
  if (g_variant_classify (value) != G_VARIANT_CLASS_BOOLEAN)
    return FALSE;

  *(gboolean*)data = g_variant_get_boolean (value);

  return TRUE;
}

static GVariant *
hyscan_param_controller_get_boolean (const gchar *name,
                                     gpointer     data)
{
  return g_variant_new_boolean (*(gboolean*)data);
}

/* Функции установки/чтения параметра типа INTEGER. */
static gboolean
hyscan_param_controller_set_integer (const gchar *name,
                                     GVariant    *value,
                                     gpointer     data)
{
  if (g_variant_classify (value) != G_VARIANT_CLASS_INT64)
    return FALSE;

  *(gint64*)data = g_variant_get_int64 (value);

  return TRUE;
}

static GVariant *
hyscan_param_controller_get_integer (const gchar *name,
                                     gpointer     data)
{
  return g_variant_new_int64 (*(gint64*)data);
}

/* Функции установки/чтения параметра типа DOUBLE. */
static gboolean
hyscan_param_controller_set_double (const gchar *name,
                                    GVariant    *value,
                                    gpointer     data)
{
  if (g_variant_classify (value) != G_VARIANT_CLASS_DOUBLE)
    return FALSE;

  *(gdouble*)data = g_variant_get_double (value);

  return TRUE;
}

static GVariant *
hyscan_param_controller_get_double (const gchar *name,
                                    gpointer     data)
{
  return g_variant_new_double (*(gdouble*)data);
}

/* Функции установки/чтения параметра типа STRING. */
static gboolean
hyscan_param_controller_set_string (const gchar *name,
                                    GVariant    *value,
                                    gpointer     data)
{
  if (value != NULL && g_variant_classify (value) != G_VARIANT_CLASS_STRING)
    return FALSE;

  if (value != NULL)
    g_string_assign (data, g_variant_get_string (value, NULL));
  else
    g_string_assign (data, "");

  return TRUE;
}

static GVariant *
hyscan_param_controller_get_string (const gchar *name,
                                    gpointer     data)
{
  GString *string = data;

  if (string->len == 0)
    return NULL;

  return g_variant_new_string (string->str);
}

static void
hyscan_param_controller_set_property (GObject      *object,
                                      guint         prop_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
  HyScanParamController *controller = HYSCAN_PARAM_CONTROLLER (object);
  HyScanParamControllerPrivate *priv = controller->priv;

  switch (prop_id)
    {
    case PROP_LOCK:
      priv->lock = g_value_get_pointer (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hyscan_param_controller_object_constructed (GObject *object)
{
  HyScanParamController *controller = HYSCAN_PARAM_CONTROLLER (object);
  HyScanParamControllerPrivate *priv = controller->priv;

  priv->params = g_hash_table_new_full (g_str_hash, g_str_equal,
                                        g_free, hyscan_param_controller_free_exec);
}

static void
hyscan_param_controller_object_finalize (GObject *object)
{
  HyScanParamController *controller = HYSCAN_PARAM_CONTROLLER (object);
  HyScanParamControllerPrivate *priv = controller->priv;

  g_hash_table_unref (priv->params);
  g_clear_object (&priv->schema);

  G_OBJECT_CLASS (hyscan_param_controller_parent_class)->finalize (object);
}

/* Функция освобождает память занятую структурой HyScanParamControllerExec. */
static void
hyscan_param_controller_free_exec (gpointer data)
{
  g_slice_free (HyScanParamControllerExec, data);
}

/**
 * hyscan_param_controller_new:
 * @schema: схема параметров
 * @lock: блокировка доступа к параметрам
 *
 * Функция создаёт новый объект #HyScanParamController.
 *
 * Returns: #HyScanParamController. Для удаления #g_object_unref.
 */
HyScanParamController *
hyscan_param_controller_new (GMutex           *lock)
{
  return g_object_new (HYSCAN_TYPE_PARAM_CONTROLLER,
                       "lock", lock,
                       NULL);
}

/**
 * hyscan_param_controller_set_schema:
 * @controller: указатель на #HyScanParamController
 * @schema: схема параметров
 *
 * Функция устанавливает используемую схему параметров.
 */
void
hyscan_param_controller_set_schema (HyScanParamController *controller,
                                    HyScanDataSchema      *schema)
{
  HyScanParamControllerPrivate *priv;

  g_return_if_fail (HYSCAN_IS_PARAM_CONTROLLER (controller));
  g_return_if_fail (HYSCAN_IS_DATA_SCHEMA (schema));

  priv = controller->priv;

  (priv->lock != NULL) ? g_mutex_lock (priv->lock) : 0;

  g_clear_object (&priv->schema);
  priv->schema = g_object_ref (schema);

  (priv->lock != NULL) ? g_mutex_unlock (priv->lock) : 0;
}

/**
 * hyscan_param_controller_add_boolean:
 * @controller: указатель на #HyScanParamController
 * @name: название параметра
 * @value: указатель на переменную со значением параметра
 *
 * Функция связывает параметр типа #HYSCAN_DATA_SCHEMA_KEY_BOOLEAN с переменной,
 * в которой будет храниться значение параметра.
 *
 * Returns: %TRUE если связвание прошло успешно, иначе %FALSE.
 */
gboolean
hyscan_param_controller_add_boolean (HyScanParamController *controller,
                                     const gchar           *name,
                                     gboolean              *value)
{
  return hyscan_param_controller_add_user (controller, name,
                                           hyscan_param_controller_set_boolean,
                                           hyscan_param_controller_get_boolean,
                                           value);
}

/**
 * hyscan_param_controller_add_integer:
 * @controller: указатель на #HyScanParamController
 * @name: название параметра
 * @value: указатель на переменную со значением параметра
 *
 * Функция связывает параметр типа #HYSCAN_DATA_SCHEMA_KEY_INTEGER с переменной,
 * в которой будет храниться значение параметра.
 *
 * Returns: %TRUE если связвание прошло успешно, иначе %FALSE.
 */
gboolean
hyscan_param_controller_add_integer (HyScanParamController *controller,
                                     const gchar           *name,
                                     gint64                *value)
{
  return hyscan_param_controller_add_user (controller, name,
                                           hyscan_param_controller_set_integer,
                                           hyscan_param_controller_get_integer,
                                           value);
}

/**
 * hyscan_param_controller_add_double:
 * @controller: указатель на #HyScanParamController
 * @name: название параметра
 * @value: указатель на переменную со значением параметра
 *
 * Функция связывает параметр типа #HYSCAN_DATA_SCHEMA_KEY_DOUBLE с переменной,
 * в которой будет храниться значение параметра.
 *
 * Returns: %TRUE если связвание прошло успешно, иначе %FALSE.
 */
gboolean
hyscan_param_controller_add_double (HyScanParamController *controller,
                                    const gchar           *name,
                                    gdouble               *value)
{
  return hyscan_param_controller_add_user (controller, name,
                                           hyscan_param_controller_set_double,
                                           hyscan_param_controller_get_double,
                                           value);
}

/**
 * hyscan_param_controller_add_string:
 * @controller: указатель на #HyScanParamController
 * @name: название параметра
 * @value: указатель на переменную со значением параметра
 *
 * Функция связывает параметр типа #HYSCAN_DATA_SCHEMA_KEY_STRING с объектом
 * #GString, в котором будет храниться значение параметра.
 *
 * Returns: %TRUE если связвание прошло успешно, иначе %FALSE.
 */
gboolean
hyscan_param_controller_add_string (HyScanParamController *controller,
                                    const gchar           *name,
                                    GString               *value)
{
  return hyscan_param_controller_add_user (controller, name,
                                           hyscan_param_controller_set_string,
                                           hyscan_param_controller_get_string,
                                           value);
}

/**
 * hyscan_param_controller_add_enum:
 * @controller: указатель на #HyScanParamController
 * @name: название параметра
 * @value: указатель на переменную со значением параметра
 *
 * Функция связывает параметр типа #HYSCAN_DATA_SCHEMA_KEY_ENUM с переменной,
 * в которой будет храниться значение параметра.
 *
 * Returns: %TRUE если связвание прошло успешно, иначе %FALSE.
 */
gboolean
hyscan_param_controller_add_enum (HyScanParamController *controller,
                                  const gchar           *name,
                                  gint64                *value)
{
  return hyscan_param_controller_add_integer (controller, name, value);
}

/**
 * hyscan_param_controller_add_user:
 * @controller: указатель на #HyScanParamController
 * @name: название параметра
 * @set_fn: функция записи значения параметра
 * @get_fn: функция чтения значения параметра
 * @user_data: пользовательские данные
 *
 * Функция устанавливает пользовательские функции чтения/записи параметра.
 *
 * Returns: %TRUE если связвание прошло успешно, иначе %FALSE.
 */
gboolean
hyscan_param_controller_add_user (HyScanParamController       *controller,
                                  const gchar                 *name,
                                  hyscan_param_controller_set  set_fn,
                                  hyscan_param_controller_get  get_fn,
                                  gpointer                     user_data)
{
  HyScanParamControllerExec *exec;
  GHashTable *params;
  GMutex *lock;

  gboolean status = FALSE;

  g_return_val_if_fail (HYSCAN_IS_PARAM_CONTROLLER (controller), FALSE);

  params = controller->priv->params;
  lock = controller->priv->lock;

  (lock != NULL) ? g_mutex_lock (lock) : 0;

  /* Параметр уже добавлен. */
  if (g_hash_table_contains (params, name))
    goto exit;

  /* Добавляем параметр к обрабатываемым. */
  exec = g_slice_new0 (HyScanParamControllerExec);
  exec->set_fn = set_fn;
  exec->get_fn = get_fn;
  exec->user_data = user_data;

  g_hash_table_insert (params, g_strdup (name), exec);
  status = TRUE;

exit:
  (lock != NULL) ? g_mutex_unlock (lock) : 0;

  return status;
}

static HyScanDataSchema *
hyscan_param_controller_param_schema (HyScanParam *param)
{
  HyScanParamController *controller = HYSCAN_PARAM_CONTROLLER (param);
  HyScanDataSchema *schema = controller->priv->schema;

  if (schema == NULL)
    return NULL;

  return g_object_ref (schema);
}

static gboolean
hyscan_param_controller_param_set (HyScanParam     *param,
                                   HyScanParamList *list)
{
  HyScanParamController *controller = HYSCAN_PARAM_CONTROLLER (param);
  HyScanDataSchema *schema = controller->priv->schema;
  GHashTable *params = controller->priv->params;
  GMutex *lock = controller->priv->lock;
  gboolean status = TRUE;

  const gchar * const *names;
  guint i;

  if (schema == NULL)
    return FALSE;

  names = hyscan_param_list_params (list);
  if (names == NULL)
    return FALSE;

  (lock != NULL) ? g_mutex_lock (lock) : 0;

  /* Проверяем значения всех параметров. */
  for (i = 0; names[i] != NULL; i++)
    {
      GVariant *value = hyscan_param_list_get (list, names[i]);

      if (!hyscan_data_schema_key_check (schema, names[i], value) ||
          (hyscan_data_schema_key_get_access (schema, names[i]) == HYSCAN_DATA_SCHEMA_ACCESS_READONLY))
        {
          status = FALSE;
        }

      g_clear_pointer (&value, g_variant_unref);

      if (!status)
        goto fail;
    }

  /* Устанавливаем новые значения параметров. */
  for (i = 0; names[i] != NULL; i++)
    {
      HyScanParamControllerExec *exec;
      GVariant *value;

      status = FALSE;
      exec = g_hash_table_lookup (params, names[i]);
      if ((exec == NULL) || (exec->set_fn == NULL))
        goto fail;

      value = hyscan_param_list_get (list, names[i]);
      if (value == NULL)
        value = hyscan_data_schema_key_get_default (schema, names[i]);

      if ((hyscan_data_schema_key_get_value_type (schema, names[i]) == HYSCAN_DATA_SCHEMA_KEY_STRING) ||
          (value != NULL))
        {
          status = exec->set_fn (names[i], value, exec->user_data);
        }

      g_clear_pointer (&value, g_variant_unref);

      if (!status)
        goto fail;
    }

fail:
  (lock != NULL) ? g_mutex_unlock (lock) : 0;

  return status;
}

static gboolean
hyscan_param_controller_param_get (HyScanParam     *param,
                                   HyScanParamList *list)
{
  HyScanParamController *controller = HYSCAN_PARAM_CONTROLLER (param);
  HyScanDataSchema *schema = controller->priv->schema;
  GHashTable *params = controller->priv->params;
  GMutex *lock = controller->priv->lock;
  gboolean status = FALSE;

  const gchar * const *names;
  guint i;

  if (schema == NULL)
    return FALSE;

  names = hyscan_param_list_params (list);
  if (names == NULL)
    return FALSE;

  (lock != NULL) ? g_mutex_lock (lock) : 0;

  /* Проверяем доступ всех параметров. */
  for (i = 0; names[i] != NULL; i++)
    {
      if (hyscan_data_schema_key_get_access (schema, names[i]) == HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY)
        goto fail;
    }

  /* Считываем значения параметров. */
  for (i = 0; names[i] != NULL; i++)
    {
      HyScanParamControllerExec *exec;
      GVariant *value;

      exec = g_hash_table_lookup (params, names[i]);
      if (exec == NULL || exec->get_fn == NULL)
        {
          value = hyscan_data_schema_key_get_default (schema, names[i]);
          g_clear_pointer (&value, g_variant_unref);
        }
      else
        {
          value = exec->get_fn (names[i], exec->user_data);
        }

      hyscan_param_list_set (list, names[i], value);
    }

  status = TRUE;

fail:
  (lock != NULL) ? g_mutex_unlock (lock) : 0;

  return status;
}

static void
hyscan_param_controller_interface_init (HyScanParamInterface *iface)
{
  iface->schema = hyscan_param_controller_param_schema;
  iface->set = hyscan_param_controller_param_set;
  iface->get = hyscan_param_controller_param_get;
}
