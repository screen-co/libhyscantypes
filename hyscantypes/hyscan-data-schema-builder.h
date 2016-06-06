/**
 * \file hyscan-data-schema-builder.h
 *
 * \brief Заголовочный файл класса генерации схемы данных.
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2016
 * \license Проприетарная лицензия ООО "Экран"
 *
 * \defgroup HyScanDataSchemaBuilder HyScanDataSchemaBuilder - класс генерации схем данных HyScan
 *
 * Класс предназначен для автоматизированного создания схем данных. Подробное описание
 * схем данных приведено в разделе \link HyScanDataSchema \endlink.
 *
 * Создание объекта производится функциями #hyscan_data_schema_builder_new и
 * #hyscan_data_schema_builder_new_with_gettext. Во втором случае можно указать
 * название gettext домена используемого для перевода описания схемы.
 *
 * Списки возможных значений параметров типа ENUM создаются с помощью функций
 * #hyscan_data_schema_builder_enum_create и #hyscan_data_schema_builder_enum_value_create.
 *
 * Параметры создаются с помощью следующих функций:
 *
 * - #hyscan_data_schema_builder_key_boolean_create - для параметров типа BOOLEAN;
 * - #hyscan_data_schema_builder_key_integer_create - для параметров типа INTEGER;
 * - #hyscan_data_schema_builder_key_double_create - для параметров типа DOUBLE;
 * - #hyscan_data_schema_builder_key_string_create - для параметров типа STRING;
 * - #hyscan_data_schema_builder_key_enum_create - для параметров типа ENUM.
 *
 * Для получения XML описания схемы, после определения параметров, необходимо
 * использовать функцию #hyscan_data_schema_builder_get_data.
 *
 */

#ifndef __HYSCAN_DATA_SCHEMA_BUILDER_H__
#define __HYSCAN_DATA_SCHEMA_BUILDER_H__

#include <hyscan-data-schema.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_DATA_SCHEMA_BUILDER             (hyscan_data_schema_builder_get_type ())
#define HYSCAN_DATA_SCHEMA_BUILDER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_DATA_SCHEMA_BUILDER, HyScanDataSchemaBuilder))
#define HYSCAN_IS_DATA_SCHEMA_BUILDER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_DATA_SCHEMA_BUILDER))
#define HYSCAN_DATA_SCHEMA_BUILDER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HYSCAN_TYPE_DATA_SCHEMA_BUILDER, HyScanDataSchemaBuilderClass))
#define HYSCAN_IS_DATA_SCHEMA_BUILDER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HYSCAN_TYPE_DATA_SCHEMA_BUILDER))
#define HYSCAN_DATA_SCHEMA_BUILDER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HYSCAN_TYPE_DATA_SCHEMA_BUILDER, HyScanDataSchemaBuilderClass))

typedef struct _HyScanDataSchemaBuilder HyScanDataSchemaBuilder;
typedef struct _HyScanDataSchemaBuilderPrivate HyScanDataSchemaBuilderPrivate;
typedef struct _HyScanDataSchemaBuilderClass HyScanDataSchemaBuilderClass;

struct _HyScanDataSchemaBuilder
{
  GObject parent_instance;

  HyScanDataSchemaBuilderPrivate *priv;
};

struct _HyScanDataSchemaBuilderClass
{
  GObjectClass parent_class;
};

HYSCAN_API
GType                     hyscan_data_schema_builder_get_type            (void);

/**
 *
 * Функция создаёт новый объект \link HyScanDataSchemaBuilder \endlink.
 *
 * \param schema_id идентификатор создаваемой схемы.
 *
 * \return Указатель на объект \link HyScanDataSchemaBuilder \endlink.
 *
 */
HYSCAN_API
HyScanDataSchemaBuilder  *hyscan_data_schema_builder_new                 (const gchar              *schema_id);

/**
 *
 * Функция создаёт новый объект \link HyScanDataSchemaBuilder \endlink с
 * возможностью перевода.
 *
 * \param schema_id идентификатор создаваемой схемы;
 * \param gettext_domain название домена перевода gettext.
 *
 * \return Указатель на объект \link HyScanDataSchemaBuilder \endlink.
 *
 */
HYSCAN_API
HyScanDataSchemaBuilder  *hyscan_data_schema_builder_new_with_gettext    (const gchar              *schema_id,
                                                                          const gchar              *gettext_domain);

/**
 *
 * Функция создаёт XML описание схемы данных.
 *
 * Пользователь должен освободить память, занимаемую схемой данных, функцией g_free.
 *
 * \param builder указатель на объект \link HyScanDataSchemaBuilder \endlink.
 *
 * \return Описание схемы данных.
 *
 */
HYSCAN_API
gchar                    *hyscan_data_schema_builder_get_data            (HyScanDataSchemaBuilder  *builder);

/**
 *
 * Функция создаёт новый список допустимых значений параметра ENUM.
 *
 * \param builder указатель на объект \link HyScanDataSchemaBuilder \endlink;
 * \param enum_id идентификатор списка.
 *
 * \return TRUE если список создан, FALSE в случае ошибки.
 *
 */
HYSCAN_API
gboolean                  hyscan_data_schema_builder_enum_create         (HyScanDataSchemaBuilder  *builder,
                                                                          const gchar              *enum_id);

/**
 *
 * Функция создаёт новое значение параметра в списке возможных значений.
 *
 * \param builder указатель на объект \link HyScanDataSchemaBuilder \endlink;
 * \param enum_id идентификатор списка;
 * \param value численное значение параметра;
 * \param name название значения параметра;
 * \param description описание значения параметра или NULL.
 *
 * \return TRUE если новое значение создано, FALSE в случае ошибки.
 *
 */
HYSCAN_API
gboolean                  hyscan_data_schema_builder_enum_value_create   (HyScanDataSchemaBuilder  *builder,
                                                                          const gchar              *enum_id,
                                                                          gint64                    value,
                                                                          const gchar              *name,
                                                                          const gchar              *description);

/**
 *
 * Функция создаёт новый параметр типа BOOLEAN.
 *
 * \param builder указатель на объект \link HyScanDataSchemaBuilder \endlink;
 * \param key_id идентификатор параметра;
 * \param name название параметра;
 * \param description описание параметра или NULL;
 * \param readonly признак только для чтения;
 * \param default_value значение по умолчанию.
 *
 * \return TRUE если новый параметр создан, FALSE в случае ошибки.
 *
 */
HYSCAN_API
gboolean                  hyscan_data_schema_builder_key_boolean_create  (HyScanDataSchemaBuilder  *builder,
                                                                          const gchar              *key_id,
                                                                          const gchar              *name,
                                                                          const gchar              *description,
                                                                          gboolean                  readonly,
                                                                          gboolean                  default_value);

/**
 *
 * Функция создаёт новый параметр типа INTEGER.
 *
 * \param builder указатель на объект \link HyScanDataSchemaBuilder \endlink;
 * \param key_id идентификатор параметра;
 * \param name название параметра;
 * \param description описание параметра или NULL;
 * \param readonly признак только для чтения;
 * \param default_value значение по умолчанию;
 * \param minimum_value минимально возможное значение параметра;
 * \param maximum_value максимально возможное значение параметра;
 * \param value_step рекомендуемый шаг изменения значения параметра.
 *
 * \return TRUE если новый параметр создан, FALSE в случае ошибки.
 *
 */
HYSCAN_API
gboolean                  hyscan_data_schema_builder_key_integer_create  (HyScanDataSchemaBuilder  *builder,
                                                                          const gchar              *key_id,
                                                                          const gchar              *name,
                                                                          const gchar              *description,
                                                                          gboolean                  readonly,
                                                                          gint64                    default_value,
                                                                          gint64                    minimum_value,
                                                                          gint64                    maximum_value,
                                                                          gint64                    value_step);

/**
 *
 * Функция создаёт новый параметр типа DOUBLE.
 *
 * \param builder указатель на объект \link HyScanDataSchemaBuilder \endlink;
 * \param key_id идентификатор параметра;
 * \param name название параметра;
 * \param description описание параметра или NULL;
 * \param readonly признак только для чтения;
 * \param default_value значение по умолчанию;
 * \param minimum_value минимально возможное значение параметра;
 * \param maximum_value максимально возможное значение параметра;
 * \param value_step рекомендуемый шаг изменения значения параметра.
 *
 * \return TRUE если новый параметр создан, FALSE в случае ошибки.
 *
 */
HYSCAN_API
gboolean                  hyscan_data_schema_builder_key_double_create   (HyScanDataSchemaBuilder  *builder,
                                                                          const gchar              *key_id,
                                                                          const gchar              *name,
                                                                          const gchar              *description,
                                                                          gboolean                  readonly,
                                                                          gdouble                   default_value,
                                                                          gdouble                   minimum_value,
                                                                          gdouble                   maximum_value,
                                                                          gdouble                   value_step);

/**
 *
 * Функция создаёт новый параметр типа STRING.
 *
 * \param builder указатель на объект \link HyScanDataSchemaBuilder \endlink;
 * \param key_id идентификатор параметра;
 * \param name название параметра;
 * \param description описание параметра или NULL;
 * \param readonly признак только для чтения;
 * \param default_value значение по умолчанию.
 *
 * \return TRUE если новый параметр создан, FALSE в случае ошибки.
 *
 */
HYSCAN_API
gboolean                  hyscan_data_schema_builder_key_string_create   (HyScanDataSchemaBuilder  *builder,
                                                                          const gchar              *key_id,
                                                                          const gchar              *name,
                                                                          const gchar              *description,
                                                                          gboolean                  readonly,
                                                                          const gchar              *default_value);

/**
 *
 * Функция создаёт новый параметр типа ENUM.
 *
 * \param builder указатель на объект \link HyScanDataSchemaBuilder \endlink;
 * \param key_id идентификатор параметра;
 * \param name название параметра;
 * \param description описание параметра или NULL;
 * \param readonly признак только для чтения;
 * \param enum_id идентификатор списка возможных значений параметра;
 * \param default_value значение по умолчанию.
 *
 * \return TRUE если новый параметр создан, FALSE в случае ошибки.
 *
 */
HYSCAN_API
gboolean                  hyscan_data_schema_builder_key_enum_create     (HyScanDataSchemaBuilder  *builder,
                                                                          const gchar              *key_id,
                                                                          const gchar              *name,
                                                                          const gchar              *description,
                                                                          gboolean                  readonly,
                                                                          const gchar              *enum_id,
                                                                          gint64                    default_value);

G_END_DECLS

#endif /* __HYSCAN_DATA_SCHEMA_BUILDER_H__ */
