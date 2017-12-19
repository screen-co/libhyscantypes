/* hyscan-param.h
 *
 * Copyright 2016-2017 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
 *
 * This file is part of HyScanTypes.
 *
 * HyScanTypes is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HyScan is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Alternatively, you can license this code under a commercial license.
 * Contact the Screen LLC in this case - info@screen-co.ru
 */

/* HyScanTypes имеет двойную лицензию.
 *
 * Во первых, вы можете распространять HyScanTypes на условиях Стандартной
 * Общественной Лицензии GNU версии 3, либо по любой более поздней версии
 * лицензии (по вашему выбору). Полные положения лицензии GNU приведены в
 * <http://www.gnu.org/licenses/>.
 *
 * Во вторых, этот программный код можно использовать по коммерческой
 * лицензии. Для этого свяжитесь с ООО Экран - info@screen-co.ru.
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

/**
 * HyScanParamInterface:
 * @g_iface: Базовый интерфейс.
 * @schema: Возврашает текущую схему данных.
 * @set: Устанавливает значения параметров.
 * @get: Считывает значения параметров.
 */
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

HYSCAN_API
HyScanDataSchema      *hyscan_param_schema                     (HyScanParam           *param);

HYSCAN_API
gboolean               hyscan_param_set                        (HyScanParam           *param,
                                                                HyScanParamList       *list);

HYSCAN_API
gboolean               hyscan_param_get                        (HyScanParam           *param,
                                                                HyScanParamList       *list);

G_END_DECLS

#endif /* __HYSCAN_PARAM_H__ */
