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

#get pybind11
#include( ${WORKSPACE_ROOT_PATH}/thirdparty/pybind11/pybind11_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

include( ../run_inja.cmake )

add_custom_target( generate_mangle_rtti
    COMMAND inja
        --data ${MEGA_API_DIR}/mega/types/types.json
        --template ${MEGA_API_DIR}/mega/types/rtti.hxx.jinja
        --output ${MEGA_API_DIR}/mega/types/rtti.hxx
    DEPENDS ${MEGA_API_DIR}/mega/types/types.json ${MEGA_API_DIR}/mega/types/rtti.hxx.jinja inja
    BYPRODUCTS ${MEGA_API_DIR}/mega/types/rtti.hxx
    COMMENT "Generating rtti for Types" 
    SOURCES ${MEGA_API_DIR}/mega/types/types.json
)
add_dependencies( generate_mangle_rtti inja )

add_custom_target( generate_any_impl
    COMMAND inja
        --data ${MEGA_API_DIR}/mega/types/types.json
        --template ${MEGA_API_DIR}/mega/types/any.cxx.jinja
        --output ${MEGA_API_DIR}/mega/types/any.cxx
    DEPENDS ${MEGA_API_DIR}/mega/types/types.json ${MEGA_API_DIR}/mega/types/any.cxx.jinja inja
    BYPRODUCTS ${MEGA_API_DIR}/mega/types/any.cxx
    COMMENT "Generating any impl for Types" 
    SOURCES ${MEGA_API_DIR}/mega/types/types.json
)
add_dependencies( generate_any_impl inja )

add_custom_target( generate_mangle_types
    COMMAND inja
        --data ${MEGA_API_DIR}/mega/types/types.json
        --template ${MEGA_API_DIR}/mega/types/mangle.cxx.jinja
        --output ${MEGA_API_DIR}/mega/types/mangle_types.cxx
    DEPENDS ${MEGA_API_DIR}/mega/types/types.json ${MEGA_API_DIR}/mega/types/mangle.cxx.jinja inja
    BYPRODUCTS  ${MEGA_API_DIR}/mega/types/mangle_types.cxx
    COMMENT "Generating Type Mangle for Types" 
    SOURCES ${MEGA_API_DIR}/mega/types/types.json
)
add_dependencies( generate_mangle_types inja )

# add_custom_target( generate_mangle_allocators
#     COMMAND inja
#         --data ${MEGA_API_DIR}/mega/types/allocators.json
#         --template ${MEGA_API_DIR}/mega/types/mangle.cxx.jinja
#         --output ${MEGA_API_DIR}/mega/types/mangle_allocators.cxx
#     DEPENDS ${MEGA_API_DIR}/mega/types/types.json ${MEGA_API_DIR}/mega/types/mangle.cxx.jinja inja
#     BYPRODUCTS  ${MEGA_API_DIR}/mega/types/mangle_allocators.cxx
#     COMMENT "Generating Type Mangle for Allocators" 
#     SOURCES ${MEGA_API_DIR}/mega/types/allocators.json
# )
# add_dependencies( generate_mangle_allocators inja )

add_custom_target( generate_python_mangle_types
    COMMAND inja
        --data ${MEGA_API_DIR}/mega/types/types.json
        --template ${MEGA_API_DIR}/mega/types/python_mangle.cxx.jinja
        --output ${MEGA_API_DIR}/mega/types/python_mangle_types.cxx
    DEPENDS ${MEGA_API_DIR}/mega/types/types.json ${MEGA_API_DIR}/mega/types/python_mangle.cxx.jinja inja
    BYPRODUCTS  ${MEGA_API_DIR}/mega/types/python_mangle_types.cxx
    COMMENT "Generating Python Type Mangle for Types" 
    SOURCES ${MEGA_API_DIR}/mega/types/types.json
)
add_dependencies( generate_python_mangle_types inja )

# add_custom_target( generate_python_mangle_allocators
#     COMMAND inja
#         --data ${MEGA_API_DIR}/mega/types/allocators.json
#         --template ${MEGA_API_DIR}/mega/types/python_mangle.cxx.jinja
#         --output ${MEGA_API_DIR}/mega/types/python_mangle_allocators.cxx
#     DEPENDS ${MEGA_API_DIR}/mega/types/types.json ${MEGA_API_DIR}/mega/types/python_mangle.cxx.jinja inja
#     BYPRODUCTS  ${MEGA_API_DIR}/mega/types/python_mangle_allocators.cxx
#     COMMENT "Generating Python Type Mangle for Allocators" 
#     SOURCES ${MEGA_API_DIR}/mega/types/allocators.json
# )
# add_dependencies( generate_python_mangle_allocators inja )


add_custom_target( generate_mega_mangle_meta
    COMMAND inja
        --data ${MEGA_API_DIR}/mega/types/types.json
        --template ${MEGA_API_DIR}/mega/types/mega_mangle_meta.hxx.jinja
        --output ${MEGA_SRC_DIR}/compiler/mega_mangle_meta.hxx
    DEPENDS ${MEGA_API_DIR}/mega/types/types.json ${MEGA_API_DIR}/mega/types/mega_mangle_meta.hxx.jinja inja
    BYPRODUCTS  ${MEGA_SRC_DIR}/compiler/mega_mangle_meta.hxx
    COMMENT "Generating Mega Mangle Meta" 
    SOURCES ${MEGA_API_DIR}/mega/types/types.json
)
add_dependencies( generate_mega_mangle_meta inja )

set_target_properties( generate_any_impl PROPERTIES FOLDER service )
set_target_properties( generate_mangle_rtti PROPERTIES FOLDER service )
set_target_properties( generate_mangle_types PROPERTIES FOLDER service )
set_target_properties( generate_python_mangle_types PROPERTIES FOLDER service )
# set_target_properties( generate_mangle_allocators PROPERTIES FOLDER service )
# set_target_properties( generate_python_mangle_allocators PROPERTIES FOLDER service )
set_target_properties( generate_mega_mangle_meta PROPERTIES FOLDER service )

set( MEGA_LIBRARY
    ${MEGA_API_DIR}/mega/address_table.hpp
    ${MEGA_API_DIR}/mega/allocator.hpp
    ${MEGA_API_DIR}/mega/allocator_io.hpp
    ${MEGA_API_DIR}/mega/any.hpp
    ${MEGA_API_DIR}/mega/any_io.hpp
    ${MEGA_API_DIR}/mega/archive_traits.hpp
    ${MEGA_API_DIR}/mega/backtrace.hpp
    ${MEGA_API_DIR}/mega/bin_archive.hpp
    ${MEGA_API_DIR}/mega/boost_serialization_workaround.hpp
    ${MEGA_API_DIR}/mega/boost_serialization_xml_workaround.hpp
    ${MEGA_API_DIR}/mega/clock.hpp
    ${MEGA_API_DIR}/mega/common_strings.hpp 
    ${MEGA_API_DIR}/mega/coroutine.hpp
    ${MEGA_API_DIR}/mega/defaults.hpp
    ${MEGA_API_DIR}/mega/eg_common_strings.hpp
    ${MEGA_API_DIR}/mega/event.hpp
    ${MEGA_API_DIR}/mega/include.hpp
    ${MEGA_API_DIR}/mega/invocation_id.hpp
    ${MEGA_API_DIR}/mega/invocation_io.hpp
    ${MEGA_API_DIR}/mega/iterator.hpp
    ${MEGA_API_DIR}/mega/iterators.hpp
    ${MEGA_API_DIR}/mega/json_io.hpp
    ${MEGA_API_DIR}/mega/logical_reference.hpp
    ${MEGA_API_DIR}/mega/logical_tree.hpp
    ${MEGA_API_DIR}/mega/macros.hpp
    ${MEGA_API_DIR}/mega/maths_types_io.hpp
    ${MEGA_API_DIR}/mega/maths_types.hpp
    ${MEGA_API_DIR}/mega/memory.hpp
    ${MEGA_API_DIR}/mega/mpo.hpp
    ${MEGA_API_DIR}/mega/make_unique_without_reorder.hpp
    ${MEGA_API_DIR}/mega/native_types_io.hpp
    ${MEGA_API_DIR}/mega/native_types.hpp
    ${MEGA_API_DIR}/mega/operation_id.hpp 
    ${MEGA_API_DIR}/mega/operations.hpp
    ${MEGA_API_DIR}/mega/printer.hpp
    ${MEGA_API_DIR}/mega/record_archive.hpp
    ${MEGA_API_DIR}/mega/reference_io.hpp
    ${MEGA_API_DIR}/mega/reference_limits.hpp
    ${MEGA_API_DIR}/mega/reference.hpp
    ${MEGA_API_DIR}/mega/reference_tree.hpp
    ${MEGA_API_DIR}/mega/relation_id.hpp
    ${MEGA_API_DIR}/mega/relation_io.hpp
    ${MEGA_API_DIR}/mega/result_type.hpp
    ${MEGA_API_DIR}/mega/resumption.hpp
    ${MEGA_API_DIR}/mega/return_reason.hpp
    ${MEGA_API_DIR}/mega/scheduler.hpp
    ${MEGA_API_DIR}/mega/snapshot.hpp
    ${MEGA_API_DIR}/mega/tag_parser.hpp
    ${MEGA_API_DIR}/mega/tree_traversal.hpp
    ${MEGA_API_DIR}/mega/tree_visitor.hpp
    ${MEGA_API_DIR}/mega/type_id_io.hpp
    ${MEGA_API_DIR}/mega/type_id_limits.hpp
    ${MEGA_API_DIR}/mega/type_id.hpp
    ${MEGA_API_DIR}/mega/type_instance.hpp
    ${MEGA_API_DIR}/mega/type_path.hpp
    ${MEGA_API_DIR}/mega/variant.hpp
    ${MEGA_API_DIR}/mega/write_operation.hpp
    ${MEGA_API_DIR}/mega/xml_archive.hpp
)

add_library( mega_interface INTERFACE ${MEGA_LIBRARY} ${MEGA_TYPES} )
set_target_properties( mega_interface PROPERTIES FOLDER service )
add_dependencies( mega_interface generate_any_impl )
add_dependencies( mega_interface generate_mangle_rtti )
add_dependencies( mega_interface generate_mangle_types )
add_dependencies( mega_interface generate_python_mangle_types )
# add_dependencies( mega_interface generate_mangle_allocators )
# add_dependencies( mega_interface generate_python_mangle_allocators )

set( MEGA_TYPES
    ${MEGA_API_DIR}/mega/types/traits.hpp
    ${MEGA_API_DIR}/mega/types/traits.cpp
    ${MEGA_API_DIR}/mega/types/any.cpp
    ${MEGA_API_DIR}/mega/types/any.cxx
    ${MEGA_API_DIR}/mega/types/mangle_types.cxx
    #${MEGA_API_DIR}/mega/types/mangle_allocators.cxx

    #${MEGA_API_DIR}/mega/types/python_mangle.hpp
    #${MEGA_API_DIR}/mega/types/python_mangle_types.cxx
    #${MEGA_API_DIR}/mega/types/python_mangle_allocators.cxx
    
    ${MEGA_SRC_DIR}/jit/tag_parser.cpp
)

add_library( mega_mangle SHARED ${MEGA_TYPES} )

set_target_properties( mega_mangle PROPERTIES FOLDER service )

IF( ${WIN32} )
target_compile_options( mega_mangle PRIVATE /bigobj)
# for windows need to work around limit of 65K symbols - /Ob2 appears to achieve this
# target_compile_options(database PRIVATE /Ob2)
# set_property(TARGET database PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS OFF )
set_property(TARGET mega_mangle PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON )

target_link_libraries( mega_mangle protocol )

ELSE( ${WIN32} )
target_link_options( mega_mangle PUBLIC -export-dynamic )
ENDIF( ${WIN32} )

target_include_directories( mega_mangle PUBLIC ${MEGA_API_DIR} )

target_link_libraries( mega_mangle mega_interface )
target_link_libraries( mega_mangle log )

link_boost( mega_mangle system )
link_boost( mega_mangle filesystem )
link_boost( mega_mangle serialization )
link_json( mega_mangle )
link_common( mega_mangle )
#link_pybind11( mega_mangle )

install( TARGETS mega_interface EXPORT mega_interface DESTINATION bin)
install( EXPORT mega_interface NAMESPACE MEGA:: FILE mega_interface-config.cmake DESTINATION share )

install( TARGETS mega_mangle EXPORT mega_mangle DESTINATION bin)
install( EXPORT mega_mangle NAMESPACE MEGA:: FILE mega_mangle-config.cmake DESTINATION share )

install( FILES ${MEGA_LIBRARY} DESTINATION include/mega )
install( FILES ${MEGA_TYPES} DESTINATION include/mega/types )