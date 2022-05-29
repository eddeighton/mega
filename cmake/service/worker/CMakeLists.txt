
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )


#get the source code
set( WORKER_API_DIR    ${MEGA_API_DIR}/service/worker )
set( WORKER_SOURCE_DIR ${MEGA_SRC_DIR}/service/worker )

set( WORKER_API
    ${WORKER_API_DIR}/worker.hpp
)

set( WORKER_SOURCE 
    ${WORKER_SOURCE_DIR}/worker.cpp 
    ${WORKER_SOURCE_DIR}/main.cpp 
    )

add_executable( worker ${WORKER_API} ${WORKER_SOURCE}  )

target_compile_options( worker PUBLIC "-fcoroutines" )

target_link_libraries( worker database )
target_link_libraries( worker utilities )
target_link_libraries( worker protocol )
target_link_libraries( worker network )
target_link_libraries( worker pipeline )

set_target_properties( worker PROPERTIES FOLDER driver )

link_boost( worker program_options )
link_boost( worker filesystem )
link_spdlog( worker )
link_common( worker )

install( FILES ${WORKER_API} DESTINATION include/service/worker )
install( TARGETS worker DESTINATION bin)
