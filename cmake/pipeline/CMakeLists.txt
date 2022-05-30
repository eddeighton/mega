cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( PIPELINE_HEADERS
    ${MEGA_API_DIR}/pipeline/task.hpp
    ${MEGA_API_DIR}/pipeline/pipeline.hpp
)

set( PIPELINE_SOURCE
    ${MEGA_SRC_DIR}/pipeline/pipeline.cpp
)

add_library( pipeline
    ${PIPELINE_HEADERS} 
    ${PIPELINE_SOURCE}
)

target_include_directories( pipeline PUBLIC ${MEGA_API_DIR} )

link_boost( pipeline system )
link_boost( pipeline filesystem )
link_boost( pipeline serialization )
link_common( pipeline )

install( FILES ${PIPELINE_HEADERS} DESTINATION include/pipeline )
install( TARGETS pipeline EXPORT pipeline DESTINATION bin)
install( EXPORT pipeline NAMESPACE MEGA:: FILE pipeline-config.cmake DESTINATION share )