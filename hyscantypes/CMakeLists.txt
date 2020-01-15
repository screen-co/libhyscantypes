
find_program (MARSHAL_COMPILER glib-genmarshal)
if (${MARSHAL_COMPILER} STREQUAL MARSHAL_COMPILER-NOTFOUND)
  message (FATAL_ERROR "GLib marshallers compiler not found.")
endif ()

file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/marshallers")
include_directories ("${CMAKE_BINARY_DIR}/marshallers")

add_custom_command (OUTPUT "${CMAKE_BINARY_DIR}/marshallers/hyscan-types-marshallers.c"
                    COMMAND ${MARSHAL_COMPILER} --header
                            --prefix hyscan_types_marshal
                            hyscan-types-marshallers.list >
                            "${CMAKE_BINARY_DIR}/marshallers/hyscan-types-marshallers.h"
                    COMMAND ${MARSHAL_COMPILER} --body
                            --prefix hyscan_types_marshal
                            hyscan-types-marshallers.list >
                            "${CMAKE_BINARY_DIR}/marshallers/hyscan-types-marshallers.c"
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    DEPENDS hyscan-types-marshallers.list
                    VERBATIM)

add_library (${HYSCAN_TYPES_LIBRARY} SHARED
             hyscan-slice-pool.c
             hyscan-types.c
             hyscan-buffer.c
             hyscan-data-schema.c
             hyscan-data-schema-builder.c
             hyscan-data-schema-internal.c
             hyscan-param-list.c
             hyscan-param.c
             hyscan-param-proxy.c
             hyscan-param-controller.c
             hyscan-data-box.c
             hyscan-cancellable.c
             "${CMAKE_BINARY_DIR}/marshallers/hyscan-types-marshallers.c")

target_link_libraries (${HYSCAN_TYPES_LIBRARY} ${GLIB2_LIBRARIES} ${LIBXML2_LIBRARIES})

set_target_properties (${HYSCAN_TYPES_LIBRARY} PROPERTIES DEFINE_SYMBOL "HYSCAN_API_EXPORTS")
set_target_properties (${HYSCAN_TYPES_LIBRARY} PROPERTIES SOVERSION ${HYSCAN_TYPES_VERSION})

configure_file (hyscantypes.pc.in
                "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/hyscantypes-${HYSCAN_MAJOR_VERSION}.pc"
                @ONLY)

install (TARGETS ${HYSCAN_TYPES_LIBRARY}
         COMPONENT runtime
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

install (FILES "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/hyscantypes-${HYSCAN_MAJOR_VERSION}.pc"
         COMPONENT development
         DESTINATION "lib/pkgconfig"
         PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

install (FILES hyscan-api.h
               hyscan-slice-pool.h
               hyscan-types.h
               hyscan-buffer.h
               hyscan-data-schema.h
               hyscan-data-schema-builder.h
               hyscan-param-list.h
               hyscan-param.h
               hyscan-param-proxy.h
               hyscan-param-controller.h
               hyscan-data-box.h
               hyscan-cancellable.h
         COMPONENT development
         DESTINATION "include/hyscan-${HYSCAN_MAJOR_VERSION}/hyscantypes"
         PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
