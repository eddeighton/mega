
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

#get the source code
set( TERMINAL_SOURCE_DIR ${MEGA_SRC_DIR}/service/terminal )

set( TERMINAL_SOURCE 
    ${TERMINAL_SOURCE_DIR}/main.cpp 
    ${TERMINAL_SOURCE_DIR}/command_project.cpp 
    ${TERMINAL_SOURCE_DIR}/command_sim.cpp 
    )

add_executable( terminal ${TERMINAL_SOURCE}  )

target_compile_options( terminal PUBLIC "-fcoroutines" )

#target_link_libraries( terminal database )
target_link_libraries( terminal utilities )
target_link_libraries( terminal protocol )
target_link_libraries( terminal network )
target_link_libraries( terminal service )

set_target_properties( terminal PROPERTIES FOLDER driver )

link_boost( terminal program_options )
link_boost( terminal filesystem )
link_spdlog( terminal )
link_common( terminal )

install( FILES ${TERMINAL_API} DESTINATION include/service/terminal )
install( TARGETS terminal DESTINATION bin)
