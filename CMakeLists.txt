
cmake_minimum_required( VERSION 2.6 )

project( libhyscantypes )

include( FindPkgConfig )

#
# Default build type.
if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE "Release" )
endif()

#
# Default CPU architecture.
if( NOT CPU_ARCH )
  set( CPU_ARCH "x86" )
endif()

#
# CPU architecture.
if( CPU_ARCH STREQUAL "x86" )
  add_definitions( -DCPU_ARCH_X86 )
else()
  message( FATAL_ERROR "${CPU_ARCH} is unsupported CPU architecture" )
endif()

#
# CPU bitness.
if( CMAKE_SIZEOF_VOID_P EQUAL 4 )
  set( CPU32 1 )
  add_definitions( -DCPU32 )
elseif( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  set( CPU64 1 )
  add_definitions( -DCPU64 )
else()
  MATH( EXPR CPU_BITNESS 8*${CMAKE_SIZEOF_VOID_P} )
  message( FATAL_ERROR "${CPU_BITNESS}bit is unsupported CPU bitness" )
endif()

#
# Compilers options.
if( ${CMAKE_C_COMPILER_ID} STREQUAL "GNU" )
  if( CPU_ARCH STREQUAL "x86" AND CPU32 )
    set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -march=pentium4 -mtune=generic -mfpmath=sse" )
    set( CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -march=pentium4 -mtune=generic -mfpmath=sse" )
  endif()
  set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -Wall" )
  set( CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Og -Wall" )
elseif( ${CMAKE_C_COMPILER_ID} STREQUAL "MSVC" )
  message( FATAL_ERROR "MSVC not yet supported" )
else()
  message( FATAL_ERROR "Unsupported compiler" )
endif()

#
# Output directories.
if( NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY )
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin" )
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin" )
  set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin" )
endif()

if( NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin" )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin" )
  set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin" )
endif()

#
# Systen dependencies.
pkg_check_modules( GLIB2 REQUIRED "glib-2.0 gobject-2.0 gthread-2.0" )
add_definitions( ${GLIB2_CFLAGS} )
link_directories( ${GLIB2_LIBRARY_DIRS} )

#
# Include directories.
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/hyscantypes" )

#
# Target.
add_definitions( -DG_LOG_DOMAIN="HyScanTypes" )
add_subdirectory( "hyscantypes" )
add_subdirectory( "tests" )
