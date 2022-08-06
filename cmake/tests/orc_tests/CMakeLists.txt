cmake_minimum_required(VERSION 3.2)

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get gtest
include( ${WORKSPACE_ROOT_PATH}/thirdparty/gtest/gtest_include.cmake )

#get clang
include( ${WORKSPACE_ROOT_PATH}/thirdparty/llvm/clang_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

include_directories( ${MEGA_API_DIR} )
include_directories( ${MEGA_SRC_DIR} )

set( ORC_UNIT_TESTS
	${MEGA_TEST_DIR}/orc/orc_tests.cpp 
	
	)

enable_testing()

add_executable( orc_tests ${ORC_UNIT_TESTS} ${MEGA_TEST_DIR}/main_test_runner.cpp )


#set_property(TARGET orc_tests PROPERTY CMAKE_ENABLE_EXPORTS ON)
target_link_options( orc_tests PUBLIC -export-dynamic )

# allow access to source folder for tests
target_include_directories(orc_tests PUBLIC ${MEGA_SRC_DIR})

set_target_properties( orc_tests PROPERTIES FOLDER mega )

link_boost( orc_tests filesystem )
link_boost( orc_tests system )
link_boost( orc_tests date_time )
link_boost( orc_tests program_options )
link_boost( orc_tests iostreams )
link_boost( orc_tests atomic )
link_gtest( orc_tests )
link_orc( orc_tests )
link_common( orc_tests )

target_link_libraries( orc_tests database )
target_link_libraries( orc_tests utilities )
target_link_libraries( orc_tests database_compiler)

gtest_add_tests( TARGET orc_tests SOURCES ${ORC_UNIT_TESTS} )

install( TARGETS orc_tests DESTINATION bin )
#install( DIRECTORY ${MEGA_TEST_DIR}/data/ DESTINATION tests)

add_custom_command( TARGET orc_tests POST_BUILD COMMAND ctest --output-on-failure )