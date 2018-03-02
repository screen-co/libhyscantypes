/* hyscan-types.c
 *
 * Copyright 2015-2017 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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

/**
 * SECTION: hyscan-types
 * @Short_description: базовые типы данных HyScan
 * @Title: HyScanTypes
 *
 * #HyScanTypes содержит определения базовых типов данных HyScan и функции
 * для работы с ними.
 *
 * Все данные в HyScan имеют определённый тип #HyScanDataType. Однако вся
 * обработка численных данных производится в виде gfloat или #HyScanComplexFloat.
 *  Для определения типа данных и его
 * характеристик используются следующие функции:
 *
 * - #hyscan_data_get_type_by_name - функция определяет тип данных по имени;
 * - #hyscan_data_get_type_name - функция возвращает название данных для указанного типа;
 * - #hyscan_data_get_point_size - функция возвращает размер одного элемента данных в байтах.
 *
 * Эти типы используются только для данных записанных в системе хранения. В
 * самом HyScan обработка данных производится в виде действительных данных
 * (тип gfloat) или комплексных (тип #HyScanComplexFloat). Для преобразования
 * данных между различными типами можно использовать класс #HyScanBuffer. При
 * преобразовании данных производится нормирование значений до величины
 * [-1.0, 1.0] для действительных и комплексных значений, [0.0, 1.0] для
 * амплитудных значений.
 *
 * Кроме этого вводится понятие - источник данных #HyScanSourceType. Все
 * источники данных разделены на два основных вида:
 *
 * - датчики
 * - источники гидролокационных данных.
 *
 * Источники гидролокационных данных разделяются на следующие виды:
 *
 * - источники сырых гидролокационных данных;
 * - источники акустических данных;
 * - источники батиметрических данных.
 *
 * Источники сырых гидролокационных данных содержат информацию об амплитуде эхо
 * сигнала дискретизированной во времени. Это вид первичной информации,
 * получаемой от приёмников гидролокатора. В зависимости от типа гидролокатора,
 * информация данного типа требует специализированной обработки, например
 * свёртки или вычисления амплитуды. Кроме этого источники сырых данных могут
 * иметь несколько каналов. Индексирование каналов начинается с единицы. Обычно
 * несколько каналов данных имеют гидролокаторы использующие фазовую обработку,
 * например интерферометры.
 *
 * Источники акустических данных содержат обработанную амплитудную информацию
 * от гидролокаторов бокового обзора, эхолота, профилографа и т.п.
 *
 * Источники батиметрических данных содержат обработанную информацию о глубине.
 *
 * Функции #hyscan_source_get_name_by_type и #hyscan_source_get_type_by_name
 * используются для получения названия источника данных по его типу и наоборот.
 *
 * Функции #hyscan_source_is_sensor, #hyscan_source_is_sonar, #hyscan_source_is_raw
 * и #hyscan_source_is_acoustic используются для проверки принадлежности источника
 * данных к определённому типу.
 *
 * Запись гидролокационных данных ведётся в так называемые галсы. Исторически,
 * галсом называется прямолинейный участок движения судна, на котором производится
 * гидроакустическая съёмка. Несколько галсов объединяются в проекты, которые
 * находятся в системе хранения - #HyScanDB.
 *
 * При записи галса, пользователь должен выбрать один из типов #HyScanTrackType
 * в качестве вспомогательной информации. Функции #hyscan_track_get_name_by_type
 * и #hyscan_track_get_type_by_name используются для преобразования типа галсов
 * в строковое представление и наоборот.
 *
 * Совокупность источника данных, его типа и номера канала образует логический
 * канал данных в системе хранения. Функции #hyscan_channel_get_name_by_types
 * и #hyscan_channel_get_types_by_name используются для преобразования типа
 * источников данных в название канала данных и наоборот.
 *
 * Данные, записанные в каналы системы хранения, имеют дополнительную метаинформацию.
 * Для её представления используются структуры: #HyScanSoundVelocity, #HyScanAntennaPosition,
 * #HyScanRawDataInfo и #HyScanAcousticDataInfo.
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

/* Уровни сообщений и их названия. */
typedef struct
{
  GQuark                       quark;
  const gchar                 *name;

  HyScanLogLevel               level;
} HyScanLogLevelInfo;

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
  { 0, "blob",                 HYSCAN_DATA_BLOB },
  { 0, "string",               HYSCAN_DATA_STRING },
  { 0, "float",                HYSCAN_DATA_FLOAT },
  { 0, "complex-float",        HYSCAN_DATA_COMPLEX_FLOAT },

  { 0, "adc14le",              HYSCAN_DATA_ADC_14LE },
  { 0, "adc16le",              HYSCAN_DATA_ADC_16LE },
  { 0, "adc24le",              HYSCAN_DATA_ADC_24LE },

  { 0, "complex-adc14le",      HYSCAN_DATA_COMPLEX_ADC_14LE },
  { 0, "complex-adc16le",      HYSCAN_DATA_COMPLEX_ADC_16LE },
  { 0, "complex-adc24le",      HYSCAN_DATA_COMPLEX_ADC_24LE },

  { 0, "amplitude-int8",       HYSCAN_DATA_AMPLITUDE_INT8 },
  { 0, "amplitude-int16",      HYSCAN_DATA_AMPLITUDE_INT16 },
  { 0, "amplitude-int32",      HYSCAN_DATA_AMPLITUDE_INT32 },
  { 0, "amplitude-float8",     HYSCAN_DATA_AMPLITUDE_FLOAT8 },
  { 0, "amplitude-float16",    HYSCAN_DATA_AMPLITUDE_FLOAT16 },

  { 0, NULL,                   HYSCAN_DATA_INVALID }
};

/* Уровни сообщений и их названия. */
static HyScanLogLevelInfo hyscan_log_level_info[] =
{
  { 0, "debug",                HYSCAN_LOG_LEVEL_DEBUG },
  { 0, "info",                 HYSCAN_LOG_LEVEL_INFO },
  { 0, "message",              HYSCAN_LOG_LEVEL_MESSAGE },
  { 0, "warning",              HYSCAN_LOG_LEVEL_WARNING },
  { 0, "critical",             HYSCAN_LOG_LEVEL_CRITICAL },
  { 0, "error",                HYSCAN_LOG_LEVEL_ERROR },

  { 0, NULL,                   0 }
};

/* Типы галсов и их названия. */
static HyScanTrackTypeInfo hyscan_track_type_info[] =
{
  { 0, "calibration",          HYSCAN_TRACK_CALIBRATION },
  { 0, "survey",               HYSCAN_TRACK_SURVEY },
  { 0, "tack",                 HYSCAN_TRACK_TACK },

  { 0, NULL,                   HYSCAN_TRACK_UNSPECIFIED }
};

/* Типы каналов и их названия. */
static HyScanChannelTypeInfo hyscan_channel_types_info[] =
{
  { 0, "log",                  HYSCAN_SOURCE_LOG,                      FALSE, 1 },

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

  { 0, "echosounder",          HYSCAN_SOURCE_ECHOSOUNDER,              FALSE, 1 },
  { 0, "echosounder-raw",      HYSCAN_SOURCE_ECHOSOUNDER,              TRUE,  1 },

  { 0, "echosounder-hi",       HYSCAN_SOURCE_ECHOSOUNDER_HI,           FALSE, 1 },
  { 0, "echosounder-hi-raw",   HYSCAN_SOURCE_ECHOSOUNDER_HI,           TRUE,  1 },

  { 0, "bathy-starboard",      HYSCAN_SOURCE_BATHYMETRY_STARBOARD,     FALSE, 1 },
  { 0, "bathy-port",           HYSCAN_SOURCE_BATHYMETRY_PORT,          FALSE, 1 },

  { 0, "profiler",             HYSCAN_SOURCE_PROFILER,                 FALSE, 1 },
  { 0, "profiler-raw",         HYSCAN_SOURCE_PROFILER,                 TRUE,  1 },

  { 0, "echoprofiler",         HYSCAN_SOURCE_ECHOPROFILER,             FALSE, 1 },
  { 0, "echoprofiler-raw",     HYSCAN_SOURCE_ECHOPROFILER,             TRUE,  1 },

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
  guint i;

  if (hyscan_types_initialized)
    return;

  for (i = 0; hyscan_data_types_info[i].name != NULL; i++)
    hyscan_data_types_info[i].quark = g_quark_from_static_string (hyscan_data_types_info[i].name);

  for (i = 0; hyscan_log_level_info[i].name != NULL; i++)
    hyscan_log_level_info[i].quark = g_quark_from_static_string (hyscan_log_level_info[i].name);

  for (i = 0; hyscan_track_type_info[i].name != NULL; i++)
    hyscan_track_type_info[i].quark = g_quark_from_static_string (hyscan_track_type_info[i].name);

  for (i = 0; hyscan_channel_types_info[i].name != NULL; i++)
    hyscan_channel_types_info[i].quark = g_quark_from_static_string (hyscan_channel_types_info[i].name);

  hyscan_types_initialized = TRUE;
}

/**
 * hyscan_antenna_position_copy:
 * @position: структура #HyScanAntennaPosition для копирования
 *
 * Функция создаёт копию структуры #HyScanAntennaPosition.
 *
 * Returns: (transfer full): Новая структура #HyScanAntennaPosition.
 * Для удаления #hyscan_antenna_position_free.
 */
HyScanAntennaPosition *
hyscan_antenna_position_copy (const HyScanAntennaPosition *position)
{
  if (position != NULL)
    return g_slice_dup (HyScanAntennaPosition, position);

  return NULL;
}

/**
 * hyscan_antenna_position_free:
 * @position: структура #HyScanAntennaPosition для удаления
 *
 * Функция удаляет структуру #HyScanAntennaPosition.
 */
void
hyscan_antenna_position_free (HyScanAntennaPosition *position)
{
  if (position != NULL)
    g_slice_free (HyScanAntennaPosition, position);
}

/**
 * hyscan_raw_data_info_copy:
 * @info: структура #HyScanRawDataInfo для копирования
 *
 * Функция создаёт копию структуры #HyScanRawDataInfo.
 *
 * Returns: (transfer full): Новая структура #HyScanRawDataInfo.
 * Для удаления #hyscan_raw_data_info_free.
 */
HyScanRawDataInfo *
hyscan_raw_data_info_copy (const HyScanRawDataInfo *info)
{
  if (info != NULL)
    return g_slice_dup (HyScanRawDataInfo, info);

  return NULL;
}

/**
 * hyscan_raw_data_info_free:
 * @info: структура #HyScanRawDataInfo для удаления
 *
 * Функция удаляет структуру #HyScanRawDataInfo.
 */
void
hyscan_raw_data_info_free (HyScanRawDataInfo *info)
{
  if (info != NULL)
    g_slice_free (HyScanRawDataInfo, info);
}

/**
 * hyscan_acoustic_data_info_copy:
 * @info: структура #HyScanAcousticDataInfo для копирования
 *
 * Функция создаёт копию структуры #HyScanAcousticDataInfo.
 *
 * Returns: (transfer full): Новая структура #HyScanAcousticDataInfo.
 * Для удаления #hyscan_acoustic_data_info_free.
 */
HyScanAcousticDataInfo *
hyscan_acoustic_data_info_copy (const HyScanAcousticDataInfo *info)
{
  if (info != NULL)
    return g_slice_dup (HyScanAcousticDataInfo, info);

  return NULL;
}

/**
 * hyscan_acoustic_data_info_free:
 * @info: структура #HyScanAcousticDataInfo для удаления
 *
 * Функция удаляет структуру #HyScanAcousticDataInfo.
 */
void
hyscan_acoustic_data_info_free (HyScanAcousticDataInfo *info)
{
  if (info != NULL)
    g_slice_free (HyScanAcousticDataInfo, info);
}

G_DEFINE_BOXED_TYPE (HyScanAntennaPosition, hyscan_antenna_position, hyscan_antenna_position_copy, hyscan_antenna_position_free)

G_DEFINE_BOXED_TYPE (HyScanRawDataInfo, hyscan_raw_data_info, hyscan_raw_data_info_copy, hyscan_raw_data_info_free)

G_DEFINE_BOXED_TYPE (HyScanAcousticDataInfo, hyscan_acoustic_data_info, hyscan_acoustic_data_info_copy, hyscan_acoustic_data_info_free)

/**
 * hyscan_data_get_name_by_type:
 * @type: тип данныx
 *
 * Функция преобразовывает нумерованное значение типа данных в название типа.
 *
 * Returns: Строка с названием типа данных.
 */
const gchar *
hyscan_data_get_name_by_type (HyScanDataType data_type)
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

/**
 * hyscan_data_get_type_by_name:
 * @name: название типа данныx
 *
 * Функция преобразовывает строку с названием типа данных в нумерованное значение.
 *
 * Returns: Тип данных.
 */
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

/**
 * hyscan_data_get_point_size:
 * @type: тип данныx
 *
 * Функция возвращает размер одного элемента данных в байтах, для указанного типа.
 *
 * Returns: Размер одного елемента данных в байтах.
 */
guint32
hyscan_data_get_point_size (HyScanDataType data_type)
{
  switch (data_type)
    {
    case HYSCAN_DATA_BLOB:
    case HYSCAN_DATA_STRING:
    case HYSCAN_DATA_AMPLITUDE_INT8:
    case HYSCAN_DATA_AMPLITUDE_FLOAT8:
      return sizeof (guint8);

    case HYSCAN_DATA_ADC_14LE:
    case HYSCAN_DATA_ADC_16LE:
    case HYSCAN_DATA_AMPLITUDE_INT16:
    case HYSCAN_DATA_AMPLITUDE_FLOAT16:
      return sizeof (guint16);

    case HYSCAN_DATA_ADC_24LE:
    case HYSCAN_DATA_AMPLITUDE_INT32:
      return sizeof (guint32);

    case HYSCAN_DATA_COMPLEX_ADC_14LE:
    case HYSCAN_DATA_COMPLEX_ADC_16LE:
      return 2 * sizeof (guint16);

    case HYSCAN_DATA_COMPLEX_ADC_24LE:
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

/**
 * hyscan_source_get_name_by_type:
 * @source: тип источника данныx
 *
 * Функция возвращает название источника данных по его типу.
 *
 * Returns: Название источника данных или NULL.
 */
const gchar *
hyscan_source_get_name_by_type (HyScanSourceType source)
{
  return hyscan_channel_get_name_by_types (source, FALSE, 1);
}

/**
 * hyscan_source_get_type_by_name:
 * @name: название источника данныx
 *
 * Функция возвращает тип источника данных по его названию.
 *
 * Returns: Тип источника данных.
 */
HyScanSourceType
hyscan_source_get_type_by_name (const gchar *name)
{
  HyScanSourceType source;

  if (!hyscan_channel_get_types_by_name (name, &source, NULL, NULL))
    return HYSCAN_SOURCE_INVALID;

  return source;
}

/**
 * hyscan_source_is_sensor:
 * @source: тип источника данныx
 *
 * Функция проверяет тип источника данных на соответствие одному из типов датчиков.
 *
 * Returns: %TRUE если источник данных является датчиком, иначе %FALSE.
 */
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

/**
 * hyscan_source_is_sonar:
 * @source: тип источника данныx
 *
 * Функция проверяет тип источника данных на соответствие одному из
 * гидролокационных типов.
 *
 * Returns: %TRUE если источник данных является гидролокационным
 * источником, иначе %FALSE.
 */
gboolean
hyscan_source_is_sonar (HyScanSourceType source)
{
  switch (source)
    {
    case HYSCAN_SOURCE_SIDE_SCAN_STARBOARD:
    case HYSCAN_SOURCE_SIDE_SCAN_PORT:
    case HYSCAN_SOURCE_SIDE_SCAN_STARBOARD_HI:
    case HYSCAN_SOURCE_SIDE_SCAN_PORT_HI:
    case HYSCAN_SOURCE_ECHOSOUNDER:
    case HYSCAN_SOURCE_ECHOSOUNDER_HI:
    case HYSCAN_SOURCE_BATHYMETRY_STARBOARD:
    case HYSCAN_SOURCE_BATHYMETRY_PORT:
    case HYSCAN_SOURCE_PROFILER:
    case HYSCAN_SOURCE_ECHOPROFILER:
    case HYSCAN_SOURCE_LOOK_AROUND_STARBOARD:
    case HYSCAN_SOURCE_LOOK_AROUND_PORT:
    case HYSCAN_SOURCE_FORWARD_LOOK:
      return TRUE;

    default:
      return FALSE;
    }

  return FALSE;
}

/**
 * hyscan_source_is_raw:
 * @source: тип источника данныx
 *
 * Функция проверяет тип источника данных на соответствие сырым гидролокационным данным.
 *
 * Returns: %TRUE если источник данных является источником сырых данных, иначе %FALSE.
 */
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
    case HYSCAN_SOURCE_ECHOSOUNDER_HI:
    case HYSCAN_SOURCE_PROFILER:
    case HYSCAN_SOURCE_ECHOPROFILER:
    case HYSCAN_SOURCE_LOOK_AROUND_STARBOARD:
    case HYSCAN_SOURCE_LOOK_AROUND_PORT:
    case HYSCAN_SOURCE_FORWARD_LOOK:
      return TRUE;

    default:
      return FALSE;
    }

  return FALSE;
}

/**
 * hyscan_source_is_acoustic:
 * @source: тип источника данныx
 *
 * Функция проверяет тип источника данных на соответствие акустическим данным.
 *
 * Returns: %TRUE если источник данных является источником акустических данных, иначе %FALSE.
 */
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
    case HYSCAN_SOURCE_ECHOSOUNDER_HI:
    case HYSCAN_SOURCE_PROFILER:
    case HYSCAN_SOURCE_ECHOPROFILER:
    case HYSCAN_SOURCE_LOOK_AROUND_STARBOARD:
    case HYSCAN_SOURCE_LOOK_AROUND_PORT:
      return TRUE;

    default:
      return FALSE;
    }

  return FALSE;
}

/**
 * hyscan_log_level_get_name_by_type:
 * @level: тип сообщения
 *
 * Функция возвращает название типа информационного сообщения.
 *
 * Returns: Название типа информационного сообщения или NULL.
 */
const gchar *
hyscan_log_level_get_name_by_type (HyScanLogLevel level)
{
  guint i;

  /* Инициализация статических данных. */
  hyscan_types_initialize ();

  /* Ищем название типа. */
  for (i = 0; hyscan_log_level_info[i].quark != 0; i++)
    {
      if (hyscan_log_level_info[i].level != level)
        continue;
      return hyscan_log_level_info[i].name;
    }

  return NULL;
}

/**
 * hyscan_log_level_get_type_by_name:
 * @name: название типа сообщения
 *
 * Функция возвращает тип информационного сообщения по его названию.
 *
 * Returns: Тип информационного сообщения.
 */
HyScanLogLevel
hyscan_log_level_get_type_by_name (const gchar *name)
{
  GQuark quark;
  guint i;

  /* Инициализация статических данных. */
  hyscan_types_initialize ();

  /* Ищем тип по названию. */
  quark = g_quark_try_string (name);
  for (i = 0; hyscan_log_level_info[i].quark != 0; i++)
    if (hyscan_log_level_info[i].quark == quark)
      return hyscan_log_level_info[i].level;

  return 0;
}

/**
 * hyscan_track_get_name_by_type:
 * @type: тип галса
 *
 * Функция возвращает название типа галса.
 *
 * Returns: Название типа галса или NULL.
 */
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

/**
 * hyscan_track_get_type_by_name:
 * @name: название типа галса
 *
 * Функция возвращает тип галса по его названию.
 *
 * Returns: Тип галса.
 */
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

/**
 * hyscan_channel_get_name_by_types:
 * @source: тип источника данныx
 * @raw: признак сырых данныx
 * @channel: индекс канала данных, начиная с 1
 *
 * Функция возвращает название канала для указанных характеристик.
 * Строка, возвращаемая этой функцией, не должна изменяться пользователем.
 *
 * Returns: Название канала данных или NULL.
 */
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

/**
 * hyscan_channel_get_types_by_name:
 * @name: название канала данныx
 * @source: (out) (allow-none): тип источника данныx
 * @raw: (out) (allow-none): признак сырых данныx
 * @channel: (out) (allow-none): индекс канала данныx
 *
 * Функция возвращает характеристики канала данных по его имени.
 *
 * Returns: %TRUE если характеристики канала определены, иначе %FALSE.
 */
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
