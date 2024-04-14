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

cmake_minimum_required( VERSION 3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get inja
include( ${WORKSPACE_ROOT_PATH}/thirdparty/inja/inja_include.cmake )

#get pybind11
include( ${WORKSPACE_ROOT_PATH}/thirdparty/pybind11/pybind11_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

################################################################################
################################################################################
set( MEGA_VALUES_DIR ${MEGA_API_DIR}/mega/values )

set( MEGA_VALUES_COMMON_HEADERS

    ${MEGA_VALUES_DIR}/runtime.h
    ${MEGA_VALUES_DIR}/native_types.h

    ${MEGA_VALUES_DIR}/native_types.hpp
    ${MEGA_VALUES_DIR}/native_types.hxx
    ${MEGA_VALUES_DIR}/native_types_io.hpp
)
set( MEGA_VALUES_COMMON_SRC

    ${MEGA_VALUES_DIR}/native_types_io.cpp  
)

set( MEGA_VALUES_COMPILATION_DIR ${MEGA_VALUES_DIR}/compilation )

set( MEGA_VALUES_COMPILATION_INTERFACE_HEADERS
    ${MEGA_VALUES_COMPILATION_DIR}/interface/inline.h

    ${MEGA_VALUES_COMPILATION_DIR}/interface/object_id.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/interface/relation_id.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/interface/sub_object_id.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/interface/symbol_id.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/interface/type_id.hpp
)

set( MEGA_VALUES_COMPILATION_CONCRETE_HEADERS
    ${MEGA_VALUES_COMPILATION_DIR}/concrete/inline.h

    ${MEGA_VALUES_COMPILATION_DIR}/concrete/instance.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/concrete/object_id.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/concrete/sub_object_id_instance.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/concrete/sub_object_id.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/concrete/type_id_instance.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/concrete/type_id.hpp
)

set( MEGA_VALUES_COMPILATION_HEADERS

    ${MEGA_VALUES_COMPILATION_DIR}/anon_symbols.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/cardinality.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/compilation_configuration.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/explicit_operation_id.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/hyper_graph.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/icontext_flags.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/invocation_id.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/megastructure_installation.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/modes.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/operator_id.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/ownership.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/reserved_symbols.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/size_alignment.hpp   
    ${MEGA_VALUES_COMPILATION_DIR}/source_location.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/symbol_table.hpp
    ${MEGA_VALUES_COMPILATION_DIR}/tool_chain_hash.hpp
)

set( MEGA_VALUES_COMPILATION_SRC

    ${MEGA_VALUES_COMPILATION_DIR}/anon_symbols.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/explicit_operation_id.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/hyper_graph.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/icontext_flags.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/megastructure_installation.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/modes.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/operator_id.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/ownership.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/reserved_symbols.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/symbol_table.cpp
    ${MEGA_VALUES_COMPILATION_DIR}/tool_chain_hash.cpp

)

set( MEGA_VALUES_CLANG_DIR ${MEGA_VALUES_DIR}/clang )
set( MEGA_VALUES_CLANG_HEADERS
    ${MEGA_VALUES_CLANG_DIR}/attribute_id.hpp
    ${MEGA_VALUES_CLANG_DIR}/mega_pointer.hpp
    ${MEGA_VALUES_CLANG_DIR}/operation_id_types.hpp
    ${MEGA_VALUES_CLANG_DIR}/result_type.hpp
    ${MEGA_VALUES_CLANG_DIR}/symbol_path.hpp
)
# NOTE: MEGA_VALUES_CLANG_HEADERS NOTE added to lib

set( MEGA_VALUES_RUNTIME_DIR ${MEGA_VALUES_DIR}/runtime )
set( MEGA_VALUES_RUNTIME_HEADERS
    ${MEGA_VALUES_RUNTIME_DIR}/inline.h

    ${MEGA_VALUES_RUNTIME_DIR}/allocation_id.hpp

    ${MEGA_VALUES_RUNTIME_DIR}/machine_id.hpp
    ${MEGA_VALUES_RUNTIME_DIR}/process_id.hpp
    ${MEGA_VALUES_RUNTIME_DIR}/owner_id.hpp

    ${MEGA_VALUES_RUNTIME_DIR}/mp.hpp
    ${MEGA_VALUES_RUNTIME_DIR}/mpo.hpp

    ${MEGA_VALUES_RUNTIME_DIR}/any_io.hpp
    ${MEGA_VALUES_RUNTIME_DIR}/any.hpp
    ${MEGA_VALUES_RUNTIME_DIR}/maths_types_io.hpp
    ${MEGA_VALUES_RUNTIME_DIR}/maths_types.hpp
    ${MEGA_VALUES_RUNTIME_DIR}/mpo.hpp
    ${MEGA_VALUES_RUNTIME_DIR}/pointer.hpp
    ${MEGA_VALUES_RUNTIME_DIR}/timestamp.hpp
)

set( MEGA_VALUES_RUNTIME_SRC
)

set( MEGA_VALUES_SERVICE_DIR ${MEGA_VALUES_DIR}/service )
set( MEGA_VALUES_SERVICE_HEADERS
    ${MEGA_VALUES_SERVICE_DIR}/logical_thread_id.hpp
    ${MEGA_VALUES_SERVICE_DIR}/node.hpp
    ${MEGA_VALUES_SERVICE_DIR}/program.hpp
    ${MEGA_VALUES_SERVICE_DIR}/project.hpp
    ${MEGA_VALUES_SERVICE_DIR}/root_config.hpp
    ${MEGA_VALUES_SERVICE_DIR}/status.hpp

)
set( MEGA_VALUES_SERVICE_SRC
    ${MEGA_VALUES_SERVICE_DIR}/node.cpp
    ${MEGA_VALUES_SERVICE_DIR}/program.cpp
    ${MEGA_VALUES_SERVICE_DIR}/project.cpp
    ${MEGA_VALUES_SERVICE_DIR}/root_config.cpp
)

set( MEGA_VALUES_HEADERS
    ${MEGA_VALUES_COMMON_HEADERS}
    ${MEGA_VALUES_COMPILATION_HEADERS} 
    ${MEGA_VALUES_COMPILATION_INTERFACE_HEADERS} 
    ${MEGA_VALUES_COMPILATION_CONCRETE_HEADERS} 
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

link_boost( mega_values atomic )
link_boost( mega_values filesystem )
link_boost( mega_values iostreams )
link_boost( mega_values serialization )
link_boost( mega_values system )
link_boost( mega_values timer )
link_boost( mega_values url )

link_json( mega_values )
link_inja( mega_values )
link_pybind11( mega_values )
link_common( mega_values )

target_include_directories( mega_values PUBLIC ${MEGA_API_DIR} )

install( TARGETS mega_values EXPORT mega_values DESTINATION bin)
install( EXPORT mega_values NAMESPACE MEGA:: FILE mega_values-config.cmake DESTINATION share )

install( FILES ${MEGA_VALUES_COMMON_HEADERS} DESTINATION include/mega/values )
install( FILES ${MEGA_VALUES_COMPILATION_HEADERS} DESTINATION include/mega/values/compilation )
install( FILES ${MEGA_VALUES_COMPILATION_INTERFACE_HEADERS} DESTINATION include/mega/values/compilation/interface )
install( FILES ${MEGA_VALUES_COMPILATION_CONCRETE_HEADERS} DESTINATION include/mega/values/compilation/concrete )
install( FILES ${MEGA_VALUES_RUNTIME_HEADERS} DESTINATION include/mega/values/runtime )
install( FILES ${MEGA_VALUES_SERVICE_HEADERS} DESTINATION include/mega/values/service )
install( FILES ${MEGA_VALUES_CLANG_HEADERS} DESTINATION include/mega/values/clang )

set( MEGA_LIBRARY
 
    ${MEGA_API_DIR}/mega/address_table.hpp
    ${MEGA_API_DIR}/mega/backtrace.hpp
    ${MEGA_API_DIR}/mega/bin_archive.hpp
    ${MEGA_API_DIR}/mega/bitset_io.hpp
    ${MEGA_API_DIR}/mega/boost_serialization_workaround.hpp
    ${MEGA_API_DIR}/mega/boost_serialization_xml_workaround.hpp
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
    ${MEGA_API_DIR}/mega/logical_pointer.hpp
    ${MEGA_API_DIR}/mega/logical_tree.hpp
    ${MEGA_API_DIR}/mega/macros.hpp
    ${MEGA_API_DIR}/mega/make_unique_without_reorder.hpp
    ${MEGA_API_DIR}/mega/memory.hpp
    ${MEGA_API_DIR}/mega/move_archive.hpp
    ${MEGA_API_DIR}/mega/printer.hpp
    ${MEGA_API_DIR}/mega/program_manifest.hpp
    ${MEGA_API_DIR}/mega/record_archive.hpp
    ${MEGA_API_DIR}/mega/resumption.hpp
    ${MEGA_API_DIR}/mega/return_reason.hpp
    ${MEGA_API_DIR}/mega/ring_allocator.hpp
    ${MEGA_API_DIR}/mega/scheduler.hpp
    ${MEGA_API_DIR}/mega/snapshot.hpp
    ${MEGA_API_DIR}/mega/tree_traversal.hpp
    ${MEGA_API_DIR}/mega/tree_visitor.hpp
    ${MEGA_API_DIR}/mega/xml_archive.hpp

 )
 
add_library( mega_interface INTERFACE ${MEGA_LIBRARY} )
set_target_properties( mega_interface PROPERTIES FOLDER service )

install( FILES ${MEGA_LIBRARY} DESTINATION include/mega )

install( TARGETS mega_interface EXPORT mega_interface DESTINATION bin)
install( EXPORT mega_interface NAMESPACE MEGA:: FILE mega_interface-config.cmake DESTINATION share )