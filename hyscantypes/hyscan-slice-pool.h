/**
 * \file hyscan-slice-pool.h
 *
 * \brief Заголовочный файл списка неиспользуемых блоков памяти.
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2016
 * \license Проприетарная лицензия ООО "Экран"
 *
 * \defgroup HyScanSlicePool HyScanSlicePool - список неиспользуемых блоков памяти
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
 * типа HyScanSlicePool, предварительно инициализированную значением NULL,
 * в функцию #hyscan_slice_pool_push.
 *
 * Достать блок памяти из списка можно с помощью функции #hyscan_slice_pool_pop.
 *
 */

#ifndef __HYSCAN_SLICE_POOL_H__
#define __HYSCAN_SLICE_POOL_H__

#include <glib.h>
#include <hyscan-api.h>

G_BEGIN_DECLS

typedef struct _HyScanSlicePool HyScanSlicePool;

/**
 *
 * Функция помещает неиспользуемый блок памяти в список.
 *
 * \param pool указатель на список блоков \link HyScanSlicePool \endlink;
 * \param slice блок памяти.
 *
 * \return Нет.
 *
 */
HYSCAN_API
void           hyscan_slice_pool_push          (HyScanSlicePool      **pool,
                                                gpointer               slice);

/**
 *
 * Функция извлекает один блок памяти из списка. Если список пустой,
 * функция вернёт значение NULL.
 *
 * \param pool указатель на список блоков \link HyScanSlicePool \endlink.
 *
 * \return Указатель на блок памяти или NULL.
 *
 */
HYSCAN_API
gpointer       hyscan_slice_pool_pop           (HyScanSlicePool      **pool);

G_END_DECLS

#endif /* __HYSCAN_SLICE_POOL_H__ */
