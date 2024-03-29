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

set( BASIC_UNIT_TESTS_DIR ${MEGA_TEST_DIR}/basic_tests )

set( BASIC_UNIT_TESTS
	${BASIC_UNIT_TESTS_DIR}/ring_allocator_tests.cpp
	${BASIC_UNIT_TESTS_DIR}/scheduler_tests.cpp
	${BASIC_UNIT_TESTS_DIR}/hashed_string.cpp
	)

enable_testing()

add_executable( basic_tests ${BASIC_UNIT_TESTS} ${BASIC_UNIT_TESTS_DIR}/main_test_runner.cpp )

#IF( ${WIN32} )
#target_compile_options( basic_tests PRIVATE "/bigobj" )
#ENDIF( ${WIN32} )

# allow access to source folder for tests
target_include_directories(basic_tests PUBLIC ${MEGA_SRC_DIR})

set_target_properties( basic_tests PROPERTIES FOLDER tests/unit )

link_boost( basic_tests filesystem )
link_boost( basic_tests system )
link_boost( basic_tests date_time )
link_boost( basic_tests program_options )
link_boost( basic_tests iostreams )
link_boost( basic_tests serialization )
link_gtest( basic_tests )
link_json( basic_tests )
link_common( basic_tests )

gtest_add_tests( TARGET basic_tests SOURCES ${BASIC_UNIT_TESTS} )

install( TARGETS basic_tests DESTINATION bin )

add_custom_command( TARGET basic_tests POST_BUILD COMMAND basic_tests WORKING_DIRECTORY ${BASIC_UNIT_TESTS_DIR} )