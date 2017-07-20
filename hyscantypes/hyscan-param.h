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
 * Чтение и запись значений параметров осуществляется с использованием функций #hyscan_param_set
 * и #hyscan_param_get. В дополнение к ним существуют функции работы с параметрами определённых
 * типов:
 *
 * - #hyscan_param_set_boolean - установка значения параметра типа BOOLEAN;
 * - #hyscan_param_set_integer - установка значения параметра типа INTEGER;
 * - #hyscan_param_set_double - установка значения параметра типа DOUBLE;
 * - #hyscan_param_set_string - установка значения параметра типа STRING;
 * - #hyscan_param_set_enum - установка значения параметра типа ENUM;
 * - #hyscan_param_get_boolean - чтение значения параметра типа BOOLEAN;
 * - #hyscan_param_get_integer - чтение значения параметра типа INTEGER;
 * - #hyscan_param_get_double - чтение значения параметра типа DOUBLE;
 * - #hyscan_param_get_string - чтение значения параметра типа STRING;
 * - #hyscan_param_get_enum - чтение значения параметра типа ENUM.
 *
 * Функция #hyscan_param_set_default устанавливает значение параметра по умолчанию.
 *
 */

#ifndef __HYSCAN_PARAM_H__
#define __HYSCAN_PARAM_H__

#include <hyscan-data-schema.h>

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
                                                                const gchar *const    *names,
                                                                GVariant             **values);

  gboolean             (*get)                                  (HyScanParam           *param,
                                                                const gchar *const    *names,
                                                                GVariant             **values);
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
 * Функция устанавливает значения параметров. Если в качестве значения параметра указать NULL,
 * будет установлено значение по умолчанию, определённое в схеме данных. В случае успешного
 * вызова, \link HyScanParam \endlink становится владельцем всех объектов GVariant с новыми
 * значениями.
 *
 * \param param указатель на интерфейс \link HyScanParam \endlink;
 * \param names NULL терминированный список названий параметров;
 * \param values список новых значений.
 *
 * \return TRUE если значения установлены, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_param_set                        (HyScanParam           *param,
                                                                const gchar *const    *names,
                                                                GVariant             **values);

/**
 *
 * Функция считывает значения параметров. Пользователь должен самостоятельно выделить
 * память для переменной values, в которую будут записаны указатели на GVariant.
 * После использования, пользователь должен освободить память, используемую
 * объектами GVariant функцией g_variant_unref.
 *
 * \param param указатель на интерфейс \link HyScanParam \endlink;
 * \param names NULL терминированный список названий параметров;
 * \param values список для значений параметров.
 *
 * \return TRUE если значения считаны, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_param_get                        (HyScanParam           *param,
                                                                const gchar *const    *names,
                                                                GVariant             **values);

/**
 *
 * Функция устанавливает значение параметра по умолчанию.
 *
 * \param param указатель на интерфейс \link HyScanParam \endlink;
 * \param name название параметра.
 *
 * \return TRUE если значение установлено, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_param_set_default                (HyScanParam           *param,
                                                                const gchar           *name);

/**
 *
 * Функция устанавливает значение параметра типа BOOLEAN.
 *
 * \param param указатель на интерфейс \link HyScanParam \endlink;
 * \param name название параметра;
 * \param value новое значение параметра.
 *
 * \return TRUE если значение установлено, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_param_set_boolean                (HyScanParam           *param,
                                                                const gchar           *name,
                                                                gboolean               value);

/**
 *
 * Функция устанавливает значение параметра типа INTEGER.
 *
 * \param param указатель на интерфейс \link HyScanParam \endlink;
 * \param name название параметра;
 * \param value новое значение параметра.
 *
 * \return TRUE если значение установлено, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_param_set_integer                (HyScanParam           *param,
                                                                const gchar           *name,
                                                                gint64                 value);

/**
 *
 * Функция устанавливает значение параметра типа DOUBLE.
 *
 * \param param указатель на интерфейс \link HyScanParam \endlink;
 * \param name название параметра;
 * \param value новое значение параметра.
 *
 * \return TRUE если значение установлено, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_param_set_double                 (HyScanParam           *param,
                                                                const gchar           *name,
                                                                gdouble                value);

/**
 *
 * Функция устанавливает значение параметра типа STRING.
 *
 * \param param указатель на интерфейс \link HyScanParam \endlink;
 * \param name название параметра;
 * \param value новое значение параметра или NULL.
 *
 * \return TRUE если значение установлено, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_param_set_string                 (HyScanParam           *param,
                                                                const gchar           *name,
                                                                const gchar           *value);

/**
 *
 * Функция устанавливает значение параметра типа ENUM.
 *
 * \param param указатель на интерфейс \link HyScanParam \endlink;
 * \param name название параметра;
 * \param value новое значение параметра.
 *
 * \return TRUE если значение установлено, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_param_set_enum                   (HyScanParam           *param,
                                                                const gchar           *name,
                                                                gint64                 value);

/**
 *
 * Функция считывает значение параметра типа BOOLEAN.
 *
 * \param param указатель на интерфейс \link HyScanParam \endlink;
 * \param name название параметра;
 * \param value указатель на переменную для значения параметра.
 *
 * \return TRUE если значение считано, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_param_get_boolean                (HyScanParam           *param,
                                                                const gchar           *name,
                                                                gboolean              *value);

/**
 *
 * Функция считывает значение параметра типа INTEGER.
 *
 * \param param указатель на интерфейс \link HyScanParam \endlink;
 * \param name название параметра;
 * \param value указатель на переменную для значения параметра.
 *
 * \return TRUE если значение считано, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_param_get_integer                (HyScanParam           *param,
                                                                const gchar           *name,
                                                                gint64                *value);

/**
 *
 * Функция считывает значение параметра типа DOUBLE.
 *
 * \param param указатель на интерфейс \link HyScanParam \endlink;
 * \param name название параметра;
 * \param value указатель на переменную для значения параметра.
 *
 * \return TRUE если значение считано, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_param_get_double                 (HyScanParam           *param,
                                                                const gchar           *name,
                                                                gdouble               *value);

/**
 *
 * Функция считывает значение параметра типа STRING.
 *
 * Пользователь должен освободить память, занятую строкой функцией g_free.
 *
 * \param param указатель на интерфейс \link HyScanParam \endlink;
 * \param name название параметра.
 *
 * \return Значение параметра, NULL - в случае ошибки.
 *
 */
HYSCAN_API
gchar                 *hyscan_param_get_string                 (HyScanParam           *param,
                                                                const gchar           *name);

/**
 *
 * Функция считывает значение параметра типа ENUM.
 *
 * \param param указатель на интерфейс \link HyScanParam \endlink;
 * \param name название параметра;
 * \param value указатель на переменную для значения параметра.
 *
 * \return TRUE если значение считано, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_param_get_enum                   (HyScanParam           *param,
                                                                const gchar           *name,
                                                                gint64                *value);

G_END_DECLS

#endif /* __HYSCAN_PARAM_H__ */
