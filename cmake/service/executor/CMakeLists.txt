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

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get inja
include( ${WORKSPACE_ROOT_PATH}/thirdparty/inja/inja_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( EXECUTOR_SRC_DIR ${MEGA_SRC_DIR}/service/executor )

####################################################################
####################################################################

set( EXECUTOR_LIB_API 
    ${MEGA_API_DIR}/service/executor.hpp 
    ${MEGA_API_DIR}/service/state_machine.hpp 
    )

set( EXECUTOR_LIB_SRC 
    ${EXECUTOR_SRC_DIR}/request.hpp
    ${EXECUTOR_SRC_DIR}/request.cpp
    ${EXECUTOR_SRC_DIR}/job.hpp
    ${EXECUTOR_SRC_DIR}/job.cpp
    ${EXECUTOR_SRC_DIR}/simulation.hpp
    ${EXECUTOR_SRC_DIR}/simulation.cpp
    ${EXECUTOR_SRC_DIR}/scheduler.hpp
    ${EXECUTOR_SRC_DIR}/scheduler.cpp
    ${EXECUTOR_SRC_DIR}/clock.hpp
    ${EXECUTOR_SRC_DIR}/clock.cpp
    ${EXECUTOR_SRC_DIR}/executor.cpp
    ${EXECUTOR_SRC_DIR}/routing.cpp
    ${EXECUTOR_SRC_DIR}/status.cpp
)

add_library( executorLib
                    ${EXECUTOR_LIB_API}
					${EXECUTOR_LIB_SRC}
				)

target_compile_options( executorLib PUBLIC "-fcoroutines" )
# target_link_options( executorLib PUBLIC -export-dynamic )

target_link_libraries( executorLib protocol )
target_link_libraries( executorLib network )

target_include_directories( executorLib PUBLIC ${MEGA_API_DIR} )
target_include_directories( executorLib PUBLIC ${MEGA_SRC_DIR} )

set_target_properties( executorLib PROPERTIES FOLDER executorLib )

link_boost( executorLib filesystem )
link_boost( executorLib system )
link_boost( executorLib atomic )
link_boost( executorLib coroutine )
link_boost( executorLib serialization )
link_common( executorLib )
link_spdlog( executorLib )

target_link_libraries( executorLib compiler_config )
target_link_libraries( executorLib protocol )
target_link_libraries( executorLib network )
target_link_libraries( executorLib leaf )
target_link_libraries( executorLib pipeline )
target_link_libraries( executorLib database )
target_link_libraries( executorLib orc )
target_link_libraries( executorLib log )

install( FILES ${EXECUTOR_LIB_API} DESTINATION include/service )
install( TARGETS executorLib EXPORT executorLib DESTINATION bin)
install( EXPORT executorLib NAMESPACE MEGA:: FILE executorLib-config.cmake DESTINATION share )

####################################################################
####################################################################

set( EXECUTOR_SOURCE 
    ${MEGA_SRC_DIR}/service/executor/main.cpp 

    ${MEGA_API_DIR}/mega/default_traits.cpp
    )

add_executable( executor ${EXECUTOR_SOURCE}  )

target_compile_options( executor PUBLIC "-fcoroutines" )

target_link_libraries( executor executorLib )

# target_link_libraries( executor 
# -Wl,-whole-archive
# runtime
# -Wl,-no-whole-archive
# )

set_target_properties( executor PROPERTIES FOLDER driver )

link_boost( executor atomic )
link_boost( executor program_options )
link_boost( executor timer )
link_boost( executor filesystem )
link_boost( executor iostreams )
link_boost( executor serialization )
link_spdlog( executor )
link_json( executor )
link_inja( executor )
link_common( executor )

install( TARGETS executor DESTINATION bin)
