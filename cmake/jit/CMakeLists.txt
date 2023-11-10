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

#get fmt
include( ${WORKSPACE_ROOT_PATH}/thirdparty/fmt/fmt_include.cmake )

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

include( ../run_inja.cmake )

run_inja_template( ${MEGA_API_DIR}/jit/invocation_functions.json
                    ${MEGA_API_DIR}/jit/invocation_functions.hxx.jinja
                    ${MEGA_API_DIR}/jit/invocation_functions.hxx )
run_inja_template( ${MEGA_API_DIR}/jit/invocation_functions.json
                    ${MEGA_SRC_DIR}/jit/invocation_functions.cxx.jinja
                    ${MEGA_SRC_DIR}/jit/invocation_functions.cxx )

run_inja_template( ${MEGA_API_DIR}/jit/object_functions.json
                ${MEGA_API_DIR}/jit/object_functions.hxx.jinja
                ${MEGA_API_DIR}/jit/object_functions.hxx )
run_inja_template( ${MEGA_API_DIR}/jit/object_functions.json
                ${MEGA_SRC_DIR}/jit/object_functions.cxx.jinja
                ${MEGA_SRC_DIR}/jit/object_functions.cxx )

run_inja_template( ${MEGA_API_DIR}/jit/program_functions.json
                ${MEGA_API_DIR}/jit/program_functions.hxx.jinja
                ${MEGA_API_DIR}/jit/program_functions.hxx )
run_inja_template( ${MEGA_API_DIR}/jit/program_functions.json
                ${MEGA_SRC_DIR}/jit/program_functions.cxx.jinja
                ${MEGA_SRC_DIR}/jit/program_functions.cxx )

run_inja_template( ${MEGA_API_DIR}/jit/relation_functions.json
                ${MEGA_API_DIR}/jit/relation_functions.hxx.jinja
                ${MEGA_API_DIR}/jit/relation_functions.hxx )
run_inja_template( ${MEGA_API_DIR}/jit/relation_functions.json
                ${MEGA_SRC_DIR}/jit/relation_functions.cxx.jinja
                ${MEGA_SRC_DIR}/jit/relation_functions.cxx )

run_inja_template( ${MEGA_API_DIR}/jit/operator_functions.json
                ${MEGA_API_DIR}/jit/operator_functions.hxx.jinja
                ${MEGA_API_DIR}/jit/operator_functions.hxx )
run_inja_template( ${MEGA_API_DIR}/jit/operator_functions.json
                ${MEGA_SRC_DIR}/jit/operator_functions.cxx.jinja
                ${MEGA_SRC_DIR}/jit/operator_functions.cxx )

set( JINJA_TEMPLATES 
    ${MEGA_SRC_DIR}/jit/templates/allocator.jinja 
    ${MEGA_SRC_DIR}/jit/templates/program.jinja 
    ${MEGA_SRC_DIR}/jit/templates/relation.jinja 
    ${MEGA_SRC_DIR}/jit/templates/invocation.jinja 
)

set( RUNTIME_HEADERS
    ${MEGA_API_DIR}/jit/allocator.hpp
    ${MEGA_API_DIR}/jit/api.hpp
    ${MEGA_API_DIR}/jit/code_generator.hpp
    ${MEGA_API_DIR}/jit/component_manager.hpp
    ${MEGA_API_DIR}/jit/function_declarations.hpp
    ${MEGA_API_DIR}/jit/functions.hpp
    ${MEGA_API_DIR}/jit/indent.hpp
    ${MEGA_API_DIR}/jit/inja.hpp
    ${MEGA_API_DIR}/jit/invocation_functions.hxx
    ${MEGA_API_DIR}/jit/jit_exception.hpp
    ${MEGA_API_DIR}/jit/jit.hpp
    ${MEGA_API_DIR}/jit/object_functions.hxx
    ${MEGA_API_DIR}/jit/object_header.hpp
    ${MEGA_API_DIR}/jit/operator_functions.hxx
    ${MEGA_API_DIR}/jit/orc.hpp
    ${MEGA_API_DIR}/jit/orc_api.hpp
    ${MEGA_API_DIR}/jit/program_functions.hxx
    ${MEGA_API_DIR}/jit/program.hpp
    ${MEGA_API_DIR}/jit/relation_functions.hxx
    ${MEGA_API_DIR}/jit/relation.hpp
    ${MEGA_API_DIR}/jit/symbol.hpp

)

set( RUNTIME_SOURCE

    ${MEGA_SRC_DIR}/jit/symbol_utils.hpp
    ${MEGA_SRC_DIR}/jit/symbols.cpp

    ${MEGA_SRC_DIR}/jit/allocator.cpp
    ${MEGA_SRC_DIR}/jit/code_gen_allocator.cpp
    ${MEGA_SRC_DIR}/jit/code_gen_decisions.cpp
    ${MEGA_SRC_DIR}/jit/code_gen_instructions.cpp
    ${MEGA_SRC_DIR}/jit/code_gen_operators.cpp
    ${MEGA_SRC_DIR}/jit/code_generator.cpp
    ${MEGA_SRC_DIR}/jit/component_manager.cpp
    ${MEGA_SRC_DIR}/jit/invocation_functions.cxx
    ${MEGA_SRC_DIR}/jit/jit.cpp
    ${MEGA_SRC_DIR}/jit/object_functions.cxx
    ${MEGA_SRC_DIR}/jit/operator_functions.cxx
    ${MEGA_SRC_DIR}/jit/program_functions.cxx
    ${MEGA_SRC_DIR}/jit/program.cpp
    ${MEGA_SRC_DIR}/jit/relation_functions.cxx
    ${MEGA_SRC_DIR}/jit/relation.cpp
    
)

add_library( jit SHARED
    ${RUNTIME_HEADERS} 
    ${RUNTIME_SOURCE}
)

set_target_properties( jit PROPERTIES FOLDER service )

include_directories( jit ${MEGA_API_DIR}/jit )

target_link_libraries( jit database )
target_link_libraries( jit orc )
target_link_libraries( jit invocation )
target_link_libraries( jit protocol )

IF( ${WIN32} )
add_definitions(-DMEGA_JIT_API_SHARED_MODULE)
set_property(TARGET jit PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS OFF )
ELSE( ${WIN32} )
# target_link_options( jit PUBLIC -export-dynamic )
set_property(TARGET jit PROPERTY C_VISIBILITY_PRESET hidden)
set_property(TARGET jit PROPERTY VISIBILITY_INLINES_HIDDEN ON)
set_property(TARGET jit PROPERTY POSITION_INDEPENDENT_CODE ON)
ENDIF( ${WIN32} )

target_include_directories( jit PUBLIC ${MEGA_API_DIR} )

link_boost( jit system )
link_boost( jit filesystem )
link_boost( jit serialization )
link_json( jit )
link_inja( jit )
link_fmt( jit )
link_spdlog( jit )
link_common( jit )

install( FILES ${RUNTIME_HEADERS} DESTINATION include/jit )
install( TARGETS jit EXPORT jit DESTINATION bin)
install( EXPORT jit NAMESPACE MEGA:: FILE jit-config.cmake DESTINATION share )
install( FILES ${JINJA_TEMPLATES} DESTINATION templates )

###############################################################################
###############################################################################
###############################################################################

add_library( symbol_test SHARED
    ${MEGA_SRC_DIR}/jit/symbol_test.cpp
)

set_target_properties( symbol_test PROPERTIES FOLDER service )
target_include_directories( symbol_test PUBLIC ${MEGA_API_DIR} )

install( TARGETS symbol_test DESTINATION bin)

link_boost( symbol_test system )
link_common( symbol_test )