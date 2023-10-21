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

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

include( ../run_inja.cmake )

################################################################################
################################################################################
set( MEGA_VALUES_DIR ${MEGA_API_DIR}/mega/values )

set( MEGA_VALUES_COMMON_HEADERS

    ${MEGA_VALUES_DIR}/native_types.hpp
    ${MEGA_VALUES_DIR}/native_types.hxx
    ${MEGA_VALUES_DIR}/native_types_io.hpp
)
set( MEGA_VALUES_COMMON_SRC

    ${MEGA_VALUES_DIR}/native_types_io.cpp  
)

set( MEGA_VALUES_COMPILATION_DIR ${MEGA_VALUES_DIR}/compilation )

set( MEGA_VALUES_COMPILATION_HEADERS

    ${MEGA_VALUES_COMPILATION_DIR}/arguments.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/cardinality.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/compilation_configuration.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/hyper_graph.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/invocation_id.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/megastructure_installation.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/modes.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/operation_id.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/operator_id.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/ownership.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/relation_id.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/size_alignment.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/source_location.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/sources.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/sub_type_instance.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/tool_chain_hash.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/type_id_sequence.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/type_id.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/type_instance.hpp
)

set( MEGA_VALUES_COMPILATION_SRC

    ${MEGA_VALUES_COMPILATION_DIR}/arguments.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/cardinality.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/hyper_graph.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/invocation_id.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/megastructure_installation.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/modes.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/operation_id.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/operator_id.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/ownership.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/relation_id.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/sub_type_instance.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/tool_chain_hash.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/type_id_sequence.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/type_id.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/type_instance.cpp

)

set( MEGA_VALUES_CLANG_DIR ${MEGA_VALUES_DIR}/clang )
set( MEGA_VALUES_CLANG_HEADERS
    ${MEGA_VALUES_CLANG_DIR}/event.hpp
    ${MEGA_VALUES_CLANG_DIR}/operation_id_types.hpp
    ${MEGA_VALUES_CLANG_DIR}/result_type.hpp
    ${MEGA_VALUES_CLANG_DIR}/type_path.hpp
    ${MEGA_VALUES_CLANG_DIR}/variant.hpp
)
# NOTE: MEGA_VALUES_CLANG_HEADERS NOTE added to lib

set( MEGA_VALUES_RUNTIME_DIR ${MEGA_VALUES_DIR}/runtime )
set( MEGA_VALUES_RUNTIME_HEADERS
    ${MEGA_VALUES_RUNTIME_DIR}/any_io.hpp
    ${MEGA_VALUES_RUNTIME_DIR}/any.hpp
    ${MEGA_VALUES_RUNTIME_DIR}/maths_types_io.hpp
    ${MEGA_VALUES_RUNTIME_DIR}/maths_types.hpp
    ${MEGA_VALUES_RUNTIME_DIR}/mpo.hpp
    ${MEGA_VALUES_RUNTIME_DIR}/reference_io.hpp
    ${MEGA_VALUES_RUNTIME_DIR}/reference_limits.hpp
    ${MEGA_VALUES_RUNTIME_DIR}/reference.hpp
)
set( MEGA_VALUES_RUNTIME_SRC
)

set( MEGA_VALUES_SERVICE_DIR ${MEGA_VALUES_DIR}/service )
set( MEGA_VALUES_SERVICE_HEADERS
    ${MEGA_VALUES_SERVICE_DIR}/project.hpp
    ${MEGA_VALUES_SERVICE_DIR}/logical_thread_id.hpp
    ${MEGA_VALUES_SERVICE_DIR}/node.hpp
    ${MEGA_VALUES_SERVICE_DIR}/root_config.hpp
    ${MEGA_VALUES_SERVICE_DIR}/status.hpp

)
set( MEGA_VALUES_SERVICE_SRC
    ${MEGA_VALUES_SERVICE_DIR}/project.cpp
    ${MEGA_VALUES_SERVICE_DIR}/node.cpp
    ${MEGA_VALUES_SERVICE_DIR}/root_config.cpp
)

set( MEGA_VALUES_HEADERS
    ${MEGA_VALUES_COMMON_HEADERS}
    ${MEGA_VALUES_COMPILATION_HEADERS} 
    ${MEGA_VALUES_RUNTIME_HEADERS} 
    ${MEGA_VALUES_SERVICE_HEADERS} 
)
set( MEGA_VALUES_SRC 
    ${MEGA_VALUES_COMMON_SRC}
    ${MEGA_VALUES_COMPILATION_SRC}
    ${MEGA_VALUES_RUNTIME_SRC} 
    ${MEGA_VALUES_SERVICE_SRC} 
)

add_library( mega_values STATIC ${MEGA_VALUES_HEADERS} ${MEGA_VALUES_SRC} )

target_include_directories( mega_values PUBLIC ${MEGA_API_DIR} )

link_boost( mega_values system )
link_boost( mega_values filesystem )
link_boost( mega_values serialization )
link_common( mega_values )

install( TARGETS mega_values EXPORT mega_values DESTINATION bin)
install( EXPORT mega_values NAMESPACE MEGA:: FILE mega_values-config.cmake DESTINATION share )

install( FILES ${MEGA_VALUES_COMMON_HEADERS} DESTINATION include/mega/values )
install( FILES ${MEGA_VALUES_COMPILATION_HEADERS} DESTINATION include/mega/values/compilation )
install( FILES ${MEGA_VALUES_RUNTIME_HEADERS} DESTINATION include/mega/values/runtime )
install( FILES ${MEGA_VALUES_SERVICE_HEADERS} DESTINATION include/mega/values/service )
install( FILES ${MEGA_VALUES_CLANG_HEADERS} DESTINATION include/mega/values/clang )

################################################################################
################################################################################

add_custom_target( generate_mangle_rtti
    COMMAND inja
        --data ${MEGA_API_DIR}/mega/mangle/types.json
        --template ${MEGA_API_DIR}/mega/mangle/rtti.hxx.jinja
        --output ${MEGA_API_DIR}/mega/mangle/rtti.hxx
    DEPENDS ${MEGA_API_DIR}/mega/mangle/types.json ${MEGA_API_DIR}/mega/mangle/rtti.hxx.jinja inja
    BYPRODUCTS ${MEGA_API_DIR}/mega/mangle/rtti.hxx
    COMMENT "Generating rtti for Types" 
    SOURCES ${MEGA_API_DIR}/mega/mangle/types.json
)
add_dependencies( generate_mangle_rtti inja )# 

add_custom_target( generate_any_impl
    COMMAND inja
        --data ${MEGA_API_DIR}/mega/mangle/types.json
        --template ${MEGA_API_DIR}/mega/mangle/any.cxx.jinja
        --output ${MEGA_API_DIR}/mega/mangle/any.cxx
    DEPENDS ${MEGA_API_DIR}/mega/mangle/types.json ${MEGA_API_DIR}/mega/mangle/any.cxx.jinja inja
    BYPRODUCTS ${MEGA_API_DIR}/mega/mangle/any.cxx
    COMMENT "Generating any impl for Types" 
    SOURCES ${MEGA_API_DIR}/mega/mangle/types.json
)
add_dependencies( generate_any_impl inja )# 

add_custom_target( generate_mangle_types
    COMMAND inja
        --data ${MEGA_API_DIR}/mega/mangle/types.json
        --template ${MEGA_API_DIR}/mega/mangle/mangle.cxx.jinja
        --output ${MEGA_API_DIR}/mega/mangle/mangle_types.cxx
    DEPENDS ${MEGA_API_DIR}/mega/mangle/types.json ${MEGA_API_DIR}/mega/mangle/mangle.cxx.jinja inja
    BYPRODUCTS  ${MEGA_API_DIR}/mega/mangle/mangle_types.cxx
    COMMENT "Generating Type Mangle for Types" 
    SOURCES ${MEGA_API_DIR}/mega/mangle/types.json
)
add_dependencies( generate_mangle_types inja )# 

add_custom_target( generate_python_mangle_types
    COMMAND inja
        --data ${MEGA_API_DIR}/mega/mangle/types.json
        --template ${MEGA_API_DIR}/mega/mangle/python_mangle.cxx.jinja
        --output ${MEGA_API_DIR}/mega/mangle/python_mangle_types.cxx
    DEPENDS ${MEGA_API_DIR}/mega/mangle/types.json ${MEGA_API_DIR}/mega/mangle/python_mangle.cxx.jinja inja
    BYPRODUCTS  ${MEGA_API_DIR}/mega/mangle/python_mangle_types.cxx
    COMMENT "Generating Python Type Mangle for Types" 
    SOURCES ${MEGA_API_DIR}/mega/mangle/types.json
)
add_dependencies( generate_python_mangle_types inja )# 

add_custom_target( generate_mega_mangle_meta
    COMMAND inja
        --data ${MEGA_API_DIR}/mega/mangle/types.json
        --template ${MEGA_API_DIR}/mega/mangle/mega_mangle_meta.hxx.jinja
        --output ${MEGA_SRC_DIR}/compiler/mega_mangle_meta.hxx
    DEPENDS ${MEGA_API_DIR}/mega/mangle/types.json ${MEGA_API_DIR}/mega/mangle/mega_mangle_meta.hxx.jinja inja
    BYPRODUCTS  ${MEGA_SRC_DIR}/compiler/mega_mangle_meta.hxx
    COMMENT "Generating Mega Mangle Meta" 
    SOURCES ${MEGA_API_DIR}/mega/mangle/types.json
)
add_dependencies( generate_mega_mangle_meta inja )

set_target_properties( generate_any_impl PROPERTIES FOLDER service )
set_target_properties( generate_mangle_rtti PROPERTIES FOLDER service )
set_target_properties( generate_mangle_types PROPERTIES FOLDER service )
set_target_properties( generate_python_mangle_types PROPERTIES FOLDER service )
set_target_properties( generate_mega_mangle_meta PROPERTIES FOLDER service )


################################################################################
################################################################################

set( MEGA_LIBRARY
 
    ${MEGA_API_DIR}/mega/address_table.hpp
    ${MEGA_API_DIR}/mega/backtrace.hpp
    ${MEGA_API_DIR}/mega/boost_serialization_workaround.hpp
    ${MEGA_API_DIR}/mega/boost_serialization_xml_workaround.hpp
    ${MEGA_API_DIR}/mega/bin_archive.hpp
    ${MEGA_API_DIR}/mega/bitset_io.hpp
    ${MEGA_API_DIR}/mega/clock.hpp
    ${MEGA_API_DIR}/mega/common_strings.hpp
    ${MEGA_API_DIR}/mega/coroutine.hpp
    ${MEGA_API_DIR}/mega/defaults.hpp
    ${MEGA_API_DIR}/mega/eg_common_strings.hpp
    ${MEGA_API_DIR}/mega/enumeration.hpp
    ${MEGA_API_DIR}/mega/include.hpp
    ${MEGA_API_DIR}/mega/iterator.hpp
    ${MEGA_API_DIR}/mega/iterators.hpp
    ${MEGA_API_DIR}/mega/logical_address_space.hpp
    ${MEGA_API_DIR}/mega/logical_reference.hpp
    ${MEGA_API_DIR}/mega/logical_tree.hpp
    ${MEGA_API_DIR}/mega/macros.hpp
    ${MEGA_API_DIR}/mega/make_unique_without_reorder.hpp
    ${MEGA_API_DIR}/mega/memory.hpp
    ${MEGA_API_DIR}/mega/move_archive.hpp
    ${MEGA_API_DIR}/mega/printer.hpp
    ${MEGA_API_DIR}/mega/record_archive.hpp
    ${MEGA_API_DIR}/mega/reference_tree.hpp
    ${MEGA_API_DIR}/mega/resumption.hpp
    ${MEGA_API_DIR}/mega/return_reason.hpp
    ${MEGA_API_DIR}/mega/ring_allocator.hpp
    ${MEGA_API_DIR}/mega/scheduler.hpp
    ${MEGA_API_DIR}/mega/snapshot.hpp
    ${MEGA_API_DIR}/mega/tag_parser.hpp
    ${MEGA_API_DIR}/mega/tree_traversal.hpp
    ${MEGA_API_DIR}/mega/tree_visitor.hpp
    ${MEGA_API_DIR}/mega/xml_archive.hpp

 )
 
 add_library( mega_interface INTERFACE ${MEGA_LIBRARY} )
 set_target_properties( mega_interface PROPERTIES FOLDER service )
 add_dependencies( mega_interface generate_any_impl )
 add_dependencies( mega_interface generate_mangle_rtti )
 add_dependencies( mega_interface generate_mangle_types )
 add_dependencies( mega_interface generate_python_mangle_types )

 set( MEGA_MANGLE_SRC
     ${MEGA_API_DIR}/mega/mangle/traits.hpp
     ${MEGA_API_DIR}/mega/mangle/traits.cpp
     ${MEGA_API_DIR}/mega/mangle/any.cpp
     ${MEGA_API_DIR}/mega/mangle/any.cxx
     ${MEGA_API_DIR}/mega/mangle/mangle_types.cxx
     
     ${MEGA_SRC_DIR}/jit/tag_parser.cpp
 )
 
 add_library( mega_mangle SHARED ${MEGA_MANGLE_SRC} )
 
 set_target_properties( mega_mangle PROPERTIES FOLDER service )
 
 target_link_libraries( mega_mangle mega_values )
 target_link_libraries( mega_mangle mega_interface )

 IF( ${WIN32} )
 target_compile_options( mega_mangle PRIVATE /bigobj)
 # for windows need to work around limit of 65K symbols - /Ob2 appears to achieve this
 # target_compile_options(database PRIVATE /Ob2)
 # set_property(TARGET database PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS OFF )
 set_property(TARGET mega_mangle PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON )
 
 ELSE( ${WIN32} )
 target_link_options( mega_mangle PUBLIC -export-dynamic )
 ENDIF( ${WIN32} )
 
 target_include_directories( mega_mangle PUBLIC ${MEGA_API_DIR} )
 
 link_boost( mega_mangle system )
 link_boost( mega_mangle filesystem )
 link_boost( mega_mangle serialization )
 link_common( mega_mangle )
 
 install( TARGETS mega_interface EXPORT mega_interface DESTINATION bin)
 install( EXPORT mega_interface NAMESPACE MEGA:: FILE mega_interface-config.cmake DESTINATION share )

 install( TARGETS mega_mangle EXPORT mega_mangle DESTINATION bin)
 install( EXPORT mega_mangle NAMESPACE MEGA:: FILE mega_mangle-config.cmake DESTINATION share )
 
 install( FILES ${MEGA_LIBRARY} DESTINATION include/mega )
 install( FILES ${MEGA_MANGLE_SRC} DESTINATION include/mega/mangle )