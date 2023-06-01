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


#get flat buffers
include( ${WORKSPACE_ROOT_PATH}/thirdparty/flatbuffers/flatbuffers_include.cmake )

find_file( FLATBUFFERS_COMPILER NAMES flatc PATHS ${BUILD_PATH}/${CFG_TUPLE}/flatbuffers/install/bin REQUIRED NO_DEFAULT_PATH )

set( MAP_SCHEMA ${MEGA_SRC_DIR}/map/format.fbs )

add_custom_target( generate_map_format ALL
    COMMAND ${FLATBUFFERS_COMPILER}
        --cpp
        -o ${MEGA_SRC_DIR}/map/
         ${MAP_SCHEMA}
    DEPENDS ${MAP_SCHEMA}
    BYPRODUCTS ${MEGA_SRC_DIR}/map/format_generated.h
    COMMENT "Generating Map Format" 
    SOURCES ${MAP_SCHEMA}
    )

add_custom_target( map_format ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MEGA_SRC_DIR}/map/format_generated.h ${MEGA_API_DIR}/map/map_format.h
    
    DEPENDS ${MEGA_SRC_DIR}/map/format_generated.h
    BYPRODUCTS ${MEGA_API_DIR}/map/map_format.h
    COMMENT "Generating Map Format Header" 
    SOURCES ${MEGA_SRC_DIR}/map/format_generated.h
    )

add_dependencies( map_format generate_map_format )

install( FILES ${MEGA_API_DIR}/map/map_format.h DESTINATION include/map )
