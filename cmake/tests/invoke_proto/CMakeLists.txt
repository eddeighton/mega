

cmake_minimum_required(VERSION 3.2)

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get gtest
include( ${WORKSPACE_ROOT_PATH}/thirdparty/gtest/gtest_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

include_directories( ${MEGA_API_DIR} )
include_directories( ${MEGA_SRC_DIR} )

set( INVOKE_PROTO_DIR ${MEGA_TEST_DIR}/invoke_proto )

add_executable( invoke_proto 
    ${INVOKE_PROTO_DIR}/test.cpp 
    ${INVOKE_PROTO_DIR}/main.cpp 
)

# allow access to source folder for tests
target_include_directories(invoke_proto PUBLIC ${MEGA_SRC_DIR})

set_target_properties( invoke_proto PROPERTIES FOLDER mega )

link_boost( invoke_proto filesystem )
link_boost( invoke_proto system )
link_boost( invoke_proto date_time )
link_boost( invoke_proto program_options )
link_boost( invoke_proto iostreams )
link_boost( invoke_proto atomic )
#link_gtest( invoke_proto )

link_common( invoke_proto )

#target_link_libraries( invoke_proto database )
#target_link_libraries( invoke_proto utilities )
#target_link_libraries( invoke_proto database_compiler)
#target_link_libraries( invoke_proto protocol)
#target_link_libraries( invoke_proto network)
#target_link_libraries( invoke_proto pipeline)

install( TARGETS invoke_proto DESTINATION bin )