/*
 * \file hyscan-data-schema-internal.h
 *
 * \brief Заголовочный файл внутренних функций для работы со схемами.
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2016
 * \license Проприетарная лицензия ООО "Экран"
 *
 */

#ifndef __HYSCAN_DATA_SCHEMA_INTERNAL_H__
#define __HYSCAN_DATA_SCHEMA_INTERNAL_H__

#include "hyscan-data-schema-builder.h"

/* Варианты значений перечисляемого типа. */
typedef struct
{
  gchar                       *id;                             /* Идентификатор значений перечисляемого типа. */

  HyScanDataSchemaEnumValue  **values;                         /* Варианты значений для перечисляемого типа. */
  gint                         n_values;                       /* Число вариантов значений. */
} HyScanDataSchemaEnum;

/* Данные параметра. */
typedef struct
{
  gchar                       *id;                             /* Идентификатор параметра. */

  HyScanDataSchemaKeyType      type;                           /* Тип параметра. */
  HyScanDataSchemaViewType     view;                           /* Вид отображения параметра. */
  GVariantClass                value_type;                     /* Тип значения параметра. */
  gchar                       *name;                           /* Название параметра. */
  gchar                       *description;                    /* Описание параметра. */
  HyScanDataSchemaKeyAccess    access;                         /* Атрибуты доступа к параметру. */

  HyScanDataSchemaEnum        *enum_values;                    /* Варианты значений для перечисляемого типа. */
  GVariant                    *default_value;                  /* Значение по умолчанию. */
  GVariant                    *minimum_value;                  /* Минимально возможное значение. */
  GVariant                    *maximum_value;                  /* Маскимально возможное значение. */
  GVariant                    *value_step;                     /* Шаг изменения значения. */
} HyScanDataSchemaInternalKey;

/* Функция проверяет имя на предмет допустимости. */
gboolean                       hyscan_data_schema_internal_validate_name       (const gchar                *name);

/* Функция проверяет идентификатор на предмет допустимости. */
gboolean                       hyscan_data_schema_internal_validate_id         (const gchar                *id);

/* Функция создаёт новый узел с параметрами. */
HyScanDataSchemaNode          *hyscan_data_schema_internal_node_new            (const gchar                *path);

/* Функция добавляет новый параметр в список. */
void                           hyscan_data_schema_internal_node_insert_key     (HyScanDataSchemaNode       *node,
                                                                                const gchar                *id,
                                                                                const gchar                *name,
                                                                                const gchar                *description,
                                                                                HyScanDataSchemaKeyType     type,
                                                                                HyScanDataSchemaViewType    view,
                                                                                HyScanDataSchemaKeyAccess   access);

/* Функция освобождает память занятую структурой HyScanDataSchemaInternalKey. */
void                           hyscan_data_schema_internal_key_free            (HyScanDataSchemaInternalKey *key);

/* Функция проверяет значение перечисляемого типа на допустимость. */
gboolean                       hyscan_data_schema_internal_enum_check          (HyScanDataSchemaEnum       *enums,
                                                                                gint64                      value);

/* Функция освобождает память занятую структурой со значениями типа enum. */
void                           hyscan_data_schema_internal_enum_free           (HyScanDataSchemaEnum       *values);

/* Функция добавляет схему или её часть в HyScanDataSchemaBuilder по указанному пути. */
gboolean                       hyscan_data_schema_internal_builder_join_schema (HyScanDataSchemaBuilder    *builder,
                                                                                const gchar                *dst_root,
                                                                                HyScanDataSchema           *schema,
                                                                                const gchar                *src_root);

#endif /* __HYSCAN_SCHEMA_INTERNAL_H__ */
