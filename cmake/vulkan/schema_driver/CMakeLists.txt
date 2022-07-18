
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

include_directories( ${MEGA_API_DIR} )
include_directories( ${MEGA_SRC_DIR} )

add_executable( vulkan_schema_compiler_driver ${MEGA_SRC_DIR}/vulkan/schema/compiler/main.cpp )

set_target_properties( vulkan_schema_compiler_driver PROPERTIES FOLDER vulkan_schema_compiler_driver )

link_boost( vulkan_schema_compiler_driver filesystem )
link_boost( vulkan_schema_compiler_driver system )
link_boost( vulkan_schema_compiler_driver atomic )
link_boost( vulkan_schema_compiler_driver program_options )
link_common( vulkan_schema_compiler_driver ) 

target_link_libraries(vulkan_schema_compiler_driver vulkan_schema_compiler)
target_link_libraries(vulkan_schema_compiler_driver utilities)

install( TARGETS vulkan_schema_compiler_driver DESTINATION bin)

add_dependencies( vulkan_schema_compiler_driver vulkan_schema_compiler )
