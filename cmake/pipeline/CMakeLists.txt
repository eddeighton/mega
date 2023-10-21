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

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( PIPELINE_HEADERS
    ${MEGA_API_DIR}/pipeline/build_hash_code.hpp
    ${MEGA_API_DIR}/pipeline/configuration.hpp
    ${MEGA_API_DIR}/pipeline/pipeline_result.hpp
    ${MEGA_API_DIR}/pipeline/pipeline.hpp
    ${MEGA_API_DIR}/pipeline/stash.hpp
    ${MEGA_API_DIR}/pipeline/task.hpp
    ${MEGA_API_DIR}/pipeline/version.hpp
)

set( PIPELINE_SOURCE
    ${MEGA_SRC_DIR}/pipeline/pipeline.cpp
)

add_library( pipeline
    ${PIPELINE_HEADERS} 
    ${PIPELINE_SOURCE}
)

set_target_properties( pipeline PROPERTIES FOLDER service )

IF( ${WIN32} )
target_compile_options(pipeline PRIVATE /bigobj)
set_property(TARGET pipeline PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON )
ELSE( ${WIN32} )
set_property(TARGET pipeline PROPERTY POSITION_INDEPENDENT_CODE ON)
ENDIF( ${WIN32} )

target_include_directories( pipeline PUBLIC ${MEGA_API_DIR} )

link_boost( pipeline system )
link_boost( pipeline filesystem )
link_boost( pipeline serialization )
link_common( pipeline )

install( FILES ${PIPELINE_HEADERS} DESTINATION include/pipeline )
install( TARGETS pipeline EXPORT pipeline DESTINATION bin)
install( EXPORT pipeline NAMESPACE MEGA:: FILE pipeline-config.cmake DESTINATION share )
