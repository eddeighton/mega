
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

set( MEGA_VULKAN_XML_COMPILER
    ${MEGA_SRC_DIR}/vulkan/xml/compiler/parser.hpp
    ${MEGA_SRC_DIR}/vulkan/xml/compiler/parser.cpp
    ${MEGA_SRC_DIR}/vulkan/xml/compiler/json_converter.hpp
    ${MEGA_SRC_DIR}/vulkan/xml/compiler/json_converter.cpp
    ${MEGA_SRC_DIR}/vulkan/xml/compiler/model.hpp
    ${MEGA_SRC_DIR}/vulkan/xml/compiler/model.cpp
	)
	
add_library( vulkan_xml_compiler
                    ${MEGA_VULKAN_XML_COMPILER}
				)

set_target_properties( vulkan_xml_compiler PROPERTIES FOLDER vulkan_xml_compiler )

link_boost( vulkan_xml_compiler filesystem )
link_boost( vulkan_xml_compiler iostreams )
link_boost( vulkan_xml_compiler serialization )
link_boost( vulkan_xml_compiler system )
link_boost( vulkan_xml_compiler atomic )

link_json( vulkan_xml_compiler )
link_inja( vulkan_xml_compiler )
link_tinyxml2( vulkan_xml_compiler )

link_common( vulkan_xml_compiler ) 

install( TARGETS vulkan_xml_compiler DESTINATION bin)