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

set( MEGA_NETWORK_API 
    ${MEGA_API_DIR}/service/network/client.hpp
    ${MEGA_API_DIR}/service/network/end_point.hpp
    ${MEGA_API_DIR}/service/network/log.hpp
    ${MEGA_API_DIR}/service/network/logical_thread_manager.hpp
    ${MEGA_API_DIR}/service/network/logical_thread.hpp
    ${MEGA_API_DIR}/service/network/network.hpp
    ${MEGA_API_DIR}/service/network/receiver_channel.hpp
    ${MEGA_API_DIR}/service/network/receiver.hpp
    ${MEGA_API_DIR}/service/network/sender_factory.hpp
    ${MEGA_API_DIR}/service/network/server.hpp
    ${MEGA_API_DIR}/service/network/status_printer.hpp
)

set( MEGA_NETWORK_SRC 
    ${MEGA_SRC_DIR}/service/network/client.cpp
    ${MEGA_SRC_DIR}/service/network/end_point.cpp
    ${MEGA_SRC_DIR}/service/network/log.cpp
    ${MEGA_SRC_DIR}/service/network/logical_thread_manager.cpp
    ${MEGA_SRC_DIR}/service/network/logical_thread.cpp
    ${MEGA_SRC_DIR}/service/network/network.cpp
    ${MEGA_SRC_DIR}/service/network/receiver.cpp
    ${MEGA_SRC_DIR}/service/network/sender_factory.cpp
    ${MEGA_SRC_DIR}/service/network/server.cpp
    ${MEGA_SRC_DIR}/service/network/status_printer.cpp
)

add_library( network
                    ${MEGA_NETWORK_API}
					${MEGA_NETWORK_SRC}
				)
target_precompile_headers( network REUSE_FROM mega_pch )

#IF( ${WIN32} )
#set_property(TARGET network PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON )
#ENDIF( ${WIN32} )

target_link_libraries( network protocol )

target_include_directories( network PUBLIC ${MEGA_API_DIR} )

set_target_properties( network PROPERTIES FOLDER service )
    
link_boost( network filesystem )
link_boost( network system )
link_boost( network atomic )
link_boost( network coroutine )
link_common( network )
link_fmt( network )
link_spdlog( network )

install( FILES ${MEGA_NETWORK_API} DESTINATION include/service/network )
install( TARGETS network EXPORT network DESTINATION bin)
install( EXPORT network NAMESPACE MEGA:: FILE network-config.cmake DESTINATION share )
