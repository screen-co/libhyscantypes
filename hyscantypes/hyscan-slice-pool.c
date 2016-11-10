/*
 * \file hyscan-slice-pool.c
 *
 * \brief Исходный файл списка неиспользуемых блоков памяти.
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2016
 * \license Проприетарная лицензия ООО "Экран"
 *
 */

#include "hyscan-slice-pool.h"

struct _HyScanSlicePool
{
  HyScanSlicePool *next;
};

/* Функция помещает неиспользуемый блок памяти в список. */
void
hyscan_slice_pool_push (HyScanSlicePool **pool,
                        gpointer          slice)
{
  HyScanSlicePool *data = (HyScanSlicePool*)slice;

  data->next = *pool;
  *pool = data;
}

/* Функция извлекает один блок памяти из списка. */
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
