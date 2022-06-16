
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( MEGA_SERVICE_API 
    ${MEGA_API_DIR}/service/leaf.hpp
    ${MEGA_API_DIR}/service/tool.hpp
    ${MEGA_API_DIR}/service/terminal.hpp
    ${MEGA_API_DIR}/service/executor.hpp
    ${MEGA_API_DIR}/service/daemon.hpp
    ${MEGA_API_DIR}/service/root.hpp
)

set( MEGA_SERVICE_SRC 
    ${MEGA_SRC_DIR}/service/leaf.cpp
    ${MEGA_SRC_DIR}/service/tool.cpp
    ${MEGA_SRC_DIR}/service/terminal.cpp
    ${MEGA_SRC_DIR}/service/daemon.cpp
    ${MEGA_SRC_DIR}/service/root.cpp
    ${MEGA_SRC_DIR}/service/executor.cpp
)

add_library( service
                    ${MEGA_SERVICE_API}
					${MEGA_SERVICE_SRC}
				)

target_compile_options( service PUBLIC "-fcoroutines" )

target_link_libraries( service protocol )
target_link_libraries( service network )

target_include_directories( service PUBLIC ${MEGA_API_DIR} )

set_target_properties( service PROPERTIES FOLDER service )

link_boost( service filesystem )
link_boost( service system )
link_boost( service atomic )
link_boost( service coroutine )
link_common( service )
link_spdlog( service )

install( FILES ${MEGA_SERVICE_API} DESTINATION include/service/service )
install( TARGETS service EXPORT service DESTINATION bin)
install( EXPORT service NAMESPACE MEGA:: FILE service-config.cmake DESTINATION share )
