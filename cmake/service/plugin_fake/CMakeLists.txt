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

set( PLUGIN_FAKE_SRC_DIR ${MEGA_SRC_DIR}/service/plugin )

####################################################################
####################################################################

set( PLUGIN_FAKE_API
    ${MEGA_API_DIR}/service/plugin/api.hpp 
    )

set( PLUGIN_FAKE_SOURCE 
    ${MEGA_SRC_DIR}/service/plugin_fake/plugin_fake.cpp 
    )

add_library( plugin_fake SHARED ${PLUGIN_FAKE_API} ${PLUGIN_FAKE_SOURCE} )

IF( ${WIN32} )
add_definitions(-DMEGA_PLUGIN_SHARED_MODULE)
set_property(TARGET plugin_fake PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS OFF )
target_compile_options( plugin_fake PUBLIC "/await:strict" )
target_compile_options( plugin_fake PRIVATE "/bigobj" )
ELSE( ${WIN32} )
target_compile_options( plugin_fake PUBLIC "-fcoroutines" )
target_link_options( plugin_fake PUBLIC -export-dynamic )
ENDIF( ${WIN32} )

target_link_libraries( plugin_fake executorLib )
target_link_libraries( plugin_fake mega_mangle )

set_target_properties( plugin_fake PROPERTIES FOLDER service )

link_boost( plugin_fake program_options )
link_boost( plugin_fake timer )
link_boost( plugin_fake filesystem )
link_boost( plugin_fake iostreams )
link_boost( plugin_fake serialization )
link_fmt( plugin_fake )
link_spdlog( plugin_fake )
link_json( plugin_fake )
link_inja( plugin_fake )
link_common( plugin_fake )

install( FILES ${PLUGIN_FAKE_API} DESTINATION include/service/plugin_fake )
install( TARGETS plugin_fake EXPORT plugin_fake DESTINATION bin)
install( EXPORT plugin_fake NAMESPACE MEGA:: FILE plugin_fake-config.cmake DESTINATION share )

