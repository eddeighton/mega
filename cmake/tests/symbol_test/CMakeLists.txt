

cmake_minimum_required(VERSION 3.2)

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get gtest
include( ${WORKSPACE_ROOT_PATH}/thirdparty/gtest/gtest_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

include_directories( ${MEGA_API_DIR} )
include_directories( ${MEGA_SRC_DIR} )

set( SYMBOL_TEST_DIR ${MEGA_TEST_DIR}/symbol_test )

add_executable( symbol_test ${SYMBOL_TEST_DIR}/main.cpp )

# allow access to source folder for tests
target_include_directories(symbol_test PUBLIC ${MEGA_SRC_DIR})

set_target_properties( symbol_test PROPERTIES FOLDER mega )

link_boost( symbol_test filesystem )
link_boost( symbol_test system )
link_boost( symbol_test date_time )
link_boost( symbol_test program_options )
link_boost( symbol_test iostreams )
link_boost( symbol_test atomic )
#link_gtest( symbol_test )

link_common( symbol_test )

#target_link_libraries( symbol_test database )
#target_link_libraries( symbol_test utilities )
#target_link_libraries( symbol_test database_compiler)
#target_link_libraries( symbol_test protocol)
#target_link_libraries( symbol_test network)
#target_link_libraries( symbol_test pipeline)

install( TARGETS symbol_test DESTINATION bin )