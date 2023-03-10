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

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get inja
include( ${WORKSPACE_ROOT_PATH}/thirdparty/inja/inja_include.cmake )

#get pybind11
include( ${WORKSPACE_ROOT_PATH}/thirdparty/pybind11/pybind11_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( MEGA_PYTHON_SRC_DIR ${MEGA_SRC_DIR}/service/python )

####################################################################
####################################################################


set( MEGA_PYTHON_SOURCE 
    ${MEGA_PYTHON_SRC_DIR}/python.hpp
    ${MEGA_PYTHON_SRC_DIR}/python.cpp
    ${MEGA_PYTHON_SRC_DIR}/mpo_conversation.hpp
    ${MEGA_PYTHON_SRC_DIR}/mpo_conversation.cpp
    ${MEGA_PYTHON_SRC_DIR}/request.hpp
    ${MEGA_PYTHON_SRC_DIR}/routing.cpp
    ${MEGA_PYTHON_SRC_DIR}/status.cpp
    ${MEGA_PYTHON_SRC_DIR}/project.cpp
    ${MEGA_PYTHON_SRC_DIR}/module.cpp 
    )

add_library( megastructure SHARED ${MEGA_PYTHON_SOURCE} )

##############################################################
# NOT DOING THIS: pybind11_add_module( megastructure SHARED ${MEGA_PYTHON_SOURCE} )
# INSTEAD DOING add_library and FOLLOWING:
# see https://pybind11.readthedocs.io/en/stable/compiling.html
pybind11_extension( megastructure )
#if(NOT MSVC AND NOT ${CMAKE_BUILD_TYPE} MATCHES Debug|RelWithDebInfo)
#    # Strip unnecessary sections of the binary on Linux/macOS
#    pybind11_strip(megastructure)
#endif()
#set_target_properties(megastructure PROPERTIES CXX_VISIBILITY_PRESET "hidden"
#                                         CUDA_VISIBILITY_PRESET "hidden")
##############################################################

IF( ${WIN32} )
target_compile_options( megastructure PUBLIC "/await:strict" )
target_compile_options( megastructure PRIVATE "/bigobj" )
ELSE( ${WIN32} )
target_compile_options( megastructure PUBLIC "-fcoroutines" )
target_link_options( megastructure PUBLIC -export-dynamic )
ENDIF( ${WIN32} )

# target_link_libraries( megastructure mega_interface )
# target_link_libraries( megastructure protocol )
# target_link_libraries( megastructure network )
# target_link_libraries( megastructure leaf )
# target_link_libraries( megastructure database )
# target_link_libraries( megastructure orc )
# target_link_libraries( megastructure log )
target_link_libraries( megastructure executorLib )
target_link_libraries( megastructure mega_mangle )

set_target_properties( megastructure PROPERTIES FOLDER service )

link_boost( megastructure atomic )
link_boost( megastructure program_options )
link_boost( megastructure timer )
link_boost( megastructure filesystem )
link_boost( megastructure iostreams )
link_boost( megastructure serialization )
link_fmt( megastructure )
link_spdlog( megastructure )
link_json( megastructure )
link_inja( megastructure )
link_pybind11( megastructure )
link_common( megastructure )

install( FILES ${MEGA_PYTHON_API} DESTINATION include/service/megastructure )
install( TARGETS megastructure EXPORT megastructure DESTINATION bin)
install( EXPORT megastructure NAMESPACE MEGA:: FILE megastructure-config.cmake DESTINATION share )

