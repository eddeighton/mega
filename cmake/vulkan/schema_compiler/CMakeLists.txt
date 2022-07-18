
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get inja
include( ${WORKSPACE_ROOT_PATH}/thirdparty/inja/inja_include.cmake )

#get tinyxml2
include( ${WORKSPACE_ROOT_PATH}/thirdparty/tinyxml2/tinyxml2_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

include_directories( ${MEGA_API_DIR} )
include_directories( ${MEGA_SRC_DIR} )

set( MEGA_VULKAN_SCHEMA_COMPILER
    ${MEGA_SRC_DIR}/vulkan/schema/compiler/parser.hpp
    ${MEGA_SRC_DIR}/vulkan/schema/compiler/parser.cpp
    ${MEGA_SRC_DIR}/vulkan/schema/compiler/json_converter.hpp
    ${MEGA_SRC_DIR}/vulkan/schema/compiler/json_converter.cpp
    ${MEGA_SRC_DIR}/vulkan/schema/compiler/model.hpp
    ${MEGA_SRC_DIR}/vulkan/schema/compiler/model.cpp
	)
	
add_library( vulkan_schema_compiler
                    ${MEGA_VULKAN_SCHEMA_COMPILER}
				)

set_target_properties( vulkan_schema_compiler PROPERTIES FOLDER vulkan_schema_compiler )

link_boost( vulkan_schema_compiler filesystem )
link_boost( vulkan_schema_compiler iostreams )
link_boost( vulkan_schema_compiler serialization )
link_boost( vulkan_schema_compiler system )
link_boost( vulkan_schema_compiler atomic )

link_json( vulkan_schema_compiler )
link_inja( vulkan_schema_compiler )
link_tinyxml2( vulkan_schema_compiler )

link_common( vulkan_schema_compiler ) 

install( TARGETS vulkan_schema_compiler DESTINATION bin)