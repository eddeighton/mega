
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( PROTOCOL_SCHEMA_DIR ${MEGA_SRC_DIR}/service/protocol/schema )

set( PROTOCOL_SCHEMAS 
    ${PROTOCOL_SCHEMA_DIR}/daemon_leaf.pts
    ${PROTOCOL_SCHEMA_DIR}/daemon_root.pts
    ${PROTOCOL_SCHEMA_DIR}/leaf_daemon.pts
    ${PROTOCOL_SCHEMA_DIR}/leaf_term.pts
    ${PROTOCOL_SCHEMA_DIR}/leaf_tool.pts
    ${PROTOCOL_SCHEMA_DIR}/leaf_worker.pts
    ${PROTOCOL_SCHEMA_DIR}/root_daemon.pts
    ${PROTOCOL_SCHEMA_DIR}/term_leaf.pts
    ${PROTOCOL_SCHEMA_DIR}/tool_leaf.pts
    ${PROTOCOL_SCHEMA_DIR}/worker_leaf.pts
)

set( PROTOCOL_HEADER_DIR ${MEGA_API_DIR}/service/protocol/model )

set( MEGA_PROTOCOL_GENERATED_HEADERS
    ${PROTOCOL_HEADER_DIR}/messages.hxx

    ${PROTOCOL_HEADER_DIR}/daemon_leaf.hxx
    ${PROTOCOL_HEADER_DIR}/daemon_root.hxx
    ${PROTOCOL_HEADER_DIR}/leaf_daemon.hxx
    ${PROTOCOL_HEADER_DIR}/leaf_term.hxx
    ${PROTOCOL_HEADER_DIR}/leaf_tool.hxx
    ${PROTOCOL_HEADER_DIR}/leaf_worker.hxx
    ${PROTOCOL_HEADER_DIR}/root_daemon.hxx
    ${PROTOCOL_HEADER_DIR}/term_leaf.hxx
    ${PROTOCOL_HEADER_DIR}/tool_leaf.hxx
    ${PROTOCOL_HEADER_DIR}/worker_leaf.hxx
)

set( PROTOCOL_SOURCE_DIR ${MEGA_SRC_DIR}/service/protocol/model )

set( MEGA_PROTOCOL_GENERATED_SOURCES
    ${PROTOCOL_SOURCE_DIR}/messages.cxx

    ${PROTOCOL_SOURCE_DIR}/daemon_leaf.cxx
    ${PROTOCOL_SOURCE_DIR}/daemon_root.cxx
    ${PROTOCOL_SOURCE_DIR}/leaf_daemon.cxx
    ${PROTOCOL_SOURCE_DIR}/leaf_term.cxx
    ${PROTOCOL_SOURCE_DIR}/leaf_tool.cxx
    ${PROTOCOL_SOURCE_DIR}/leaf_worker.cxx
    ${PROTOCOL_SOURCE_DIR}/root_daemon.cxx
    ${PROTOCOL_SOURCE_DIR}/term_leaf.cxx
    ${PROTOCOL_SOURCE_DIR}/tool_leaf.cxx
    ${PROTOCOL_SOURCE_DIR}/worker_leaf.cxx
)
set( COMPILER_OUTPUTS 
    ${MEGA_PROTOCOL_GENERATED_HEADERS} 
    ${MEGA_PROTOCOL_GENERATED_SOURCES} )

set( COMPILER_BYPRODUCTS
    messages.json 

    root_daemon.json
    daemon_root.json
    daemon_leaf.json
    leaf_daemon.json
    tool_leaf.json
    leaf_tool.json
    term_leaf.json
    leaf_term.json
    worker_leaf.json
    leaf_worker.json
)

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
        --input ${PROTOCOL_SCHEMAS}
    DEPENDS ${PROTOCOL_SCHEMAS} ${TEMPLATES} protocol_compiler_driver
    COMMENT "Generating protocol" 
    )

set( MEGA_PROTOCOL_COMMON_HEADERS
    ${MEGA_API_DIR}/service/protocol/common/header.hpp
    ${MEGA_API_DIR}/service/protocol/common/node.hpp
    ${MEGA_API_DIR}/service/protocol/common/pipeline_result.hpp
    ${MEGA_API_DIR}/service/protocol/common/project.hpp
    ${MEGA_API_DIR}/service/protocol/common/root_config.hpp
	)

set( MEGA_PROTOCOL_COMMON_SOURCE
    ${MEGA_SRC_DIR}/service/protocol/common/header.cpp
    ${MEGA_SRC_DIR}/service/protocol/common/node.cpp
    ${MEGA_SRC_DIR}/service/protocol/common/project.cpp
    ${MEGA_SRC_DIR}/service/protocol/common/root_config.cpp
    )

add_library( protocol
                    ${MEGA_PROTOCOL_COMMON_HEADERS}
                    ${MEGA_PROTOCOL_COMMON_SOURCE}
					${MEGA_PROTOCOL_GENERATED_HEADERS}
					${MEGA_PROTOCOL_GENERATED_SOURCES}
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
