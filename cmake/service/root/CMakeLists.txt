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

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )


#get the source code
set( ROOT_SOURCE_DIR ${MEGA_SRC_DIR}/service/root )

set( ROOT_SOURCE 
    ${ROOT_SOURCE_DIR}/mpo_manager.hpp
    ${ROOT_SOURCE_DIR}/root.hpp
    ${ROOT_SOURCE_DIR}/root.cpp
    ${ROOT_SOURCE_DIR}/pipeline.hpp
    ${ROOT_SOURCE_DIR}/pipeline.cpp
    ${ROOT_SOURCE_DIR}/job.hpp
    ${ROOT_SOURCE_DIR}/job.cpp
    ${ROOT_SOURCE_DIR}/simulation.hpp
    ${ROOT_SOURCE_DIR}/simulation.cpp
    ${ROOT_SOURCE_DIR}/main.cpp
    ${ROOT_SOURCE_DIR}/request.hpp
    ${ROOT_SOURCE_DIR}/routing.cpp
    ${ROOT_SOURCE_DIR}/project.cpp
    ${ROOT_SOURCE_DIR}/enrole.cpp
    ${ROOT_SOURCE_DIR}/status.cpp
    ${ROOT_SOURCE_DIR}/stash.cpp
 )

add_executable( root ${ROOT_SOURCE} )

IF( ${WIN32} )
target_compile_options( root PUBLIC "/await:strict" )
ELSE( ${WIN32} )
target_compile_options( root PUBLIC "-fcoroutines" )
ENDIF( ${WIN32} )

#target_compile_definitions( daemon PUBLIC -DMEGA_CONTEXT_IMPLEMENTOR )

target_link_libraries( root protocol )
target_link_libraries( root network )
target_link_libraries( root pipeline )

set_target_properties( root PROPERTIES FOLDER service )

link_boost( root program_options )
link_boost( root filesystem )
link_boost( root iostreams )
link_boost( root serialization )
link_boost( root system )
link_fmt( root )
link_spdlog( root )
link_common( root )

install( TARGETS root DESTINATION bin)
