cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )


set( COMPILER_TASKS
    ${MEGA_SRC_DIR}/compiler/base_task.hpp  
    ${MEGA_SRC_DIR}/compiler/task_concrete.cpp
    ${MEGA_SRC_DIR}/compiler/task_dependencies.cpp 
    ${MEGA_SRC_DIR}/compiler/task_includes.cpp  
    ${MEGA_SRC_DIR}/compiler/task_interface_analysis.cpp  
    ${MEGA_SRC_DIR}/compiler/task_interface.cpp  
    ${MEGA_SRC_DIR}/compiler/task_manifest.cpp
    ${MEGA_SRC_DIR}/compiler/task_parse_ast.cpp  
    ${MEGA_SRC_DIR}/compiler/task_symbols.cpp
)

set( COMPILER_HEADERS
    ${MEGA_API_DIR}/compiler/compiler.hpp
)

set( COMPILER_SOURCE
    ${COMPILER_TASKS}
    ${MEGA_SRC_DIR}/compiler/compiler.cpp
)

add_library( compiler SHARED
    ${COMPILER_HEADERS} 
    ${COMPILER_SOURCE}
)

target_link_libraries( compiler utilities)
target_link_libraries( compiler database )
target_link_libraries( compiler pipeline)

target_include_directories( compiler PUBLIC ${MEGA_API_DIR} )

link_boost( compiler system )
link_boost( compiler filesystem )
link_boost( compiler serialization )
link_common( compiler )

install( FILES ${COMPILER_HEADERS} DESTINATION include/compiler )
install( TARGETS compiler EXPORT compiler DESTINATION bin)
install( EXPORT compiler NAMESPACE MEGA:: FILE compiler-config.cmake DESTINATION share )