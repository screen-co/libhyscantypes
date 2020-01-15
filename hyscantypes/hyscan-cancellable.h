/* hyscan-cancellable.h
 *
 * Copyright 2019 Screen LLC, Alexander Dmitriev <m1n7@yandex.ru>
 *
 * This file is part of HyScanCore.
 *
 * HyScanCore is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HyScanCore is distributed in the hope that it will be useful,
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

/* HyScanCore имеет двойную лицензию.
 *
 * Во-первых, вы можете распространять HyScanCore на условиях Стандартной
 * Общественной Лицензии GNU версии 3, либо по любой более поздней версии
 * лицензии (по вашему выбору). Полные положения лицензии GNU приведены в
 * <http://www.gnu.org/licenses/>.
 *
 * Во-вторых, этот программный код можно использовать по коммерческой
 * лицензии. Для этого свяжитесь с ООО Экран - <info@screen-co.ru>.
 */

#ifndef __HYSCAN_CANCELLABLE_H__
#define __HYSCAN_CANCELLABLE_H__

#include <gio/gio.h>
#include <hyscan-api.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_CANCELLABLE             (hyscan_cancellable_get_type ())
#define HYSCAN_CANCELLABLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_CANCELLABLE, HyScanCancellable))
#define HYSCAN_IS_CANCELLABLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_CANCELLABLE))
#define HYSCAN_CANCELLABLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HYSCAN_TYPE_CANCELLABLE, HyScanCancellableClass))
#define HYSCAN_IS_CANCELLABLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HYSCAN_TYPE_CANCELLABLE))
#define HYSCAN_CANCELLABLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HYSCAN_TYPE_CANCELLABLE, HyScanCancellableClass))

typedef struct _HyScanCancellable HyScanCancellable;
typedef struct _HyScanCancellablePrivate HyScanCancellablePrivate;
typedef struct _HyScanCancellableClass HyScanCancellableClass;

struct _HyScanCancellable
{
  GCancellable parent_instance;

  HyScanCancellablePrivate *priv;
};

struct _HyScanCancellableClass
{
  GCancellableClass parent_class;
};

HYSCAN_API
GType                  hyscan_cancellable_get_type         (void);

HYSCAN_API
HyScanCancellable *    hyscan_cancellable_new              (void);

HYSCAN_API
void                   hyscan_cancellable_set              (HyScanCancellable *cancellable,
                                                            gfloat             current,
                                                            gfloat             next);

HYSCAN_API
void                   hyscan_cancellable_set_total        (HyScanCancellable *cancellable,
                                                            gfloat             current,
                                                            gfloat             start,
                                                            gfloat             end);

HYSCAN_API
gfloat                 hyscan_cancellable_get              (HyScanCancellable *cancellable);

HYSCAN_API
void                   hyscan_cancellable_push             (HyScanCancellable *cancellable);

HYSCAN_API
void                   hyscan_cancellable_pop              (HyScanCancellable *cancellable);

HYSCAN_API
void                   hyscan_cancellable_freeze           (HyScanCancellable *cancellable);

HYSCAN_API
void                   hyscan_cancellable_unfreeze         (HyScanCancellable *cancellable);

G_END_DECLS

#endif /* __HYSCAN_CANCELLABLE_H__ */
