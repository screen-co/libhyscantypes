/*
 * \file hyscan-param.c
 *
 * \brief Исходный файл интерфейса для работы с именованными параметрами.
 * \author Andrei Fadeev (andrei@webcontrol.ru)
 * \date 2016
 * \license Проприетарная лицензия ООО "Экран"
 *
 */

#include "hyscan-param.h"

G_DEFINE_INTERFACE (HyScanParam, hyscan_param, G_TYPE_OBJECT)

static void
hyscan_param_default_init (HyScanParamInterface *iface)
{
}

/* Функция возвращает схему параметров. */
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

/* Функция устанавливает значения параметров. */
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

/* Функция считывает значения параметров. */
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
