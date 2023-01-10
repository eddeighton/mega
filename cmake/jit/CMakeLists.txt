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

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get inja
include( ${WORKSPACE_ROOT_PATH}/thirdparty/inja/inja_include.cmake )

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )


set( JINJA_TEMPLATES 
    ${MEGA_SRC_DIR}/jit/templates/allocation.jinja 
    ${MEGA_SRC_DIR}/jit/templates/allocate.jinja 
    ${MEGA_SRC_DIR}/jit/templates/read.jinja 
    ${MEGA_SRC_DIR}/jit/templates/write.jinja 
    ${MEGA_SRC_DIR}/jit/templates/call.jinja 
    ${MEGA_SRC_DIR}/jit/templates/start.jinja 
    ${MEGA_SRC_DIR}/jit/templates/stop.jinja 
    ${MEGA_SRC_DIR}/jit/templates/save.jinja 
    ${MEGA_SRC_DIR}/jit/templates/load.jinja 
    ${MEGA_SRC_DIR}/jit/templates/program.jinja 
)

set( RUNTIME_HEADERS
    ${MEGA_API_DIR}/jit/api.hpp
    ${MEGA_API_DIR}/jit/functions.hpp
    ${MEGA_API_DIR}/jit/jit.hpp
    ${MEGA_API_DIR}/jit/orc.hpp
    ${MEGA_API_DIR}/jit/component_manager.hpp
    ${MEGA_API_DIR}/jit/code_generator.hpp
    ${MEGA_API_DIR}/jit/allocator.hpp
    ${MEGA_API_DIR}/jit/object_header.hpp
    ${MEGA_API_DIR}/jit/program.hpp
)

set( RUNTIME_SOURCE
    ${MEGA_SRC_DIR}/jit/symbol_utils.hpp

    ${MEGA_SRC_DIR}/jit/component_manager.cpp
    ${MEGA_SRC_DIR}/jit/code_generator.cpp
    ${MEGA_SRC_DIR}/jit/jit.cpp
    ${MEGA_SRC_DIR}/jit/allocator.cpp
    ${MEGA_SRC_DIR}/jit/program.cpp
    
)

add_library( jit
    ${RUNTIME_HEADERS} 
    ${RUNTIME_SOURCE}
)

add_definitions(-DMEGA_JIT_API_SHARED_MODULE)

include_directories( jit ${MEGA_API_DIR}/jit )

target_link_libraries( jit database )
target_link_libraries( jit orc )
target_link_libraries( jit protocol )

IF( ${WIN32} )
#set_property(TARGET jit PROPERTY ENABLE_EXPORTS ON )
set_property(TARGET jit PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS OFF )
ELSE( ${WIN32} )
target_link_options( jit PUBLIC -export-dynamic )
ENDIF( ${WIN32} )

#set_property(TARGET jit PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories( jit PUBLIC ${MEGA_API_DIR} )

link_boost( jit system )
link_boost( jit filesystem )
link_boost( jit serialization )
link_json( jit )
link_inja( jit )
link_spdlog( jit )
link_common( jit )

install( FILES ${RUNTIME_HEADERS} DESTINATION include/jit )
install( TARGETS jit EXPORT jit DESTINATION bin)
install( EXPORT jit NAMESPACE MEGA:: FILE jit-config.cmake DESTINATION share )
install( FILES ${JINJA_TEMPLATES} DESTINATION templates )
