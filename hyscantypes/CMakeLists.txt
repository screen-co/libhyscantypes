
find_program (MARSHAL_COMPILER glib-genmarshal)
if (${MARSHAL_COMPILER} STREQUAL MARSHAL_COMPILER-NOTFOUND)
  message (FATAL_ERROR "GLib marshallers compiler not found.")
endif ()

file (MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/marshallers)
include_directories (${CMAKE_BINARY_DIR}/marshallers)

add_custom_command (OUTPUT ${CMAKE_BINARY_DIR}/marshallers/hyscan-types-marshallers.c
                    COMMAND ${MARSHAL_COMPILER} --header
                            hyscan-types-marshallers.list >
                            ${CMAKE_BINARY_DIR}/marshallers/hyscan-types-marshallers.h
                    COMMAND ${MARSHAL_COMPILER} --body
                            hyscan-types-marshallers.list >
                            ${CMAKE_BINARY_DIR}/marshallers/hyscan-types-marshallers.c
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    DEPENDS hyscan-types-marshallers.list
                    VERBATIM)

add_library (hyscantypes SHARED
             hyscan-data.c
             hyscan-data-schema.c
             hyscan-data-schema-builder.c
             hyscan-data-schema-internal.c
             hyscan-data-box.c
             ${CMAKE_BINARY_DIR}/marshallers/hyscan-types-marshallers.c)

target_link_libraries (hyscantypes ${GLIB2_LIBRARIES} ${LIBXML2_LIBRARIES})

set_target_properties (hyscantypes PROPERTIES DEFINE_SYMBOL "HYSCAN_API_EXPORTS")
