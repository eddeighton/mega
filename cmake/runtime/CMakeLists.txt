
#  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
#  Author: Edward Deighton
#  License: Please see license.txt in the project root folder.
#
#  Use and copying of this software and preparation of derivative works
#  based upon this software are permitted. Any copy of this software or
#  of any derivative work must include the above copyright notice, this
#  paragraph and the one after it.  Any distribution of this software or
#  derivative works must comply with all applicable laws.
#
#  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
#  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
#  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
#  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
#  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
#  OF THE POSSIBILITY OF SUCH DAMAGES.

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
    ${MEGA_SRC_DIR}/runtime/templates/allocation.jinja 
    ${MEGA_SRC_DIR}/runtime/templates/allocate.jinja 
    ${MEGA_SRC_DIR}/runtime/templates/read.jinja 
    ${MEGA_SRC_DIR}/runtime/templates/write.jinja 
    ${MEGA_SRC_DIR}/runtime/templates/call.jinja 
	)

set( RUNTIME_HEADERS
    ${MEGA_API_DIR}/runtime/runtime_functions.hpp
    ${MEGA_API_DIR}/runtime/runtime_api.hpp
    ${MEGA_API_DIR}/runtime/mpo_context.hpp
)

set( RUNTIME_SOURCE
    ${MEGA_SRC_DIR}/runtime/component_manager.hpp
    ${MEGA_SRC_DIR}/runtime/component_manager.cpp
    ${MEGA_SRC_DIR}/runtime/code_generator.hpp
    ${MEGA_SRC_DIR}/runtime/code_generator.cpp
    ${MEGA_SRC_DIR}/runtime/database.hpp
    ${MEGA_SRC_DIR}/runtime/database.cpp
    ${MEGA_SRC_DIR}/runtime/runtime.hpp
    ${MEGA_SRC_DIR}/runtime/runtime.cpp
    ${MEGA_SRC_DIR}/runtime/object_allocator.hpp
    ${MEGA_SRC_DIR}/runtime/object_allocator.cpp
    ${MEGA_SRC_DIR}/runtime/runtime_api.cpp
    ${MEGA_SRC_DIR}/runtime/symbol_utils.hpp
    ${MEGA_SRC_DIR}/runtime/indexed_buffer_allocator.hpp
    ${MEGA_SRC_DIR}/runtime/indexed_buffer_allocator.cpp
    
)

add_library( runtime
    ${RUNTIME_HEADERS} 
    ${RUNTIME_SOURCE}
)

target_link_libraries( runtime database )
target_link_libraries( runtime jit )

target_link_options( runtime PUBLIC -export-dynamic )
#set_property(TARGET runtime PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories( runtime PUBLIC ${MEGA_API_DIR} )

link_boost( runtime system )
link_boost( runtime filesystem )
link_boost( runtime serialization )
link_json( runtime )
link_inja( runtime )
link_spdlog( runtime )
link_common( runtime )

install( FILES ${RUNTIME_HEADERS} DESTINATION include/runtime )
install( TARGETS runtime EXPORT runtime DESTINATION bin)
install( EXPORT runtime NAMESPACE MEGA:: FILE runtime-config.cmake DESTINATION share )
install( FILES ${JINJA_TEMPLATES} DESTINATION templates )
