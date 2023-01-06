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

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get clang
include( ${WORKSPACE_ROOT_PATH}/thirdparty/llvm/clang_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

include_directories( ${MEGA_API_DIR} )
include_directories( ${MEGA_SRC_DIR} )

set( CLANG_PLUGIN_API ${MEGA_API_DIR}/parser/parser.hpp )

set( CLANG_PLUGIN_SOURCES
    ${MEGA_SRC_DIR}/parser/clang.hpp
    ${MEGA_SRC_DIR}/parser/clang.cpp
    ${MEGA_SRC_DIR}/parser/parser.cpp
    )

add_definitions(-DMEGA_PARSER_MODULE)

add_library( parser MODULE
                    ${CLANG_PLUGIN_API}
                    ${CLANG_PLUGIN_SOURCES} )

set_property(TARGET parser PROPERTY LLVM_ENABLE_RTTI ON )
set_property(TARGET parser PROPERTY LLVM_ENABLE_EH ON )

set_target_properties( parser PROPERTIES FOLDER parser )

link_boost( parser filesystem )
link_boost( parser iostreams )
link_boost( parser serialization )
link_boost( parser system )
link_clang( parser )
link_json( parser )
link_common( parser )

# NOTE: DOES NOT link the database
# except on windows

IF( ${WIN32} )
target_compile_options(database PRIVATE /bigobj)
set_property(TARGET database PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON )

target_link_libraries( parser database )
# set_source_files_properties( 
#     ${MEGA_DATABASE_SOURCES} ${MEGA_DATABASE_GENERATED_SOURCES} ${MEGA_DATABASE_STAGE_SOURCES}
#     PROPERTIES
#     COMPILE_FLAGS -O1 
# )

ELSE( ${WIN32} )
set_property(TARGET parser PROPERTY POSITION_INDEPENDENT_CODE ON)
ENDIF( ${WIN32} )

install( FILES ${CLANG_PLUGIN_API} DESTINATION include/parser )
install( TARGETS parser DESTINATION bin)
