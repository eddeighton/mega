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

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

#get fmt
include( ${WORKSPACE_ROOT_PATH}/thirdparty/fmt/fmt_include.cmake )

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

set( LOG_HEADERS
    ${MEGA_API_DIR}/log/log.hpp
)

set( LOG_SOURCE
    ${MEGA_SRC_DIR}/log/log.cpp
)

add_library( log
    ${LOG_HEADERS} 
    ${LOG_SOURCE}
)

target_link_libraries( log mega_values )

set_target_properties( log PROPERTIES FOLDER service )

target_include_directories( log PUBLIC ${MEGA_API_DIR} )

link_boost( log system )
link_boost( log filesystem )
link_boost( log serialization )
link_common( log )
link_fmt( log )
link_spdlog( log )

install( FILES ${EVENT_HEADERS} DESTINATION include/log )
install( TARGETS log EXPORT log DESTINATION bin)
install( EXPORT log NAMESPACE MEGA:: FILE log-config.cmake DESTINATION share )
