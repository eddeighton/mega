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

#insist on ALL projects using c++ 17
#set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD 20 )

# add_link_options("-fuse-ld=gold;LINKER:--threads,--thread-count=12,--split-stack-adjust-size=1048510")


######################################
#win32 specific settings
IF( ${WIN32} )

add_definitions(-D_WIN32_WINNT=0x0601)
add_definitions(-DNOMINMAX -D_NOMINMAX)
add_definitions(-DMTd )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi /O2 /Ob1 /D NDEBUG ")
set( CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} /Zi /O2 /Ob1 /D NDEBUG ")

string(APPEND CMAKE_EXE_LINKER_FLAGS " /DEBUG /OPT:ICF /OPT:REF")
string(APPEND CMAKE_SHARED_LINKER_FLAGS " /DEBUG /OPT:ICF /OPT:REF")

# set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:DebugDLL>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

#enable folders within generated IDE build systems
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# add_compile_options(/W4 /WX)
add_compile_options( /MP ) #Build with multiple processes

ELSE( ${WIN32} )

#add_compile_options("-Wshadow")
add_compile_options("-Wfatal-errors")
#add_compile_options("-Werror")
#add_compile_options("-Wall")
#add_compile_options("-Wextra")

ENDIF( ${WIN32} )

set_property(GLOBAL PROPERTY LLVM_ENABLE_RTTI ON )
set_property(GLOBAL PROPERTY LLVM_ENABLE_EH ON )
set_property(GLOBAL PROPERTY POSITION_INDEPENDENT_CODE ON)

#setup some common directories from the root of the project
set( CMAKE_PARENT_PATH ${CMAKE_CURRENT_LIST_DIR}/.. )
find_path( MEGA_ROOT_DIR NAMES include PATHS ${CMAKE_PARENT_PATH} REQUIRED NO_DEFAULT_PATH )
set( MEGA_API_DIR ${MEGA_ROOT_DIR}/include )
set( MEGA_SRC_DIR ${MEGA_ROOT_DIR}/src )

#enable folders within generated IDE build systems
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# attempt to enable link time optimizations for ALL targets
set_property(GLOBAL PROPERTY CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

#ensure language runtimes and other system libs are copied to installation folder
include (InstallRequiredSystemLibraries)

add_subdirectory(inja)
add_subdirectory(version)
add_subdirectory(utilities)
add_subdirectory(pipeline)

add_subdirectory(database)

add_subdirectory(map)

IF( ${WIN32} )

# windows specific projects

ELSE( ${WIN32} )

# linux specific projects
add_subdirectory(schematic)

ENDIF( ${WIN32} )

add_subdirectory(invocation)
add_subdirectory(clang_plugin)

add_subdirectory(parser)
add_subdirectory(compiler)
add_subdirectory(reports)
add_subdirectory(log)
add_subdirectory(orc)
add_subdirectory(jit)
add_subdirectory(service)
add_subdirectory(driver)
add_subdirectory(tests)
add_subdirectory(mega)

IF( ${WIN32} )

# windows specific projects

ELSE( ${WIN32} )

# linux specific projects
add_subdirectory(editor)

ENDIF( ${WIN32} )

# always last - toolchain reads other files from build
add_subdirectory(toolchain)

# install scripts
set( MEGA_SCRIPTS
    ${MEGA_ROOT_DIR}/scripts/run_service.sh 
    ${MEGA_ROOT_DIR}/scripts/run_cleanup.sh 
)

install( FILES ${MEGA_SCRIPTS} DESTINATION bin )
