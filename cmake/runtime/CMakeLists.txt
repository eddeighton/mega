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

set( RUNTIME_HEADERS
    ${MEGA_API_DIR}/runtime/runtime.hpp
    ${MEGA_API_DIR}/runtime/exception.hpp
    ${MEGA_API_DIR}/runtime/mangle.hpp
    ${MEGA_API_DIR}/runtime/llvm.hpp
    ${MEGA_API_DIR}/runtime/function_ptr.hpp
)

set( RUNTIME_FUNCTIONS_HEADERS
    ${MEGA_API_DIR}/runtime/functions/invocations.hxx
    ${MEGA_API_DIR}/runtime/functions/operators.hxx
)

set( RUNTIME_SOURCE
    ${MEGA_SRC_DIR}/runtime/runtime.cpp
    
    ${MEGA_SRC_DIR}/runtime/functions/invocations.cxx
    ${MEGA_SRC_DIR}/runtime/functions/operators.cxx
)

add_library( runtime SHARED
    ${RUNTIME_HEADERS}
    ${RUNTIME_FUNCTIONS_HEADERS}
    ${RUNTIME_SOURCE}
)
target_precompile_headers( runtime REUSE_FROM mega_pch )

set_target_properties( runtime PROPERTIES FOLDER service )

include_directories( runtime ${MEGA_API_DIR}/runtime )

target_link_libraries( runtime database )
#target_link_libraries( runtime orc )
target_link_libraries( runtime invocation )
target_link_libraries( runtime protocol )

# target_link_options( runtime PUBLIC -export-dynamic )
set_property(TARGET runtime PROPERTY C_VISIBILITY_PRESET hidden)
set_property(TARGET runtime PROPERTY VISIBILITY_INLINES_HIDDEN ON)

target_include_directories( runtime PUBLIC ${MEGA_API_DIR} )

link_boost( runtime system )
link_boost( runtime filesystem )
link_boost( runtime serialization )
link_json( runtime )
link_inja( runtime )
link_fmt( runtime )
link_spdlog( runtime )
link_common( runtime )

install( FILES ${RUNTIME_HEADERS} DESTINATION include/runtime )
install( FILES ${RUNTIME_FUNCTIONS_HEADERS} DESTINATION include/runtime/functions )
install( TARGETS runtime EXPORT runtime DESTINATION bin)
install( EXPORT runtime NAMESPACE MEGA:: FILE runtime-config.cmake DESTINATION share )
install( FILES ${JINJA_TEMPLATES} DESTINATION templates )
