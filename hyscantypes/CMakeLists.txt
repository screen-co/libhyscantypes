
add_library (hyscantypes SHARED
             hyscan-data.c
             hyscan-data-schema.c
             hyscan-data-schema-builder.c
             hyscan-data-schema-internal.c
             hyscan-data-box.c)

target_link_libraries (hyscantypes ${GLIB2_LIBRARIES} ${LIBXML2_LIBRARIES})
