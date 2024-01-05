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

set( COMPILER_TESTS_DIR ${MEGA_TEST_DIR}/compiler_tests )

set( COMPILER_UNIT_TESTS
	${COMPILER_TESTS_DIR}/parser_tests.cpp 
	${COMPILER_TESTS_DIR}/ast_tests.cpp 
	${COMPILER_TESTS_DIR}/symbols_tests.cpp 
	${COMPILER_TESTS_DIR}/concrete_tests.cpp 
	${COMPILER_TESTS_DIR}/concrete_id_tests.cpp 
	${COMPILER_TESTS_DIR}/hyper_graph_tests.cpp 
	${COMPILER_TESTS_DIR}/automata_tests.cpp 
	${COMPILER_TESTS_DIR}/clang_traits_gen_tests.cpp 
	)

enable_testing()

add_executable( compiler_tests ${COMPILER_UNIT_TESTS} ${COMPILER_TESTS_DIR}/main_test_runner.cpp )

#IF( ${WIN32} )
#target_compile_options( compiler_tests PRIVATE "/bigobj" )
#ENDIF( ${WIN32} )

# allow access to source folder for tests
target_include_directories(compiler_tests PUBLIC ${MEGA_SRC_DIR})

set_target_properties( compiler_tests PROPERTIES FOLDER tests/unit )

link_boost( compiler_tests filesystem )
link_boost( compiler_tests system )
link_boost( compiler_tests date_time )
link_boost( compiler_tests program_options )
link_boost( compiler_tests iostreams )
link_boost( compiler_tests atomic )
link_boost( compiler_tests serialization )
link_gtest( compiler_tests )

target_link_libraries( compiler_tests compiler_config )
target_link_libraries( compiler_tests database )
target_link_libraries( compiler_tests pipeline)
target_link_libraries( compiler_tests reports)
target_link_libraries( compiler_tests database_reporters)

gtest_add_tests( TARGET compiler_tests SOURCES ${COMPILER_UNIT_TESTS} )

install( TARGETS compiler_tests DESTINATION bin )

message( "Generating the ToolChain.xml for compiler tests" )

find_path(LLVM_CONFIGURATION_PATH NAMES "llvm/install" PATHS "${CMAKE_INSTALL_PREFIX}/../../" REQUIRED NO_DEFAULT_PATH)

set( CLANG_PROGRAM_NAME )
IF( ${WIN32} )
set( CLANG_PROGRAM_NAME clang++.exe)
ELSE( ${WIN32} )
set( CLANG_PROGRAM_NAME clang++)
ENDIF( ${WIN32} )

find_file(SCRIPT_CLANG_COMPILER NAMES ${CLANG_PROGRAM_NAME} PATHS ${LLVM_CONFIGURATION_PATH}/llvm/install/bin REQUIRED NO_DEFAULT_PATH)

include( ${MEGA_ROOT_DIR}/../mega_flags.cmake )

# add_custom_command( TARGET compiler_tests POST_BUILD COMMAND ctest --output-on-failure -C $<CONFIG> )

add_custom_target( run_compiler_tests 

	COMMAND $<TARGET_FILE_NAME:compiler_tests> 
        --input ${COMPILER_TESTS_DIR}/input
        --result ${COMPILER_TESTS_DIR}/result
        --expected ${COMPILER_TESTS_DIR}/expected

		--clang_compiler ${SCRIPT_CLANG_COMPILER}
		--parser $<TARGET_FILE:parser> 
		--mega_compiler $<TARGET_FILE:compiler>
		#--mega_executor $<TARGET_FILE:executor>
		#--clang_plugin $<TARGET_FILE:clang_plugin>
		--database $<TARGET_FILE:database>
		#--leaf $<TARGET_FILE:leaf>

		--cpp_flags \"${CLANG_CXX_FLAGS_STR}\"
		--cpp_defines \"${CLANG_CXX_DEFINES_STR}\"
		--include_dirs \"${CMAKE_INSTALL_PREFIX}/../../boost/install/include\;${CMAKE_INSTALL_PREFIX}/../../common/install/include\;${MEGA_API_DIR}\"

		--compiler_templates_dir ${MEGA_SRC_DIR}/compiler/templates
		--report_templates_dir ${MEGA_SRC_DIR}/reports/templates
		--stash_dir ${CMAKE_CURRENT_BINARY_DIR}/stash

		--cout

	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)

add_dependencies( run_compiler_tests compiler_tests )

#add_custom_command( TARGET compiler_tests POST_BUILD 
#	COMMAND $<TARGET_FILE_NAME:compiler_tests> 
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
