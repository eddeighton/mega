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

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get inja
include( ${WORKSPACE_ROOT_PATH}/thirdparty/inja/inja_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

#get handy function to preprocess files for the xmacros stuff
include( ../preprocess_cpp.cmake )

#################################################################
#################################################################
# configuration library that can be statically linked
set( COMPILER_LIB_HEADERS
    ${MEGA_API_DIR}/compiler/configuration.hpp
    ${MEGA_API_DIR}/compiler/build_report.hpp

    # reusable code generators
    ${MEGA_API_DIR}/compiler/generator_utility.hpp
    ${MEGA_API_DIR}/compiler/generator_interface.hpp
    ${MEGA_API_DIR}/compiler/generator_operations.hpp
    ${MEGA_API_DIR}/compiler/printer.hpp
)

set( COMPILER_LIB_SOURCE
    ${MEGA_SRC_DIR}/compiler/configuration.cpp  
    ${MEGA_SRC_DIR}/compiler/build_report.cpp
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
	${MEGA_SRC_DIR}/compiler/templates/impl.jinja 
	${MEGA_SRC_DIR}/compiler/templates/initialiser.jinja 
	${MEGA_SRC_DIR}/compiler/templates/interface.jinja 
	${MEGA_SRC_DIR}/compiler/templates/operations_externs.jinja 
	${MEGA_SRC_DIR}/compiler/templates/operations.jinja 
	${MEGA_SRC_DIR}/compiler/templates/python_wrapper.jinja 
    ${MEGA_SRC_DIR}/compiler/templates/context.jinja 
	)

set( COMPILER_TASKS
    ${MEGA_SRC_DIR}/compiler/automata.cpp
    ${MEGA_SRC_DIR}/compiler/automata.hpp
    ${MEGA_SRC_DIR}/compiler/task_allocators.cpp
    ${MEGA_SRC_DIR}/compiler/task_automata.cpp
    ${MEGA_SRC_DIR}/compiler/task_complete.cpp
    ${MEGA_SRC_DIR}/compiler/task_concrete_id.cpp
    ${MEGA_SRC_DIR}/compiler/task_concrete.cpp
    ${MEGA_SRC_DIR}/compiler/task_cpp.cpp
    ${MEGA_SRC_DIR}/compiler/task_dependencies.cpp
    ${MEGA_SRC_DIR}/compiler/task_global_memory.cpp
    ${MEGA_SRC_DIR}/compiler/task_hyper_graph.cpp
    ${MEGA_SRC_DIR}/compiler/task_implementation.cpp
    ${MEGA_SRC_DIR}/compiler/task_includes.cpp
    ${MEGA_SRC_DIR}/compiler/task_inheritance.cpp
    ${MEGA_SRC_DIR}/compiler/task_initialiser.cpp
    ${MEGA_SRC_DIR}/compiler/task_interface_analysis.cpp
    ${MEGA_SRC_DIR}/compiler/task_interface_tree.cpp
    ${MEGA_SRC_DIR}/compiler/task_manifest.cpp
    ${MEGA_SRC_DIR}/compiler/task_meta.cpp
    ${MEGA_SRC_DIR}/compiler/task_operations_locs.cpp
    ${MEGA_SRC_DIR}/compiler/task_operations.cpp
    ${MEGA_SRC_DIR}/compiler/task_parse_ast.cpp
    ${MEGA_SRC_DIR}/compiler/task_python.cpp
    ${MEGA_SRC_DIR}/compiler/task_symbols.cpp
    ${MEGA_SRC_DIR}/compiler/task_unity.cpp
    ${MEGA_SRC_DIR}/compiler/task_value_space.cpp
)


set( COMPILER_SOURCE
    ${COMPILER_TASKS}
    ${MEGA_SRC_DIR}/compiler/compiler.cpp

    # generate file from mega mangle
    ${MEGA_SRC_DIR}/compiler/mega_mangle_meta.hxx

    ## ensure the version is baked into the shared object
    ${MEGA_SRC_DIR}/version/version.cxx
)

add_library( compiler MODULE
    ${COMPILER_SOURCE}
)

add_dependencies( compiler generate_mega_mangle_meta )

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
target_link_libraries( compiler schematic )

target_include_directories( compiler PUBLIC ${MEGA_API_DIR} )

link_boost( compiler system )
link_boost( compiler filesystem )
link_boost( compiler serialization )
link_json( compiler )
link_inja( compiler )
link_common( compiler )


#################################################################
#################################################################
## install everything
install( FILES ${JINJA_TEMPLATES} DESTINATION templates )
install( FILES ${COMPILER_LIB_HEADERS} DESTINATION include/compiler )
install( TARGETS compiler_config EXPORT compiler_config DESTINATION bin)
install( EXPORT compiler_config NAMESPACE MEGA:: FILE compiler_config-config.cmake DESTINATION share )

install( TARGETS compiler DESTINATION bin)
