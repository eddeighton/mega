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

add_executable( orc_tests ${ORC_UNIT_TESTS} ${MEGA_TEST_DIR}/unit_tests/main_test_runner.cpp )

IF( ${WIN32} )
set_property(TARGET orc_tests PROPERTY ENABLE_EXPORTS ON )
set_property(TARGET orc_tests PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON )
ELSE( ${WIN32} )
# target_link_options( orc_tests PUBLIC -export-dynamic )
ENDIF( ${WIN32} )

# allow access to source folder for tests
target_include_directories(orc_tests PUBLIC ${MEGA_SRC_DIR})

set_target_properties( orc_tests PROPERTIES FOLDER tests/orc )

link_boost( orc_tests filesystem )
link_boost( orc_tests system )
link_boost( orc_tests date_time )
link_boost( orc_tests program_options )
link_boost( orc_tests iostreams )
link_boost( orc_tests serialization )
link_gtest( orc_tests )
link_orc( orc_tests )
link_common( orc_tests )

target_link_libraries( orc_tests database )

gtest_add_tests( TARGET orc_tests SOURCES ${ORC_UNIT_TESTS} )

install( TARGETS orc_tests DESTINATION bin )
#install( DIRECTORY ${MEGA_TEST_DIR}/data/ DESTINATION tests)

add_custom_command( TARGET orc_tests POST_BUILD COMMAND ctest --output-on-failure -C $<CONFIG> )