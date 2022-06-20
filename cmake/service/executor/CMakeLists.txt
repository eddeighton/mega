
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
set( EXECUTOR_SOURCE_DIR ${MEGA_SRC_DIR}/service/executor )

set( EXECUTOR_SOURCE 
    ${EXECUTOR_SOURCE_DIR}/main.cpp 
    )

add_executable( executor ${EXECUTOR_API} ${EXECUTOR_SOURCE}  )

target_compile_options( executor PUBLIC "-fcoroutines" )

target_link_libraries( executor compiler_config )
target_link_libraries( executor protocol )
target_link_libraries( executor network )
target_link_libraries( executor service )
target_link_libraries( executor pipeline )
target_link_libraries( executor database )
target_link_libraries( executor runtime )

set_target_properties( executor PROPERTIES FOLDER driver )

link_boost( executor atomic )
link_boost( executor program_options )
link_boost( executor timer )
link_boost( executor filesystem )
link_boost( executor iostreams )
link_boost( executor serialization )
link_spdlog( executor )
link_json( executor )
link_inja( executor )
link_common( executor )

install( FILES ${EXECUTOR_API} DESTINATION include/service/executor )
install( TARGETS executor DESTINATION bin)
