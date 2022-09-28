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