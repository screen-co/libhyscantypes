/* hyscan-data-schema-builder.h
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
 * HyScanTypes is distributed in the hope that it will be useful,
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

#ifndef __HYSCAN_DATA_SCHEMA_BUILDER_H__
#define __HYSCAN_DATA_SCHEMA_BUILDER_H__

#include <hyscan-data-schema.h>

G_BEGIN_DECLS

#define HYSCAN_TYPE_DATA_SCHEMA_BUILDER             (hyscan_data_schema_builder_get_type ())
#define HYSCAN_DATA_SCHEMA_BUILDER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HYSCAN_TYPE_DATA_SCHEMA_BUILDER, HyScanDataSchemaBuilder))
#define HYSCAN_IS_DATA_SCHEMA_BUILDER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HYSCAN_TYPE_DATA_SCHEMA_BUILDER))
#define HYSCAN_DATA_SCHEMA_BUILDER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HYSCAN_TYPE_DATA_SCHEMA_BUILDER, HyScanDataSchemaBuilderClass))
#define HYSCAN_IS_DATA_SCHEMA_BUILDER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HYSCAN_TYPE_DATA_SCHEMA_BUILDER))
#define HYSCAN_DATA_SCHEMA_BUILDER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HYSCAN_TYPE_DATA_SCHEMA_BUILDER, HyScanDataSchemaBuilderClass))

typedef struct _HyScanDataSchemaBuilder HyScanDataSchemaBuilder;
typedef struct _HyScanDataSchemaBuilderPrivate HyScanDataSchemaBuilderPrivate;
typedef struct _HyScanDataSchemaBuilderClass HyScanDataSchemaBuilderClass;

struct _HyScanDataSchemaBuilder
{
  GObject parent_instance;

  HyScanDataSchemaBuilderPrivate *priv;
};

struct _HyScanDataSchemaBuilderClass
{
  GObjectClass parent_class;
};

HYSCAN_API
GType                     hyscan_data_schema_builder_get_type            (void);

HYSCAN_API
HyScanDataSchemaBuilder * hyscan_data_schema_builder_new                 (const gchar              *schema_id);

HYSCAN_API
HyScanDataSchemaBuilder * hyscan_data_schema_builder_new_with_gettext    (const gchar              *schema_id,
                                                                          const gchar              *gettext_domain);

HYSCAN_API
gchar *                   hyscan_data_schema_builder_get_data            (HyScanDataSchemaBuilder  *builder);

HYSCAN_API
gchar *                   hyscan_data_schema_builder_get_id              (HyScanDataSchemaBuilder  *builder);

HYSCAN_API
gboolean                  hyscan_data_schema_builder_enum_create         (HyScanDataSchemaBuilder  *builder,
                                                                          const gchar              *enum_id);

HYSCAN_API
gboolean                  hyscan_data_schema_builder_enum_value_create   (HyScanDataSchemaBuilder  *builder,
                                                                          const gchar              *enum_id,
                                                                          gint64                    value,
                                                                          const gchar              *name,
                                                                          const gchar              *description);

HYSCAN_API
gboolean                  hyscan_data_schema_builder_node_set_name       (HyScanDataSchemaBuilder  *builder,
                                                                          const gchar              *path,
                                                                          const gchar              *name,
                                                                          const gchar              *description);

HYSCAN_API
gboolean                  hyscan_data_schema_builder_key_boolean_create  (HyScanDataSchemaBuilder  *builder,
                                                                          const gchar              *key_id,
                                                                          const gchar              *name,
                                                                          const gchar              *description,
                                                                          gboolean                  default_value);

HYSCAN_API
gboolean                  hyscan_data_schema_builder_key_integer_create  (HyScanDataSchemaBuilder  *builder,
                                                                          const gchar              *key_id,
                                                                          const gchar              *name,
                                                                          const gchar              *description,
                                                                          gint64                    default_value);

HYSCAN_API
gboolean                  hyscan_data_schema_builder_key_double_create   (HyScanDataSchemaBuilder  *builder,
                                                                          const gchar              *key_id,
                                                                          const gchar              *name,
                                                                          const gchar              *description,
                                                                          gdouble                   default_value);

HYSCAN_API
gboolean                  hyscan_data_schema_builder_key_string_create   (HyScanDataSchemaBuilder  *builder,
                                                                          const gchar              *key_id,
                                                                          const gchar              *name,
                                                                          const gchar              *description,
                                                                          const gchar              *default_value);

HYSCAN_API
gboolean                  hyscan_data_schema_builder_key_enum_create     (HyScanDataSchemaBuilder  *builder,
                                                                          const gchar              *key_id,
                                                                          const gchar              *name,
                                                                          const gchar              *description,
                                                                          const gchar              *enum_id,
                                                                          gint64                    default_value);

HYSCAN_API
gboolean                  hyscan_data_schema_builder_key_set_view        (HyScanDataSchemaBuilder  *builder,
                                                                          const gchar              *key_id,
                                                                          HyScanDataSchemaViewType  view);

HYSCAN_API
gboolean                  hyscan_data_schema_builder_key_set_access      (HyScanDataSchemaBuilder   *builder,
                                                                          const gchar               *key_id,
                                                                          HyScanDataSchemaKeyAccess  access);

HYSCAN_API
gboolean                  hyscan_data_schema_builder_key_integer_range   (HyScanDataSchemaBuilder  *builder,
                                                                          const gchar              *key_id,
                                                                          gint64                    minimum_value,
                                                                          gint64                    maximum_value,
                                                                          gint64                    value_step);

HYSCAN_API
gboolean                  hyscan_data_schema_builder_key_double_range    (HyScanDataSchemaBuilder  *builder,
                                                                          const gchar              *key_id,
                                                                          gdouble                   minimum_value,
                                                                          gdouble                   maximum_value,
                                                                          gdouble                   value_step);

HYSCAN_API
gboolean                  hyscan_data_schema_builder_schema_join         (HyScanDataSchemaBuilder  *builder,
                                                                          const gchar              *dst_root,
                                                                          HyScanDataSchema         *schema,
                                                                          const gchar              *src_root);

G_END_DECLS

#endif /* __HYSCAN_DATA_SCHEMA_BUILDER_H__ */
