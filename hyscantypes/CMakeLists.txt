
add_library( hyscantypes SHARED
             hyscan-types.c )

target_link_libraries( hyscantypes ${GLIB2_LIBRARIES} )
