
# compile test databases
set( MEGA_TEST_DIR ${MEGA_ROOT_DIR}/tests )

add_subdirectory(database)
add_subdirectory(mega_tests)
add_subdirectory(symbol_test)
add_subdirectory(invoke_proto)
add_subdirectory(vulkan_tests)
add_subdirectory(orc_tests)
