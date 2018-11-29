/* hyscan-param.c
 *
 * Copyright 2016-2018 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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
 * SECTION: hyscan-param
 * @Short_description: интерфейс для работы с именованными параметрами
 * @Title: HyScanParam
 *
 * Интерфейс обеспечивает унифицированный доступ к именованным параметрам.
 *
 * Схему параметрв можно получить с помощью функции #hyscan_param_schema.
 *
 * Чтение и запись значений параметров осуществляется с использованием функций
 * #hyscan_param_set и #hyscan_param_get. Список параметров и их значения
 * передаются с помощью #HyScanParamList.
 */

#include "hyscan-param.h"

G_DEFINE_INTERFACE (HyScanParam, hyscan_param, G_TYPE_OBJECT)

static void
hyscan_param_default_init (HyScanParamInterface *iface)
{
}

/**
 * hyscan_param_schema:
 * @param: указатель на #HyScanParam
 *
 * Функция возвращает текущую схему параметров.
 *
 * Returns: (transfer full): #HyScanDataSchema или NULL в случае ошибки.
 * Для удаления #g_object_unref.
 */
HyScanDataSchema *
hyscan_param_schema (HyScanParam *param)
{
  HyScanParamInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_PARAM (param), NULL);

  iface = HYSCAN_PARAM_GET_IFACE (param);
  if (iface->schema != NULL)
    return (* iface->schema) (param);

  return NULL;
}

/**
 * hyscan_param_set:
 * @param: указатель на #HyScanParam
 * @list: список параметров для записи
 *
 * Функция записывает значения параметров. Если в списке присутствует параметр
 * без значения, будет установлено значение по умолчанию, определённое в схеме
 * данных.
 *
 * Returns: %TRUE если значения установлены, иначе %FALSE.
 */
gboolean
hyscan_param_set (HyScanParam     *param,
                  HyScanParamList *list)
{
  HyScanParamInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_PARAM (param), FALSE);

  iface = HYSCAN_PARAM_GET_IFACE (param);
  if (iface->set != NULL)
    return (* iface->set) (param, list);

  return FALSE;
}

/**
 * hyscan_param_get:
 * @param: указатель на #HyScanParam
 * @list: список параметров для чтения
 *
 * Функция считывает значения параметров, указанные в списке.
 *
 * Returns: %TRUE если значения считаны, иначе %FALSE.
 *
 */
gboolean
hyscan_param_get (HyScanParam     *param,
                  HyScanParamList *list)
{
  HyScanParamInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_PARAM (param), FALSE);

  iface = HYSCAN_PARAM_GET_IFACE (param);
  if (iface->get != NULL)
    return (* iface->get) (param, list);

  return FALSE;
}
