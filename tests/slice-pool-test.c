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
