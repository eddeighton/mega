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

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( RUNTIME_SOURCE
    ${MEGA_API_DIR}/jit/orc_api.hpp
    ${MEGA_API_DIR}/jit/orc.hpp
    ${MEGA_SRC_DIR}/jit/orc.cpp
)

add_library( orc SHARED
    ${RUNTIME_HEADERS} 
    ${RUNTIME_SOURCE}
)

target_link_libraries( orc mega_interface )

# target_link_libraries( orc "X:/win8_static_release/llvm/install/lib/clang/15.0.0/lib/x86_64-pc-windows-msvc/libclang_rt.builtins.a" )

set_target_properties( orc PROPERTIES FOLDER service )
add_definitions(-DMEGA_ORC_API_SHARED_MODULE)

IF( ${WIN32} )
set_property(TARGET orc PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS OFF )
ELSE( ${WIN32} )
#target_link_options( jit PUBLIC -export-dynamic )
set_property(TARGET orc PROPERTY C_VISIBILITY_PRESET hidden)
set_property(TARGET orc PROPERTY VISIBILITY_INLINES_HIDDEN ON)
set_property(TARGET orc PROPERTY POSITION_INDEPENDENT_CODE ON)
ENDIF( ${WIN32} )

target_include_directories( orc PUBLIC ${MEGA_API_DIR} )

link_boost( orc system )
link_common( orc )
link_orc( orc )

install( FILES ${RUNTIME_HEADERS} DESTINATION include/jit )
install( TARGETS orc EXPORT orc DESTINATION bin)
install( EXPORT orc NAMESPACE MEGA:: FILE orc-config.cmake DESTINATION share )
