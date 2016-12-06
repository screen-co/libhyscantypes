/**
 * \file hyscan-data.h
 *
 * \brief Заголовочный файл вспомогательных функций с определениями типов данных HyScan
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2015
 * \license Проприетарная лицензия ООО "Экран"
 *
 * \defgroup HyScanData HyScanData - вспомогательные функции с определениями типов данных HyScan
 *
 * HyScanData содержит определения базовых типов данных HyScan и функция для работы с ними.
 * Доступны функции определения типа по имени данных и наоборот, определения размера одного отсчёта
 * в данных и преобразования данных:
 *
 * - #hyscan_data_get_type_by_name - функция определяет тип данных по имени;
 * - #hyscan_data_get_type_name - функция возвращает название данных для указанного типа;
 * - #hyscan_data_get_point_size - функция возвращает размер одного элемента данных в байтах;
 * - #hyscan_data_import_float - функция преобразовывает данные из низкоуровневого формата в float;
 * - #hyscan_data_import_complex_float - функция преобразовывает данные из низкоуровневого формата в \link HyScanComplexFloat \endlink.
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

  HYSCAN_DATA_ADC_14LE                       = 201,            /**< Действительные отсчёты АЦП младшие 14 бит из 16, формат little endian. */
  HYSCAN_DATA_ADC_16LE                       = 202,            /**< Действительные отсчёты АЦП 16 бит, формат little endian. */
  HYSCAN_DATA_ADC_24LE                       = 203,            /**< Действительные отсчёты АЦП младшие 24 бит из 32, формат little endian. */

  HYSCAN_DATA_COMPLEX_ADC_14LE               = 301,            /**< Комплексные отсчёты АЦП младшие 14 бит из 16, формат little endian. */
  HYSCAN_DATA_COMPLEX_ADC_16LE               = 302,            /**< Комплексные отсчёты АЦП 16 бит, формат little endian. */
  HYSCAN_DATA_COMPLEX_ADC_24LE               = 303,            /**< Комплексные отсчёты АЦП младшие 24 бит из 32, формат little endian. */

  HYSCAN_DATA_UINT8                          = 401,            /**< Действительные unsigned int8 значения. */
  HYSCAN_DATA_UINT16                         = 402,            /**< Действительные unsigned int16 значения. */
  HYSCAN_DATA_UINT32                         = 403,            /**< Действительные unsigned int32 значения. */
  HYSCAN_DATA_FLOAT                          = 404,            /**< Действительные float значения. */

  HYSCAN_DATA_COMPLEX_UINT8                  = 501,            /**< Комплексные unsigned int8 значения. */
  HYSCAN_DATA_COMPLEX_UINT16                 = 502,            /**< Комплексные unsigned int16 значения. */
  HYSCAN_DATA_COMPLEX_UINT32                 = 503,            /**< Комплексные unsigned int32 значения. */
  HYSCAN_DATA_COMPLEX_FLOAT                  = 504             /**< Комплексные float значения. */
} HyScanDataType;

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
                                                                gint32                         data_size,
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
                                                                gint32                         data_size,
                                                                HyScanComplexFloat            *buffer,
                                                                guint32                       *buffer_size);

G_END_DECLS

#endif /* __HYSCAN_TYPES_H__ */
