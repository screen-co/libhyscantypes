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

#include "hyscan-data-schema.h"

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

  HyScanDataSchemaType         type;                           /* Тип параметра. */
  gchar                       *name;                           /* Название параметра. */
  gchar                       *description;                    /* Описание параметра. */
  gboolean                     readonly;                       /* Параметр доступен только для чтения. */

  HyScanDataSchemaEnum        *enum_values;                    /* Варианты значений для перечисляемого типа. */
  gint64                       default_value;                  /* Значение по умолчанию. */
  gint64                       minimum_value;                  /* Минимально возможное значение. */
  gint64                       maximum_value;                  /* Маскимально возможное значение. */
  gint64                       value_step;                     /* Шаг изменения значения. */
} HyScanDataSchemaInternalKey;

/* Функция записывает в область памяти data значение типа gboolean. */
void                   hyscan_data_schema_value_set_boolean    (gpointer                    data,
                                                                gboolean                    value);

/* Функция записывает в область памяти data значение типа gint64. */
void                   hyscan_data_schema_value_set_integer    (gpointer                    data,
                                                                gint64                      value);

/* Функция записывает в область памяти data значение типа gdouble. */
void                   hyscan_data_schema_value_set_double     (gpointer                    data,
                                                                gdouble                     value);

/* Функция записывает в область памяти data указатель на копию строки. */
void                   hyscan_data_schema_value_set_string     (gpointer                    data,
                                                                const gchar                *value);

/* Функция считывает из области памяти data значение типа gboolean. */
gboolean               hyscan_data_schema_value_get_boolean    (gconstpointer               data);

/* Функция считывает из области памяти data значение типа gint64. */
gint64                 hyscan_data_schema_value_get_integer    (gconstpointer               data);

/* Функция считывает из области памяти data значение типа gdouble. */
gdouble                hyscan_data_schema_value_get_double     (gconstpointer               data);

/* Функция считывает из области памяти data указатель на строку. */
const gchar           *hyscan_data_schema_value_get_string     (gconstpointer               data);

/* Функция проверяет имя на предмет допустимости. */
gboolean               hyscan_data_schema_validate_name        (const gchar                *name);

/* Функция проверяет идентификатор на предмет допустимости. */
gboolean               hyscan_data_schema_validate_id          (const gchar                *id);

/* Функция проверяет значение перечисляемого типа на допустимость. */
gboolean               hyscan_data_schema_check_enum           (HyScanDataSchemaEnum       *enums,
                                                                gint64                      value);

/* Функция создаёт новый узел с параметрами. */
HyScanDataSchemaNode  *hyscan_data_schema_new_node             (const gchar                *path);

/* Функция создаёт новую структуру с описанием параметра. */
HyScanDataSchemaKey   *hyscan_data_schema_new_param            (const gchar                *id,
                                                                const gchar                *name,
                                                                const gchar                *description,
                                                                HyScanDataSchemaType        type,
                                                                gboolean                    readonly);

/* Функция добавляет новый параметр в список. */
void                   hyscan_data_schema_insert_param         (HyScanDataSchemaNode       *node,
                                                                const gchar                *id,
                                                                const gchar                *name,
                                                                const gchar                *description,
                                                                HyScanDataSchemaType        type,
                                                                gboolean                    readonly);

/* Функция освобождает память занятую структурой со значениями типа enum. */
void                   hyscan_data_schema_free_enum            (HyScanDataSchemaEnum       *values);

/* Функция освобождает память занятую структурой с параметром. */
void                   hyscan_data_schema_free_key             (HyScanDataSchemaInternalKey *key);

#endif /* __HYSCAN_SCHEMA_INTERNAL_H__ */
