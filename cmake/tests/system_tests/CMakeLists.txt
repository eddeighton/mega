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

set( SYSTEM_TESTS_DIR ${MEGA_TEST_DIR}/system_tests )

set( SYSTEM_TESTS

	)

enable_testing()

add_executable( system_tests ${SYSTEM_TESTS} ${SYSTEM_TESTS_DIR}/main.cpp )

# allow access to source folder for tests
target_include_directories(system_tests PUBLIC ${MEGA_SRC_DIR})

set_target_properties( system_tests PROPERTIES FOLDER tests/system )

link_boost( system_tests filesystem )
link_boost( system_tests system )
link_boost( system_tests date_time )
link_boost( system_tests program_options )
link_boost( system_tests iostreams )
link_boost( system_tests atomic )
link_boost( system_tests serialization )
link_gtest( system_tests )

target_link_libraries( system_tests database )
target_link_libraries( system_tests utilities )
target_link_libraries( system_tests database_compiler)
target_link_libraries( system_tests protocol)
target_link_libraries( system_tests network)
target_link_libraries( system_tests pipeline)
target_link_libraries( system_tests terminal )
target_link_libraries( system_tests log )

gtest_add_tests( TARGET system_tests SOURCES ${MEGA_UNIT_TESTS} )

install( TARGETS system_tests DESTINATION bin )

add_custom_command( TARGET system_tests POST_BUILD COMMAND ctest --output-on-failure -C $<CONFIG> )