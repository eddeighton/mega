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

#get inja
include( ${WORKSPACE_ROOT_PATH}/thirdparty/inja/inja_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

include_directories( ${MEGA_API_DIR} )
include_directories( ${MEGA_SRC_DIR} )

set( MEGA_PROTOCOL_COMPILER
    ${MEGA_SRC_DIR}/service/protocol/compiler/generator.hpp
    ${MEGA_SRC_DIR}/service/protocol/compiler/generator.cpp
    ${MEGA_SRC_DIR}/service/protocol/compiler/grammar.hpp
    ${MEGA_SRC_DIR}/service/protocol/compiler/grammar.cpp
    ${MEGA_SRC_DIR}/service/protocol/compiler/json_converter.hpp
    ${MEGA_SRC_DIR}/service/protocol/compiler/json_converter.cpp
	)
	
add_library( protocol_compiler
                    ${MEGA_PROTOCOL_COMPILER}
				)

IF( ${WIN32} )
target_compile_options( protocol_compiler PUBLIC "/await:strict" )
set_property(TARGET protocol_compiler PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON )
ELSE( ${WIN32} )
target_compile_options( protocol_compiler PUBLIC "-fcoroutines" )
ENDIF( ${WIN32} )

target_link_libraries( protocol_compiler utilities )

set_target_properties( protocol_compiler PROPERTIES FOLDER protocol_compiler )

link_boost( protocol_compiler filesystem )
link_boost( protocol_compiler iostreams )
link_boost( protocol_compiler serialization )
link_boost( protocol_compiler system )
link_boost( protocol_compiler atomic )

link_json( protocol_compiler )
link_inja( protocol_compiler )

link_common( protocol_compiler ) 

install( TARGETS protocol_compiler DESTINATION bin)