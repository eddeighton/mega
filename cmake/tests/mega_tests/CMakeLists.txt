cmake_minimum_required(VERSION 3.2)

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get gtest
include( ${WORKSPACE_ROOT_PATH}/thirdparty/gtest/gtest_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

include_directories( ${MEGA_API_DIR} )
include_directories( ${MEGA_SRC_DIR} )

set( MEGA_UNIT_TESTS
	${MEGA_TEST_DIR}/database_tests.cpp 
	${MEGA_TEST_DIR}/schema_grammar_tests.cpp
	${MEGA_TEST_DIR}/io_tests.cpp
	${MEGA_TEST_DIR}/glob_tests.cpp
	${MEGA_TEST_DIR}/manifest_tests.cpp)

enable_testing()

add_executable( mega_tests ${MEGA_UNIT_TESTS} ${MEGA_TEST_DIR}/main_test_runner.cpp )

# allow access to source folder for tests
target_include_directories(mega_tests PUBLIC ${MEGA_SRC_DIR})

set_target_properties( mega_tests PROPERTIES FOLDER mega )

link_boost( mega_tests filesystem )
link_boost( mega_tests system )
link_boost( mega_tests date_time )
link_boost( mega_tests program_options )
link_boost( mega_tests iostreams )
link_boost( mega_tests atomic )
link_gtest( mega_tests )

target_link_libraries( mega_tests database )
target_link_libraries( mega_tests utilities )
target_link_libraries( mega_tests parser )
target_link_libraries( mega_tests database_compiler)

gtest_add_tests( TARGET mega_tests SOURCES ${MEGA_UNIT_TESTS} )

install( TARGETS mega_tests DESTINATION bin )
#install( DIRECTORY ${MEGA_TEST_DIR}/data/ DESTINATION tests)

add_custom_command( TARGET mega_tests POST_BUILD COMMAND ctest --output-on-failure )