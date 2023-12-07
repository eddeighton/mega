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

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

#get the source code
set( IL_FRONTEND_API ${MEGA_API_DIR}/il/frontend )
set( IL_FRONTEND_SRC ${MEGA_SRC_DIR}/il/frontend )

set( IL_FRONTEND_HEADERS
		${IL_FRONTEND_API}/factory.hpp
	)

set( IL_FRONTEND_SOURCES
		${IL_FRONTEND_SRC}/factory.cpp
	)
	
add_library( frontend ${IL_FRONTEND_HEADERS} ${IL_FRONTEND_SOURCES} )

target_include_directories( frontend PUBLIC ${MEGA_API_DIR} )

target_precompile_headers( frontend REUSE_FROM mega_pch )

source_group( src FILES ${IL_FRONTEND_HEADERS} ${IL_FRONTEND_SOURCES} )

link_spdlog( frontend )
link_common( frontend )

install( FILES ${IL_FRONTEND_HEADERS} DESTINATION include/il/frontend )
install( TARGETS frontend EXPORT frontend DESTINATION bin)
install( EXPORT frontend NAMESPACE MEGA:: FILE frontend-config.cmake DESTINATION share )
