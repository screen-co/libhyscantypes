/* hyscan-config.h
 *
 * Copyright 2020 Screen LLC, Alexander Dmitriev <m1n7@yandex.ru>
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

/*
 * SECTION: hyscan-config
 * @Short_description: конфигурационные параметры времени компиляции
 * @Title: HyScanConfig
 *
 * Здесь предоставлены функции получения путей к профилям и файлам локализации.
 * Они могут быть настроены только на этапе компиляции для portable и standalone
 * сборок.
 *
 * В Linux все пути задаются абсолютными. В Windows они относительно папки с
 * выполняемой программой ("path/to/app.exe/../").
 *
 * Путь к файлам локализации определяется переменной FNN_LOCALE_DIR. По умолчанию
 * это "/usr/share/locale" для Linux и "/share/locale" для Windows.
 *
 * Пути к файлам профилей составляются чуть сложней.
 * Безусловно присутствует каталог с т.н. системными профилями. В случае standalone
 * сборки добавляется пользовательская папка с конфигурационными файлами.
 *
 * Если сборка standalone (задана переменная FNN_PROFILE_STANDALONE),
 * то к списку путей добавляется пользовательская папка с конфигурационными
 * файлами (g_get_user_config_dir()).
 *
 * При этом в случае standalone базовый путь к профилям - "/usr/share/HYSCAN_GTK_PROFILE_PATH",
 * а для portable -- "/../config/"
 *
 * В псевдокоде это можно представить так:
 * |[<!-- language="C" -->
 * if (STANDALONE)
 *  dirs += g_get_user_config_dir ()
 *
 * if (STANDALONE)
 *   infix = "/share/"
 * else
 *   infix = "./../config/"
 *
 * if (G_OS_WIN32)
 *   prefix = "path/to/app/../"
 * if (G_OS_UNIX and STANDALONE)
 *   prefix = "/usr/"
 * if (G_OS_UNIX and not STANDALONE)
 *   prefix = "../"
 * ]|
 */


#include "hyscan-config.h"
#include <hyscan-constructor.h>
#include <stdlib.h>


static void          hyscan_config_initialise (void);
static void          hyscan_config_clear      (void);
static const gchar * hyscan_config_base_path  (void);

static gchar **      hyscan_config_profile_dirs = NULL;
static gchar *       hyscan_config_locale_dir = NULL;

HYSCAN_CONSTRUCTOR (hyscan_config_initialise);

/* Инициализация. */
static void
hyscan_config_initialise (void)
{
  gint i = 0;
  gchar ** profile_dirs = NULL;
  gchar * locale_dir = NULL;

  #ifdef G_OS_WIN32
    gchar *utf8_path;
  #endif

  /* Локализация. */
  locale_dir = g_build_filename (hyscan_config_base_path (),
                                 HYSCAN_LOCALE_PREFIX,
                                 HYSCAN_LOCALE_POSTFIX,
                                 NULL);
  #ifdef G_OS_WIN32
    utf8_path = locale_dir;
    locale_dir = g_win32_locale_filename_from_utf8 (utf8_path);
    g_free (utf8_path);
  #endif

  /* Профили. */
  #ifndef HYSCAN_PORTABLE
    profile_dirs = g_realloc (profile_dirs, ++i * sizeof (gchar*));
    profile_dirs[i - 1] = g_build_filename (g_get_user_config_dir (),
                                            HYSCAN_PROFILE_DIR,
                                            NULL);
  #endif

  profile_dirs = g_realloc (profile_dirs, ++i * sizeof (gchar*));
  profile_dirs[i - 1] = g_build_filename (hyscan_config_base_path (),
                                          HYSCAN_PROFILE_PREFIX,
                                          HYSCAN_PROFILE_DIR,
                                          NULL);
  /* NULL-терминируем. */
  profile_dirs = g_realloc (profile_dirs, ++i * sizeof (gchar*));
  profile_dirs[i - 1] = NULL;

  hyscan_config_locale_dir = locale_dir;
  hyscan_config_profile_dirs = profile_dirs;

  atexit (hyscan_config_clear);
}

/* Очистка. */
static void
hyscan_config_clear (void)
{
  g_strfreev (hyscan_config_profile_dirs);
  g_free (hyscan_config_locale_dir);
}

/* Функция возвращает базовый путь. В виндоус это папка, где установлено
 * запускаемое приложение, в линуксе это либо корень ФС, либо папка, где лежит
 * приложение. */
static const gchar *
hyscan_config_base_path (void)
{
  static gchar *base_path = NULL;

  if (base_path != NULL)
    return base_path;

  #if G_OS_WIN32
    /* ... If that directory's last component is "bin" or "lib",
     * its parent directory is returned, otherwise the directory itself. */
    base_path = g_win32_get_package_installation_directory_of_module (NULL);
    if (base_path == NULL)
      {
        g_error ("Something is totally wrong");
        return NULL;
      }
  #else
    #if HYSCAN_PORTABLE
      base_path = g_strdup ("../");
    #else
      base_path = g_strdup ("/usr");
    #endif
  #endif

  return base_path;
}

/**
 * hyscan_config_get_profile_dirs:
 *
 * Returns: (transfer none): список папок, в которых следует искать профили.
 */
const gchar **
hyscan_config_get_profile_dirs (void)
{
  return (const gchar **) hyscan_config_profile_dirs;
}

/**
 * hyscan_config_get_locale_dir:
 *
 * Returns: (transfer none): папка с файлами локализации.
 */
const gchar *
hyscan_config_get_locale_dir (void)
{
  return hyscan_config_locale_dir;
}
