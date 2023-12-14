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

set( INVOCATION_TESTS_DIR ${MEGA_TEST_DIR}/invocation_tests )

set( INVOCATION_UNIT_TESTS
	${INVOCATION_TESTS_DIR}/basic_tests.cpp 
	${INVOCATION_TESTS_DIR}/get_tests.cpp 
	${INVOCATION_TESTS_DIR}/link_tests.cpp 
	)

enable_testing()

add_executable( invocation_tests ${INVOCATION_UNIT_TESTS} ${INVOCATION_TESTS_DIR}/main_test_runner.cpp )

#IF( ${WIN32} )
#target_compile_options( invocation_tests PRIVATE "/bigobj" )
#ENDIF( ${WIN32} )

# allow access to source folder for tests
target_include_directories(invocation_tests PUBLIC ${MEGA_SRC_DIR})

set_target_properties( invocation_tests PROPERTIES FOLDER tests/unit )

link_boost( invocation_tests filesystem )
link_boost( invocation_tests system )
link_boost( invocation_tests date_time )
link_boost( invocation_tests program_options )
link_boost( invocation_tests iostreams )
link_boost( invocation_tests atomic )
link_boost( invocation_tests serialization )
link_gtest( invocation_tests )

target_link_libraries( invocation_tests compiler_config )
target_link_libraries( invocation_tests database )
target_link_libraries( invocation_tests pipeline)
target_link_libraries( invocation_tests invocation)

gtest_add_tests( TARGET invocation_tests SOURCES ${INVOCATION_UNIT_TESTS} )

install( TARGETS invocation_tests DESTINATION bin )

message( "Generating the ToolChain.xml for invocation tests" )

find_path(LLVM_CONFIGURATION_PATH NAMES "llvm/install" PATHS "${CMAKE_INSTALL_PREFIX}/../../" REQUIRED NO_DEFAULT_PATH)

set( CLANG_PROGRAM_NAME )
IF( ${WIN32} )
set( CLANG_PROGRAM_NAME clang++.exe)
ELSE( ${WIN32} )
set( CLANG_PROGRAM_NAME clang++)
ENDIF( ${WIN32} )

find_file(SCRIPT_CLANG_COMPILER NAMES ${CLANG_PROGRAM_NAME} PATHS ${LLVM_CONFIGURATION_PATH}/llvm/install/bin REQUIRED NO_DEFAULT_PATH)

include( ${MEGA_ROOT_DIR}/../mega_flags.cmake )

# add_custom_command( TARGET invocation_tests POST_BUILD COMMAND ctest --output-on-failure -C $<CONFIG> )

add_custom_target( run_invocation_tests 

	COMMAND $<TARGET_FILE_NAME:invocation_tests> 
		--clang_compiler ${SCRIPT_CLANG_COMPILER}
		--parser $<TARGET_FILE:parser> 
		--mega_compiler $<TARGET_FILE:compiler>
		--mega_executor $<TARGET_FILE:executor>
		--clang_plugin $<TARGET_FILE:clang_plugin>
		--database $<TARGET_FILE:database>
		#--jit $<TARGET_FILE:jit>
		--mega_mangle $<TARGET_FILE:mega_mangle>
		--leaf $<TARGET_FILE:leaf>

		--cpp_flags \"${CLANG_CXX_FLAGS_STR}\"
		--cpp_defines \"${CLANG_CXX_DEFINES_STR}\"
		--include_dirs \"${CMAKE_INSTALL_PREFIX}/../../boost/install/include\;${CMAKE_INSTALL_PREFIX}/../../common/install/include\;${MEGA_API_DIR}\"

		--templates_dir ${MEGA_SRC_DIR}/compiler/templates
		--stash_dir ${CMAKE_CURRENT_BINARY_DIR}/stash

	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)

add_dependencies( run_invocation_tests invocation_tests )

#add_custom_command( TARGET invocation_tests POST_BUILD 
#	COMMAND $<TARGET_FILE_NAME:invocation_tests> 
#		--clang_compiler ${SCRIPT_CLANG_COMPILER}
#		--parser $<TARGET_FILE:parser> 
#		--mega_compiler $<TARGET_FILE:compiler>
#		--mega_executor $<TARGET_FILE:executor>
#		--clang_plugin $<TARGET_FILE:clang_plugin>
#		--database $<TARGET_FILE:database>
#		--jit $<TARGET_FILE:jit>
#		--mega_mangle $<TARGET_FILE:mega_mangle>
#		--leaf $<TARGET_FILE:leaf>
#
#		--cpp_flags \"${CLANG_CXX_FLAGS_STR}\"
#		--cpp_defines \"${CLANG_CXX_DEFINES_STR}\"
#		--include_dirs \"${CMAKE_INSTALL_PREFIX}/../../boost/install/include\;${CMAKE_INSTALL_PREFIX}/../../common/install/include\;${MEGA_API_DIR}\"
#
#		--templates_dir ${MEGA_SRC_DIR}/compiler/templates
#		--stash_dir ${CMAKE_CURRENT_BINARY_DIR}/stash
#	)
