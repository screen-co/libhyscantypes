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
 *
 * - id - идентификатор ветки;
 * - schema - идентификатор дочерней схемы.
 *
 * Тэг &lt;node&gt; также может использоваться для создания вложенных структур. В этом случае атрибут "schema"
 * не указывается, а необходимые параметры объявляются в этом теге. Класс \link HyScanDataSchema \endlink
 * поддерживает неограниченную вложенность.
 *
 * Параметры в схеме определяются тегом &lt;key&gt;. Обязательными атрибутами этого тега являются:
 *
 * - id - идентификатор параметра;
 * - name - название параметра (может содержать перевод);
 * - type - тип параметра.
 *
 * Дополнительным атрибутом может быть "flags". Если его значение установлено в "readonly", значение этого
 * параметра нельзя изменять.
 *
 * В схеме данных поддерживаются следующие типы параметров - \link HyScanDataSchemaType \endlink:
 *
 * - "boolean" - #HYSCAN_DATA_SCHEMA_TYPE_BOOLEAN, логический тип - gboolean;
 * - "integer" - #HYSCAN_DATA_SCHEMA_TYPE_INTEGER, целые числа со знаком - gint64;
 * - "double" - #HYSCAN_DATA_SCHEMA_TYPE_DOUBLE, числа с плавающей точкой - gdouble;
 * - "string" - #HYSCAN_DATA_SCHEMA_TYPE_STRING, строка с нулём на конце - gchar*;
 * - "enum" - #HYSCAN_DATA_SCHEMA_TYPE_ENUM, перечисляемый тип - gint64.
 *
 * Тэг &lt;key&gt; может содержать вложенные тэги:
 *
 * - description - строка с описанием параметра (может содержать перевод);
 * - default - значение параметра по умолчанию;
 * - range - диапазон допустимых значений и рекомендуемый шаг изменения.
 *
 * Тэг &lt;range&gt; применим только для типов данных "integer" и "double" и может содержать следующие атрибуты:
 *
 * - min - минимальное значение параметра;
 * - max - максимальное значение параметра;
 * - step - рекомендуемый шаг изменения параметра.
 *
 * Если для типа "string" не задано значение по умолчанию, в качестве такового будет использована пустая строка.
 *
 * При определении параметра перечисляемого типа, вместо атрибута "type" используется "enum", в котором
 * указывается идентификатор группы вариантов значений.
 *
 * Группы вариантов значение определяется тегом &lt;enum&gt;. Тег содержит обязательный атрибут
 * "id" - идентификатор группы значений.
 *
 * Значения в группе определяются тэгом &lt;value&gt;. Обязательными атрибутами этого тега являются:
 *
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
 * #hyscan_data_schema_new_from_file и #hyscan_data_schema_new_from_resource.
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
 * - #hyscan_data_schema_key_get_enum_values - возвращает варианты допустимых значений для параметра с типом ENUM.
 * - #hyscan_data_schema_key_get_default - возвращает значение параметра по умолчанию;
 * - #hyscan_data_schema_key_get_minimum - Функция возвращает минимальное значение параметра;
 * - #hyscan_data_schema_key_get_maximum - Функция возвращает максимальное значение параметра;
 * - #hyscan_data_schema_key_get_step - возвращает рекомендуемый шаг изменения значения параметра.
 *
 * Проверить значение параметра на предмет нахождения в допустимом диапазоне можно функцией
 * #hyscan_data_schema_key_check.
 *
 * Значения параметров определяются с помощью GVariant. Используются следующие типы GVariant:
 *
 * - #HYSCAN_DATA_SCHEMA_TYPE_BOOLEAN - G_VARIANT_CLASS_BOOLEAN;
 * - #HYSCAN_DATA_SCHEMA_TYPE_INTEGER - G_VARIANT_CLASS_INT64;
 * - #HYSCAN_DATA_SCHEMA_TYPE_DOUBLE - G_VARIANT_CLASS_DOUBLE;
 * - #HYSCAN_DATA_SCHEMA_TYPE_STRING - G_VARIANT_CLASS_STRING;
 * - #HYSCAN_DATA_SCHEMA_TYPE_ENUM - G_VARIANT_CLASS_INT64.
 *
 */

#ifndef __HYSCAN_DATA_SCHEMA_H__
#define __HYSCAN_DATA_SCHEMA_H__

#include <glib-object.h>
#include <hyscan-api.h>

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
typedef struct _HyScanDataSchemaKey HyScanDataSchemaKey;

/** \brief Узел с параметрами */
struct _HyScanDataSchemaNode
{
  gchar                               *path;           /**< Путь до узла. */

  HyScanDataSchemaNode               **nodes;          /**< Дочерние узлы. */
  guint                                n_nodes;        /**< Число дочерних узлов. */

  HyScanDataSchemaKey                **keys;           /**< Параметры. */
  guint                                n_keys;         /**< Число параметров. */
};

/** \brief Описание параметра */
struct _HyScanDataSchemaKey
{
  gchar                               *id;             /**< Идентификатор параметров. */
  gchar                               *name;           /**< Название параметра. */
  gchar                               *description;    /**< Описание параметра. */
  HyScanDataSchemaType                 type;           /**< Тип параметра. */
  gboolean                             readonly;       /**< Параметр доступен только для чтения. */
};

HYSCAN_API
GType                  hyscan_data_schema_get_type                     (void);

/**
 *
 * Функция создаёт новый объект \link HyScanDataSchema \endlink.
 *
 * \param schema_data строка с описанием схемы в формате XML;
 * \param schema_id идентификатор загружаемой схемы.
 *
 * \return Указатель на объект \link HyScanDataSchema \endlink.
 *
 */
HYSCAN_API
HyScanDataSchema      *hyscan_data_schema_new_from_string              (const gchar           *schema_data,
                                                                        const gchar           *schema_id);

/**
 *
 * Функция создаёт новый объект \link HyScanDataSchema \endlink.
 *
 * \param schema_path путь к XML файлу с описанием схемы;
 * \param schema_id идентификатор загружаемой схемы.
 *
 * \return Указатель на объект \link HyScanDataSchema \endlink или NULL если указанный файл отсутствует.
 *
 */
HYSCAN_API
HyScanDataSchema      *hyscan_data_schema_new_from_file                (const gchar           *schema_path,
                                                                        const gchar           *schema_id);

/**
 *
 * Функция создаёт новый объект \link HyScanDataSchema \endlink.
 *
 * \param schema_resource путь к ресурсу GResource;
 * \param schema_id идентификатор загружаемой схемы.
 *
 * \return Указатель на объект \link HyScanDataSchema \endlink или NULL если указанный ресурс отсутствует.
 *
 */
HYSCAN_API
HyScanDataSchema      *hyscan_data_schema_new_from_resource            (const gchar           *schema_resource,
                                                                        const gchar           *schema_id);

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
HYSCAN_API
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
HYSCAN_API
gchar                 *hyscan_data_schema_get_id                       (HyScanDataSchema      *schema);

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
HYSCAN_API
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
HYSCAN_API
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
HYSCAN_API
gboolean               hyscan_data_schema_has_key                      (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id);

/**
 *
 * Функция возвращает тип параметра.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра.
 *
 * \return Тип параметра или HYSCAN_DATA_SCHEMA_TYPE_INVALID - если такого параметра нет в схеме.
 *
 */
HYSCAN_API
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
HYSCAN_API
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
HYSCAN_API
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
HYSCAN_API
gboolean               hyscan_data_schema_key_is_readonly              (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id);

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
HYSCAN_API
HyScanDataSchemaEnumValue   **hyscan_data_schema_key_get_enum_values   (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id);

/**
 *
 * Функция возвращает значение параметра по умолчанию. Пользователь должен
 * освободить память, занимаемую значением, функцией g_variant_unref.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра.
 *
 * \return GVariant или NULL - если такого параметра нет в схеме или не установлено значение по умолчанию.
 *
 */
HYSCAN_API
GVariant              *hyscan_data_schema_key_get_default              (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id);

/**
 *
 * Функция возвращает минимальное значение параметра. Данная функция возвращает
 * значение только для параметров типа INTEGER и DOUBLE. Пользователь должен
 * освободить память, занимаемую значением, функцией g_variant_unref.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра.
 *
 * \return GVariant или NULL - если такого параметра нет в схеме или не совпадает его тип.
 *
 */
HYSCAN_API
GVariant              *hyscan_data_schema_key_get_minimum              (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id);

/**
 *
 * Функция возвращает максимальное значение параметра. Данная функция возвращает
 * значение только для параметров типа INTEGER и DOUBLE. Пользователь должен
 * освободить память, занимаемую значением, функцией g_variant_unref.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра.
 *
 * \return GVariant или NULL - если такого параметра нет в схеме или не совпадает его тип.
 *
 */
HYSCAN_API
GVariant              *hyscan_data_schema_key_get_maximum              (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id);

/**
 *
 * Функция возвращает рекомендуемый шаг изменения значения параметра. Данная функция возвращает
 * значение только для параметров типа INTEGER и DOUBLE. Пользователь должен
 * освободить память, занимаемую значением, функцией g_variant_unref.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра.
 *
 * \return GVariant или NULL - если такого параметра нет в схеме или не совпадает его тип.
 *
 */
HYSCAN_API
GVariant              *hyscan_data_schema_key_get_step                 (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id);

/**
 *
 * Функция проверяет значение параметра на предмет нахождения в допустимом диапазоне.
 *
 * \param schema указатель на объект \link HyScanDataSchema \endlink;
 * \param key_id идентификатор параметра;
 * \param value значение параметра.
 *
 * \return TRUE если значение находится в допустимых пределах, иначе FALSE.
 *
 */
HYSCAN_API
gboolean               hyscan_data_schema_key_check                    (HyScanDataSchema      *schema,
                                                                        const gchar           *key_id,
                                                                        GVariant              *value);

/**
 *
 * Функция освобождает память занятую списком узлов и параметров.
 *
 * \param nodes указатель на список узлов и параметров.
 *
 * \return Нет.
 *
 */
HYSCAN_API
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
HYSCAN_API
void                   hyscan_data_schema_free_enum_values             (HyScanDataSchemaEnumValue **values);

G_END_DECLS

#endif /* __HYSCAN_DATA_SCHEMA_H__ */
