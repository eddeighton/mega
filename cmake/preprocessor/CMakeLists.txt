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

set( PREPROCESSOR_API ${MEGA_API_DIR}/preprocessor/preprocessor.hpp )

set( PREPROCESSOR_SOURCES
    ${MEGA_SRC_DIR}/preprocessor/preprocessor.cpp
    )

add_library( preprocessor
                    ${PREPROCESSOR_API}
                    ${PREPROCESSOR_SOURCES} )

target_precompile_headers( preprocessor REUSE_FROM mega_pch )

target_include_directories( preprocessor PUBLIC ${MEGA_API_DIR} )

# target_link_libraries( invocation mega_values )

set_target_properties( preprocessor PROPERTIES FOLDER compiler )

link_boost( preprocessor filesystem )
link_boost( preprocessor system )
link_boost( preprocessor wave )
link_common( preprocessor )

install( FILES ${PREPROCESSOR_API} DESTINATION include/preprocessor )
install( TARGETS preprocessor EXPORT preprocessor DESTINATION bin)
install( EXPORT preprocessor NAMESPACE MEGA:: FILE preprocessor-config.cmake DESTINATION share )
