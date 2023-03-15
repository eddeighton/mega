##  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
##  Author: Edward Deighton
##  License: Please see license.txt in the project root folder.

##  Use and copying of this software and preparation of derivative works
##  based upon this software are permitted. Any copy of this software or
##  of any derivative work must include the above copyright notice, this
##  paragraph and the one after it.  Any distribution of this software or
##  derivative works must comply with all applicable laws.

##  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
##  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
##  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
##  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
##  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
##  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
##  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
##  OF THE POSSIBILITY OF SUCH DAMAGES.



cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

#get handy function to preprocess files for the xmacros stuff
include( ../preprocess_cpp.cmake )

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

set_target_properties( compiler_config PROPERTIES FOLDER compiler )
target_link_libraries( compiler_config pipeline )

IF( ${WIN32} )
target_compile_options(compiler_config PRIVATE /bigobj)
set_property(TARGET compiler_config PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON )
ELSE( ${WIN32} )
set_property(TARGET compiler_config PROPERTY POSITION_INDEPENDENT_CODE ON)
ENDIF( ${WIN32} )

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
	${MEGA_SRC_DIR}/compiler/templates/operations.jinja 
	${MEGA_SRC_DIR}/compiler/templates/impl.jinja 
	${MEGA_SRC_DIR}/compiler/templates/python_wrapper.jinja 
	)

set( COMPILER_TASKS
    ${MEGA_SRC_DIR}/compiler/task_allocators.cpp
    ${MEGA_SRC_DIR}/compiler/task_complete.cpp
    ${MEGA_SRC_DIR}/compiler/task_concrete_id.cpp
    ${MEGA_SRC_DIR}/compiler/task_concrete.cpp
    ${MEGA_SRC_DIR}/compiler/task_cpp.cpp
    ${MEGA_SRC_DIR}/compiler/task_dependencies.cpp
    ${MEGA_SRC_DIR}/compiler/task_derivation.cpp
    ${MEGA_SRC_DIR}/compiler/task_hyper_graph.cpp
    ${MEGA_SRC_DIR}/compiler/task_implementation.cpp
    ${MEGA_SRC_DIR}/compiler/task_includes.cpp
    ${MEGA_SRC_DIR}/compiler/task_interface_analysis.cpp
    ${MEGA_SRC_DIR}/compiler/task_interface.cpp
    ${MEGA_SRC_DIR}/compiler/task_manifest.cpp
    ${MEGA_SRC_DIR}/compiler/task_meta.cpp
    ${MEGA_SRC_DIR}/compiler/task_operations.cpp
    ${MEGA_SRC_DIR}/compiler/task_parse_ast.cpp
    ${MEGA_SRC_DIR}/compiler/task_python.cpp
    ${MEGA_SRC_DIR}/compiler/task_schematic_connection.cpp
    ${MEGA_SRC_DIR}/compiler/task_schematic_contour.cpp
    ${MEGA_SRC_DIR}/compiler/task_schematic_extrusion.cpp
    ${MEGA_SRC_DIR}/compiler/task_schematic_floor_plan.cpp
    ${MEGA_SRC_DIR}/compiler/task_schematic_map_file.cpp
    ${MEGA_SRC_DIR}/compiler/task_schematic_parse.cpp
    ${MEGA_SRC_DIR}/compiler/task_schematic_spawn_points.cpp
    ${MEGA_SRC_DIR}/compiler/task_schematic_value_space.cpp
    ${MEGA_SRC_DIR}/compiler/task_schematic_visibility.cpp
    ${MEGA_SRC_DIR}/compiler/task_schematic_wall_sections.cpp
    ${MEGA_SRC_DIR}/compiler/task_symbols.cpp
    ${MEGA_SRC_DIR}/compiler/task_unity.cpp
)


set( COMPILER_SOURCE
    ${COMPILER_TASKS}
    ${MEGA_SRC_DIR}/compiler/compiler.cpp

    ## ensure the version is baked into the shared object
    ${MEGA_SRC_DIR}/version/version.cxx
)

add_library( compiler MODULE
    ${COMPILER_SOURCE}
)

set_target_properties( compiler PROPERTIES FOLDER compiler )

IF( ${WIN32} )
target_compile_options(compiler PRIVATE /bigobj)
set_property(TARGET compiler PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON )
ELSE( ${WIN32} )
#set_property(TARGET compiler PROPERTY POSITION_INDEPENDENT_CODE ON)
ENDIF( ${WIN32} )

add_c_preprocessor_command( OUTPUT task_utils.hpp.i SOURCE ${MEGA_SRC_DIR}/compiler/task_utils.hpp TARGET compiler )
add_custom_target( PreprocessCompilerXMacros 
	SOURCES task_utils.hpp.i
	COMMENT "Proprocessing task_utils.hpp")

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
install( EXPORT compiler_config NAMESPACE MEGA:: FILE compiler_config-config.cmake DESTINATION share )

install( TARGETS compiler DESTINATION bin)
