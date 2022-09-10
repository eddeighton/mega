cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get inja
include( ${WORKSPACE_ROOT_PATH}/thirdparty/inja/inja_include.cmake )

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )


set( JINJA_TEMPLATES 
    ${MEGA_SRC_DIR}/runtime/templates/allocation.jinja 
    ${MEGA_SRC_DIR}/runtime/templates/allocate.jinja 
    ${MEGA_SRC_DIR}/runtime/templates/read.jinja 
    ${MEGA_SRC_DIR}/runtime/templates/write.jinja 
    ${MEGA_SRC_DIR}/runtime/templates/call.jinja 
	)

set( RUNTIME_HEADERS
    ${MEGA_API_DIR}/runtime/runtime_functions.hpp
    ${MEGA_API_DIR}/runtime/runtime.hpp
)

set( RUNTIME_SOURCE
    ${MEGA_SRC_DIR}/runtime/component_manager.hpp
    ${MEGA_SRC_DIR}/runtime/component_manager.cpp
    ${MEGA_SRC_DIR}/runtime/code_generator.hpp
    ${MEGA_SRC_DIR}/runtime/code_generator.cpp
    ${MEGA_SRC_DIR}/runtime/database.hpp
    ${MEGA_SRC_DIR}/runtime/database.cpp
    ${MEGA_SRC_DIR}/runtime/runtime.cpp
)

add_library( runtime
    ${RUNTIME_HEADERS} 
    ${RUNTIME_SOURCE}
)

target_link_libraries( runtime database )
target_link_libraries( runtime jit )

target_link_options( runtime PUBLIC -export-dynamic )
#set_property(TARGET runtime PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories( runtime PUBLIC ${MEGA_API_DIR} )

link_boost( runtime system )
link_boost( runtime filesystem )
link_boost( runtime serialization )
link_json( runtime )
link_inja( runtime )
link_spdlog( runtime )
link_common( runtime )

install( FILES ${RUNTIME_HEADERS} DESTINATION include/runtime )
install( TARGETS runtime EXPORT runtime DESTINATION bin)
install( EXPORT runtime NAMESPACE MEGA:: FILE runtime-config.cmake DESTINATION share )
install( FILES ${JINJA_TEMPLATES} DESTINATION templates )
