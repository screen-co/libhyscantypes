/**
 * \file hyscan-data-box.h
 *
 * \brief Заголовочный файл класса работы с параметрами.
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2016
 * \license Проприетарная лицензия ООО "Экран"
 *
 * \defgroup HyScanDataBox HyScanDataBox - класс работы с параметрами
 *
 * Класс предоставляет набор функций записи и чтения значений параметров, определённых
 * схемой \link HyScanDataSchema \endlink. Имеется возможность зарегистрировать обработчики,
 * вызываемые при изменении значений параметров, а также отслеживать наличие изменений.
 *
 * Создание объекта класса осуществляется функциями #hyscan_data_box_new_from_schema,
 * #hyscan_data_box_new_from_file и #hyscan_data_box_new_from_resource.
 * Описание используемой схемы можно получить функцией #hyscan_data_box_get_schema.
 *
 * Чтение и запись значений параметров осуществляется с использованием функций
 * #hyscan_data_box_set и #hyscan_data_box_get. В дополнение к ним существуют функции
 * работы с параметрами определённых типов:
 *
 * - #hyscan_data_box_set_boolean - установка значения параметра типа BOOLEAN;
 * - #hyscan_data_box_set_integer - установка значения параметра типа INTEGER;
 * - #hyscan_data_box_set_double - установка значения параметра типа DOUBLE;
 * - #hyscan_data_box_set_string - установка значения параметра типа STRING;
 * - #hyscan_data_box_set_enum - установка значения параметра типа ENUM;
 * - #hyscan_data_box_get_boolean - чтение значения параметра типа BOOLEAN;
 * - #hyscan_data_box_get_integer - чтение значения параметра типа INTEGER;
 * - #hyscan_data_box_get_double - чтение значения параметра типа DOUBLE;
 * - #hyscan_data_box_get_string - чтение значения параметра типа STRING;
 * - #hyscan_data_box_get_enum - чтение значения параметра типа ENUM.
 *
 * При изменении значения любого параметра увеличивается счётчик, связанный с этим параметром,
 * а также глобальный счётчик. Получить текущее значение счётчика можно функцией
 * #hyscan_data_box_get_mod_count. Пользователь может узнать об изменениях в значениях параметров,
 * используя значения этих счётчиков.
 *
 * Также, при изменении любого параметра, происходит отправка сигнала "changed". Сигнал поддерживает
 * возможность указания имени параметра, для которого будет происходить его отправка "changed::name".
 * Прототип callback функции для сигнала:
 * - void callback_function (HyDataBox *data, const gchar *name, gpointer user_data);
 *
 */

#ifndef __HYSCAN_DATA_BOX_H__
#define __HYSCAN_DATA_BOX_H__

#include <hyscan-data-schema.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_DATA_BOX             (hyscan_data_box_get_type ())
#define HYSCAN_DATA_BOX(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_DATA_BOX, HyScanDataBox))
#define HYSCAN_IS_DATA_BOX(obj )         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_DATA_BOX))
#define HYSCAN_DATA_BOX_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HYSCAN_TYPE_DATA_BOX, HyScanDataBoxClass))
#define HYSCAN_IS_DATA_BOX_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HYSCAN_TYPE_DATA_BOX))
#define HYSCAN_DATA_BOX_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HYSCAN_TYPE_DATA_BOX, HyScanDataBoxClass))

typedef struct _HyScanDataBox HyScanDataBox;
typedef struct _HyScanDataBoxPrivate HyScanDataBoxPrivate;
typedef struct _HyScanDataBoxClass HyScanDataBoxClass;

struct _HyScanDataBox
{
  GObject parent_instance;

  HyScanDataBoxPrivate *priv;
};

struct _HyScanDataBoxClass
{
  GObjectClass parent_class;
};

HYSCAN_TYPES_EXPORT
GType                  hyscan_data_box_get_type                (void);

/**
 *
 * Функция создаёт новый объект \link HyScanDataBox \endlink. При создании объекта
 * будет увеличен счётчик ссылок на передаваемый объект \link HyScanDataSchema \endlink.
 *
 * \param schema объект \link HyScanDataSchema \endlink с описанием схемы параметров.
 *
 * \return Указатель на объект \link HyScanDataBox \endlink.
 *
 */
HYSCAN_TYPES_EXPORT
HyScanDataBox         *hyscan_data_box_new_from_schema         (HyScanDataSchema      *schema);

/**
 *
 * Функция создаёт новый объект \link HyScanDataBox \endlink.
 *
 * \param schema_path путь к XML файлу с описанием схемы;
 * \param schema_id идентификатор загружаемой схемы;
 * \param overrides_data строка с переопределениями схемы в формате INI или NULL.
 *
 * \return Указатель на объект \link HyScanDataBox \endlink.
 *
 */
HYSCAN_TYPES_EXPORT
HyScanDataBox         *hyscan_data_box_new_from_file           (const gchar           *schema_path,
                                                                const gchar           *schema_id,
                                                                const gchar           *overrides_data);

/**
 *
 * Функция создаёт новый объект \link HyScanDataBox \endlink.
 *
 * \param schema_resource путь к ресурсу GResource;
 * \param schema_id идентификатор загружаемой схемы;
 * \param overrides_data строка с переопределениями схемы в формате INI или NULL.
 *
 * \return Указатель на объект \link HyScanDataBox \endlink.
 *
 */
HYSCAN_TYPES_EXPORT
HyScanDataBox         *hyscan_data_box_new_from_resource       (const gchar           *schema_resource,
                                                                const gchar           *schema_id,
                                                                const gchar           *overrides_data);

/**
 *
 * Функция возвращает указатель на объект \link HyScanDataSchema \endlink с описанием
 * используемой схемы данных. Владельцем объекта является \link HyScanDataBox \endlink.
 * Если пользователь хочет использовать этот объект отдельно от \link HyScanDataBox \endlink
 * необходимо явным образом увеличить число ссылок на объект функцией g_object_ref.
 *
 * \param data_box указатель на объект \link HyScanDataBox \endlink.
 *
 * \return Указатель на объект \link HyScanDataSchema \endlink.
 *
 */
HYSCAN_TYPES_EXPORT
HyScanDataSchema      *hyscan_data_box_get_schema              (HyScanDataBox         *data_box);

/**
 *
 * Функция возвращает значение счётчика изменений параметра. Если имя параметра
 * равно NULL, возвращается значение глобального счётчика изменений для всех параметров.
 *
 * \param data_box указатель на объект \link HyScanDataBox \endlink;
 * \param name название параметра или NULL.
 *
 * \return Значение счётчика изменений.
 *
 */
HYSCAN_TYPES_EXPORT
guint32                hyscan_data_box_get_mod_count           (HyScanDataBox         *data_box,
                                                                const gchar           *name);

/**
 *
 * Функция устанавливает значение параметра. Если в качестве значения параметра указать NULL,
 * будет установлено значение по умолчанию, определённое в схеме данных.
 *
 * \param data_box указатель на объект \link HyScanDataBox \endlink;
 * \param name название параметра;
 * \param type тип параметра;
 * \param value указатель на переменную с новым значением;
 * \param size размер переменной с новым значением.
 *
 * \return TRUE если значение установлено, FALSE - в случае ошибки.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_box_set                     (HyScanDataBox         *data_box,
                                                                const gchar           *name,
                                                                HyScanDataSchemaType   type,
                                                                gconstpointer          value,
                                                                gint32                 size);

/**
 *
 * Функция считывает значение параметра. Если в качестве указатель на буфер для данных передать NULL,
 * функция вернёт размер памяти требуемый для хранения значения параметра.
 *
 * \param data_box указатель на объект \link HyScanDataBox \endlink;
 * \param name название параметра;
 * \param type тип параметра;
 * \param buffer указатель на буфер для хранения значения параметра;
 * \param buffer_size размер буфера.
 *
 * \return TRUE если значение считано, FALSE - в случае ошибки.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_box_get                     (HyScanDataBox         *data_box,
                                                                const gchar           *name,
                                                                HyScanDataSchemaType   type,
                                                                gpointer               buffer,
                                                                gint32                *buffer_size);

/**
 *
 * Функция устанавливает значение параметра типа BOOLEAN.
 *
 * \param data_box указатель на объект \link HyScanDataBox \endlink;
 * \param name название параметра;
 * \param value новое значение параметра.
 *
 * \return TRUE если значение установлено, FALSE - в случае ошибки.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_box_set_boolean             (HyScanDataBox         *data_box,
                                                                const gchar           *name,
                                                                gboolean               value);

/**
 *
 * Функция устанавливает значение параметра типа INTEGER.
 *
 * \param data_box указатель на объект \link HyScanDataBox \endlink;
 * \param name название параметра;
 * \param value новое значение параметра.
 *
 * \return TRUE если значение установлено, FALSE - в случае ошибки.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_box_set_integer             (HyScanDataBox         *data_box,
                                                                const gchar           *name,
                                                                gint64                 value);

/**
 *
 * Функция устанавливает значение параметра типа DOUBLE.
 *
 * \param data_box указатель на объект \link HyScanDataBox \endlink;
 * \param name название параметра;
 * \param value новое значение параметра.
 *
 * \return TRUE если значение установлено, FALSE - в случае ошибки.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_box_set_double              (HyScanDataBox         *data_box,
                                                                const gchar           *name,
                                                                gdouble                value);

/**
 *
 * Функция устанавливает значение параметра типа STRING.
 *
 * \param data_box указатель на объект \link HyScanDataBox \endlink;
 * \param name название параметра;
 * \param value новое значение параметра.
 *
 * \return TRUE если значение установлено, FALSE - в случае ошибки.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_box_set_string              (HyScanDataBox         *data_box,
                                                                const gchar           *name,
                                                                const gchar           *value);

/**
 *
 * Функция устанавливает значение параметра типа ENUM.
 *
 * \param data_box указатель на объект \link HyScanDataBox \endlink;
 * \param name название параметра;
 * \param value новое значение параметра.
 *
 * \return TRUE если значение установлено, FALSE - в случае ошибки.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_box_set_enum                (HyScanDataBox         *data_box,
                                                                const gchar           *name,
                                                                gint64                 value);

/**
 *
 * Функция считывает значение параметра типа BOOLEAN.
 *
 * \param data_box указатель на объект \link HyScanDataBox \endlink;
 * \param name название параметра;
 * \param value указатель на переменную для значения параметра.
 *
 * \return TRUE если значение считано, FALSE - в случае ошибки.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_box_get_boolean             (HyScanDataBox         *data_box,
                                                                const gchar           *name,
                                                                gboolean              *value);

/**
 *
 * Функция считывает значение параметра типа INTEGER.
 *
 * \param data_box указатель на объект \link HyScanDataBox \endlink;
 * \param name название параметра;
 * \param value указатель на переменную для значения параметра.
 *
 * \return TRUE если значение считано, FALSE - в случае ошибки.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_box_get_integer             (HyScanDataBox         *data_box,
                                                                const gchar           *name,
                                                                gint64                *value);

/**
 *
 * Функция считывает значение параметра типа DOUBLE.
 *
 * \param data_box указатель на объект \link HyScanDataBox \endlink;
 * \param name название параметра;
 * \param value указатель на переменную для значения параметра.
 *
 * \return TRUE если значение считано, FALSE - в случае ошибки.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_box_get_double              (HyScanDataBox         *data_box,
                                                                const gchar           *name,
                                                                gdouble               *value);

/**
 *
 * Функция считывает значение параметра типа STRING.
 *
 * Пользователь должен освободить память, занятую строкой функцией g_free.
 *
 * \param data_box указатель на объект \link HyScanDataBox \endlink;
 * \param name название параметра.
 *
 * \return Значение параметра, NULL - в случае ошибки.
 *
 */
HYSCAN_TYPES_EXPORT
gchar                 *hyscan_data_box_get_string              (HyScanDataBox         *data_box,
                                                                const gchar           *name);

/**
 *
 * Функция считывает значение параметра типа ENUM.
 *
 * \param data_box указатель на объект \link HyScanDataBox \endlink;
 * \param name название параметра;
 * \param value указатель на переменную для значения параметра.
 *
 * \return TRUE если значение считано, FALSE - в случае ошибки.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_box_get_enum                (HyScanDataBox         *data_box,
                                                                const gchar           *name,
                                                                gint64                *value);

G_END_DECLS

#endif /* __HYSCAN_DATA_BOX_H__ */
