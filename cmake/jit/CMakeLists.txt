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

#get clang
include( ${WORKSPACE_ROOT_PATH}/thirdparty/llvm/clang_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( RUNTIME_SOURCE
    ${MEGA_SRC_DIR}/runtime/jit.hpp
    ${MEGA_SRC_DIR}/runtime/jit.cpp
)

add_library( jit SHARED
    ${RUNTIME_HEADERS} 
    ${RUNTIME_SOURCE}
)

#target_link_options( jit PUBLIC -export-dynamic )
#set_property(TARGET jit PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories( jit PUBLIC ${MEGA_API_DIR} )

link_boost( jit system )
link_common( jit )
link_orc( jit )

install( FILES ${RUNTIME_HEADERS} DESTINATION include/runtime )
install( TARGETS jit EXPORT jit DESTINATION bin)
install( EXPORT jit NAMESPACE MEGA:: FILE jit-config.cmake DESTINATION share )
