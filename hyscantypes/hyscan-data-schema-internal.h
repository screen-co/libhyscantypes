/* hyscan-data-schema-internal.h
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

#ifndef __HYSCAN_DATA_SCHEMA_INTERNAL_H__
#define __HYSCAN_DATA_SCHEMA_INTERNAL_H__

#include "hyscan-data-schema-builder.h"

/* Данные узла. */
typedef struct
{
  gchar                       *path;                           /* Путь до узла. */
  gchar                       *name;                           /* Название узла. */
  gchar                       *description;                    /* Описание узла. */
} HyScanDataSchemaInternalNode;

/* Данные параметра. */
typedef struct
{
  gchar                       *id;                             /* Идентификатор параметра. */
  gchar                       *name;                           /* Название параметра. */
  gchar                       *description;                    /* Описание параметра. */

  HyScanDataSchemaKeyType      type;                           /* Тип параметра. */
  HyScanDataSchemaViewType     view;                           /* Вид отображения параметра. */
  HyScanDataSchemaKeyAccess    access;                         /* Атрибуты доступа к параметру. */

  GVariantClass                value_type;                     /* Тип значения параметра. */
  gchar                       *enum_id;                        /* Варианты значений для перечисляемого типа. */
  GVariant                    *default_value;                  /* Значение по умолчанию. */
  GVariant                    *minimum_value;                  /* Минимально возможное значение. */
  GVariant                    *maximum_value;                  /* Маскимально возможное значение. */
  GVariant                    *value_step;                     /* Шаг изменения значения. */
} HyScanDataSchemaInternalKey;

/* Функция проверяет имя на предмет допустимости. */
gboolean                       hyscan_data_schema_internal_validate_name       (const gchar                 *name);

/* Функция проверяет идентификатор на предмет допустимости. */
gboolean                       hyscan_data_schema_internal_validate_id         (const gchar                 *id);

/* Функция проверяет значение перечисляемого типа на допустимость. */
gboolean                       hyscan_data_schema_internal_enum_check          (GList                       *values,
                                                                                gint64                       value);

/* Функция освобождает память занятую списком со значениями типа enum. */
void                           hyscan_data_schema_internal_enum_free           (GList                       *values);

/* Функция создаёт новую структуру HyScanDataSchemaInternalNode. */
HyScanDataSchemaInternalNode * hyscan_data_schema_internal_node_new            (const gchar                 *path,
                                                                                const gchar                 *name,
                                                                                const gchar                 *description);

/* Функция освобождает память занятую структурой HyScanDataSchemaInternalNode. */
void                           hyscan_data_schema_internal_node_free           (HyScanDataSchemaInternalNode *node);

/* Функция создаёт новую структуру HyScanDataSchemaInternalKey. */
HyScanDataSchemaInternalKey *  hyscan_data_schema_internal_key_new             (const gchar                 *id,
                                                                                const gchar                 *name,
                                                                                const gchar                 *description);

/* Функция освобождает память занятую структурой HyScanDataSchemaInternalKey. */
void                           hyscan_data_schema_internal_key_free            (HyScanDataSchemaInternalKey *key);

/* Функция добавляет новый узел в список. */
HyScanDataSchemaNode *         hyscan_data_schema_internal_insert_node         (HyScanDataSchemaNode        *node,
                                                                                const gchar                 *path);

/* Функция добавляет новый параметр в список. */
void                           hyscan_data_schema_internal_node_insert_key     (HyScanDataSchemaNode        *node,
                                                                                HyScanDataSchemaKey         *key);

#endif /* __HYSCAN_SCHEMA_INTERNAL_H__ */
