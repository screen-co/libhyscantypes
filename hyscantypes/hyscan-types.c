/* hyscan-types.c
 *
 * Copyright 2015-2018 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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
 * SECTION: hyscan-types
 * @Short_description: базовые типы данных HyScan
 * @Title: HyScanTypes
 *
 * #HyScanTypes содержит определения базовых типов данных HyScan и функции
 * для работы с ними.
 *
 * Все данные в HyScan имеют определённый тип #HyScanDataType. Для определения
 * типа данных и его характеристик используются следующие функции:
 *
 * - #hyscan_data_get_name_by_type - функция возвращает название данных для указанного типа;
 * - #hyscan_data_get_type_by_name - функция определяет тип данных по имени;
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
 * Тип дискретизации данных определяется как #HyScandiscretizationType. Для
 * определения типа дискретизации и её характеристик используются следующие
 * функции:
 *
 * - #hyscan_discretization_get_name_by_type - функция возвращает название дискретизации для указанного типа.
 * - #hyscan_discretization_get_type_by_name - функция определяет тип дискретизации по имени;
 * - #hyscan_discretization_get_type_by_data - функция возвращает тип дискретизации данных по их типу.
 *
 * Также вводится понятие источник данных - #HyScanSourceType. Источники
 * данных содержат гидролокационную информацию, например амплитуду эхо
 * сигнала дискретизированную во времени, и данные от датчиков, например
 * NMEA строки.
 *
 * Функции #hyscan_source_get_name_by_type и #hyscan_source_get_type_by_name
 * используются для получения названия источника данных по его типу и наоборот.
 *
 * Функции #hyscan_source_is_sensor и #hyscan_source_is_sonar используются
 * для проверки принадлежности источника данных к определённому классу данных:
 * гидролкационные данные или данные датчиков.
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
 * Совокупность источника данных и его типа образует логический канал данных в
 * системе хранения. Данные, записанные в каналы системы хранения, имеют
 * дополнительную метаинформацию. Для её представления используются структуры:
 * #HyScanSoundVelocity, #HyScanAntennaOffset, #HyScanRawDataInfo и
 * #HyScanAcousticDataInfo.
 *
 * Для формирования названия канала данных можно использовать функцию
 * #hyscan_channel_get_name_by_types. Функция #hyscan_channel_get_types_by_name
 * используется для получения типа источника данных по названию канала.
 */

#include "hyscan-types.h"

#include <libxml/parser.h>
#include <stdlib.h>
#include <string.h>

#define DATA_CHANNEL_PREFIX    ""
#define NOISE_CHANNEL_PREFIX   "-noise"
#define SIGNAL_CHANNEL_PREFIX  "-signal"
#define TVG_CHANNEL_PREFIX     "-tvg"

/* Типы данных и их названия. */
typedef struct
{
  GQuark                       quark;
  const gchar                 *name;
  HyScanDataType               type;
  guint32                      size;
  HyScanDiscretizationType     discretization;
} HyScanDataTypeInfo;

/* Типы дискретизации и их названия. */
typedef struct
{
  GQuark                       quark;
  const gchar                 *name;
  HyScanDiscretizationType     type;
} HyScanDiscretizationTypeInfo;

/* Типы источников данных и их названия. */
typedef struct
{
  GQuark                       quark;
  const gchar                 *name;
  HyScanSourceType             type;
} HyScanSourceTypeInfo;

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

/* Типы данных и их названия. */
static HyScanDataTypeInfo hyscan_data_types_info[] =
{
  { 0, "blob",                 HYSCAN_DATA_BLOB,
    sizeof (guint8),           HYSCAN_DISCRETIZATION_INVALID },
  { 0, "string",               HYSCAN_DATA_STRING,
    sizeof (guint8),           HYSCAN_DISCRETIZATION_INVALID },
  { 0, "float",                HYSCAN_DATA_FLOAT,
    sizeof (gfloat),           HYSCAN_DISCRETIZATION_REAL },
  { 0, "complex-float",        HYSCAN_DATA_COMPLEX_FLOAT,
    sizeof (HyScanComplexFloat), HYSCAN_DISCRETIZATION_COMPLEX },

  { 0, "adc14le",              HYSCAN_DATA_ADC14LE,
    sizeof (guint16),          HYSCAN_DISCRETIZATION_REAL },
  { 0, "adc16le",              HYSCAN_DATA_ADC16LE,
    sizeof (guint16),          HYSCAN_DISCRETIZATION_REAL },
  { 0, "adc24le",              HYSCAN_DATA_ADC24LE,
    sizeof (guint32),          HYSCAN_DISCRETIZATION_REAL },

  { 0, "float16le",            HYSCAN_DATA_FLOAT16LE,
    sizeof (guint16),          HYSCAN_DISCRETIZATION_REAL },
  { 0, "float32le",            HYSCAN_DATA_FLOAT32LE,
    sizeof (guint32),          HYSCAN_DISCRETIZATION_REAL },

  { 0, "complex-adc14le",      HYSCAN_DATA_COMPLEX_ADC14LE,
    2 * sizeof (guint16),      HYSCAN_DISCRETIZATION_COMPLEX },
  { 0, "complex-adc16le",      HYSCAN_DATA_COMPLEX_ADC16LE,
    2 * sizeof (guint16),      HYSCAN_DISCRETIZATION_COMPLEX },
  { 0, "complex-adc24le",      HYSCAN_DATA_COMPLEX_ADC24LE,
    2 * sizeof (guint32),      HYSCAN_DISCRETIZATION_COMPLEX },

  { 0, "complex-float16le",    HYSCAN_DATA_COMPLEX_FLOAT16LE,
    2 * sizeof (guint16),      HYSCAN_DISCRETIZATION_COMPLEX },
  { 0, "complex-float32le",    HYSCAN_DATA_COMPLEX_FLOAT32LE,
    2 * sizeof (guint32),      HYSCAN_DISCRETIZATION_COMPLEX },

  { 0, "amplitude-int8",       HYSCAN_DATA_AMPLITUDE_INT8,
    sizeof (guint8),           HYSCAN_DISCRETIZATION_AMPLITUDE },
  { 0, "amplitude-int16le",    HYSCAN_DATA_AMPLITUDE_INT16LE,
    sizeof (guint16),          HYSCAN_DISCRETIZATION_AMPLITUDE },
  { 0, "amplitude-int24le",    HYSCAN_DATA_AMPLITUDE_INT24LE,
    sizeof (guint32),          HYSCAN_DISCRETIZATION_AMPLITUDE },
  { 0, "amplitude-int32le",    HYSCAN_DATA_AMPLITUDE_INT32LE,
    sizeof (guint32),          HYSCAN_DISCRETIZATION_AMPLITUDE },

  { 0, "amplitude-float16le",  HYSCAN_DATA_AMPLITUDE_FLOAT16LE,
    sizeof (guint16),          HYSCAN_DISCRETIZATION_AMPLITUDE },
  { 0, "amplitude-float32le",  HYSCAN_DATA_AMPLITUDE_FLOAT32LE,
    sizeof (guint32),          HYSCAN_DISCRETIZATION_AMPLITUDE },

  { 0, NULL,                   HYSCAN_DATA_INVALID,
    0,                         HYSCAN_DISCRETIZATION_INVALID }
};

/* Типы дискретизации и их названия. */
static HyScanDiscretizationTypeInfo hyscan_discretization_types_info[] =
{
  { 0, "real",                 HYSCAN_DISCRETIZATION_REAL },
  { 0, "complex",              HYSCAN_DISCRETIZATION_COMPLEX },
  { 0, "amplitude",            HYSCAN_DISCRETIZATION_AMPLITUDE },

  { 0, NULL,                   HYSCAN_DISCRETIZATION_INVALID }
};

/* Типы источников данных и их названия. */
static HyScanSourceTypeInfo hyscan_source_types_info[] =
{
  { 0, "log",                  HYSCAN_SOURCE_LOG },

  { 0, "ss-starboard",         HYSCAN_SOURCE_SIDE_SCAN_STARBOARD },
  { 0, "ss-starboard-hi",      HYSCAN_SOURCE_SIDE_SCAN_STARBOARD_HI },

  { 0, "ss-port",              HYSCAN_SOURCE_SIDE_SCAN_PORT },
  { 0, "ss-port-hi",           HYSCAN_SOURCE_SIDE_SCAN_PORT_HI },

  { 0, "echosounder",          HYSCAN_SOURCE_ECHOSOUNDER },
  { 0, "echosounder-hi",       HYSCAN_SOURCE_ECHOSOUNDER_HI },

  { 0, "bathy-starboard",      HYSCAN_SOURCE_BATHYMETRY_STARBOARD },
  { 0, "bathy-port",           HYSCAN_SOURCE_BATHYMETRY_PORT },

  { 0, "profiler",             HYSCAN_SOURCE_PROFILER },
  { 0, "profiler-echo",        HYSCAN_SOURCE_PROFILER_ECHO },

  { 0, "around-starboard",     HYSCAN_SOURCE_LOOK_AROUND_STARBOARD },
  { 0, "around-port",          HYSCAN_SOURCE_LOOK_AROUND_PORT },

  { 0, "forward-look",         HYSCAN_SOURCE_FORWARD_LOOK },
  { 0, "forward-echo",         HYSCAN_SOURCE_FORWARD_ECHO },

  { 0, "sas",                  HYSCAN_SOURCE_SAS },
  { 0, "nmea",                 HYSCAN_SOURCE_NMEA },
  { 0, "1pps",                 HYSCAN_SOURCE_1PPS },

  { 0, NULL,                   HYSCAN_SOURCE_INVALID }
};

/* Уровни сообщений и их названия. */
static HyScanLogLevelInfo hyscan_log_levels_info[] =
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
static HyScanTrackTypeInfo hyscan_track_types_info[] =
{
  { 0, "calibration",          HYSCAN_TRACK_CALIBRATION },
  { 0, "survey",               HYSCAN_TRACK_SURVEY },
  { 0, "tack",                 HYSCAN_TRACK_TACK },

  { 0, NULL,                   HYSCAN_TRACK_UNSPECIFIED }
};

#if  __GNUC__ > 2

#define HYSCAM_CONSTRUCTOR(_func) static void __attribute__((constructor)) _func (void);

#elif _MSC_VER >= 1500

#ifdef _WIN64
#define G_MSVC_SYMBOL_PREFIX ""
#else
#define G_MSVC_SYMBOL_PREFIX "_"
#endif

#define HYSCAM_CONSTRUCTOR(_func) G_MSVC_CTOR (_func, G_MSVC_SYMBOL_PREFIX)

#define G_MSVC_CTOR(_func,_sym_prefix) \
  static void _func(void); \
  extern int (* _array ## _func)(void);              \
  int _func ## _wrapper(void) { _func(); g_slist_find (NULL,  _array ## _func); return 0; } \
  __pragma(comment(linker,"/include:" _sym_prefix # _func "_wrapper")) \
  __pragma(section(".CRT$XCU",read)) \
  __declspec(allocate(".CRT$XCU")) int (* _array ## _func)(void) = _func ## _wrapper;

#else

#error "constructors not supported for this compiler"

#endif

HYSCAM_CONSTRUCTOR(hyscan_types_initialize)

/* Функция инициализации статических данных. */
static void
hyscan_types_initialize (void)
{
  guint i;

  for (i = 0; hyscan_data_types_info[i].name != NULL; i++)
    hyscan_data_types_info[i].quark = g_quark_from_static_string (hyscan_data_types_info[i].name);

  for (i = 0; hyscan_discretization_types_info[i].name != NULL; i++)
    hyscan_discretization_types_info[i].quark = g_quark_from_static_string (hyscan_discretization_types_info[i].name);

  for (i = 0; hyscan_log_levels_info[i].name != NULL; i++)
    hyscan_log_levels_info[i].quark = g_quark_from_static_string (hyscan_log_levels_info[i].name);

  for (i = 0; hyscan_track_types_info[i].name != NULL; i++)
    hyscan_track_types_info[i].quark = g_quark_from_static_string (hyscan_track_types_info[i].name);

  for (i = 0; hyscan_source_types_info[i].name != NULL; i++)
    hyscan_source_types_info[i].quark = g_quark_from_static_string (hyscan_source_types_info[i].name);

  atexit (xmlCleanupParser);
}

/**
 * hyscan_sound_velocity_copy:
 * @svp: структура #HyScanSoundVelocity для копирования
 *
 * Функция создаёт копию структуры #HyScanSoundVelocity.
 *
 * Returns: (transfer full): Новая структура #HyScanSoundVelocity.
 * Для удаления #hyscan_sound_velocity_free.
 */
HyScanSoundVelocity *
hyscan_sound_velocity_copy (const HyScanSoundVelocity *svp)
{
  if (svp != NULL)
    return g_slice_dup (HyScanSoundVelocity, svp);

  return NULL;
}

/**
 * hyscan_sound_velocity_free:
 * @svp: структура #HyScanSoundVelocity для удаления
 *
 * Функция удаляет структуру #HyScanSoundVelocity.
 */
void
hyscan_sound_velocity_free (HyScanSoundVelocity *svp)
{
  if (svp != NULL)
    g_slice_free (HyScanSoundVelocity, svp);
}

/**
 * hyscan_antenna_offset_copy:
 * @offset: структура #HyScanAntennaOffset для копирования
 *
 * Функция создаёт копию структуры #HyScanAntennaOffset.
 *
 * Returns: (transfer full): Новая структура #HyScanAntennaOffset.
 * Для удаления #hyscan_antenna_offset_free.
 */
HyScanAntennaOffset *
hyscan_antenna_offset_copy (const HyScanAntennaOffset *offset)
{
  if (offset != NULL)
    return g_slice_dup (HyScanAntennaOffset, offset);

  return NULL;
}

/**
 * hyscan_antenna_offset_free:
 * @offset: структура #HyScanAntennaOffset для удаления
 *
 * Функция удаляет структуру #HyScanAntennaOffset.
 */
void
hyscan_antenna_offset_free (HyScanAntennaOffset *offset)
{
  if (offset != NULL)
    g_slice_free (HyScanAntennaOffset, offset);
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

G_DEFINE_BOXED_TYPE (HyScanSoundVelocity, hyscan_sound_velocity, hyscan_sound_velocity_copy, hyscan_sound_velocity_free)

G_DEFINE_BOXED_TYPE (HyScanAntennaOffset, hyscan_antenna_offset, hyscan_antenna_offset_copy, hyscan_antenna_offset_free)

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
hyscan_data_get_name_by_type (HyScanDataType type)
{
  guint i;

  /* Ищем строку с указанным типом данных. */
  for (i = 0; hyscan_data_types_info[i].quark != 0; i++)
    {
      if (hyscan_data_types_info[i].type == type)
        return hyscan_data_types_info[i].name;
    }

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
hyscan_data_get_type_by_name (const gchar *name)
{
  GQuark quark;
  guint i;

  /* Ищем тип данных с указанным именем. */
  quark = g_quark_try_string (name);
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
hyscan_data_get_point_size (HyScanDataType type)
{
  guint i;

  /* Ищем строку с указанным типом данных. */
  for (i = 0; hyscan_data_types_info[i].quark != 0; i++)
    {
      if (hyscan_data_types_info[i].type == type)
        return hyscan_data_types_info[i].size;
    }

  return 0;
}

/**
 * hyscan_discretization_get_name_by_type:
 * @type: тип дискретизации данныx
 *
 * Функция преобразовывает нумерованное значение типа дискретизации в
 * название типа.
 *
 * Returns: Строка с названием типа дискретизации.
 */
const gchar *
hyscan_discretization_get_name_by_type (HyScanDiscretizationType type)
{
  guint i;

  /* Ищем строку с указанным типом дискретизации. */
  for (i = 0; hyscan_discretization_types_info[i].quark != 0; i++)
    {
      if (hyscan_discretization_types_info[i].type == type)
        return hyscan_discretization_types_info[i].name;
    }

  return NULL;
}

/**
 * hyscan_data_get_type_by_name:
 * @name: название типа дискретизации
 *
 * Функция преобразовывает строку с названием типа дискретизации в
 * нумерованное значение.
 *
 * Returns: Тип дискретизации.
 */
HyScanDiscretizationType
hyscan_discretization_get_type_by_name (const gchar *name)
{
  GQuark quark;
  guint i;

  /* Ищем тип дискретизации с указанным именем. */
  quark = g_quark_try_string (name);
  for (i = 0; hyscan_discretization_types_info[i].quark != 0; i++)
    {
      if (hyscan_discretization_types_info[i].quark == quark)
        return hyscan_discretization_types_info[i].type;
    }

  return HYSCAN_DISCRETIZATION_INVALID;
}

/**
 * hyscan_discretization_get_type_by_data:
 * @type: тип данныx
 *
 * Функция возвращает тип дискретизации данных в зависимости от их типа.
 *
 * Returns: Тип дискретизации.
 */
HyScanDiscretizationType
hyscan_discretization_get_type_by_data (HyScanDataType type)
{
  guint i;

  /* Ищем строку с указанным типом данных. */
  for (i = 0; hyscan_data_types_info[i].quark != 0; i++)
    {
      if (hyscan_data_types_info[i].type == type)
        return hyscan_data_types_info[i].discretization;
    }

  return HYSCAN_DISCRETIZATION_INVALID;
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
  guint i;

  /* Ищем название канала для указанных характеристик. */
  for (i = 0; hyscan_source_types_info[i].quark != 0; i++)
    {
      if (hyscan_source_types_info[i].type == source)
        return hyscan_source_types_info[i].name;
    }

  return NULL;
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
  GQuark quark;
  guint i;

  /* Ищем канал с указанным именем. */
  quark = g_quark_try_string (name);
  for (i = 0; hyscan_source_types_info[i].quark != 0; i++)
    {
      if (hyscan_source_types_info[i].quark == quark)
        return hyscan_source_types_info[i].type;
    }

  return HYSCAN_SOURCE_INVALID;
}

/**
 * hyscan_source_is_sensor:
 * @source: тип источника данныx
 *
 * Функция проверяет тип источника данных на соответствие одному из
 * типов датчиков.
 *
 * Returns: %TRUE если источник данных является датчиком, иначе %FALSE.
 */
gboolean
hyscan_source_is_sensor (HyScanSourceType source)
{
  switch (source)
    {
    case HYSCAN_SOURCE_SAS:
    case HYSCAN_SOURCE_NMEA:
    case HYSCAN_SOURCE_1PPS:
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
    case HYSCAN_SOURCE_PROFILER_ECHO:
    case HYSCAN_SOURCE_LOOK_AROUND_STARBOARD:
    case HYSCAN_SOURCE_LOOK_AROUND_PORT:
    case HYSCAN_SOURCE_FORWARD_LOOK:
    case HYSCAN_SOURCE_FORWARD_ECHO:
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

  /* Ищем название типа. */
  for (i = 0; hyscan_log_levels_info[i].quark != 0; i++)
    {
      if (hyscan_log_levels_info[i].level == level)
        return hyscan_log_levels_info[i].name;
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

  /* Ищем тип по названию. */
  quark = g_quark_try_string (name);
  for (i = 0; hyscan_log_levels_info[i].quark != 0; i++)
    {
      if (hyscan_log_levels_info[i].quark == quark)
        return hyscan_log_levels_info[i].level;
    }

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

  /* Ищем название типа. */
  for (i = 0; hyscan_track_types_info[i].quark != 0; i++)
    {
      if (hyscan_track_types_info[i].type == type)
        return hyscan_track_types_info[i].name;
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

  /* Ищем тип по названию. */
  quark = g_quark_try_string (name);
  for (i = 0; hyscan_track_types_info[i].quark != 0; i++)
    {
      if (hyscan_track_types_info[i].quark == quark)
        return hyscan_track_types_info[i].type;
    }

  return HYSCAN_TRACK_UNSPECIFIED;
}

/**
 * hyscan_channel_get_name_by_types:
 * @source: тип источника данныx
 * @type: тип канала данныx
 * @channel: индекс канала данныx
 *
 * Функция возвращает название канала данных для указанных характеристик.
 *
 * Returns: Название канала данных.
 */
const gchar *
hyscan_channel_get_name_by_types (HyScanSourceType  source,
                                  HyScanChannelType type,
                                  guint             channel)
{
  const gchar *source_name;
  const gchar *channel_type;
  gchar channel_name[256];
  GQuark id;

  if (channel == 0)
    return NULL;

  source_name = hyscan_source_get_name_by_type (source);
  if (source_name == NULL)
    return NULL;

  if (source == HYSCAN_SOURCE_LOG)
    return source_name;

  if (hyscan_source_is_sensor (source))
    type = HYSCAN_CHANNEL_DATA;

  switch (type)
    {
    case HYSCAN_CHANNEL_DATA:
      channel_type = DATA_CHANNEL_PREFIX;
      break;

    case HYSCAN_CHANNEL_NOISE:
      channel_type = NOISE_CHANNEL_PREFIX;
      break;

    case HYSCAN_CHANNEL_SIGNAL:
      channel_type = SIGNAL_CHANNEL_PREFIX;
      break;

    case HYSCAN_CHANNEL_TVG:
      channel_type = TVG_CHANNEL_PREFIX;
      break;

    default:
      return NULL;
    }

  if (channel == 1)
    {
      g_snprintf (channel_name, sizeof (channel_name),
                  "%s%s", source_name, channel_type);
    }
  else
    {
      g_snprintf (channel_name, sizeof (channel_name),
                  "%s%s-%u", source_name, channel_type, channel);
    }

  id = g_quark_from_string (channel_name);

  return g_quark_to_string (id);
}

/**
 * hyscan_channel_get_types_by_name:
 * @name: название канала данныx
 * @source: (out): тип источника данныx
 * @type: (out): тип канала данныx
 * @channel: (out): индекс канала данныx
 *
 * Функция возвращает характеристики канала данных по его имени.
 *
 * Returns: %TRUE если характеристики канала определены, иначе %FALSE.
 */
gboolean
hyscan_channel_get_types_by_name (const gchar       *name,
                                  HyScanSourceType  *source,
                                  HyScanChannelType *type,
                                  guint             *channel)
{
  const gchar *cur;
  gchar *end;
  guint i;

  for (i = 0; hyscan_source_types_info[i].name != NULL; i++)
    {
      cur = name;
      if (g_str_has_prefix (cur, hyscan_source_types_info[i].name))
        {
          *source = hyscan_source_types_info[i].type;
          *type = HYSCAN_CHANNEL_DATA;

          /* Проверяем что название источника данных не имеет продолжения. */
          cur += strlen (hyscan_source_types_info[i].name);
          if (*cur == 0)
            {
              *channel = 1;
              return TRUE;
            }
          else if (*cur != '-')
            {
              continue;
            }

          /* Определяем тип канала данных. */
          if (g_str_has_prefix (cur, NOISE_CHANNEL_PREFIX))
            {
              *type = HYSCAN_CHANNEL_NOISE;
              cur += sizeof (NOISE_CHANNEL_PREFIX) - 1;
            }
          else if (g_str_has_prefix (cur, SIGNAL_CHANNEL_PREFIX))
            {
              *type = HYSCAN_CHANNEL_SIGNAL;
              cur += sizeof (SIGNAL_CHANNEL_PREFIX) - 1;
            }
          else if (g_str_has_prefix (cur, TVG_CHANNEL_PREFIX))
            {
              *type = HYSCAN_CHANNEL_TVG;
              cur += sizeof (TVG_CHANNEL_PREFIX) - 1;
            }

          /* Определяем индекс канала данных. */
          if (*cur == 0)
            {
              *channel = 1;
              return TRUE;
            }
          else
            {
              cur += 1;
            }


          *channel = g_ascii_strtoull (cur, &end, 10);
          if ((*channel > 0) && (*end == 0))
            return TRUE;
        }
    }

  return FALSE;
}

/**
 * hyscan_param_name_constructor:
 * @buffer: буфер для имени
 * @size: размер буфера
 * @...: NULL терминированный список компонентов имени
 *
 * Функция объединяет несколько компонентов имени в единый путь,
 * используя в качестве разделителя символ '/'.
 */
void
hyscan_param_name_constructor (gchar *buffer,
                               guint  size,
                               ...)
{
  gchar *buf_ptr = buffer;
  const gchar *name;
  va_list names;
  gint n;

  va_start (names, size);

  while ((name = va_arg (names, const gchar*)) != NULL)
    {
      n = g_snprintf (buf_ptr, size, "/%s", name);
      buf_ptr += n;
      size -= n;
    }

  va_end (names);
}
