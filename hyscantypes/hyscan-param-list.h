/**
 * \file hyscan-param-list.h
 *
 * \brief Заголовочный файл класса для чтения/записи значений параметров.
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2017
 * \license Проприетарная лицензия ООО "Экран"
 *
 * \defgroup HyScanParamList HyScanParamList - класс для чтения/записи значений параметров.
 *
 * Класс предназначен для чтения/записи параметров через интерфейс \link HyScanParam \endlink.
 *
 * Создание объекта класса осуществляется функцией #hyscan_param_list_new. Класс содержит список
 * имён параметров и при необходимости их значений.
 *
 * Добавление параметров в список осуществляется функциями:
 *
 * - #hyscan_param_list_add - добавляет параметр без значения;
 * - #hyscan_param_list_set - добавляет параметр со значением в виде GVariant;
 * - #hyscan_param_list_set_boolean - добавляет параметр с boolean значением;
 * - #hyscan_param_list_set_integer - добавляет параметр с integer значением;
 * - #hyscan_param_list_set_double - добавляет параметр с double значением;
 * - #hyscan_param_list_set_string - добавляет параметр с строковым значением;
 * - #hyscan_param_list_set_enum - добавляет параметр с enum значением.
 *
 * Добавление параметров без значения может использоваться для считывания значений
 * \link hyscan_param_set \endlink и для установки значений по умолчанию \link hyscan_param_set \endlink.
 *
 * Считывание значений параметров из списка осуществляется функциями:
 *
 * - #hyscan_param_list_get - считывает значение параметра в виде GVariant;
 * - #hyscan_param_list_get_boolean - считывает значение boolean параметра;
 * - #hyscan_param_list_get_integer - считывает значение integer параметра;
 * - #hyscan_param_list_get_double - считывает значение double параметра;
 * - #hyscan_param_list_get_string - считывает значение строкового параметра;
 * - #hyscan_param_list_get_enum - считывает значение enum параметра.
 *
 * Текущий список имён параметров можно получить функцией #hyscan_param_list_params.
 *
 * Проверить параметр на присутствие в списке можно функцией #hyscan_param_list_contains.
 *
 * Очистить список параметров можно функцией #hyscan_param_list_clear.
 *
 * Данный класс не является потокобезопасным.
 *
 */

#ifndef __HYSCAN_PARAM_LIST_H__
#define __HYSCAN_PARAM_LIST_H__

#include <glib-object.h>
#include <hyscan-api.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_PARAM_LIST             (hyscan_param_list_get_type ())
#define HYSCAN_PARAM_LIST(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_PARAM_LIST, HyScanParamList))
#define HYSCAN_IS_PARAM_LIST(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_PARAM_LIST))
#define HYSCAN_PARAM_LIST_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HYSCAN_TYPE_PARAM_LIST, HyScanParamListClass))
#define HYSCAN_IS_PARAM_LIST_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HYSCAN_TYPE_PARAM_LIST))
#define HYSCAN_PARAM_LIST_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HYSCAN_TYPE_PARAM_LIST, HyScanParamListClass))

typedef struct _HyScanParamList HyScanParamList;
typedef struct _HyScanParamListPrivate HyScanParamListPrivate;
typedef struct _HyScanParamListClass HyScanParamListClass;

struct _HyScanParamList
{
  GObject parent_instance;

  HyScanParamListPrivate *priv;
};

struct _HyScanParamListClass
{
  GObjectClass parent_class;
};

HYSCAN_API
GType                  hyscan_param_list_get_type         (void);

/**
 *
 * Функция создаёт новый объект \link HyScanParamList \endlink.
 *
 * \return Указатель на объект \link HyScanParamList \endlink.
 *
 */
HYSCAN_API
HyScanParamList *      hyscan_param_list_new              (void);

/**
 *
 * Функция возвращает текущий список имён параметров. Память, занимаемая
 * списком, принадлежит объекту и не должна изменяться пользователем.
 * Список имён действителен только до изменения параметров.
 *
 * \param list указатель на объект \link HyScanParamList \endlink.
 *
 * \return Текущий список имён параметров.
 *
 */
HYSCAN_API
const gchar * const *  hyscan_param_list_params           (HyScanParamList    *list);

/**
 *
 * Функция проверяет список на присутствие в нём параметр с указанным именем.
 *
 * \param list указатель на объект \link HyScanParamList \endlink;
 * \param name название параметра.
 *
 *
 * \return TRUE - если параметр есть в списке, FALSE - иначе.
 *
 */
HYSCAN_API
gboolean               hyscan_param_list_contains         (HyScanParamList    *list,
                                                           const gchar        *name);

/**
 *
 * Функция очищает текущий список имён параметров.
 *
 * \param list указатель на объект \link HyScanParamList \endlink.
 *
 * \return Нет.
 *
 */
HYSCAN_API
void                   hyscan_param_list_clear            (HyScanParamList    *list);

/**
 *
 * Функция добавляет параметр без значения в список. Если параметр
 * с таким именем уже существует, он будет заменён на параметр без значения.
 *
 * \param list указатель на объект \link HyScanParamList \endlink;
 * \param name название параметра.
 *
 * \return Нет.
 *
 */
HYSCAN_API
void                   hyscan_param_list_add              (HyScanParamList    *list,
                                                           const gchar        *name);

/**
 *
 * Функция добавляет параметр в список со значением в виде GVariant.
 * Если параметр с таким именем уже существует, он будет заменён.
 *
 * \param list указатель на объект \link HyScanParamList \endlink;
 * \param name название параметра;
 * \param value значение параметра или NULL.
 *
 * \return Нет.
 *
 */
HYSCAN_API
void                   hyscan_param_list_set              (HyScanParamList    *list,
                                                           const gchar        *name,
                                                           GVariant           *value);

/**
 *
 * Функция добавляет boolean параметр в список. Если параметр с таким именем уже
 * существует, он будет заменён.
 *
 * \param list указатель на объект \link HyScanParamList \endlink;
 * \param name название параметра;
 * \param value значение параметра.
 *
 * \return Нет.
 *
 */
HYSCAN_API
void                   hyscan_param_list_set_boolean      (HyScanParamList    *list,
                                                           const gchar        *name,
                                                           gboolean            value);

/**
 *
 * Функция добавляет integer параметр в список. Если параметр с таким именем уже
 * существует, он будет заменён.
 *
 * \param list указатель на объект \link HyScanParamList \endlink;
 * \param name название параметра;
 * \param value значение параметра.
 *
 * \return Нет.
 *
 */
HYSCAN_API
void                   hyscan_param_list_set_integer      (HyScanParamList    *list,
                                                           const gchar        *name,
                                                           gint64              value);

/**
 *
 * Функция добавляет double параметр в список. Если параметр с таким именем уже
 * существует, он будет заменён.
 *
 * \param list указатель на объект \link HyScanParamList \endlink;
 * \param name название параметра;
 * \param value значение параметра.
 *
 * \return Нет.
 *
 */
HYSCAN_API
void                   hyscan_param_list_set_double       (HyScanParamList    *list,
                                                           const gchar        *name,
                                                           gdouble             value);

/**
 *
 * Функция добавляет строковый параметр в список. Если параметр с таким именем уже
 * существует, он будет заменён.
 *
 * \param list указатель на объект \link HyScanParamList \endlink;
 * \param name название параметра;
 * \param value значение параметра.
 *
 * \return Нет.
 *
 */
HYSCAN_API
void                   hyscan_param_list_set_string       (HyScanParamList    *list,
                                                           const gchar        *name,
                                                           const gchar        *value);

/**
 *
 * Функция добавляет enum параметр в список. Если параметр с таким именем уже
 * существует, он будет заменён.
 *
 * \param list указатель на объект \link HyScanParamList \endlink;
 * \param name название параметра;
 * \param value значение параметра.
 *
 * \return Нет.
 *
 */
HYSCAN_API
void                   hyscan_param_list_set_enum         (HyScanParamList    *list,
                                                           const gchar        *name,
                                                           gint64              value);

/**
 *
 * Функция считывает значение параметра из списка в виде GVariant. Пользователь
 * должен освободить возвращаемое значение с помощью функции g_variant_unref.
 *
 * \param list указатель на объект \link HyScanParamList \endlink;
 * \param name название параметра.
 *
 * \return Значение параметра или NULL.
 *
 */
HYSCAN_API
GVariant *             hyscan_param_list_get              (HyScanParamList    *list,
                                                           const gchar        *name);

/**
 *
 * Функция считывает значение boolean параметра из списка.
 *
 * \param list указатель на объект \link HyScanParamList \endlink;
 * \param name название параметра.
 *
 * \return Значение параметра или FALSE.
 *
 */
HYSCAN_API
gboolean               hyscan_param_list_get_boolean      (HyScanParamList    *list,
                                                           const gchar        *name);

/**
 *
 * Функция считывает значение integer параметра из списка.
 *
 * \param list указатель на объект \link HyScanParamList \endlink;
 * \param name название параметра.
 *
 * \return Значение параметра или 0.
 *
 */
HYSCAN_API
gint64                 hyscan_param_list_get_integer      (HyScanParamList    *list,
                                                           const gchar        *name);

/**
 *
 * Функция считывает значение double параметра из списка.
 *
 * \param list указатель на объект \link HyScanParamList \endlink;
 * \param name название параметра.
 *
 * \return Значение параметра или 0.0.
 *
 */
HYSCAN_API
gdouble                hyscan_param_list_get_double       (HyScanParamList    *list,
                                                           const gchar        *name);

/**
 *
 * Функция считывает значение строкового параметра из списка. Память, занятая строкой,
 * принадлежит классу и не должна изменяться пользователем.
 *
 * \param list указатель на объект \link HyScanParamList \endlink;
 * \param name название параметра.
 *
 * \return Значение параметра или NULL.
 *
 */
HYSCAN_API
const gchar *          hyscan_param_list_get_string       (HyScanParamList    *list,
                                                           const gchar        *name);

/**
 *
 * Функция считывает значение enum параметра из списка.
 *
 * \param list указатель на объект \link HyScanParamList \endlink;
 * \param name название параметра.
 *
 * \return Значение параметра или 0.
 *
 */
HYSCAN_API
gint64                 hyscan_param_list_get_enum         (HyScanParamList    *list,
                                                           const gchar        *name);

G_END_DECLS

#endif /* __HYSCAN_PARAM_LIST_H__ */
