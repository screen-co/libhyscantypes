
add_executable (data-schema-test data-schema-test.c)
add_executable (data-box-test data-box-test.c)

target_link_libraries (data-schema-test ${GLIB2_LIBRARIES} hyscantypes)
target_link_libraries (data-box-test ${GLIB2_LIBRARIES} hyscantypes)
