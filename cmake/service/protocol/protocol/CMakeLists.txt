
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( MEGA_PROTOCOL_GENERATED_HEADERS
    ${MEGA_API_DIR}/service/protocol/model/messages.hxx
    ${MEGA_API_DIR}/service/protocol/model/daemon_host.hxx
    ${MEGA_API_DIR}/service/protocol/model/daemon_worker.hxx
    ${MEGA_API_DIR}/service/protocol/model/daemon_root.hxx
    ${MEGA_API_DIR}/service/protocol/model/host_daemon.hxx
    ${MEGA_API_DIR}/service/protocol/model/worker_daemon.hxx
    ${MEGA_API_DIR}/service/protocol/model/root_daemon.hxx
)

set( MEGA_PROTOCOL_GENERATED_SOURCES
    ${MEGA_SRC_DIR}/service/protocol/model/messages.cxx
    ${MEGA_SRC_DIR}/service/protocol/model/daemon_host.cxx
    ${MEGA_SRC_DIR}/service/protocol/model/daemon_worker.cxx
    ${MEGA_SRC_DIR}/service/protocol/model/daemon_root.cxx
    ${MEGA_SRC_DIR}/service/protocol/model/host_daemon.cxx
    ${MEGA_SRC_DIR}/service/protocol/model/worker_daemon.cxx
    ${MEGA_SRC_DIR}/service/protocol/model/root_daemon.cxx
)

set( PROTOCOL_SCHEMAS 
    ${MEGA_SRC_DIR}/service/protocol/daemon_host.pts
    ${MEGA_SRC_DIR}/service/protocol/daemon_worker.pts
    ${MEGA_SRC_DIR}/service/protocol/daemon_root.pts
    ${MEGA_SRC_DIR}/service/protocol/host_daemon.pts
    ${MEGA_SRC_DIR}/service/protocol/worker_daemon.pts
    ${MEGA_SRC_DIR}/service/protocol/root_daemon.pts
)

set( COMPILER_OUTPUTS 
    ${MEGA_PROTOCOL_GENERATED_HEADERS} 
    ${MEGA_PROTOCOL_GENERATED_SOURCES} )

set( COMPILER_BYPRODUCTS
    daemon_host.json 
    daemon_worker.json 
    daemon_root.json 
    host_daemon.json 
    worker_daemon.json 
    root_daemon.json 
    messages.json 
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

set( MEGA_PROTOCOL_COMMON
    ${MEGA_API_DIR}/service/protocol/common/serialisation.hpp
    ${MEGA_API_DIR}/service/protocol/common/header.hpp
    ${MEGA_API_DIR}/service/protocol/common/pipeline_result.hpp
	)

add_library( protocol
                    ${MEGA_PROTOCOL_COMMON}
					${MEGA_PROTOCOL_GENERATED_HEADERS}
					${MEGA_PROTOCOL_GENERATED_SOURCES}
				)

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

install( FILES ${MEGA_PROTOCOL_COMMON} DESTINATION include/service/protocol/common )
install( FILES ${MEGA_TYPES_HEADERS} DESTINATION include/service/protocol/types )
install( FILES ${MEGA_PROTOCOL_GENERATED_HEADERS} DESTINATION include/service/protocol/model )
install( TARGETS protocol EXPORT protocol DESTINATION bin)
install( EXPORT protocol NAMESPACE MEGA:: FILE protocol-config.cmake DESTINATION share )
