
#
# Searching for Glib resource compiler.
find_program (RESOURCE_COMPILER glib-compile-resources)
if (${RESOURCE_COMPILER} STREQUAL RESOURCE_COMPILER-NOTFOUND)
  message (FATAL_ERROR "GLib resource compiler not found.")
endif ()

#
# Generate resource C file.
file (MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/resources)
execute_process (COMMAND ${RESOURCE_COMPILER}
                         --generate-source
                         --target ${CMAKE_BINARY_DIR}/resources/data-schema-resources.c
                         data-schema-resources.xml
                         WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable (data-schema-test data-schema-test.c ${CMAKE_BINARY_DIR}/resources/data-schema-resources.c)
add_executable (data-schema-list data-schema-list.c)
add_executable (data-box-test data-box-test.c ${CMAKE_BINARY_DIR}/resources/data-schema-resources.c)

target_link_libraries (data-schema-test ${GLIB2_LIBRARIES} hyscantypes)
target_link_libraries (data-schema-list ${GLIB2_LIBRARIES} hyscantypes)
target_link_libraries (data-box-test ${GLIB2_LIBRARIES} hyscantypes)
