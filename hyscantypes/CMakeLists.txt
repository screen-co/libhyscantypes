
add_library (hyscantypes SHARED
             hyscan-types.c
             hyscan-data-schema.c)

target_link_libraries (hyscantypes ${GLIB2_LIBRARIES} ${LIBXML2_LIBRARIES})
