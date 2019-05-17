
set (TEST_LIBRARIES ${GLIB2_LIBRARIES}
                    ${LIBXML2_LIBRARIES}
                    ${HYSCAN_TYPES_LIBRARY})

add_executable (slice-pool-test slice-pool-test.c)
add_executable (channel-name-test channel-name-test.c)
add_executable (buffer-test buffer-test.c)
add_executable (data-schema-test data-schema-test.c data-schema-create.c)
add_executable (param-list-test param-list-test.c)
add_executable (param-test param-test.c data-schema-create.c)
add_executable (data-schema-list data-schema-list.c)

target_link_libraries (slice-pool-test ${TEST_LIBRARIES})
target_link_libraries (channel-name-test ${TEST_LIBRARIES})
target_link_libraries (buffer-test ${TEST_LIBRARIES})
target_link_libraries (data-schema-test ${TEST_LIBRARIES})
target_link_libraries (param-list-test ${TEST_LIBRARIES})
target_link_libraries (param-test ${TEST_LIBRARIES})
target_link_libraries (data-schema-list ${TEST_LIBRARIES})

add_test (NAME SlicePoolTest COMMAND slice-pool-test
          WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
add_test (NAME ChannelNameTest COMMAND channel-name-test
          WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
add_test (NAME BufferTest COMMAND buffer-test
          WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
add_test (NAME DataSchemaTest COMMAND data-schema-test
          WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
add_test (NAME ParamListTest COMMAND param-list-test
          WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
add_test (NAME ParamTest:data-box COMMAND param-test -p data-box
          WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
add_test (NAME ParamTest:controller COMMAND param-test -p controller
          WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

install (TARGETS slice-pool-test
                 channel-name-test
                 buffer-test
                 data-schema-test
                 param-list-test
                 param-test
                 data-schema-list
         COMPONENT test
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
