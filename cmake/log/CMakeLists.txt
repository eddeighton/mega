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

set( LOG_HEADERS
    ${MEGA_API_DIR}/log/offset.hpp
    ${MEGA_API_DIR}/log/filename.hpp
    ${MEGA_API_DIR}/log/log.hpp
    ${MEGA_API_DIR}/log/record.hpp
)

set( LOG_SOURCE
    ${MEGA_SRC_DIR}/log/log.cpp
    ${MEGA_SRC_DIR}/log/filename.cpp
)

add_library( log
    ${LOG_HEADERS} 
    ${LOG_SOURCE}
)

set_property(TARGET log PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories( log PUBLIC ${MEGA_API_DIR} )

link_boost( log system )
link_boost( log filesystem )
link_boost( log serialization )
link_common( log )

install( FILES ${LOG_HEADERS} DESTINATION include/log )
install( TARGETS log EXPORT log DESTINATION bin)
install( EXPORT log NAMESPACE MEGA:: FILE log-config.cmake DESTINATION share )