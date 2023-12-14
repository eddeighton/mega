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


set( IL_GEN_API

    ${MEGA_API_DIR}/il/backend/backend.hpp
)

set( IL_GEN_SRC

    ${MEGA_SRC_DIR}/il/backend/cpp.cpp
    ${MEGA_SRC_DIR}/il/backend/ir.cpp
)

add_library( backend
    ${IL_GEN_API}
    ${IL_GEN_SRC} 
)

target_include_directories( backend PUBLIC ${MEGA_API_DIR})

target_precompile_headers( backend REUSE_FROM mega_pch )

link_boost( backend system )
link_boost( backend filesystem )
link_boost( backend serialization )
link_json( backend )
link_inja( backend )
link_common( backend )
link_orc( backend )

#################################################################
#################################################################
## install everything
install( FILES ${IL_GEN_API} DESTINATION include/il/backend )
install( TARGETS backend EXPORT backend DESTINATION bin)
install( EXPORT backend NAMESPACE MEGA:: FILE backend-config.cmake DESTINATION share )

install( TARGETS backend DESTINATION bin)

