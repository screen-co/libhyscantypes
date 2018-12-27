/* hyscan-param-controller.h
 *
 * Copyright 2018 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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

#ifndef __HYSCAN_PARAM_CONTROLLER_H__
#define __HYSCAN_PARAM_CONTROLLER_H__

#include <hyscan-param.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_PARAM_CONTROLLER             (hyscan_param_controller_get_type ())
#define HYSCAN_PARAM_CONTROLLER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_PARAM_CONTROLLER, HyScanParamController))
#define HYSCAN_IS_PARAM_CONTROLLER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_PARAM_CONTROLLER))
#define HYSCAN_PARAM_CONTROLLER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HYSCAN_TYPE_PARAM_CONTROLLER, HyScanParamControllerClass))
#define HYSCAN_IS_PARAM_CONTROLLER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HYSCAN_TYPE_PARAM_CONTROLLER))
#define HYSCAN_PARAM_CONTROLLER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HYSCAN_TYPE_PARAM_CONTROLLER, HyScanParamControllerClass))

/**
 * hyscan_param_controller_set:
 * @name: название параметра
 * @value: значение параметра
 * @user_data: пользовательские данные
 *
 * Функция устанавливает новое значение параметра.
 *
 * Returns: %TRUE если значение установлено, иначе %FALSE.
 */
typedef gboolean     (*hyscan_param_controller_set)           (const gchar            *name,
                                                               GVariant               *value,
                                                               gpointer                user_data);

/**
 * hyscan_param_controller_get:
 * @name: название параметра
 * @user_data: пользовательские данные
 *
 * Функция считывает значение параметра.
 *
 * Returns: Значение параметра. Для удаления #g_variant_unref.
 */
typedef GVariant *   (*hyscan_param_controller_get)           (const gchar            *name,
                                                               gpointer                user_data);


typedef struct _HyScanParamController HyScanParamController;
typedef struct _HyScanParamControllerPrivate HyScanParamControllerPrivate;
typedef struct _HyScanParamControllerClass HyScanParamControllerClass;

struct _HyScanParamController
{
  GObject parent_instance;

  HyScanParamControllerPrivate *priv;
};

struct _HyScanParamControllerClass
{
  GObjectClass parent_class;
};

HYSCAN_API
GType                  hyscan_param_controller_get_type          (void);

HYSCAN_API
HyScanParamController *hyscan_param_controller_new               (GMutex                      *lock);

HYSCAN_API
void                   hyscan_param_controller_set_schema        (HyScanParamController       *controller,
                                                                  HyScanDataSchema            *schema);

HYSCAN_API
gboolean               hyscan_param_controller_add_boolean       (HyScanParamController       *controller,
                                                                  const gchar                 *name,
                                                                  gboolean                    *value);

HYSCAN_API
gboolean               hyscan_param_controller_add_integer       (HyScanParamController       *controller,
                                                                  const gchar                 *name,
                                                                  gint64                      *value);

HYSCAN_API
gboolean               hyscan_param_controller_add_double        (HyScanParamController       *controller,
                                                                  const gchar                 *name,
                                                                  gdouble                     *value);

HYSCAN_API
gboolean               hyscan_param_controller_add_string        (HyScanParamController       *controller,
                                                                  const gchar                 *name,
                                                                  GString                     *value);

HYSCAN_API
gboolean               hyscan_param_controller_add_enum          (HyScanParamController       *controller,
                                                                  const gchar                 *name,
                                                                  gint64                      *value);

HYSCAN_API
gboolean               hyscan_param_controller_add_user          (HyScanParamController       *controller,
                                                                  const gchar                 *name,
                                                                  hyscan_param_controller_set  set_fn,
                                                                  hyscan_param_controller_get  get_fn,
                                                                  gpointer                     user_data);

G_END_DECLS

#endif /* __HYSCAN_PARAM_CONTROLLER_H__ */
