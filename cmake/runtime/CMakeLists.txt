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

#get clang
include( ${WORKSPACE_ROOT_PATH}/thirdparty/llvm/clang_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( RUNTIME_HEADERS

    ${MEGA_API_DIR}/runtime/clang.hpp
    #${MEGA_API_DIR}/runtime/component_manager.hpp
    ${MEGA_API_DIR}/runtime/exception.hpp
    ${MEGA_API_DIR}/runtime/function_cache.hpp
    ${MEGA_API_DIR}/runtime/function_provider.hpp
    ${MEGA_API_DIR}/runtime/function_ptr.hpp
    ${MEGA_API_DIR}/runtime/functor_dispatch.hpp
    ${MEGA_API_DIR}/runtime/mangle.hpp
    ${MEGA_API_DIR}/runtime/orc.hpp
    ${MEGA_API_DIR}/runtime/context.hpp

    ${MEGA_API_DIR}/runtime/functor_id.hxx
    ${MEGA_API_DIR}/runtime/functor_cpp.hxx


    # ${MEGA_API_DIR}/service/protocol/common/mpo_context_interface.hpp  
)


set( RUNTIME_SOURCE
    ${MEGA_SRC_DIR}/runtime/clang.cpp
    #${MEGA_SRC_DIR}/runtime/component_manager.cpp
    ${MEGA_SRC_DIR}/runtime/orc.cpp
    ${MEGA_SRC_DIR}/runtime/runtime.cpp
    ${MEGA_SRC_DIR}/runtime/context.cpp

    ${MEGA_SRC_DIR}/runtime/functor_dispatch.cxx
    ${MEGA_SRC_DIR}/runtime/functor_cpp.cxx
)

add_library( runtime
    ${RUNTIME_HEADERS}
    ${RUNTIME_SOURCE}
)

set_target_properties( runtime PROPERTIES FOLDER service )

include_directories( runtime ${MEGA_API_DIR}/runtime )

target_link_libraries( runtime database )
target_link_libraries( runtime invocation )
target_link_libraries( runtime frontend )
target_link_libraries( runtime backend )
target_link_libraries( runtime log )

# target_link_options( runtime PUBLIC -export-dynamic )
# set_property(TARGET runtime PROPERTY C_VISIBILITY_PRESET hidden)
# set_property(TARGET runtime PROPERTY VISIBILITY_INLINES_HIDDEN ON)

target_include_directories( runtime PUBLIC ${MEGA_API_DIR} )

link_boost( runtime system )
link_boost( runtime filesystem )
link_boost( runtime serialization )
link_json( runtime )
link_inja( runtime )
link_common( runtime )
link_orc( runtime )

install( FILES ${RUNTIME_HEADERS} DESTINATION include/runtime )
install( TARGETS runtime EXPORT runtime DESTINATION bin)
install( EXPORT runtime NAMESPACE MEGA:: FILE runtime-config.cmake DESTINATION share )
install( FILES ${JINJA_TEMPLATES} DESTINATION templates )
