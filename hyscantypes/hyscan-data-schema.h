/* hyscan-data-schema.h
 *
 * Copyright 2016-2019 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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

#ifndef __HYSCAN_DATA_SCHEMA_H__
#define __HYSCAN_DATA_SCHEMA_H__

#include <glib-object.h>
#include <hyscan-api.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_DATA_SCHEMA_ENUM_VALUE  (hyscan_data_schema_enum_value_get_type ())
#define HYSCAN_TYPE_DATA_SCHEMA_NODE        (hyscan_data_schema_node_get_type ())
#define HYSCAN_TYPE_DATA_SCHEMA_KEY         (hyscan_data_schema_key_get_type ())

#define HYSCAN_TYPE_DATA_SCHEMA             (hyscan_data_schema_get_type ())
#define HYSCAN_DATA_SCHEMA(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_DATA_SCHEMA, HyScanDataSchema))
#define HYSCAN_IS_DATA_SCHEMA(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_DATA_SCHEMA))
#define HYSCAN_DATA_SCHEMA_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HYSCAN_TYPE_DATA_SCHEMA, HyScanDataSchemaClass))
#define HYSCAN_IS_DATA_SCHEMA_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HYSCAN_TYPE_DATA_SCHEMA))
#define HYSCAN_DATA_SCHEMA_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HYSCAN_TYPE_DATA_SCHEMA, HyScanDataSchemaClass))

typedef struct _HyScanDataSchema HyScanDataSchema;
typedef struct _HyScanDataSchemaPrivate HyScanDataSchemaPrivate;
typedef struct _HyScanDataSchemaClass HyScanDataSchemaClass;
typedef struct _HyScanDataSchemaEnumValue HyScanDataSchemaEnumValue;
typedef struct _HyScanDataSchemaNode HyScanDataSchemaNode;
typedef struct _HyScanDataSchemaKey HyScanDataSchemaKey;

struct _HyScanDataSchema
{
  GObject parent_instance;

  HyScanDataSchemaPrivate *priv;
};

struct _HyScanDataSchemaClass
{
  GObjectClass parent_class;
};

/**
 * HyScanDataSchemaKeyType:
 * @HYSCAN_DATA_SCHEMA_KEY_INVALID: недопустимый тип, ошибка
 * @HYSCAN_DATA_SCHEMA_KEY_BOOLEAN: логический тип
 * @HYSCAN_DATA_SCHEMA_KEY_INTEGER: целые числа со знаком - gint64
 * @HYSCAN_DATA_SCHEMA_KEY_DOUBLE: числа с плавающей точкой - gdouble
 * @HYSCAN_DATA_SCHEMA_KEY_STRING: строка с нулём на конце
 * @HYSCAN_DATA_SCHEMA_KEY_ENUM: перечисляемый тип
 *
 * Тип параметра схемы данных.
 */
typedef enum
{
  HYSCAN_DATA_SCHEMA_KEY_INVALID,
  HYSCAN_DATA_SCHEMA_KEY_BOOLEAN,
  HYSCAN_DATA_SCHEMA_KEY_INTEGER,
  HYSCAN_DATA_SCHEMA_KEY_DOUBLE,
  HYSCAN_DATA_SCHEMA_KEY_STRING,
  HYSCAN_DATA_SCHEMA_KEY_ENUM
} HyScanDataSchemaKeyType;

/**
 * HyScanDataSchemaViewType:
 * @HYSCAN_DATA_SCHEMA_VIEW_DEFAULT: по умолчанию
 * @HYSCAN_DATA_SCHEMA_VIEW_BIN: двоичный вид
 * @HYSCAN_DATA_SCHEMA_VIEW_DEC: десятичный вид
 * @HYSCAN_DATA_SCHEMA_VIEW_HEX: шестнадцатиричный вид
 * @HYSCAN_DATA_SCHEMA_VIEW_DATE: дата, UTC unix time
 * @HYSCAN_DATA_SCHEMA_VIEW_TIME: время, UTC unix time
 * @HYSCAN_DATA_SCHEMA_VIEW_DATE_TIME: дата и время, UTC unix time
 * @HYSCAN_DATA_SCHEMA_VIEW_RGB: цвет без прозрачности
 * @HYSCAN_DATA_SCHEMA_VIEW_RGBA: цвет с прозрачностью
 * @HYSCAN_DATA_SCHEMA_VIEW_BUTTON: кнопка
 * @HYSCAN_DATA_SCHEMA_VIEW_FILE: путь к файлу
 * @HYSCAN_DATA_SCHEMA_VIEW_DIR: путь к каталогу
 * @HYSCAN_DATA_SCHEMA_VIEW_SCHEMA: схема данных
 *
 * Рекомендуемый вид отображения значения параметра.
 */
typedef enum
{
  HYSCAN_DATA_SCHEMA_VIEW_DEFAULT,
  HYSCAN_DATA_SCHEMA_VIEW_BIN,
  HYSCAN_DATA_SCHEMA_VIEW_DEC,
  HYSCAN_DATA_SCHEMA_VIEW_HEX,
  HYSCAN_DATA_SCHEMA_VIEW_DATE,
  HYSCAN_DATA_SCHEMA_VIEW_TIME,
  HYSCAN_DATA_SCHEMA_VIEW_DATE_TIME,
  HYSCAN_DATA_SCHEMA_VIEW_RGB,
  HYSCAN_DATA_SCHEMA_VIEW_RGBA,
  HYSCAN_DATA_SCHEMA_VIEW_BUTTON,
  HYSCAN_DATA_SCHEMA_VIEW_FILE,
  HYSCAN_DATA_SCHEMA_VIEW_DIR,
  HYSCAN_DATA_SCHEMA_VIEW_SCHEMA
} HyScanDataSchemaViewType;

/**
 * HyScanDataSchemaKeyAccess:
 * @HYSCAN_DATA_SCHEMA_ACCESS_NONE: нет доступа
 * @HYSCAN_DATA_SCHEMA_ACCESS_READ: доступ на чтение
 * @HYSCAN_DATA_SCHEMA_ACCESS_WRITE: доступ на запись
 * @HYSCAN_DATA_SCHEMA_ACCESS_HIDDEN: невидимый параметр
 * @HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT: доступ по умолчанию - чтение и запись
 *
 * Атрибуты доступа к параметру.
 */
typedef enum
{
  HYSCAN_DATA_SCHEMA_ACCESS_NONE   = 0,
  HYSCAN_DATA_SCHEMA_ACCESS_READ   = 1,
  HYSCAN_DATA_SCHEMA_ACCESS_WRITE  = 1 << 1,
  HYSCAN_DATA_SCHEMA_ACCESS_HIDDEN = 1 << 2,
  HYSCAN_DATA_SCHEMA_ACCESS_DEFAULT = HYSCAN_DATA_SCHEMA_ACCESS_READ | HYSCAN_DATA_SCHEMA_ACCESS_WRITE
} HyScanDataSchemaKeyAccess;

/**
 * HyScanDataSchemaEnumValue:
 * @value: численное значение параметра
 * @id: идентификатор значения параметра
 * @name: название значения параметра
 * @description: описание значения параметра
 *
 * Значение перечисляемого типа #HYSCAN_DATA_SCHEMA_KEY_ENUM.
 */
struct _HyScanDataSchemaEnumValue
{
  gint64                               value;
  const gchar                         *id;
  const gchar                         *name;
  const gchar                         *description;
};

/**
 * HyScanDataSchemaNode:
 * @path: путь до узла
 * @name: название узла
 * @description: описание узла
 * @nodes: (element-type HyScanDataSchemaNode) (transfer none): дочерние узлы
 * @keys: (element-type HyScanDataSchemaKey) (transfer none): параметры
 *
 * Узел с параметрами и подузлами.
 */
struct _HyScanDataSchemaNode
{
  const gchar                         *path;
  const gchar                         *name;
  const gchar                         *description;
  GList                               *nodes;
  GList                               *keys;
};

/**
 * HyScanDataSchemaKey:
 * @id: идентификатор параметра
 * @name: название параметра
 * @description: описание параметра
 * @type: тип параметра
 * @view: рекомендуемый вид отображения параметра
 * @access: атрибуты доступа к параметру
 *
 * Описание параметра.
 */
struct _HyScanDataSchemaKey
{
  const gchar                         *id;
  const gchar                         *name;
  const gchar                         *description;
  HyScanDataSchemaKeyType              type;
  HyScanDataSchemaViewType             view;
  HyScanDataSchemaKeyAccess            access;
};

HYSCAN_API
GType                             hyscan_data_schema_enum_value_get_type  (void);

HYSCAN_API
GType                             hyscan_data_schema_node_get_type        (void);

HYSCAN_API
GType                             hyscan_data_schema_key_get_type         (void);

HYSCAN_API
GType                             hyscan_data_schema_get_type             (void);

HYSCAN_API
HyScanDataSchema *                hyscan_data_schema_new_from_string      (const gchar                     *schema_data,
                                                                           const gchar                     *schema_id);

HYSCAN_API
HyScanDataSchema *                hyscan_data_schema_new_from_file        (const gchar                     *schema_path,
                                                                           const gchar                     *schema_id);

HYSCAN_API
HyScanDataSchema  *               hyscan_data_schema_new_from_resource    (const gchar                     *schema_resource,
                                                                           const gchar                     *schema_id);

HYSCAN_API
const gchar *                     hyscan_data_schema_get_data             (HyScanDataSchema                *schema);

HYSCAN_API
const gchar *                     hyscan_data_schema_get_id               (HyScanDataSchema                *schema);

HYSCAN_API
gint64                            hyscan_data_schema_get_version          (HyScanDataSchema                *schema);

HYSCAN_API
const gchar * const *             hyscan_data_schema_list_keys            (HyScanDataSchema                *schema);

HYSCAN_API
const HyScanDataSchemaNode *      hyscan_data_schema_list_nodes           (HyScanDataSchema                *schema);

HYSCAN_API
gboolean                          hyscan_data_schema_has_key              (HyScanDataSchema                *schema,
                                                                           const gchar                     *key_id);

HYSCAN_API
const gchar *                     hyscan_data_schema_node_get_name        (HyScanDataSchema                *schema,
                                                                           const gchar                     *path);

HYSCAN_API
const gchar *                     hyscan_data_schema_node_get_description (HyScanDataSchema                *schema,
                                                                           const gchar                     *path);

HYSCAN_API
const gchar *                     hyscan_data_schema_key_get_name         (HyScanDataSchema                *schema,
                                                                           const gchar                     *key_id);

HYSCAN_API
const gchar *                     hyscan_data_schema_key_get_description  (HyScanDataSchema                *schema,
                                                                           const gchar                     *key_id);

HYSCAN_API
HyScanDataSchemaKeyType           hyscan_data_schema_key_get_value_type   (HyScanDataSchema                *schema,
                                                                           const gchar                     *key_id);

HYSCAN_API
HyScanDataSchemaViewType          hyscan_data_schema_key_get_view         (HyScanDataSchema                *schema,
                                                                           const gchar                     *key_id);

HYSCAN_API
HyScanDataSchemaKeyAccess         hyscan_data_schema_key_get_access       (HyScanDataSchema                *schema,
                                                                           const gchar                     *key_id);

HYSCAN_API
const gchar                      *hyscan_data_schema_key_get_enum_id      (HyScanDataSchema                *schema,
                                                                           const gchar                     *key_id);

HYSCAN_API
GVariant *                        hyscan_data_schema_key_get_default      (HyScanDataSchema                *schema,
                                                                           const gchar                     *key_id);

HYSCAN_API
GVariant *                        hyscan_data_schema_key_get_minimum      (HyScanDataSchema                *schema,
                                                                           const gchar                     *key_id);

HYSCAN_API
GVariant *                        hyscan_data_schema_key_get_maximum      (HyScanDataSchema                *schema,
                                                                           const gchar                     *key_id);

HYSCAN_API
GVariant *                        hyscan_data_schema_key_get_step         (HyScanDataSchema                *schema,
                                                                           const gchar                     *key_id);

HYSCAN_API
gboolean                          hyscan_data_schema_key_get_boolean      (HyScanDataSchema                *schema,
                                                                           const gchar                     *key_id,
                                                                           gboolean                        *default_value);

HYSCAN_API
gboolean                          hyscan_data_schema_key_get_integer      (HyScanDataSchema                *schema,
                                                                           const gchar                     *key_id,
                                                                           gint64                          *minimum_value,
                                                                           gint64                          *maximum_value,
                                                                           gint64                          *default_value,
                                                                           gint64                          *value_step);

HYSCAN_API
gboolean                          hyscan_data_schema_key_get_double       (HyScanDataSchema                *schema,
                                                                           const gchar                     *key_id,
                                                                           gdouble                         *minimum_value,
                                                                           gdouble                         *maximum_value,
                                                                           gdouble                         *default_value,
                                                                           gdouble                         *value_step);

HYSCAN_API
const gchar *                     hyscan_data_schema_key_get_string       (HyScanDataSchema                *schema,
                                                                           const gchar                     *key_id);

HYSCAN_API
gboolean                          hyscan_data_schema_key_get_enum         (HyScanDataSchema                *schema,
                                                                           const gchar                     *key_id,
                                                                           gint64                          *default_value);

HYSCAN_API
gboolean                          hyscan_data_schema_key_check            (HyScanDataSchema                *schema,
                                                                           const gchar                     *key_id,
                                                                           GVariant                        *value);

HYSCAN_API
gchar *                           hyscan_data_schema_key_print_value      (HyScanDataSchema                *schema,
                                                                           const gchar                     *key_id,
                                                                           GVariant                        *value);

HYSCAN_API
GList *                           hyscan_data_schema_enum_get_values      (HyScanDataSchema                *schema,
                                                                           const gchar                     *enum_id);

HYSCAN_API
const HyScanDataSchemaEnumValue * hyscan_data_schema_enum_find_by_id      (HyScanDataSchema                *schema,
                                                                           const gchar                     *enum_id,
                                                                           const gchar                     *value_id);

HYSCAN_API
const HyScanDataSchemaEnumValue * hyscan_data_schema_enum_find_by_value   (HyScanDataSchema                *schema,
                                                                           const gchar                     *enum_id,
                                                                           gint64                           enum_value);

HYSCAN_API
HyScanDataSchemaEnumValue *       hyscan_data_schema_enum_value_new       (gint64                           value,
                                                                           const gchar                     *id,
                                                                           const gchar                     *name,
                                                                           const gchar                     *description);

HYSCAN_API
HyScanDataSchemaEnumValue *       hyscan_data_schema_enum_value_copy      (const HyScanDataSchemaEnumValue *value);

HYSCAN_API
void                              hyscan_data_schema_enum_value_free      (HyScanDataSchemaEnumValue       *value);

HYSCAN_API
HyScanDataSchemaNode *            hyscan_data_schema_node_new             (const gchar                     *path,
                                                                           const gchar                     *name,
                                                                           const gchar                     *description,
                                                                           GList                           *nodes,
                                                                           GList                           *keys);

HYSCAN_API
HyScanDataSchemaNode *            hyscan_data_schema_node_copy            (const HyScanDataSchemaNode      *node);

HYSCAN_API
void                              hyscan_data_schema_node_free            (HyScanDataSchemaNode            *node);

HYSCAN_API
HyScanDataSchemaKey *             hyscan_data_schema_key_new              (const gchar                     *id,
                                                                           const gchar                     *name,
                                                                           const gchar                     *description,
                                                                           HyScanDataSchemaKeyType          type,
                                                                           HyScanDataSchemaViewType         view,
                                                                           HyScanDataSchemaKeyAccess        access);
HYSCAN_API
HyScanDataSchemaKey *             hyscan_data_schema_key_copy             (const HyScanDataSchemaKey       *key);

HYSCAN_API
void                              hyscan_data_schema_key_free             (HyScanDataSchemaKey             *key);

G_END_DECLS

#endif /* __HYSCAN_DATA_SCHEMA_H__ */
