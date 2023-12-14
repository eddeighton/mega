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

set( IL_TESTS_DIR ${MEGA_TEST_DIR}/il_tests )

set( IL_UNIT_TESTS
	# ${IL_TESTS_DIR}/cpp_tests.cpp
	${IL_TESTS_DIR}/ir_tests.cpp

	)

enable_testing()

add_executable( il_tests ${IL_UNIT_TESTS} ${IL_TESTS_DIR}/main_test_runner.cpp )


target_link_libraries( il_tests backend )

#IF( ${WIN32} )
#target_compile_options( il_tests PRIVATE "/bigobj" )
#ENDIF( ${WIN32} )

# allow access to source folder for tests
target_include_directories(il_tests PUBLIC ${MEGA_SRC_DIR})

set_target_properties( il_tests PROPERTIES FOLDER tests/unit )

link_boost( il_tests filesystem )
link_boost( il_tests system )
link_boost( il_tests date_time )
link_boost( il_tests program_options )
link_boost( il_tests iostreams )
link_boost( il_tests atomic )
link_boost( il_tests serialization )
link_gtest( il_tests )
link_json( il_tests )
link_common( il_tests )

gtest_add_tests( TARGET il_tests SOURCES ${IL_UNIT_TESTS} )

install( TARGETS il_tests DESTINATION bin )

add_custom_command( TARGET il_tests POST_BUILD COMMAND il_tests WORKING_DIRECTORY ${VALUE_TESTS_DIR} )