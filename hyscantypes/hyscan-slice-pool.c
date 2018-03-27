/* hyscan-slice-pool.c
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

/**
 * SECTION: hyscan-slice-pool
 * @Short_description: список неиспользуемых блоков памяти
 * @Title: HyScanSlicePool
 *
 * Список предназначен для хранения множества выделенных и не используемых
 * в данный момент блоков памяти. Блоки памяти хранятся в виде связанного
 * списка. Размер блока памяти не может быть меньше чем размер указателя на
 * целевой платформе. Указатель на следующий блок памяти в списке хранится в
 * самом блоке.
 *
 * Рекомендуется хранить в списке блоки памяти одного размера.
 *
 * Для создания нового списка достаточно передать указатель на переменную
 * типа #HyScanSlicePool, предварительно инициализированную значением NULL,
 * в функцию #hyscan_slice_pool_push.
 *
 * Достать блок памяти из списка можно с помощью функции #hyscan_slice_pool_pop.
 */

#include "hyscan-slice-pool.h"

struct _HyScanSlicePool
{
  HyScanSlicePool *next;
};

/**
 * hyscan_slice_pool_push:
 * @pool: указатель на #HyScanSlicePool
 * @slice: блок памяти
 *
 * Функция помещает неиспользуемый блок памяти в список.
 */
void
hyscan_slice_pool_push (HyScanSlicePool **pool,
                        gpointer          slice)
{
  HyScanSlicePool *data = (HyScanSlicePool*)slice;

  data->next = *pool;
  *pool = data;
}

/**
 * hyscan_slice_pool_pop:
 * @pool: указатель на #HyScanSlicePool
 *
 * Функция извлекает один блок памяти из списка.
 *
 * Returns: Указатель на блок памяти или NULL.
 */
gpointer
hyscan_slice_pool_pop (HyScanSlicePool **pool)
{
  HyScanSlicePool *data = *pool;

  if (data != NULL)
    {
      *pool = data->next;

      data->next = NULL;
    }

  return data;
}
