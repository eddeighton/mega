
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

#get the source code
set( HOST_SOURCE_DIR ${MEGA_SRC_DIR}/service/host )

set( HOST_SOURCE 
    ${HOST_SOURCE_DIR}/main.cpp 
    )

add_executable( host ${HOST_SOURCE}  )

target_compile_options( host PUBLIC "-fcoroutines" )

#target_link_libraries( host database )
target_link_libraries( host utilities )
target_link_libraries( host protocol )
target_link_libraries( host network )
target_link_libraries( host service )

set_target_properties( host PROPERTIES FOLDER driver )

link_boost( host program_options )
link_boost( host filesystem )
link_spdlog( host )
link_common( host )

install( FILES ${HOST_API} DESTINATION include/service/host )
install( TARGETS host DESTINATION bin)
