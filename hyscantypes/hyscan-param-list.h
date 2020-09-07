/* hyscan-param-list.h
 *
 * Copyright 2017-2018 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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

HYSCAN_API
HyScanParamList *      hyscan_param_list_new              (void);

HYSCAN_API
void                   hyscan_param_list_update           (HyScanParamList    *list,
                                                           HyScanParamList    *orig);

HYSCAN_API
const gchar * const *  hyscan_param_list_params           (HyScanParamList    *list);

HYSCAN_API
gboolean               hyscan_param_list_contains         (HyScanParamList    *list,
                                                           const gchar        *name);

HYSCAN_API
void                   hyscan_param_list_clear            (HyScanParamList    *list);

HYSCAN_API
void                   hyscan_param_list_add              (HyScanParamList    *list,
                                                           const gchar        *name);

HYSCAN_API
void                   hyscan_param_list_set              (HyScanParamList    *list,
                                                           const gchar        *name,
                                                           GVariant           *value);

HYSCAN_API
void                   hyscan_param_list_set_boolean      (HyScanParamList    *list,
                                                           const gchar        *name,
                                                           gboolean            value);

HYSCAN_API
void                   hyscan_param_list_set_integer      (HyScanParamList    *list,
                                                           const gchar        *name,
                                                           gint64              value);

HYSCAN_API
void                   hyscan_param_list_set_double       (HyScanParamList    *list,
                                                           const gchar        *name,
                                                           gdouble             value);

HYSCAN_API
void                   hyscan_param_list_set_string       (HyScanParamList    *list,
                                                           const gchar        *name,
                                                           const gchar        *value);

HYSCAN_API
void                   hyscan_param_list_set_enum         (HyScanParamList    *list,
                                                           const gchar        *name,
                                                           gint64              value);

HYSCAN_API
GVariant *             hyscan_param_list_get              (HyScanParamList    *list,
                                                           const gchar        *name);

HYSCAN_API
gboolean               hyscan_param_list_get_boolean      (HyScanParamList    *list,
                                                           const gchar        *name);

HYSCAN_API
gint64                 hyscan_param_list_get_integer      (HyScanParamList    *list,
                                                           const gchar        *name);

HYSCAN_API
gdouble                hyscan_param_list_get_double       (HyScanParamList    *list,
                                                           const gchar        *name);

HYSCAN_API
const gchar *          hyscan_param_list_get_string       (HyScanParamList    *list,
                                                           const gchar        *name);

HYSCAN_API
gchar *                hyscan_param_list_dup_string       (HyScanParamList    *list,
                                                           const gchar        *name);

HYSCAN_API
gchar **               hyscan_param_list_dup_stringv      (HyScanParamList    *list,
                                                           const gchar        *name,
                                                           const gchar        *delimiter);

HYSCAN_API
gint64                 hyscan_param_list_get_enum         (HyScanParamList    *list,
                                                           const gchar        *name);

G_END_DECLS

#endif /* __HYSCAN_PARAM_LIST_H__ */
