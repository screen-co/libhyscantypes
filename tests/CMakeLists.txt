
add_executable (data-box-test data-box-test.c data-schema-create.c)
add_executable (data-schema-test data-schema-test.c data-schema-create.c)
add_executable (slice-pool-test slice-pool-test.c)
add_executable (data-schema-list data-schema-list.c)

target_link_libraries (data-box-test ${GLIB2_LIBRARIES} hyscantypes)
target_link_libraries (data-schema-test ${GLIB2_LIBRARIES} hyscantypes)
target_link_libraries (slice-pool-test ${GLIB2_LIBRARIES} hyscantypes)
target_link_libraries (data-schema-list ${GLIB2_LIBRARIES} hyscantypes)
