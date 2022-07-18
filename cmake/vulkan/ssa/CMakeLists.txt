
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

#get vulkan
include( ${WORKSPACE_ROOT_PATH}/thirdparty/khronosgroup/vulkan_include.cmake )


set( VULKAN_SCHEMA ${VULKAN_DOC}/xml/registry.rnc )
set( VULKAN_XML ${VULKAN_DOC}/xml/vk.xml )
#set( VULKAN_XML ${VULKAN_INSTALLATION}/share/vulkan/registry/vk.xml )

set( MEGA_VULKAN_XML_GENERATED_HEADERS
    ${MEGA_API_DIR}/vulkan/model/xml.hxx
)

set( MEGA_VULKAN_XML_GENERATED_SOURCES
    ${MEGA_SRC_DIR}/vulkan/model/xml.cxx
)

set( SCHEMA_COMPILER_OUTPUTS 
    ${MEGA_VULKAN_XML_GENERATED_HEADERS} 
    ${MEGA_VULKAN_XML_GENERATED_SOURCES} )

set( COMPILER_BYPRODUCTS
    ssa.json 
    )

set( SCHEMA_TEMPLATES 
    ${MEGA_SRC_DIR}/vulkan/schema/compiler/templates/ssa.hxx.jinja
    ${MEGA_SRC_DIR}/vulkan/schema/compiler/templates/ssa.cxx.jinja
)

add_custom_command( 
    OUTPUT ${SCHEMA_COMPILER_OUTPUTS}
    COMMAND vulkan_schema_compiler_driver
        --api ${MEGA_API_DIR}/vulkan/model/
        --src ${MEGA_SRC_DIR}/vulkan/model/
        --data_dir ${MEGA_SRC_DIR}/vulkan/schema/data/
        --inja_dir ${MEGA_SRC_DIR}/vulkan/schema/compiler/templates/
        --json
        --template
        --vkschema ${VULKAN_SCHEMA}
    DEPENDS ${VULKAN_SCHEMA} ${SCHEMA_TEMPLATES} vulkan_schema_compiler_driver
    COMMENT "Generating vulkan xml parser" 
    )

set( XML_TEMPLATES 
    ${MEGA_SRC_DIR}/vulkan/xml/compiler/templates/ssa.hxx.jinja
    ${MEGA_SRC_DIR}/vulkan/xml/compiler/templates/ssa.cxx.jinja
)

set( MEGA_VULKAN_GENERATED_HEADERS
    ${MEGA_API_DIR}/vulkan/model/ssa.hxx
)

set( MEGA_VULKAN_GENERATED_SOURCES
    ${MEGA_SRC_DIR}/vulkan/model/ssa.cxx
)

set( XML_COMPILER_OUTPUTS 
    ${MEGA_VULKAN_GENERATED_HEADERS} 
    ${MEGA_VULKAN_GENERATED_SOURCES} )

add_custom_command( 
    OUTPUT ${XML_COMPILER_OUTPUTS}
    COMMAND vulkan_xml_compiler_driver
        --api ${MEGA_API_DIR}/vulkan/model/
        --src ${MEGA_SRC_DIR}/vulkan/model/
        --data_dir ${MEGA_SRC_DIR}/vulkan/xml/data/
        --inja_dir ${MEGA_SRC_DIR}/vulkan/xml/compiler/templates/
        --json
        --template
        --vkxml ${VULKAN_XML}
    DEPENDS ${VULKAN_XML} ${XML_TEMPLATES} vulkan_xml_compiler_driver
    COMMENT "Generating vulkan ssa" 
    )

# NOTE these files are duplicated in the ssa tests cmake
set( MEGA_VULKAN_COMMON
	)

set( MEGA_VULKAN_SOURCES
    )

add_library( ssa SHARED
                    ${MEGA_VULKAN_COMMON}
					${MEGA_VULKAN_SOURCES}
					${MEGA_VULKAN_GENERATED_HEADERS}
					${MEGA_VULKAN_GENERATED_SOURCES}
				)

add_dependencies( ssa vulkan_schema_compiler_driver )
add_dependencies( ssa vulkan_xml_compiler_driver )

target_include_directories( ssa PUBLIC ${MEGA_API_DIR} )

set_target_properties( ssa PROPERTIES FOLDER ssa )
set_property(TARGET ssa PROPERTY POSITION_INDEPENDENT_CODE ON)

link_boost( ssa filesystem )
link_boost( ssa iostreams )
link_boost( ssa serialization )
link_boost( ssa system )
link_boost( ssa atomic )
link_json( ssa )
link_common( ssa )

install( FILES ${MEGA_VULKAN_COMMON} DESTINATION include/vulkan/common )
install( FILES ${MEGA_VULKAN_GENERATED_HEADERS} DESTINATION include/vulkan/model )
install( TARGETS ssa EXPORT ssa DESTINATION bin)
install( EXPORT ssa NAMESPACE MEGA:: FILE ssa-config.cmake DESTINATION share )
