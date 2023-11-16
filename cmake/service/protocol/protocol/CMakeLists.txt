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

#get fmt
include( ${WORKSPACE_ROOT_PATH}/thirdparty/fmt/fmt_include.cmake )

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( PROTOCOL_SCHEMA_DIR ${MEGA_SRC_DIR}/service/protocol/schema )

set( ROUTING_SCHEMAS
    root_daemon

    daemon_root
    daemon_leaf

    leaf_daemon
    leaf_term
    leaf_tool
    leaf_python
    leaf_report
    leaf_exe

    term_leaf
    tool_leaf
    python_leaf
    report_leaf
    exe_leaf
    
    mpo 
)

set( INTERFACE_SCHEMAS
    "project"
    enrole
    jit
    job
    memory
    pipeline
    python
    report
    sim
    stash
    status
)

set( PROTOCOL_HEADER_DIR ${MEGA_API_DIR}/service/protocol/model )
set( PROTOCOL_SOURCE_DIR ${MEGA_SRC_DIR}/service/protocol/model )

set( MEGA_PROTOCOL_PROTOCOL_SCHEMAS )
set( MEGA_PROTOCOL_GENERATED_HEADERS "${PROTOCOL_HEADER_DIR}/messages.hxx" )
set( MEGA_PROTOCOL_GENERATED_SOURCES "${PROTOCOL_SOURCE_DIR}/messages.cxx" )
set( COMPILER_BYPRODUCTS messages.json )

foreach(loop_var IN LISTS ROUTING_SCHEMAS)
    list(APPEND MEGA_PROTOCOL_PROTOCOL_SCHEMAS "${PROTOCOL_SCHEMA_DIR}/routing/${loop_var}.pts" )
    list(APPEND MEGA_PROTOCOL_GENERATED_HEADERS "${PROTOCOL_HEADER_DIR}/${loop_var}.hxx" )
    list(APPEND MEGA_PROTOCOL_GENERATED_SOURCES "${PROTOCOL_SOURCE_DIR}/${loop_var}.cxx" )
    list(APPEND COMPILER_BYPRODUCTS "${loop_var}.json" )
endforeach()

foreach(loop_var IN LISTS INTERFACE_SCHEMAS)
    list(APPEND MEGA_PROTOCOL_PROTOCOL_SCHEMAS "${PROTOCOL_SCHEMA_DIR}/interface/${loop_var}.pts" )
    list(APPEND MEGA_PROTOCOL_GENERATED_HEADERS "${PROTOCOL_HEADER_DIR}/${loop_var}.hxx" )
    list(APPEND MEGA_PROTOCOL_GENERATED_SOURCES "${PROTOCOL_SOURCE_DIR}/${loop_var}.cxx" )
    list(APPEND COMPILER_BYPRODUCTS "${loop_var}.json" )
endforeach()

set( COMPILER_OUTPUTS 
    ${MEGA_PROTOCOL_GENERATED_HEADERS} 
    ${MEGA_PROTOCOL_GENERATED_SOURCES} )

set( TEMPLATES 
    ${MEGA_SRC_DIR}/service/protocol/compiler/templates/messages.hxx.jinja
    ${MEGA_SRC_DIR}/service/protocol/compiler/templates/messages.cxx.jinja
    ${MEGA_SRC_DIR}/service/protocol/compiler/templates/protocol.hxx.jinja
    ${MEGA_SRC_DIR}/service/protocol/compiler/templates/protocol.cxx.jinja
)

add_custom_command( 
    OUTPUT ${COMPILER_OUTPUTS}
    COMMAND protocol_compiler_driver
        --api ${MEGA_API_DIR}/service/protocol/model/
        --src ${MEGA_SRC_DIR}/service/protocol/model/
        --data_dir ${MEGA_SRC_DIR}/service/protocol/data/
        --inja_dir ${MEGA_SRC_DIR}/service/protocol/compiler/templates/
        --json
        --template
        --input ${MEGA_PROTOCOL_PROTOCOL_SCHEMAS}
    DEPENDS ${MEGA_PROTOCOL_PROTOCOL_SCHEMAS} ${TEMPLATES} protocol_compiler_driver
    COMMENT "Generating protocol" 
    )

set( MEGA_PROTOCOL_COMMON_HEADERS
    ${MEGA_API_DIR}/service/protocol/common/context.hpp  
    ${MEGA_API_DIR}/service/protocol/common/jit_base.hpp  
    ${MEGA_API_DIR}/service/protocol/common/logical_thread_base.hpp  
    ${MEGA_API_DIR}/service/protocol/common/mpo_context_interface.hpp  
    ${MEGA_API_DIR}/service/protocol/common/received_message.hpp  
    ${MEGA_API_DIR}/service/protocol/common/sender_ref.hpp
    ${MEGA_API_DIR}/service/protocol/common/sender.hpp  
    ${MEGA_API_DIR}/service/protocol/common/transaction.hpp
    ${MEGA_API_DIR}/service/protocol/common/type_erase.hpp  
	)

set( MEGA_PROTOCOL_COMMON_SOURCE
    ${MEGA_SRC_DIR}/service/protocol/common/context.cpp  
    ${MEGA_SRC_DIR}/service/protocol/common/logical_thread_base.cpp  
    ${MEGA_SRC_DIR}/service/protocol/common/transaction.cpp
    )

add_library( protocol 
                    ${MEGA_PROTOCOL_COMMON_HEADERS}
                    ${MEGA_PROTOCOL_COMMON_SOURCE}
					${MEGA_PROTOCOL_GENERATED_HEADERS}
					${MEGA_PROTOCOL_GENERATED_SOURCES}
				)
target_precompile_headers( protocol REUSE_FROM mega_pch )

target_link_libraries( protocol mega_interface )
target_link_libraries( protocol mega_values )
target_link_libraries( protocol pipeline )
target_link_libraries( protocol log )
target_link_libraries( protocol reports )

IF( ${WIN32} )

target_compile_options(protocol PRIVATE /bigobj)
# for windows need to work around limit of 65K symbols - /Ob2 appears to achieve this
# target_compile_options(protocol PRIVATE /Ob2)

set_property(TARGET protocol PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON )
ENDIF( ${WIN32} )

add_dependencies( protocol protocol_compiler_driver )

target_include_directories( protocol PUBLIC ${MEGA_API_DIR} )

set_target_properties( protocol PROPERTIES FOLDER service/protocol )

link_boost( protocol filesystem )
link_boost( protocol iostreams )
link_boost( protocol serialization )
link_boost( protocol system )
link_common( protocol )
link_fmt( protocol )
link_spdlog( protocol )

install( FILES ${MEGA_PROTOCOL_COMMON_HEADERS} DESTINATION include/service/protocol/common )
install( FILES ${MEGA_TYPES_HEADERS} DESTINATION include/service/protocol/types )
install( FILES ${MEGA_PROTOCOL_GENERATED_HEADERS} DESTINATION include/service/protocol/model )
install( TARGETS protocol EXPORT protocol DESTINATION bin)
install( EXPORT protocol NAMESPACE MEGA:: FILE protocol-config.cmake DESTINATION share )
