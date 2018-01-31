/* hyscan-buffer.c
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
 * Во первых, вы можете распространять HyScanTypes на условиях Стандартной
 * Общественной Лицензии GNU версии 3, либо по любой более поздней версии
 * лицензии (по вашему выбору). Полные положения лицензии GNU приведены в
 * <http://www.gnu.org/licenses/>.
 *
 * Во вторых, этот программный код можно использовать по коммерческой
 * лицензии. Для этого свяжитесь с ООО Экран - info@screen-co.ru.
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
 * вспомогательные функции #hyscan_buffer_set_float, #hyscan_buffer_set_complex_float,
 * #hyscan_buffer_get_float и #hyscan_buffer_get_complex_float. Они работают
 * аналогично функциям #hyscan_buffer_set_data и #hyscan_buffer_get_data, но
 * обеспечивают приведение типов данных.
 */

#include "hyscan-buffer.h"
#include <string.h>

struct _HyScanBufferPrivate
{
  gboolean                     self_allocated;
  guint32                      allocated_size;
  HyScanDataType               data_type;
  guint32                      data_size;
  gpointer                     data;
};

static void             hyscan_buffer_object_finalize      (GObject               *object);

static inline gfloat           decode_adc_14le             (guint16                code);
static inline gfloat           decode_adc_16le             (guint16                code);
static inline gfloat           decode_adc_24le             (guint32                code);
static inline gfloat           decode_uint8                (guint8                 code);
static inline gfloat           decode_uint16               (guint16                code);
static inline gfloat           decode_uint32               (guint32                code);
static inline gfloat           decode_normal8              (guint8                 code);
static inline gfloat           decode_normal16             (guint16                code);

static inline guint16          encode_adc_14le             (gfloat                 value);
static inline guint16          encode_adc_16le             (gfloat                 value);
static inline guint32          encode_adc_24le             (gfloat                 value);
static inline guint8           encode_uint8                (gfloat                 value);
static inline guint16          encode_uint16               (gfloat                 value);
static inline guint32          encode_uint32               (gfloat                 value);
static inline guint8           encode_normal8              (gfloat                 value);
static inline guint16          encode_normal16             (gfloat                 value);

G_DEFINE_TYPE_WITH_PRIVATE (HyScanBuffer, hyscan_buffer, G_TYPE_OBJECT)

static void
hyscan_buffer_class_init (HyScanBufferClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = hyscan_buffer_object_finalize;
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

/* функции декодирования данных. */

static inline gfloat
decode_adc_14le (guint16 code)
{
  code = GUINT16_FROM_LE (code) & 0x3fff;
  return 2.0 * (code / 16383.0) - 1.0;
}

static inline gfloat
decode_adc_16le (guint16 code)
{
  code = GUINT16_FROM_LE (code);
  return 2.0 * (code / 65535.0) - 1.0;
}

static inline gfloat
decode_adc_24le (guint32 code)
{
  code = GUINT32_FROM_LE (code) & 0x00ffffff;
  return 2.0 * (code / 16777215.0) - 1.0;
}

static inline gfloat
decode_uint8 (guint8 code)
{
  return code / 255.0;
}

static inline gfloat
decode_uint16 (guint16 code)
{
  code = GUINT16_FROM_LE (code);
  return code / 65535.0;
}

static inline gfloat
decode_uint32 (guint32 code)
{
  code = GUINT32_FROM_LE (code);
  return code / 4294967295.0;
}

static inline gfloat
decode_normal8 (guint8 code)
{
  guint32 power;
  gfloat value;

  power = code >> 6;
  value = (code & 0x3f) / 63.0;

  if (power == 1)
    value /= 10.0;
  else if (power == 2)
    value /= 100.0;
  else if (power == 3)
    value /= 1000.0;

  return value;
}

static inline gfloat
decode_normal16 (guint16 code)
{
  gfloat value;
  guint32 power;

  code = GUINT16_FROM_LE (code);
  power = code >> 13;
  value = (code & 0x1fff) / 8191.0;

  if (power == 1)
    value /= 10.0;
  else if (power == 2)
    value /= 100.0;
  else if (power == 3)
    value /= 1000.0;
  else if (power == 4)
    value /= 10000.0;
  else if (power == 5)
    value /= 100000.0;
  else if (power == 6)
    value /= 1000000.0;
  else if (power == 7)
    value /= 10000000.0;

  return value;
}

/* Функции кодирования данных. */

static inline guint16
encode_adc_14le (gfloat value)
{
  guint16 code;
  value = 0.5 * value + 0.5;
  value = CLAMP (value, 0.0, 0.9999999);
  code = 16384 * value;
  return GUINT16_TO_LE (code);
}

static inline guint16
encode_adc_16le (gfloat value)
{
  guint16 code;
  value = 0.5 * value + 0.5;
  value = CLAMP (value, 0.0, 0.9999999);
  code = 65536 * value;
  return GUINT16_TO_LE (code);
}

static inline guint32
encode_adc_24le (gfloat value)
{
  guint32 code;
  value = 0.5 * value + 0.5;
  value = CLAMP (value, 0.0, 0.9999999);
  code = 16777216 * value;
  return GUINT32_TO_LE (code);
}

static inline guint8
encode_uint8 (gfloat value)
{
  value = CLAMP (value, 0.0, 0.9999999);
  return 256 * value;
}

static inline guint16
encode_uint16 (gfloat value)
{
  guint16 code;
  value = CLAMP (value, 0.0, 0.9999999);
  code = 65536 * value;
  return GUINT16_TO_LE (code);
}

static inline guint32
encode_uint32 (gfloat value)
{
  guint32 code;
  value = CLAMP (value, 0.0, 0.9999999);
  code = 4294967296 * value;
  return GUINT32_TO_LE (code);
}

static inline guint8
encode_normal8 (gfloat value)
{
  guint8 code;

  value = CLAMP (value, 0.0, 0.9999999);

  if (value > 0.1)
    code = (64 * value);
  else if (value > 0.01)
    code = (guint8)(640 * value) | 0x40;
  else if (value > 0.001)
    code = (guint8)(6400 * value) | 0x80;
  else
    code = (guint8)(64000 * value) | 0xC0;

  return code;
}

static inline guint16
encode_normal16 (gfloat value)
{
  guint16 code;

  value = CLAMP (value, 0.0, 0.9999999);

  if (value > 0.1)
    code = (8192 * value);
  else if (value > 0.01)
    code = (guint16)(81920 * value) | 0x2000;
  else if (value > 0.001)
    code = (guint16)(819200 * value) | 0x4000;
  else if (value > 0.0001)
    code = (guint16)(8192000 * value) | 0x6000;
  else if (value > 0.00001)
    code = (guint16)(81920000 * value) | 0x8000;
  else if (value > 0.000001)
    code = (guint16)(819200000 * value) | 0xA000;
  else if (value > 0.0000001)
    code = (guint16)(8192000000 * value) | 0xC000;
  else
    code = (guint16)(81920000000 * value) | 0xE000;

  return GUINT16_TO_LE (code);
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
    case HYSCAN_DATA_ADC_14LE:
    case HYSCAN_DATA_ADC_16LE:
    case HYSCAN_DATA_ADC_24LE:
    case HYSCAN_DATA_UINT8:
    case HYSCAN_DATA_UINT16:
    case HYSCAN_DATA_UINT32:
    case HYSCAN_DATA_FLOAT:
    case HYSCAN_DATA_NORMAL8:
    case HYSCAN_DATA_NORMAL16:
      n_points = data_size / hyscan_data_get_point_size (data_type);
      hyscan_buffer_set_size (buffer, n_points * sizeof (gfloat));
      hyscan_buffer_set_data_type (buffer, HYSCAN_DATA_FLOAT);
      float_buffer = hyscan_buffer_get_float (buffer, &n_points);
      break;

    case HYSCAN_DATA_COMPLEX_ADC_14LE:
    case HYSCAN_DATA_COMPLEX_ADC_16LE:
    case HYSCAN_DATA_COMPLEX_ADC_24LE:
    case HYSCAN_DATA_COMPLEX_UINT8:
    case HYSCAN_DATA_COMPLEX_UINT16:
    case HYSCAN_DATA_COMPLEX_UINT32:
    case HYSCAN_DATA_COMPLEX_FLOAT:
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
    case HYSCAN_DATA_ADC_14LE:
      for (i = 0; i < n_points; i++)
        float_buffer[i] = decode_adc_14le (*((guint16 *)data + i));
      break;

    case HYSCAN_DATA_ADC_16LE:
      for (i = 0; i < n_points; i++)
        float_buffer[i] = decode_adc_16le (*((guint16 *)data + i));
      break;

    case HYSCAN_DATA_ADC_24LE:
      for (i = 0; i < n_points; i++)
        float_buffer[i] = decode_adc_24le (*((guint32 *)data + i));
      break;

    case HYSCAN_DATA_UINT8:
      for (i = 0; i < n_points; i++)
        float_buffer[i] = decode_uint8 (*((guint8 *)data + i));
      break;

    case HYSCAN_DATA_UINT16:
      for (i = 0; i < n_points; i++)
        float_buffer[i] = decode_uint16 (*((guint16 *)data + i));
      break;

    case HYSCAN_DATA_UINT32:
      for (i = 0; i < n_points; i++)
        float_buffer[i] = decode_uint32 (*((guint32 *)data + i));
      break;

    case HYSCAN_DATA_FLOAT:
      memcpy (float_buffer, data, n_points * sizeof (gfloat));
      break;

    case HYSCAN_DATA_NORMAL8:
      for (i = 0; i < n_points; i++)
        float_buffer[i] = decode_normal8 (*((guint8 *)data + i));
      break;

    case HYSCAN_DATA_NORMAL16:
      for (i = 0; i < n_points; i++)
        float_buffer[i] = decode_normal16 (*((guint16 *)data + i));
      break;

    case HYSCAN_DATA_COMPLEX_ADC_14LE:
      for (i = 0; i < n_points; i++)
        {
          complex_float_buffer[i].re = decode_adc_14le (*((guint16 *)data + 2 * i));
          complex_float_buffer[i].im = decode_adc_14le (*((guint16 *)data + 2 * i + 1));
        }
      break;

    case HYSCAN_DATA_COMPLEX_ADC_16LE:
      for (i = 0; i < n_points; i++)
        {
          complex_float_buffer[i].re = decode_adc_16le (*((guint16 *)data + 2 * i));
          complex_float_buffer[i].im = decode_adc_16le (*((guint16 *)data + 2 * i + 1));
        }
      break;

    case HYSCAN_DATA_COMPLEX_ADC_24LE:
      for (i = 0; i < n_points; i++)
        {
          complex_float_buffer[i].re = decode_adc_24le (*((guint32 *)data + 2 * i));
          complex_float_buffer[i].im = decode_adc_24le (*((guint32 *)data + 2 * i + 1));
        }
      break;

    case HYSCAN_DATA_COMPLEX_UINT8:
      for (i = 0; i < n_points; i++)
        {
          complex_float_buffer[i].re = decode_uint8 (*((guint8 *)data + 2 * i));
          complex_float_buffer[i].im = decode_uint8 (*((guint8 *)data + 2 * i + 1));
        }
      break;

    case HYSCAN_DATA_COMPLEX_UINT16:
      for (i = 0; i < n_points; i++)
        {
          complex_float_buffer[i].re = decode_uint16 (*((guint16 *)data + 2 * i));
          complex_float_buffer[i].im = decode_uint16 (*((guint16 *)data + 2 * i + 1));
        }
      break;

    case HYSCAN_DATA_COMPLEX_UINT32:
      for (i = 0; i < n_points; i++)
        {
          complex_float_buffer[i].re = decode_uint32 (*((guint32 *)data + 2 * i));
          complex_float_buffer[i].im = decode_uint32 (*((guint32 *)data + 2 * i + 1));
        }
      break;

    case HYSCAN_DATA_COMPLEX_FLOAT:
      memcpy (complex_float_buffer, data, n_points * sizeof (HyScanComplexFloat));
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
    case HYSCAN_DATA_ADC_14LE:
    case HYSCAN_DATA_ADC_16LE:
    case HYSCAN_DATA_ADC_24LE:
    case HYSCAN_DATA_UINT8:
    case HYSCAN_DATA_UINT16:
    case HYSCAN_DATA_UINT32:
    case HYSCAN_DATA_FLOAT:
    case HYSCAN_DATA_NORMAL8:
    case HYSCAN_DATA_NORMAL16:
      float_buffer = hyscan_buffer_get_float (buffer, &n_points);
      if (float_buffer == NULL)
        return FALSE;
      break;

    case HYSCAN_DATA_COMPLEX_ADC_14LE:
    case HYSCAN_DATA_COMPLEX_ADC_16LE:
    case HYSCAN_DATA_COMPLEX_ADC_24LE:
    case HYSCAN_DATA_COMPLEX_UINT8:
    case HYSCAN_DATA_COMPLEX_UINT16:
    case HYSCAN_DATA_COMPLEX_UINT32:
    case HYSCAN_DATA_COMPLEX_FLOAT:
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
    case HYSCAN_DATA_ADC_14LE:
      for (i = 0; i < n_points; i++)
        *((guint16*)raw_data + i) = encode_adc_14le (float_buffer[i]);
      break;

    case HYSCAN_DATA_ADC_16LE:
      for (i = 0; i < n_points; i++)
        *((guint16*)raw_data + i) = encode_adc_16le (float_buffer[i]);
      break;

    case HYSCAN_DATA_ADC_24LE:
      for (i = 0; i < n_points; i++)
        *((guint32*)raw_data + i) = encode_adc_24le (float_buffer[i]);
      break;

    case HYSCAN_DATA_UINT8:
      for (i = 0; i < n_points; i++)
        *((guint8*)raw_data + i) = encode_uint8 (float_buffer[i]);
      break;

    case HYSCAN_DATA_UINT16:
      for (i = 0; i < n_points; i++)
        *((guint16*)raw_data + i) = encode_uint16 (float_buffer[i]);
      break;

    case HYSCAN_DATA_UINT32:
      for (i = 0; i < n_points; i++)
        *((guint32*)raw_data + i) = encode_uint32 (float_buffer[i]);
      break;

    case HYSCAN_DATA_FLOAT:
      memcpy (raw_data, float_buffer, n_points * sizeof (gfloat));
      break;

    case HYSCAN_DATA_NORMAL8:
      for (i = 0; i < n_points; i++)
        *((guint8*)raw_data + i) = encode_normal8 (float_buffer[i]);
      break;

    case HYSCAN_DATA_NORMAL16:
      for (i = 0; i < n_points; i++)
        *((guint16*)raw_data + i) = encode_normal16 (float_buffer[i]);
      break;

    case HYSCAN_DATA_COMPLEX_ADC_14LE:
      for (i = 0; i < n_points; i++)
        {
          *((guint16*)raw_data + 2 * i) = encode_adc_14le (complex_float_buffer[i].re);
          *((guint16*)raw_data + 2 * i + 1) = encode_adc_14le (complex_float_buffer[i].im);
        }
      break;

    case HYSCAN_DATA_COMPLEX_ADC_16LE:
      for (i = 0; i < n_points; i++)
        {
          *((guint16*)raw_data + 2 * i) = encode_adc_16le (complex_float_buffer[i].re);
          *((guint16*)raw_data + 2 * i + 1) = encode_adc_16le (complex_float_buffer[i].im);
        }
      break;

    case HYSCAN_DATA_COMPLEX_ADC_24LE:
      for (i = 0; i < n_points; i++)
        {
          *((guint32*)raw_data + 2 * i) = encode_adc_24le (complex_float_buffer[i].re);
          *((guint32*)raw_data + 2 * i + 1) = encode_adc_24le (complex_float_buffer[i].im);
        }
      break;

    case HYSCAN_DATA_COMPLEX_UINT8:
      for (i = 0; i < n_points; i++)
        {
          *((guint8*)raw_data + 2 * i) = encode_uint8 (complex_float_buffer[i].re);
          *((guint8*)raw_data + 2 * i + 1) = encode_uint8 (complex_float_buffer[i].im);
        }
      break;

    case HYSCAN_DATA_COMPLEX_UINT16:
      for (i = 0; i < n_points; i++)
        {
          *((guint16*)raw_data + 2 * i) = encode_uint16 (complex_float_buffer[i].re);
          *((guint16*)raw_data + 2 * i + 1) = encode_uint16 (complex_float_buffer[i].im);
        }
      break;

    case HYSCAN_DATA_COMPLEX_UINT32:
      for (i = 0; i < n_points; i++)
        {
          *((guint32*)raw_data + 2 * i) = encode_uint32 (complex_float_buffer[i].re);
          *((guint32*)raw_data + 2 * i + 1) = encode_uint32 (complex_float_buffer[i].im);
        }
      break;

    case HYSCAN_DATA_COMPLEX_FLOAT:
      memcpy (raw_data, complex_float_buffer, n_points * sizeof (HyScanComplexFloat));
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
 * hyscan_buffer_set_float:
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
 * hyscan_buffer_set_complex_float:
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
