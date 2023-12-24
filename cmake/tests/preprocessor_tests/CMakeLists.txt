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

set( PREPROCESSOR_TESTS_DIR ${MEGA_TEST_DIR}/preprocessor_tests )

set( PREPROCESSOR_UNIT_TESTS
	${MEGA_TEST_DIR}/preprocessor_tests/basic.cpp 
	
	)

enable_testing()

add_executable( preprocessor_tests ${PREPROCESSOR_UNIT_TESTS} ${MEGA_TEST_DIR}/preprocessor_tests/main_test_runner.cpp )


# allow access to source folder for tests
target_include_directories(preprocessor_tests PUBLIC ${MEGA_SRC_DIR})

set_target_properties( preprocessor_tests PROPERTIES FOLDER tests/preprocessor )

target_link_libraries( preprocessor_tests preprocessor )

link_boost( preprocessor_tests filesystem )
link_boost( preprocessor_tests system )
link_boost( preprocessor_tests date_time )
link_boost( preprocessor_tests program_options )
link_boost( preprocessor_tests iostreams )
link_boost( preprocessor_tests serialization )
link_boost( preprocessor_tests wave )
link_gtest( preprocessor_tests )
link_common( preprocessor_tests )

gtest_add_tests( TARGET preprocessor_tests SOURCES ${PREPROCESSOR_UNIT_TESTS} )

install( TARGETS preprocessor_tests DESTINATION bin )

#add_custom_command( TARGET preprocessor_tests POST_BUILD COMMAND ctest --output-on-failure -C $<CONFIG> )

include( ${MEGA_ROOT_DIR}/../mega_flags.cmake )

add_custom_target( run_preprocessor_tests 

	COMMAND $<TARGET_FILE_NAME:preprocessor_tests> 
        --input ${PREPROCESSOR_TESTS_DIR}/input
        --result ${PREPROCESSOR_TESTS_DIR}/result
        --expected ${PREPROCESSOR_TESTS_DIR}/expected

		--cpp_flags \"${CLANG_CXX_FLAGS_STR}\"
		--cpp_defines \"${CLANG_CXX_DEFINES_STR}\"
		--include_dirs \"${CMAKE_INSTALL_PREFIX}/../../boost/install/include\;${CMAKE_INSTALL_PREFIX}/../../common/install/include\;${MEGA_API_DIR}\"

		--stash_dir ${CMAKE_CURRENT_BINARY_DIR}/stash

		--cout

	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)

add_dependencies( run_compiler_tests compiler_tests )