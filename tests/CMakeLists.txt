
set (TEST_LIBRARIES ${GLIB2_LIBRARIES}
                    ${LIBXML2_LIBRARIES}
                    ${HYSCAN_TYPES_LIBRARY})

add_executable (param-list-test param-list-test.c)
add_executable (data-box-test data-box-test.c data-schema-create.c)
add_executable (data-schema-test data-schema-test.c data-schema-create.c)
add_executable (slice-pool-test slice-pool-test.c)
add_executable (data-schema-list data-schema-list.c)

target_link_libraries (param-list-test ${TEST_LIBRARIES})
target_link_libraries (data-box-test ${TEST_LIBRARIES})
target_link_libraries (data-schema-test ${TEST_LIBRARIES})
target_link_libraries (slice-pool-test ${TEST_LIBRARIES})
target_link_libraries (data-schema-list ${TEST_LIBRARIES})

install (TARGETS data-box-test
                 data-schema-test
                 slice-pool-test
                 data-schema-list
         COMPONENT test
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
