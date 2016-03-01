/**
 * \file hyscan-data-schema.h
 *
 * \brief Заголовочный файл класса схемы данных.
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2016
 * \license Проприетарная лицензия ООО "Экран"
 *
 * \defgroup HyScanDataSchema HyScanDataSchema - класс схем данных HyScan
 *
 * Класс предназначен для описания модели данных. Под моделью данных подразумевается
 * список параметров имеющих следующие атрибуты:
 *
 * - уникальный идентификатор параметра;
 * - название параметра;
 * - описание параметра;
 * - тип параметра;
 * - значение по умолчанию;
 * - минимальное и максимальное значения параметра;
 * - рекомендуемый шаг изменения параметра;
 * - варианты значений параметра для перечисляемых типов.
 *
 * В схеме данных поддерживаются следующие типы параметров - \link HyScanDataSchemaType \endlink:
 *
 * - HYSCAN_DATA_SCHEMA_TYPE_BOOLEAN - логический тип;
 * - HYSCAN_DATA_SCHEMA_TYPE_INTEGER - целые числа со знаком - gint64;
 * - HYSCAN_DATA_SCHEMA_TYPE_DOUBLE - числа с плавающей точкой - gdouble;
 * - HYSCAN_DATA_SCHEMA_TYPE_STRING - строка с нулём на конце;
 * - HYSCAN_DATA_SCHEMA_TYPE_ENUM - перечисляемый тип.
 *
 * Функции этого класса предназначены, в первую очередь, для классов реализующих доступ к данным
 * или для программ исследующих струкутуру данных. Разработчик должен знать схему данных с которыми
 * он работает.
 *
 * Создание объекта со схемой данных осуществляется функциями #hyscan_data_schema_new_from_file и
 * #hyscan_data_schema_new_from_string. Описание схемы осуществляется с помощью языка XML.
 *
 * Список всех параметров схемы можно получить с помощью функции #hyscan_data_schema_list_keys.
 * Проверить существование параметра в схеме можно функцией #hyscan_data_schema_has_key.
 *
 * Следующая группа функций предназначена для получения атрибутов параметров:
 *
 * - #hyscan_data_schema_key_get_type - возвращает тип параметра;
 * - #hyscan_data_schema_key_get_name - возвращает название параметра;
 * - #hyscan_data_schema_key_get_description - возвращает описание параметра;
 * - #hyscan_data_schema_key_get_default_boolean - возвращает значение по умолчанию для параметра с типом BOOLEAN;
 * - #hyscan_data_schema_key_get_default_integer - возвращает значение по умолчанию для параметра с типом INTEGER;
 * - #hyscan_data_schema_key_get_default_double - возвращает значение по умолчанию для параметра с типом DOUBLE;
 * - #hyscan_data_schema_key_get_default_string - возвращает значение по умолчанию для параметра с типом STRING;
 * - #hyscan_data_schema_key_get_default_enum - возвращает значение по умолчанию для параметра с типом ENUM;
 * - #hyscan_data_schema_key_get_integer_range - возвращает диапазон допустимых значений для параметра с типом INTEGER;
 * - #hyscan_data_schema_key_get_double_range - возвращает диапазон допустимых значений для параметра с типом DOUBLE;
 * - #hyscan_data_schema_key_get_enum_values - возвращает варианты допустимых значений для параметра с типом ENUM.
 * - #hyscan_data_schema_key_get_integer_step - возвращает рекомендуемый шаг изменения значения для параметра с типом INTEGER;
 * - #hyscan_data_schema_key_get_double_step - возвращает рекомендуемый шаг изменения значения для параметра с типом DOUBLE;
 *
 * Проверить значение параметра на предмет нахождения в допустимом диапазоне можно функциями
 * #hyscan_data_schema_key_check_integer, #hyscan_data_schema_key_check_double или
 * #hyscan_data_schema_key_check_enum.
 *
 */

#ifndef __HYSCAN_DATA_SCHEMA_H__
#define __HYSCAN_DATA_SCHEMA_H__

#include <glib-object.h>
#include <hyscan-types-exports.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_DATA_SCHEMA             (hyscan_data_schema_get_type ())
#define HYSCAN_DATA_SCHEMA(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_DATA_SCHEMA, HyScanDataSchema))
#define HYSCAN_IS_DATA_SCHEMA(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_DATA_SCHEMA))
#define HYSCAN_DATA_SCHEMA_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HYSCAN_TYPE_DATA_SCHEMA, HyScanDataSchemaClass))
#define HYSCAN_IS_DATA_SCHEMA_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HYSCAN_TYPE_DATA_SCHEMA))
#define HYSCAN_DATA_SCHEMA_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HYSCAN_TYPE_DATA_SCHEMA, HyScanDataSchemaClass))

typedef struct _HyScanDataSchema HyScanDataSchema;
typedef struct _HyScanDataSchemaClass HyScanDataSchemaClass;

struct _HyScanDataSchemaClass
{
  GObjectClass parent_class;
};

/** \brief Типы параметров */
typedef enum
{
  HYSCAN_DATA_SCHEMA_TYPE_INVALID      = 0,            /**< Недопустимый тип, ошибка. */
  HYSCAN_DATA_SCHEMA_TYPE_BOOLEAN      = 101,          /**< Логический тип. */
  HYSCAN_DATA_SCHEMA_TYPE_INTEGER      = 102,          /**< Целые числа со знаком - gint64. */
  HYSCAN_DATA_SCHEMA_TYPE_DOUBLE       = 103,          /**< Числа с плавающей точкой - gdouble. */
  HYSCAN_DATA_SCHEMA_TYPE_STRING       = 104,          /**< Строка с нулём на конце. */
  HYSCAN_DATA_SCHEMA_TYPE_ENUM         = 105,          /**< Перечисляемый тип. */
} HyScanDataSchemaType;

/** \brief Значение перечисляемого типа */
typedef struct
{
  gint64                               value;          /**< Численное значение параметра. */
  gchar                               *name;           /**< Название значения параметра. */
  gchar                               *description;    /**< Описание значения параметра. */
} HyScanDataSchemaEnumValue;


HYSCAN_TYPES_EXPORT
GType                  hyscan_data_schema_get_type                     (void);

/**
 *
 * Функция создаёт новый объект \link HyScanDataSchema \endlink.
 *
 * \param path путь к XML файлу с описанием схемы;
 * \param schema_id идентификатор загружаемой схемы.
 *
 * \return Указатель на объект \link HyScanDataSchema \endlink.
 *
 */
HYSCAN_TYPES_EXPORT
HyScanDataSchema      *hyscan_data_schema_new_from_file                (const gchar           *path,
                                                                        const gchar           *schema_id);

/**
 *
 * Функция создаёт новый объект \link HyScanDataSchema \endlink.
 *
 * \param data строка с описанием схемы в формате XML;
 * \param schema_id идентификатор загружаемой схемы.
 *
 * \return Указатель на объект \link HyScanDataSchema \endlink.
 *
 */
HYSCAN_TYPES_EXPORT
HyScanDataSchema      *hyscan_data_schema_new_from_string              (const gchar           *data,
                                                                        const gchar           *schema_id);

/**
 *
 * Функция возвращает список параметров определённых в схеме.
 *
 * Пользователь не должен изменять этот список или освобождать память, занимаемую им.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink.
 *
 * \return Список параметров в схеме.
 *
 */
HYSCAN_TYPES_EXPORT
const gchar* const    *hyscan_data_schema_list_keys                    (HyScanDataSchema      *schema);

/**
 *
 * Функция проверяет существование параметра в схеме.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра.
 *
 * \return TRUE - если параметр существует в схеме, иначе - FALSE.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_schema_has_key                      (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id);

/**
 *
 * Функция возвращает тип параметра.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра.
 *
 * \return Тип параметра или HYSCAN_DATA_BOX_TYPE_INVALID - если такого параметра нет в схеме.
 *
 */
HYSCAN_TYPES_EXPORT
HyScanDataSchemaType   hyscan_data_schema_key_get_type                 (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id);

/**
 *
 * Функция возвращает название параметра.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра.
 *
 * \return Название параметра или NULL - если такого параметра нет в схеме.
 *
 */
HYSCAN_TYPES_EXPORT
const gchar           *hyscan_data_schema_key_get_name                 (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id);

/**
 *
 * Функция возвращает описание параметра.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра.
 *
 * \return Описание параметра или NULL - если такого параметра нет в схеме.
 *
 */
HYSCAN_TYPES_EXPORT
const gchar           *hyscan_data_schema_key_get_description          (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id);

/**
 *
 * Функция возвращает значение по умолчанию для параметра с типом BOOLEAN.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра;
 * \param value указатель на переменную для значения по умолчанию.
 *
 * \return TRUE если значение определено, FALSE - если такого параметра нет в схеме или не свопадает тип.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_schema_key_get_default_boolean      (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id,
                                                                        gboolean              *value);

/**
 *
 * Функция возвращает значение по умолчанию для параметра с типом INTEGER.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра;
 * \param value указатель на переменную для значения по умолчанию.
 *
 * \return TRUE если значение определено, FALSE - если такого параметра нет в схеме или не свопадает тип.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_schema_key_get_default_integer      (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id,
                                                                        gint64                *value);

/**
 *
 * Функция возвращает значение по умолчанию для параметра с типом DOUBLE.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра;
 * \param value указатель на переменную для значения по умолчанию.
 *
 * \return TRUE если значение определено, FALSE - если такого параметра нет в схеме или не свопадает тип.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_schema_key_get_default_double       (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id,
                                                                        gdouble               *value);

/**
 *
 * Функция возвращает значение по умолчанию для параметра с типом STRING.
 *
 * Пользователь не должен изменять эту строку или освобождать память занимаемую ей.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра.
 *
 * \return значение по умолчанию или NULL - если такого параметра нет в схеме или не свопадает тип.
 *
 */
HYSCAN_TYPES_EXPORT
const gchar           *hyscan_data_schema_key_get_default_string       (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id);

/**
 *
 * Функция возвращает значение по умолчанию для параметра с типом ENUM.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра;
 * \param value указатель на переменную для значения по умолчанию.
 *
 * \return TRUE если значение определено, FALSE - если такого параметра нет в схеме или не свопадает тип.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_schema_key_get_default_enum         (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id,
                                                                        gint64                *value);

/**
 *
 * Функция возвращает диапазон допустимых значений для параметра с типом INTEGER.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра;
 * \param minimum указатель на переменную для минимального значения;
 * \param maximum указатель на переменную для максимального значения.
 *
 * \return TRUE если значения определены, FALSE - если такого параметра нет в схеме или не свопадает тип.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_schema_key_get_integer_range        (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id,
                                                                        gint64                *minimum,
                                                                        gint64                *maximum);

/**
 *
 * Функция возвращает диапазон допустимых значений для параметра с типом DOUBLE.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра;
 * \param minimum указатель на переменную для минимального значения;
 * \param maximum указатель на переменную для максимального значения.
 *
 * \return TRUE если значения определены, FALSE - если такого параметра нет в схеме или не свопадает тип.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_schema_key_get_double_range         (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id,
                                                                        gdouble               *minimum,
                                                                        gdouble               *maximum);

/**
 *
 * Функция возвращает варианты допустимых значений для параметра с типом ENUM.
 *
 * Пользователь не должен изменять этот список или освобождать память, занимаемую им.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра.
 *
 * \return Список допустимых значений параметра или NULL если такого параметра нет в схеме или не совпадает тип.
 *
 */
HYSCAN_TYPES_EXPORT
const HyScanDataSchemaEnumValue* const *hyscan_data_schema_key_get_enum_values   (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id);

/**
 *
 * Функция возвращает рекомендуемый шаг изменения значения для параметра с типом INTEGER.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра;
 * \param value указатель на переменную для шага изменения значения.
 *
 * \return TRUE если значение определено, FALSE - если такого параметра нет в схеме или не свопадает тип.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_schema_key_get_integer_step         (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id,
                                                                        gint64                *value);

/**
 *
 * Функция возвращает рекомендуемый шаг изменения значения для параметра с типом DOUBLE.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра;
 * \param value указатель на переменную для шага изменения значения.
 *
 * \return TRUE если значение определено, FALSE - если такого параметра нет в схеме или не свопадает тип.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_schema_key_get_double_step          (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id,
                                                                        gdouble               *value);

/**
 *
 * Функция проверяет значение параметра типа INTEGER на предмет нахождения в допустимом диапазоне.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра;
 * \param value значение параметра.
 *
 * \return TRUE если значение находится в допустимых пределах, иначе FALSE.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_schema_key_check_integer            (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id,
                                                                        gint64                 value);

/**
 *
 * Функция проверяет значение параметра типа DOUBLE на предмет нахождения в допустимом диапазоне.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра;
 * \param value значение параметра.
 *
 * \return TRUE если значение находится в допустимых пределах, иначе FALSE.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_schema_key_check_double             (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id,
                                                                        gdouble                value);

/**
 *
 * Функция проверяет значение параметра типа ENUM на предмет нахождения в допустимом диапазоне.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра;
 * \param value значение параметра.
 *
 * \return TRUE если значение находится в допустимых пределах, иначе FALSE.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_schema_key_check_enum               (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id,
                                                                        gint64                 value);

G_END_DECLS

#endif /* __HYSCAN_DATA_SCHEMA_H__ */
