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

set( REPORT_SOURCE 
    ${MEGA_SRC_DIR}/service/report/status.cpp 
    ${MEGA_SRC_DIR}/service/report/project.cpp 
    ${MEGA_SRC_DIR}/service/report/request.hpp 
    ${MEGA_SRC_DIR}/service/report/routing.cpp 
    ${MEGA_SRC_DIR}/service/report/report.hpp 
    ${MEGA_SRC_DIR}/service/report/report.cpp 
    ${MEGA_SRC_DIR}/service/report/http_logical_thread.hpp 
    ${MEGA_SRC_DIR}/service/report/http_logical_thread.cpp 
    ${MEGA_SRC_DIR}/service/report/http_server.cpp 
    ${MEGA_SRC_DIR}/service/report/main.cpp 
    )

add_executable( report ${REPORT_SOURCE} )

IF( ${WIN32} )
target_compile_options( report PUBLIC "/await:strict" )
target_compile_options( report PRIVATE "/bigobj" )
ELSE( ${WIN32} )
target_compile_options( report PUBLIC "-fcoroutines" )
target_link_options( report PUBLIC -export-dynamic )
ENDIF( ${WIN32} )

target_link_libraries( report executorLib )
target_link_libraries( report mega_mangle )
target_link_libraries( report reports )
target_link_libraries( report reporters )
target_link_libraries( report database_reporters )

set_target_properties( report PROPERTIES FOLDER service )

link_boost( report program_options )
link_boost( report timer )
link_boost( report filesystem )
link_boost( report iostreams )
link_boost( report serialization )
link_fmt( report )
link_spdlog( report )
link_json( report )
link_inja( report )
link_common( report )

install( TARGETS report DESTINATION bin)
