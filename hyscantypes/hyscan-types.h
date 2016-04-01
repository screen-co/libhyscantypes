/**
 * \file hyscan-types.h
 *
 * \brief Заголовочный файл вспомогательных функций с определениями типов объектов HyScan
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2015
 * \license Проприетарная лицензия ООО "Экран"
 *
 * \defgroup HyScanTypes HyScanTypes - вспомогательные функции с определениями типов объектов HyScan
 *
 * Группа функции HyScanTypes используется для определения следующих типов:
 *
 * - \link HyScanSonarType \endlink - типы гидролокаторов;
 * - \link HyScanSonarBoardType \endlink - типы бортов гидролокатора;
 * - \link HyScanSonarDataType \endlink - типы гидролокационных данных;
 * - \link HyScanDataType \endlink - типы данных.
 *
 * Для определения названий каналов, по типу сохраняемых в них данных и их характеристикам,
 * предназначена функция #hyscan_get_channel_name_by_types.  Определить характеристики данных
 * по названию канала можно функцией #hyscan_get_channel_types_by_name.
 *
 * Для данных доступны функции определения типа по имени данных и наоборот, определения размера одного отсчёта
 * в данных и преобразования данных:
 *
 * - #hyscan_get_data_type_by_name - функция определяет тип данных по имени;
 * - #hyscan_get_data_type_name - функция возвращает название данных для указанного типа;
 * - #hyscan_get_data_point_size - функция возвращает размер одного элемента данных в байтах;
 * - #hyscan_data_import_complex_float - функция преобразовывает данные из низкоуровневого формата в \link HyScanComplexFloat \endlink;
 * - #hyscan_data_import_complex_double - функция преобразовывает данные из низкоуровневого формата в \link HyScanComplexFloat \endlink.
 *
 */

#ifndef __HYSCAN_TYPES_H__
#define __HYSCAN_TYPES_H__

#include <glib.h>
#include <hyscan-types-exports.h>

G_BEGIN_DECLS

/** \brief Типы гидролокаторов */
typedef enum
{
  HYSCAN_SONAR_INVALID                 = 0,            /**< Недопустимый тип, ошибка. */

  HYSCAN_SONAR_ECHOSOUNDER             = 101,          /**< Эхолот. */
  HYSCAN_SONAR_SIDE_SCAN               = 102,          /**< Гидролокатор бокового обзора. */
  HYSCAN_SONAR_SIDE_SCAN_DF            = 104,          /**< Гидролокатор бокового обзора двухчастотный. */
  HYSCAN_SONAR_LOOK_AROUND             = 106,          /**< Гидролокатор кругового обзора. */
  HYSCAN_SONAR_LOOK_AHEAD              = 107,          /**< Вперёд смотрящий гидролокатор. */
  HYSCAN_SONAR_PROFILER                = 108,          /**< Профилограф. */
  HYSCAN_SONAR_MULTI_BEAM              = 109,          /**< Многолучевой эхолот. */
  HYSCAN_SONAR_INTERFEROMETER          = 110           /**< Интерферометрический гидролокатор. */
} HyScanSonarType;

/** \brief Типы бортов */
typedef enum
{
  HYSCAN_SONAR_BOARD_INVALID           = 0,            /**< Недопустимый тип, ошибка. */

  HYSCAN_SONAR_BOARD_ANY               = 101,          /**< Не определён. */

  HYSCAN_SONAR_BOARD_FORWARD           = 201,          /**< Вперёд. */
  HYSCAN_SONAR_BOARD_BACKWARD          = 202,          /**< Назад. */
  HYSCAN_SONAR_BOARD_BOTTOM            = 203,          /**< Под собой. */
  HYSCAN_SONAR_BOARD_LEFT              = 204,          /**< Влево. */
  HYSCAN_SONAR_BOARD_RIGHT             = 205,          /**< Вправо. */
  HYSCAN_SONAR_BOARD_AROUND            = 206           /**< Круговой обзор. */
} HyScanSonarBoardType;

/** \brief Типы гидролокационных данных. */
typedef enum
{
  HYSCAN_SONAR_DATA_INVALID            = 0,            /**< Недопустимый тип, ошибка. */

  HYSCAN_SONAR_DATA_ECHOSOUNDER        = 101,          /**< Эхолот. */
  HYSCAN_SONAR_DATA_SIDE_SCAN          = 102,          /**< Боковой обзор. */

  HYSCAN_SONAR_DATA_SAS                = 201,          /**< Сообщения САД. */

  HYSCAN_SONAR_DATA_NMEA_GSA           = 301,          /**< Сообщения NMEA GSA. */
  HYSCAN_SONAR_DATA_NMEA_GSV           = 302,          /**< Сообщения NMEA GSV. */
  HYSCAN_SONAR_DATA_NMEA_GGA           = 303,          /**< Сообщения NMEA GGA. */
  HYSCAN_SONAR_DATA_NMEA_GLL           = 304,          /**< Сообщения NMEA GLL. */
  HYSCAN_SONAR_DATA_NMEA_RMC           = 305,          /**< Сообщения NMEA RMC. */
  HYSCAN_SONAR_DATA_NMEA_DPT           = 306           /**< Сообщения NMEA DPT. */
} HyScanSonarDataType;

/** \brief Типы данных */
typedef enum
{
  HYSCAN_DATA_TYPE_INVALID             = 0,            /**< Недопустимый тип, ошибка. */

  HYSCAN_DATA_TYPE_STRING              = 101,          /**< Строка с нулём на конце. */

  HYSCAN_DATA_TYPE_ADC_8BIT            = 201,          /**< Действительные отсчёты АЦП 8 бит. */
  HYSCAN_DATA_TYPE_ADC_10BIT           = 202,          /**< Действительные отсчёты АЦП 10 бит. */
  HYSCAN_DATA_TYPE_ADC_12BIT           = 203,          /**< Действительные отсчёты АЦП 12 бит. */
  HYSCAN_DATA_TYPE_ADC_14BIT           = 204,          /**< Действительные отсчёты АЦП 14 бит. */
  HYSCAN_DATA_TYPE_ADC_16BIT           = 205,          /**< Действительные отсчёты АЦП 16 бит. */

  HYSCAN_DATA_TYPE_COMPLEX_ADC_8BIT    = 301,          /**< Комплексные отсчёты АЦП 8 бит. */
  HYSCAN_DATA_TYPE_COMPLEX_ADC_10BIT   = 302,          /**< Комплексные отсчёты АЦП 10 бит. */
  HYSCAN_DATA_TYPE_COMPLEX_ADC_12BIT   = 303,          /**< Комплексные отсчёты АЦП 12 бит. */
  HYSCAN_DATA_TYPE_COMPLEX_ADC_14BIT   = 304,          /**< Комплексные отсчёты АЦП 14 бит. */
  HYSCAN_DATA_TYPE_COMPLEX_ADC_16BIT   = 305,          /**< Комплексные отсчёты АЦП 16 бит. */

  HYSCAN_DATA_TYPE_INT8                = 401,          /**< Действительные целые 8 битные значения со знаком. */
  HYSCAN_DATA_TYPE_UINT8               = 402,          /**< Действительные целые 8 битные значения без знака. */
  HYSCAN_DATA_TYPE_INT16               = 403,          /**< Действительные целые 16 битные значения со знаком. */
  HYSCAN_DATA_TYPE_UINT16              = 404,          /**< Действительные целые 16 битные значения без знака. */
  HYSCAN_DATA_TYPE_INT32               = 405,          /**< Действительные целые 32 битные значения со знаком. */
  HYSCAN_DATA_TYPE_UINT32              = 406,          /**< Действительные целые 32 битные значения без знака. */
  HYSCAN_DATA_TYPE_INT64               = 407,          /**< Действительные целые 64 битные значения со знаком. */
  HYSCAN_DATA_TYPE_UINT64              = 408,          /**< Действительные целые 64 битные значения без знака. */

  HYSCAN_DATA_TYPE_COMPLEX_INT8        = 501,          /**< Комплексные целые 8 битные значения со знаком. */
  HYSCAN_DATA_TYPE_COMPLEX_UINT8       = 502,          /**< Комплексные целые 8 битные значения без знака. */
  HYSCAN_DATA_TYPE_COMPLEX_INT16       = 503,          /**< Комплексные целые 16 битные значения со знаком. */
  HYSCAN_DATA_TYPE_COMPLEX_UINT16      = 504,          /**< Комплексные целые 16 битные значения без знака. */
  HYSCAN_DATA_TYPE_COMPLEX_INT32       = 505,          /**< Комплексные целые 32 битные значения со знаком. */
  HYSCAN_DATA_TYPE_COMPLEX_UINT32      = 506,          /**< Комплексные целые 32 битные значения без знака. */
  HYSCAN_DATA_TYPE_COMPLEX_INT64       = 507,          /**< Комплексные целые 64 битные значения со знаком. */
  HYSCAN_DATA_TYPE_COMPLEX_UINT64      = 508,          /**< Комплексные целые 64 битные значения без знака. */

  HYSCAN_DATA_TYPE_FLOAT               = 601,          /**< Действительные float значения. */
  HYSCAN_DATA_TYPE_DOUBLE              = 602,          /**< Действительные double значения. */

  HYSCAN_DATA_TYPE_COMPLEX_FLOAT       = 701,          /**< Комплексные float значения. */
  HYSCAN_DATA_TYPE_COMPLEX_DOUBLE      = 702           /**< Комплексные double значения. */
} HyScanDataType;

/** \brief Комплексные float числа */
typedef struct
{
  gfloat                               re;             /**< Действительная часть. */
  gfloat                               im;             /**< Мнимая часть. */
} HyScanComplexFloat;

/** \brief Комплексные double числа */
typedef struct
{
  gdouble                              re;             /**< Действительная часть. */
  gdouble                              im;             /**< Мнимая часть. */
} HyScanComplexDouble;

/**
 *
 * Функция возвращает название канала для указанных характеристик.
 * Строка, возвращаемая этой функцией, не должна изменяться пользователем.
 *
 * Индекс канала данных для одноканальных систем (ГБО, Эхолот) должен быть равным нулю.
 *
 * \param board_type тип борта;
 * \param data_type тип данных;
 * \param hi_res признак повышенного разрешения;
 * \param raw признак "сырых" данных;
 * \param index индекс канала данных.
 *
 * \return Строка с названием канала или NULL - в случае ошибки.
 *
 */
HYSCAN_TYPES_EXPORT
const gchar           *hyscan_get_channel_name_by_types        (HyScanSonarBoardType           board_type,
                                                                HyScanSonarDataType            data_type,
                                                                gboolean                       hi_res,
                                                                gboolean                       raw,
                                                                gint                           index);

/**
 *
 * Функция возвращает характеристики канала данных по его имени.
 *
 * \param channel_name название канала данных;
 * \param board_type переменная для типа борта или NULL;
 * \param data_type переменная для типа данных или NULL;
 * \param hi_res переменная для признака повышенного разрешения или NULL;
 * \param raw переменная для признака "сырых" данных или NULL;
 * \param index переменная для индекса канала данных.
 *
 * \return TRUE - если характеристики канала определены, FALSE - в случае ошибки.
 *
 */
gboolean               hyscan_get_channel_types_by_name        (const gchar                   *channel_name,
                                                                HyScanSonarBoardType          *board_type,
                                                                HyScanSonarDataType           *data_type,
                                                                gboolean                      *hi_res,
                                                                gboolean                      *raw,
                                                                gint                          *index);

/**
 *
 *  Функция преобразовывает строку с названием типа данных в нумерованное значение.
 *
 * \param data_name название типа данных.
 *
 * \return Тип данных \link HyScanDataType \endlink.
 *
 */
HYSCAN_TYPES_EXPORT
HyScanDataType         hyscan_get_data_type_by_name            (const gchar                   *data_name);

/**
 *
 *  Функция преобразовывает нумерованное значение типа данных в название типа.
 *
 * Функция возвращает строку статически размещённую в памяти, эта строка не должна модифицироваться.
 *
 * \param data_type тип данных.
 *
 * \return Строка с названием типа данных.
 *
 */
HYSCAN_TYPES_EXPORT
const gchar           *hyscan_get_data_type_name               (HyScanDataType                 data_type);

/**
 *
 *  Функция возвращает размер одного элемента данных в байтах, для указанного типа.
 *
 * \param data_type тип данных.
 *
 * \return Размер одного елемента данных в байтах.
 *
 */
HYSCAN_TYPES_EXPORT
gint32                 hyscan_get_data_point_size              (HyScanDataType                 data_type);

/**
 *
 * Функция преобразовывает данные из низкоуровневого формата в HyScanComplexFloat.
 *
 * Для данных, хранящихся в целочисленных форматах, используется преобразование из little endian в формат
 * хранения данных используемого процессора.
 *
 * \param data_type тип данных;
 * \param data указатель на преобразовываемые данные;
 * \param data_size размер преобразовываемых данных;
 * \param buffer указатель на буфер для преобразованных данных;
 * \param buffer_size размер буфера для преобразованных данных в точках.
 *
 * \return TRUE - если преобразование выполнено, FALSE - в случае ошибки.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_import_complex_float        (HyScanDataType                 data_type,
                                                                gpointer                       data,
                                                                gint32                         data_size,
                                                                HyScanComplexFloat            *buffer,
                                                                gint32                        *buffer_size);

/**
 *
 * Функция преобразовывает данные из низкоуровневого формата в HyScanComplexDouble.
 *
 * Для данных, хранящихся в целочисленных форматах, используется преобразование из little endian в формат
 * хранения данных используемого процессора.
 *
 * \param data_type тип данных;
 * \param data указатель на преобразовываемые данные;
 * \param data_size размер преобразовываемых данных;
 * \param buffer указатель на буфер для преобразованных данных;
 * \param buffer_size размер буфера для преобразованных данных в точках.
 *
 * \return TRUE - если преобразование выполнено, FALSE - в случае ошибки.
 *
 */
HYSCAN_TYPES_EXPORT
gboolean               hyscan_data_import_complex_double       (HyScanDataType                 data_type,
                                                                gpointer                       data,
                                                                gint32                         data_size,
                                                                HyScanComplexDouble           *buffer,
                                                                gint32                        *buffer_size);

G_END_DECLS

#endif /* __HYSCAN_TYPES_H__ */
