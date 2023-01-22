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

include_directories( ${MEGA_API_DIR} )
include_directories( ${MEGA_SRC_DIR} )

set( CLANG_PLUGIN_API ${MEGA_API_DIR}/clang_plugin/clang_plugin.hpp )

set( CLANG_PLUGIN_SOURCES
    ${MEGA_SRC_DIR}/clang_plugin/clang_plugin.cpp
    ${MEGA_SRC_DIR}/clang_plugin/clang_utils.hpp
    ${MEGA_SRC_DIR}/clang_plugin/clang_utils.cpp
    ${MEGA_SRC_DIR}/clang_plugin/session.hpp
    ${MEGA_SRC_DIR}/clang_plugin/interface_session.cpp
    ${MEGA_SRC_DIR}/clang_plugin/operations_session.cpp
    )

add_definitions(-DMEGA_CLANG_PLUGIN_MODULE)

add_library( clang_plugin MODULE
                    ${CLANG_PLUGIN_API}
                    ${CLANG_PLUGIN_SOURCES} )

set_target_properties( clang_plugin PROPERTIES FOLDER clang_plugin )

link_boost_usual( clang_plugin )
link_common( clang_plugin )

IF (WIN32)
# windows needs symbols when compile
link_clang( clang_plugin )
ELSE()
# import clang - symbols linked dynamically when load plugin
import_clang( clang_plugin )

ENDIF()

target_link_libraries( clang_plugin database )
target_link_libraries( clang_plugin invocation )

install( FILES ${CLANG_PLUGIN_API} DESTINATION include/clang_plugin )
install( TARGETS clang_plugin DESTINATION bin)
