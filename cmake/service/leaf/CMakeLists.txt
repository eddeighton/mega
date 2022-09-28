


cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( MEGA_LEAF_API 
    ${MEGA_API_DIR}/service/leaf.hpp
)

set( MEGA_LEAF_SRC 
    ${MEGA_SRC_DIR}/service/leaf/request.hpp
    ${MEGA_SRC_DIR}/service/leaf/leaf.cpp
    ${MEGA_SRC_DIR}/service/leaf/routing.cpp
    ${MEGA_SRC_DIR}/service/leaf/status.cpp
)

add_library( leaf
                    ${MEGA_LEAF_API}
					${MEGA_LEAF_SRC}
				)

target_compile_options( leaf PUBLIC "-fcoroutines" )

target_link_libraries( leaf protocol )
target_link_libraries( leaf network )

target_include_directories( leaf PUBLIC ${MEGA_API_DIR} )
target_include_directories( leaf PUBLIC ${MEGA_SRC_DIR} )

set_target_properties( leaf PROPERTIES FOLDER leaf )

link_boost( leaf filesystem )
link_boost( leaf system )
link_boost( leaf atomic )
link_boost( leaf coroutine )
link_common( leaf )
link_spdlog( leaf )

install( FILES ${MEGA_LEAF_API} DESTINATION include/service )
install( TARGETS leaf EXPORT leaf DESTINATION bin)
install( EXPORT leaf NAMESPACE MEGA:: FILE leaf-config.cmake DESTINATION share )
