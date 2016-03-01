
add_executable (data-schema-test data-schema-test.c)

target_link_libraries (data-schema-test ${GLIB2_LIBRARIES} hyscantypes)
