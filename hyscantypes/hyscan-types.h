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
 * Для данных доступны функции определения типа по имени данных и наоборот, определения размера одного отсчёта
 * в данных и преобразования данных:
 *
 * - #hyscan_get_data_type_by_name - функция определяет тип данных по имени;
 * - #hyscan_get_data_type_name - функция возвращает название данных для указанного типа;
 * - #hyscan_get_data_point_size - функция возвращает размер одного элемента данных в байтах;
 * - #hyscan_import_data - функция преобразовывает данные из низкоуровневого формата в \link HyScanComplexFloat \endlink.
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

  HYSCAN_SONAR_ECHO                    = 101,          /**< Эхолот. */
  HYSCAN_SONAR_SIDE_SCAN               = 102,          /**< Гидролокатор бокового обзора. */
  HYSCAN_SONAR_SIDE_SCAN_WITH_ECHO     = 103,          /**< Гидролокатор бокового обзора с эхолотом */
  HYSCAN_SONAR_SIDE_SCAN_DF            = 104,          /**< Гидролокатор бокового обзора двухчастотный. */
  HYSCAN_SONAR_SIDE_SCAN_WITH_ECHO_DF  = 105,          /**< Гидролокатор бокового обзора с эхолотом двухчастотный. */
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

  HYSCAN_SONAR_BOARD_FORWARD           = 101,          /**< Вперёд. */
  HYSCAN_SONAR_BOARD_BOTTOM            = 102,          /**< Под собой. */
  HYSCAN_SONAR_BOARD_LEFT              = 103,          /**< Левый борт. */
  HYSCAN_SONAR_BOARD_RIGHT             = 104           /**< Правый борт. */
} HyScanSonarBoardType;

/** \brief Типы гидролокационных данных */
typedef enum
{
  HYSCAN_SONAR_DATA_TYPE_INVALID       = 0,            /**< Недопустимый тип, ошибка. */

  HYSCAN_SONAR_DATA_TYPE_NMEA          = 101,          /**< Данные навигационных датчиков типа NMEA. */

  HYSCAN_SONAR_DATA_TYPE_SIDE_SCAN     = 201,          /**< Данные бокового обзора или эхолота. */
  HYSCAN_SONAR_DATA_TYPE_MULTI_BEAM    = 202           /**< Данные многолучевого локатора. */
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
 *  Функция преобразовывает строку с названием типа данных в нумерованное значение.
 *
 * \param data_name название типа данных.
 *
 * \return Тип данных \link HyScanDataType \endlink.
 *
 */
HYSCAN_TYPES_EXPORT
HyScanDataType         hyscan_get_data_type_by_name            (const gchar           *data_name);

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
const gchar           *hyscan_get_data_type_name               (HyScanDataType         data_type);

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
gint32                 hyscan_get_data_point_size              (HyScanDataType         data_type);

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
gboolean               hyscan_data_import_complex_float        (HyScanDataType         data_type,
                                                                gpointer               data,
                                                                gint32                 data_size,
                                                                HyScanComplexFloat    *buffer,
                                                                gint32                *buffer_size);

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
gboolean               hyscan_data_import_complex_double       (HyScanDataType         data_type,
                                                                gpointer               data,
                                                                gint32                 data_size,
                                                                HyScanComplexDouble   *buffer,
                                                                gint32                *buffer_size);

G_END_DECLS

#endif /* __HYSCAN_TYPES_H__ */
