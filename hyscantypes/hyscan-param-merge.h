/* hyscan-param-merge.h
 *
 * Copyright 2020 Screen LLC, Alexey Sakhnov <alexsakhnov@gmail.com>
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

#ifndef __HYSCAN_PARAM_MERGE_H__
#define __HYSCAN_PARAM_MERGE_H__

#include <hyscan-param.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_PARAM_MERGE             (hyscan_param_merge_get_type ())
#define HYSCAN_PARAM_MERGE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_PARAM_MERGE, HyScanParamMerge))
#define HYSCAN_IS_PARAM_MERGE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_PARAM_MERGE))
#define HYSCAN_PARAM_MERGE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HYSCAN_TYPE_PARAM_MERGE, HyScanParamMergeClass))
#define HYSCAN_IS_PARAM_MERGE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HYSCAN_TYPE_PARAM_MERGE))
#define HYSCAN_PARAM_MERGE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HYSCAN_TYPE_PARAM_MERGE, HyScanParamMergeClass))

typedef struct _HyScanParamMerge HyScanParamMerge;
typedef struct _HyScanParamMergePrivate HyScanParamMergePrivate;
typedef struct _HyScanParamMergeClass HyScanParamMergeClass;

struct _HyScanParamMerge
{
  GObject parent_instance;

  HyScanParamMergePrivate *priv;
};

struct _HyScanParamMergeClass
{
  GObjectClass parent_class;
};

HYSCAN_API
GType                  hyscan_param_merge_get_type    (void);

HYSCAN_API
HyScanParamMerge *     hyscan_param_merge_new         (void);

HYSCAN_API
gboolean               hyscan_param_merge_add         (HyScanParamMerge *merge,
                                                       HyScanParam      *param);

HYSCAN_API
gboolean               hyscan_param_merge_bind        (HyScanParamMerge *merge);

HYSCAN_API
gboolean               hyscan_param_merge_get_keep    (HyScanParamMerge *merge,
                                                       const gchar      *key_id);

HYSCAN_API
void                   hyscan_param_merge_set_keep    (HyScanParamMerge *merge,
                                                       const gchar      *key_id,
                                                       gboolean          keep);

G_END_DECLS

#endif /* __HYSCAN_PARAM_MERGE_H__ */
