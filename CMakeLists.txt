cmake_minimum_required (VERSION 2.6)

project (libhyscantypes)

include (FindPkgConfig)

#
# Default build type.
if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE "Release")
endif ()

#
# Compilers options.
if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
  set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -Wall $ENV{CFLAGS}")
  set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Og -Wall $ENV{CFLAGS}")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
  set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /wd4244 /wd4305 $ENV{CFLAGS}")
  set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /wd4244 /wd4305 $ENV{CFLAGS}")
endif ()

#
# Output directories.
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

#
# System dependencies.
pkg_check_modules (GLIB2 REQUIRED "glib-2.0 gobject-2.0 gthread-2.0 gio-2.0")
pkg_check_modules (LIBXML2 REQUIRED "libxml-2.0")
add_definitions (${GLIB2_CFLAGS} ${LIBXML2_CFLAGS})
link_directories (${GLIB2_LIBRARY_DIRS} ${LIBXML2_LIBRARY_DIRS})

#
# Include directories.
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/hyscantypes")

#
# Target.
add_definitions (-DG_LOG_DOMAIN="HyScanTypes")
add_subdirectory ("hyscantypes")
add_subdirectory ("tests")
