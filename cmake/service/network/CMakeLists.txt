
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )


set( MEGA_NETWORK_API 
    ${MEGA_API_DIR}/service/network/network.hpp    
)

set( MEGA_NETWORK_SRC 
    ${MEGA_SRC_DIR}/service/network/network.cpp    
)

add_library( network
                    ${MEGA_NETWORK_API}
					${MEGA_NETWORK_SRC}
				)

target_include_directories( network PUBLIC ${MEGA_API_DIR} )

set_target_properties( network PROPERTIES FOLDER network )

link_boost( network filesystem )
link_boost( network system )
link_boost( network atomic )
link_common( network )

install( FILES ${MEGA_NETWORK_API} DESTINATION include/service/network )
install( TARGETS network EXPORT network DESTINATION bin)
install( EXPORT network NAMESPACE MEGA:: FILE network-config.cmake DESTINATION share )
