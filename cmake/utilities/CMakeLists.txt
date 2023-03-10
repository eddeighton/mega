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

#get clang
#include( ${WORKSPACE_ROOT_PATH}/thirdparty/llvm/clang_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get inja
include( ${WORKSPACE_ROOT_PATH}/thirdparty/inja/inja_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( UTILITIES_HEADERS
    ${MEGA_API_DIR}/utilities/glob.hpp
    ${MEGA_API_DIR}/utilities/clang_format.hpp
    ${MEGA_API_DIR}/utilities/cmake.hpp
    ${MEGA_API_DIR}/utilities/tool_chain_hash.hpp
    ${MEGA_API_DIR}/utilities/serialization_helpers.hpp
    ${MEGA_API_DIR}/utilities/project.hpp
    ${MEGA_API_DIR}/utilities/megastructure_installation.hpp
)

set( UTILITIES_SOURCE
    ${MEGA_SRC_DIR}/utilities/glob.cpp
    ${MEGA_SRC_DIR}/utilities/clang_format.cpp
    ${MEGA_SRC_DIR}/utilities/tool_chain_hash.cpp
    ${MEGA_SRC_DIR}/utilities/project.cpp
    ${MEGA_SRC_DIR}/utilities/megastructure_installation.cpp
)


add_library( utilities
    ${UTILITIES_HEADERS} 
    ${UTILITIES_SOURCE}
)

set_target_properties( utilities PROPERTIES FOLDER utilities )

target_include_directories( utilities PUBLIC ${MEGA_API_DIR} )

set_property(TARGET utilities PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET utilities PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON )

target_link_libraries( utilities mega_interface )
#target_compile_definitions( utilities PRIVATE DO_NOT_DO_THIS_BECAUSE_MASSIVE_CODE_BLOAT )

link_boost( utilities atomic )
link_boost( utilities program_options )
link_boost( utilities timer )
link_boost( utilities filesystem )
link_boost( utilities iostreams )
link_boost( utilities serialization )
#link_clang( utilities )
link_json( utilities )
link_inja( utilities )
link_common( utilities )

install( FILES ${UTILITIES_HEADERS} DESTINATION include/utilities )
install( TARGETS utilities EXPORT utilities DESTINATION bin)
install( EXPORT utilities NAMESPACE MEGA:: FILE utilities-config.cmake DESTINATION share )