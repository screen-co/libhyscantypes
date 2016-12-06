/**
 * \file hyscan-data-box.h
 *
 * \brief Заголовочный файл класса параметров в памяти.
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2016
 * \license Проприетарная лицензия ООО "Экран"
 *
 * \defgroup HyScanDataBox HyScanDataBox - класс параметров в памяти
 *
 * Класс предоставляет набор функций для работы с параметрами, определённых схемой
 * \link HyScanDataSchema \endlink и размещаемых в оперативной памяти. Имеется возможность
 * зарегистрировать обработчики, вызываемые при изменении значений параметров, а также
 * отслеживать наличие изменений.
 *
 * Создание объекта класса осуществляется функциями #hyscan_data_box_new_from_string,
 * #hyscan_data_box_new_from_file и #hyscan_data_box_new_from_resource.
 *
 * Класс реализует интерфейс \link HyScanParam \endlink для работы с параметрами.
 *
 * При изменении значения любого параметра увеличивается счётчик, связанный с этим параметром,
 * а также глобальный счётчик. Получить текущее значение счётчика можно функцией
 * #hyscan_data_box_get_mod_count. Пользователь может узнать об изменениях в значениях параметров,
 * используя значения этих счётчиков.
 *
 * Класс предоставляет возможность сохранить текущее значение всех параметров в виде
 * строки и восстановить эти значения в дальнейшем. Для этих целей используются функции
 * #hyscan_data_box_serialize и #hyscan_data_box_deserialize.
 *
 * Перед изменением объект посылает сигнал "set". В нём передаются названия изменяемых параметров
 * и их новые значения. Пользователь может обработать этот сигнал и проверить валидность новых
 * значений. Пользователь может зарегистрировать несколько обработчиков сигнала "set". Если любой
 * из обработчиков сигнала вернёт значение FALSE, новые значения не будут установлены.
 * Прототип callback функции для сигнала:
 *
 * \code
 *
 * gboolean     set_cb (HyDataBox           *data,
 *                      const gchar *const  *names,
 *                      GVarint            **values,
 *                      gpointer             user_data);
 *
 * \endcode
 *
 * Также, при изменении любого параметра, происходит отправка сигнала "changed". Сигнал поддерживает
 * возможность указания имени параметра, для которого будет происходить его отправка "changed::name".
 * Прототип callback функции для сигнала:
 *
 * \code
 *
 * void         changed_cb (HyDataBox   *data,
 *                          const gchar *name,
 *                          gpointer     user_data);
 *
 * \endcode
 *
 */

#ifndef __HYSCAN_DATA_BOX_H__
#define __HYSCAN_DATA_BOX_H__

#include <hyscan-param.h>

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
 * Функция возвращает строку с текущими значениями параметров. Эта строка
 * может использоваться для начальной инициализации значений при создании
 * объекта. Пользователь должен освободить память, занимаемую строкой,
 * с помощью функции g_free.
 *
 * \param data_box указатель на объект \link HyScanDataBox \endlink.
 *
 * \return Строка с текущими значениями параметров или NULL.
 *
 */
HYSCAN_API
gchar                 *hyscan_data_box_serialize               (HyScanDataBox         *data_box);

/**
 *
 * Функция устанавливает значения параметров в значения возвращаемые функцией
 * #hyscan_data_box_serialize.
 *
 * \param data_box указатель на объект \link HyScanDataBox \endlink;
 * \param svalues строка со значениями параметров.
 *
 * \return TRUE если значения установлены, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_data_box_deserialize             (HyScanDataBox         *data_box,
                                                                const gchar           *svalues);

G_END_DECLS

#endif /* __HYSCAN_DATA_BOX_H__ */
