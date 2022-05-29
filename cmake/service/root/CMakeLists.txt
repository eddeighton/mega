
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )


#get the source code
set( DAEMON_API_DIR ${MEGA_API_DIR}/service/root )
set( DAEMON_SOURCE_DIR ${MEGA_SRC_DIR}/service/root )

set( DAEMON_SOURCE 
    ${DAEMON_API_DIR}/root.hpp
    ${DAEMON_SOURCE_DIR}/root.cpp
    ${DAEMON_SOURCE_DIR}/main.cpp
 )

add_executable( root ${DAEMON_SOURCE}  )

target_compile_options( root PUBLIC "-fcoroutines" )

target_link_libraries( root database )
target_link_libraries( root utilities )
target_link_libraries( root protocol )
target_link_libraries( root network )
target_link_libraries( root pipeline )

set_target_properties( root PROPERTIES FOLDER driver )

link_boost( root program_options )
link_boost( root filesystem )
link_spdlog( root )
link_common( root )

install( TARGETS root DESTINATION bin)
