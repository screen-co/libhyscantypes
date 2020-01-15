/* buffer-test.c
 *
 * Copyright 2019 Screen LLC, Alexander Dmitriev <m1n7@yandex.ru>
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
#include <hyscan-cancellable.h>
#include <glib/gprintf.h>

static gfloat prev;

gfloat
get (HyScanCancellable *c,
     gfloat             a,
     gfloat             b)
{
  gfloat value;

  value = hyscan_cancellable_get (c);
  g_message ("%.16f", value);
  {
    if (value - prev > 0.0)
      {
        hyscan_cancellable_set (c, a, b);
        hyscan_cancellable_get (c);
      }
  }
  prev = value;
  return 100. * value;
}

gfloat
sget (HyScanCancellable *c,
      gfloat             a,
      gfloat             b)
{
  hyscan_cancellable_set (c, a, b);

  return get(c,a,b);
}


void
func (HyScanCancellable *c,
      gint               depth)
{
  if (depth < 0)
    return;

  hyscan_cancellable_push (c);
  sget (c, 0., 1/3.);
  func (c, depth - 1);
  sget (c, 1/3., 2/3.);
  func (c, depth - 1);
  sget (c, 2/3., 1.);
  func (c, depth - 1);
  hyscan_cancellable_pop (c);
}

void
func2 (HyScanCancellable *c,
       gint               depth)
{
  hyscan_cancellable_push (c);

  if (depth < 2)
    {
      func (c, depth - 1);
      func (c, depth - 1);
      func (c, depth - 1);
    }
  else
    {
      func2 (c, depth - 1);
      func2 (c, depth - 1);
      func2 (c, depth - 1);
    }
  hyscan_cancellable_pop (c);
}

int
main (int argc, char const *argv[])
{
  HyScanCancellable *cancellable = hyscan_cancellable_new ();

  func2 (cancellable, 3);

  g_object_unref (cancellable);
  return 0;
}
