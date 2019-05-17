/* buffer-test.c
 *
 * Copyright 2017-2018 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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

#include <hyscan-buffer.h>
#include <math.h>

#define N_POINTS 10000000

typedef struct _test_info test_info;
struct _test_info
{
  const gchar    *name;
  HyScanDataType  type;
  gdouble         min_value;
  gdouble         max_value;
  gdouble         max_error;
};

test_info float_test_info [] =
{
  { "HYSCAN_DATA_FLOAT",               HYSCAN_DATA_FLOAT,                0.0, 1.0, 0.0 },
  { "HYSCAN_DATA_ADC14LE",             HYSCAN_DATA_ADC14LE,             -1.0, 1.0, 2.0 / 16384.0 },
  { "HYSCAN_DATA_ADC16LE",             HYSCAN_DATA_ADC16LE,             -1.0, 1.0, 2.0 / 65536.0 },
  { "HYSCAN_DATA_ADC24LE",             HYSCAN_DATA_ADC24LE,             -1.0, 1.0, 4.0 / 16777216.0 },
  { "HYSCAN_DATA_FLOAT16LE",           HYSCAN_DATA_FLOAT16LE,           -1.0, 1.0, 1.0 / 2048.0 },
  { "HYSCAN_DATA_FLOAT32LE",           HYSCAN_DATA_FLOAT32LE,           -1.0, 1.0, 0.0 },
  { "HYSCAN_DATA_AMPLITUDE_INT8",      HYSCAN_DATA_AMPLITUDE_INT8,       0.0, 1.0, 1.0 / 256.0 },
  { "HYSCAN_DATA_AMPLITUDE_INT16LE",   HYSCAN_DATA_AMPLITUDE_INT16LE,    0.0, 1.0, 1.0 / 16384.0 },
  { "HYSCAN_DATA_AMPLITUDE_INT24LE",   HYSCAN_DATA_AMPLITUDE_INT24LE,    0.0, 1.0, 2.0 / 16777216.0 },
  { "HYSCAN_DATA_AMPLITUDE_INT32LE",   HYSCAN_DATA_AMPLITUDE_INT32LE,    0.0, 1.0, 2.0 / 4294967296.0 },
  { "HYSCAN_DATA_AMPLITUDE_FLOAT16LE", HYSCAN_DATA_AMPLITUDE_FLOAT16LE,  0.0, 1.0, 1.0 / 2048.0 },
  { "HYSCAN_DATA_AMPLITUDE_FLOAT32LE", HYSCAN_DATA_AMPLITUDE_FLOAT32LE,  0.0, 1.0, 0.0 }
};

test_info complex_float_test_info [] =
{
  { "HYSCAN_DATA_COMPLEX_FLOAT",       HYSCAN_DATA_COMPLEX_FLOAT,        0.0, 1.0, 0.0 },
  { "HYSCAN_DATA_COMPLEX_ADC14LE",     HYSCAN_DATA_COMPLEX_ADC14LE,     -1.0, 1.0, 4.0 / 16384.0 },
  { "HYSCAN_DATA_COMPLEX_ADC16LE",     HYSCAN_DATA_COMPLEX_ADC16LE,     -1.0, 1.0, 4.0 / 65536.0 },
  { "HYSCAN_DATA_COMPLEX_ADC24LE",     HYSCAN_DATA_COMPLEX_ADC24LE,     -1.0, 1.0, 5.0 / 16777216.0 },
  { "HYSCAN_DATA_COMPLEX_FLOAT16LE",   HYSCAN_DATA_COMPLEX_FLOAT16LE,   -1.0, 1.0, 1.0 / 2048.0 },
  { "HYSCAN_DATA_COMPLEX_FLOAT32LE",   HYSCAN_DATA_COMPLEX_FLOAT32LE,   -1.0, 1.0, 0.0 }
};

test_info doa_test_info [] =
{
  { "HYSCAN_DATA_DOA",                 HYSCAN_DATA_DOA,                 -1.0, 1.0, 0.0 },
  { "HYSCAN_DATA_DOA_FLOAT32LE",       HYSCAN_DATA_DOA_FLOAT32LE,       -1.0, 1.0, 0.0 }
};

int
main (int    argc,
      char **argv)
{
  HyScanBuffer *in;
  HyScanBuffer *out;
  HyScanBuffer *raw;
  HyScanBuffer *copy;
  HyScanBuffer *wrapper;

  gfloat *float_data_in;
  gfloat *float_data_out;
  HyScanComplexFloat *complex_float_data_in;
  HyScanComplexFloat *complex_float_data_out;
  HyScanDOA *doa_data_in;
  HyScanDOA *doa_data_out;

  HyScanDataType copy_type;
  gpointer copy_data;
  guint32 copy_size;

  guint32 n_points;
  guint32 i, j;

  in = hyscan_buffer_new ();
  out = hyscan_buffer_new ();
  raw = hyscan_buffer_new ();
  copy = hyscan_buffer_new ();
  wrapper = hyscan_buffer_new ();

  /* Тест форматов действительных данных. */
  for (i = 0; i < sizeof (float_test_info) / sizeof (test_info); i++)
    {
      g_message ("Testing %s data format.", float_test_info[i].name);

      /* Подготовка тестовых данных. */
      hyscan_buffer_set_float (in, NULL, N_POINTS);
      float_data_in = hyscan_buffer_get_float (in, &n_points);
      if ((float_data_in == NULL) || (n_points != N_POINTS))
        g_error ("input size error");

      for (j = 0; j < N_POINTS; j++)
        {
          float_data_in[j] = float_test_info[i].max_value - float_test_info[i].min_value;
          float_data_in[j] = (float_data_in[j] / (N_POINTS - 1)) * j;
          float_data_in[j] = float_data_in[j] + float_test_info[i].min_value;
        }

      /* Экспорт и импорт данных через выбранный формат. */
      if (!hyscan_buffer_export (in, raw, float_test_info[i].type))
        g_error ("can't export data");

      hyscan_buffer_copy (copy, raw);
      copy_data = hyscan_buffer_get (copy, &copy_type, &copy_size);
      if (copy_size != N_POINTS * hyscan_data_get_point_size (float_test_info[i].type))
        g_error ("data size mismatch %d %d", copy_size, N_POINTS * hyscan_data_get_point_size (float_test_info[i].type));

      hyscan_buffer_wrap (wrapper, copy_type, copy_data, copy_size);
      if (!hyscan_buffer_import (out, wrapper))
        g_error ("can't import data");

      /* Импортированые данные. */
      float_data_out = hyscan_buffer_get_float (out, &n_points);
      if ((float_data_out == NULL) || (n_points != N_POINTS))
        g_error ("output size error %d %d", n_points, N_POINTS);

      /* Проверка данных. */
      for (j = 0; j < N_POINTS; j++)
        if (fabs (float_data_in[j] - float_data_out[j]) > float_test_info[i].max_error)
          g_error ("data error at %d: %.12f %.12f", j, float_data_in[j], float_data_out[j]);
    }

  /* Тест форматов комплексных данных. */
  for (i = 0; i < sizeof (complex_float_test_info) / sizeof (test_info); i++)
    {
      g_message ("Testing %s data format.", complex_float_test_info[i].name);

      /* Подготовка тестовых данных. */
      hyscan_buffer_set_complex_float (in, NULL, N_POINTS);
      complex_float_data_in = hyscan_buffer_get_complex_float (in, &n_points);
      if ((complex_float_data_in == NULL) || (n_points != N_POINTS))
        g_error ("input size error");

      for (j = 0; j < N_POINTS; j++)
        {
          gdouble value;

          value = complex_float_test_info[i].max_value - complex_float_test_info[i].min_value;
          value = (value / (N_POINTS - 1)) * j;
          value = value + complex_float_test_info[i].min_value;

          complex_float_data_in[j].re = value;
          complex_float_data_in[j].im = value / 2.0;
        }

      /* Экспорт и импорт данных через выбранный формат. */
      if (!hyscan_buffer_export (in, raw, complex_float_test_info[i].type))
        g_error ("can't export data");

      hyscan_buffer_copy (copy, raw);
      copy_data = hyscan_buffer_get (copy, &copy_type, &copy_size);
      if (copy_size != N_POINTS * hyscan_data_get_point_size (complex_float_test_info[i].type))
        g_error ("data size mismatch");

      hyscan_buffer_wrap (wrapper, copy_type, copy_data, copy_size);
      if (!hyscan_buffer_import (out, wrapper))
        g_error ("can't import data");

      /* Импортированые данные. */
      complex_float_data_out = hyscan_buffer_get_complex_float (out, &n_points);
      if ((complex_float_data_out == NULL) || (n_points != N_POINTS))
        g_error ("output size error");

      /* Проверка данных. */
      for (j = 0; j < N_POINTS; j++)
        if ((fabs (complex_float_data_in[j].re - complex_float_data_out[j].re) > complex_float_test_info[i].max_error) ||
            (fabs (complex_float_data_in[j].im - complex_float_data_out[j].im) > complex_float_test_info[i].max_error))
          {
            g_error ("data error at %d: (%.12f %.12f) (%.12f %.12f)", i,
                     complex_float_data_in[j].re, complex_float_data_in[j].im,
                     complex_float_data_out[j].re, complex_float_data_out[j].im);
          }
    }

  /* Тест форматов пространственных данных. */
  for (i = 0; i < sizeof (doa_test_info) / sizeof (test_info); i++)
    {
      g_message ("Testing %s data format.", doa_test_info[i].name);

      /* Подготовка тестовых данных. */
      hyscan_buffer_set_doa (in, NULL, N_POINTS);
      doa_data_in = hyscan_buffer_get_doa (in, &n_points);
      if ((doa_data_in == NULL) || (n_points != N_POINTS))
        g_error ("input size error");

      for (j = 0; j < N_POINTS; j++)
        {
          gdouble value;

          value = doa_test_info[i].max_value - doa_test_info[i].min_value;
          value = (value / (N_POINTS - 1)) * j;
          value = value + doa_test_info[i].min_value;

          doa_data_in[j].angle = value;
          doa_data_in[j].distance = value / 2.0;
          doa_data_in[j].amplitude = value / 3.0;
        }

      /* Экспорт и импорт данных через выбранный формат. */
      if (!hyscan_buffer_export (in, raw, doa_test_info[i].type))
        g_error ("can't export data");

      hyscan_buffer_copy (copy, raw);
      copy_data = hyscan_buffer_get (copy, &copy_type, &copy_size);
      if (copy_size != N_POINTS * hyscan_data_get_point_size (doa_test_info[i].type))
        g_error ("data size mismatch");

      hyscan_buffer_wrap (wrapper, copy_type, copy_data, copy_size);
      if (!hyscan_buffer_import (out, wrapper))
        g_error ("can't import data");

      /* Импортированые данные. */
      doa_data_out = hyscan_buffer_get_doa (out, &n_points);
      if ((doa_data_out == NULL) || (n_points != N_POINTS))
        g_error ("output size error");

      /* Проверка данных. */
      for (j = 0; j < N_POINTS; j++)
        if ((fabs (doa_data_in[j].angle - doa_data_out[j].angle) > doa_test_info[i].max_error) ||
            (fabs (doa_data_in[j].distance - doa_data_out[j].distance) > doa_test_info[i].max_error) ||
            (fabs (doa_data_in[j].amplitude - doa_data_out[j].amplitude) > doa_test_info[i].max_error))
          {
            g_error ("data error at %d: (%.12f %.12f %.12f) (%.12f %.12f %.12f)", i,
                     doa_data_in[j].angle, doa_data_in[j].distance,doa_data_in[j].amplitude,
                     doa_data_out[j].angle, doa_data_out[j].distance, doa_data_out[j].amplitude);
          }
    }

  g_object_unref (in);
  g_object_unref (out);
  g_object_unref (raw);
  g_object_unref (copy);
  g_object_unref (wrapper);

  g_message ("All done.");

  return 0;
}
