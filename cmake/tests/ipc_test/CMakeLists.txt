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



cmake_minimum_required(VERSION 3.2)

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

include_directories( ${MEGA_API_DIR} )
include_directories( ${MEGA_SRC_DIR} )

set( IPC_TEST_DIR ${MEGA_TEST_DIR}/ipc_test )

#################################################
#################################################
## ipc_test
add_executable( ipc_test ${IPC_TEST_DIR}/main.cpp )

# allow access to source folder for tests
target_include_directories(ipc_test PUBLIC ${MEGA_SRC_DIR})

set_target_properties( ipc_test PROPERTIES FOLDER mega )

link_boost( ipc_test filesystem )
link_boost( ipc_test system )
link_boost( ipc_test date_time )
link_boost( ipc_test program_options )
link_boost( ipc_test iostreams )
link_boost( ipc_test atomic )

link_common( ipc_test )

install( TARGETS ipc_test DESTINATION bin )

#################################################
#################################################
## ipc_client
add_executable( ipc_client ${IPC_TEST_DIR}/client.cpp )

# allow access to source folder for tests
target_include_directories(ipc_client PUBLIC ${MEGA_SRC_DIR})

set_target_properties( ipc_client PROPERTIES FOLDER mega )

link_boost( ipc_client filesystem )
link_boost( ipc_client system )
link_boost( ipc_client date_time )
link_boost( ipc_client program_options )
link_boost( ipc_client iostreams )
link_boost( ipc_client atomic )

link_common( ipc_client )

install( TARGETS ipc_client DESTINATION bin )