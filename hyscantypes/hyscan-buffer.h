/* hyscan-buffer.h
 *
 * Copyright 2017 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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

#ifndef __HYSCAN_BUFFER_H__
#define __HYSCAN_BUFFER_H__

#include <glib-object.h>
#include <hyscan-types.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_BUFFER             (hyscan_buffer_get_type ())
#define HYSCAN_BUFFER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_BUFFER, HyScanBuffer))
#define HYSCAN_IS_BUFFER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_BUFFER))
#define HYSCAN_BUFFER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HYSCAN_TYPE_BUFFER, HyScanBufferClass))
#define HYSCAN_IS_BUFFER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HYSCAN_TYPE_BUFFER))
#define HYSCAN_BUFFER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HYSCAN_TYPE_BUFFER, HyScanBufferClass))

typedef struct _HyScanBuffer HyScanBuffer;
typedef struct _HyScanBufferPrivate HyScanBufferPrivate;
typedef struct _HyScanBufferClass HyScanBufferClass;

struct _HyScanBuffer
{
  GObject parent_instance;

  HyScanBufferPrivate *priv;
};

struct _HyScanBufferClass
{
  GObjectClass parent_class;
};

HYSCAN_API
GType                  hyscan_buffer_get_type           (void);

HYSCAN_API
HyScanBuffer *         hyscan_buffer_new                (void);

HYSCAN_API
gboolean               hyscan_buffer_import_data        (HyScanBuffer          *buffer,
                                                         HyScanBuffer          *raw);

HYSCAN_API
gboolean               hyscan_buffer_export_data        (HyScanBuffer          *buffer,
                                                         HyScanBuffer          *raw,
                                                         HyScanDataType         type);

HYSCAN_API
void                   hyscan_buffer_set_size           (HyScanBuffer          *buffer,
                                                         guint32                size);

HYSCAN_API
guint32                hyscan_buffer_get_size           (HyScanBuffer          *buffer);

HYSCAN_API
void                   hyscan_buffer_set_data_type      (HyScanBuffer          *buffer,
                                                         HyScanDataType         type);

HYSCAN_API
HyScanDataType         hyscan_buffer_get_data_type      (HyScanBuffer          *buffer);

HYSCAN_API
void                   hyscan_buffer_wrap_data          (HyScanBuffer          *buffer,
                                                         HyScanDataType         type,
                                                         gpointer               data,
                                                         guint32                size);

HYSCAN_API
void                   hyscan_buffer_set_data           (HyScanBuffer          *buffer,
                                                         HyScanDataType         type,
                                                         gpointer               data,
                                                         guint32                size);

HYSCAN_API
gpointer               hyscan_buffer_get_data           (HyScanBuffer          *buffer,
                                                         guint32               *size);

HYSCAN_API
void                   hyscan_buffer_wrap_float         (HyScanBuffer          *buffer,
                                                         gfloat                *data,
                                                         guint32                n_points);

HYSCAN_API
void                   hyscan_buffer_set_float          (HyScanBuffer          *buffer,
                                                         gfloat                *data,
                                                         guint32                n_points);

HYSCAN_API
void                   hyscan_buffer_wrap_complex_float (HyScanBuffer          *buffer,
                                                         HyScanComplexFloat    *data,
                                                         guint32                n_points);

HYSCAN_API
void                   hyscan_buffer_set_complex_float  (HyScanBuffer          *buffer,
                                                         HyScanComplexFloat    *data,
                                                         guint32                n_points);

HYSCAN_API
gfloat *               hyscan_buffer_get_float          (HyScanBuffer          *buffer,
                                                         guint32               *n_points);

HYSCAN_API
HyScanComplexFloat *   hyscan_buffer_get_complex_float  (HyScanBuffer          *buffer,
                                                         guint32               *n_points);

G_END_DECLS

#endif /* __HYSCAN_BUFFER_H__ */
