/**
 * \file hyscan-param.h
 *
 * \brief Заголовочный файл интерфейса для работы с именованными параметрами.
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2016
 * \license Проприетарная лицензия ООО "Экран"
 *
 * \defgroup HyScanParam HyScanParam - интерфейс для работы с именованными параметрами
 *
 * Интерфейс обеспечивает унифицированный доступ к именованным параметрам.
 *
 * Схему параметрв можно получить с помощью функции #hyscan_param_schema.
 *
 * Чтение и запись значений параметров осуществляется с использованием функций
 * #hyscan_param_set и #hyscan_param_get.
 *
 */

#ifndef __HYSCAN_PARAM_H__
#define __HYSCAN_PARAM_H__

#include <hyscan-data-schema.h>
#include <hyscan-param-list.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_PARAM            (hyscan_param_get_type ())
#define HYSCAN_PARAM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_PARAM, HyScanParam))
#define HYSCAN_IS_PARAM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_PARAM))
#define HYSCAN_PARAM_GET_IFACE(obj)  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), HYSCAN_TYPE_PARAM, HyScanParamInterface))

typedef struct _HyScanParam HyScanParam;
typedef struct _HyScanParamInterface HyScanParamInterface;

struct _HyScanParamInterface
{
  GTypeInterface       g_iface;

  HyScanDataSchema    *(*schema)                               (HyScanParam           *param);

  gboolean             (*set)                                  (HyScanParam           *param,
                                                                HyScanParamList       *list);

  gboolean             (*get)                                  (HyScanParam           *param,
                                                                HyScanParamList       *list);
};

HYSCAN_API
GType                  hyscan_param_get_type                   (void);

/**
 *
 * Функция возвращает указатель на объект \link HyScanDataSchema \endlink, с описанием текущей
 * схемы параметров. После использования пользователь должен освободить этот объект функцией
 * g_object_unref.
 *
 * \param param указатель на интерфейс \link HyScanParam \endlink.
 *
 * \return Указатель на \link HyScanDataSchema \endlink или NULL.
 *
 */
HYSCAN_API
HyScanDataSchema      *hyscan_param_schema                     (HyScanParam           *param);

/**
 *
 * Функция записывает значения параметров. Если в списке присутствует параметр без значения,
 * будет установлено значение по умолчанию, определённое в схеме данных.
 *
 * \param param указатель на интерфейс \link HyScanParam \endlink;
 * \param list список параметров для записи.
 *
 * \return TRUE если значения установлены, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_param_set                        (HyScanParam           *param,
                                                                HyScanParamList       *list);

/**
 *
 * Функция считывает значения параметров. Функция считывает только параметры
 * присутствующие в списке.
 *
 * \param param указатель на интерфейс \link HyScanParam \endlink;
 * \param list список параметров для чтения.
 *
 * \return TRUE если значения считаны, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_param_get                        (HyScanParam           *param,
                                                                HyScanParamList       *list);

G_END_DECLS

#endif /* __HYSCAN_PARAM_H__ */
