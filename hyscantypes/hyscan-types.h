/* hyscan-types.h
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
 * Во первых, вы можете распространять HyScanTypes на условиях Стандартной
 * Общественной Лицензии GNU версии 3, либо по любой более поздней версии
 * лицензии (по вашему выбору). Полные положения лицензии GNU приведены в
 * <http://www.gnu.org/licenses/>.
 *
 * Во вторых, этот программный код можно использовать по коммерческой
 * лицензии. Для этого свяжитесь с ООО Экран - info@screen-co.ru.
 */

#ifndef __HYSCAN_TYPES_H__
#define __HYSCAN_TYPES_H__

#include <glib.h>
#include <hyscan-api.h>

G_BEGIN_DECLS

typedef struct _HyScanComplexFloat HyScanComplexFloat;
typedef struct _HyScanSoundVelocity HyScanSoundVelocity;
typedef struct _HyScanAntennaPosition HyScanAntennaPosition;
typedef struct _HyScanRawDataInfo HyScanRawDataInfo;
typedef struct _HyScanAcousticDataInfo HyScanAcousticDataInfo;

/**
 * HyScanDataType:
 * @HYSCAN_DATA_INVALID: Недопустимый тип, ошибка.
 * @HYSCAN_DATA_BLOB: Неструктурированные двоичные данные.
 * @HYSCAN_DATA_STRING: Строка с нулём на конце.
 * @HYSCAN_DATA_ADC_14LE: Действительные отсчёты АЦП младшие 14 бит из 16, формат little endian.
 * @HYSCAN_DATA_ADC_16LE: Действительные отсчёты АЦП 16 бит, формат little endian.
 * @HYSCAN_DATA_ADC_24LE : Действительные отсчёты АЦП младшие 24 бит из 32, формат little endian.
 * @HYSCAN_DATA_COMPLEX_ADC_14LE: Комплексные отсчёты АЦП младшие 14 бит из 16, формат little endian.
 * @HYSCAN_DATA_COMPLEX_ADC_16LE: Комплексные отсчёты АЦП 16 бит, формат little endian.
 * @HYSCAN_DATA_COMPLEX_ADC_24LE: Комплексные отсчёты АЦП младшие 24 бит из 32, формат little endian.
 * @HYSCAN_DATA_UINT8: Действительные unsigned int8 значения.
 * @HYSCAN_DATA_UINT16: Действительные unsigned int16 значения.
 * @HYSCAN_DATA_UINT32: Действительные unsigned int32 значения.
 * @HYSCAN_DATA_FLOAT: Действительные float значения.
 * @HYSCAN_DATA_NORMAL8: Действительные значения с плавающей точкой, 8 бит, диапазон от 0 до 1.
 * @HYSCAN_DATA_NORMAL16: Действительные значения с плавающей точкой, 16 бит, диапазон от 0 до 1.
 * @HYSCAN_DATA_COMPLEX_UINT8: Комплексные unsigned int8 значения.
 * @HYSCAN_DATA_COMPLEX_UINT16: Комплексные unsigned int16 значения.
 * @HYSCAN_DATA_COMPLEX_UINT32: Комплексные unsigned int32 значения.
 * @HYSCAN_DATA_COMPLEX_FLOAT: Комплексные float значения.
 *
 * Типы данных HyScan.
 */
typedef enum
{
  HYSCAN_DATA_INVALID                      = 0,

  HYSCAN_DATA_BLOB                         = 101,
  HYSCAN_DATA_STRING                       = 102,

  HYSCAN_DATA_ADC_14LE                     = 201,
  HYSCAN_DATA_ADC_16LE                     = 202,
  HYSCAN_DATA_ADC_24LE                     = 203,

  HYSCAN_DATA_COMPLEX_ADC_14LE             = 301,
  HYSCAN_DATA_COMPLEX_ADC_16LE             = 302,
  HYSCAN_DATA_COMPLEX_ADC_24LE             = 303,

  HYSCAN_DATA_UINT8                        = 401,
  HYSCAN_DATA_UINT16                       = 402,
  HYSCAN_DATA_UINT32                       = 403,
  HYSCAN_DATA_FLOAT                        = 404,
  HYSCAN_DATA_NORMAL8                      = 405,
  HYSCAN_DATA_NORMAL16                     = 406,

  HYSCAN_DATA_COMPLEX_UINT8                = 501,
  HYSCAN_DATA_COMPLEX_UINT16               = 502,
  HYSCAN_DATA_COMPLEX_UINT32               = 503,
  HYSCAN_DATA_COMPLEX_FLOAT                = 504
} HyScanDataType;

/**
 * HyScanSourceType:
 * @HYSCAN_SOURCE_INVALID: Недопустимый тип, ошибка.
 * @HYSCAN_SOURCE_LOG: Информационные сообщения.
 * @HYSCAN_SOURCE_SIDE_SCAN_STARBOARD: Боковой обзор, правый борт.
 * @HYSCAN_SOURCE_SIDE_SCAN_PORT: Боковой обзор, левый борт.
 * @HYSCAN_SOURCE_SIDE_SCAN_STARBOARD_HI: Боковой обзор, правый борт, высокое разрешение.
 * @HYSCAN_SOURCE_SIDE_SCAN_PORT_HI: Боковой обзор, левый борт, высокое разрешение.
 * @HYSCAN_SOURCE_ECHOSOUNDER: Эхолот.
 * @HYSCAN_SOURCE_ECHOSOUNDER_HI: Эхолот, высокое разрешение.
 * @HYSCAN_SOURCE_BATHYMETRY_STARBOARD: Батиметрия, правый борт.
 * @HYSCAN_SOURCE_BATHYMETRY_PORT: Батиметрия, левый борт.
 * @HYSCAN_SOURCE_PROFILER: Профилограф.
 * @HYSCAN_SOURCE_LOOK_AROUND_STARBOARD: Круговой обзор, правый борт.
 * @HYSCAN_SOURCE_LOOK_AROUND_PORT: Круговой обзор, левый борт.
 * @HYSCAN_SOURCE_FORWARD_LOOK: Вперёдсмотрящий гидролокатор.
 * @HYSCAN_SOURCE_SAS: Сообщения САД.
 * @HYSCAN_SOURCE_SAS_V2: Сообщения САД, версия 2.
 * @HYSCAN_SOURCE_NMEA_ANY: Любые сообщения NMEA.
 * @HYSCAN_SOURCE_NMEA_GGA: Сообщения NMEA GGA.
 * @HYSCAN_SOURCE_NMEA_RMC: Сообщения NMEA RMC.
 * @HYSCAN_SOURCE_NMEA_DPT: Сообщения NMEA DPT.
 *
 * Типы источников данных.
 */
typedef enum
{
  HYSCAN_SOURCE_INVALID                    = 0,

  HYSCAN_SOURCE_LOG                        = 101,

  HYSCAN_SOURCE_SIDE_SCAN_STARBOARD        = 201,
  HYSCAN_SOURCE_SIDE_SCAN_PORT             = 202,
  HYSCAN_SOURCE_SIDE_SCAN_STARBOARD_HI     = 203,
  HYSCAN_SOURCE_SIDE_SCAN_PORT_HI          = 204,
  HYSCAN_SOURCE_ECHOSOUNDER                = 205,
  HYSCAN_SOURCE_ECHOSOUNDER_HI             = 206,
  HYSCAN_SOURCE_BATHYMETRY_STARBOARD       = 207,
  HYSCAN_SOURCE_BATHYMETRY_PORT            = 208,
  HYSCAN_SOURCE_PROFILER                   = 209,
  HYSCAN_SOURCE_LOOK_AROUND_STARBOARD      = 210,
  HYSCAN_SOURCE_LOOK_AROUND_PORT           = 211,
  HYSCAN_SOURCE_FORWARD_LOOK               = 212,

  HYSCAN_SOURCE_SAS                        = 301,
  HYSCAN_SOURCE_SAS_V2                     = 302,

  HYSCAN_SOURCE_NMEA_ANY                   = 401,
  HYSCAN_SOURCE_NMEA_GGA                   = 402,
  HYSCAN_SOURCE_NMEA_RMC                   = 403,
  HYSCAN_SOURCE_NMEA_DPT                   = 404
} HyScanSourceType;

/**
 * HyScanLogLevel:
 * @HYSCAN_LOG_LEVEL_DEBUG: Отладочное сообщение.
 * @HYSCAN_LOG_LEVEL_INFO: Информационное сообщение.
 * @HYSCAN_LOG_LEVEL_MESSAGE: Обычное сообщение.
 * @HYSCAN_LOG_LEVEL_WARNING: Предупреждающее сообщение.
 * @HYSCAN_LOG_LEVEL_CRITICAL: Критически важное сообщение.
 * @HYSCAN_LOG_LEVEL_ERROR: Сообщение об ошибке.
 *
 * Типы информационных сообщений.
 */
typedef enum
{
  HYSCAN_LOG_LEVEL_DEBUG                   = (1),
  HYSCAN_LOG_LEVEL_INFO                    = (1 << 1),
  HYSCAN_LOG_LEVEL_MESSAGE                 = (1 << 2),
  HYSCAN_LOG_LEVEL_WARNING                 = (1 << 3),
  HYSCAN_LOG_LEVEL_CRITICAL                = (1 << 4),
  HYSCAN_LOG_LEVEL_ERROR                   = (1 << 5)
} HyScanLogLevel;

/**
 * HyScanTrackType:
 * @HYSCAN_TRACK_UNSPECIFIED: Неопределённый тип.
 * @HYSCAN_TRACK_CALIBRATION: Галс с данными калибровки.
 * @HYSCAN_TRACK_SURVEY: Галс с данными съёмки.
 * @HYSCAN_TRACK_TACK: Лавировочный галс.
 *
 * Типы галсов.
 */
typedef enum
{
  HYSCAN_TRACK_UNSPECIFIED                 = 0,

  HYSCAN_TRACK_CALIBRATION                 = 101,
  HYSCAN_TRACK_SURVEY                      = 102,
  HYSCAN_TRACK_TACK                        = 103
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
  gfloat                                   re;
  gfloat                                   im;
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
  gdouble                                  depth;
  gdouble                                  velocity;
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
 * Углы установки антенны указываются для вектора, перпиндикулярного её
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
  gdouble                                  x;
  gdouble                                  y;
  gdouble                                  z;

  gdouble                                  psi;
  gdouble                                  gamma;
  gdouble                                  theta;
};

/**
 * HyScanRawDataInfo:
 * @data_type: тип данныx
 * @data_rate: частота дискретизации, Гц
 * @antenna_voffset: смещение антенны в "решётке" в вертикальной плоскости, м
 * @antenna_hoffset: смещение антенны в "решётке" в горизонтальной плоскости, м
 * @antenna_vpattern: диаграмма направленности в вертикальной плоскости, рад
 * @antenna_hpattern: диаграмма направленности в горизонтальной плоскости, рад
 * @antenna_frequency: центральная частота, Гц
 * @antenna_bandwidth: полоса пропускания, Гц
 * @adc_vref: опорное напряжение, В
 * @adc_offset: смещение нуля, отсчёты
 *
 * Параметры сырых гидролокационных данных.
 */
struct _HyScanRawDataInfo
{
  HyScanDataType                           data_type;
  gdouble                                  data_rate;

  gdouble                                  antenna_voffset;
  gdouble                                  antenna_hoffset;
  gdouble                                  antenna_vpattern;
  gdouble                                  antenna_hpattern;
  gdouble                                  antenna_frequency;
  gdouble                                  antenna_bandwidth;

  gdouble                                  adc_vref;
  gint                                     adc_offset;
};

/**
 * HyScanAcousticDataInfo:
 * @data_type: тип данныx
 * @data_rate: частота дискретизации, Гц
 * @antenna_vpattern: диаграмма направленности в вертикальной плоскости, рад
 * @antenna_hpattern: диаграмма направленности в горизонтальной плоскости, рад
 *
 * Параметры акустических данных.
 */
struct _HyScanAcousticDataInfo
{
  HyScanDataType                           data_type;
  gdouble                                  data_rate;

  gdouble                                  antenna_vpattern;
  gdouble                                  antenna_hpattern;
};

HYSCAN_API
const gchar           *hyscan_data_get_name_by_type            (HyScanDataType                 type);

HYSCAN_API
HyScanDataType         hyscan_data_get_type_by_name            (const gchar                   *name);

HYSCAN_API
guint32                hyscan_data_get_point_size              (HyScanDataType                 type);

HYSCAN_API
const gchar           *hyscan_source_get_name_by_type          (HyScanSourceType               source);

HYSCAN_API
HyScanSourceType       hyscan_source_get_type_by_name          (const gchar                   *name);

HYSCAN_API
gboolean               hyscan_source_is_sensor                 (HyScanSourceType               source);

HYSCAN_API
gboolean               hyscan_source_is_raw                    (HyScanSourceType               source);

HYSCAN_API
gboolean               hyscan_source_is_acoustic               (HyScanSourceType               source);

HYSCAN_API
const gchar           *hyscan_channel_get_name_by_types        (HyScanSourceType               source,
                                                                gboolean                       raw,
                                                                guint                          channel);

HYSCAN_API
gboolean               hyscan_channel_get_types_by_name        (const gchar                   *name,
                                                                HyScanSourceType              *source,
                                                                gboolean                      *raw,
                                                                guint                         *channel);

HYSCAN_API
const gchar           *hyscan_track_get_name_by_type           (HyScanTrackType                type);

HYSCAN_API
HyScanTrackType        hyscan_track_get_type_by_name           (const gchar                   *name);

G_END_DECLS

#endif /* __HYSCAN_TYPES_H__ */
