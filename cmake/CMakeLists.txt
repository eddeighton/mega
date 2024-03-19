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

cmake_minimum_required( VERSION 3.1...3.16 )

# enable configuration MSVC runtime with CMAKE_MSVC_RUNTIME_LIBRARY
cmake_policy(SET CMP0091 NEW)

project( mega VERSION 1.1 LANGUAGES CXX )

include( ${WORKSPACE_ROOT_PATH}/src/env/src/common_settings.cmake )

# generically enable coroutines project wide
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

add_compile_options("-fcoroutines")

add_compile_options("-Wno-sign-conversion")
add_compile_options("-Wno-unused-function")

endif()


set_property(GLOBAL PROPERTY LLVM_ENABLE_RTTI ON )
set_property(GLOBAL PROPERTY LLVM_ENABLE_EH ON )

#setup some common directories from the root of the project
set( CMAKE_PARENT_PATH ${CMAKE_CURRENT_LIST_DIR}/.. )
find_path( MEGA_ROOT_DIR NAMES include PATHS ${CMAKE_PARENT_PATH} REQUIRED NO_DEFAULT_PATH )
set( MEGA_API_DIR ${MEGA_ROOT_DIR}/include )
set( MEGA_SRC_DIR ${MEGA_ROOT_DIR}/src )

add_subdirectory(pch)
add_subdirectory(inja)
add_subdirectory(il)
add_subdirectory(mega)
add_subdirectory(database)
add_subdirectory(log)
add_subdirectory(parser)
add_subdirectory(pipeline)
add_subdirectory(invocation)
add_subdirectory(compiler)
add_subdirectory(clang_plugin)
# add_subdirectory(database_reporters)
# add_subdirectory(event)
# add_subdirectory(runtime)
# add_subdirectory(map)
# add_subdirectory(schematic)
# add_subdirectory(service)
# add_subdirectory(driver)
# add_subdirectory(editor)
# add_subdirectory(tests)
# add_subdirectory(toolchain)

# # install scripts
set( MEGA_SCRIPTS
    ${MEGA_ROOT_DIR}/scripts/run_service.sh 
    ${MEGA_ROOT_DIR}/scripts/run_cleanup.sh 
    ${MEGA_ROOT_DIR}/scripts/clear.sh 
    ${MEGA_ROOT_DIR}/scripts/wipe_mega_builds.sh 
    ${MEGA_ROOT_DIR}/scripts/util_cleanup.sh 
)

install( FILES ${MEGA_SCRIPTS} DESTINATION bin )
