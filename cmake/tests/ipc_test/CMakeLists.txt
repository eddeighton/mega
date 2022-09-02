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