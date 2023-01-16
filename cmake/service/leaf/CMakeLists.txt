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

#get fmt
include( ${WORKSPACE_ROOT_PATH}/thirdparty/fmt/fmt_include.cmake )

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( MEGA_LEAF_API 
    ${MEGA_API_DIR}/service/cycle.hpp
    ${MEGA_API_DIR}/service/leaf.hpp
    ${MEGA_API_DIR}/service/lock_tracker.hpp
    ${MEGA_API_DIR}/service/memory_manager.hpp
    ${MEGA_API_DIR}/service/mpo_context.hpp
    ${MEGA_API_DIR}/service/remote_memory_manager.hpp
)

set( MEGA_LEAF_SRC 
    ${MEGA_SRC_DIR}/service/leaf/jit.cpp
    ${MEGA_SRC_DIR}/service/leaf/leaf.cpp
    ${MEGA_SRC_DIR}/service/leaf/llvm_compiler.cpp
    ${MEGA_SRC_DIR}/service/leaf/llvm_compiler.hpp
    ${MEGA_SRC_DIR}/service/leaf/memory.cpp
    ${MEGA_SRC_DIR}/service/leaf/mpo_context.cpp
    ${MEGA_SRC_DIR}/service/leaf/mpo_lifetime.cpp
    ${MEGA_SRC_DIR}/service/leaf/mpo_lifetime.hpp
    ${MEGA_SRC_DIR}/service/leaf/project.cpp
    ${MEGA_SRC_DIR}/service/leaf/request.hpp
    ${MEGA_SRC_DIR}/service/leaf/routing.cpp
    ${MEGA_SRC_DIR}/service/leaf/status.cpp
)

add_library( leaf
                    ${MEGA_LEAF_API}
					${MEGA_LEAF_SRC}
				)

IF( ${WIN32} )
target_compile_options( leaf PUBLIC "/await:strict" )
set_property(TARGET leaf PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON )
ELSE( ${WIN32} )
target_compile_options( leaf PUBLIC "-fcoroutines" )
ENDIF( ${WIN32} )

#target_compile_definitions( leaf PUBLIC -DMEGA_CONTEXT_IMPLEMENTOR )

target_link_libraries( leaf protocol )
target_link_libraries( leaf network )
target_link_libraries( leaf jit )
target_link_libraries( leaf log )

target_include_directories( leaf PUBLIC ${MEGA_API_DIR} )
target_include_directories( leaf PUBLIC ${MEGA_SRC_DIR} )

set_target_properties( leaf PROPERTIES FOLDER service )

link_boost( leaf filesystem )
link_boost( leaf system )
link_boost( leaf atomic )
link_boost( leaf coroutine )
link_boost( leaf serialization )

link_common( leaf )
link_fmt( leaf )
link_spdlog( leaf )

install( FILES ${MEGA_LEAF_API} DESTINATION include/service )

install( TARGETS leaf EXPORT leaf DESTINATION bin)
install( EXPORT leaf NAMESPACE MEGA:: FILE leaf-config.cmake DESTINATION share )
