/* hyscan-param-proxy.h
 *
 * Copyright 2019 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
 *
 * This file is part of HyScanTypes.
 *
 * HyScanTypes is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HyScanTypes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Alternatively, you can license this code under a commercial license.
 * Contact the Screen LLC in this case - <info@screen-co.ru>.
 */

/* HyScanTypes имеет двойную лицензию.
 *
 * Во-первых, вы можете распространять HyScanTypes на условиях Стандартной
 * Общественной Лицензии GNU версии 3, либо по любой более поздней версии
 * лицензии (по вашему выбору). Полные положения лицензии GNU приведены в
 * <http://www.gnu.org/licenses/>.
 *
 * Во-вторых, этот программный код можно использовать по коммерческой
 * лицензии. Для этого свяжитесь с ООО Экран - <info@screen-co.ru>.
 */

#ifndef __HYSCAN_PARAM_PROXY_H__
#define __HYSCAN_PARAM_PROXY_H__

#include <hyscan-param.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_PARAM_PROXY             (hyscan_param_proxy_get_type ())
#define HYSCAN_PARAM_PROXY(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_PARAM_PROXY, HyScanParamProxy))
#define HYSCAN_IS_PARAM_PROXY(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_PARAM_PROXY))
#define HYSCAN_PARAM_PROXY_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HYSCAN_TYPE_PARAM_PROXY, HyScanParamProxyClass))
#define HYSCAN_IS_PARAM_PROXY_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HYSCAN_TYPE_PARAM_PROXY))
#define HYSCAN_PARAM_PROXY_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HYSCAN_TYPE_PARAM_PROXY, HyScanParamProxyClass))

typedef struct _HyScanParamProxy HyScanParamProxy;
typedef struct _HyScanParamProxyPrivate HyScanParamProxyPrivate;
typedef struct _HyScanParamProxyClass HyScanParamProxyClass;

struct _HyScanParamProxy
{
  GObject parent_instance;

  HyScanParamProxyPrivate *priv;
};

struct _HyScanParamProxyClass
{
  GObjectClass parent_class;
};

HYSCAN_API
GType                  hyscan_param_proxy_get_type     (void);

HYSCAN_API
HyScanParamProxy *     hyscan_param_proxy_new          (void);

HYSCAN_API
HyScanParamProxy *     hyscan_param_proxy_new_full     (const gchar      *schema_id,
                                                        gint64            schema_version);

HYSCAN_API
gboolean               hyscan_param_proxy_add          (HyScanParamProxy *proxy,
                                                        const gchar      *prefix,
                                                        HyScanParam      *param,
                                                        const gchar      *root);

HYSCAN_API
gboolean               hyscan_param_proxy_node_set_name(HyScanParamProxy *proxy,
                                                        const gchar      *path,
                                                        const gchar      *name,
                                                        const gchar      *description);

HYSCAN_API
gboolean               hyscan_param_proxy_bind         (HyScanParamProxy *proxy);

G_END_DECLS

#endif /* __HYSCAN_PARAM_PROXY_H__ */
