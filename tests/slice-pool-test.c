/* slice-pool-test.c
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

#include <hyscan-slice-pool.h>

int
main (int    argc,
      char **argv)
{
  HyScanSlicePool *pool = NULL;
  gint32 *data;

  gint block_size = 65536 / sizeof (guint32);
  gint n_blocks = 4096;
  gint i, j;

  /* Выделяем память, формируем шаблон теста и помещаем в список. */
  for (i = 0; i < n_blocks; i++)
    {
      data = g_new0 (gint32, block_size);

      for (j = 2; j < block_size; j++)
        data[j] = i + j;

      hyscan_slice_pool_push (&pool, data);
    }

  for (i = n_blocks - 1; i >= 0; i--)
    {
      data = hyscan_slice_pool_pop (&pool);
      if (data == NULL)
        g_error ("can't pop data block %d", i);

      for (j = 2; j < block_size; j++)
        if (data[j] != (i + j))
          g_error ("data mismatch at block %d index %d", i, j);

      g_free (data);
    }

  if (hyscan_slice_pool_pop (&pool) != NULL)
    g_error ("pool isn't empty");

  g_message ("All done");

  return 0;
}
