
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

set( TOOL_SRC_DIR ${MEGA_SRC_DIR}/service/tool )

####################################################################
####################################################################

set( TOOL_LIB_API ${MEGA_API_DIR}/service/tool.hpp )

set( TOOL_LIB_SRC 
    ${TOOL_SRC_DIR}/tool.cpp
    ${TOOL_SRC_DIR}/request.hpp
    ${TOOL_SRC_DIR}/routing.cpp
    ${TOOL_SRC_DIR}/status.cpp
)

add_library( tool
                    ${TOOL_LIB_API}
					${TOOL_LIB_SRC}
				)

target_compile_options( tool PUBLIC "-fcoroutines" )

target_link_libraries( tool protocol )
target_link_libraries( tool network )

target_include_directories( tool PUBLIC ${MEGA_API_DIR} )
target_include_directories( tool PUBLIC ${MEGA_SRC_DIR} )

set_target_properties( tool PROPERTIES FOLDER tool )

link_boost( tool filesystem )
link_boost( tool system )
link_boost( tool atomic )
link_boost( tool coroutine )
link_boost( tool serialization )
link_common( tool )
link_spdlog( tool )

target_link_libraries( tool protocol )
target_link_libraries( tool network )
target_link_libraries( tool leaf )
target_link_libraries( tool database )
target_link_libraries( tool jit )
target_link_libraries( tool runtime )

install( FILES ${TOOL_LIB_API} DESTINATION include/service )
install( TARGETS tool EXPORT tool DESTINATION bin)
install( EXPORT tool NAMESPACE MEGA:: FILE tool-config.cmake DESTINATION share )
