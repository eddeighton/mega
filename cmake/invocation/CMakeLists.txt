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

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( INVOCATION_HEADERS
    ${MEGA_API_DIR}/invocation/invocation.hpp
)

set( INVOCATION_SOURCE
    ${MEGA_SRC_DIR}/invocation/invocation.cpp
    ${MEGA_SRC_DIR}/invocation/transitions.cpp
    ${MEGA_SRC_DIR}/invocation/events.cpp
)

add_library( invocation
    ${INVOCATION_HEADERS} 
    ${INVOCATION_SOURCE}
)

set_target_properties( invocation PROPERTIES FOLDER compiler )

target_link_libraries( invocation mega_values )
target_link_libraries( invocation database )

IF( ${WIN32} )
set_property(TARGET invocation PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON )
ELSE( ${WIN32} )
set_property(TARGET invocation PROPERTY POSITION_INDEPENDENT_CODE ON)
ENDIF( ${WIN32} )

target_include_directories( invocation PUBLIC ${MEGA_API_DIR} )

link_boost( invocation system )
link_boost( invocation filesystem )
link_boost( invocation serialization )
link_json( invocation )
link_common( invocation )

install( FILES ${INVOCATION_HEADERS} DESTINATION include/invocation )
install( TARGETS invocation EXPORT invocation DESTINATION bin)
install( EXPORT invocation NAMESPACE MEGA:: FILE invocation-config.cmake DESTINATION share )
