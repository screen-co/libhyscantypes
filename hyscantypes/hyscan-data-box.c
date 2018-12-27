/* hyscan-data-box.c
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

/**
 * SECTION: hyscan-data-box
 * @Short_description: класс параметров в памяти
 * @Title: HyScanDataBox
 *
 * Класс предоставляет набор функций для работы с параметрами, определённых
 * схемой #HyScanDataSchema и размещаемых в оперативной памяти. Имеется
 * возможность зарегистрировать обработчики, вызываемые при изменении значений
 * параметров, а также отслеживать наличие изменений.
 *
 * Создание объекта класса осуществляется функциями #hyscan_data_box_new_from_string,
 * #hyscan_data_box_new_from_file и #hyscan_data_box_new_from_resource.
 *
 * Класс реализует интерфейс #HyScanParam для работы с параметрами.
 *
 * При изменении значения любого параметра увеличивается счётчик, связанный
 * с этим параметром, а также глобальный счётчик. Получить текущее значение
 * счётчика можно функцией #hyscan_data_box_get_mod_count. Пользователь может
 * узнать об изменениях в значениях параметров, используя значения этих счётчиков.
 *
 * Класс предоставляет возможность сохранить текущее значение всех параметров в виде
 * строки и восстановить эти значения в дальнейшем. Для этих целей используются функции
 * #hyscan_data_box_serialize и #hyscan_data_box_deserialize.
 */

#include "hyscan-data-box.h"
#include "hyscan-types-marshallers.h"
#include <gio/gio.h>
#include <string.h>

enum
{
  PROP_O,
  PROP_SCHEMA
};

enum
{
  SIGNAL_SET,
  SIGNAL_CHANGED,
  SIGNAL_LAST
};

/* Значение параметра. */
typedef struct
{
  GQuark                       id;                     /* Идентификатор названия параметра. */
  HyScanDataSchemaKeyType      type;                   /* Тип параметра. */
  GVariant                    *value;                  /* Значение параметра. */
  GVariantClass                value_type;             /* Тип значения параметра. */
  guint32                      mod_count;              /* Счётчик изменений значений параметра. */
  HyScanDataSchemaKeyAccess    access;                 /* Атрибут доступа к параметру. */
} HyScanDataBoxParam;

struct _HyScanDataBoxPrivate
{
  HyScanDataSchema            *schema;                 /* Схема параметров. */

  const gchar * const         *keys_list;              /* Список параметров. */
  GHashTable                  *params;                 /* Значения параметров. */

  guint32                      mod_count;              /* Глобальный счётчик изменений значений параметра. */

  GMutex                       lock;                   /* Блокировка. */
};

static void    hyscan_data_box_interface_init          (HyScanParamInterface  *iface);
static void    hyscan_data_box_set_property            (GObject               *object,
                                                        guint                  prop_id,
                                                        const GValue          *value,
                                                        GParamSpec            *pspec);
static void    hyscan_data_box_object_constructed      (GObject               *object);
static void    hyscan_data_box_object_finalize         (GObject               *object);

static void    hyscan_data_box_param_free              (gpointer               param);

static gboolean hyscan_data_box_signal_accumulator     (GSignalInvocationHint *ihint,
                                                        GValue                *return_accu,
                                                        const GValue          *handler_return,
                                                        gpointer               data);

static guint   hyscan_data_box_signals[SIGNAL_LAST] = { 0 };

G_DEFINE_TYPE_WITH_CODE (HyScanDataBox, hyscan_data_box, G_TYPE_OBJECT,
                         G_ADD_PRIVATE (HyScanDataBox)
                         G_IMPLEMENT_INTERFACE (HYSCAN_TYPE_PARAM, hyscan_data_box_interface_init));


static void hyscan_data_box_class_init( HyScanDataBoxClass *klass )
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = hyscan_data_box_set_property;

  object_class->constructed = hyscan_data_box_object_constructed;
  object_class->finalize = hyscan_data_box_object_finalize;

  g_object_class_install_property (object_class, PROP_SCHEMA,
    g_param_spec_object ("schema", "Schema", "HyScanDataSchema object", HYSCAN_TYPE_DATA_SCHEMA,
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

  /**
   * HyScanDataBox::set:
   * @data_box: указатель на #HyScanDataBox
   * @param_list: (type HyScanParamList): список параметров и их значения
   *
   * Сигнал посылается перед изменением параметров. В нём передаются названия
   * изменяемых параметров и их новые значения. Пользователь может обработать
   * этот сигнал и проверить валидность новых значений. Если обработчик сигнала
   * вернёт значение %FALSE, новые значения не будут установлены.
   *
   * Returns: %TRUE для установки значений, иначе %FALSE.
   */
  hyscan_data_box_signals[SIGNAL_SET] =
    g_signal_new ("set", HYSCAN_TYPE_DATA_BOX, G_SIGNAL_RUN_LAST, 0,
                  hyscan_data_box_signal_accumulator, NULL,
                  hyscan_types_marshal_BOOLEAN__OBJECT,
                  G_TYPE_BOOLEAN, 1, G_TYPE_OBJECT);

  /**
   * HyScanDataBox::changed:
   * @data_box: указатель на #HyScanDataBox
   * @name: название параметра
   *
   * Сигнал посылается при изменении параметров. Сигнал поддерживает возможность
   * детализации. При подключении к сигналу вида "changed::name", он будет
   * посылаться только для параметра name.
   */
  hyscan_data_box_signals[SIGNAL_CHANGED] =
    g_signal_new ("changed", HYSCAN_TYPE_DATA_BOX, G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED, 0,
                  NULL, NULL,
                  g_cclosure_marshal_VOID__STRING, G_TYPE_NONE, 1, G_TYPE_STRING);
}

static void
hyscan_data_box_init (HyScanDataBox *data_box)
{
  data_box->priv = hyscan_data_box_get_instance_private (data_box);
}

static void
hyscan_data_box_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  HyScanDataBox *data_box = HYSCAN_DATA_BOX (object);
  HyScanDataBoxPrivate *priv = data_box->priv;

  switch (prop_id)
    {
    case PROP_SCHEMA:
      priv->schema = g_value_dup_object (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hyscan_data_box_object_constructed (GObject *object)
{
  HyScanDataBox *data_box = HYSCAN_DATA_BOX (object);
  HyScanDataBoxPrivate *priv = data_box->priv;

  gint i;

  G_OBJECT_CLASS (hyscan_data_box_parent_class)->constructed (object);

  g_mutex_init (&priv->lock);

  /* Схема параметров должна быть определена. */
  if (priv->schema == NULL)
    return;

  /* Таблица со значениями параметров. */
  priv->params = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, hyscan_data_box_param_free);

  /* Список параметров. */
  priv->keys_list = hyscan_data_schema_list_keys (priv->schema);
  if (priv->keys_list == NULL)
    return;

  for (i = 0; priv->keys_list[i] != NULL; i++)
    {
      HyScanDataBoxParam *param = g_slice_new0 (HyScanDataBoxParam);

      param->type = hyscan_data_schema_key_get_value_type (priv->schema, priv->keys_list[i]);
      param->id = g_quark_from_string (priv->keys_list[i]);
      param->access = hyscan_data_schema_key_get_access (priv->schema, priv->keys_list[i]);

      switch (param->type)
        {
        case HYSCAN_DATA_SCHEMA_KEY_BOOLEAN:
          param->value_type = G_VARIANT_CLASS_BOOLEAN;
          break;

        case HYSCAN_DATA_SCHEMA_KEY_INTEGER:
          param->value_type = G_VARIANT_CLASS_INT64;
          break;

        case HYSCAN_DATA_SCHEMA_KEY_DOUBLE:
          param->value_type = G_VARIANT_CLASS_DOUBLE;
          break;

        case HYSCAN_DATA_SCHEMA_KEY_STRING:
          param->value_type = G_VARIANT_CLASS_STRING;
          break;

        case HYSCAN_DATA_SCHEMA_KEY_ENUM:
          param->value_type = G_VARIANT_CLASS_INT64;
          break;

        default:
          param->value_type = 0;
          break;
        }

      g_hash_table_insert (priv->params, (gpointer)priv->keys_list[i], param);
    }
}

static void
hyscan_data_box_object_finalize (GObject *object)
{
  HyScanDataBox *data_box = HYSCAN_DATA_BOX (object);
  HyScanDataBoxPrivate *priv = data_box->priv;

  g_mutex_clear (&priv->lock);
  g_clear_pointer (&priv->params, g_hash_table_unref);
  g_clear_object (&priv->schema);

  G_OBJECT_CLASS (hyscan_data_box_parent_class)->finalize (object);
}

/* Функция освобождает память занятую структурой с параметром. */
static void
hyscan_data_box_param_free (gpointer data)
{
  HyScanDataBoxParam *param = data;

  g_clear_pointer (&param->value, g_variant_unref);
  g_slice_free (HyScanDataBoxParam, param);
}

/* Функция аккумулирует ответы всех callback'ов сигнала set.
 * Здесь действует обратная логика, если какой-либо из callback'ов
 * вернёт FALSE, аккумулятор вернёт TRUE. Это будет сигналом
 * прекратить обработку запроса установки параметров. */
static gboolean
hyscan_data_box_signal_accumulator (GSignalInvocationHint *ihint,
                                    GValue                *return_accu,
                                    const GValue          *handler_return,
                                    gpointer               data)
{
  if (!g_value_get_boolean (handler_return))
    {
      g_value_set_boolean (return_accu, TRUE);
      return FALSE;
    }

  return TRUE;
}

/**
 * hyscan_data_box_new:
 * @schema: схема данныx
 *
 * Функция создаёт новый объект #HyScanDataBox.
 *
 * Returns: #HyScanDataBox. Для удаления #g_object_unref.
 */
HyScanDataBox *
hyscan_data_box_new (HyScanDataSchema *schema)
{
  return g_object_new (HYSCAN_TYPE_DATA_BOX,
                       "schema", schema,
                       NULL);
}

/**
 * hyscan_data_box_new_from_string:
 * @schema_data: строка с описанием схемы в формате XML
 * @schema_id: идентификатор загружаемой схемы
 *
 * Функция создаёт новый объект #HyScanDataBox из описания схемы в виде
 * строки с XML данными.
 *
 * Returns: #HyScanDataBox. Для удаления #g_object_unref.
 */
HyScanDataBox *
hyscan_data_box_new_from_string (const gchar  *schema_data,
                                 const gchar  *schema_id)
{
  HyScanDataBox *data_box;
  HyScanDataSchema *schema;

  schema = hyscan_data_schema_new_from_string (schema_data, schema_id);
  data_box = hyscan_data_box_new (schema);
  g_object_unref (schema);

  return data_box;
}

/**
 * hyscan_data_box_new_from_file:
 * @schema_path: путь к XML файлу с описанием схемы
 * @schema_id: идентификатор загружаемой схемы
 *
 * Функция создаёт новый объект #HyScanDataBox из описания схемы в XML файле.
 *
 * Returns: #HyScanDataBox. Для удаления #g_object_unref.
 */
HyScanDataBox *
hyscan_data_box_new_from_file (const gchar *schema_path,
                               const gchar *schema_id)
{
  HyScanDataBox *data_box;
  HyScanDataSchema *schema;

  schema = hyscan_data_schema_new_from_file (schema_path, schema_id);
  data_box = hyscan_data_box_new (schema);
  g_object_unref (schema);

  return data_box;
}

/**
 * hyscan_data_box_new_from_resource:
 * @schema_resource: путь к ресурсу GResource
 * @schema_id: идентификатор загружаемой схемы
 *
 * Функция создаёт новый объект #HyScanDataBox из описания схемы в виде
 * строки с XML данными загружаемой из ресурсов.
 *
 * Returns: #HyScanDataBox. Для удаления #g_object_unref.
 */
HyScanDataBox *
hyscan_data_box_new_from_resource (const gchar *schema_resource,
                                   const gchar *schema_id)
{
  HyScanDataBox *data_box;
  HyScanDataSchema *schema;

  schema = hyscan_data_schema_new_from_resource (schema_resource, schema_id);
  data_box = hyscan_data_box_new (schema);
  g_object_unref (schema);

  return data_box;
}

/**
 * hyscan_data_box_get_mod_count:
 * @data_box: указатель на #HyScanDataBox
 * @name: (nullable): название параметра
 *
 * Функция возвращает значение счётчика изменений параметра. Если имя параметра
 * равно NULL, возвращается значение глобального счётчика изменений для всех
 * параметров.
 *
 * Returns: Значение счётчика изменений.
 */
guint32
hyscan_data_box_get_mod_count (HyScanDataBox *data_box,
                               const gchar   *name)
{
  HyScanDataBoxParam *param;

  g_return_val_if_fail (HYSCAN_IS_DATA_BOX (data_box), 0);

  if (name == NULL)
    return data_box->priv->mod_count;

  param = g_hash_table_lookup (data_box->priv->params, name);
  if (param != NULL)
    return param->mod_count;

  return 0;
}

/**
 * hyscan_data_box_serialize:
 * @data_box: указатель на #HyScanDataBox
 *
 * Функция сериализует внутреннее состояние объекта в строку.
 *
 * Returns: (transfer full): Сериализованные данные или NULL.
 */
gchar *
hyscan_data_box_serialize (HyScanDataBox *data_box)
{
  HyScanDataBoxPrivate *priv;
  GVariantDict *dict;
  GVariant *vdict;

  gboolean has_params = FALSE;
  gchar *sparams = NULL;
  gsize i;

  g_return_val_if_fail (HYSCAN_IS_DATA_BOX (data_box), NULL);

  priv = data_box->priv;

  if (priv->keys_list == NULL)
    return NULL;

  g_mutex_lock (&priv->lock);

  dict = g_variant_dict_new (NULL);

  /* Проверяем все параметры на предмет установки значения
   * отличного от значения по умолчанию. */
  for (i = 0; priv->keys_list[i] != NULL; i++)
    {
      HyScanDataBoxParam *param;
      GVariant *value;

      param = g_hash_table_lookup (priv->params, priv->keys_list[i]);
      if ((param == NULL) || (param->access == HYSCAN_DATA_SCHEMA_ACCESS_READONLY))
        continue;

      value = param->value;
      if (value == NULL)
        continue;

      /* Добавляем изменённые значения в массив. */
      g_variant_dict_insert_value (dict, priv->keys_list[i], value);
      has_params = TRUE;
    }

  if (has_params)
    {
      vdict = g_variant_dict_end (dict);
      sparams = g_variant_print (vdict, TRUE);
      g_variant_unref (vdict);
    }

  g_variant_dict_unref (dict);

  g_mutex_unlock (&priv->lock);

  return sparams;
}

/**
 * hyscan_data_box_deserialize:
 * @data_box: указатель на #HyScanDataBox
 * @svalues: сериализованные данные
 *
 * Функция де-сериализует состояние объекта.
 *
 * Returns: %TRUE если де-сериализация прошла успешно, иначе %FALSE.
 */
gboolean
hyscan_data_box_deserialize (HyScanDataBox *data_box,
                             const gchar   *svalues)
{
  HyScanParamList *list;
  GVariant *vdict;
  gboolean status;
  gsize n_params;
  gsize i;

  g_return_val_if_fail (HYSCAN_IS_DATA_BOX (data_box), FALSE);

  /* Загружаем значения из строки. */
  vdict = g_variant_parse (NULL, svalues, NULL, NULL, NULL);
  if (vdict == NULL)
    return FALSE;

  /* Список параметров. */
  list = hyscan_param_list_new ();

  /* Массивы имён параметров и их значения. */
  n_params = g_variant_n_children (vdict);
  for (i = 0; i < n_params; i++)
    {
      GVariant *param;
      GVariant *value;
      gchar *name;

      param = g_variant_get_child_value (vdict, i);

      g_variant_get (param, "{sv}", &name, &value);
      hyscan_param_list_set (list, name, value);
      g_variant_unref (value);
      g_free (name);

      g_variant_unref (param);
    }

  g_variant_unref (vdict);

  /* Устанавливаем новые значения. */
  status = hyscan_param_set (HYSCAN_PARAM (data_box), list);

  g_object_unref (list);

  return status;
}

/* Функция возвращает схему параметров. */
static HyScanDataSchema *
hyscan_data_box_schema (HyScanParam *param)
{
  HyScanDataBox *data_box = HYSCAN_DATA_BOX (param);
  HyScanDataBoxPrivate *priv = data_box->priv;

  return g_object_ref (priv->schema);
}

/* Функция устанавливает значения параметров. */
static gboolean
hyscan_data_box_set (HyScanParam     *param,
                     HyScanParamList *list)
{
  HyScanDataBox *data_box = HYSCAN_DATA_BOX (param);
  HyScanDataBoxPrivate *priv = data_box->priv;
  HyScanDataSchema *schema = priv->schema;
  gboolean status = FALSE;
  gboolean cancel = FALSE;

  const gchar * const *names;
  GVariant **values = NULL;
  guint n_names;
  guint i;

  /* Список устанавливаемых параметров. */
  names = hyscan_param_list_params (list);
  if (names == NULL)
    return FALSE;

  /* Список значений параметров. */
  n_names = g_strv_length ((gchar **)names);
  values = g_new0 (GVariant *, n_names);

  /* Проверяем параметы. */
  for (i = 0; i < n_names; i++)
    {
      HyScanDataBoxParam *param;

      /* Описание параметра. */
      param = g_hash_table_lookup (priv->params, names[i]);
      if (param == NULL)
        goto exit;

      /* Параметр только для чтения. */
      if (param->access == HYSCAN_DATA_SCHEMA_ACCESS_READONLY)
        goto exit;

      /* Новое значение параметра. */
      values[i]= hyscan_param_list_get (list, names[i]);

      /* Установка значения по умолчанию. */
      if (values[i] == NULL)
        {
          values[i] = hyscan_data_schema_key_get_default (schema, names[i]);
          continue;
        }

      /* Ошибка в типе нового значения параметра. */
      if (param->value_type != g_variant_classify (values[i]))
        goto exit;

      /* Недопустимое значение параметра. */
      if (!hyscan_data_schema_key_check (schema, names[i], values[i]))
        goto exit;
    }

  /* Сигнал перед изменением параметров. */
  g_signal_emit (data_box, hyscan_data_box_signals[SIGNAL_SET], 0, list, &cancel);

  if (cancel)
    goto exit;

  g_mutex_lock (&priv->lock);

  /* Изменяем параметры. */
  for (i = 0; i < n_names; i++)
    {
      HyScanDataBoxParam *param;

      param = g_hash_table_lookup (priv->params, names[i]);

      /* Устанавливаем новое значение. */
      g_clear_pointer (&param->value, g_variant_unref);
      param->value = values[i];
      param->mod_count += 1;
    }

  priv->mod_count += 1;
  status = TRUE;

  g_mutex_unlock (&priv->lock);

  /* Сигнал об изменении параметров. */
  for (i = 0; i < n_names; i++)
    {
      HyScanDataBoxParam *param;

      param = g_hash_table_lookup (priv->params, names[i]);

      g_signal_emit (data_box, hyscan_data_box_signals[SIGNAL_CHANGED], param->id, names[i]);
    }

exit:
  if (!status)
    for (i = 0; i < n_names; i++)
      g_clear_pointer (&values[i], g_variant_unref);

  g_free (values);
  return status;
}

/* Функция считывает значения параметров. */
static gboolean
hyscan_data_box_get (HyScanParam     *param,
                     HyScanParamList *list)
{
  HyScanDataBox *data_box = HYSCAN_DATA_BOX (param);
  HyScanDataBoxPrivate *priv = data_box->priv;
  HyScanDataSchema *schema = priv->schema;

  const gchar * const *names;
  guint n_names;
  guint i;

  /* Список считываемых параметров. */
  names = hyscan_param_list_params (list);
  if (names == NULL)
    return FALSE;

  /* Проверяем параметы. */
  n_names = g_strv_length ((gchar **)names);
  for (i = 0; i < n_names; i++)
    {
      HyScanDataBoxParam *param;

      param = g_hash_table_lookup (priv->params, names[i]);
      if (param == NULL)
        return FALSE;

      /* Параметр только для записи. */
      if (param->access == HYSCAN_DATA_SCHEMA_ACCESS_WRITEONLY)
        return FALSE;
    }

  g_mutex_lock (&priv->lock);

  /* Считываем параметы. */
  for (i = 0; i < n_names; i++)
    {
      HyScanDataBoxParam *param;
      GVariant *value;

      /* Описание параметра. */
      param = g_hash_table_lookup (priv->params, names[i]);

      /* Считываем значение параметра. */
      if (param->value != NULL)
        value = g_variant_ref (param->value);
      else
        value = hyscan_data_schema_key_get_default (schema, names[i]);

      /* Значение параметра. */
      hyscan_param_list_set (list, names[i], value);

      g_clear_pointer (&value, g_variant_unref);
    }

  g_mutex_unlock (&priv->lock);

  return TRUE;
}

static void
hyscan_data_box_interface_init (HyScanParamInterface *iface)
{
  iface->schema = hyscan_data_box_schema;
  iface->set = hyscan_data_box_set;
  iface->get = hyscan_data_box_get;
}
