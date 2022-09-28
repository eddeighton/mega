
#  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
#  Author: Edward Deighton
#  License: Please see license.txt in the project root folder.
#
#  Use and copying of this software and preparation of derivative works
#  based upon this software are permitted. Any copy of this software or
#  of any derivative work must include the above copyright notice, this
#  paragraph and the one after it.  Any distribution of this software or
#  derivative works must comply with all applicable laws.
#
#  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
#  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
#  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
#  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
#  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
#  OF THE POSSIBILITY OF SUCH DAMAGES.

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
	${MEGA_TEST_DIR}/manifest_tests.cpp
	${MEGA_TEST_DIR}/protocol_tests.cpp
	${MEGA_TEST_DIR}/network_tests.cpp
	${MEGA_TEST_DIR}/pipeline_tests.cpp
	${MEGA_TEST_DIR}/compiler_pipeline_tests.cpp
	${MEGA_TEST_DIR}/asio_tests.cpp
	${MEGA_TEST_DIR}/allocator_tests.cpp
	${MEGA_TEST_DIR}/sim_state_machine_tests.cpp
	${MEGA_TEST_DIR}/reference_tests.cpp
	)

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
target_link_libraries( mega_tests database_compiler)
target_link_libraries( mega_tests protocol)
target_link_libraries( mega_tests network)
target_link_libraries( mega_tests pipeline)

gtest_add_tests( TARGET mega_tests SOURCES ${MEGA_UNIT_TESTS} )

install( TARGETS mega_tests DESTINATION bin )
#install( DIRECTORY ${MEGA_TEST_DIR}/data/ DESTINATION tests)

add_custom_command( TARGET mega_tests POST_BUILD COMMAND ctest --output-on-failure )