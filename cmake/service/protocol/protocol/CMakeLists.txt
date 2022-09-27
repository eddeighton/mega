
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( PROTOCOL_SCHEMA_DIR ${MEGA_SRC_DIR}/service/protocol/schema )

set( ROUTING_SCHEMAS
    daemon_root
    root_daemon
    daemon_leaf
    leaf_daemon
    leaf_term
    leaf_tool
    leaf_exe
    term_leaf
    tool_leaf
    exe_leaf
    mpo 
)

set( INTERFACE_SCHEMAS
    "project"
    sim
    job
    enrole
    pipeline
    status
    stash
    memory
    address
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
    ${MEGA_API_DIR}/service/protocol/common/environment.hpp  
    ${MEGA_API_DIR}/service/protocol/common/header.hpp  
    ${MEGA_API_DIR}/service/protocol/common/node.hpp
    ${MEGA_API_DIR}/service/protocol/common/megastructure_installation.hpp
    ${MEGA_API_DIR}/service/protocol/common/pipeline_result.hpp
    ${MEGA_API_DIR}/service/protocol/common/project.hpp
    ${MEGA_API_DIR}/service/protocol/common/root_config.hpp
    ${MEGA_API_DIR}/service/protocol/common/status.hpp
	)

set( MEGA_PROTOCOL_COMMON_SOURCE
    ${MEGA_SRC_DIR}/service/protocol/common/header.cpp
    ${MEGA_SRC_DIR}/service/protocol/common/node.cpp
    ${MEGA_SRC_DIR}/service/protocol/common/megastructure_installation.cpp
    ${MEGA_SRC_DIR}/service/protocol/common/project.cpp
    ${MEGA_SRC_DIR}/service/protocol/common/root_config.cpp
    ${MEGA_SRC_DIR}/service/protocol/common/status.cpp
    )

add_library( protocol
                    ${MEGA_PROTOCOL_COMMON_HEADERS}
                    ${MEGA_PROTOCOL_COMMON_SOURCE}
					${MEGA_PROTOCOL_GENERATED_HEADERS}
					${MEGA_PROTOCOL_GENERATED_SOURCES}
                    ${MEGA_SRC_DIR}/runtime/mpo_context.cpp
				)

target_link_libraries( protocol utilities )
target_link_libraries( protocol pipeline )

target_compile_options( protocol PUBLIC "-fcoroutines" )

add_dependencies( protocol protocol_compiler_driver )

target_include_directories( protocol PUBLIC ${MEGA_API_DIR} )

set_target_properties( protocol PROPERTIES FOLDER protocol )

link_boost( protocol filesystem )
link_boost( protocol iostreams )
link_boost( protocol serialization )
link_boost( protocol system )
link_boost( protocol atomic )
link_common( protocol )
link_spdlog( protocol )

install( FILES ${MEGA_PROTOCOL_COMMON_HEADERS} DESTINATION include/service/protocol/common )
install( FILES ${MEGA_TYPES_HEADERS} DESTINATION include/service/protocol/types )
install( FILES ${MEGA_PROTOCOL_GENERATED_HEADERS} DESTINATION include/service/protocol/model )
install( TARGETS protocol EXPORT protocol DESTINATION bin)
install( EXPORT protocol NAMESPACE MEGA:: FILE protocol-config.cmake DESTINATION share )
