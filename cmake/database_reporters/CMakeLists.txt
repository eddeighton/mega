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

set( DATABASE_REPORTERS_API ${MEGA_API_DIR}/database_reporters )
set( DATABASE_REPORTERS_SRC ${MEGA_SRC_DIR}/database_reporters )

set( DATABASE_REPORTERS_HEADERS
    ${DATABASE_REPORTERS_API}/factory.hpp
    ${DATABASE_REPORTERS_API}/compilation_reporter_args.hpp
)

set( DATABASE_REPORTERS_SRC 
    ${DATABASE_REPORTERS_SRC}/reporters.hpp
    
    ${DATABASE_REPORTERS_SRC}/factory.cpp

    ${DATABASE_REPORTERS_SRC}/parser.cpp
    ${DATABASE_REPORTERS_SRC}/ast.cpp
    # ${DATABASE_REPORTERS_SRC}/and_or_tree.cpp
    # ${DATABASE_REPORTERS_SRC}/bdd.cpp
    # ${DATABASE_REPORTERS_SRC}/derivation.cpp
    # ${DATABASE_REPORTERS_SRC}/enum.cpp
    # ${DATABASE_REPORTERS_SRC}/functions.cpp
    # ${DATABASE_REPORTERS_SRC}/inheritance.cpp
    # ${DATABASE_REPORTERS_SRC}/interface.cpp
    # ${DATABASE_REPORTERS_SRC}/symbols.cpp
    # ${DATABASE_REPORTERS_SRC}/truth_table.cpp
)


add_library( database_reporters SHARED
    ${DATABASE_REPORTERS_HEADERS} 
    ${DATABASE_REPORTERS_SRC} 
)
target_precompile_headers( database_reporters REUSE_FROM mega_pch )

target_include_directories( database_reporters PUBLIC ${MEGA_API_DIR} )

target_link_libraries( database_reporters mega_values )
target_link_libraries( database_reporters reports )
target_link_libraries( database_reporters database )

link_boost( database_reporters filesystem )
link_boost( database_reporters system )
link_boost( database_reporters iostreams )
link_boost( database_reporters serialization )
link_common( database_reporters )

install( FILES ${DATABASE_REPORTERS_HEADERS} DESTINATION include/database_reporters )
install( FILES ${ENVIRONMENT_HEADERS} DESTINATION include/environment )
install( EXPORT database_reporters NAMESPACE MEGA:: FILE database_reporters-config.cmake DESTINATION share )

install( TARGETS database_reporters EXPORT database_reporters DESTINATION bin)