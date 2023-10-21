##  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
##  Author: Edward Deighton
##  License: Please see license.txt in the project root folder.

##  Use and copying of this software and preparation of derivative works
##  based upon this software are permitted. Any copy of this software or
##  of any derivative work must include the above copyright notice, this
##  paragraph and the one after it.  Any distribution of this software or
##  derivative works must comply with all applicable laws.

##  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
##  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
##  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
##  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
##  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
##  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
##  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
##  OF THE POSSIBILITY OF SUCH DAMAGES.



cmake_minimum_required(VERSION 3.2)

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get gtest
include( ${WORKSPACE_ROOT_PATH}/thirdparty/gtest/gtest_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

include_directories( ${MEGA_API_DIR} )
include_directories( ${MEGA_SRC_DIR} )

set( MEGA_UNIT_TESTS_DIR ${MEGA_TEST_DIR}/unit_tests )

set( COMPILER_SRC
	${MEGA_SRC_DIR}/compiler/automata.cpp )

set( MEGA_UNIT_TESTS
	#${MEGA_UNIT_TESTS_DIR}/scheduler_tests.cpp
	${MEGA_UNIT_TESTS_DIR}/arguments_tests.cpp
	${MEGA_UNIT_TESTS_DIR}/asio_tests.cpp
	${MEGA_UNIT_TESTS_DIR}/automata_tests.cpp
	${MEGA_UNIT_TESTS_DIR}/compiler_pipeline_tests.cpp
	${MEGA_UNIT_TESTS_DIR}/database_tests.cpp 
	${MEGA_UNIT_TESTS_DIR}/glob_tests.cpp
	${MEGA_UNIT_TESTS_DIR}/io_tests.cpp
	${MEGA_UNIT_TESTS_DIR}/log_tests.cpp
	${MEGA_UNIT_TESTS_DIR}/manifest_tests.cpp
	${MEGA_UNIT_TESTS_DIR}/network_tests.cpp
	${MEGA_UNIT_TESTS_DIR}/pipeline_tests.cpp
	${MEGA_UNIT_TESTS_DIR}/protocol_tests.cpp
	${MEGA_UNIT_TESTS_DIR}/schema_grammar_tests.cpp
	${MEGA_UNIT_TESTS_DIR}/schematic_tests.cpp
	${MEGA_UNIT_TESTS_DIR}/sim_state_machine_tests.cpp
	${MEGA_UNIT_TESTS_DIR}/visitor_tests.cpp
	${MEGA_UNIT_TESTS_DIR}/xml_tag_parser_tests.cpp
	)

set( MEGA_UNIT_TESTS_DATA
		${MEGA_UNIT_TESTS_DIR}/test_data/xml_tags_data.xml
)

enable_testing()

add_executable( mega_tests ${MEGA_UNIT_TESTS} ${COMPILER_SRC} ${MEGA_UNIT_TESTS_DIR}/main_test_runner.cpp )

IF( ${WIN32} )
target_compile_options( mega_tests PUBLIC "/await:strict" )
target_compile_options( mega_tests PRIVATE "/bigobj" )
ELSE( ${WIN32} )
target_compile_options( mega_tests PUBLIC "-fcoroutines" )
ENDIF( ${WIN32} )

target_compile_definitions( mega_tests PUBLIC -DBOOST_PHOENIX_STL_TUPLE_H_ )

# allow access to source folder for tests
target_include_directories(mega_tests PUBLIC ${MEGA_SRC_DIR})

set_target_properties( mega_tests PROPERTIES FOLDER tests/unit )

link_boost( mega_tests filesystem )
link_boost( mega_tests system )
link_boost( mega_tests date_time )
link_boost( mega_tests program_options )
link_boost( mega_tests iostreams )
link_boost( mega_tests atomic )
link_boost( mega_tests serialization )
link_gtest( mega_tests )

target_link_libraries( mega_tests database )
target_link_libraries( mega_tests database_compiler)
target_link_libraries( mega_tests protocol)
target_link_libraries( mega_tests network)
target_link_libraries( mega_tests pipeline)
target_link_libraries( mega_tests executorLib)
target_link_libraries( mega_tests log)
target_link_libraries( mega_tests mega_mangle)
target_link_libraries( mega_tests schematic)

gtest_add_tests( TARGET mega_tests SOURCES ${MEGA_UNIT_TESTS} )

install( TARGETS mega_tests DESTINATION bin )
install( FILES ${MEGA_UNIT_TESTS_DATA} DESTINATION bin/test_data )	

add_custom_command( TARGET mega_tests POST_BUILD COMMAND mega_tests WORKING_DIRECTORY ${MEGA_UNIT_TESTS_DIR} )