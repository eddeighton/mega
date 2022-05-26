
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get graphviz
#include( ${WORKSPACE_ROOT_PATH}/thirdparty/graphviz/graphviz_include.cmake )

#get json
#include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

#get handy function to preprocess files for the xmacros stuff
#include( ../preprocess_cpp.cmake )

set( MEGA_PROTOCOL_GENERATED_HEADERS
    ${MEGA_API_DIR}/service/protocol/model/messages.hxx
    ${MEGA_API_DIR}/service/protocol/model/daemon_host.hxx
    ${MEGA_API_DIR}/service/protocol/model/daemon_root.hxx
    ${MEGA_API_DIR}/service/protocol/model/root_daemon.hxx
    ${MEGA_API_DIR}/service/protocol/model/host_daemon.hxx
	)

set( MEGA_PROTOCOL_GENERATED_SOURCES
    ${MEGA_SRC_DIR}/service/protocol/model/daemon_host.cxx
    ${MEGA_SRC_DIR}/service/protocol/model/daemon_root.cxx
    ${MEGA_SRC_DIR}/service/protocol/model/root_daemon.cxx
    ${MEGA_SRC_DIR}/service/protocol/model/host_daemon.cxx
)

set( PROTOCOL_SCHEMAS 
        ${MEGA_SRC_DIR}/service/protocol/daemon_host.pts
        ${MEGA_SRC_DIR}/service/protocol/daemon_root.pts
        ${MEGA_SRC_DIR}/service/protocol/root_daemon.pts
        ${MEGA_SRC_DIR}/service/protocol/host_daemon.pts
        )

set( COMPILER_OUTPUTS 
    ${MEGA_PROTOCOL_GENERATED_HEADERS} 
    ${MEGA_PROTOCOL_GENERATED_SOURCES} )

set( COMPILER_BYPRODUCTS
    daemon_host.json 
    daemon_root.json 
    root_daemon.json 
    host_daemon.json 
    )

set( TEMPLATES 
    ${MEGA_SRC_DIR}/service/protocol/compiler/templates/protocol.hxx.jinja
    ${MEGA_SRC_DIR}/service/protocol/compiler/templates/protocol.cxx.jinja
)

#add_custom_command( 
#    OUTPUT ${COMPILER_OUTPUTS}
#    COMMAND protocol_compiler_driver
#        --api ${MEGA_API_DIR}/service/protocol/model/
#        --src ${MEGA_SRC_DIR}/service/protocol/model/
#        --data_dir ${MEGA_SRC_DIR}/service/protocol/data/
#        --inja_dir ${MEGA_SRC_DIR}/service/protocol/compiler/templates/
#        --json
#        --template
#        --input ${PROTOCOL_SCHEMAS}
#    DEPENDS ${PROTOCOL_SCHEMAS} ${TEMPLATES} protocol_compiler_driver
#    COMMENT "Generating protocol" 
#    )

set( MEGA_PROTOCOL_COMMON
    ${MEGA_API_DIR}/service/protocol/common/serialisation.hpp
    ${MEGA_API_DIR}/service/protocol/common/header.hpp
    ${MEGA_API_DIR}/service/protocol/common/decoder.hpp
	)

set( MEGA_PROTOCOL_SOURCES
   # ${MEGA_SRC_DIR}/service/protocol/common/archive.cpp
    )


add_library( protocol
                    ${MEGA_PROTOCOL_COMMON}
					${MEGA_PROTOCOL_SOURCES}
					${MEGA_PROTOCOL_GENERATED_HEADERS}
					${MEGA_PROTOCOL_GENERATED_SOURCES}
				)

add_dependencies( protocol protocol_compiler_driver )
#add_dependencies( protocol protocol_generation )

target_include_directories( protocol PUBLIC ${MEGA_API_DIR} )

set_target_properties( protocol PROPERTIES FOLDER protocol )
#set_property(TARGET protocol PROPERTY POSITION_INDEPENDENT_CODE ON)

link_boost( protocol filesystem )
link_boost( protocol iostreams )
link_boost( protocol serialization )
link_boost( protocol system )
link_boost( protocol atomic )
#link_json( protocol )
link_common( protocol )

#install( FILES ${MEGA_LIBRARY} DESTINATION include/protocol/mega )
install( FILES ${MEGA_PROTOCOL_COMMON} DESTINATION include/service/protocol/common )
install( FILES ${MEGA_TYPES_HEADERS} DESTINATION include/service/protocol/types )
install( FILES ${MEGA_PROTOCOL_GENERATED_HEADERS} DESTINATION include/service/protocol/model )
install( TARGETS protocol EXPORT protocol DESTINATION bin)
install( EXPORT protocol NAMESPACE MEGA:: FILE protocol-config.cmake DESTINATION share )

#add_c_preprocessor_command( OUTPUT stages.hpp.i SOURCE ${MEGA_API_DIR}/protocol/io/stages.hpp TARGET protocol )
#add_custom_target( PreprocessStages
#	SOURCES stages.hpp.i
#	COMMENT "Proprocessing stages.hpp")
#
#add_c_preprocessor_command( OUTPUT file_info.hxx.i SOURCE ${MEGA_API_DIR}/protocol/io/file_info.hxx TARGET protocol )
#add_custom_target( PreprocessFileInfo
#    SOURCES file_info.hxx.i
#    COMMENT "Proprocessing file_info.hxx")
