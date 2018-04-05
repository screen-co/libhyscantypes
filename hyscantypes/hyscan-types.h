/**
 * \file hyscan-types.h
 *
 * \brief Заголовочный файл базовых типов данных HyScan
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2015
 * \license Проприетарная лицензия ООО "Экран"
 *
 * \defgroup HyScanTypes HyScanTypes - базовые типы данных HyScan
 *
 * HyScanTypes содержит определения базовых типов данных HyScan и функция для работы с ними.
 * Доступны функции определения типа по имени данных и наоборот, определения размера одного отсчёта
 * в данных и преобразования данных:
 *
 * - #hyscan_data_get_type_by_name - функция определяет тип данных по имени;
 * - #hyscan_data_get_type_name - функция возвращает название данных для указанного типа;
 * - #hyscan_data_get_point_size - функция возвращает размер одного элемента данных в байтах;
 * - #hyscan_data_import_float - функция преобразовывает данные из низкоуровневого формата в float;
 * - #hyscan_data_import_complex_float - функция преобразовывает данные из низкоуровневого формата в \link HyScanComplexFloat \endlink.
 *
 * Кроме этого вводится понятие - источник данных (\link HyScanSourceType \endlink).
 * Все источники данных разделены на два основных вида:
 *
 * - датчики
 * - источники гидролокационных данных.
 *
 * Источники гидролокационных данных разделяются на следующие виды:
 *
 * - источники "сырых" гидролокационных данных;
 * - источники акустических данных;
 * - источники батиметрических данных.
 *
 * Источники "сырых" гидролокационных данных содержат информацию об амплитуде эхо сигнала
 * дискретизированной во времени. Это вид первичной информации, получаемой от приёмников
 * гидролокатора.
 *
 * Источники акустических данных содержат обработанную информацию от гидролокаторов
 * бокового обзора, эхолота, профилографа и т.п.
 *
 * Источники батиметрических данных содержат обработанную информацию о глубине.
 *
 * Функции #hyscan_source_is_sensor, #hyscan_source_is_raw и #hyscan_source_is_acoustic
 * могут использоваться для проверки принадлежности источника данных к определённому типу.
 *
 * Функции #hyscan_channel_get_name_by_types и #hyscan_channel_get_types_by_name используются для
 * преобразования типа источников данных в название канала данных и наоборот.
 *
 * Функции #hyscan_track_get_name_by_type и #hyscan_track_get_type_by_name используются для
 * преобразования типа галсов в строковое представление и наоборот.
 *
 */

#ifndef __HYSCAN_TYPES_H__
#define __HYSCAN_TYPES_H__

#include <glib.h>
#include <hyscan-api.h>

G_BEGIN_DECLS

/** \brief Типы данных */
typedef enum
{
  HYSCAN_DATA_INVALID                        = 0,              /**< Недопустимый тип, ошибка. */

  HYSCAN_DATA_BLOB                           = 101,            /**< Неструктурированные двоичные данные. */
  HYSCAN_DATA_STRING                         = 102,            /**< Строка с нулём на конце. */
  HYSCAN_DATA_FLOAT                          = 103,            /**< Действительные float значения. */
  HYSCAN_DATA_COMPLEX_FLOAT                  = 104,            /**< Комплексные float значения. */

  HYSCAN_DATA_ADC_14LE                       = 201,            /**< Действительные отсчёты АЦП младшие 14 бит из 16, формат little endian. */
  HYSCAN_DATA_ADC_16LE                       = 202,            /**< Действительные отсчёты АЦП 16 бит, формат little endian. */
  HYSCAN_DATA_ADC_24LE                       = 203,            /**< Действительные отсчёты АЦП младшие 24 бита из 32, формат little endian. */

  HYSCAN_DATA_COMPLEX_ADC_14LE               = 301,            /**< Комплексные отсчёты АЦП младшие 14 бит из 16, формат little endian. */
  HYSCAN_DATA_COMPLEX_ADC_16LE               = 302,            /**< Комплексные отсчёты АЦП 16 бит, формат little endian. */
  HYSCAN_DATA_COMPLEX_ADC_24LE               = 303,            /**< Комплексные отсчёты АЦП младшие 24 бита из 32, формат little endian. */

  HYSCAN_DATA_AMPLITUDE_INT_8LE              = 401,            /**< Амплитудные значения целые 8 бит, формат little endian. */
  HYSCAN_DATA_AMPLITUDE_INT_16LE             = 402,            /**< Амплитудные значения целые 16 бит, формат little endian. */
  HYSCAN_DATA_AMPLITUDE_INT_24LE             = 403,            /**< Амплитудные значения целые младшие 24 бита из 32, формат little endian. */
  HYSCAN_DATA_AMPLITUDE_INT_32LE             = 404,            /**< Амплитудные значения целые 32 бита, формат little endian. */
  HYSCAN_DATA_AMPLITUDE_FLOAT                = 405,            /**< Амплитудные float значения. */
} HyScanDataType;

/** \brief Типы источников данных */
typedef enum
{
  HYSCAN_SOURCE_INVALID                        = 0,            /**< Недопустимый тип, ошибка. */

  HYSCAN_SOURCE_SIDE_SCAN_STARBOARD            = 101,          /**< Боковой обзор, правый борт. */
  HYSCAN_SOURCE_SIDE_SCAN_PORT                 = 102,          /**< Боковой обзор, левый борт. */
  HYSCAN_SOURCE_SIDE_SCAN_STARBOARD_HI         = 103,          /**< Боковой обзор, правый борт, высокое разрешение. */
  HYSCAN_SOURCE_SIDE_SCAN_PORT_HI              = 104,          /**< Боковой обзор, левый борт, высокое разрешение. */
  HYSCAN_SOURCE_INTERFEROMETRY_STARBOARD       = 105,          /**< Интерферометрия, правый борт. */
  HYSCAN_SOURCE_INTERFEROMETRY_PORT            = 106,          /**< Интерферометрия, левый борт. */
  HYSCAN_SOURCE_ECHOSOUNDER                    = 107,          /**< Эхолот. */
  HYSCAN_SOURCE_PROFILER                       = 108,          /**< Профилограф. */
  HYSCAN_SOURCE_LOOK_AROUND_STARBOARD          = 109,          /**< Круговой обзор, правый борт. */
  HYSCAN_SOURCE_LOOK_AROUND_PORT               = 110,          /**< Круговой обзор, левый борт. */
  HYSCAN_SOURCE_FORWARD_LOOK                   = 111,          /**< Вперёдсмотрящий гидролокатор. */

  HYSCAN_SOURCE_SAS                            = 201,          /**< Сообщения САД. */
  HYSCAN_SOURCE_SAS_V2                         = 202,          /**< Сообщения САД, версия 2. */

  HYSCAN_SOURCE_NMEA_ANY                       = 301,          /**< Любые сообщения NMEA. */
  HYSCAN_SOURCE_NMEA_GGA                       = 302,          /**< Сообщения NMEA GGA. */
  HYSCAN_SOURCE_NMEA_RMC                       = 303,          /**< Сообщения NMEA RMC. */
  HYSCAN_SOURCE_NMEA_DPT                       = 304           /**< Сообщения NMEA DPT. */
} HyScanSourceType;

/** \brief Типы информационных сообщений */
typedef enum
{
  HYSCAN_LOG_LEVEL_DEBUG                       = (1),          /**< Отладочное сообщение. */
  HYSCAN_LOG_LEVEL_INFO                        = (1 << 1),     /**< Информационное сообщение. */
  HYSCAN_LOG_LEVEL_MESSAGE                     = (1 << 2),     /**< Обычное сообщение. */
  HYSCAN_LOG_LEVEL_WARNING                     = (1 << 3),     /**< Предупреждающее сообщение. */
  HYSCAN_LOG_LEVEL_CRITICAL                    = (1 << 4),     /**< Критически важное сообщение. */
  HYSCAN_LOG_LEVEL_ERROR                       = (1 << 5)      /**< Сообщение об ошибке. */
} HyScanLogLevel;

/** \brief Типы галсов */
typedef enum
{
  HYSCAN_TRACK_UNSPECIFIED                     = 0,            /**< Неопределённый тип. */

  HYSCAN_TRACK_CALIBRATION                     = 101,          /**< Галс с данными калибровки. */
  HYSCAN_TRACK_SURVEY                          = 102,          /**< Галс с данными съёмки. */
  HYSCAN_TRACK_TACK                            = 103,          /**< Лавировочный галс. */
} HyScanTrackType;

/** \brief Комплексные float числа */
typedef struct
{
  gfloat                                       re;             /**< Действительная часть. */
  gfloat                                       im;             /**< Мнимая часть. */
} HyScanComplexFloat;

/**
 *
 * Функция преобразовывает численное значение типа данных в название типа.
 *
 * Функция возвращает строку статически размещённую в памяти, эта строка не должна модифицироваться.
 *
 * \param data_type тип данных.
 *
 * \return Строка с названием типа данных.
 *
 */
HYSCAN_API
const gchar           *hyscan_data_get_type_name               (HyScanDataType                 data_type);

/**
 *
 * Функция преобразовывает строку с названием типа данных в численное значение.
 *
 * \param data_name название типа данных.
 *
 * \return Тип данных \link HyScanDataType \endlink.
 *
 */
HYSCAN_API
HyScanDataType         hyscan_data_get_type_by_name            (const gchar                   *data_name);

/**
 *
 * Функция возвращает размер одного элемента данных в байтах, для указанного типа.
 *
 * \param data_type тип данных.
 *
 * \return Размер одного елемента данных в байтах.
 *
 */
HYSCAN_API
guint32                hyscan_data_get_point_size              (HyScanDataType                 data_type);

/**
 *
 * Функция преобразовывает данные из низкоуровневого формата в float.
 *
 * \param data_type тип данных;
 * \param data указатель на преобразовываемые данные;
 * \param data_size размер преобразовываемых данных;
 * \param buffer указатель на буфер для преобразованных данных;
 * \param buffer_size размер буфера для преобразованных данных, в точках.
 *
 * \return TRUE - если преобразование выполнено, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_data_import_float                (HyScanDataType                 data_type,
                                                                gconstpointer                  data,
                                                                guint32                        data_size,
                                                                gfloat                        *buffer,
                                                                guint32                       *buffer_size);

/**
 *
 * Функция преобразовывает данные из низкоуровневого формата в HyScanComplexFloat.
 *
 * \param data_type тип данных;
 * \param data указатель на преобразовываемые данные;
 * \param data_size размер преобразовываемых данных;
 * \param buffer указатель на буфер для преобразованных данных;
 * \param buffer_size размер буфера для преобразованных данных, в точках.
 *
 * \return TRUE - если преобразование выполнено, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_data_import_complex_float        (HyScanDataType                 data_type,
                                                                gconstpointer                  data,
                                                                guint32                        data_size,
                                                                HyScanComplexFloat            *buffer,
                                                                guint32                       *buffer_size);

/**
 *
 * Функция проверяет тип источника данных на соответствие одному из типов датчиков.
 *
 * \param source тип источника данных.
 *
 * \return TRUE - если тип относится к данным датчиков, иначе - FALSE;
 *
 */
HYSCAN_API
gboolean               hyscan_source_is_sensor                 (HyScanSourceType               source);

/**
 *
 * Функция проверяет тип источника данных на соответствие "сырым" гидролокационным данным.
 *
 * \param source тип источника данных.
 *
 * \return TRUE - если тип относится к "сырым" гидролокационным данным, иначе - FALSE;
 *
 */
HYSCAN_API
gboolean               hyscan_source_is_raw                    (HyScanSourceType               source);

/**
 *
 * Функция проверяет тип источника данных на соответствие акустическим данным.
 *
 * \param source тип источника данных.
 *
 * \return TRUE - если тип относится к акустическим данным, иначе - FALSE;
 *
 */
HYSCAN_API
gboolean               hyscan_source_is_acoustic               (HyScanSourceType               source);

/**
 *
 * Функция возвращает название канала для указанных характеристик.
 * Строка, возвращаемая этой функцией, не должна изменяться пользователем.
 *
 * \param source тип источника данных;
 * \param raw признак "сырых" данных;
 * \param channel индекс канала данных, начиная с 1.
 *
 * \return Строка с названием канала или NULL - в случае ошибки.
 *
 */
HYSCAN_API
const gchar           *hyscan_channel_get_name_by_types        (HyScanSourceType               source,
                                                                gboolean                       raw,
                                                                guint                          channel);

/**
 *
 * Функция возвращает характеристики канала данных по его имени.
 *
 * \param name название канала данных;
 * \param source переменная для типа источника данных или NULL;
 * \param raw переменная для признака "сырых" данных или NULL;
 * \param channel переменная для индекса канала данных или NULL.
 *
 * \return TRUE - если характеристики канала определены, FALSE - в случае ошибки.
 *
 */
HYSCAN_API
gboolean               hyscan_channel_get_types_by_name        (const gchar                   *name,
                                                                HyScanSourceType              *source,
                                                                gboolean                      *raw,
                                                                guint                         *channel);

/**
 *
 * Функция преобразовывает численное значение типа галса в название типа.
 *
 * \param type тип галса.
 *
 * \return Строка с названием типа галса.
 *
 */
HYSCAN_API
const gchar           *hyscan_track_get_name_by_type           (HyScanTrackType                type);

/**
 *
 * Функция преобразовывает строку с названием типа галса в численное значение.
 *
 * \param name название типа галса.
 *
 * \return Тип галса \link HyScanTrackType \endlink.
 *
 */
HYSCAN_API
HyScanTrackType        hyscan_track_get_type_by_name           (const gchar                   *name);

G_END_DECLS

#endif /* __HYSCAN_TYPES_H__ */
