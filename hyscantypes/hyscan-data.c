/*
 * \file hyscan-data.c
 *
 * \brief Исходный файл вспомогательных функций с определениями типов данных HyScan
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2015
 * \license Проприетарная лицензия ООО "Экран"
 *
*/

#include "hyscan-data.h"

#include <string.h>

/* Типы данных и их названия. */
typedef struct
{
  GQuark                       quark;
  const gchar                 *name;
  HyScanDataType               type;
} HyScanDataTypeInfo;

/* Типы данных и их названия. */
static HyScanDataTypeInfo hyscan_data_types_info[] =
{
  { 0, "string", HYSCAN_DATA_STRING },

  { 0, "adc14le", HYSCAN_DATA_ADC_14LE },
  { 0, "adc16le", HYSCAN_DATA_ADC_16LE },

  { 0, "complex-adc14le", HYSCAN_DATA_COMPLEX_ADC_14LE },
  { 0, "complex-adc16le", HYSCAN_DATA_COMPLEX_ADC_16LE },

  { 0, "float",  HYSCAN_DATA_FLOAT },
  { 0, "complex-float",  HYSCAN_DATA_COMPLEX_FLOAT },

  { 0, NULL, HYSCAN_DATA_INVALID }
};

/* Функция инициализации статических данных. */
static void
hyscan_types_initialize (void)
{
  static gboolean hyscan_types_initialized = FALSE;
  gint i;

  if (hyscan_types_initialized)
    return;

  for (i = 0; hyscan_data_types_info[i].name != NULL; i++)
    hyscan_data_types_info[i].quark = g_quark_from_static_string (hyscan_data_types_info[i].name);

  hyscan_types_initialized = TRUE;
}

/* Функция преобразовывает нумерованное значение типа данных в название типа. */
const gchar *
hyscan_data_get_type_name (HyScanDataType data_type)
{
  gint i;

  /* Инициализация статических данных. */
  hyscan_types_initialize ();

  /* Ищем строку с указанным типом данных. */
  for (i = 0; hyscan_data_types_info[i].quark != 0; i++)
    if (hyscan_data_types_info[i].type == data_type)
      return hyscan_data_types_info[i].name;

  return NULL;
}

/* Функция преобразовывает строку с названием типа данных в нумерованное значение. */
HyScanDataType
hyscan_data_get_type_by_name (const gchar *data_name)
{
  GQuark quark;
  gint i;

  /* Инициализация статических данных. */
  hyscan_types_initialize ();

  /* Ищем тип данных с указанным именем. */
  quark = g_quark_try_string (data_name);
  for (i = 0; hyscan_data_types_info[i].quark != 0; i++)
    {
      if (hyscan_data_types_info[i].quark == quark)
        return hyscan_data_types_info[i].type;
    }

  return HYSCAN_DATA_INVALID;
}

/* Функция возвращает размер одного элемента данных в байтах, для указанного типа. */
gint32
hyscan_data_get_point_size (HyScanDataType data_type)
{
  switch (data_type)
    {
    case HYSCAN_DATA_STRING:
      return sizeof (gchar);

    case HYSCAN_DATA_ADC_14LE:
    case HYSCAN_DATA_ADC_16LE:
      return sizeof (gint16);

    case HYSCAN_DATA_COMPLEX_ADC_14LE:
    case HYSCAN_DATA_COMPLEX_ADC_16LE:
      return 2 * sizeof (gint16);

    case HYSCAN_DATA_FLOAT:
      return sizeof (gfloat);

    case HYSCAN_DATA_COMPLEX_FLOAT:
      return 2 * sizeof (gfloat);

    default:
      return 0;
    }

  return 0;
}

/* Функция преобразовывает данные из низкоуровневого формата в float размером data_size. */
gboolean
hyscan_data_import_float (HyScanDataType  data_type,
                          gpointer        data,
                          gint32          data_size,
                          gfloat         *buffer,
                          gint32         *buffer_size)
{
  gint32 i;
  gint32 n_points;

  switch (data_type)
    {
    case HYSCAN_DATA_ADC_14LE:
      n_points = data_size / sizeof (gint16);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint16 raw_re = GUINT16_FROM_LE (*((guint16 *)data + n_points)) & 0x3fff;
          buffer[i] = (gfloat)(raw_re / 8192.0) - 1.0;
        }
      break;

    case HYSCAN_DATA_ADC_16LE:
      n_points = data_size / sizeof (gint16);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint16 raw_re = GUINT16_FROM_LE (*((guint16 *)data + n_points));
          buffer[i] = (gfloat)(raw_re / 32768.0) - 1.0;
        }
      break;

    case HYSCAN_DATA_FLOAT:
      n_points = data_size / sizeof (gfloat);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      memcpy (buffer, data, n_points * sizeof (gfloat));
      break;

    default:
      return FALSE;

    }

  return TRUE;
}

/* Функция преобразовывает данные из низкоуровневого формата в HyScanComplexFloat размером data_size. */
gboolean
hyscan_data_import_complex_float (HyScanDataType      data_type,
                                  gpointer            data,
                                  gint32              data_size,
                                  HyScanComplexFloat *buffer,
                                  gint32             *buffer_size)
{
  gint32 i;
  gint32 n_points;

  switch (data_type)
    {
    case HYSCAN_DATA_COMPLEX_ADC_14LE:
      n_points = data_size / (2 * sizeof (gint16));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint16 raw_re = GUINT16_FROM_LE (*((guint16 *)data + 2 * i)) & 0x3fff;
          guint16 raw_im = GUINT16_FROM_LE (*((guint16 *)data + 2 * i + 1)) & 0x3fff;
          buffer[i].re = (gfloat)(raw_re / 8192.0) - 1.0;
          buffer[i].im = (gfloat)(raw_im / 8192.0) - 1.0;
        }
      break;

    case HYSCAN_DATA_COMPLEX_ADC_16LE:
      n_points = data_size / (2 * sizeof (gint16));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint16 raw_re = GUINT16_FROM_LE (*((guint16 *)data + 2 * i));
          guint16 raw_im = GUINT16_FROM_LE (*((guint16 *)data + 2 * i + 1));
          buffer[i].re = (gfloat)(raw_re / 32768.0) - 1.0;
          buffer[i].im = (gfloat)(raw_im / 32768.0) - 1.0;
        }
      break;

    case HYSCAN_DATA_COMPLEX_FLOAT:
      n_points = data_size / (2 * sizeof (gfloat));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        memcpy (buffer, data, n_points * 2 * sizeof (gfloat));
      break;

    default:
      return FALSE;

    }

  return TRUE;
}
