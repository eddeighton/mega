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


include( ../run_inja.cmake )

add_custom_target( generate_event_records
    COMMAND inja
        --data ${MEGA_API_DIR}/event/records.json
        --template ${MEGA_API_DIR}/event/records.hxx.jinja
        --output ${MEGA_API_DIR}/event/records.hxx
    DEPENDS ${MEGA_API_DIR}/event/records.json ${MEGA_API_DIR}/event/records.hxx.jinja inja
    BYPRODUCTS  ${MEGA_API_DIR}/event/records.hxx
    COMMENT "Generating Log Record types" 
    SOURCES ${MEGA_API_DIR}/event/records.json
)

set( EVENT_HEADERS
    ${MEGA_API_DIR}/event/buffer.hpp
    ${MEGA_API_DIR}/event/file_log.hpp
    ${MEGA_API_DIR}/event/filename.hpp
    ${MEGA_API_DIR}/event/index_record.hpp
    ${MEGA_API_DIR}/event/iterator.hpp
    ${MEGA_API_DIR}/event/memory_log.hpp
    ${MEGA_API_DIR}/event/offset.hpp
    ${MEGA_API_DIR}/event/range.hpp
    ${MEGA_API_DIR}/event/records.hxx
    ${MEGA_API_DIR}/event/storage.hpp
)

set( EVENT_SOURCE
    ${MEGA_SRC_DIR}/event/file_log.cpp
    ${MEGA_SRC_DIR}/event/filename.cpp
)

add_library( event
    ${EVENT_HEADERS} 
    ${EVENT_SOURCE}
)

set_target_properties( event PROPERTIES FOLDER service )

#IF( ${WIN32} )
#target_compile_options( event PUBLIC "/await:strict" )
#set_property(TARGET event PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON )
#ELSE( ${WIN32} )
#ENDIF( ${WIN32} )

target_include_directories( event PUBLIC ${MEGA_API_DIR} )

link_boost( event system )
link_boost( event filesystem )
link_boost( event serialization )
link_common( event )

install( FILES ${EVENT_HEADERS} DESTINATION include/event )
install( TARGETS event EXPORT event DESTINATION bin)
install( EXPORT event NAMESPACE MEGA:: FILE event-config.cmake DESTINATION share )
