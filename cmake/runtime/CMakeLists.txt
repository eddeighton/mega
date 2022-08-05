cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get clang
include( ${WORKSPACE_ROOT_PATH}/thirdparty/llvm/clang_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( RUNTIME_HEADERS
    ${MEGA_API_DIR}/runtime/runtime.hpp
    ${MEGA_API_DIR}/runtime/runtime_api.hpp
    ${MEGA_API_DIR}/runtime/component_manager.hpp
)

set( RUNTIME_SOURCE
    ${MEGA_SRC_DIR}/runtime/runtime.cpp
    ${MEGA_SRC_DIR}/runtime/component_manager.cpp
)

add_library( runtime
    ${RUNTIME_HEADERS} 
    ${RUNTIME_SOURCE}
)

target_link_libraries( runtime database )

set_property(TARGET runtime PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories( runtime PUBLIC ${MEGA_API_DIR} )

link_boost( runtime system )
link_boost( runtime filesystem )
link_boost( runtime serialization )
link_orc( runtime )
link_json( runtime )
link_spdlog( runtime )
link_common( runtime )

install( FILES ${RUNTIME_HEADERS} DESTINATION include/runtime )
install( TARGETS runtime EXPORT runtime DESTINATION bin)
install( EXPORT runtime NAMESPACE MEGA:: FILE runtime-config.cmake DESTINATION share )
