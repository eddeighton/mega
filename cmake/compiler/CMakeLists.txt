cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

#################################################################
#################################################################
# configuration library that can be statically linked
set( COMPILER_LIB_HEADERS
    ${MEGA_API_DIR}/compiler/configuration.hpp
)

set( COMPILER_LIB_SOURCE
    ${MEGA_SRC_DIR}/compiler/configuration.cpp  
)

add_library( compiler_config
    ${COMPILER_LIB_SOURCE} 
    ${COMPILER_LIB_SOURCE}
)

set_property(TARGET compiler_config PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories( compiler_config PUBLIC ${MEGA_API_DIR} )

link_boost( compiler_config system )
link_boost( compiler_config filesystem )
link_boost( compiler_config serialization )
link_common( compiler_config )


#################################################################
#################################################################
# dynamically loaded module that implements the compiler pipeline
set( JINJA_TEMPLATES 
    ${MEGA_SRC_DIR}/compiler/templates/context.jinja 
	${MEGA_SRC_DIR}/compiler/templates/interface.jinja 
	${MEGA_SRC_DIR}/compiler/templates/namespace.jinja 
	${MEGA_SRC_DIR}/compiler/templates/operations.jinja 
	)

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
    ${MEGA_SRC_DIR}/compiler/task_operations.cpp
    ${MEGA_SRC_DIR}/compiler/task_implementation.cpp
)

set( COMPILER_SOURCE
    ${COMPILER_TASKS}
    ${MEGA_SRC_DIR}/compiler/compiler.cpp

    ## ensure the version is baked into the shared object
    ${MEGA_SRC_DIR}/version/version.cxx
)

add_library( compiler SHARED
    ${COMPILER_SOURCE}
)

target_link_libraries( compiler compiler_config )
target_link_libraries( compiler utilities )
target_link_libraries( compiler database )
target_link_libraries( compiler pipeline )

target_include_directories( compiler PUBLIC ${MEGA_API_DIR} )

link_boost( compiler system )
link_boost( compiler filesystem )
link_boost( compiler serialization )
link_common( compiler )


#################################################################
#################################################################
## install everything
install( FILES ${JINJA_TEMPLATES} DESTINATION templates )
install( FILES ${COMPILER_LIB_HEADERS} DESTINATION include/compiler )
install( TARGETS compiler_config EXPORT compiler_config DESTINATION bin)
install( TARGETS compiler EXPORT compiler DESTINATION bin)
install( EXPORT compiler_config NAMESPACE MEGA:: FILE compiler_config-config.cmake DESTINATION share )
install( EXPORT compiler NAMESPACE MEGA:: FILE compiler-config.cmake DESTINATION share )