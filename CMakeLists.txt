
cmake_minimum_required (VERSION 2.8)

project (libhyscantypes)

enable_testing()

set (HYSCAN_MAJOR_VERSION 5)
set (HYSCAN_TYPES_VERSION 0)
set (HYSCAN_TYPES_LIBRARY "hyscantypes-${HYSCAN_MAJOR_VERSION}")
# set (HYSCAN_INSTALLED 1 )

include (FindPkgConfig)

if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Release)
endif ()

if (${CMAKE_C_COMPILER_ID} STREQUAL GNU)
  set (DEBUG_OPT_LEVEL "-Og")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL Clang)
  set (DEBUG_OPT_LEVEL "-O1")
endif ()

if (${CMAKE_C_COMPILER_ID} STREQUAL GNU OR ${CMAKE_C_COMPILER_ID} STREQUAL Clang)
  set (C_FLAGS "-Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers $ENV{CFLAGS}")
  set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 ${C_FLAGS}")
  set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${DEBUG_OPT_LEVEL} ${C_FLAGS}")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL MSVC)
  set (C_FLAGS "/wd4244 $ENV{CFLAGS}")
  set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${C_FLAGS}")
  set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${C_FLAGS}")
else ()
  message (FATAL_ERROR "Unsupported compiler ${CMAKE_C_COMPILER_ID}")
endif ()

if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
  set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin")
  set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin")
endif ()

if (NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
  set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin")
  set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin")
endif ()

if (NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
  set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin")
  set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin")
endif ()

pkg_check_modules (GLIB2 REQUIRED glib-2.0 gobject-2.0 gthread-2.0 gio-2.0)
pkg_check_modules (LIBXML2 REQUIRED libxml-2.0)
link_directories (${GLIB2_LIBRARY_DIRS} ${LIBXML2_LIBRARY_DIRS})
add_definitions (${GLIB2_CFLAGS} ${LIBXML2_CFLAGS})

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/hyscantypes")

# Папка с локализацией.
add_definitions(-DHYSCAN_LOCALE_PREFIX="share")
add_definitions(-DHYSCAN_LOCALE_POSTFIX="locale")

# Папка с профилями.
if (NOT DEFINED HYSCAN_USER_FILES_DIR)
  message ("Profile path not defined. Using \"hyscan\"")
  set (HYSCAN_USER_FILES_DIR "hyscan")
endif()

message ("Profile path:" ${HYSCAN_USER_FILES_DIR})
add_definitions(-DHYSCAN_USER_FILES_DIR="${HYSCAN_USER_FILES_DIR}")

# Пути в portable и standalone сборках
if (DEFINED HYSCAN_INSTALLED)
  add_definitions(-DHYSCAN_INSTALLED=1)
  add_definitions (-DHYSCAN_USER_FILES_PREFIX="share")
  message ("HyScan will be standalone")
else()
  add_definitions (-DHYSCAN_USER_FILES_PREFIX="../config")
  message ("HyScan will be portable")
endif()


add_definitions (-DG_LOG_DOMAIN="HyScanTypes")
add_definitions (-DGETTEXT_PACKAGE="hyscantypes")
add_subdirectory (hyscantypes)
add_subdirectory (tests)
