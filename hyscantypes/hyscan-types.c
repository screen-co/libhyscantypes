/*
 * \file hyscan-types.c
 *
 * \brief Исходный файл базовых типов данных HyScan
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2015
 * \license Проприетарная лицензия ООО "Экран"
 *
*/

#include "hyscan-types.h"

#include <string.h>

/* Типы данных и их названия. */
typedef struct
{
  GQuark                       quark;
  const gchar                 *name;
  HyScanDataType               type;
} HyScanDataTypeInfo;

/* Типы галсов и их названия. */
typedef struct
{
  GQuark                       quark;
  const gchar                 *name;

  HyScanTrackType              type;
} HyScanTrackTypeInfo;

/* Типы каналов и их названия. */
typedef struct
{
  GQuark                       quark;
  const gchar                 *name;

  HyScanSourceType             source;
  gboolean                     raw;
  guint                        channel;
} HyScanChannelTypeInfo;

/* Типы данных и их названия. */
static HyScanDataTypeInfo hyscan_data_types_info[] =
{
  { 0, "blob", HYSCAN_DATA_BLOB },
  { 0, "string", HYSCAN_DATA_STRING },

  { 0, "adc14le", HYSCAN_DATA_ADC_14LE },
  { 0, "adc16le", HYSCAN_DATA_ADC_16LE },
  { 0, "adc24le", HYSCAN_DATA_ADC_24LE },

  { 0, "complex-adc14le", HYSCAN_DATA_COMPLEX_ADC_14LE },
  { 0, "complex-adc16le", HYSCAN_DATA_COMPLEX_ADC_16LE },
  { 0, "complex-adc24le", HYSCAN_DATA_COMPLEX_ADC_16LE },

  { 0, "uint8",  HYSCAN_DATA_UINT8 },
  { 0, "uint16", HYSCAN_DATA_UINT16 },
  { 0, "uint32", HYSCAN_DATA_UINT32 },
  { 0, "float",  HYSCAN_DATA_FLOAT },

  { 0, "complex-uint8",  HYSCAN_DATA_COMPLEX_UINT8 },
  { 0, "complex-uint16", HYSCAN_DATA_COMPLEX_UINT16 },
  { 0, "complex-uint32", HYSCAN_DATA_COMPLEX_UINT32 },
  { 0, "complex-float",  HYSCAN_DATA_COMPLEX_FLOAT },

  { 0, NULL, HYSCAN_DATA_INVALID }
};

/* Типы галсов и их названия. */
static HyScanTrackTypeInfo hyscan_track_type_info[] =
{
  { 0, "calibration", HYSCAN_TRACK_CALIBRATION },
  { 0, "survey", HYSCAN_TRACK_SURVEY },
  { 0, "tack", HYSCAN_TRACK_TACK },

  { 0, NULL, HYSCAN_TRACK_UNSPECIFIED }
};

/* Типы каналов и их названия. */
static HyScanChannelTypeInfo hyscan_channel_types_info[] =
{
  { 0, "ss-starboard",         HYSCAN_SOURCE_SIDE_SCAN_STARBOARD,      FALSE, 1 },
  { 0, "ss-starboard-raw",     HYSCAN_SOURCE_SIDE_SCAN_STARBOARD,      TRUE,  1 },
  { 0, "ss-starboard-raw-2",   HYSCAN_SOURCE_SIDE_SCAN_STARBOARD,      TRUE,  2 },
  { 0, "ss-starboard-raw-3",   HYSCAN_SOURCE_SIDE_SCAN_STARBOARD,      TRUE,  3 },

  { 0, "ss-port",              HYSCAN_SOURCE_SIDE_SCAN_PORT,           FALSE, 1 },
  { 0, "ss-port-raw",          HYSCAN_SOURCE_SIDE_SCAN_PORT,           TRUE,  1 },
  { 0, "ss-port-raw-2",        HYSCAN_SOURCE_SIDE_SCAN_PORT,           TRUE,  2 },
  { 0, "ss-port-raw-3",        HYSCAN_SOURCE_SIDE_SCAN_PORT,           TRUE,  3 },

  { 0, "ss-starboard-hi",      HYSCAN_SOURCE_SIDE_SCAN_STARBOARD_HI,   FALSE, 1 },
  { 0, "ss-starboard-hi-raw",  HYSCAN_SOURCE_SIDE_SCAN_STARBOARD_HI,   TRUE,  1 },

  { 0, "ss-port-hi",           HYSCAN_SOURCE_SIDE_SCAN_PORT_HI,        FALSE, 1 },
  { 0, "ss-port-hi-raw",       HYSCAN_SOURCE_SIDE_SCAN_PORT_HI,        TRUE,  1 },

  { 0, "bathy-starboard",      HYSCAN_SOURCE_INTERFEROMETRY_STARBOARD, FALSE, 1 },
  { 0, "bathy-port",           HYSCAN_SOURCE_INTERFEROMETRY_PORT,      FALSE, 1 },

  { 0, "echosounder",          HYSCAN_SOURCE_ECHOSOUNDER,              FALSE, 1 },
  { 0, "echosounder-raw",      HYSCAN_SOURCE_ECHOSOUNDER,              TRUE,  1 },

  { 0, "profiler",             HYSCAN_SOURCE_PROFILER,                 FALSE, 1 },
  { 0, "profiler-raw",         HYSCAN_SOURCE_PROFILER,                 TRUE,  1 },

  { 0, "around-starboard",     HYSCAN_SOURCE_LOOK_AROUND_STARBOARD,    FALSE, 1 },
  { 0, "around-port",          HYSCAN_SOURCE_LOOK_AROUND_PORT,         FALSE, 1 },
  { 0, "around-starboard-raw", HYSCAN_SOURCE_LOOK_AROUND_STARBOARD,    TRUE,  1 },
  { 0, "around-port-raw",      HYSCAN_SOURCE_LOOK_AROUND_PORT,         TRUE,  1 },

  { 0, "forward-look",         HYSCAN_SOURCE_FORWARD_LOOK,             FALSE, 1 },
  { 0, "forward-look-raw-1",   HYSCAN_SOURCE_FORWARD_LOOK,             TRUE,  1 },
  { 0, "forward-look-raw-2",   HYSCAN_SOURCE_FORWARD_LOOK,             TRUE,  2 },

  { 0, "sas",                  HYSCAN_SOURCE_SAS,                      TRUE,  1 },
  { 0, "sas-2",                HYSCAN_SOURCE_SAS,                      TRUE,  2 },
  { 0, "sas-3",                HYSCAN_SOURCE_SAS,                      TRUE,  3 },
  { 0, "sas-4",                HYSCAN_SOURCE_SAS,                      TRUE,  4 },
  { 0, "sas-5",                HYSCAN_SOURCE_SAS,                      TRUE,  5 },

  { 0, "sas-v2",               HYSCAN_SOURCE_SAS_V2,                   TRUE,  1 },
  { 0, "sas-v2-2",             HYSCAN_SOURCE_SAS_V2,                   TRUE,  2 },
  { 0, "sas-v2-3",             HYSCAN_SOURCE_SAS_V2,                   TRUE,  3 },
  { 0, "sas-v2-4",             HYSCAN_SOURCE_SAS_V2,                   TRUE,  4 },
  { 0, "sas-v2-5",             HYSCAN_SOURCE_SAS_V2,                   TRUE,  5 },

  { 0, "nmea",                 HYSCAN_SOURCE_NMEA_ANY,                 TRUE,  1 },
  { 0, "nmea-gga",             HYSCAN_SOURCE_NMEA_GGA,                 TRUE,  1 },
  { 0, "nmea-rmc",             HYSCAN_SOURCE_NMEA_RMC,                 TRUE,  1 },
  { 0, "nmea-dpt",             HYSCAN_SOURCE_NMEA_DPT,                 TRUE,  1 },

  { 0, "nmea-2",               HYSCAN_SOURCE_NMEA_ANY,                 TRUE,  2 },
  { 0, "nmea-gga-2",           HYSCAN_SOURCE_NMEA_GGA,                 TRUE,  2 },
  { 0, "nmea-rmc-2",           HYSCAN_SOURCE_NMEA_RMC,                 TRUE,  2 },
  { 0, "nmea-dpt-2",           HYSCAN_SOURCE_NMEA_DPT,                 TRUE,  2 },

  { 0, "nmea-3",               HYSCAN_SOURCE_NMEA_ANY,                 TRUE,  3 },
  { 0, "nmea-gga-3",           HYSCAN_SOURCE_NMEA_GGA,                 TRUE,  3 },
  { 0, "nmea-rmc-3",           HYSCAN_SOURCE_NMEA_RMC,                 TRUE,  3 },
  { 0, "nmea-dpt-3",           HYSCAN_SOURCE_NMEA_DPT,                 TRUE,  3 },

  { 0, "nmea-4",               HYSCAN_SOURCE_NMEA_ANY,                 TRUE,  4 },
  { 0, "nmea-gga-4",           HYSCAN_SOURCE_NMEA_GGA,                 TRUE,  4 },
  { 0, "nmea-rmc-4",           HYSCAN_SOURCE_NMEA_RMC,                 TRUE,  4 },
  { 0, "nmea-dpt-4",           HYSCAN_SOURCE_NMEA_DPT,                 TRUE,  4 },

  { 0, "nmea-5",               HYSCAN_SOURCE_NMEA_ANY,                 TRUE,  5 },
  { 0, "nmea-gga-5",           HYSCAN_SOURCE_NMEA_GGA,                 TRUE,  5 },
  { 0, "nmea-rmc-5",           HYSCAN_SOURCE_NMEA_RMC,                 TRUE,  5 },
  { 0, "nmea-dpt-5",           HYSCAN_SOURCE_NMEA_DPT,                 TRUE,  5 },

  { 0, NULL,                   HYSCAN_SOURCE_INVALID,                  FALSE, 0 }
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

  for (i = 0; hyscan_track_type_info[i].name != NULL; i++)
    hyscan_track_type_info[i].quark = g_quark_from_static_string (hyscan_track_type_info[i].name);

  for (i = 0; hyscan_channel_types_info[i].name != NULL; i++)
    hyscan_channel_types_info[i].quark = g_quark_from_static_string (hyscan_channel_types_info[i].name);

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
guint32
hyscan_data_get_point_size (HyScanDataType data_type)
{
  switch (data_type)
    {
    case HYSCAN_DATA_BLOB:
    case HYSCAN_DATA_STRING:
    case HYSCAN_DATA_UINT8:
      return sizeof (gchar);

    case HYSCAN_DATA_ADC_14LE:
    case HYSCAN_DATA_ADC_16LE:
    case HYSCAN_DATA_UINT16:
      return sizeof (guint16);

    case HYSCAN_DATA_ADC_24LE:
    case HYSCAN_DATA_UINT32:
      return sizeof (guint32);

    case HYSCAN_DATA_COMPLEX_UINT8:
      return 2 * sizeof (guint8);

    case HYSCAN_DATA_COMPLEX_ADC_14LE:
    case HYSCAN_DATA_COMPLEX_ADC_16LE:
    case HYSCAN_DATA_COMPLEX_UINT16:
      return 2 * sizeof (guint16);

    case HYSCAN_DATA_COMPLEX_ADC_24LE:
    case HYSCAN_DATA_COMPLEX_UINT32:
      return 2 * sizeof (guint32);

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
                          gconstpointer   data,
                          guint32         data_size,
                          gfloat         *buffer,
                          guint32        *buffer_size)
{
  guint32 i;
  guint32 n_points;

  switch (data_type)
    {
    case HYSCAN_DATA_ADC_14LE:
      n_points = data_size / sizeof (guint16);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint16 raw_re = GUINT16_FROM_LE (*((guint16 *)data + i)) & 0x3fff;
          buffer[i] = 2.0 * (raw_re / 16383.0) - 1.0;
        }
      break;

    case HYSCAN_DATA_ADC_16LE:
      n_points = data_size / sizeof (guint16);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint16 raw_re = GUINT16_FROM_LE (*((guint16 *)data + i));
          buffer[i] = 2.0 * (raw_re / 65535.0) - 1.0;
        }
      break;

    case HYSCAN_DATA_ADC_24LE:
      n_points = data_size / sizeof (guint32);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint32 raw_re = GUINT32_FROM_LE (*((guint32 *)data + i)) & 0x00ffffff;
          buffer[i] = 2.0 * (raw_re / 16777215.0) - 1.0;
        }
      break;

    case HYSCAN_DATA_UINT8:
      n_points = data_size / sizeof (guint8);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint8 raw_re = *((guint8 *)data + i);
          buffer[i] = raw_re / 255.0;
        }
      break;

    case HYSCAN_DATA_UINT16:
      n_points = data_size / sizeof (guint16);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint16 raw_re = GUINT16_FROM_LE (*((guint16 *)data + i));
          buffer[i] = raw_re / 65535.0;
        }
      break;

    case HYSCAN_DATA_UINT32:
      n_points = data_size / sizeof (guint32);
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint32 raw_re = GUINT32_FROM_LE (*((guint32 *)data + i));
          buffer[i] = raw_re / 4294967296.0;
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
                                  gconstpointer       data,
                                  guint32             data_size,
                                  HyScanComplexFloat *buffer,
                                  guint32            *buffer_size)
{
  guint32 i;
  guint32 n_points;

  switch (data_type)
    {
    case HYSCAN_DATA_COMPLEX_ADC_14LE:
      n_points = data_size / (2 * sizeof (guint16));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint16 raw_re = GUINT16_FROM_LE (*((guint16 *)data + 2 * i)) & 0x3fff;
          guint16 raw_im = GUINT16_FROM_LE (*((guint16 *)data + 2 * i + 1)) & 0x3fff;
          buffer[i].re = 2.0 * (gfloat)(raw_re / 16383.0) - 1.0;
          buffer[i].im = 2.0 * (gfloat)(raw_im / 16383.0) - 1.0;
        }
      break;

    case HYSCAN_DATA_COMPLEX_ADC_16LE:
      n_points = data_size / (2 * sizeof (guint16));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint16 raw_re = GUINT16_FROM_LE (*((guint16 *)data + 2 * i));
          guint16 raw_im = GUINT16_FROM_LE (*((guint16 *)data + 2 * i + 1));
          buffer[i].re = 2.0 * (gfloat)(raw_re / 65535.0) - 1.0;
          buffer[i].im = 2.0 * (gfloat)(raw_im / 65535.0) - 1.0;
        }
      break;

    case HYSCAN_DATA_COMPLEX_ADC_24LE:
      n_points = data_size / (2 * sizeof (guint32));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint32 raw_re = GUINT32_FROM_LE (*((guint32 *)data + 2 * i)) & 0x00ffffff;
          guint32 raw_im = GUINT32_FROM_LE (*((guint32 *)data + 2 * i + 1)) & 0x00ffffff;
          buffer[i].re = 2.0 * (gfloat)(raw_re / 16777215.0) - 1.0;
          buffer[i].im = 2.0 * (gfloat)(raw_im / 16777215.0) - 1.0;
        }
      break;

    case HYSCAN_DATA_COMPLEX_UINT8:
      n_points = data_size / (2 * sizeof (guint8));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint8 raw_re = *((guint8 *)data + 2 * i);
          guint8 raw_im = *((guint8 *)data + 2 * i + 1);
          buffer[i].re = raw_re / 255.0;
          buffer[i].im = raw_im / 255.0;
        }
      break;

    case HYSCAN_DATA_COMPLEX_UINT16:
      n_points = data_size / (2 * sizeof (guint16));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint16 raw_re = GUINT16_FROM_LE (*((guint16 *)data + 2 * i));
          guint16 raw_im = GUINT16_FROM_LE (*((guint16 *)data + 2 * i + 1));
          buffer[i].re = raw_re / 65535.0;
          buffer[i].im = raw_im / 65535.0;
        }
      break;

    case HYSCAN_DATA_COMPLEX_UINT32:
      n_points = data_size / (2 * sizeof (guint32));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      for (i = 0; i < n_points; i++)
        {
          guint32 raw_re = GUINT32_FROM_LE (*((guint32 *)data + 2 * i));
          guint32 raw_im = GUINT32_FROM_LE (*((guint32 *)data + 2 * i + 1));
          buffer[i].re = raw_re / 4294967296.0;
          buffer[i].im = raw_im / 4294967296.0;
        }
      break;

    case HYSCAN_DATA_COMPLEX_FLOAT:
      n_points = data_size / (2 * sizeof (gfloat));
      *buffer_size = n_points = (n_points > *buffer_size) ? *buffer_size : n_points;
      memcpy (buffer, data, n_points * 2 * sizeof (gfloat));
      break;

    default:
      return FALSE;

    }

  return TRUE;
}

/* Функция проверяет тип источника данных на соответствие одному из типов датчиков. */
gboolean
hyscan_source_is_sensor (HyScanSourceType source)
{
  switch (source)
    {
    case HYSCAN_SOURCE_SAS:
    case HYSCAN_SOURCE_SAS_V2:
    case HYSCAN_SOURCE_NMEA_ANY:
    case HYSCAN_SOURCE_NMEA_GGA:
    case HYSCAN_SOURCE_NMEA_RMC:
    case HYSCAN_SOURCE_NMEA_DPT:
      return TRUE;

    default:
      return FALSE;
    }

  return FALSE;
}

/* Функция проверяет тип источника данных на соответствие "сырым" гидролокационным данным. */
gboolean
hyscan_source_is_raw (HyScanSourceType source)
{
  switch (source)
    {
    case HYSCAN_SOURCE_SIDE_SCAN_STARBOARD:
    case HYSCAN_SOURCE_SIDE_SCAN_PORT:
    case HYSCAN_SOURCE_SIDE_SCAN_STARBOARD_HI:
    case HYSCAN_SOURCE_SIDE_SCAN_PORT_HI:
    case HYSCAN_SOURCE_ECHOSOUNDER:
    case HYSCAN_SOURCE_PROFILER:
    case HYSCAN_SOURCE_LOOK_AROUND_STARBOARD:
    case HYSCAN_SOURCE_LOOK_AROUND_PORT:
    case HYSCAN_SOURCE_FORWARD_LOOK:
      return TRUE;

    default:
      return FALSE;
    }

  return FALSE;
}

/* Функция проверяет тип источника данных на соответствие акустическим данным. */
gboolean
hyscan_source_is_acoustic (HyScanSourceType source)
{
  switch (source)
    {
    case HYSCAN_SOURCE_SIDE_SCAN_STARBOARD:
    case HYSCAN_SOURCE_SIDE_SCAN_PORT:
    case HYSCAN_SOURCE_SIDE_SCAN_STARBOARD_HI:
    case HYSCAN_SOURCE_SIDE_SCAN_PORT_HI:
    case HYSCAN_SOURCE_ECHOSOUNDER:
    case HYSCAN_SOURCE_PROFILER:
    case HYSCAN_SOURCE_LOOK_AROUND_STARBOARD:
    case HYSCAN_SOURCE_LOOK_AROUND_PORT:
      return TRUE;

    default:
      return FALSE;
    }

  return FALSE;
}

/* Функция возвращает название типа галса. */
const gchar *
hyscan_track_get_name_by_type (HyScanTrackType type)
{
  guint i;

  /* Инициализация статических данных. */
  hyscan_types_initialize ();

  /* Ищем название типа. */
  for (i = 0; hyscan_track_type_info[i].quark != 0; i++)
    {
      if (hyscan_track_type_info[i].type != type)
        continue;
      return hyscan_track_type_info[i].name;
    }

  return NULL;
}

/* Функция возвращает тип галса по его названию. */
HyScanTrackType
hyscan_track_get_type_by_name (const gchar *name)
{
  GQuark quark;
  guint i;

  /* Инициализация статических данных. */
  hyscan_types_initialize ();

  /* Ищем тип по названию. */
  quark = g_quark_try_string (name);
  for (i = 0; hyscan_track_type_info[i].quark != 0; i++)
    if (hyscan_track_type_info[i].quark == quark)
      return hyscan_track_type_info[i].type;

  return HYSCAN_TRACK_UNSPECIFIED;
}

/* Функция возвращает название канала для указанных характеристик. */
const gchar *
hyscan_channel_get_name_by_types (HyScanSourceType source,
                                  gboolean         raw,
                                  guint            channel)
{
  guint i;

  /* Инициализация статических данных. */
  hyscan_types_initialize ();

  /* Ищем название канала для указанных характеристик. */
  for (i = 0; hyscan_channel_types_info[i].quark != 0; i++)
    {
      if (hyscan_channel_types_info[i].source != source)
        continue;
      if (hyscan_channel_types_info[i].raw != raw)
        continue;
      if (hyscan_channel_types_info[i].channel != channel)
        continue;
      return hyscan_channel_types_info[i].name;
    }

  return NULL;
}

/* Функция возвращает характеристики канала данных по его имени. */
gboolean
hyscan_channel_get_types_by_name (const gchar      *name,
                                  HyScanSourceType *source,
                                  gboolean         *raw,
                                  guint            *channel)
{
  GQuark quark;
  guint i;

  /* Инициализация статических данных. */
  hyscan_types_initialize ();

  /* Ищем канал с указанным именем. */
  quark = g_quark_try_string (name);
  for (i = 0; hyscan_channel_types_info[i].quark != 0; i++)
    {
      if (hyscan_channel_types_info[i].quark == quark)
        {
          if (source != NULL)
            *source = hyscan_channel_types_info[i].source;
          if (raw != NULL)
            *raw = hyscan_channel_types_info[i].raw;
          if (channel != NULL)
            *channel = hyscan_channel_types_info[i].channel;
          return TRUE;
        }
    }

  return FALSE;
}
