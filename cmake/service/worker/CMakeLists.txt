
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get inja
include( ${WORKSPACE_ROOT_PATH}/thirdparty/inja/inja_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

#get the source code
set( WORKER_SOURCE_DIR ${MEGA_SRC_DIR}/service/worker )

set( WORKER_SOURCE 
    ${WORKER_SOURCE_DIR}/main.cpp 
    )

add_executable( worker ${WORKER_API} ${WORKER_SOURCE}  )

target_compile_options( worker PUBLIC "-fcoroutines" )

target_link_libraries( worker compiler_config )
target_link_libraries( worker protocol )
target_link_libraries( worker network )
target_link_libraries( worker service )
target_link_libraries( worker pipeline )
target_link_libraries( worker database )
target_link_libraries( parser utilities )

set_target_properties( worker PROPERTIES FOLDER driver )

link_boost( worker atomic )
link_boost( worker program_options )
link_boost( worker timer )
link_boost( worker filesystem )
link_boost( worker iostreams )
link_boost( worker serialization )
link_spdlog( worker )
link_json( worker )
link_inja( worker )
link_common( worker )

install( FILES ${WORKER_API} DESTINATION include/service/worker )
install( TARGETS worker DESTINATION bin)
