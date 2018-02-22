/* buffer-test.c
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
 * HyScan is distributed in the hope that it will be useful,
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
  { "HYSCAN_DATA_FLOAT",             HYSCAN_DATA_FLOAT,              0.0, 1.0, 0.0 },
  { "HYSCAN_DATA_ADC_14LE",          HYSCAN_DATA_ADC_14LE,          -1.0, 1.0, 2.0 / 16384.0 },
  { "HYSCAN_DATA_ADC_16LE",          HYSCAN_DATA_ADC_16LE,          -1.0, 1.0, 2.0 / 65536.0 },
  { "HYSCAN_DATA_ADC_24LE",          HYSCAN_DATA_ADC_24LE,          -1.0, 1.0, 2.0 / 16777216.0 },
  { "HYSCAN_DATA_AMPLITUDE_INT8",    HYSCAN_DATA_AMPLITUDE_INT8,     0.0, 1.0, 1.0 / 256.0 },
  { "HYSCAN_DATA_AMPLITUDE_INT16",   HYSCAN_DATA_AMPLITUDE_INT16,    0.0, 1.0, 1.0 / 16384.0 },
  { "HYSCAN_DATA_AMPLITUDE_INT32",   HYSCAN_DATA_AMPLITUDE_INT32,    0.0, 1.0, 2.0 / 16777216.0 },
  { "HYSCAN_DATA_AMPLITUDE_FLOAT8",  HYSCAN_DATA_AMPLITUDE_FLOAT8,   0.0, 1.0, 2.0 / 64.0 },
  { "HYSCAN_DATA_AMPLITUDE_FLOAT16", HYSCAN_DATA_AMPLITUDE_FLOAT16,  0.0, 1.0, 2.0 / 8192.0 }
};

test_info complex_float_test_info [] =
{
  { "HYSCAN_DATA_COMPLEX_FLOAT",     HYSCAN_DATA_COMPLEX_FLOAT,     0.0, 1.0, 0.0 },
  { "HYSCAN_DATA_COMPLEX_ADC_14LE",  HYSCAN_DATA_COMPLEX_ADC_14LE, -1.0, 1.0, 4.0 / 16384.0 },
  { "HYSCAN_DATA_COMPLEX_ADC_16LE",  HYSCAN_DATA_COMPLEX_ADC_16LE, -1.0, 1.0, 4.0 / 65536.0 },
  { "HYSCAN_DATA_COMPLEX_ADC_24LE",  HYSCAN_DATA_COMPLEX_ADC_24LE, -1.0, 1.0, 4.0 / 16777216.0 }
};

int
main (int    argc,
      char **argv)
{
  HyScanBuffer *in;
  HyScanBuffer *out;
  HyScanBuffer *raw;
  HyScanBuffer *wrapper;

  gfloat *float_data_in;
  gfloat *float_data_out;
  HyScanComplexFloat *complex_float_data_in;
  HyScanComplexFloat *complex_float_data_out;

  gpointer wrapped_data;
  gpointer raw_data;
  guint32 raw_size;

  guint32 n_points;
  guint32 i, j;

  in = hyscan_buffer_new ();
  out = hyscan_buffer_new ();
  raw = hyscan_buffer_new ();
  wrapper = hyscan_buffer_new ();

  /* Тест форматов действительных данных. */
  for (i = 0; i < sizeof (float_test_info) / sizeof (test_info); i++)
    {
      g_message ("Testing %s data format.", float_test_info[i].name);

      /* Подготовка тестовых данных. */
      hyscan_buffer_set_data_type (in, HYSCAN_DATA_FLOAT);
      hyscan_buffer_set_size (in, N_POINTS * sizeof (gfloat));
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
      if (!hyscan_buffer_export_data (in, raw, float_test_info[i].type))
        g_error ("can't export data");

      raw_data = hyscan_buffer_get_data (raw, &raw_size);
      wrapped_data = g_memdup (raw_data, raw_size);
      hyscan_buffer_wrap_data (wrapper, float_test_info[i].type,
                               wrapped_data, raw_size);

      if (!hyscan_buffer_import_data (out, wrapper))
        g_error ("can't import data");

      g_free (wrapped_data);

      /* Импортированые данные. */
      float_data_out = hyscan_buffer_get_float (out, &n_points);
      if ((float_data_out == NULL) || (n_points != N_POINTS))
        g_error ("output size error");

      /* Проверка данных. */
      for (j = 0; j < N_POINTS; j++)
        if (fabs (float_data_in[j] - float_data_out[j]) > float_test_info[i].max_error)
          g_error ("data error at %d: %f %f", j, float_data_in[j], float_data_out[j]);
    }

  /* Тест форматов комплексных данных. */
  for (i = 0; i < sizeof (complex_float_test_info) / sizeof (test_info); i++)
    {
      g_message ("Testing %s data format.", complex_float_test_info[i].name);

      /* Подготовка тестовых данных. */
      hyscan_buffer_set_data_type (in, HYSCAN_DATA_COMPLEX_FLOAT);
      hyscan_buffer_set_size (in, N_POINTS * sizeof (HyScanComplexFloat));
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
          complex_float_data_in[j].im = 0.0;
        }

      /* Экспорт и импорт данных через выбранный формат. */
      if (!hyscan_buffer_export_data (in, raw, complex_float_test_info[i].type))
        g_error ("can't export data");

      raw_data = hyscan_buffer_get_data (raw, &raw_size);
      hyscan_buffer_set_data (wrapper, complex_float_test_info[i].type,
                              raw_data, raw_size);

      if (!hyscan_buffer_import_data (out, wrapper))
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
            g_error ("data error at %d: (%f %f) (%f %f)", i,
                     complex_float_data_in[j].re, complex_float_data_in[j].im,
                     complex_float_data_out[j].re, complex_float_data_out[j].im);
          }
    }

  g_object_unref (in);
  g_object_unref (out);
  g_object_unref (raw);
  g_object_unref (wrapper);

  g_message ("All done.");

  return 0;
}
