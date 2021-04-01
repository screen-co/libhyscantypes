/* hyscan-config.h
 *
 * Copyright 2020 Screen LLC, Alexander Dmitriev <m1n7@yandex.ru>
 * Copyright 2021 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
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
 * Здесь предоставлены функции получения путей к каталогам с файлами
 * переводов, профилям, лог файлам и т.п. Настройка этих путей выполняется
 * на этапе компиляции и зависит от типа сборки (portable или installed),
 * а также платформы (Windows, Unix).
 *
 * Существует два варианта сборки программы portable и installed. Portable
 * версия предполагает возможность запуска программы из любого места, без
 * необходимости выполнения установки в операционной системе. Installed версия
 * требует установки с использованием инсталятора HyScan (в Windows) или
 * пакетного менеджера дистрибутива (в Linux).
 *
 * В portable версии все файлы профилей, настроек, логов и пр. располагаются
 * внутри каталога с программой. А в installed, они размещаются в различных
 * каталогах в соответствии с идеологией операционной системы.
 *
 * Доступны следующие функции для определения путей:
 *
 * - #hyscan_config_get_locale_dir - возвращает путь к файлам с переводами;
 * - #hyscan_config_get_driver_dir - возвращает путь к драйверам устройств;
 * - #hyscan_config_get_system_dir - возвращает путь к общесистемным файлам;
 * - #hyscan_config_get_user_dir - возвращает путь к пользовательским файлам;
 * - #hyscan_config_get_log_dir - возвращает путь к лог файлам.
 *
 * Программы HyScan имеют возможность создавать свою иерархию подкаталогов
 * с общесистемными и пользовательскими файлами. При этом необходимо иметь
 * ввиду, что общесистемные файлы доступны только для чтения.
 */

#include "hyscan-config.h"
#include <hyscan-constructor.h>
#include <stdlib.h>

#ifdef __linux__
#include <unistd.h>
#endif

static gchar *         hyscan_config_prefix = NULL;
static gchar *         hyscan_config_locale = NULL;
static gchar *         hyscan_config_driver = NULL;
static gchar *         hyscan_config_system = NULL;
static gchar *         hyscan_config_user = NULL;
static gchar *         hyscan_config_log = NULL;

static void            hyscan_config_initialise        (void);
static void            hyscan_config_clear             (void);

HYSCAN_CONSTRUCTOR (hyscan_config_initialise);

/* Инициализация. */
static void
hyscan_config_initialise (void)
{
  /* Базовый путь. */

#ifdef G_OS_WIN32

  /* В Windows исполняемый файл HyScan находится в подкаталоге bin основного
   * каталога приложения. Функция g_win32_get_package_installation_directory_of_module
   * возвращает путь к основному каталогу, автоматически исключая bin. */

  hyscan_config_prefix = g_win32_get_package_installation_directory_of_module (NULL);
  if (hyscan_config_prefix == NULL)
    g_error ("HyScanConfig: can't get prefix path");

#else
  #ifdef HYSCAN_INSTALLED

    /* Если программа установлена в системный каталог дистрибутива,
     * используется путь HYSCAN_INSTALL_PREFIX. */

    hyscan_config_prefix = g_strdup (HYSCAN_INSTALL_PREFIX);

  #else
    #ifdef __linux__

      /* В Linux используется ссылка /proc/self/exe для определения
       * пути к исполняемому файлу в portable варианте сборки. */
      {
        gchar *exe_path;
        char self_path[2*NAME_MAX+1];
        ssize_t path_len;

        path_len = readlink ("/proc/self/exe", self_path, 2*NAME_MAX);
        if (path_len < 0)
          g_error ("HyScanConfig: can't get executable path");

        self_path[path_len] = 0;

        exe_path = g_path_get_dirname (self_path);
        hyscan_config_prefix = g_path_get_dirname (exe_path);
        g_free (exe_path);

      }
    #else

      #error "unsupported platform"

    #endif
  #endif
#endif

  /* Файлы переводов. */

  /* Файлы переводов находятся в ${hyscan_config_prefix}/HYSCAN_INSTALL_LOCALEDIR. */
  hyscan_config_locale = g_build_filename (hyscan_config_prefix,
                                           HYSCAN_INSTALL_LOCALEDIR,
                                           NULL);
  #ifdef G_OS_WIN32
    {
      gchar *utf8_path = hyscan_config_locale;
      hyscan_config_locale = g_win32_locale_filename_from_utf8 (utf8_path);
      g_free (utf8_path);
    }
  #endif

  /* Путь к драйверам. */

  #if defined (G_OS_WIN32) || !defined (HYSCAN_INSTALLED)

    /* В Windows драйверы устройств всегда хранятся в каталоге с исполняемым
     * файлом. Аналогичный путь используется в portable версиях. */

    hyscan_config_driver = g_build_filename (hyscan_config_prefix,
                                              "bin",
                                              NULL);

  #else

    /* Если программа установлена в системный каталог дистрибутива,
     * используется путь HYSCAN_INSTALL_LIBDIR. */

    hyscan_config_driver = g_build_filename (HYSCAN_INSTALL_LIBDIR,
                                             "hyscan"HYSCAN_MAJOR_VERSION,
                                             "drivers",
                                             NULL);

  #endif

    /* Системные настройки. */

    #if defined (G_OS_WIN32) || !defined (HYSCAN_INSTALLED)

      /* В Windows системные настройки программы всегда хранятся в каталоге
       * ${hyscan_config_prefix}/etc/hyscanX, где X - основная версия HyScan.
       * Аналогичный путь используется в portable версиях. */

      hyscan_config_system = g_build_filename (hyscan_config_prefix,
                                                "etc",
                                                "hyscan"HYSCAN_MAJOR_VERSION,
                                                NULL);

    #else

      /* Если программа установлена в системный каталог дистрибутива,
       * используется путь HYSCAN_INSTALL_SYSCONFDIR. */

      hyscan_config_system = g_build_filename (HYSCAN_INSTALL_SYSCONFDIR,
                                               "hyscan"HYSCAN_MAJOR_VERSION,
                                               NULL);

    #endif

  /* Пользовательские настройки. */

  #ifdef HYSCAN_INSTALLED

    /* Если программа установлена в системный каталог дистрибутива,
     * пользовательские настройки хранятся в домашнем каталоге. */

    hyscan_config_user = g_build_filename (g_get_user_config_dir (),
                                           "hyscan"HYSCAN_MAJOR_VERSION,
                                           NULL);

  #else

    /* В portable версии нет разделения на пользовательские и
     * системные настройки. */

    hyscan_config_user = g_strdup (hyscan_config_system);

  #endif

  /* Путь к лог файлам. */

  #ifdef HYSCAN_INSTALLED

    /* Если программа установлена в системный каталог дистрибутива,
     * лог файлы хранятся в домашнем каталоге. */

    hyscan_config_log = g_build_filename (g_get_user_data_dir (),
                                          "hyscan"HYSCAN_MAJOR_VERSION,
                                          NULL);

  #else

    /* В portable версии лог файлы хранятся в каталоге ${hyscan_config_prefix}/log. */

    hyscan_config_log = g_build_filename (hyscan_config_prefix,
                                          "log",
                                          NULL);

  #endif

  atexit (hyscan_config_clear);
}

/* Очистка. */
static void
hyscan_config_clear (void)
{
  g_free (hyscan_config_prefix);
  g_free (hyscan_config_locale);
  g_free (hyscan_config_driver);
  g_free (hyscan_config_system);
  g_free (hyscan_config_user);
  g_free (hyscan_config_log);
}

/**
 * hyscan_config_get_locale_dir:
 *
 * Функция возвращает путь к файлам с переводами. Возвращаемый путь представлен
 * в системной кодировке, как того требует функция bindtextdomain.
 *
 * Сами файлы с переводами должны размещаться про этому пути в соответствии
 * с требованиями системы Gettext.
 *
 * Returns: (transfer none): Путь к файлам с переводами.
 */
const gchar *
hyscan_config_get_locale_dir (void)
{
  return hyscan_config_locale;
}

/**
 * hyscan_config_get_driver_dir:
 *
 * Функция возвращает путь к драйверам устройств. В Windows путь представлен в
 * кодировке UTF-8, в Unix в системной кодировке.
 *
 * Returns: (transfer none): Путь к драйверам устройств.
 */
const gchar *
hyscan_config_get_driver_dir (void)
{
  return hyscan_config_driver;
}

/**
 * hyscan_config_get_system_dir:
 *
 * Функция возвращает путь к общесистемным файлам. В Windows путь представлен в
 * кодировке UTF-8, в Unix в системной кодировке.
 *
 * Returns: (transfer none): Путь к общесистемным файлам.
 */
const gchar *
hyscan_config_get_system_dir (void)
{
  return hyscan_config_system;
}

/**
 * hyscan_config_get_system_dir:
 *
 * Функция возвращает путь к пользовательским файлам. В Windows путь представлен в
 * кодировке UTF-8, в Unix в системной кодировке.
 *
 * Returns: (transfer none): Путь к пользовательским файлам.
 */
const gchar *
hyscan_config_get_user_dir (void)
{
  return hyscan_config_user;
}

/**
 * hyscan_config_get_log_dir:
 *
 * Функция возвращает путь к лог файлам. В Windows путь представлен в
 * кодировке UTF-8, в Unix в системной кодировке.
 *
 * Returns: (transfer none): Путь к лог файлам.
 */
const gchar *
hyscan_config_get_log_dir (void)
{
  return hyscan_config_log;
}
