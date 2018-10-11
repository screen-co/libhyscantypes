/* channel-name-test.c
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

#include <hyscan-types.h>

int
main (int    argc,
      char **argv)
{
  HyScanSourceType source_in, source_out;
  HyScanChannelType type_in, type_out;
  guint index_in, index_out;

  for (source_in = HYSCAN_SOURCE_LOG; source_in < HYSCAN_SOURCE_LAST; source_in++)
    {
      g_message ("Checking source %s.", hyscan_source_get_name_by_type (source_in));

      for (type_in = HYSCAN_CHANNEL_DATA; type_in < HYSCAN_CHANNEL_LAST; type_in++)
        {
          for (index_in = 1; index_in <= 16; index_in++)
            {
              const gchar *name;

              name = hyscan_channel_get_name_by_types (source_in, type_in, index_in);
              if (name == NULL)
                g_error ("can't get channel name for source %s", hyscan_source_get_name_by_type (source_in));

              if (!hyscan_channel_get_types_by_name (name, &source_out, &type_out, &index_out))
                g_error ("can't parse channel name %s", name);

              if (source_in == HYSCAN_SOURCE_LOG)
                {
                  type_out = type_in;
                  index_out = index_in;
                }

              if (hyscan_source_is_sensor (source_in))
                {
                  type_out = type_in;
                }

              if ((source_out != source_in) ||
                  (type_out != type_in) ||
                  (index_out != index_in))
                {
                  g_error ("error in channel name %s", name);
                }
            }
        }
    }

  g_message ("All done");

  return 0;
}
