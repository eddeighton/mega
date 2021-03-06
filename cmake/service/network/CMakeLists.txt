
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( MEGA_NETWORK_API 
    ${MEGA_API_DIR}/service/network/log.hpp
    ${MEGA_API_DIR}/service/network/end_point.hpp
    ${MEGA_API_DIR}/service/network/sender.hpp
    ${MEGA_API_DIR}/service/network/network.hpp
    ${MEGA_API_DIR}/service/network/client.hpp
    ${MEGA_API_DIR}/service/network/server.hpp
    ${MEGA_API_DIR}/service/network/receiver.hpp
    ${MEGA_API_DIR}/service/network/conversation.hpp
    ${MEGA_API_DIR}/service/network/conversation_manager.hpp
)

set( MEGA_NETWORK_SRC 
    ${MEGA_SRC_DIR}/service/network/log.cpp
    ${MEGA_SRC_DIR}/service/network/end_point.cpp
    ${MEGA_SRC_DIR}/service/network/sender.cpp
    ${MEGA_SRC_DIR}/service/network/network.cpp
    ${MEGA_SRC_DIR}/service/network/client.cpp
    ${MEGA_SRC_DIR}/service/network/server.cpp
    ${MEGA_SRC_DIR}/service/network/receiver.cpp
    ${MEGA_SRC_DIR}/service/network/conversation.cpp
    ${MEGA_SRC_DIR}/service/network/conversation_manager.cpp
)

add_library( network
                    ${MEGA_NETWORK_API}
					${MEGA_NETWORK_SRC}
				)

target_compile_options( network PUBLIC "-fcoroutines" )

target_link_libraries( network protocol )

target_include_directories( network PUBLIC ${MEGA_API_DIR} )

set_target_properties( network PROPERTIES FOLDER network )
    
link_boost( network filesystem )
link_boost( network system )
link_boost( network atomic )
link_boost( network coroutine )
link_common( network )
link_spdlog( network )

install( FILES ${MEGA_NETWORK_API} DESTINATION include/service/network )
install( TARGETS network EXPORT network DESTINATION bin)
install( EXPORT network NAMESPACE MEGA:: FILE network-config.cmake DESTINATION share )
