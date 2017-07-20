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
hyscan_param_set (HyScanParam         *param,
                  const gchar *const  *names,
                  GVariant           **values)
{
  HyScanParamInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_PARAM (param), FALSE);

  iface = HYSCAN_PARAM_GET_IFACE (param);
  if (iface->set != NULL)
    return (* iface->set) (param, names, values);

  return FALSE;
}

/* Функция считывает значения параметров. */
gboolean
hyscan_param_get (HyScanParam         *param,
                  const gchar *const  *names,
                  GVariant           **values)
{
  HyScanParamInterface *iface;

  g_return_val_if_fail (HYSCAN_IS_PARAM (param), FALSE);

  iface = HYSCAN_PARAM_GET_IFACE (param);
  if (iface->get != NULL)
    return (* iface->get) (param, names, values);

  return FALSE;
}

/* Функция устанавливает значение параметра по умолчанию. */
gboolean
hyscan_param_set_default (HyScanParam *param,
                          const gchar *name)
{
  const gchar *names[2];
  GVariant *values[1];

  names[0] = name;
  names[1] = NULL;

  values[0] = NULL;

  return hyscan_param_set (param, names, values);
}

/* Функция устанавливает значение параметра типа BOOLEAN. */
gboolean
hyscan_param_set_boolean (HyScanParam *param,
                          const gchar *name,
                          gboolean     value)
{
  const gchar *names[2];
  GVariant *values[1];

  names[0] = name;
  names[1] = NULL;

  values[0] = g_variant_new_boolean (value);

  if (hyscan_param_set (param, names, values))
    return TRUE;

  g_variant_unref (values[0]);

  return FALSE;
}

/* Функция устанавливает значение параметра типа INTEGER. */
gboolean
hyscan_param_set_integer (HyScanParam *param,
                          const gchar *name,
                          gint64       value)
{
  const gchar *names[2];
  GVariant *values[1];

  names[0] = name;
  names[1] = NULL;

  values[0] = g_variant_new_int64 (value);

  if (hyscan_param_set (param, names, values))
    return TRUE;

  g_variant_unref (values[0]);

  return FALSE;
}

/* Функция устанавливает значение параметра типа DOUBLE. */
gboolean
hyscan_param_set_double (HyScanParam *param,
                         const gchar *name,
                         gdouble      value)
{
  const gchar *names[2];
  GVariant *values[1];

  names[0] = name;
  names[1] = NULL;

  values[0] = g_variant_new_double (value);

  if (hyscan_param_set (param, names, values))
    return TRUE;

  g_variant_unref (values[0]);

  return FALSE;
}

/* Функция устанавливает значение параметра типа STRING. */
gboolean
hyscan_param_set_string (HyScanParam *param,
                         const gchar *name,
                         const gchar *value)
{
  const gchar *names[2];
  GVariant *values[1];

  names[0] = name;
  names[1] = NULL;

  if (value != NULL)
    values[0] = g_variant_new_string (value);
  else
    values[0] = NULL;

  if (hyscan_param_set (param, names, values))
    return TRUE;

  if (values[0] != NULL)
    g_variant_unref (values[0]);

  return FALSE;
}

/* Функция устанавливает значение параметра типа ENUM. */
gboolean
hyscan_param_set_enum (HyScanParam *param,
                       const gchar *name,
                       gint64       value)
{
  const gchar *names[2];
  GVariant *values[1];

  names[0] = name;
  names[1] = NULL;

  values[0] = g_variant_new_int64 (value);

  if (hyscan_param_set (param, names, values))
    return TRUE;

  g_variant_unref (values[0]);

  return FALSE;
}

/* Функция считывает значение параметра типа BOOLEAN. */
gboolean
hyscan_param_get_boolean (HyScanParam *param,
                          const gchar *name,
                          gboolean    *value)
{
  const gchar *names[2];
  GVariant *values[1];

  names[0] = name;
  names[1] = NULL;

  if (!hyscan_param_get (param, names, values))
    return FALSE;

  *value = g_variant_get_boolean (values[0]);
  g_variant_unref (values[0]);

  return TRUE;
}

/* Функция считывает значение параметра типа INTEGER. */
gboolean
hyscan_param_get_integer (HyScanParam *param,
                          const gchar *name,
                          gint64      *value)
{
  const gchar *names[2];
  GVariant *values[1];

  names[0] = name;
  names[1] = NULL;

  if (!hyscan_param_get (param, names, values))
    return FALSE;

  *value = g_variant_get_int64 (values[0]);
  g_variant_unref (values[0]);

  return TRUE;
}

/* Функция считывает значение параметра типа DOUBLE. */
gboolean
hyscan_param_get_double (HyScanParam *param,
                         const gchar *name,
                         gdouble     *value)
{
  const gchar *names[2];
  GVariant *values[1];

  names[0] = name;
  names[1] = NULL;

  if (!hyscan_param_get (param, names, values))
    return FALSE;

  *value = g_variant_get_double (values[0]);
  g_variant_unref (values[0]);

  return TRUE;
}

/* Функция считывает значение параметра типа STRING. */
gchar *
hyscan_param_get_string (HyScanParam *param,
                         const gchar *name)
{
  const gchar *names[2];
  GVariant *values[1];
  gchar *value;

  names[0] = name;
  names[1] = NULL;

  if (!hyscan_param_get (param, names, values))
    return NULL;

  if (values[0] == NULL)
    return NULL;

  value = g_variant_dup_string (values[0], NULL);
  g_variant_unref (values[0]);

  return value;
}

/* Функция считывает значение параметра типа ENUM. */
gboolean
hyscan_param_get_enum (HyScanParam *param,
                       const gchar *name,
                       gint64      *value)
{
  const gchar *names[2];
  GVariant *values[1];

  names[0] = name;
  names[1] = NULL;

  if (!hyscan_param_get (param, names, values))
    return FALSE;

  *value = g_variant_get_int64 (values[0]);
  g_variant_unref (values[0]);

  return TRUE;
}
