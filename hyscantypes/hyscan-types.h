/* hyscan-types.h
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

#ifndef __HYSCAN_TYPES_H__
#define __HYSCAN_TYPES_H__

#include <glib-object.h>
#include <hyscan-api.h>

G_BEGIN_DECLS

typedef struct _HyScanComplexFloat HyScanComplexFloat;
typedef struct _HyScanSoundVelocity HyScanSoundVelocity;
typedef struct _HyScanAntennaPosition HyScanAntennaPosition;
typedef struct _HyScanAcousticDataInfo HyScanAcousticDataInfo;

/**
 * HyScanDataType:
 * @HYSCAN_DATA_INVALID: недопустимый тип, ошибка
 * @HYSCAN_DATA_BLOB: неструктурированные двоичные данные
 * @HYSCAN_DATA_STRING: строка с нулём на конце
 * @HYSCAN_DATA_FLOAT: действительные float значения
 * @HYSCAN_DATA_COMPLEX_FLOAT: комплексные float значения
 * @HYSCAN_DATA_ADC_14LE: действительные отсчёты АЦП младшие 14 бит из 16, формат little endian
 * @HYSCAN_DATA_ADC_16LE: действительные отсчёты АЦП 16 бит, формат little endian
 * @HYSCAN_DATA_ADC_24LE: действительные отсчёты АЦП младшие 24 бит из 32, формат little endian
 * @HYSCAN_DATA_COMPLEX_ADC_14LE: комплексные отсчёты АЦП младшие 14 бит из 16, формат little endian
 * @HYSCAN_DATA_COMPLEX_ADC_16LE: комплексные отсчёты АЦП 16 бит, формат little endian
 * @HYSCAN_DATA_COMPLEX_ADC_24LE: комплексные отсчёты АЦП младшие 24 бит из 32, формат little endian
 * @HYSCAN_DATA_AMPLITUDE_INT8: амплитудные значения, 8 бит
 * @HYSCAN_DATA_AMPLITUDE_INT16: амплитудные значения, 16 бит
 * @HYSCAN_DATA_AMPLITUDE_INT24: амплитудные значения, младшие 24 бит из 32
 * @HYSCAN_DATA_AMPLITUDE_INT32: амплитудные значения, 32 бит
 * @HYSCAN_DATA_AMPLITUDE_FLOAT8: амплитудные значения с плавающей точкой, 8 бит
 * @HYSCAN_DATA_AMPLITUDE_FLOAT16: амплитудные значения с плавающей точкой, 16 бит
 * @HYSCAN_DATA_AMPLITUDE_FLOAT32: амплитудные значения с плавающей точкой, 32 бит
 *
 * Типы данных.
 */
typedef enum
{
  HYSCAN_DATA_INVALID,

  HYSCAN_DATA_BLOB,
  HYSCAN_DATA_STRING,
  HYSCAN_DATA_FLOAT,
  HYSCAN_DATA_COMPLEX_FLOAT,

  HYSCAN_DATA_ADC_14LE,
  HYSCAN_DATA_ADC_16LE,
  HYSCAN_DATA_ADC_24LE,

  HYSCAN_DATA_COMPLEX_ADC_14LE,
  HYSCAN_DATA_COMPLEX_ADC_16LE,
  HYSCAN_DATA_COMPLEX_ADC_24LE,

  HYSCAN_DATA_AMPLITUDE_INT8,
  HYSCAN_DATA_AMPLITUDE_INT16,
  HYSCAN_DATA_AMPLITUDE_INT24,
  HYSCAN_DATA_AMPLITUDE_INT32,

  HYSCAN_DATA_AMPLITUDE_FLOAT8,
  HYSCAN_DATA_AMPLITUDE_FLOAT16,
  HYSCAN_DATA_AMPLITUDE_FLOAT32
} HyScanDataType;

/**
 * HyScanDiscretizationType:
 * @HYSCAN_DISCRETIZATION_INVALID: недопустимый тип, ошибка
 * @HYSCAN_DISCRETIZATION_REAL: действительная оцифровка
 * @HYSCAN_DISCRETIZATION_COMPLEX: комплексная оцифровка
 * @HYSCAN_DISCRETIZATION_AMPLITUDE: амплитудная оцифровка
 *
 * Тип оцифровки данных.
 */
typedef enum
{
  HYSCAN_DISCRETIZATION_INVALID,

  HYSCAN_DISCRETIZATION_REAL,
  HYSCAN_DISCRETIZATION_COMPLEX,
  HYSCAN_DISCRETIZATION_AMPLITUDE
} HyScanDiscretizationType;

/**
 * HyScanSourceType:
 * @HYSCAN_SOURCE_INVALID: недопустимый тип, ошибка
 * @HYSCAN_SOURCE_LOG: информационные сообщения
 * @HYSCAN_SOURCE_SIDE_SCAN_STARBOARD: боковой обзор, правый борт
 * @HYSCAN_SOURCE_SIDE_SCAN_PORT: боковой обзор, левый борт
 * @HYSCAN_SOURCE_SIDE_SCAN_STARBOARD_HI: боковой обзор, правый борт, высокое разрешение
 * @HYSCAN_SOURCE_SIDE_SCAN_PORT_HI: боковой обзор, левый борт, высокое разрешение
 * @HYSCAN_SOURCE_ECHOSOUNDER: эхолот
 * @HYSCAN_SOURCE_ECHOSOUNDER_HI: эхолот, высокое разрешение
 * @HYSCAN_SOURCE_BATHYMETRY_STARBOARD: батиметрия, правый борт
 * @HYSCAN_SOURCE_BATHYMETRY_PORT: батиметрия, левый борт
 * @HYSCAN_SOURCE_PROFILER: профилограф
 * @HYSCAN_SOURCE_ECHOPROFILER: эхолот профилографа
 * @HYSCAN_SOURCE_LOOK_AROUND_STARBOARD: круговой обзор, правый борт
 * @HYSCAN_SOURCE_LOOK_AROUND_PORT: круговой обзор, левый борт
 * @HYSCAN_SOURCE_FORWARD_LOOK: вперёдсмотрящий гидролокатор
 * @HYSCAN_SOURCE_SAS: сообщения САД
 * @HYSCAN_SOURCE_NMEA_ANY: любые сообщения NMEA
 * @HYSCAN_SOURCE_NMEA_GGA: сообщения NMEA GGA
 * @HYSCAN_SOURCE_NMEA_RMC: сообщения NMEA RMC
 * @HYSCAN_SOURCE_NMEA_DPT: сообщения NMEA DPT
 * @HYSCAN_SOURCE_1PPS: сообщения 1PPS
 * @HYSCAN_SOURCE_LAST: финальный идентификатор
 *
 * Типы источников данных.
 */
typedef enum
{
  HYSCAN_SOURCE_INVALID,

  HYSCAN_SOURCE_LOG,

  HYSCAN_SOURCE_SIDE_SCAN_STARBOARD,
  HYSCAN_SOURCE_SIDE_SCAN_PORT,
  HYSCAN_SOURCE_SIDE_SCAN_STARBOARD_HI,
  HYSCAN_SOURCE_SIDE_SCAN_PORT_HI,
  HYSCAN_SOURCE_ECHOSOUNDER,
  HYSCAN_SOURCE_ECHOSOUNDER_HI,
  HYSCAN_SOURCE_BATHYMETRY_STARBOARD,
  HYSCAN_SOURCE_BATHYMETRY_PORT,
  HYSCAN_SOURCE_PROFILER,
  HYSCAN_SOURCE_ECHOPROFILER,
  HYSCAN_SOURCE_LOOK_AROUND_STARBOARD,
  HYSCAN_SOURCE_LOOK_AROUND_PORT,
  HYSCAN_SOURCE_FORWARD_LOOK,

  HYSCAN_SOURCE_SAS,
  HYSCAN_SOURCE_NMEA_ANY,
  HYSCAN_SOURCE_NMEA_GGA,
  HYSCAN_SOURCE_NMEA_RMC,
  HYSCAN_SOURCE_NMEA_DPT,
  HYSCAN_SOURCE_1PPS,

  HYSCAN_SOURCE_LAST
} HyScanSourceType;

/**
 * HyScanChannelType:
 * @HYSCAN_CHANNEL_INVALID: недопустимый тип, ошибка
 * @HYSCAN_CHANNEL_DATA: канал данныx
 * @HYSCAN_CHANNEL_NOISE: канал шума
 * @HYSCAN_CHANNEL_SIGNAL: канал сигналов
 * @HYSCAN_CHANNEL_TVG: канал параметров ВАРУ
 * @HYSCAN_CHANNEL_LAST: финальный идентификатор
 *
 * Типы каналов данных.
 */
typedef enum
{
  HYSCAN_CHANNEL_INVALID,

  HYSCAN_CHANNEL_DATA,
  HYSCAN_CHANNEL_NOISE,
  HYSCAN_CHANNEL_SIGNAL,
  HYSCAN_CHANNEL_TVG,

  HYSCAN_CHANNEL_LAST
} HyScanChannelType;

/**
 * HyScanLogLevel:
 * @HYSCAN_LOG_LEVEL_DEBUG: отладочное сообщение
 * @HYSCAN_LOG_LEVEL_INFO: информационное сообщение
 * @HYSCAN_LOG_LEVEL_MESSAGE: обычное сообщение
 * @HYSCAN_LOG_LEVEL_WARNING: предупреждающее сообщение
 * @HYSCAN_LOG_LEVEL_CRITICAL: критически важное сообщение
 * @HYSCAN_LOG_LEVEL_ERROR: сообщение об ошибке
 *
 * Типы информационных сообщений.
 */
typedef enum
{
  HYSCAN_LOG_LEVEL_DEBUG     = (1 << 0),
  HYSCAN_LOG_LEVEL_INFO      = (1 << 1),
  HYSCAN_LOG_LEVEL_MESSAGE   = (1 << 2),
  HYSCAN_LOG_LEVEL_WARNING   = (1 << 3),
  HYSCAN_LOG_LEVEL_CRITICAL  = (1 << 4),
  HYSCAN_LOG_LEVEL_ERROR     = (1 << 5)
} HyScanLogLevel;

/**
 * HyScanTrackType:
 * @HYSCAN_TRACK_UNSPECIFIED: неопределённый тип
 * @HYSCAN_TRACK_CALIBRATION: галс с данными калибровки
 * @HYSCAN_TRACK_SURVEY: галс с данными съёмки
 * @HYSCAN_TRACK_TACK: лавировочный галс
 *
 * Типы галсов.
 */
typedef enum
{
  HYSCAN_TRACK_UNSPECIFIED,
  HYSCAN_TRACK_CALIBRATION,
  HYSCAN_TRACK_SURVEY,
  HYSCAN_TRACK_TACK
} HyScanTrackType;

/**
 * HyScanComplexFloat:
 * @re: действительная часть
 * @im: мнимая часть
 *
 * Структура содержит описание комплексного float числа.
 */
struct _HyScanComplexFloat
{
  gfloat                  re;
  gfloat                  im;
};

/**
 * HyScanSoundVelocity:
 * @depth: глубина, м
 * @velocity: скорость звука, м/c
 *
 * Элемент таблицы профиля скорости звука.
 */
struct _HyScanSoundVelocity
{
  gdouble                 depth;
  gdouble                 velocity;
};

/**
 * HyScanAntennaPosition:
 * @x: смещение антенны по оси X, м
 * @y: смещение антенны по оси Y, м
 * @z: смещение антенны по оси Z, м
 * @psi: поворот антенны по курсу, рад
 * @gamma: поворот антенны по крену, рад
 * @theta: поворот антенны по дифференту, рад
 *
 * Параметры местоположения приёмной антенны. Смещения приёмной антенны
 * указываются относительно центра масс судна. При этом ось X направлена
 * в нос, ось Y на правый борт, ось Z вверх.
 *
 * Углы установки антенны указываются для вектора, перпендикулярного её
 * рабочей плоскости. Угол psi учитывает разворот антенны по курсу от её
 * нормального положения. Угол gamma учитывает разворот антенны по крену.
 * Угол theta учитывает разворот антенны по дифференту от её нормального
 * положения.
 *
 * Положительные значения указываются для углов отсчитываемых против часовой
 * стрелки.
 */
struct _HyScanAntennaPosition
{
  gdouble                 x;
  gdouble                 y;
  gdouble                 z;

  gdouble                 psi;
  gdouble                 gamma;
  gdouble                 theta;
};

/**
 * HyScanAcousticDataInfo:
 * @data_type: тип данныx
 * @data_rate: частота дискретизации, Гц
 * @signal_frequency: несущая частота излучаемого сигнала, Гц
 * @signal_bandwidth: полоса излучаемого сигнала, Гц
 * @antenna_voffset: смещение антенны в "решётке" в вертикальной плоскости, м
 * @antenna_hoffset: смещение антенны в "решётке" в горизонтальной плоскости, м
 * @antenna_vpattern: диаграмма направленности в вертикальной плоскости, рад
 * @antenna_hpattern: диаграмма направленности в горизонтальной плоскости, рад
 * @antenna_frequency: центральная частота антенны, Гц
 * @antenna_bandwidth: полоса пропускания антенны, Гц
 * @adc_vref: опорное напряжение АЦП, В
 * @adc_offset: смещение нуля АЦП, отсчёты
 *
 * Параметры гидроакустических данных.
 */
struct _HyScanAcousticDataInfo
{
  HyScanDataType          data_type;
  gdouble                 data_rate;

  gdouble                 signal_frequency;
  gdouble                 signal_bandwidth;

  gdouble                 antenna_voffset;
  gdouble                 antenna_hoffset;
  gdouble                 antenna_vpattern;
  gdouble                 antenna_hpattern;
  gdouble                 antenna_frequency;
  gdouble                 antenna_bandwidth;

  gdouble                 adc_vref;
  gint                    adc_offset;
};

HYSCAN_API
GType                     hyscan_sound_velocity_get_type          (void);

HYSCAN_API
GType                     hyscan_antenna_position_get_type        (void);

HYSCAN_API
GType                     hyscan_acoustic_data_info_get_type      (void);

HYSCAN_API
HyScanSoundVelocity *     hyscan_sound_velocity_copy              (const HyScanSoundVelocity    *svp);

HYSCAN_API
void                      hyscan_sound_velocity_free              (HyScanSoundVelocity          *svp);

HYSCAN_API
HyScanAntennaPosition *   hyscan_antenna_position_copy            (const HyScanAntennaPosition  *position);

HYSCAN_API
void                      hyscan_antenna_position_free            (HyScanAntennaPosition        *position);

HYSCAN_API
HyScanAcousticDataInfo *  hyscan_acoustic_data_info_copy          (const HyScanAcousticDataInfo *info);

HYSCAN_API
void                      hyscan_acoustic_data_info_free          (HyScanAcousticDataInfo       *info);

HYSCAN_API
const gchar *             hyscan_data_get_name_by_type            (HyScanDataType                type);

HYSCAN_API
HyScanDataType            hyscan_data_get_type_by_name            (const gchar                  *name);

HYSCAN_API
guint32                   hyscan_data_get_point_size              (HyScanDataType                type);

HYSCAN_API
const gchar *             hyscan_discretization_get_name_by_type  (HyScanDiscretizationType      type);

HYSCAN_API
HyScanDiscretizationType  hyscan_discretization_get_type_by_name  (const gchar                  *name);

HYSCAN_API
HyScanDiscretizationType  hyscan_discretization_get_type_by_data  (HyScanDataType                type);

HYSCAN_API
const gchar              *hyscan_source_get_name_by_type          (HyScanSourceType              source);

HYSCAN_API
HyScanSourceType          hyscan_source_get_type_by_name          (const gchar                  *name);

HYSCAN_API
gboolean                  hyscan_source_is_sensor                 (HyScanSourceType              source);

HYSCAN_API
gboolean                  hyscan_source_is_sonar                  (HyScanSourceType              source);

HYSCAN_API
const gchar *             hyscan_log_level_get_name_by_type       (HyScanLogLevel                level);

HYSCAN_API
HyScanLogLevel            hyscan_log_level_get_type_by_name       (const gchar                  *name);

HYSCAN_API
const gchar *             hyscan_track_get_name_by_type           (HyScanTrackType               type);

HYSCAN_API
HyScanTrackType           hyscan_track_get_type_by_name           (const gchar                  *name);

HYSCAN_API
const gchar *             hyscan_channel_get_name_by_types        (HyScanSourceType              source,
                                                                   HyScanChannelType             type,
                                                                   guint                         channel);

HYSCAN_API
gboolean                  hyscan_channel_get_types_by_name        (const gchar                  *name,
                                                                   HyScanSourceType             *source,
                                                                   HyScanChannelType            *type,
                                                                   guint                        *channel);

G_END_DECLS

#endif /* __HYSCAN_TYPES_H__ */
