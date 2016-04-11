/**
 * \file hyscan-types.c
 *
 * \brief Исходный файл вспомогательных функций с определениями типов данных HyScan
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2015
 * \license Проприетарная лицензия ООО "Экран"
 *
*/

#include "hyscan-types.h"

/* Типы каналов и их названия. */
typedef struct
{
  GQuark                       quark;
  const gchar                 *name;

  HyScanSonarBoardType         board_type;
  HyScanSonarDataType          data_type;
  gboolean                     hi_res;
  gboolean                     raw;
  gint                         index;
} HyScanChannelTypeInfo;

/* Типы данных и их названия. */
typedef struct
{
  GQuark                       quark;
  const gchar                 *name;
  HyScanDataType               type;
} HyScanDataTypeInfo;

/* Типы каналов и их названия. */
static HyScanChannelTypeInfo hyscan_channel_types_info[] =
{
  { 0, "echo-sounder",     HYSCAN_SONAR_BOARD_BOTTOM, HYSCAN_SONAR_DATA_ECHOSOUNDER, FALSE, FALSE, 1 },
  { 0, "echo-sounder-raw", HYSCAN_SONAR_BOARD_BOTTOM, HYSCAN_SONAR_DATA_ECHOSOUNDER, FALSE, TRUE,  1 },

  { 0, "ss-left",          HYSCAN_SONAR_BOARD_LEFT, HYSCAN_SONAR_DATA_SIDE_SCAN, FALSE, FALSE, 1 },
  { 0, "ss-left-hi",       HYSCAN_SONAR_BOARD_LEFT, HYSCAN_SONAR_DATA_SIDE_SCAN, TRUE,  FALSE, 1 },
  { 0, "ss-left-raw",      HYSCAN_SONAR_BOARD_LEFT, HYSCAN_SONAR_DATA_SIDE_SCAN, FALSE, TRUE,  1 },
  { 0, "ss-left-hi-raw",   HYSCAN_SONAR_BOARD_LEFT, HYSCAN_SONAR_DATA_SIDE_SCAN, TRUE,  TRUE,  1 },

  { 0, "ss-right",         HYSCAN_SONAR_BOARD_RIGHT, HYSCAN_SONAR_DATA_SIDE_SCAN, FALSE, FALSE, 1 },
  { 0, "ss-right-hi",      HYSCAN_SONAR_BOARD_RIGHT, HYSCAN_SONAR_DATA_SIDE_SCAN, TRUE,  FALSE, 1 },
  { 0, "ss-right-raw",     HYSCAN_SONAR_BOARD_RIGHT, HYSCAN_SONAR_DATA_SIDE_SCAN, FALSE, TRUE,  1 },
  { 0, "ss-right-hi-raw",  HYSCAN_SONAR_BOARD_RIGHT, HYSCAN_SONAR_DATA_SIDE_SCAN, TRUE,  TRUE,  1 },

  { 0, "sas",              HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_SAS, FALSE, FALSE, 1 },

  { 0, "nmea",             HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_ANY, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_1 },
  { 0, "nmea-gsa",         HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_GSA, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_1 },
  { 0, "nmea-gsv",         HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_GSV, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_1 },
  { 0, "nmea-gga",         HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_GGA, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_1 },
  { 0, "nmea-gll",         HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_GLL, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_1 },
  { 0, "nmea-rmc",         HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_RMC, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_1 },
  { 0, "nmea-dpt",         HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_DPT, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_1 },

  { 0, "nmea-2",           HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_ANY, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_2 },
  { 0, "nmea-gsa-2",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_GSA, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_2 },
  { 0, "nmea-gsv-2",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_GSV, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_2 },
  { 0, "nmea-gga-2",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_GGA, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_2 },
  { 0, "nmea-gll-2",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_GLL, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_2 },
  { 0, "nmea-rmc-2",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_RMC, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_2 },
  { 0, "nmea-dpt-2",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_DPT, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_2 },

  { 0, "nmea-3",           HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_ANY, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_3 },
  { 0, "nmea-gsa-3",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_GSA, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_3 },
  { 0, "nmea-gsv-3",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_GSV, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_3 },
  { 0, "nmea-gga-3",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_GGA, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_3 },
  { 0, "nmea-gll-3",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_GLL, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_3 },
  { 0, "nmea-rmc-3",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_RMC, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_3 },
  { 0, "nmea-dpt-3",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_DPT, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_3 },

  { 0, "nmea-4",           HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_ANY, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_4 },
  { 0, "nmea-gsa-4",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_GSA, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_4 },
  { 0, "nmea-gsv-4",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_GSV, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_4 },
  { 0, "nmea-gga-4",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_GGA, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_4 },
  { 0, "nmea-gll-4",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_GLL, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_4 },
  { 0, "nmea-rmc-4",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_RMC, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_4 },
  { 0, "nmea-dpt-4",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_DPT, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_4 },

  { 0, "nmea-5",           HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_ANY, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_5 },
  { 0, "nmea-gsa-5",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_GSA, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_5 },
  { 0, "nmea-gsv-5",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_GSV, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_5 },
  { 0, "nmea-gga-5",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_GGA, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_5 },
  { 0, "nmea-gll-5",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_GLL, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_5 },
  { 0, "nmea-rmc-5",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_RMC, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_5 },
  { 0, "nmea-dpt-5",       HYSCAN_SONAR_BOARD_ANY, HYSCAN_SONAR_DATA_NMEA_DPT, FALSE, FALSE, HYSCAN_SONAR_SENSOR_CHANNEL_5 },

  { 0, NULL, HYSCAN_SONAR_BOARD_INVALID, HYSCAN_SONAR_DATA_INVALID, FALSE, FALSE, 0 }
};

/* Типы данных и их названия. */
static HyScanDataTypeInfo hyscan_data_types_info[] =
{
  { 0, "HyScan-String", HYSCAN_DATA_TYPE_STRING },

  { 0, "HyScan-ADC8bit", HYSCAN_DATA_TYPE_ADC_8BIT },
  { 0, "HyScan-ADC10bit", HYSCAN_DATA_TYPE_ADC_10BIT },
  { 0, "HyScan-ADC12bit", HYSCAN_DATA_TYPE_ADC_12BIT },
  { 0, "HyScan-ADC14bit", HYSCAN_DATA_TYPE_ADC_14BIT },
  { 0, "HyScan-ADC16bit", HYSCAN_DATA_TYPE_ADC_16BIT },

  { 0, "HyScan-Complex-ADC8bit", HYSCAN_DATA_TYPE_COMPLEX_ADC_8BIT },
  { 0, "HyScan-Complex-ADC10bit", HYSCAN_DATA_TYPE_COMPLEX_ADC_10BIT },
  { 0, "HyScan-Complex-ADC12bit", HYSCAN_DATA_TYPE_COMPLEX_ADC_12BIT },
  { 0, "HyScan-Complex-ADC14bit", HYSCAN_DATA_TYPE_COMPLEX_ADC_14BIT },
  { 0, "HyScan-Complex-ADC16bit", HYSCAN_DATA_TYPE_COMPLEX_ADC_16BIT },

  { 0, "HyScan-Int8",   HYSCAN_DATA_TYPE_INT8 },
  { 0, "HyScan-UInt8",  HYSCAN_DATA_TYPE_UINT8 },
  { 0, "HyScan-Int16",  HYSCAN_DATA_TYPE_INT16 },
  { 0, "HyScan-UInt16", HYSCAN_DATA_TYPE_UINT16 },
  { 0, "HyScan-Int32",  HYSCAN_DATA_TYPE_INT32 },
  { 0, "HyScan-UInt32", HYSCAN_DATA_TYPE_UINT32 },
  { 0, "HyScan-Int64",  HYSCAN_DATA_TYPE_INT64 },
  { 0, "HyScan-UInt64", HYSCAN_DATA_TYPE_UINT64 },

  { 0, "HyScan-Complex-Int8",   HYSCAN_DATA_TYPE_COMPLEX_INT8 },
  { 0, "HyScan-Complex-UInt8",  HYSCAN_DATA_TYPE_COMPLEX_UINT8 },
  { 0, "HyScan-Complex-Int16",  HYSCAN_DATA_TYPE_COMPLEX_INT16 },
  { 0, "HyScan-Complex-UInt16", HYSCAN_DATA_TYPE_COMPLEX_UINT16 },
  { 0, "HyScan-Complex-Int32",  HYSCAN_DATA_TYPE_COMPLEX_INT32 },
  { 0, "HyScan-Complex-UInt32", HYSCAN_DATA_TYPE_COMPLEX_UINT32 },
  { 0, "HyScan-Complex-Int64",  HYSCAN_DATA_TYPE_COMPLEX_INT64 },
  { 0, "HyScan-Complex-UInt64", HYSCAN_DATA_TYPE_COMPLEX_UINT64 },

  { 0, "HyScan-Float",  HYSCAN_DATA_TYPE_FLOAT },
  { 0, "HyScan-Double", HYSCAN_DATA_TYPE_DOUBLE },

  { 0, "HyScan-Complex-Float",  HYSCAN_DATA_TYPE_COMPLEX_FLOAT },
  { 0, "HyScan-Complex-Double", HYSCAN_DATA_TYPE_COMPLEX_DOUBLE },

  { 0, NULL, HYSCAN_DATA_TYPE_INVALID }
};

/* Функция инициализации статических данных. */
static void
hyscan_types_initialize (void)
{
  static gboolean hyscan_initialized = FALSE;
  gint i;

  if (hyscan_initialized)
    return;

  for (i = 0; hyscan_data_types_info[i].name != NULL; i++)
    hyscan_data_types_info[i].quark = g_quark_from_static_string (hyscan_data_types_info[i].name);

  for (i = 0; hyscan_channel_types_info[i].name != NULL; i++)
    hyscan_channel_types_info[i].quark = g_quark_from_static_string (hyscan_channel_types_info[i].name);

  hyscan_initialized = TRUE;
}

/* Функция возвращает название канала для указанных характеристик. */
const gchar *
hyscan_get_channel_name_by_types (HyScanSonarBoardType board_type,
                                  HyScanSonarDataType  data_type,
                                  gboolean             hi_res,
                                  gboolean             raw,
                                  gint                 index)
{
  gint i;

  /* Инициализация статических данных. */
  hyscan_types_initialize ();

  /* Ищем название канала для указанных характеристик. */
  for (i = 0; hyscan_channel_types_info[i].quark != 0; i++)
    {
      if (hyscan_channel_types_info[i].board_type != board_type)
        continue;
      if (hyscan_channel_types_info[i].data_type != data_type)
        continue;
      if (hyscan_channel_types_info[i].hi_res != hi_res)
        continue;
      if (hyscan_channel_types_info[i].raw != raw)
        continue;
      if (hyscan_channel_types_info[i].index != index)
        continue;
      return hyscan_channel_types_info[i].name;
    }

  return NULL;
}

/* Функция возвращает характеристики канала данных по его имени. */
gboolean
hyscan_get_channel_types_by_name (const gchar          *channel_name,
                                  HyScanSonarBoardType *board_type,
                                  HyScanSonarDataType  *data_type,
                                  gboolean             *hi_res,
                                  gboolean             *raw,
                                  gint                 *index)
{
  GQuark quark;
  gint i;

  /* Инициализация статических данных. */
  hyscan_types_initialize ();

  /* Ищем канал с указанным именем. */
  quark = g_quark_try_string (channel_name);
  for (i = 0; hyscan_channel_types_info[i].quark != 0; i++)
    {
      if (hyscan_channel_types_info[i].quark == quark)
        {
          if (board_type != NULL)
            *board_type = hyscan_channel_types_info[i].board_type;
          if (data_type != NULL)
            *data_type = hyscan_channel_types_info[i].data_type;
          if (hi_res != NULL)
            *hi_res = hyscan_channel_types_info[i].hi_res;
          if (raw != NULL)
            *raw = hyscan_channel_types_info[i].raw;
          if (index != NULL)
            *index = hyscan_channel_types_info[i].index;
          return TRUE;
        }
    }

  return FALSE;
}

/* Функция преобразовывает строку с названием типа данных в нумерованное значение. */
HyScanDataType
hyscan_get_data_type_by_name (const gchar *data_name)
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

  return HYSCAN_DATA_TYPE_INVALID;
}

/* Функция преобразовывает нумерованное значение типа данных в название типа. */
const gchar *
hyscan_get_data_type_name (HyScanDataType data_type)
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

/* Функция возвращает размер одного элемента данных в байтах, для указанного типа. */
gint32
hyscan_get_data_point_size (HyScanDataType data_type)
{
  switch (data_type)
    {
    case HYSCAN_DATA_TYPE_STRING:
      return sizeof (gchar);

    case HYSCAN_DATA_TYPE_ADC_8BIT:
      return sizeof (gint8);

    case HYSCAN_DATA_TYPE_ADC_10BIT:
    case HYSCAN_DATA_TYPE_ADC_12BIT:
    case HYSCAN_DATA_TYPE_ADC_14BIT:
    case HYSCAN_DATA_TYPE_ADC_16BIT:
      return sizeof (gint16);

    case HYSCAN_DATA_TYPE_COMPLEX_ADC_8BIT:
      return 2 * sizeof (gint8);

    case HYSCAN_DATA_TYPE_COMPLEX_ADC_10BIT:
    case HYSCAN_DATA_TYPE_COMPLEX_ADC_12BIT:
    case HYSCAN_DATA_TYPE_COMPLEX_ADC_14BIT:
    case HYSCAN_DATA_TYPE_COMPLEX_ADC_16BIT:
      return 2 * sizeof (gint16);

    case HYSCAN_DATA_TYPE_INT8:
    case HYSCAN_DATA_TYPE_UINT8:
      return sizeof (gint8);

    case HYSCAN_DATA_TYPE_INT16:
    case HYSCAN_DATA_TYPE_UINT16:
      return sizeof (gint16);

    case HYSCAN_DATA_TYPE_INT32:
    case HYSCAN_DATA_TYPE_UINT32:
      return sizeof (gint32);

    case HYSCAN_DATA_TYPE_INT64:
    case HYSCAN_DATA_TYPE_UINT64:
      return sizeof (gint64);

    case HYSCAN_DATA_TYPE_COMPLEX_INT8:
    case HYSCAN_DATA_TYPE_COMPLEX_UINT8:
      return 2 * sizeof (gint8);

    case HYSCAN_DATA_TYPE_COMPLEX_INT16:
    case HYSCAN_DATA_TYPE_COMPLEX_UINT16:
      return 2 * sizeof (gint16);

    case HYSCAN_DATA_TYPE_COMPLEX_INT32:
    case HYSCAN_DATA_TYPE_COMPLEX_UINT32:
      return 2 * sizeof (gint32);

    case HYSCAN_DATA_TYPE_COMPLEX_INT64:
    case HYSCAN_DATA_TYPE_COMPLEX_UINT64:
      return 2 * sizeof (gint64);

    case HYSCAN_DATA_TYPE_FLOAT:
      return sizeof (gfloat);

    case HYSCAN_DATA_TYPE_DOUBLE:
      return sizeof (gdouble);

    case HYSCAN_DATA_TYPE_COMPLEX_FLOAT:
      return 2 * sizeof (gfloat);

    case HYSCAN_DATA_TYPE_COMPLEX_DOUBLE:
      return 2 * sizeof (gdouble);

    default:
      return 0;
    }

  return 0;
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
    case HYSCAN_DATA_TYPE_ADC_8BIT:
      n_points = data_size / sizeof (gint8);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint8 *)data + n_points)) & 0x0ff;
          buffer[i].re = (gfloat) raw_re / 128.0;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_ADC_10BIT:
      n_points = data_size / sizeof (gint16);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint16 *)data + n_points)) & 0x03ff;
          buffer[i].re = (gfloat) raw_re / 512.0;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_ADC_12BIT:
      n_points = data_size / sizeof (gint16);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint16 *)data + n_points)) & 0x0fff;
          buffer[i].re = (gfloat) raw_re / 2048.0;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_ADC_14BIT:
      n_points = data_size / sizeof (gint16);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint16 *)data + n_points)) & 0x3fff;
          buffer[i].re = (gfloat) raw_re / 8192.0;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_ADC_16BIT:
      n_points = data_size / sizeof (gint16);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint16 *)data + n_points));
          buffer[i].re = (gfloat) raw_re / 32768.0;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_ADC_8BIT:
      n_points = data_size / (2 * sizeof (gint8));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint8 *)data + 2 * n_points)) & 0x0ff;
          gint16 raw_im = GINT16_FROM_LE (*((gint8 *)data + 2 * n_points + 1)) & 0x0ff;
          buffer[i].re = (gfloat) raw_re / 128.0;
          buffer[i].im = (gfloat) raw_im / 128.0;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_ADC_10BIT:
      n_points = data_size / (2 * sizeof (gint16));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint16 *)data + 2 * n_points)) & 0x03ff;
          gint16 raw_im = GINT16_FROM_LE (*((gint16 *)data + 2 * n_points + 1)) & 0x03ff;
          buffer[i].re = (gfloat) raw_re / 512.0;
          buffer[i].im = (gfloat) raw_im / 512.0;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_ADC_12BIT:
      n_points = data_size / (2 * sizeof (gint16));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint16 *)data + 2 * n_points)) & 0x0fff;
          gint16 raw_im = GINT16_FROM_LE (*((gint16 *)data + 2 * n_points + 1)) & 0x0fff;
          buffer[i].re = (gfloat) raw_re / 2048.0;
          buffer[i].im = (gfloat) raw_im / 2048.0;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_ADC_14BIT:
      n_points = data_size / (2 * sizeof (gint16));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint16 *)data + 2 * n_points)) & 0x3fff;
          gint16 raw_im = GINT16_FROM_LE (*((gint16 *)data + 2 * n_points + 1)) & 0x3fff;
          buffer[i].re = (gfloat) raw_re / 8192.0;
          buffer[i].im = (gfloat) raw_im / 8192.0;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_ADC_16BIT:
      n_points = data_size / (2 * sizeof (gint16));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint16 *)data + 2 * i));
          gint16 raw_im = GINT16_FROM_LE (*((gint16 *)data + 2 * i + 1));
          buffer[i].re = (gfloat) raw_re / 32768.0;
          buffer[i].im = (gfloat) raw_im / 32768.0;
        }
      break;

    case HYSCAN_DATA_TYPE_INT8:
      n_points = data_size / sizeof (gint8);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint8 raw_re = *((gint8 *)data + n_points);
          buffer[i].re = (gfloat) raw_re / G_MAXINT8;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_UINT8:
      n_points = data_size / sizeof (guint8);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint8 raw_re = *((guint8 *)data + n_points);
          buffer[i].re = (gfloat) raw_re / G_MAXUINT8;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_INT16:
      n_points = data_size / sizeof (gint16);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint16 *)data + n_points));
          buffer[i].re = (gfloat) raw_re / G_MAXINT16;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_UINT16:
      n_points = data_size / sizeof (guint16);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint16 raw_re = GUINT16_FROM_LE (*((guint16 *)data + n_points));
          buffer[i].re = (gfloat) raw_re / G_MAXUINT16;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_INT32:
      n_points = data_size / sizeof (gint32);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint32 raw_re = GINT32_FROM_LE (*((gint32 *)data + n_points));
          buffer[i].re = (gfloat) raw_re / G_MAXINT32;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_UINT32:
      n_points = data_size / sizeof (guint32);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint32 raw_re = GUINT32_FROM_LE (*((guint32 *)data + n_points));
          buffer[i].re = (gfloat) raw_re / G_MAXUINT32;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_INT64:
      n_points = data_size / sizeof (gint64);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint64 raw_re = GINT64_FROM_LE (*((gint64 *)data + n_points));
          buffer[i].re = (gfloat) raw_re / G_MAXINT64;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_UINT64:
      n_points = data_size / sizeof (guint64);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint64 raw_re = GUINT64_FROM_LE (*((guint64 *)data + n_points));
          buffer[i].re = (gfloat) raw_re / G_MAXUINT64;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_INT8:
      n_points = data_size / (2 * sizeof (gint8));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint8 raw_re = *((gint8 *)data + 2 * n_points);
          gint8 raw_im = *((gint8 *)data + 2 * n_points + 1);
          buffer[i].re = (gfloat) raw_re / G_MAXINT8;
          buffer[i].im = (gfloat) raw_im / G_MAXINT8;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_UINT8:
      n_points = data_size / (2 * sizeof (guint8));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint8 raw_re = *((guint8 *)data + 2 * n_points);
          guint8 raw_im = *((guint8 *)data + 2 * n_points + 1);
          buffer[i].re = (gfloat) raw_re / G_MAXUINT8;
          buffer[i].im = (gfloat) raw_im / G_MAXUINT8;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_INT16:
      n_points = data_size / (2 * sizeof (gint16));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint16 *)data + 2 * n_points));
          gint16 raw_im = GINT16_FROM_LE (*((gint16 *)data + 2 * n_points + 1));
          buffer[i].re = (gfloat) raw_re / G_MAXINT16;
          buffer[i].im = (gfloat) raw_im / G_MAXINT16;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_UINT16:
      n_points = data_size / (2 * sizeof (guint16));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint16 raw_re = GUINT16_FROM_LE (*((guint16 *)data + 2 * n_points));
          guint16 raw_im = GUINT16_FROM_LE (*((guint16 *)data + 2 * n_points + 1));
          buffer[i].re = (gfloat) raw_re / G_MAXUINT16;
          buffer[i].im = (gfloat) raw_im / G_MAXUINT16;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_INT32:
      n_points = data_size / (2 * sizeof (gint32));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint32 raw_re = GINT32_FROM_LE (*((gint32 *)data + 2 * n_points));
          gint32 raw_im = GINT32_FROM_LE (*((gint32 *)data + 2 * n_points + 1));
          buffer[i].re = (gfloat) raw_re / G_MAXINT32;
          buffer[i].im = (gfloat) raw_im / G_MAXINT32;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_UINT32:
      n_points = data_size / (2 * sizeof (guint32));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint32 raw_re = GUINT32_FROM_LE (*((guint32 *)data + 2 * n_points));
          guint32 raw_im = GUINT32_FROM_LE (*((guint32 *)data + 2 * n_points + 1));
          buffer[i].re = (gfloat) raw_re / G_MAXUINT32;
          buffer[i].im = (gfloat) raw_im / G_MAXUINT32;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_INT64:
      n_points = data_size / (2 * sizeof (gint64));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint64 raw_re = GINT64_FROM_LE (*((gint64 *)data + 2 * n_points));
          gint64 raw_im = GINT64_FROM_LE (*((gint64 *)data + 2 * n_points + 1));
          buffer[i].re = (gfloat) raw_re / G_MAXINT64;
          buffer[i].im = (gfloat) raw_im / G_MAXINT64;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_UINT64:
      n_points = data_size / (2 * sizeof (guint64));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint64 raw_re = GUINT64_FROM_LE (*((guint64 *)data + 2 * n_points));
          guint64 raw_im = GUINT64_FROM_LE (*((guint64 *)data + 2 * n_points + 1));
          buffer[i].re = (gfloat) raw_re / G_MAXUINT64;
          buffer[i].im = (gfloat) raw_im / G_MAXUINT64;
        }
      break;

    case HYSCAN_DATA_TYPE_FLOAT:
      n_points = data_size / sizeof (gfloat);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          buffer[i].re = *((gfloat *)data + n_points);
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_DOUBLE:
      n_points = data_size / sizeof (gdouble);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          buffer[i].re = *((gdouble *)data + n_points);
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_FLOAT:
      n_points = data_size / (2 * sizeof (gfloat));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          buffer[i].re = *((gfloat *)data + 2 * n_points);
          buffer[i].im = *((gfloat *)data + 2 * n_points + 1);
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_DOUBLE:
      n_points = data_size / (2 * sizeof (gdouble));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          buffer[i].re = *((gdouble *)data + 2 * n_points);
          buffer[i].im = *((gdouble *)data + 2 * n_points + 1);
        }
      break;

    default:
      return FALSE;

    }

  return TRUE;

}

/* Функция преобразовывает данные из низкоуровневого формата в HyScanComplexDouble размером data_size. */
gboolean
hyscan_data_import_complex_double (HyScanDataType       data_type,
                                   gpointer             data,
                                   gint32               data_size,
                                   HyScanComplexDouble *buffer,
                                   gint32              *buffer_size)
{
  gint32 i;
  gint32 n_points;

  switch (data_type)
    {
    case HYSCAN_DATA_TYPE_ADC_8BIT:
      n_points = data_size / sizeof (gint8);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint8 *)data + n_points)) & 0x0ff;
          buffer[i].re = (gdouble) raw_re / 128.0;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_ADC_10BIT:
      n_points = data_size / sizeof (gint16);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint16 *)data + n_points)) & 0x03ff;
          buffer[i].re = (gdouble) raw_re / 512.0;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_ADC_12BIT:
      n_points = data_size / sizeof (gint16);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint16 *)data + n_points)) & 0x0fff;
          buffer[i].re = (gdouble) raw_re / 2048.0;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_ADC_14BIT:
      n_points = data_size / sizeof (gint16);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint16 *)data + n_points)) & 0x3fff;
          buffer[i].re = (gdouble) raw_re / 8192.0;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_ADC_16BIT:
      n_points = data_size / sizeof (gint16);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint16 *)data + n_points));
          buffer[i].re = (gdouble) raw_re / 32768.0;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_ADC_8BIT:
      n_points = data_size / (2 * sizeof (gint8));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint8 *)data + 2 * n_points)) & 0x0ff;
          gint16 raw_im = GINT16_FROM_LE (*((gint8 *)data + 2 * n_points + 1)) & 0x0ff;
          buffer[i].re = (gdouble) raw_re / 128.0;
          buffer[i].im = (gdouble) raw_im / 128.0;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_ADC_10BIT:
      n_points = data_size / (2 * sizeof (gint16));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint16 *)data + 2 * n_points)) & 0x03ff;
          gint16 raw_im = GINT16_FROM_LE (*((gint16 *)data + 2 * n_points + 1)) & 0x03ff;
          buffer[i].re = (gdouble) raw_re / 512.0;
          buffer[i].im = (gdouble) raw_im / 512.0;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_ADC_12BIT:
      n_points = data_size / (2 * sizeof (gint16));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint16 *)data + 2 * n_points)) & 0x0fff;
          gint16 raw_im = GINT16_FROM_LE (*((gint16 *)data + 2 * n_points + 1)) & 0x0fff;
          buffer[i].re = (gdouble) raw_re / 2048.0;
          buffer[i].im = (gdouble) raw_im / 2048.0;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_ADC_14BIT:
      n_points = data_size / (2 * sizeof (gint16));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint16 *)data + 2 * n_points)) & 0x3fff;
          gint16 raw_im = GINT16_FROM_LE (*((gint16 *)data + 2 * n_points + 1)) & 0x3fff;
          buffer[i].re = (gdouble) raw_re / 8192.0;
          buffer[i].im = (gdouble) raw_im / 8192.0;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_ADC_16BIT:
      n_points = data_size / (2 * sizeof (gint16));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint16 *)data + 2 * i));
          gint16 raw_im = GINT16_FROM_LE (*((gint16 *)data + 2 * i + 1));
          buffer[i].re = (gdouble) raw_re / 32768.0;
          buffer[i].im = (gdouble) raw_im / 32768.0;
        }
      break;

    case HYSCAN_DATA_TYPE_INT8:
      n_points = data_size / sizeof (gint8);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint8 raw_re = *((gint8 *)data + n_points);
          buffer[i].re = (gdouble) raw_re / G_MAXINT8;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_UINT8:
      n_points = data_size / sizeof (guint8);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint8 raw_re = *((guint8 *)data + n_points);
          buffer[i].re = (gdouble) raw_re / G_MAXUINT8;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_INT16:
      n_points = data_size / sizeof (gint16);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint16 *)data + n_points));
          buffer[i].re = (gdouble) raw_re / G_MAXINT16;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_UINT16:
      n_points = data_size / sizeof (guint16);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint16 raw_re = GUINT16_FROM_LE (*((guint16 *)data + n_points));
          buffer[i].re = (gdouble) raw_re / G_MAXUINT16;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_INT32:
      n_points = data_size / sizeof (gint32);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint32 raw_re = GINT32_FROM_LE (*((gint32 *)data + n_points));
          buffer[i].re = (gdouble) raw_re / G_MAXINT32;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_UINT32:
      n_points = data_size / sizeof (guint32);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint32 raw_re = GUINT32_FROM_LE (*((guint32 *)data + n_points));
          buffer[i].re = (gdouble) raw_re / G_MAXUINT32;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_INT64:
      n_points = data_size / sizeof (gint64);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint64 raw_re = GINT64_FROM_LE (*((gint64 *)data + n_points));
          buffer[i].re = (gdouble) raw_re / G_MAXINT64;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_UINT64:
      n_points = data_size / sizeof (guint64);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint64 raw_re = GUINT64_FROM_LE (*((guint64 *)data + n_points));
          buffer[i].re = (gdouble) raw_re / G_MAXUINT64;
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_INT8:
      n_points = data_size / (2 * sizeof (gint8));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint8 raw_re = *((gint8 *)data + 2 * n_points);
          gint8 raw_im = *((gint8 *)data + 2 * n_points + 1);
          buffer[i].re = (gdouble) raw_re / G_MAXINT8;
          buffer[i].im = (gdouble) raw_im / G_MAXINT8;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_UINT8:
      n_points = data_size / (2 * sizeof (guint8));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint8 raw_re = *((guint8 *)data + 2 * n_points);
          guint8 raw_im = *((guint8 *)data + 2 * n_points + 1);
          buffer[i].re = (gdouble) raw_re / G_MAXUINT8;
          buffer[i].im = (gdouble) raw_im / G_MAXUINT8;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_INT16:
      n_points = data_size / (2 * sizeof (gint16));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint16 raw_re = GINT16_FROM_LE (*((gint16 *)data + 2 * n_points));
          gint16 raw_im = GINT16_FROM_LE (*((gint16 *)data + 2 * n_points + 1));
          buffer[i].re = (gdouble) raw_re / G_MAXINT16;
          buffer[i].im = (gdouble) raw_im / G_MAXINT16;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_UINT16:
      n_points = data_size / (2 * sizeof (guint16));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint16 raw_re = GUINT16_FROM_LE (*((guint16 *)data + 2 * n_points));
          guint16 raw_im = GUINT16_FROM_LE (*((guint16 *)data + 2 * n_points + 1));
          buffer[i].re = (gdouble) raw_re / G_MAXUINT16;
          buffer[i].im = (gdouble) raw_im / G_MAXUINT16;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_INT32:
      n_points = data_size / (2 * sizeof (gint32));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint32 raw_re = GINT32_FROM_LE (*((gint32 *)data + 2 * n_points));
          gint32 raw_im = GINT32_FROM_LE (*((gint32 *)data + 2 * n_points + 1));
          buffer[i].re = (gdouble) raw_re / G_MAXINT32;
          buffer[i].im = (gdouble) raw_im / G_MAXINT32;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_UINT32:
      n_points = data_size / (2 * sizeof (guint32));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint32 raw_re = GUINT32_FROM_LE (*((guint32 *)data + 2 * n_points));
          guint32 raw_im = GUINT32_FROM_LE (*((guint32 *)data + 2 * n_points + 1));
          buffer[i].re = (gdouble) raw_re / G_MAXUINT32;
          buffer[i].im = (gdouble) raw_im / G_MAXUINT32;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_INT64:
      n_points = data_size / (2 * sizeof (gint64));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          gint64 raw_re = GINT64_FROM_LE (*((gint64 *)data + 2 * n_points));
          gint64 raw_im = GINT64_FROM_LE (*((gint64 *)data + 2 * n_points + 1));
          buffer[i].re = (gdouble) raw_re / G_MAXINT64;
          buffer[i].im = (gdouble) raw_im / G_MAXINT64;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_UINT64:
      n_points = data_size / (2 * sizeof (guint64));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint64 raw_re = GUINT64_FROM_LE (*((guint64 *)data + 2 * n_points));
          guint64 raw_im = GUINT64_FROM_LE (*((guint64 *)data + 2 * n_points + 1));
          buffer[i].re = (gdouble) raw_re / G_MAXUINT64;
          buffer[i].im = (gdouble) raw_im / G_MAXUINT64;
        }
      break;

    case HYSCAN_DATA_TYPE_FLOAT:
      n_points = data_size / sizeof (gfloat);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          buffer[i].re = *((gfloat *)data + n_points);
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_DOUBLE:
      n_points = data_size / sizeof (gdouble);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          buffer[i].re = *((gdouble *)data + n_points);
          buffer[i].im = 0.0;
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_FLOAT:
      n_points = data_size / (2 * sizeof (gfloat));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          buffer[i].re = *((gfloat *)data + 2 * n_points);
          buffer[i].im = *((gfloat *)data + 2 * n_points + 1);
        }
      break;

    case HYSCAN_DATA_TYPE_COMPLEX_DOUBLE:
      n_points = data_size / (2 * sizeof (gdouble));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          buffer[i].re = *((gdouble *)data + 2 * n_points);
          buffer[i].im = *((gdouble *)data + 2 * n_points + 1);
        }
      break;

    default:
      return FALSE;

    }

  return TRUE;

}
