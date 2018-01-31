#include <hyscan-param-list.h>

#define N_PARAMS 1000

gboolean
strv_contains (const gchar * const *strv,
               const gchar         *str)
{
  guint i;

  for (i = 0; strv[i] != NULL; i++)
    if (g_strcmp0 (strv[i], str) == 0)
      return TRUE;

  return FALSE;
}

int
main (int    argc,
      char **argv)
{
  HyScanParamList *list;
  const gchar * const * names;
  gchar *name;
  guint i;

  list = hyscan_param_list_new ();

  /* Проверяем, что изначально список пустой. */
  names = hyscan_param_list_params (list);
  if (names != NULL)
    g_error ("parameters list isn't empty (new)");

  /* Добавляем пустые параметры. */
  for (i = 0; i < N_PARAMS; i++)
    {
      name = g_strdup_printf ("/name.%d", i);
      hyscan_param_list_add (list, name);
      g_free (name);
    }

  /* Теперь список должен содержить добавленные параметры. */
  names = hyscan_param_list_params (list);
  if (names == NULL)
    g_error ("parameters list is empty (add)");

  if (g_strv_length ((gchar**)names) != N_PARAMS)
    g_error ("parameters list size mismatch (add)");

  /* Проверяем, что все параметры есть в списке. */
  for (i = 0; i < N_PARAMS; i++)
    {
      name = g_strdup_printf ("/name.%d", i);

      if (!hyscan_param_list_contains (list, name))
        g_error ("parameter %s not found in list", name);

      if (!strv_contains (names, name))
        g_error ("parameter %s not found in names", name);

      if (hyscan_param_list_get (list, name) != NULL)
        g_error ("parameter %s isn't empty", name);

      if (hyscan_param_list_get_boolean (list, name))
        g_error ("parameter %s isn't empty (boolean)", name);

      if (hyscan_param_list_get_integer (list, name) != 0)
        g_error ("parameter %s isn't empty (integer)", name);

      if (hyscan_param_list_get_double (list, name) != 0.0)
        g_error ("parameter %s isn't empty (double)", name);

      if (hyscan_param_list_get_string (list, name) != NULL)
        g_error ("parameter %s isn't empty (string)", name);

      if (hyscan_param_list_get_enum (list, name) != 0)
        g_error ("parameter %s isn't empty (enum)", name);

      g_free (name);
    }

  /* Очищаем список параметров. */
  hyscan_param_list_clear (list);

  /* После очистки список параметров должен быть пустым. */
  names = hyscan_param_list_params (list);
  if (names != NULL)
    g_error ("parameters list isn't empty (clear)");

  /* Добавляем все типы параметров. */
  for (i = 0; i < N_PARAMS; i++)
    {
      GVariant *vvalue;
      gboolean bvalue;
      gint64 ivalue;
      gdouble dvalue;
      gchar *svalue;
      gint64 evalue;

      /* Добавляем boolean значения. */
      name = g_strdup_printf ("/boolean/name.%d", i);
      bvalue = (i % 5) ? TRUE : FALSE;
      if (i % 2)
        {
          vvalue = g_variant_new_boolean (bvalue);
          hyscan_param_list_set (list, name, vvalue);
        }
      else
        {
          hyscan_param_list_set_boolean (list, name, bvalue);
        }
      g_free (name);

      /* Добавляем integer значения. */
      name = g_strdup_printf ("/integer/name.%d", i);
      ivalue = i - N_PARAMS / 2;
      if (i % 2)
        {
          vvalue = g_variant_new_int64 (ivalue);
          hyscan_param_list_set (list, name, vvalue);
        }
      else
        {
          hyscan_param_list_set_integer (list, name, ivalue);
        }
      g_free (name);

      /* Добавляем double значения. */
      name = g_strdup_printf ("/double/name.%d", i);
      dvalue = 1.0 / (gdouble)(i + 1);
      if (i % 2)
        {
          vvalue = g_variant_new_double (dvalue);
          hyscan_param_list_set (list, name, vvalue);
        }
      else
        {
          hyscan_param_list_set_double (list, name, dvalue);
        }
      g_free (name);

      /* Добавляем string значения. */
      name = g_strdup_printf ("/string/name.%d", i);
      svalue = g_strdup_printf ("string %d", i);
      if (i % 2)
        {
          vvalue = g_variant_new_string (svalue);
          hyscan_param_list_set (list, name, vvalue);
        }
      else
        {
          hyscan_param_list_set_string (list, name, svalue);
        }
      g_free (svalue);
      g_free (name);

      /* Добавляем enum значения. */
      name = g_strdup_printf ("/enum/name.%d", i);
      evalue = i + N_PARAMS / 2;
      if (i % 2)
        {
          vvalue = g_variant_new_int64 (evalue);
          hyscan_param_list_set (list, name, vvalue);
        }
      else
        {
          hyscan_param_list_set_enum (list, name, evalue);
        }
      g_free (name);
    }

  /* Проверяем список параметров. */
  names = hyscan_param_list_params (list);
  if (names == NULL)
    g_error ("parameters list is empty (set)");

  /* Проверяем, что все параметры есть в списке. */
  for (i = 0; i < N_PARAMS; i++)
    {
      GVariant *vvalue;
      gboolean bvalue;
      gint64 ivalue;
      gdouble dvalue;
      gchar *svalue;
      gint64 evalue;

      /* Проверяем boolean значения. */
      name = g_strdup_printf ("/boolean/name.%d", i);
      bvalue = (i % 5) ? TRUE : FALSE;

      if (!hyscan_param_list_contains (list, name))
        g_error ("parameter %s not found in list", name);

      if (!strv_contains (names, name))
        g_error ("parameter %s not found in names", name);

      if (hyscan_param_list_get_boolean (list, name) != bvalue)
        g_error ("parameter %s error", name);

      vvalue = hyscan_param_list_get (list, name);
      if (g_variant_classify (vvalue) != G_VARIANT_CLASS_BOOLEAN)
        g_error ("parameter %s type error", name);

      if (g_variant_get_boolean (vvalue) != bvalue)
        g_error ("parameter %s error (variant)", name);

      g_variant_unref (vvalue);
      g_free (name);

      /* Проверяем integer значения. */
      name = g_strdup_printf ("/integer/name.%d", i);
      ivalue = i - N_PARAMS / 2;

      if (!hyscan_param_list_contains (list, name))
        g_error ("parameter %s not found in list", name);

      if (!strv_contains (names, name))
        g_error ("parameter %s not found in names", name);

      if (hyscan_param_list_get_integer (list, name) != ivalue)
        g_error ("parameter %s error", name);

      vvalue = hyscan_param_list_get (list, name);
      if (g_variant_classify (vvalue) != G_VARIANT_CLASS_INT64)
        g_error ("parameter %s type error", name);

      if (g_variant_get_int64 (vvalue) != ivalue)
        g_error ("parameter %s error (variant)", name);

      g_variant_unref (vvalue);
      g_free (name);

      /* Проверяем double значения. */
      name = g_strdup_printf ("/double/name.%d", i);
      dvalue = 1.0 / (gdouble)(i + 1);

      if (!hyscan_param_list_contains (list, name))
        g_error ("parameter %s not found in list", name);

      if (!strv_contains (names, name))
        g_error ("parameter %s not found in names", name);

      dvalue -= hyscan_param_list_get_double (list, name);
      if (ABS (dvalue) > 1e-10)
        g_error ("parameter %s error", name);

      dvalue = 1.0 / (gdouble)(i + 1);
      vvalue = hyscan_param_list_get (list, name);
      if (g_variant_classify (vvalue) != G_VARIANT_CLASS_DOUBLE)
        g_error ("parameter %s type error", name);

      dvalue -= g_variant_get_double (vvalue);
      if (ABS (dvalue) > 1e-10)
        g_error ("parameter %s error (variant)", name);

      g_variant_unref (vvalue);
      g_free (name);

      /* Проверяем string значения. */
      name = g_strdup_printf ("/string/name.%d", i);
      svalue = g_strdup_printf ("string %d", i);

      if (!hyscan_param_list_contains (list, name))
        g_error ("parameter %s not found in list", name);

      if (!strv_contains (names, name))
        g_error ("parameter %s not found in names", name);

      if (g_strcmp0 (hyscan_param_list_get_string (list, name), svalue) != 0)
        g_error ("parameter %s error", name);

      vvalue = hyscan_param_list_get (list, name);
      if (g_variant_classify (vvalue) != G_VARIANT_CLASS_STRING)
        g_error ("parameter %s type error", name);

      if (g_strcmp0 (g_variant_get_string (vvalue, NULL), svalue) != 0)
        g_error ("parameter %s error (variant)", name);

      g_variant_unref (vvalue);
      g_free (svalue);
      g_free (name);

      /* Проверяем enum значения. */
      name = g_strdup_printf ("/enum/name.%d", i);
      evalue = i + N_PARAMS / 2;

      if (!hyscan_param_list_contains (list, name))
        g_error ("parameter %s not found in list", name);

      if (!strv_contains (names, name))
        g_error ("parameter %s not found in names", name);

      if (hyscan_param_list_get_enum (list, name) != evalue)
        g_error ("parameter %s error", name);

      vvalue = hyscan_param_list_get (list, name);
      if (g_variant_classify (vvalue) != G_VARIANT_CLASS_INT64)
        g_error ("parameter %s type error", name);

      if (g_variant_get_int64 (vvalue) != evalue)
        g_error ("parameter %s error (variant)", name);

      g_variant_unref (vvalue);
      g_free (name);
    }

  g_object_unref (list);

  g_message ("All done");

  return 0;
}
