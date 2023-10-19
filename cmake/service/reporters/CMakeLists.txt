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

set( REPORTERS_SRC_DIR ${MEGA_SRC_DIR}/service/reporters )

####################################################################
####################################################################

set( REPORTERS_LIB_API 
    ${MEGA_API_DIR}/service/reporters.hpp
    )

set( REPORTERS_LIB_SRC 
    ${REPORTERS_SRC_DIR}/memory.cpp
)

add_library( reportersLib
                    ${REPORTERS_LIB_API}
					${REPORTERS_LIB_SRC}
				)

IF( ${WIN32} )
target_compile_options( reportersLib PUBLIC "/await:strict" )
target_compile_options( reportersLib PRIVATE "/bigobj" )
set_property(TARGET reportersLib PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON )
ELSE( ${WIN32} )
target_compile_options( reportersLib PUBLIC "-fcoroutines" )
ENDIF( ${WIN32} )

target_include_directories( reportersLib PUBLIC ${MEGA_API_DIR} )
target_include_directories( reportersLib PUBLIC ${MEGA_SRC_DIR} )

set_target_properties( reportersLib PROPERTIES FOLDER service )

link_boost( reportersLib filesystem )
link_boost( reportersLib system )
link_boost( reportersLib atomic )
link_boost( reportersLib coroutine )
link_boost( reportersLib serialization )
link_common( reportersLib )
link_fmt( reportersLib )
link_spdlog( reportersLib )

target_link_libraries( reportersLib compiler_config )
target_link_libraries( reportersLib reports )
target_link_libraries( reportersLib protocol )
target_link_libraries( reportersLib network )
target_link_libraries( reportersLib pipeline )
target_link_libraries( reportersLib database )
target_link_libraries( reportersLib orc )
target_link_libraries( reportersLib log )

install( FILES ${REPORTERS_LIB_API} DESTINATION include/service )
install( TARGETS reportersLib EXPORT reportersLib DESTINATION bin)
install( EXPORT reportersLib NAMESPACE MEGA:: FILE reportersLib-config.cmake DESTINATION share )
