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

include_directories( ${MEGA_API_DIR} )
include_directories( ${MEGA_SRC_DIR} )

add_executable( protocol_compiler_driver ${MEGA_SRC_DIR}/service/protocol/compiler/main.cpp )

set_target_properties( protocol_compiler_driver PROPERTIES FOLDER protocol_compiler_driver )

link_boost( protocol_compiler_driver filesystem )
link_boost( protocol_compiler_driver system )
link_boost( protocol_compiler_driver atomic )
link_boost( protocol_compiler_driver program_options )
link_common( protocol_compiler_driver ) 

target_link_libraries(protocol_compiler_driver protocol_compiler)
target_link_libraries(protocol_compiler_driver utilities)

install( TARGETS protocol_compiler_driver DESTINATION bin)

add_dependencies( protocol_compiler_driver protocol_compiler )
