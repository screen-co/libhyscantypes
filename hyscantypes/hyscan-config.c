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
 * Они могут быть настроены только на этапе компиляции для portable и installed
 * сборок.
 *
 * В Linux все пути задаются абсолютными. В Windows они относительно папки с
 * выполняемой программой ("path/to/app.exe/../").
 *
 * Путь к файлам локализации определяется переменными HYSCAN_LOCALE_PREFIX и 
 * HYSCAN_LOCALE_POSTFIX. По умолчанию это "/usr/share/locale" для Linux и
 * "/share/locale" для Windows.
 *
 * Пути к файлам профилей составляются чуть сложней.
 * Безусловно присутствует каталог с т.н. системными профилями. В случае installed
 * сборки добавляется пользовательская папка с конфигурационными файлами.
 *
 * Если сборка installed (задана переменная HYSCAN_INSTALLED),
 * то к списку путей добавляется пользовательская папка с конфигурационными
 * файлами (g_get_user_config_dir()).
 *
 * При этом в случае installed базовый путь к профилям - 
 * "/usr/HYSCAN_USER_FILES_PREFIX/HYSCAN_USER_FILES_DIR",
 * а для portable -- "/../config/"
 *
 * В псевдокоде это можно представить так:
 * |[<!-- language="C" -->
 * if (INSTALLED)
 *  dirs += g_get_user_config_dir ()
 *
 * if (INSTALLED)
 *   infix = "/share/"
 * else
 *   infix = "./../config/"
 *
 * if (G_OS_WIN32)
 *   prefix = "path/to/app/../"
 * if (G_OS_UNIX and INSTALLED)
 *   prefix = "/usr/"
 * if (G_OS_UNIX and not INSTALLED)
 *   prefix = "../"
 * ]|
 */


#include "hyscan-config.h"
#include <hyscan-constructor.h>
#include <stdlib.h>

static void          hyscan_config_initialise        (void);
static void          hyscan_config_clear             (void);
static const gchar * hyscan_config_installation_dir  (void);

static gchar **      hyscan_config_profiles = NULL;
static gchar *       hyscan_config_locale = NULL;
static gchar *       hyscan_config_user_files = NULL;

HYSCAN_CONSTRUCTOR (hyscan_config_initialise);

/* Инициализация. */
static void
hyscan_config_initialise (void)
{
  gint i = 0;
  gchar ** profiles = NULL;
  gchar * locale = NULL;
  gchar * user_files = NULL;

  #ifdef G_OS_WIN32
    gchar *utf8_path;
  #endif

  /* Локализация. */
  locale = g_build_filename (hyscan_config_installation_dir (),
                             HYSCAN_LOCALE_PREFIX,
                             HYSCAN_LOCALE_POSTFIX,
                             NULL);
  #ifdef G_OS_WIN32
    utf8_path = locale;
    locale = g_win32_locale_filename_from_utf8 (utf8_path);
    g_free (utf8_path);
  #endif

  /* Пользовательские файлы. */
  #ifdef HYSCAN_INSTALLED
    user_files = g_build_filename (g_get_user_config_dir (),
                                   HYSCAN_USER_FILES_DIR, 
                                   NULL);
  #else
    user_files = g_build_filename (hyscan_config_installation_dir (),
                                   HYSCAN_USER_FILES_PREFIX,
                                   HYSCAN_USER_FILES_DIR, 
                                   NULL);
  #endif

  /* Профили. */
  #ifdef HYSCAN_INSTALLED
    profiles = g_realloc (profiles, ++i * sizeof (gchar*));
    profiles[i - 1] = g_build_filename (hyscan_config_installation_dir (),
                                        HYSCAN_USER_FILES_PREFIX,
                                        HYSCAN_USER_FILES_DIR,
                                        NULL);
  #endif

  profiles = g_realloc (profiles, ++i * sizeof (gchar*));
  profiles[i - 1] = g_strdup (user_files);

  /* NULL-терминируем. */
  profiles = g_realloc (profiles, ++i * sizeof (gchar*));
  profiles[i - 1] = NULL;

  hyscan_config_locale = locale;
  hyscan_config_user_files = user_files;
  hyscan_config_profiles = profiles;

  atexit (hyscan_config_clear);
}

/* Очистка. */
static void
hyscan_config_clear (void)
{
  g_strfreev (hyscan_config_profiles);
  g_free (hyscan_config_locale);
  g_free (hyscan_config_user_files);
}

/* Функция возвращает базовый путь. В виндоус это папка, где установлено
 * запускаемое приложение, в линуксе это либо корень ФС, либо папка, где лежит
 * приложение. */
static const gchar *
hyscan_config_installation_dir (void)
{
  static gchar *base_path = NULL;

  if (base_path != NULL)
    return base_path;

  #ifdef G_OS_WIN32
    /* ... If that directory's last component is "bin" or "lib",
     * its parent directory is returned, otherwise the directory itself. */
    base_path = g_win32_get_package_installation_directory_of_module (NULL);
    if (base_path == NULL)
      {
        g_error ("Something is totally wrong");
        return NULL;
      }
  #else
    #ifdef HYSCAN_INSTALLED
      base_path = g_strdup ("/usr");
    #else
      base_path = g_strdup ("../");
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
  return (const gchar **) hyscan_config_profiles;
}

/**
 * hyscan_config_get_locale_dir:
 *
 * Returns: (transfer none): папка с файлами локализации.
 */
const gchar *
hyscan_config_get_locale_dir (void)
{
  return hyscan_config_locale;
}

/**
 * hyscan_config_get_user_files_dir:
 *
 * Returns: (transfer none): папка, в которую можно класть файлы пользователя 
 * (логи, конфигурации, etc)
 */
const gchar *
hyscan_config_get_user_files_dir (void)
{
  return hyscan_config_user_files;
}
