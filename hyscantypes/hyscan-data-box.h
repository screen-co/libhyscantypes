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
 * Создание объекта класса осуществляется функциями #hyscan_data_box_new_from_string,
 * #hyscan_data_box_new_from_file и #hyscan_data_box_new_from_resource.
 * Объект HyScanDataBox является наследуемым от \link HyScanDataSchema \endlink.
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
 * Функция #hyscan_data_box_set_default устанавливает значение параметра по умолчанию.
 *
 * При изменении значения любого параметра увеличивается счётчик, связанный с этим параметром,
 * а также глобальный счётчик. Получить текущее значение счётчика можно функцией
 * #hyscan_data_box_get_mod_count. Пользователь может узнать об изменениях в значениях параметров,
 * используя значения этих счётчиков.
 *
 * Перед изменением объект посылает сигнал "set". В нём передаются названия изменяемых параметров
 * и их новые значения. Пользователь может обработать этот сигнал и проверить валидность новых
 * значений. Пользователь может зарегистрировать несколько обработчиков сигнала "set". Если любой
 * из обработчиков сигнала вернёт значение FALSE, новые значения не будут установлены.
 * Прототип callback функции для сигнала:
 * - gboolean set_cb (HyDataBox *data, const gchar *const *names, GVarint *values, gpointer user_data);
 *
 * Также, при изменении любого параметра, происходит отправка сигнала "changed". Сигнал поддерживает
 * возможность указания имени параметра, для которого будет происходить его отправка "changed::name".
 * Прототип callback функции для сигнала:
 * - void changed_cb (HyDataBox *data, const gchar *name, gpointer user_data);
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
  HyScanDataSchema parent_instance;

  HyScanDataBoxPrivate *priv;
};

struct _HyScanDataBoxClass
{
  HyScanDataSchemaClass parent_class;
};

HYSCAN_API
GType                  hyscan_data_box_get_type                (void);

/**
 *
 * Функция создаёт новый объект \link HyScanDataBox \endlink.
 *
 * \param schema_data строка с описанием схемы в формате XML;
 * \param schema_id идентификатор загружаемой схемы.
 *
 * \return Указатель на объект \link HyScanDataBox \endlink.
 *
 */
HYSCAN_API
HyScanDataBox         *hyscan_data_box_new_from_string         (const gchar           *schema_data,
                                                                const gchar           *schema_id);

/**
 *
 * Функция создаёт новый объект \link HyScanDataBox \endlink.
 *
 * \param schema_path путь к XML файлу с описанием схемы;
 * \param schema_id идентификатор загружаемой схемы.
 *
 * \return Указатель на объект \link HyScanDataBox \endlink.
 *
 */
HYSCAN_API
HyScanDataBox         *hyscan_data_box_new_from_file           (const gchar           *schema_path,
                                                                const gchar           *schema_id);

/**
 *
 * Функция создаёт новый объект \link HyScanDataBox \endlink.
 *
 * \param schema_resource путь к ресурсу GResource;
 * \param schema_id идентификатор загружаемой схемы.
 *
 * \return Указатель на объект \link HyScanDataBox \endlink.
 *
 */
HYSCAN_API
HyScanDataBox         *hyscan_data_box_new_from_resource       (const gchar           *schema_resource,
                                                                const gchar           *schema_id);

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
HYSCAN_API
guint32                hyscan_data_box_get_mod_count           (HyScanDataBox         *data_box,
                                                                const gchar           *name);

/**
 *
 * Функция устанавливает значения параметров. Если в качестве значения параметра указать NULL,
 * будет установлено значение по умолчанию, определённое в схеме данных. В случае успешной
 * установки значений, объект \link HyScanDataBox \endlink становится владельцем всех объектов
 * GVariant с новыми значениями.
 *
 * \param data_box указатель на объект \link HyScanDataBox \endlink;
 * \param names NULL терминированный список названий параметров;
 * \param values список новых значений.
 *
 * \return TRUE если значение установлено, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_data_box_set                     (HyScanDataBox         *data_box,
                                                                const gchar    *const *names,
                                                                GVariant             **values);

/**
 *
 * Функция считывает значения параметров. Пользователь должен самостоятельно выделить
 * память для переменной values, в которую будут записаны указатели на GVariant.
 * После использования, пользователь должен освободить память, используемую
 * объектами GVariant функцией g_variant_unref.
 *
 * \param data_box указатель на объект \link HyScanDataBox \endlink;
 * \param names NULL терминированный список названий параметров;
 * \param values список для значений параметров.
 *
 * \return TRUE если значение считано, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_data_box_get                     (HyScanDataBox         *data_box,
                                                                const gchar    *const *names,
                                                                GVariant             **values);

/**
 *
 * Функция устанавливает значение параметра по умолчанию.
 *
 * \param data_box указатель на объект \link HyScanDataBox \endlink;
 * \param name название параметра.
 *
 * \return TRUE если значение установлено, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_data_box_set_default             (HyScanDataBox         *data_box,
                                                                const gchar           *name);

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
HYSCAN_API
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
HYSCAN_API
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
HYSCAN_API
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
HYSCAN_API
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
HYSCAN_API
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
HYSCAN_API
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
HYSCAN_API
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
HYSCAN_API
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
HYSCAN_API
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
HYSCAN_API
gboolean               hyscan_data_box_get_enum                (HyScanDataBox         *data_box,
                                                                const gchar           *name,
                                                                gint64                *value);

G_END_DECLS

#endif /* __HYSCAN_DATA_BOX_H__ */
