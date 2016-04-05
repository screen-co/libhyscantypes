/**
 * \file hyscan-data-schema-overrides.h
 *
 * \brief Заголовочный файл класса переопределений схемы данных.
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2016
 * \license Проприетарная лицензия ООО "Экран"
 *
 * \defgroup HyScanDataSchemaOverrides HyScanDataOverrides - класс переопределений схем данных HyScan
 *
 * Класс предназначен для переопределений значений по умолчанию и диапазонов значений параметров
 * схем данных. Переопределения учитываются при создании объектов \link HyScanDataSchema \endlink.
 * Переопределения хранятся в формате INI и имеют следующий вид:
 *
 * \code
 *
 * [key-name]
 * type = (boolean|integer|double|string|enum)
 * default = value
 * min = value
 * max = value
 * step = value
 * readonly = (true|false)
 *
 * \endcode
 *
 * Для каждого параметра создаётся отдельная группа, имя которой должно совпадать с его именем.
 * В этой группе указываются следующие атрибуты:
 *
 * - type - тип параметра;
 * - default - значение по умолчанию;
 * - readonly - доступность параметра на запись.
 *
 * Для параметров с типами integer и double дополнительно указываются следующие атрибуты:
 *
 * - min - минимальное значение параметра;
 * - max - максимальное значение параметра;
 * - step - рекомендуемый шаг изменения параметра.
 *
 * Создание объекта с переопределениями осуществляется функциями #hyscan_data_schema_overrides_new,
 * #hyscan_data_schema_overrides_new_from_data и #hyscan_data_schema_overrides_new_from_file.
 *
 * Получить строку с переопределениями в формате INI можно функцией #hyscan_data_schema_overrides_get_data.
 *
 * Установить переопределения для параметров можно функциями:
 *
 * - #hyscan_data_schema_overrides_set_boolean - для параметров типа BOOLEAN;
 * - #hyscan_data_schema_overrides_set_integer - для параметров типа INTEGER;
 * - #hyscan_data_schema_overrides_set_double - для параметров типа DOUBLE;
 * - #hyscan_data_schema_overrides_set_string - для параметров типа STRING;
 * - #hyscan_data_schema_overrides_set_enum - для параметров типа ENUM.
 *
 * Считать переопределения можно функциями:
 *
 * - #hyscan_data_schema_overrides_get_boolean - для параметров типа BOOLEAN;
 * - #hyscan_data_schema_overrides_get_integer - для параметров типа INTEGER;
 * - #hyscan_data_schema_overrides_get_double - для параметров типа DOUBLE;
 * - #hyscan_data_schema_overrides_get_string - для параметров типа STRING;
 * - #hyscan_data_schema_overrides_get_enum - для параметров типа ENUM.
 *
 * Установить и считать варианты допустимых значений для параметров ENUM можно функциями
 * #hyscan_data_schema_overrides_set_enum_values и #hyscan_data_schema_overrides_get_enum_values.
 *
 */

#ifndef __HYSCAN_DATA_SCHEMA_OVERRIDES_H__
#define __HYSCAN_DATA_SCHEMA_OVERRIDES_H__

#include <hyscan-data-schema.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_DATA_SCHEMA_OVERRIDES             (hyscan_data_schema_overrides_get_type ())
#define HYSCAN_DATA_SCHEMA_OVERRIDES(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_DATA_SCHEMA_OVERRIDES, HyScanDataSchemaOverrides))
#define HYSCAN_IS_DATA_SCHEMA_OVERRIDES(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_DATA_SCHEMA_OVERRIDES))
#define HYSCAN_DATA_SCHEMA_OVERRIDES_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HYSCAN_TYPE_DATA_SCHEMA_OVERRIDES, HyScanDataSchemaOverridesClass))
#define HYSCAN_IS_DATA_SCHEMA_OVERRIDES_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HYSCAN_TYPE_DATA_SCHEMA_OVERRIDES))
#define HYSCAN_DATA_SCHEMA_OVERRIDES_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HYSCAN_TYPE_DATA_SCHEMA_OVERRIDES, HyScanDataSchemaOverridesClass))

typedef struct _HyScanDataSchemaOverrides HyScanDataSchemaOverrides;
typedef struct _HyScanDataSchemaOverridesPrivate HyScanDataSchemaOverridesPrivate;
typedef struct _HyScanDataSchemaOverridesClass HyScanDataSchemaOverridesClass;

struct _HyScanDataSchemaOverrides
{
  GObject parent_instance;

  HyScanDataSchemaOverridesPrivate *priv;
};

struct _HyScanDataSchemaOverridesClass
{
  GObjectClass parent_class;
};

HYSCAN_TYPES_EXPORT
GType                       hyscan_data_schema_overrides_get_type         (void);

/**
 *
 * Функция создаёт пустой объект \link HyScanDataSchemaOverrides \endlink.
 *
 * \return Указатель на объект \link HyScanDataSchemaOverrides \endlink.
 *
 */
HYSCAN_TYPES_EXPORT
HyScanDataSchemaOverrides  *hyscan_data_schema_overrides_new              (void);

/**
 *
 * Функция создаёт новый объект \link HyScanDataSchemaOverrides \endlink.
 *
 * \param data строка с переопределениями схемы в формате INI;
 *
 * \return Указатель на объект \link HyScanDataSchemaOverrides \endlink.
 *
 */
HYSCAN_TYPES_EXPORT
HyScanDataSchemaOverrides  *hyscan_data_schema_overrides_new_from_data    (const gchar               *data);

/**
 *
 * Функция создаёт новый объект \link HyScanDataSchemaOverrides \endlink.
 *
 * \param path путь к INI файлу с переопределениями схемы;
 *
 * \return Указатель на объект \link HyScanDataSchemaOverrides \endlink.
 *
 */
HYSCAN_TYPES_EXPORT
HyScanDataSchemaOverrides  *hyscan_data_schema_overrides_new_from_file    (const gchar               *path);

/**
 *
 * Функция возвращает переопределения схемы данных в фомате INI.
 *
 * Пользователь должен освободить память, занятую строкой с переопределениями, функцией g_free.
 *
 * \param overrides указатель на объект \link HyScanDataSchemaOverrides \endlink.
 *
 * \return Переопределения схемы данных.
 *
 */
HYSCAN_TYPES_EXPORT
gchar                      *hyscan_data_schema_overrides_get_data         (HyScanDataSchemaOverrides *overrides);

/**
 *
 * Функция задаёт новые значения для параметра типа BOOLEAN.
 *
 * \param overrides указатель на объект \link HyScanDataSchemaOverrides \endlink;
 * \param key_id идентификатор параметра;
 * \param default_value значение по умолчанию;
 * \param readonly доступность параметра на запись.
 *
 * \return TRUE - если новые значения установлены, FALSE - в случае ошибки.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean                    hyscan_data_schema_overrides_set_boolean      (HyScanDataSchemaOverrides *overrides,
                                                                           const gchar               *key_id,
                                                                           gboolean                   default_value,
                                                                           gboolean                   readonly);

/**
 *
 * Функция возвращает значения для параметра типа BOOLEAN.
 *
 * \param overrides указатель на объект \link HyScanDataSchemaOverrides \endlink;
 * \param key_id идентификатор параметра;
 * \param default_value указатель на переменную для значения по умолчанию или NULL.
 * \param readonly указатель на переменную для признака доступности на запись или NULL.
 *
 * \return TRUE если значения определены, FALSE - если такого параметра нет в схеме или не совпадает тип.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean                    hyscan_data_schema_overrides_get_boolean      (HyScanDataSchemaOverrides *overrides,
                                                                           const gchar               *key_id,
                                                                           gboolean                  *default_value,
                                                                           gboolean                  *readonly);

/**
 *
 * Функция задаёт новые значения для параметра типа INTEGER.
 *
 * \param overrides указатель на объект \link HyScanDataSchemaOverrides \endlink;
 * \param key_id идентификатор параметра;
 * \param default_value значение по умолчанию;
 * \param minimum_value минимальное значение;
 * \param maximum_value максимальное значение;
 * \param value_step шаг изменения значения;
 * \param readonly доступность параметра на запись.
 *
 * \return TRUE - если новые значения установлены, FALSE - в случае ошибки.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean                    hyscan_data_schema_overrides_set_integer      (HyScanDataSchemaOverrides *overrides,
                                                                           const gchar               *key_id,
                                                                           gint64                     default_value,
                                                                           gint64                     minimum_value,
                                                                           gint64                     maximum_value,
                                                                           gint64                     value_step,
                                                                           gboolean                   readonly);

/**
 *
 * Функция возвращает значения для параметра типа INTEGER.
 *
 * \param overrides указатель на объект \link HyScanDataSchemaOverrides \endlink;
 * \param key_id идентификатор параметра;
 * \param default_value указатель на переменную для значения по умолчанию или NULL.
 * \param minimum_value указатель на переменную для минимального значения или NULL.
 * \param maximum_value указатель на переменную для максимального значения или NULL.
 * \param value_step указатель на переменную для шага изменения значения или NULL.
 * \param readonly указатель на переменную для признака доступности на запись или NULL.
 *
 * \return TRUE если значения определены, FALSE - если такого параметра нет в схеме или не совпадает тип.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean                    hyscan_data_schema_overrides_get_integer      (HyScanDataSchemaOverrides *overrides,
                                                                           const gchar               *key_id,
                                                                           gint64                    *default_value,
                                                                           gint64                    *minimum_value,
                                                                           gint64                    *maximum_value,
                                                                           gint64                    *value_step,
                                                                           gboolean                  *readonly);

/**
 *
 * Функция задаёт новые значения для параметра типа DOUBLE.
 *
 * \param overrides указатель на объект \link HyScanDataSchemaOverrides \endlink;
 * \param key_id идентификатор параметра;
 * \param default_value значение по умолчанию;
 * \param minimum_value минимальное значение;
 * \param maximum_value максимальное значение;
 * \param value_step шаг изменения значения;
 * \param readonly доступность параметра на запись.
 *
 * \return TRUE - если новые значения установлены, FALSE - в случае ошибки.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean                    hyscan_data_schema_overrides_set_double       (HyScanDataSchemaOverrides *overrides,
                                                                           const gchar               *key_id,
                                                                           gdouble                    default_value,
                                                                           gdouble                    minimum_value,
                                                                           gdouble                    maximum_value,
                                                                           gdouble                    value_step,
                                                                           gboolean                   readonly);

/**
 *
 * Функция возвращает значения для параметра типа DOUBLE.
 *
 * \param overrides указатель на объект \link HyScanDataSchemaOverrides \endlink;
 * \param key_id идентификатор параметра;
 * \param default_value указатель на переменную для значения по умолчанию или NULL.
 * \param minimum_value указатель на переменную для минимального значения или NULL.
 * \param maximum_value указатель на переменную для максимального значения или NULL.
 * \param value_step указатель на переменную для шага изменения значения или NULL.
 * \param readonly указатель на переменную для признака доступности на запись или NULL.
 *
 * \return TRUE если значения определены, FALSE - если такого параметра нет в схеме или не совпадает тип.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean                    hyscan_data_schema_overrides_get_double       (HyScanDataSchemaOverrides *overrides,
                                                                           const gchar               *key_id,
                                                                           gdouble                   *default_value,
                                                                           gdouble                   *minimum_value,
                                                                           gdouble                   *maximum_value,
                                                                           gdouble                   *value_step,
                                                                           gboolean                  *readonly);

/**
 *
 * Функция задаёт новые значения для параметра типа STRING.
 *
 * \param overrides указатель на объект \link HyScanDataSchemaOverrides \endlink;
 * \param key_id идентификатор параметра;
 * \param default_value значение по умолчанию;
 * \param readonly доступность параметра на запись.
 *
 * \return TRUE - если новые значения установлены, FALSE - в случае ошибки.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean                    hyscan_data_schema_overrides_set_string       (HyScanDataSchemaOverrides *overrides,
                                                                           const gchar               *key_id,
                                                                           const gchar               *default_value,
                                                                           gboolean                   readonly);

/**
 *
 * Функция возвращает значения для параметра типа STRING.
 *
 * Пользователь должен освободить память, занятую значением по умолчанию, функцией g_free.
 *
 * \param overrides указатель на объект \link HyScanDataSchemaOverrides \endlink;
 * \param key_id идентификатор параметра;
 * \param default_value указатель на переменную для значения по умолчанию или NULL.
 * \param readonly указатель на переменную для признака доступности на запись или NULL.
 *
 * \return TRUE если значения определены, FALSE - если такого параметра нет в схеме или не совпадает тип.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean                    hyscan_data_schema_overrides_get_string       (HyScanDataSchemaOverrides *overrides,
                                                                           const gchar               *key_id,
                                                                           gchar                    **default_value,
                                                                           gboolean                  *readonly);

/**
 *
 * Функция задаёт новые значения для параметра типа ENUM.
 *
 * \param overrides указатель на объект \link HyScanDataSchemaOverrides \endlink;
 * \param key_id идентификатор параметра;
 * \param default_value значение по умолчанию;
 * \param readonly доступность параметра на запись.
 *
 * \return TRUE - если новые значения установлены, FALSE - в случае ошибки.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean                    hyscan_data_schema_overrides_set_enum         (HyScanDataSchemaOverrides *overrides,
                                                                           const gchar               *key_id,
                                                                           gint64                     default_value,
                                                                           gboolean                   readonly);

/**
 *
 * Функция возвращает значения для параметра типа ENUM.
 *
 * \param overrides указатель на объект \link HyScanDataSchemaOverrides \endlink;
 * \param key_id идентификатор параметра;
 * \param default_value указатель на переменную для значения по умолчанию или NULL.
 * \param readonly указатель на переменную для признака доступности на запись или NULL.
 *
 * \return TRUE если значения определены, FALSE - если такого параметра нет в схеме или не совпадает тип.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean                    hyscan_data_schema_overrides_get_enum         (HyScanDataSchemaOverrides *overrides,
                                                                           const gchar               *key_id,
                                                                           gint64                    *default_value,
                                                                           gboolean                  *readonly);

/**
 *
 * Функция задаёт новые варианты допустимых значений для параметра с типом ENUM.
 *
 * \param overrides указатель на объект \link HyScanDataSchemaOverrides \endlink;
 * \param enum_id идентификатор значений ENUM;
 * \param values новые значения.
 *
 * \return TRUE - если новые значения установлены, FALSE - в случае ошибки.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean                    hyscan_data_schema_overrides_set_enum_values  (HyScanDataSchemaOverrides *overrides,
                                                                           const gchar               *enum_id,
                                                                           const HyScanDataSchemaEnumValue* const *values);

/**
 *
 * Функция возвращает NULL терминированный список вариантов допустимых значений
 * для параметра с типом ENUM.
 *
 * Пользователь должен освободить память, занимаемую списком, функцией #hyscan_data_schema_free_enum_values.
 *
 * \param overrides указатель на объект \link HyScanDataSchemaOverrides \endlink;
 * \param enum_id идентификатор значений ENUM;
 *
 * \return Список допустимых значений параметра или NULL если такого идентификатора ENUM нет.
 *
 */
HYSCAN_TYPES_EXPORT
HyScanDataSchemaEnumValue **hyscan_data_schema_overrides_get_enum_values  (HyScanDataSchemaOverrides *overrides,
                                                                           const gchar               *enum_id);

G_END_DECLS

#endif /* __HYSCAN_DATA_SCHEMA_OVERRIDES_H__ */
