
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

#get the source code
set( TERMINAL_SOURCE_DIR ${MEGA_SRC_DIR}/service/terminal )

set( TERMINAL_LIB_API 
    ${MEGA_API_DIR}/service/terminal.hpp 
    )

set( TERMINAL_LIB_SRC 
    ${TERMINAL_SOURCE_DIR}/terminal.cpp
)

add_library( terminal
                    ${TERMINAL_LIB_API}
					${TERMINAL_LIB_SRC}
				)

target_compile_options( terminal PUBLIC "-fcoroutines" )

target_link_libraries( terminal protocol )
target_link_libraries( terminal network )

target_include_directories( terminal PUBLIC ${MEGA_API_DIR} )
target_include_directories( terminal PUBLIC ${MEGA_SRC_DIR} )

set_target_properties( terminal PROPERTIES FOLDER terminal )

link_boost( terminal filesystem )
link_boost( terminal system )
link_boost( terminal atomic )
link_boost( terminal coroutine )
link_boost( terminal serialization )
link_common( terminal )
link_spdlog( terminal )

target_link_libraries( terminal compiler_config )
target_link_libraries( terminal protocol )
target_link_libraries( terminal network )
target_link_libraries( terminal leaf )
target_link_libraries( terminal pipeline )
target_link_libraries( terminal database )
target_link_libraries( terminal jit )
target_link_libraries( terminal runtime )

install( FILES ${TERMINAL_LIB_API} DESTINATION include/service )
install( TARGETS terminal EXPORT terminal DESTINATION bin)
install( EXPORT terminal NAMESPACE MEGA:: FILE terminal-config.cmake DESTINATION share )
