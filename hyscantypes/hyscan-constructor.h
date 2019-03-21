/* hyscan-constructor.h
 *
 * Copyright 2019 Screen LLC, Andrei Fadeev <andrei@webcontrol.ru>
 * Copyright 2019 Screen LLC, Alexander Dmitriev <m1n7@yandex.ru>
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
 * This file is based on gconstructor.h from GLIB project. 
 */

#ifdef __HYSCAN_CONSTRUCTOR_H__
  #error "<hyscan-constructor.h> already included."
  #pragma message ("It may mean that you have included it in header file.")
  #pragma message ("This is forbidden.")
#endif /* __HYSCAN_CONSTRUCTOR_H__ */

#define __HYSCAN_CONSTRUCTOR_H__

#if  __GNUC__ > 2
  #define HYSCAN_CONSTRUCTOR(_func) static void __attribute__((constructor)) _func (void);

#elif _MSC_VER >= 1500
  #ifdef _WIN64
    #define HYSCAN_MSVC_SYMBOL_PREFIX ""
  #else
    #define HYSCAN_MSVC_SYMBOL_PREFIX "_"
  #endif /* _WIN64 */

  #define HYSCAN_CONSTRUCTOR(_func) HYSCAN_MSVC_CTOR (_func, HYSCAN_MSVC_SYMBOL_PREFIX)

  #define HYSCAN_MSVC_CTOR(_func,_sym_prefix)                                  \
    static void _func(void);                                                   \
    extern int (* _array ## _func)(void);                                      \
    int _func ## _wrapper(void)                                                \
      {                                                                        \
        _func();                                                               \
        g_slist_find (NULL, _array ## _func);                                  \
        return 0;                                                              \
      }                                                                        \
    __pragma(comment(linker,"/include:" _sym_prefix # _func "_wrapper"))       \
    __pragma(section(".CRT$XCU",read))                                         \
    __declspec(allocate(".CRT$XCU")) int (* _array ## _func)(void) = _func ## _wrapper;

#else
  #error "Your compiler does not support constructors"

#endif
