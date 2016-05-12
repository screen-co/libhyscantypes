
add_library (hyscantypes SHARED
             hyscan-data.c
             hyscan-data-schema.c
             hyscan-data-schema-overrides.c
             hyscan-data-box.c)

target_link_libraries (hyscantypes ${GLIB2_LIBRARIES} ${LIBXML2_LIBRARIES})
