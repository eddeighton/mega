
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )


#get the source code
set( DAEMON_SOURCE_DIR ${MEGA_SRC_DIR}/service/daemon )

set( DAEMON_SOURCE 
    ${DAEMON_SOURCE_DIR}/shared_memory_manager.hpp
    ${DAEMON_SOURCE_DIR}/daemon.hpp
    ${DAEMON_SOURCE_DIR}/daemon.cpp
    ${DAEMON_SOURCE_DIR}/main.cpp 
    ${DAEMON_SOURCE_DIR}/request.hpp
    ${DAEMON_SOURCE_DIR}/routing.cpp
    ${DAEMON_SOURCE_DIR}/enrole.cpp
    ${DAEMON_SOURCE_DIR}/status.cpp
    )

add_executable( daemon ${DAEMON_HEADERS} ${DAEMON_SOURCE}  )

target_compile_options( daemon PUBLIC "-fcoroutines" )

target_link_libraries( daemon utilities )
target_link_libraries( daemon protocol )
target_link_libraries( daemon network )

set_target_properties( daemon PROPERTIES FOLDER driver )

link_boost( daemon program_options )
link_boost( daemon filesystem )
link_boost( daemon system )
link_spdlog( daemon )
link_common( daemon )

install( TARGETS daemon DESTINATION bin)
