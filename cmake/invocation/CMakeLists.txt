cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( INVOCATION_HEADERS
    ${MEGA_API_DIR}/invocation/invocation.hpp
    ${MEGA_API_DIR}/invocation/name_resolution.hpp
)

set( INVOCATION_SOURCE
    ${MEGA_SRC_DIR}/invocation/invocation.cpp
    ${MEGA_SRC_DIR}/invocation/name_resolution.cpp
)

add_library( invocation
    ${INVOCATION_HEADERS} 
    ${INVOCATION_SOURCE}
)

set_property(TARGET invocation PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories( invocation PUBLIC ${MEGA_API_DIR} )

link_boost( invocation system )
link_boost( invocation filesystem )
link_boost( invocation serialization )
link_json( invocation )
link_common( invocation )

install( FILES ${INVOCATION_HEADERS} DESTINATION include/invocation )
install( TARGETS invocation EXPORT invocation DESTINATION bin)
install( EXPORT invocation NAMESPACE MEGA:: FILE invocation-config.cmake DESTINATION share )
