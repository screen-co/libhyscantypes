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
 * Схема данных приводится в формате XML. Описание схемы имеет следующий вид:
 *
 * \code
 *
 * <schemalist gettext-domain="domain">
 *
 *   <enum id="modes">
 *     <value name="Fill" value="1">
 *       <description>Filled figure</description>
 *     </value>
 *     <value name="Empty" value="2">
 *       <description>Empty figure</description>
 *     </value>
 *   </enum>
 *
 *   <schema id="color">
 *     <key id="red" name="Red" type="double">
 *       <range min="0.0" max="1.0" step="0.05"/>
 *       <default>0.5</default>
 *       <description>Red component</description>
 *     </key>
 *     <key id="green" name="Green" type="double">
 *       <range min="0.0" max="1.0" step="0.05"/>
 *       <default>0.5</default>
 *       <description>Green component</description>
 *     </key>
 *     <key id="blue" name="Blue" type="double">
 *       <range min="0.0" max="1.0" step="0.05"/>
 *       <default>0.5</default>
 *       <description>Blue component</description>
 *     </key>
 *     <key id="alpha" name="Alpha" type="double" flags="readonly">
 *       <default>0.5</default>
 *       <description>Alpha component</description>
 *     </key>
 *   </schema>
 *
 *   <schema id="circle">
 *     <node id="color" schema="color">
 *     <key id="mode" name="Mode" enum="modes">
 *       <default>1</default>
 *       <description>Filling modee</description>
 *     </key>
 *   </schema>
 *
 * </schemalis>
 *
 * \endcode
 *
 * Все данные с описанием схем хранятся в теге &lt;schemalist&gt;. Опциональным атрибутом этого тега является
 * "gettext-domain" - имя домена с переводами для имён и описаний параметров схем. При загрузке схемы
 * классом \link HyScanDataSchema \endlink, будет использован этот домен для поиска переводов.
 *
 * Описание может хранить несколько схем одновременно. Для определения схемы используется тег &lt;schema&gt;.
 * Тег содержит обязательный атрибут "id" - идентификатор схемы. По этому идентификатору схема загружается
 * классом \link HyScanDataSchema \endlink или может использоваться в составе другой схемы.
 *
 * Включение одной схемы в другую осуществляется тегом &lt;node&gt;. Обязательными атрибутами, в этом случае,
 * являются:
 * - id - идентификатор ветки;
 * - schema - идентификатор дочерней схемы.
 *
 * Тэг &lt;node&gt; также может использоваться для создания вложенных структур. В этом случае атрибут "schema"
 * не указывается, а необходимые параметры объявляются в этом теге. Класс \link HyScanDataSchema \endlink
 * поддерживает неограниченную вложенность.
 *
 * Параметры в схеме определяются тегом &lt;key&gt;. Обязательными атрибутами этого тега являются:
 * - id - идентификатор параметра;
 * - name - название параметра (может содержать перевод);
 * - type - тип параметра.
 *
 * Дополнительным атрибутом может быть "flags". Если его значение установлено в "readonly", значение этого
 * параметра нельзя изменять.
 *
 * В схеме данных поддерживаются следующие типы параметров - \link HyScanDataSchemaType \endlink:
 * - "boolean" - #HYSCAN_DATA_SCHEMA_TYPE_BOOLEAN, логический тип;
 * - "integer" - #HYSCAN_DATA_SCHEMA_TYPE_INTEGER, целые числа со знаком - gint64;
 * - "double" - #HYSCAN_DATA_SCHEMA_TYPE_DOUBLE, числа с плавающей точкой - gdouble;
 * - "string" - #HYSCAN_DATA_SCHEMA_TYPE_STRING, строка с нулём на конце;
 * - "enum" - #HYSCAN_DATA_SCHEMA_TYPE_ENUM, перечисляемый тип.
 *
 * Тэг &lt;key&gt; может содержать вложенные тэги:
 * - description - строка с описанием параметра (может содержать перевод);
 * - default - значение параметра по умолчанию;
 * - range - диапазон допустимых значений и рекомендуемый шаг изменения.
 *
 * Тэг &lt;range&gt; применим только для типов данных "integer" и "double" и может содержать следующие атрибуты:
 * - min - минимальное значение параметра;
 * - max - максимальное значение параметра;
 * - step - рекомендуемый шаг изменения параметра.
 *
 * При определении параметра перечисляемого типа, вместо атрибута "type" используется "enum", в котором
 * указывается идентификатор группы вариантов значений.
 *
 * Группы вариантов значение определяется тегом &lt;enum&gt;. Тег содержит обязательный атрибут
 * "id" - идентификатор группы значений.
 *
 * Значения в группе определяются тэгом &lt;value&gt;. Обязательными атрибутами этого тега являются:
 * - name - название значения (может содержать перевод);
 * - value - численный идентификатор значения.
 *
 * Тэг &lt;value&gt; может содержать вложенный тег &lt;description&gt; с описанием значения. Описание
 * значения может содержать перевод.
 *
 * Функции этого класса предназначены, в первую очередь, для классов реализующих доступ к данным
 * или для программ исследующих струкутуру данных. Разработчик должен знать схему данных с которыми
 * он работает.
 *
 * Получить описание схемы данных можно функцией #hyscan_data_schema_get_data, а идентификатор схемы
 * функцией #hyscan_data_schema_get_id.
 *
 * Создание объекта со схемой данных осуществляется функциями #hyscan_data_schema_new_from_string,
 * #hyscan_data_schema_new_from_file, #hyscan_data_schema_new_from_file_all,
 * #hyscan_data_schema_new_from_resource или #hyscan_data_schema_new_from_resource_all.
 *
 * Список всех параметров схемы можно получить с помощью функции #hyscan_data_schema_list_keys.
 * Проверить существование параметра в схеме можно функцией #hyscan_data_schema_has_key.
 *
 * Структурированный список параметров и их описаний можно получить функцией #hyscan_data_schema_list_nodes.
 * Функция возвращает указатель на вновь созданую структуру \link _HyScanDataSchemaNode \endlink.
 * После использования необходимо освободить память, занятую этой структурой, с помощью функции
 * #hyscan_data_schema_free_nodes.
 *
 * Функция #hyscan_data_schema_key_is_readonly предназначена для проверки доступности параметра на запись.
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
typedef struct _HyScanDataSchemaPrivate HyScanDataSchemaPrivate;
typedef struct _HyScanDataSchemaClass HyScanDataSchemaClass;

struct _HyScanDataSchema
{
  GObject parent_instance;

  HyScanDataSchemaPrivate *priv;
};

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

typedef struct _HyScanDataSchemaNode HyScanDataSchemaNode;
typedef struct _HyScanDataSchemaParam HyScanDataSchemaParam;

/** \brief Узел с параметрами */
struct _HyScanDataSchemaNode
{
  gchar                               *path;           /**< Путь до узла. */

  HyScanDataSchemaNode               **nodes;          /**< Дочерние узлы. */
  gint                                 n_nodes;        /**< Число дочерних узлов. */

  HyScanDataSchemaParam              **params;         /**< Параметры. */
  gint                                 n_params;       /**< Число параметров. */
};

/** \brief Описание параметра */
struct _HyScanDataSchemaParam
{
  gchar                               *id;             /**< Идентификатор параметров. */
  gchar                               *name;           /**< Название параметра. */
  gchar                               *description;    /**< Описание параметра. */
  HyScanDataSchemaType                 type;           /**< Тип параметра. */
  gboolean                             readonly;       /**< Параметр доступен только для чтения. */
};

HYSCAN_TYPES_EXPORT
GType                  hyscan_data_schema_get_type                     (void);

/**
 *
 * Функция создаёт новый объект \link HyScanDataSchema \endlink.
 *
 * \param schema_data строка с описанием схемы в формате XML;
 * \param schema_id идентификатор загружаемой схемы;
 * \param overrides_data строка с переопределениями схемы в формате INI или NULL.
 *
 * \return Указатель на объект \link HyScanDataSchema \endlink.
 *
 */
HYSCAN_TYPES_EXPORT
HyScanDataSchema      *hyscan_data_schema_new_from_string              (const gchar           *schema_data,
                                                                        const gchar           *schema_id,
                                                                        const gchar           *overrides_data);

/**
 *
 * Функция создаёт новый объект \link HyScanDataSchema \endlink.
 *
 * \param schema_path путь к XML файлу с описанием схемы;
 * \param schema_id идентификатор загружаемой схемы;
 * \param overrides_data строка с переопределениями схемы в формате INI или NULL.
 *
 * \return Указатель на объект \link HyScanDataSchema \endlink или NULL если указанный файл отсутствует.
 *
 */
HYSCAN_TYPES_EXPORT
HyScanDataSchema      *hyscan_data_schema_new_from_file                (const gchar           *schema_path,
                                                                        const gchar           *schema_id,
                                                                        const gchar           *overrides_data);

/**
 *
 * Функция создаёт новый объект \link HyScanDataSchema \endlink.
 *
 * \param schema_path путь к XML файлу с описанием схемы;
 * \param schema_id идентификатор загружаемой схемы;
 * \param overrides_path путь к INI файлу с переопределениями схемы.
 *
 * \return Указатель на объект \link HyScanDataSchema \endlink или NULL если указанный файл отсутствует.
 *
 */
HYSCAN_TYPES_EXPORT
HyScanDataSchema      *hyscan_data_schema_new_from_file_all            (const gchar           *schema_path,
                                                                        const gchar           *schema_id,
                                                                        const gchar           *overrides_path);

/**
 *
 * Функция создаёт новый объект \link HyScanDataSchema \endlink.
 *
 * \param schema_resource путь к ресурсу GResource;
 * \param schema_id идентификатор загружаемой схемы;
 * \param overrides_data строка с переопределениями схемы в формате INI или NULL.
 *
 * \return Указатель на объект \link HyScanDataSchema \endlink или NULL если указанный ресурс отсутствует.
 *
 */
HYSCAN_TYPES_EXPORT
HyScanDataSchema      *hyscan_data_schema_new_from_resource            (const gchar           *schema_resource,
                                                                        const gchar           *schema_id,
                                                                        const gchar           *overrides_data);

/**
 *
 * Функция создаёт новый объект \link HyScanDataSchema \endlink.
 *
 * \param schema_resource путь к ресурсу GResource со схемой;
 * \param schema_id идентификатор загружаемой схемы;
 * \param overrides_resource путь к ресурсу GResource с переопределениями схемы.
 *
 * \return Указатель на объект \link HyScanDataSchema \endlink или NULL если указанный ресурс отсутствует.
 *
 */
HYSCAN_TYPES_EXPORT
HyScanDataSchema      *hyscan_data_schema_new_from_resource_all        (const gchar           *schema_resource,
                                                                        const gchar           *schema_id,
                                                                        const gchar           *overrides_resource);

/**
 *
 * Функция возвращает описание схемы данных в фомате XML.
 *
 * Пользователь должен освободить память, занимаемую схемой данных, функцией g_free.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink.
 *
 * \return Описание схемы данных.
 *
 */
HYSCAN_TYPES_EXPORT
gchar                 *hyscan_data_schema_get_data                     (HyScanDataSchema      *schema);

/**
 *
 * Функция возвращает идентификатор используемой схемы данных.
 *
 * Пользователь должен освободить память, занимаемую идентификатором, функцией g_free.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink.
 *
 * \return Идентификатор схемы данных.
 *
 */
HYSCAN_TYPES_EXPORT
gchar                 *hyscan_data_schema_get_id                       (HyScanDataSchema      *schema);

/**
 *
 * Функция возвращает переопределения используемой схемы данных.
 *
 * Пользователь должен освободить память, занимаемую переопределениями, функцией g_free.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink.
 *
 * \return Переопределения схемы данных.
 *
 */
HYSCAN_TYPES_EXPORT
gchar                 *hyscan_data_schema_get_overrides                (HyScanDataSchema      *schema);

/**
 *
 * Функция возвращает список параметров определённых в схеме.
 *
 * Пользователь должен освободить память, занимаемую списком, функцией g_strfreev.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink.
 *
 * \return Список параметров в схеме.
 *
 */
HYSCAN_TYPES_EXPORT
gchar                **hyscan_data_schema_list_keys                    (HyScanDataSchema      *schema);

/**
 *
 * Функция возвращает иеархический список узлов и параметров определённых в схеме.
 *
 * Пользователь должен освободить память, занимаемую списком, функцией #hyscan_data_schema_free_nodes.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink.
 *
 * \return Список узлов и параметров в схеме.
 *
 */
HYSCAN_TYPES_EXPORT
HyScanDataSchemaNode  *hyscan_data_schema_list_nodes                   (HyScanDataSchema      *schema);

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
 * Функция возвращает доступность параметра на запись.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра.
 *
 * \return TRUE - если параметр доступен только для чтения, FALSE - если доступен на запись.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_schema_key_is_readonly              (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id);

/**
 *
 * Функция возвращает значение по умолчанию для параметра с типом BOOLEAN.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра;
 * \param value указатель на переменную для значения по умолчанию.
 *
 * \return TRUE если значение определено, FALSE - если такого параметра нет в схеме или не совпадает тип.
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
 * \return TRUE если значение определено, FALSE - если такого параметра нет в схеме или не совпадает тип.
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
 * \return TRUE если значение определено, FALSE - если такого параметра нет в схеме или не совпадает тип.
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
 * \return значение по умолчанию или NULL - если такого параметра нет в схеме или не совпадает тип.
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
 * \return TRUE если значение определено, FALSE - если такого параметра нет в схеме или не совпадает тип.
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
 * \return TRUE если значения определены, FALSE - если такого параметра нет в схеме или не совпадает тип.
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
 * \return TRUE если значения определены, FALSE - если такого параметра нет в схеме или не совпадает тип.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_schema_key_get_double_range         (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id,
                                                                        gdouble               *minimum,
                                                                        gdouble               *maximum);

/**
 *
 * Функция возвращает NULL терминированный список вариантов допустимых значений
 * для параметра с типом ENUM.
 *
 * Пользователь должен освободить память, занимаемую списком, функцией #hyscan_data_schema_free_enum_values.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра.
 *
 * \return Список допустимых значений параметра или NULL если такого параметра нет в схеме или не совпадает тип.
 *
 */
HYSCAN_TYPES_EXPORT
HyScanDataSchemaEnumValue   **hyscan_data_schema_key_get_enum_values   (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id);

/**
 *
 * Функция возвращает рекомендуемый шаг изменения значения для параметра с типом INTEGER.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра;
 * \param value указатель на переменную для шага изменения значения.
 *
 * \return TRUE если значение определено, FALSE - если такого параметра нет в схеме или не совпадает тип.
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
 * \return TRUE если значение определено, FALSE - если такого параметра нет в схеме или не совпадает тип.
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

/**
 *
 * Функция освобождает память занятую списком узлов и параметров.
 *
 * \param nodes указатель на список узлов и параметров.
 *
 * \return Нет.
 *
 */
HYSCAN_TYPES_EXPORT
void                   hyscan_data_schema_free_nodes                   (HyScanDataSchemaNode  *nodes);

/**
 *
 * Функция освобождает память занятую списком вариантов допустимых значений для параметра с типом ENUM.
 *
 * \param values указатель на список значений.
 *
 * \return Нет.
 *
 */
HYSCAN_TYPES_EXPORT
void                   hyscan_data_schema_free_enum_values             (HyScanDataSchemaEnumValue **values);

G_END_DECLS

#endif /* __HYSCAN_DATA_SCHEMA_H__ */
