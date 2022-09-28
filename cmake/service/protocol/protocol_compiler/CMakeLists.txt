


cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get inja
include( ${WORKSPACE_ROOT_PATH}/thirdparty/inja/inja_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

include_directories( ${MEGA_API_DIR} )
include_directories( ${MEGA_SRC_DIR} )

set( MEGA_PROTOCOL_COMPILER
    ${MEGA_SRC_DIR}/service/protocol/compiler/generator.hpp
    ${MEGA_SRC_DIR}/service/protocol/compiler/generator.cpp
    ${MEGA_SRC_DIR}/service/protocol/compiler/grammar.hpp
    ${MEGA_SRC_DIR}/service/protocol/compiler/grammar.cpp
    ${MEGA_SRC_DIR}/service/protocol/compiler/json_converter.hpp
    ${MEGA_SRC_DIR}/service/protocol/compiler/json_converter.cpp
	)
	
add_library( protocol_compiler
                    ${MEGA_PROTOCOL_COMPILER}
				)

set_target_properties( protocol_compiler PROPERTIES FOLDER protocol_compiler )

link_boost( protocol_compiler filesystem )
link_boost( protocol_compiler iostreams )
link_boost( protocol_compiler serialization )
link_boost( protocol_compiler system )
link_boost( protocol_compiler atomic )

link_json( protocol_compiler )
link_inja( protocol_compiler )

link_common( protocol_compiler ) 

install( TARGETS protocol_compiler DESTINATION bin)