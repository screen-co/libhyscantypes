/* hyscan-cancellable.c
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

/**
 * SECTION: hyscan-cancellable
 * @Title HyScanCancellable
 * @Short_description GCancellable с процентом выполнения.
 *
 * HyScanCancellable расширяет GCancellable, добавляя к нему процент выполнения.
 * Так как операции могут состоять из подопераций, значения хранятся в стеке,
 * позволяя дробить каждый шаг на подшаги незаметно для вызывающей стороны.
 *
 * Так как это чисто информационный класс (например, для отрисовки в интерфейсе
 * прогресс-бара), большая точность не предполагается. Поэтому размер этого
 * стека может быть (и это действительно так) ограничен для экономии памяти.
 * На превышающих глубинах этого стека вызовы отработают, но будут тихо
 * проигнорированы.
 */

#include "hyscan-cancellable.h"

#define N_LEVELS (8 /* Размер стека. */ )
#define MULTIPLIER (1e8 /* Множитель. */ )

/*
 * HACKING
 *
 * Принцип работы.
 * Рассмотрим вот такой пример:
 *
 * | index        |  0 |    1    |        2        |
 * |--------------|:--:|:-------:|:---------------:|
 * | .current     | .2 |   .5    |       .7        |
 * | .next        | .3 |   .6    |       1.0       |
 * |--------------|----|---------|-----------------|
 * | result       | .2 |   .25   |      .257       |
 * |--------------|----|---------|-----------------|
 * | .multiplier  |  1 | (30-20) | (30-20)*(60-50) |
 * | .total       |  0 |    20   |        25       |
 *
 * Поле .next нужно, чтобы следующий уровни мог определить, сколько он выполнил
 * % от предыдущего шага. По сути, математика такая:
 * result = .total[i] + .multiplier[i] * .current[i], где .total и .multiplier
 * зависят от предыдущих шагов.
 *
 * ---
 *
 * Проверка типов.
 * Я не знаю, кто и как собирается использовать мой класс. Я не исключаю того,
 * что случайно (или автоматически глибом) мне будет передан GCancellable вместо
 * HyScanCancellable. Проверка NULL сделана аналогично GCancellable. Итого:
 * NULL ? return :
 * HYSCAN_CANCELLABLE ? work :
 * G_CANCELLBALE ? return : warning;
 */

#define HYSCAN_CANCELLABLE_CHECK_TYPE(object)                                  \
  if ((object) == NULL)                                                        \
    return;                                                                    \
  g_return_if_fail (G_IS_CANCELLABLE (object));                                \
  if (!HYSCAN_IS_CANCELLABLE (object))                                         \
    return;

#define HYSCAN_CANCELLABLE_CHECK_TYPE_WITH_VAL(object, val)                    \
  if ((object) == NULL)                                                        \
    return val;                                                                \
  g_return_val_if_fail (G_IS_CANCELLABLE (object), val);                       \
  if (!HYSCAN_IS_CANCELLABLE (object))                                         \
    return val;

typedef struct
{
  gfloat  current;     /* Текущий % выполнения. */
  gfloat  next;        /* Следующий % выполнения. */

  gfloat  multiplier;  /* Множитель этого шага. */
  gfloat  total;       /* Сумма % всех предыдущих шагов. */
} HyScanCancellablePercents;

struct _HyScanCancellablePrivate
{
  HyScanCancellablePercents  values[N_LEVELS]; /* Массив значений. */

  gint                       index;            /* Текущий уровень влож-ти. */
  gint                       current;          /* Текущее значение. */
  gboolean                   freeze;           /* "Заморозка". */
};

G_DEFINE_TYPE_WITH_PRIVATE (HyScanCancellable, hyscan_cancellable, G_TYPE_CANCELLABLE);

static void
hyscan_cancellable_class_init (HyScanCancellableClass *klass)
{
}

static void
hyscan_cancellable_init (HyScanCancellable *cancellable)
{
  HyScanCancellablePrivate *priv;

  priv = hyscan_cancellable_get_instance_private (cancellable);
  cancellable->priv = priv;

  /* Если множитель будет ноль -- ничего не взлетит. */
  priv->values[0].multiplier = 1.0;
  priv->values[0].total = 0.0;
  hyscan_cancellable_set (cancellable, 0.0, 1.0);
}

/**
 * hyscan_cancellable_new:
 * Создает новый HyScanCancellable.
 * Returns: (transfer full): HyScanCancellable
 */
HyScanCancellable *
hyscan_cancellable_new (void)
{
  return g_object_new (HYSCAN_TYPE_CANCELLABLE, NULL);
}

/**
 * hyscan_cancellable_set:
 * @cancellable: HyScanCancellable
 * @current: текущее значение прогресса (0.0 -- 1.0)
 * @next: следующее значение (0.0 -- 1.0)
 *
 * Задает значения на текущем уровне и пересчитывает общий прогресс.
 */
void
hyscan_cancellable_set (HyScanCancellable *cancellable,
                        gfloat             current,
                        gfloat             next)
{
  gint perc;
  HyScanCancellablePrivate *priv;
  HyScanCancellablePercents *link;

  HYSCAN_CANCELLABLE_CHECK_TYPE (cancellable);
  priv = cancellable->priv;

  /* Если слишком большая глубина -- просто выходим. */
  if (priv->index >= N_LEVELS)
    return;

  /* Если заморожено -- выходим. */
  if (priv->freeze)
    return;

  /* Иначе обновляем значения и пересчитываем общий прогресс. */
  link = &priv->values[priv->index];

  link->current = current;
  link->next = next;

  perc = (link->total + link->multiplier * link->current) * MULTIPLIER;
  g_atomic_int_set (&priv->current, perc);
}

/**
 * hyscan_cancellable_set_total:
 * @cancellable: HyScanCancellable
 * @current: номер текущего шага
 * @start: номер начального шага
 * @end: номер конечного шага
 *
 * Вспомогательная функция для задания прогресса не в процентах, а как номер
 * текущего шага. Например, если обрабатываются определенные индексы.
 *
 * for (i = 10; i < 20; ++i)
 *   hyscan_cancellable_set_total (canc, i, 10, 20);
 */
void
hyscan_cancellable_set_total (HyScanCancellable *cancellable,
                              gfloat             current,
                              gfloat             start,
                              gfloat             end)
{
  hyscan_cancellable_set (cancellable,
                          (current - start) / (end - start),
                          (current - start + 1) / (end - start));
}
/**
 * hyscan_cancellable_get:
 * @cancellable: HyScanCancellable
 *
 * Возвращает общий прогресс.
 *
 * Returns: общий прогресс.
 */
gfloat
hyscan_cancellable_get (HyScanCancellable *cancellable)
{
  gint current;

  /* Cм HACKING выше, почему так сделано. */
  HYSCAN_CANCELLABLE_CHECK_TYPE_WITH_VAL (cancellable, 0.0);

  current = g_atomic_int_get (&cancellable->priv->current);
  return current / MULTIPLIER;
}

/**
 * hyscan_cancellable_push:
 * @cancellable: HyScanCancellable
 *
 * Добавляет один уровень вложенности.
 */
void
hyscan_cancellable_push (HyScanCancellable *cancellable)
{
  HyScanCancellablePrivate *priv;
  gint i;

  /* Cм HACKING выше, почему так сделано. */
  HYSCAN_CANCELLABLE_CHECK_TYPE (cancellable);
  priv = cancellable->priv;

  /* Для слишком глубоких уровней значения реально не обновляем,
   * но индекс запомнить надо (чтобы pop отрабатывал нормально). */
  if (priv->index >= N_LEVELS - 1)
    {
      priv->index++;
      return;
    }

  i = priv->index;

  priv->values[i + 1].total = priv->values[i].total +
                              priv->values[i].current *
                              priv->values[i].multiplier;
  priv->values[i + 1].multiplier = priv->values[i].multiplier *
                                   (priv->values[i].next -
                                    priv->values[i].current);

  priv->index++;

  hyscan_cancellable_set (cancellable, 0.0, 1.0);
}

/**
 * hyscan_cancellable_pop:
 * @cancellable: HyScanCancellable
 *
 * Убирает один уровень вложенности.
 */
void
hyscan_cancellable_pop (HyScanCancellable *cancellable)
{
  HyScanCancellablePrivate *priv;

  HYSCAN_CANCELLABLE_CHECK_TYPE (cancellable);
  priv = cancellable->priv;

  /* Я не большой фанат выходов за границы массивов,
   * а ещё это конкретная ошибка программиста. */
  g_return_if_fail (priv->index >= 0);

  priv->index--;
  hyscan_cancellable_set (cancellable,
                          priv->values[priv->index].next,
                          priv->values[priv->index].next);
}

/**
 * hyscan_cancellable_freeze:
 * @cancellable: HyScanCancellable
 *
 * "Размораживает" объект. См. hyscan_cancellable_freeze()
 */
void
hyscan_cancellable_freeze (HyScanCancellable *cancellable)
{
  HYSCAN_CANCELLABLE_CHECK_TYPE (cancellable);
  cancellable->priv->freeze = TRUE;
}

/**
 * hyscan_cancellable_unfreeze:
 * @cancellable: HyScanCancellable
 *
 * "Замораживает" объект. Это значит, что последующие вызовы
 * hyscan_cancellable_set() не будут приводить к изменению процента выполнения.
 * Удобно использовать, когда лень задавать промежуточные значения.
 */
void
hyscan_cancellable_unfreeze (HyScanCancellable *cancellable)
{
  /* Cм HACKING выше, почему так сделано. */
  if (cancellable == NULL)
    return;
  g_return_if_fail (G_IS_CANCELLABLE (cancellable));
  if (!HYSCAN_IS_CANCELLABLE (cancellable))
    return;

  cancellable->priv->freeze = FALSE;
}
