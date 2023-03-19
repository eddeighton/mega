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

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get inja
include( ${WORKSPACE_ROOT_PATH}/thirdparty/inja/inja_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( PLUGIN_SRC_DIR ${MEGA_SRC_DIR}/service/plugin )

####################################################################
####################################################################

set( PLUGIN_API
    ${MEGA_API_DIR}/service/plugin/plugin.hpp 
    )

set( PLUGIN_SOURCE 
    ${MEGA_SRC_DIR}/service/plugin/plugin.cpp 
    ${MEGA_SRC_DIR}/service/plugin/platform.hpp 
    ${MEGA_SRC_DIR}/service/plugin/platform.cpp 
    ${MEGA_SRC_DIR}/service/plugin/player_network.hpp 
    ${MEGA_SRC_DIR}/service/plugin/player_network.cpp 
    )

add_library( plugin SHARED ${PLUGIN_API} ${PLUGIN_SOURCE} )

IF( ${WIN32} )
target_compile_options( plugin PUBLIC "/await:strict" )
target_compile_options( plugin PRIVATE "/bigobj" )
ELSE( ${WIN32} )
target_compile_options( plugin PUBLIC "-fcoroutines" )
target_link_options( plugin PUBLIC -export-dynamic )
ENDIF( ${WIN32} )

target_link_libraries( plugin executorLib )
target_link_libraries( plugin mega_mangle )

set_target_properties( plugin PROPERTIES FOLDER service )

link_boost( plugin atomic )
link_boost( plugin program_options )
link_boost( plugin timer )
link_boost( plugin filesystem )
link_boost( plugin iostreams )
link_boost( plugin serialization )
link_fmt( plugin )
link_spdlog( plugin )
link_json( plugin )
link_inja( plugin )
link_common( plugin )

install( FILES ${PLUGIN_API} DESTINATION include/service/plugin )
install( TARGETS plugin EXPORT plugin DESTINATION bin)
install( EXPORT plugin NAMESPACE MEGA:: FILE plugin-config.cmake DESTINATION share )


####################################################################
####################################################################


set( PLUGIN_TEST_SOURCE 
    ${MEGA_SRC_DIR}/service/plugin/main.cpp 
    )

add_executable( plugin_test ${PLUGIN_TEST_SOURCE}  )

IF( ${WIN32} )
target_compile_options( plugin_test PUBLIC "/await:strict" )
target_compile_options( plugin_test PRIVATE "/bigobj" )
ELSE( ${WIN32} )
target_compile_options( plugin_test PUBLIC "-fcoroutines" )
target_link_options( plugin_test PUBLIC -export-dynamic )
ENDIF( ${WIN32} )

target_link_libraries( plugin_test plugin )
target_link_libraries( plugin_test mega_mangle )

set_target_properties( plugin_test PROPERTIES FOLDER service )

link_boost( plugin_test atomic )
link_boost( plugin_test program_options )
link_boost( plugin_test timer )
link_boost( plugin_test filesystem )
link_boost( plugin_test iostreams )
link_boost( plugin_test serialization )
link_fmt( plugin_test )
link_spdlog( plugin_test )
link_json( plugin_test )
link_inja( plugin_test )
link_common( plugin_test )

install( TARGETS plugin_test DESTINATION bin)
