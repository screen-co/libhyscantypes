/* hyscan-buffer.c
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

/**
 * SECTION: hyscan-buffer
 * @Short_description: буфер данных
 * @Title: HyScanBuffer
 *
 * Класс предназначен для хранения блоков данных и преобразования их между
 * различными форматами. Он поддерживает все типы #HyScanDataType, допускающие
 * хранение численных данных. Класс может импортировать действительные данные
 * типа gfloat и комплексные данные типа #HyScanComplexFloat. Кроме этого
 * класс обеспечивает экспорт текущих данных в любой из совместимых типов
 * #HyScanDataType.
 *
 * Создание объекта класс осуществляется функцией #hyscan_buffer_new.
 *
 * Для импорта данных используется функция #hyscan_buffer_import, для
 * экспорта #hyscan_buffer_export.
 *
 * Изменить размер буфера можно с помощью функции #hyscan_buffer_set_size, а
 * тип хранимых в нём данных, с помощью функции #hyscan_buffer_set_data_type.
 * Получить текущий размер буфера можно с помощью функции #hyscan_buffer_get_size,
 * а тип хранимых в нём данных, с помощью функции #hyscan_buffer_get_data_type.
 *
 * Записать данные в буфер можно с помощью функции #hyscan_buffer_set_data.
 * Кроме этого, #HyScanBuffer может являться обёрткой над любым другим блоком
 * данных, для этого используется функция #hyscan_buffer_wrap_data. При этом
 * данные не копируются в буфер, а используется уже выделенный блок памяти.
 *
 * Получить доступ к данным можно с помощью функции #hyscan_buffer_get_data.
 * Пользователь может изменять данные в буфере в пределах установленного размера.
 *
 * Для упрощения работы с данными типа gfloat и #HyScanComplexFloat предназначены
 * вспомогательные функции #hyscan_buffer_wrap_float,
 * #hyscan_buffer_wrap_complex_float, #hyscan_buffer_set_float,
 * #hyscan_buffer_set_complex_float, #hyscan_buffer_get_float
 * и #hyscan_buffer_get_complex_float. Они работают аналогично функциям
 * #hyscan_buffer_wrap_data, #hyscan_buffer_set_data и #hyscan_buffer_get_data,
 * но обеспечивают приведение типов данных.
 */

#include "hyscan-buffer.h"
#include <string.h>

union float32int
{
  gfloat                       value;          /* Значение числа. */
  guint32                      code;           /* 32-х битное представление. */
};

struct _HyScanBufferPrivate
{
  gboolean                     self_allocated; /* Признак выделения памяти. */
  guint32                      allocated_size; /* Размер буфера. */
  HyScanDataType               data_type;      /* Тип данных. */
  guint32                      data_size;      /* Размер данных. */
  gpointer                     data;           /* Данные. */
};

static void                    hyscan_buffer_object_finalize   (GObject       *object);

static void                    hyscan_buffer_setup_table16     (void);

static inline gfloat           hyscan_buffer_decode_adc_14le   (guint16        code);
static inline gfloat           hyscan_buffer_decode_adc_16le   (guint16        code);
static inline gfloat           hyscan_buffer_decode_adc_24le   (guint32        code);
static inline gfloat           hyscan_buffer_decode_int8       (guint8         code);
static inline gfloat           hyscan_buffer_decode_int16le    (guint16        code);
static inline gfloat           hyscan_buffer_decode_int24le    (guint32        code);
static inline gfloat           hyscan_buffer_decode_int32le    (guint32        code);
static inline gfloat           hyscan_buffer_decode_float16le  (guint16        code);
static inline gfloat           hyscan_buffer_decode_float32le  (guint32        code);

static inline guint16          hyscan_buffer_encode_adc_14le   (gfloat         value);
static inline guint16          hyscan_buffer_encode_adc_16le   (gfloat         value);
static inline guint32          hyscan_buffer_encode_adc_24le   (gfloat         value);
static inline guint8           hyscan_buffer_encode_int8       (gfloat         value);
static inline guint16          hyscan_buffer_encode_int16le    (gfloat         value);
static inline guint32          hyscan_buffer_encode_int24le    (gfloat         value);
static inline guint32          hyscan_buffer_encode_int32le    (gfloat         value);
static inline guint16          hyscan_buffer_encode_float16le  (gfloat         value);
static inline guint32          hyscan_buffer_encode_float32le  (gfloat         value);

static guint32                 hyscan_buffer_mantissa16[2048];
static guint32                 hyscan_buffer_exponent16[64];
static guint32                 hyscan_buffer_offset16[64];

static guint16                 hyscan_buffer_shift16[512];
static guint16                 hyscan_buffer_base16[1924];

G_DEFINE_TYPE_WITH_PRIVATE (HyScanBuffer, hyscan_buffer, G_TYPE_OBJECT)

static void
hyscan_buffer_class_init (HyScanBufferClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = hyscan_buffer_object_finalize;

  hyscan_buffer_setup_table16();
}

static void
hyscan_buffer_init (HyScanBuffer *buffer)
{
  buffer->priv = hyscan_buffer_get_instance_private (buffer);

  buffer->priv->self_allocated = TRUE;
}

static void
hyscan_buffer_object_finalize (GObject *object)
{
  HyScanBuffer *buffer = HYSCAN_BUFFER (object);
  HyScanBufferPrivate *priv = buffer->priv;

  if (priv->self_allocated)
    g_free (priv->data);

  G_OBJECT_CLASS (hyscan_buffer_parent_class)->finalize (object);
}

/* Функция вычисляет таблицы для преобразования float32 <-> float16. Сам
 * алгоритм преобразования описан в статье "Fast Half Float Conversions"
 * за авторством Jeroen van der Zijp. */
static void
hyscan_buffer_setup_table16 (void)
{
  guint i;

  /* Таблицы преобразования float16 -> float32. */
  hyscan_buffer_mantissa16[0] = 0;

  for (i = 1; i <= 1023; i++)
    {
      guint m = i << 13;
      guint e=0;
      while (!(m & 0x00800000))
        {
          e-=0x00800000;
          m <<= 1;
        }
      m &= ~0x00800000;
      e += 0x38800000;
      hyscan_buffer_mantissa16[i] = m | e;
    }

  for (i = 1024; i <= 2047; i++)
    hyscan_buffer_mantissa16[i] = 0x38000000 + ((i - 1024) << 13);

  hyscan_buffer_exponent16[0] = 0;
  hyscan_buffer_exponent16[32] = 0x80000000;

  for (i = 1; i <= 30; i++)
    hyscan_buffer_exponent16[i] = i << 23;

  for (i = 33; i <= 62; i++)
    hyscan_buffer_exponent16[i] = 0x80000000 + ((i - 32) << 23);

  hyscan_buffer_exponent16[31] = 0x47800000;
  hyscan_buffer_exponent16[63] = 0xC7800000;

  for (i = 0; i <= 63; i++)
    hyscan_buffer_offset16[i] = 1024;

  hyscan_buffer_offset16[0] = 0;
  hyscan_buffer_offset16[32] = 0;

  /* Таблицы преобразования float32 -> float16. */
  for (i = 0; i < 256; i++)
    {
    gint e = i - 127;
    if (e < -24)
      {
        hyscan_buffer_base16[i|0x000] = 0x0000;
        hyscan_buffer_base16[i|0x100] = 0x8000;
        hyscan_buffer_shift16[i|0x000] = 24;
        hyscan_buffer_shift16[i|0x100] = 24;
      }
    else if (e < -14)
      {
        hyscan_buffer_base16[i|0x000] = (0x0400 >> (-e - 14));
        hyscan_buffer_base16[i|0x100] = (0x0400 >> (-e - 14)) | 0x8000;
        hyscan_buffer_shift16[i|0x000] = -e-1;
        hyscan_buffer_shift16[i|0x100] = -e-1;
    }
    else if (e <= 15)
      {
        hyscan_buffer_base16[i|0x000] = ((e + 15) << 10);
        hyscan_buffer_base16[i|0x100] = ((e + 15) << 10) | 0x8000;
        hyscan_buffer_shift16[i|0x000] = 13;
        hyscan_buffer_shift16[i|0x100] = 13;
      }
    else if(e < 128)
      {
        hyscan_buffer_base16[i|0x000] = 0x7C00;
        hyscan_buffer_base16[i|0x100] = 0xFC00;
        hyscan_buffer_shift16[i|0x000] = 24;
        hyscan_buffer_shift16[i|0x100] = 24;
      }
    else
      {
        hyscan_buffer_base16[i|0x000] = 0x7C00;
        hyscan_buffer_base16[i|0x100] = 0xFC00;
        hyscan_buffer_shift16[i|0x000] = 13;
        hyscan_buffer_shift16[i|0x100] = 13;
      }
    }
}

/* функции декодирования данных. */

static inline gfloat
hyscan_buffer_decode_adc_14le (guint16 code)
{
  static const gfloat scale = 2.0f / 16383.0f;
  code = GUINT16_FROM_LE (code) & 0x3fff;
  return scale * code - 1.0;
}

static inline gfloat
hyscan_buffer_decode_adc_16le (guint16 code)
{
  static const gfloat scale = 2.0f / 65535.0f;
  code = GUINT16_FROM_LE (code);
  return scale * code - 1.0;
}

static inline gfloat
hyscan_buffer_decode_adc_24le (guint32 code)
{
  static const gfloat scale = 2.0f / 16777215.0f;
  code = GUINT32_FROM_LE (code) & 0x00ffffff;
  return scale * code - 1.0;
}

static inline gfloat
hyscan_buffer_decode_int8 (guint8 code)
{
  static const gfloat scale = 1.0f / 255.0f;
  return scale * code;
}

static inline gfloat
hyscan_buffer_decode_int16le (guint16 code)
{
  static const gfloat scale = 1.0f / 65536.0f;
  code = GUINT16_FROM_LE (code);
  return scale * code;
}

static inline gfloat
hyscan_buffer_decode_int24le (guint32 code)
{
  static const gfloat scale = 1.0f / 16777215.0f;
  code = GUINT32_FROM_LE (code) & 0x00ffffff;
  return scale * code;
}

static inline gfloat
hyscan_buffer_decode_int32le (guint32 code)
{
  static const gfloat scale = 1.0f / 4294967295.0f;
  code = GUINT32_FROM_LE (code);
  return scale * code;
}

static inline gfloat
hyscan_buffer_decode_float16le (guint16 code)
{
  union float32int value;

  code = GUINT16_FROM_LE (code);
  value.code  = hyscan_buffer_mantissa16[hyscan_buffer_offset16[code >> 10] + (code & 0x3ff)];
  value.code += hyscan_buffer_exponent16[code>>10];

  return value.value;
}

static inline gfloat
hyscan_buffer_decode_float32le (guint32 code)
{
  union float32int value;

  value.code = GUINT32_FROM_LE (code);

  return value.value;
}

/* Функции кодирования данных. */

static inline guint16
hyscan_buffer_encode_adc_14le (gfloat value)
{
  guint16 code;
  value = 8192.0 * (value + 1.0);
  code = CLAMP (value, 0.0, 16383.0);
  return GUINT16_TO_LE (code);
}

static inline guint16
hyscan_buffer_encode_adc_16le (gfloat value)
{
  guint16 code;
  value = 32768.0 * (value + 1.0);
  code = CLAMP (value, 0.0, 65535.0);
  return GUINT16_TO_LE (code);
}

static inline guint32
hyscan_buffer_encode_adc_24le (gfloat value)
{
  guint32 code;
  value = 8388608.0 * (value + 1.0);
  code = CLAMP (value, 0.0, 16777215.0);
  return GUINT32_TO_LE (code);
}

static inline guint8
hyscan_buffer_encode_int8 (gfloat value)
{
  guint8 code;
  value = 256.0 * value;
  code = CLAMP (value, 0.0, 255.0);
  return code;
}

static inline guint16
hyscan_buffer_encode_int16le (gfloat value)
{
  guint16 code;
  value = 65536.0 * value;
  code = CLAMP (value, 0.0, 65535.0);
  return GUINT16_TO_LE (code);
}

static inline guint32
hyscan_buffer_encode_int24le (gfloat value)
{
  guint32 code;
  value = 16777215.0 * value;
  code = CLAMP (value, 0.0, 16777215.0);
  return GUINT32_TO_LE (code);
}

static inline guint32
hyscan_buffer_encode_int32le (gfloat value)
{
  guint32 code;
  value = 4294967296.0 * value;
  code = CLAMP (value, 0.0, 4294967295.0);
  return GUINT32_TO_LE (code);
}

static inline guint16
hyscan_buffer_encode_float16le (gfloat value)
{
  union float32int code32;
  guint16 code16;

  code32.value = value;

  code16  = hyscan_buffer_base16[(code32.code >> 23) & 0x1ff];
  code16 += (code32.code & 0x007fffff) >> hyscan_buffer_shift16[(code32.code >> 23) & 0x1ff];

  return GUINT16_TO_LE (code16);
}


static inline guint32
hyscan_buffer_encode_float32le (gfloat value)
{
  union float32int code;

  code.value = value;

  return GUINT32_TO_LE (code.code);
}

/**
 * hyscan_buffer_new:
 *
 * Функция создаёт новый объект #HyScanBuffer.
 *
 * Returns: #HyScanBuffer. Для удаления #g_object_unref.
 */
HyScanBuffer *
hyscan_buffer_new (void)
{
  return g_object_new (HYSCAN_TYPE_BUFFER, NULL);
}

/**
 * hyscan_buffer_import_data:
 * @buffer: указатель на #HyScanBuffer
 * @raw: указатель на #HyScanBuffer с данными для импорта
 *
 * Функция импортирует данные из внешнего буфера и преобразовывает их в массив
 * gfloat или #HyScanComplexFloat в зависимости от их типа.
 *
 * Returns: %TRUE если данные успешно импортированы, иначе %FALSE.
 */
gboolean
hyscan_buffer_import_data (HyScanBuffer *buffer,
                           HyScanBuffer *raw)
{
  HyScanComplexFloat *complex_float_buffer = NULL;
  gfloat *float_buffer = NULL;
  HyScanDataType data_type;
  guint32 data_size;
  guint32 n_points;
  gpointer data;
  guint32 i;

  g_return_val_if_fail (HYSCAN_IS_BUFFER (buffer), FALSE);

  /* Размер и тип импортируемых данных. */
  data = hyscan_buffer_get_data (raw, &data_size);
  data_type = hyscan_buffer_get_data_type (raw);

  /* Тип данных: действительные или комплексные. */
  switch (data_type)
    {
    case HYSCAN_DATA_FLOAT:
    case HYSCAN_DATA_ADC14LE:
    case HYSCAN_DATA_ADC16LE:
    case HYSCAN_DATA_ADC24LE:
    case HYSCAN_DATA_FLOAT16LE:
    case HYSCAN_DATA_FLOAT32LE:
    case HYSCAN_DATA_AMPLITUDE_INT8:
    case HYSCAN_DATA_AMPLITUDE_INT16LE:
    case HYSCAN_DATA_AMPLITUDE_INT24LE:
    case HYSCAN_DATA_AMPLITUDE_INT32LE:
    case HYSCAN_DATA_AMPLITUDE_FLOAT16LE:
    case HYSCAN_DATA_AMPLITUDE_FLOAT32LE:
      n_points = data_size / hyscan_data_get_point_size (data_type);
      hyscan_buffer_set_size (buffer, n_points * sizeof (gfloat));
      hyscan_buffer_set_data_type (buffer, HYSCAN_DATA_FLOAT);
      float_buffer = hyscan_buffer_get_float (buffer, &n_points);
      break;

    case HYSCAN_DATA_COMPLEX_FLOAT:
    case HYSCAN_DATA_COMPLEX_ADC14LE:
    case HYSCAN_DATA_COMPLEX_ADC16LE:
    case HYSCAN_DATA_COMPLEX_ADC24LE:
    case HYSCAN_DATA_COMPLEX_FLOAT16LE:
    case HYSCAN_DATA_COMPLEX_FLOAT32LE:
      n_points = data_size / hyscan_data_get_point_size (data_type);
      hyscan_buffer_set_size (buffer, n_points * sizeof (HyScanComplexFloat));
      hyscan_buffer_set_data_type (buffer, HYSCAN_DATA_COMPLEX_FLOAT);
      complex_float_buffer = hyscan_buffer_get_complex_float (buffer, &n_points);
      break;

    default:
      return FALSE;
    }

  /* Импорт данных. */
  switch (data_type)
    {
    case HYSCAN_DATA_FLOAT:
      {
        memcpy (float_buffer, data, n_points * sizeof (gfloat));
      }
      break;

    case HYSCAN_DATA_ADC14LE:
      {
        guint16 *raw_data = data;
        for (i = 0; i < n_points; i++)
          float_buffer[i] = hyscan_buffer_decode_adc_14le (*raw_data++);
      }
      break;

    case HYSCAN_DATA_ADC16LE:
      {
        guint16 *raw_data = data;
        for (i = 0; i < n_points; i++)
          float_buffer[i] = hyscan_buffer_decode_adc_16le (*raw_data++);
      }
      break;

    case HYSCAN_DATA_ADC24LE:
      {
        guint32 *raw_data = data;
        for (i = 0; i < n_points; i++)
          float_buffer[i] = hyscan_buffer_decode_adc_24le (*raw_data++);
      }
      break;

    case HYSCAN_DATA_AMPLITUDE_INT8:
      {
        guint8 *raw_data = data;
        for (i = 0; i < n_points; i++)
          float_buffer[i] = hyscan_buffer_decode_int8 (*raw_data++);
      }
      break;

    case HYSCAN_DATA_AMPLITUDE_INT16LE:
      {
        guint16 *raw_data = data;
        for (i = 0; i < n_points; i++)
          float_buffer[i] = hyscan_buffer_decode_int16le (*raw_data++);
      }
      break;

    case HYSCAN_DATA_AMPLITUDE_INT24LE:
      {
        guint32 *raw_data = data;
        for (i = 0; i < n_points; i++)
          float_buffer[i] = hyscan_buffer_decode_int24le (*raw_data++);
      }
      break;

    case HYSCAN_DATA_AMPLITUDE_INT32LE:
      {
        guint32 *raw_data = data;
        for (i = 0; i < n_points; i++)
          float_buffer[i] = hyscan_buffer_decode_int32le (*raw_data++);
      }
      break;

    case HYSCAN_DATA_FLOAT16LE:
    case HYSCAN_DATA_AMPLITUDE_FLOAT16LE:
      {
        guint16 *raw_data = data;
        for (i = 0; i < n_points; i++)
          float_buffer[i] = hyscan_buffer_decode_float16le (*raw_data++);
      }
      break;

    case HYSCAN_DATA_FLOAT32LE:
    case HYSCAN_DATA_AMPLITUDE_FLOAT32LE:
      {
        guint32 *raw_data = data;
        for (i = 0; i < n_points; i++)
          float_buffer[i] = hyscan_buffer_decode_float32le (*raw_data++);
      }
      break;

    case HYSCAN_DATA_COMPLEX_FLOAT:
      {
        memcpy (complex_float_buffer, data, n_points * sizeof (HyScanComplexFloat));
      }
      break;

    case HYSCAN_DATA_COMPLEX_ADC14LE:
      {
        guint16 *raw_data = data;
        for (i = 0; i < n_points; i++)
          {
            complex_float_buffer[i].re = hyscan_buffer_decode_adc_14le (*raw_data++);
            complex_float_buffer[i].im = hyscan_buffer_decode_adc_14le (*raw_data++);
          }
      }
      break;

    case HYSCAN_DATA_COMPLEX_ADC16LE:
      {
        guint16 *raw_data = data;
        for (i = 0; i < n_points; i++)
          {
            complex_float_buffer[i].re = hyscan_buffer_decode_adc_16le (*raw_data++);
            complex_float_buffer[i].im = hyscan_buffer_decode_adc_16le (*raw_data++);
          }
      }
      break;

    case HYSCAN_DATA_COMPLEX_ADC24LE:
      {
        guint32 *raw_data = data;
        for (i = 0; i < n_points; i++)
          {
            complex_float_buffer[i].re = hyscan_buffer_decode_adc_24le (*raw_data++);
            complex_float_buffer[i].im = hyscan_buffer_decode_adc_24le (*raw_data++);
          }
      }
      break;

    case HYSCAN_DATA_COMPLEX_FLOAT16LE:
      {
        guint16 *raw_data = data;
        for (i = 0; i < n_points; i++)
          {
            complex_float_buffer[i].re = hyscan_buffer_decode_float16le (*raw_data++);
            complex_float_buffer[i].im = hyscan_buffer_decode_float16le (*raw_data++);
          }
      }
      break;

    case HYSCAN_DATA_COMPLEX_FLOAT32LE:
      {
        guint32 *raw_data = data;
        for (i = 0; i < n_points; i++)
          {
            complex_float_buffer[i].re = hyscan_buffer_decode_float32le (*raw_data++);
            complex_float_buffer[i].im = hyscan_buffer_decode_float32le (*raw_data++);
          }
      }
      break;

    default:
      return FALSE;
    }

  return TRUE;
}

/**
 * hyscan_buffer_export_data:
 * @buffer: указатель на #HyScanBuffer
 * @raw: указатель на #HyScanBuffer для экспортируемых данныx
 * @type: тип экспортируемых данныx
 *
 * Функция экспортирует данные gfloat или #HyScanComplexFloat во внешний буфер и
 * преобразовывает их в указанный тип.
 *
 * Returns: %TRUE если данные успешно экспортированы, иначе %FALSE.
 */
gboolean
hyscan_buffer_export_data (HyScanBuffer   *buffer,
                           HyScanBuffer   *raw,
                           HyScanDataType  type)
{
  HyScanComplexFloat *complex_float_buffer = NULL;
  gfloat *float_buffer = NULL;
  guint8 *raw_data = NULL;
  guint32 data_size;
  guint32 n_points;
  guint32 i;

  g_return_val_if_fail (HYSCAN_IS_BUFFER (buffer), FALSE);

  /* Тип данных: действительные или комплексные. */
  switch (type)
    {
    case HYSCAN_DATA_FLOAT:
    case HYSCAN_DATA_ADC14LE:
    case HYSCAN_DATA_ADC16LE:
    case HYSCAN_DATA_ADC24LE:
    case HYSCAN_DATA_FLOAT16LE:
    case HYSCAN_DATA_FLOAT32LE:
    case HYSCAN_DATA_AMPLITUDE_INT8:
    case HYSCAN_DATA_AMPLITUDE_INT16LE:
    case HYSCAN_DATA_AMPLITUDE_INT24LE:
    case HYSCAN_DATA_AMPLITUDE_INT32LE:
    case HYSCAN_DATA_AMPLITUDE_FLOAT16LE:
    case HYSCAN_DATA_AMPLITUDE_FLOAT32LE:
      float_buffer = hyscan_buffer_get_float (buffer, &n_points);
      if (float_buffer == NULL)
        return FALSE;
      break;

    case HYSCAN_DATA_COMPLEX_FLOAT:
    case HYSCAN_DATA_COMPLEX_ADC14LE:
    case HYSCAN_DATA_COMPLEX_ADC16LE:
    case HYSCAN_DATA_COMPLEX_ADC24LE:
    case HYSCAN_DATA_COMPLEX_FLOAT16LE:
    case HYSCAN_DATA_COMPLEX_FLOAT32LE:
      complex_float_buffer = hyscan_buffer_get_complex_float (buffer, &n_points);
      if (complex_float_buffer == NULL)
        return FALSE;
      break;

    default:
      return FALSE;
    }

  /* Размер экспортируемых данных. */
  hyscan_buffer_set_data_type (raw, type);
  hyscan_buffer_set_size (raw, n_points * hyscan_data_get_point_size (type));
  raw_data = hyscan_buffer_get_data (raw, &data_size);
  n_points = data_size / hyscan_data_get_point_size (type);

  /* Экспорт данных. */
  switch (type)
    {
    case HYSCAN_DATA_FLOAT:
      {
        memcpy (raw_data, float_buffer, n_points * sizeof (gfloat));
      }
      break;

    case HYSCAN_DATA_ADC14LE:
      {
        guint16 *raw_data16 = (guint16*)raw_data;
        for (i = 0; i < n_points; i++)
          *raw_data16++ = hyscan_buffer_encode_adc_14le (float_buffer[i]);
      }
      break;

    case HYSCAN_DATA_ADC16LE:
      {
        guint16 *raw_data16 = (guint16*)raw_data;
        for (i = 0; i < n_points; i++)
          *raw_data16++ = hyscan_buffer_encode_adc_16le (float_buffer[i]);
      }
      break;

    case HYSCAN_DATA_ADC24LE:
      {
        guint32 *raw_data32 = (guint32*)raw_data;
        for (i = 0; i < n_points; i++)
          *raw_data32++ = hyscan_buffer_encode_adc_24le (float_buffer[i]);
      }
      break;

    case HYSCAN_DATA_AMPLITUDE_INT8:
      {
        for (i = 0; i < n_points; i++)
          *raw_data++ = hyscan_buffer_encode_int8 (float_buffer[i]);
      }
      break;

    case HYSCAN_DATA_AMPLITUDE_INT16LE:
      {
        guint16 *raw_data16 = (guint16*)raw_data;
        for (i = 0; i < n_points; i++)
          *raw_data16++ = hyscan_buffer_encode_int16le (float_buffer[i]);
      }
      break;

    case HYSCAN_DATA_AMPLITUDE_INT24LE:
      {
        guint32 *raw_data32 = (guint32*)raw_data;
        for (i = 0; i < n_points; i++)
          *raw_data32++ = hyscan_buffer_encode_int24le (float_buffer[i]);
      }
      break;

    case HYSCAN_DATA_AMPLITUDE_INT32LE:
      {
        guint32 *raw_data32 = (guint32*)raw_data;
        for (i = 0; i < n_points; i++)
          *raw_data32++ = hyscan_buffer_encode_int32le (float_buffer[i]);
      }
      break;

    case HYSCAN_DATA_FLOAT16LE:
    case HYSCAN_DATA_AMPLITUDE_FLOAT16LE:
      {
        guint16 *raw_data16 = (guint16*)raw_data;
        for (i = 0; i < n_points; i++)
          *raw_data16++ = hyscan_buffer_encode_float16le (float_buffer[i]);
      }
      break;

    case HYSCAN_DATA_FLOAT32LE:
    case HYSCAN_DATA_AMPLITUDE_FLOAT32LE:
      {
        guint32 *raw_data32 = (guint32*)raw_data;
        for (i = 0; i < n_points; i++)
          *raw_data32++ = hyscan_buffer_encode_float32le (float_buffer[i]);
      }
      break;

    case HYSCAN_DATA_COMPLEX_FLOAT:
      {
        memcpy (raw_data, complex_float_buffer, n_points * sizeof (HyScanComplexFloat));
      }
      break;

    case HYSCAN_DATA_COMPLEX_ADC14LE:
      {
        guint16 *raw_data16 = (guint16*)raw_data;
        for (i = 0; i < n_points; i++)
          {
            *raw_data16++ = hyscan_buffer_encode_adc_14le (complex_float_buffer[i].re);
            *raw_data16++ = hyscan_buffer_encode_adc_14le (complex_float_buffer[i].im);
          }
      }
      break;

    case HYSCAN_DATA_COMPLEX_ADC16LE:
      {
        guint16 *raw_data16 = (guint16*)raw_data;
        for (i = 0; i < n_points; i++)
          {
            *raw_data16++ = hyscan_buffer_encode_adc_16le (complex_float_buffer[i].re);
            *raw_data16++ = hyscan_buffer_encode_adc_16le (complex_float_buffer[i].im);
          }
      }
      break;

    case HYSCAN_DATA_COMPLEX_ADC24LE:
      {
        guint32 *raw_data32 = (guint32*)raw_data;
        for (i = 0; i < n_points; i++)
          {
            *raw_data32++ = hyscan_buffer_encode_adc_24le (complex_float_buffer[i].re);
            *raw_data32++ = hyscan_buffer_encode_adc_24le (complex_float_buffer[i].im);
          }
      }
      break;

    case HYSCAN_DATA_COMPLEX_FLOAT16LE:
      {
        guint16 *raw_data16 = (guint16*)raw_data;
        for (i = 0; i < n_points; i++)
          {
            *raw_data16++ = hyscan_buffer_encode_float16le (complex_float_buffer[i].re);
            *raw_data16++ = hyscan_buffer_encode_float16le (complex_float_buffer[i].im);
          }
      }
      break;

    case HYSCAN_DATA_COMPLEX_FLOAT32LE:
      {
        guint32 *raw_data32 = (guint32*)raw_data;
        for (i = 0; i < n_points; i++)
          {
            *raw_data32++ = hyscan_buffer_encode_float32le (complex_float_buffer[i].re);
            *raw_data32++ = hyscan_buffer_encode_float32le (complex_float_buffer[i].im);
          }
      }
      break;

    default:
      return FALSE;
    }

  return TRUE;
}

/**
 * hyscan_buffer_set_size:
 * @buffer: указатель на #HyScanBuffer
 * @size: размер буфера
 *
 * Функция изменяет размер буфера данных не затрагивая его содержимое. Если
 * буфер находится в режиме обёртки над внешними данными, максимальный размер
 * данных будет ограничен их размером.
 */
void
hyscan_buffer_set_size  (HyScanBuffer *buffer,
                         guint32       size)
{
  HyScanBufferPrivate *priv;

  g_return_if_fail (HYSCAN_IS_BUFFER (buffer));

  priv = buffer->priv;

  if (priv->self_allocated && (priv->allocated_size < size))
    {
      priv->data = g_realloc (priv->data, size);
      priv->allocated_size = size;
    }

  priv->data_size = (priv->allocated_size >= size) ? size : priv->allocated_size;
}

/**
 * hyscan_buffer_get_size:
 * @buffer: указатель на #HyScanBuffer
 *
 * Функция возвращает размер данных в буфере.
 *
 * Returns: Размер данных в буфере.
 */
guint32
hyscan_buffer_get_size (HyScanBuffer *buffer)
{
  g_return_val_if_fail (HYSCAN_IS_BUFFER (buffer), 0);

  return buffer->priv->data_size;
}

/**
 * hyscan_buffer_set_data_type:
 * @buffer: указатель на #HyScanBuffer
 * @type: тип данныx
 *
 * Функция устанавливает тип данных, хранящихся в буфере.
 */
void
hyscan_buffer_set_data_type (HyScanBuffer   *buffer,
                             HyScanDataType  type)
{
  g_return_if_fail (HYSCAN_IS_BUFFER (buffer));

  buffer->priv->data_type = type;
}

/**
 * hyscan_buffer_get_data_type:
 * @buffer: указатель на #HyScanBuffer
 *
 * Функция возвращает тип данных в буфере.
 *
 * Returns: Тип данных в буфере.
 */
HyScanDataType
hyscan_buffer_get_data_type (HyScanBuffer *buffer)
{
  g_return_val_if_fail (HYSCAN_IS_BUFFER (buffer), HYSCAN_DATA_INVALID);

  return buffer->priv->data_type;
}

/**
 * hyscan_buffer_wrap_data:
 * @buffer: указатель на #HyScanBuffer
 * @type: тип данныx
 * @data: данные
 * @size: размер данныx
 *
 * Функция конфигурирует буфер как обёртку над блоком данных.
 *
 * Данную функцию нельзя использовать через систему GIR, например
 * в python-gi.
 */
void
hyscan_buffer_wrap_data (HyScanBuffer   *buffer,
                         HyScanDataType  type,
                         gpointer        data,
                         guint32         size)
{
  HyScanBufferPrivate *priv;

  g_return_if_fail (HYSCAN_IS_BUFFER (buffer));

  priv = buffer->priv;

  if (priv->self_allocated)
    g_free (priv->data);

  priv->self_allocated = FALSE;
  priv->allocated_size = priv->data_size = size;
  priv->data_type = type;
  priv->data = data;
}

/**
 * hyscan_buffer_set_data:
 * @buffer: указатель на #HyScanBuffer
 * @type: тип данныx
 * @data: (array length=size) (element-type guint8) (transfer none): данные
 * @size: размер данныx
 *
 * Функция записывает данные в буфер. При этом буфер переводится в режим
 * динамического выделения памяти, если до этого он был сконфигурирован
 * в режиме обёртки над внешними данными.
 */
void
hyscan_buffer_set_data (HyScanBuffer   *buffer,
                        HyScanDataType  type,
                        gpointer        data,
                        guint32         size)
{
  HyScanBufferPrivate *priv;

  g_return_if_fail (HYSCAN_IS_BUFFER (buffer));

  priv = buffer->priv;

  if (!priv->self_allocated)
    {
      priv->data = NULL;
      priv->allocated_size = 0;
    }

  if (priv->allocated_size < size)
    {
      g_free (priv->data);
      priv->data = g_malloc0 (size);
      priv->allocated_size = size;
      priv->self_allocated = TRUE;
    }

  priv->data_size = size;
  priv->data_type = type;
  if (data != NULL)
    memcpy (priv->data, data, priv->data_size);
  else
    memset (priv->data, 0, priv->data_size);
}

/**
 * hyscan_buffer_get_data:
 * @buffer: указатель на #HyScanBuffer
 * @size: (out): размер данныx
 *
 * Функция возвращает указатель на данные в буфере. Пользователь может
 * изменять эти данные в пределах их размера.
 *
 * При использовании этой функции через систему GIR, например в python-gi,
 * возвращается копия данных. Необходимо это учитывать и для изменения
 * данных в буфере использовать функцию #hyscan_buffer_set_data.
 *
 * Returns: (nullable) (array length=size) (element-type guint8) (transfer none):
 *          Данные или NULL.
 */
gpointer
hyscan_buffer_get_data (HyScanBuffer *buffer,
                        guint32      *size)
{
  g_return_val_if_fail (HYSCAN_IS_BUFFER (buffer), NULL);

  *size = buffer->priv->data_size;

  return buffer->priv->data;
}

/**
 * hyscan_buffer_wrap_float:
 * @buffer: указатель на #HyScanBuffer
 * @data: (array length=n_points) (transfer none): массив gfloat
 * @n_points: число точек массива
 *
 * Функция конфигурирует буфер как обёртку над массивом gfloat.
 *
 * Данную функцию нельзя использовать через систему GIR, например
 * в python-gi.
 */
void
hyscan_buffer_wrap_float (HyScanBuffer *buffer,
                          gfloat       *data,
                          guint32       n_points)
{
  hyscan_buffer_wrap_data (buffer, HYSCAN_DATA_FLOAT,
                           data, n_points * sizeof (gfloat));
}

/**
 * hyscan_buffer_set_float:
 * @buffer: указатель на #HyScanBuffer
 * @data: (array length=n_points) (transfer none): массив gfloat
 * @n_points: число точек массива
 *
 * Функция записывает массив gfloat в буфер.
 */
void
hyscan_buffer_set_float (HyScanBuffer *buffer,
                         gfloat       *data,
                         guint32       n_points)
{
  hyscan_buffer_set_data (buffer, HYSCAN_DATA_FLOAT,
                          data, n_points * sizeof (gfloat));
}

/**
* hyscan_buffer_wrap_complex_float:
* @buffer: указатель на #HyScanBuffer
* @data: (array length=n_points) (transfer none): массив HyScanComplexFloat
* @n_points: число точек массива
*
* Функция конфигурирует буфер как обёртку над массивом HyScanComplexFloat.
*
* Данную функцию нельзя использовать через систему GIR, например
* в python-gi.
*/
void
hyscan_buffer_wrap_complex_float (HyScanBuffer       *buffer,
                                  HyScanComplexFloat *data,
                                  guint32             n_points)
{
  hyscan_buffer_wrap_data (buffer, HYSCAN_DATA_COMPLEX_FLOAT,
                           data, n_points * sizeof (HyScanComplexFloat));
}

/**
 * hyscan_buffer_set_complex_float:
 * @buffer: указатель на #HyScanBuffer
 * @data: (array length=n_points) (transfer none): массив #HyScanComplexFloat
 * @n_points: число точек массива
 *
 * Функция записывает массив #HyScanComplexFloat в буфер.
 */
void
hyscan_buffer_set_complex_float (HyScanBuffer       *buffer,
                                 HyScanComplexFloat *data,
                                 guint32             n_points)
{
  hyscan_buffer_set_data (buffer, HYSCAN_DATA_COMPLEX_FLOAT,
                          data, n_points * sizeof (HyScanComplexFloat));
}

/**
 * hyscan_buffer_get_float:
 * @buffer: указатель на #HyScanBuffer
 * @n_points: (out): число точек массива
 *
 * Функция возвращает указатель на данные в буфере в виде массива gfloat.
 * Пользователь может изменять эти данные в пределах их размера.
 *
 * При использовании этой функции через систему GIR, например в python-gi,
 * возвращается копия данных. Необходимо это учитывать и для изменения
 * данных в буфере использовать функцию #hyscan_buffer_set_float.
 *
 * Returns: (nullable) (array length=n_points) (transfer none):
 *          Массив gfloat или NULL.
 */
gfloat *
hyscan_buffer_get_float (HyScanBuffer *buffer,
                         guint32      *n_points)
{
  g_return_val_if_fail (HYSCAN_IS_BUFFER (buffer), NULL);

  if (buffer->priv->data_type != HYSCAN_DATA_FLOAT)
    return NULL;

  *n_points = buffer->priv->data_size / sizeof (gfloat);

  return buffer->priv->data;
}

/**
 * hyscan_buffer_get_complex_float:
 * @buffer: указатель на #HyScanBuffer
 * @n_points: (out): число точек массива
 *
 * Функция возвращает указатель на данные в буфере в виде массива #HyScanComplexFloat.
 * Пользователь может изменять эти данные в пределах их размера.
 *
 * При использовании этой функции через систему GIR, например в python-gi,
 * возвращается копия данных. Необходимо это учитывать и для изменения
 * данных в буфере использовать функцию #hyscan_buffer_set_complex_float.
 *
 * Returns: (nullable) (array length=n_points) (transfer none):
 *          Массив #HyScanComplexFloat или NULL.
 */
HyScanComplexFloat *
hyscan_buffer_get_complex_float (HyScanBuffer *buffer,
                                 guint32      *n_points)
{
  g_return_val_if_fail (HYSCAN_IS_BUFFER (buffer), NULL);

  if (buffer->priv->data_type != HYSCAN_DATA_COMPLEX_FLOAT)
    return NULL;

  *n_points = buffer->priv->data_size / sizeof (HyScanComplexFloat);

  return buffer->priv->data;
}
