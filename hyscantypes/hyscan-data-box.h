/* hyscan-data-box.h
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
 * Во-первых, вы можете распространять HyScanTypes на условиях Стандартной
 * Общественной Лицензии GNU версии 3, либо по любой более поздней версии
 * лицензии (по вашему выбору). Полные положения лицензии GNU приведены в
 * <http://www.gnu.org/licenses/>.
 *
 * Во-вторых, этот программный код можно использовать по коммерческой
 * лицензии. Для этого свяжитесь с ООО Экран - info@screen-co.ru.
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

HYSCAN_API
HyScanDataBox         *hyscan_data_box_new_from_string         (const gchar           *schema_data,
                                                                const gchar           *schema_id);

HYSCAN_API
HyScanDataBox         *hyscan_data_box_new_from_file           (const gchar           *schema_path,
                                                                const gchar           *schema_id);

HYSCAN_API
HyScanDataBox         *hyscan_data_box_new_from_resource       (const gchar           *schema_resource,
                                                                const gchar           *schema_id);

HYSCAN_API
HyScanDataBox         *hyscan_data_box_new_from_schema         (HyScanDataSchema      *schema);

HYSCAN_API
guint32                hyscan_data_box_get_mod_count           (HyScanDataBox         *data_box,
                                                                const gchar           *name);

HYSCAN_API
gchar                 *hyscan_data_box_serialize               (HyScanDataBox         *data_box);

HYSCAN_API
gboolean               hyscan_data_box_deserialize             (HyScanDataBox         *data_box,
                                                                const gchar           *svalues);

G_END_DECLS

#endif /* __HYSCAN_DATA_BOX_H__ */
