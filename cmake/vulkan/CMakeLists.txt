
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

#get vulkan
include( ${WORKSPACE_ROOT_PATH}/thirdparty/khronosgroup/vulkan_include.cmake )

set( VULKAN_PYTHON_SCRIPTS ${VULKAN_DOC}/scripts )
set( VULKAN_SCHEMA ${VULKAN_DOC}/xml/registry.rnc )
set( VULKAN_XML ${VULKAN_DOC}/xml/vk.xml )
#set( VULKAN_XML ${VULKAN_INSTALLATION}/share/vulkan/registry/vk.xml )

# /workspace/root/thirdparty/khronosgroup/Vulkan-Docs/xml/registry.rnc
# /workspace/root/thirdparty/khronosgroup/Vulkan-Docs/xml/vk.xml

set( VULKAN_SSA_JSON
    ${MEGA_SRC_DIR}/vulkan/json_to_cpp/data/ssa.json 
    )

add_custom_command( 
    OUTPUT ${VULKAN_SSA_JSON}
    COMMAND env PYTHONPATH=${VULKAN_PYTHON_SCRIPTS} python ${MEGA_SRC_DIR}/vulkan/xml_to_json/vulkan_xml_to_json.py
        #--vkschema ${VULKAN_SCHEMA}
        #--json ${VULKAN_SSA_JSON}
        -registry ${VULKAN_XML}
        -o ${MEGA_SRC_DIR}/vulkan/json_to_cpp/data
        ssa.json
    #COMMAND env PYTHONPATH=${VULKAN_PYTHON_SCRIPTS} python ${MEGA_SRC_DIR}/vulkan/xml_to_json/vulkan_xml_to_json.py
    #    #--vkschema ${VULKAN_SCHEMA}
    #    #--json ${VULKAN_SSA_JSON}
    #    -registry ${VULKAN_XML}
    #    -o ${MEGA_SRC_DIR}/vulkan/json_to_cpp/test
    #    vulkan11.h
    WORKING_DIRECTORY ${VULKAN_PYTHON_SCRIPTS}
    DEPENDS ${VULKAN_SCHEMA} ${VULKAN_XML} ${MEGA_SRC_DIR}/vulkan/xml_to_json/vulkan_xml_to_json.py ${MEGA_SRC_DIR}/vulkan/xml_to_json/json_generator.py
    COMMENT "Generating vulkan xml parser" 
    )
    
set( SSA_TEMPLATES 
    ${MEGA_SRC_DIR}/vulkan/json_to_cpp/compiler/templates/ssa.hxx.jinja
    ${MEGA_SRC_DIR}/vulkan/json_to_cpp/compiler/templates/ssa.cxx.jinja
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
    OUTPUT ${MEGA_VULKAN_GENERATED_HEADERS}
    COMMAND mega --inja
        --data ${VULKAN_SSA_JSON}
        --template ${MEGA_SRC_DIR}/vulkan/json_to_cpp/compiler/templates/ssa.hxx.jinja
        --output ${MEGA_VULKAN_GENERATED_HEADERS}
    DEPENDS ${SSA_TEMPLATES} ${VULKAN_SSA_JSON} mega
    COMMENT "Generating vulkan ssa" 
    )

add_custom_command( 
    OUTPUT ${MEGA_VULKAN_GENERATED_SOURCES}
    COMMAND mega --inja
        --data ${VULKAN_SSA_JSON}
        --template ${MEGA_SRC_DIR}/vulkan/json_to_cpp/compiler/templates/ssa.cxx.jinja
        --output ${MEGA_VULKAN_GENERATED_SOURCES}
    DEPENDS ${SSA_TEMPLATES} ${VULKAN_SSA_JSON} mega
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

add_dependencies( ssa mega )

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
