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

#get the database macro
include( ${WORKSPACE_ROOT_PATH}/src/database/db_include.cmake )

find_path( DB_LIB NAMES "api/component_info.hpp" PATHS "${BUILD_PATH}/${CFG_TUPLE}/db/install/lib" REQUIRED NO_DEFAULT_PATH)
find_path( DB_TEMPLATES NAMES "manifest.cxx.jinja" PATHS "${BUILD_PATH}/${CFG_TUPLE}/db/install/templates" REQUIRED NO_DEFAULT_PATH)
find_file( DB_COMPILER NAMES "database_compiler_driver" PATHS "${BUILD_PATH}/${CFG_TUPLE}/db/install/bin" REQUIRED NO_DEFAULT_PATH)

list(APPEND CMAKE_PREFIX_PATH "${COMMON_INSTALL_PATH}/share" )

set( MEGA_DATABASE_STAGES

    AutomataStage
    ComponentListing
    ComponentListingView
    ConcreteStage
    ConcreteTypeAnalysis
    HyperGraphAnalysis
    InterfaceStage
    ParserStage
    SymbolAnalysis
    SymbolRollout
    ClangTraitsStage
    ClangAnalysisStage
    ClangCompilationStage
    
    # TypeAnalysis
    # TypeAnalysisRollout
    # MetaStage
    # OperationsLocs
    # OperationsStage
    # UnityStage
    # UnityStageView
    # ValueSpaceStage
    # FinalStage
)

set( DATABASE_SCHEMAS 
    ${MEGA_SRC_DIR}/schema/stages.dbs
    ${MEGA_SRC_DIR}/schema/components.dbs
    ${MEGA_SRC_DIR}/schema/parser.dbs
    ${MEGA_SRC_DIR}/schema/interface.dbs
    ${MEGA_SRC_DIR}/schema/concrete.dbs
    #${MEGA_SRC_DIR}/schema/operations.dbs
    ${MEGA_SRC_DIR}/schema/analysis.dbs
    ${MEGA_SRC_DIR}/schema/automata.dbs
    ${MEGA_SRC_DIR}/schema/derivation.dbs
)

MegaDatabase( 
    DBNAME      database
    DBCOMPILER  ${DB_COMPILER}
    LIB         ${DB_LIB}
    TEMPLATE    ${DB_TEMPLATES}
    DATA        ${MEGA_SRC_DIR}/database/data/
    API         ${MEGA_API_DIR}
    SRC         ${MEGA_SRC_DIR}
    SCHEMAS     ${DATABASE_SCHEMAS}
    STAGES      ${MEGA_DATABASE_STAGES}
)

set( ENVIRONMENT_HEADERS 
    ${MEGA_API_DIR}/environment/environment.hpp
    ${MEGA_API_DIR}/environment/environment_archive.hpp
    ${MEGA_API_DIR}/environment/environment_build.hpp
    ${MEGA_API_DIR}/environment/environment_stash.hpp
    ${MEGA_API_DIR}/environment/jit_database.hpp
    ${MEGA_API_DIR}/environment/mpo_database.hpp
    ${MEGA_API_DIR}/environment/python_database.hpp
)
set( ENVIRONMENT_SRC
    ${MEGA_SRC_DIR}/environment/environment.cpp
    ${MEGA_SRC_DIR}/environment/environment_archive.cpp
    ${MEGA_SRC_DIR}/environment/environment_build.cpp
    ${MEGA_SRC_DIR}/environment/environment_stash.cpp
    ${MEGA_SRC_DIR}/environment/jit_database.cpp
    ${MEGA_SRC_DIR}/environment/mpo_database.cpp
    ${MEGA_SRC_DIR}/environment/python_database.cpp
)

add_library( database SHARED
    ${DATABASE_COPIED_FILES} 
    ${DATABASE_GENERATED_FILES} 
    ${ENVIRONMENT_HEADERS} ${ENVIRONMENT_SRC}
)
target_precompile_headers( database REUSE_FROM mega_pch )

target_include_directories( database PUBLIC ${MEGA_API_DIR} )

target_link_libraries( database mega_values )

link_boost( database filesystem )
link_boost( database system )
link_boost( database iostreams )
link_boost( database serialization )
link_common( database )

install( FILES ${DATABASE_COPIED_FILES_HPP} ${DATABASE_GENERATED_FILES_HXX} DESTINATION include/database )
install( FILES ${ENVIRONMENT_HEADERS} DESTINATION include/environment )
install( EXPORT database NAMESPACE MEGA:: FILE database-config.cmake DESTINATION share )

install( TARGETS database EXPORT database DESTINATION bin)