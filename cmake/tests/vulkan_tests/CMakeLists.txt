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

#get vulkan
include( ${WORKSPACE_ROOT_PATH}/thirdparty/khronosgroup/vulkan_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

include_directories( ${MEGA_API_DIR} )
include_directories( ${MEGA_SRC_DIR} )

set( VULKAN_UNIT_TESTS
	${MEGA_TEST_DIR}/vulkan/vulkan_tests.cpp 
	
	)

enable_testing()

add_executable( vulkan_tests ${VULKAN_UNIT_TESTS} ${MEGA_TEST_DIR}/unit_tests/main_test_runner.cpp )

target_compile_definitions( vulkan_tests PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)

# allow access to source folder for tests
target_include_directories(vulkan_tests PUBLIC ${MEGA_SRC_DIR})

set_target_properties( vulkan_tests PROPERTIES FOLDER mega )

link_boost( vulkan_tests filesystem )
link_boost( vulkan_tests system )
link_boost( vulkan_tests date_time )
link_boost( vulkan_tests program_options )
link_boost( vulkan_tests iostreams )
link_boost( vulkan_tests atomic )
link_gtest( vulkan_tests )
link_vulkan( vulkan_tests )

target_link_libraries( vulkan_tests database )
target_link_libraries( vulkan_tests utilities )
target_link_libraries( vulkan_tests database_compiler)
target_link_libraries( vulkan_tests ssa)

gtest_add_tests( TARGET vulkan_tests SOURCES ${VULKAN_UNIT_TESTS} )

install( TARGETS vulkan_tests DESTINATION bin )
#install( DIRECTORY ${MEGA_TEST_DIR}/data/ DESTINATION tests)

add_custom_command( TARGET vulkan_tests POST_BUILD COMMAND ctest --output-on-failure )