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

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

include_directories( ${MEGA_API_DIR} )
include_directories( ${MEGA_SRC_DIR} )

set( VALUE_TESTS_DIR ${MEGA_TEST_DIR}/value_tests )

set( VALUE_UNIT_TESTS
	${VALUE_TESTS_DIR}/arguments_tests.cpp
	${VALUE_TESTS_DIR}/bitset_tests.cpp
	${VALUE_TESTS_DIR}/component_info_tests.cpp
	${VALUE_TESTS_DIR}/reference_tests.cpp
	${VALUE_TESTS_DIR}/relation_id_tests.cpp
	${VALUE_TESTS_DIR}/report_value_tests.cpp
	${VALUE_TESTS_DIR}/sub_type_instance_tests.cpp
	${VALUE_TESTS_DIR}/type_id_tests.cpp

	)

enable_testing()

add_executable( value_tests ${VALUE_UNIT_TESTS} ${VALUE_TESTS_DIR}/main_test_runner.cpp )

target_link_libraries( value_tests mega_values )
target_link_libraries( value_tests reports )

IF( ${WIN32} )
target_compile_options( value_tests PUBLIC "/await:strict" )
target_compile_options( value_tests PRIVATE "/bigobj" )
ELSE( ${WIN32} )
target_compile_options( value_tests PUBLIC "-fcoroutines" )
ENDIF( ${WIN32} )

target_compile_definitions( value_tests PUBLIC -DBOOST_PHOENIX_STL_TUPLE_H_ )

# allow access to source folder for tests
target_include_directories(value_tests PUBLIC ${MEGA_SRC_DIR})

set_target_properties( value_tests PROPERTIES FOLDER tests/unit )

link_boost( value_tests filesystem )
link_boost( value_tests system )
link_boost( value_tests date_time )
link_boost( value_tests program_options )
link_boost( value_tests iostreams )
link_boost( value_tests atomic )
link_boost( value_tests serialization )
link_gtest( value_tests )
link_json( value_tests )
link_common( value_tests )

gtest_add_tests( TARGET value_tests SOURCES ${VALUE_UNIT_TESTS} )

install( TARGETS value_tests DESTINATION bin )

add_custom_command( TARGET value_tests POST_BUILD COMMAND value_tests WORKING_DIRECTORY ${VALUE_TESTS_DIR} )