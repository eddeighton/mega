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


#get the source code
set( DAEMON_SOURCE_DIR ${MEGA_SRC_DIR}/service/daemon )

set( DAEMON_SOURCE 
    ${DAEMON_SOURCE_DIR}/daemon.hpp
    ${DAEMON_SOURCE_DIR}/daemon.cpp
    ${DAEMON_SOURCE_DIR}/main.cpp 
    ${DAEMON_SOURCE_DIR}/request.hpp
    ${DAEMON_SOURCE_DIR}/routing.cpp
    ${DAEMON_SOURCE_DIR}/enrole.cpp
    ${DAEMON_SOURCE_DIR}/status.cpp
    ${DAEMON_SOURCE_DIR}/memory.cpp
    ${DAEMON_SOURCE_DIR}/project.cpp
    )

add_executable( daemon ${DAEMON_HEADERS} ${DAEMON_SOURCE}  )
 
IF( ${WIN32} )
target_compile_options( daemon PUBLIC "/await:strict" )
ELSE( ${WIN32} )
target_compile_options( daemon PUBLIC "-fcoroutines" )
ENDIF( ${WIN32} )

target_link_libraries( daemon utilities )
target_link_libraries( daemon protocol )
target_link_libraries( daemon network )

set_target_properties( daemon PROPERTIES FOLDER service )

link_boost( daemon program_options )
link_boost( daemon filesystem )
link_boost( daemon system )
link_fmt( daemon )
link_spdlog( daemon )
link_common( daemon )

install( TARGETS daemon DESTINATION bin)
