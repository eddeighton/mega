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

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( MEGA_DATABASE_GENERATED_HEADERS
    ${MEGA_API_DIR}/database/model/environment.hxx
    ${MEGA_API_DIR}/database/model/manifest.hxx
    ${MEGA_API_DIR}/database/model/file_info.hxx
    ${MEGA_API_DIR}/database/model/data.hxx
	)

set( MEGA_DATABASE_GENERATED_SOURCES
    ${MEGA_SRC_DIR}/database/model/data.cxx
    ${MEGA_SRC_DIR}/database/model/environment.cxx
    ${MEGA_SRC_DIR}/database/model/manifest.cxx
    ${MEGA_SRC_DIR}/database/model/file_info.cxx
)


set( MEGA_DATABASE_INTERFACE_NAMES
    AutomataStage
    ComponentListing
    ComponentListingView
    ConcreteStage
    ConcreteTypeAnalysis
    ConcreteTypeAnalysisView
    ConcreteTypeRollout
    DependencyAnalysis
    DependencyAnalysisView
    DerivationAnalysis
    DerivationAnalysisRollout
    DerivationAnalysisView
    FinalStage
    GlobalMemoryStage
    GlobalMemoryStageRollout
    HyperGraphAnalysis
    HyperGraphAnalysisRollout
    HyperGraphAnalysisView
    InterfaceAnalysisStage
    InterfaceStage
    MemoryStage
    MetaStage
    OperationsLocs
    OperationsStage
    ParserStage
    SymbolAnalysis
    SymbolAnalysisView
    SymbolRollout
    UnityStage
    UnityStageView
    ValueSpaceStage
)

set( MEGA_DATABASE_STAGE_HEADERS )
set( MEGA_DATABASE_STAGE_SOURCES )
set( COMPILER_BYPRODUCTS
    data.json 
    stages.json
)

foreach(interface_name ${MEGA_DATABASE_INTERFACE_NAMES})

    list(APPEND MEGA_DATABASE_STAGE_HEADERS "${MEGA_API_DIR}/database/model/${interface_name}.hxx" )
    list(APPEND MEGA_DATABASE_STAGE_SOURCES "${MEGA_SRC_DIR}/database/model/${interface_name}.cxx" )
    list(APPEND COMPILER_BYPRODUCTS "${interface_name}.json" )

endforeach()

set( DATABASE_SCHEMAS 
        ${MEGA_SRC_DIR}/database/stages.dbs
        ${MEGA_SRC_DIR}/database/parser.dbs
        ${MEGA_SRC_DIR}/database/interface.dbs
        ${MEGA_SRC_DIR}/database/invocation.dbs
        ${MEGA_SRC_DIR}/database/concrete.dbs
        ${MEGA_SRC_DIR}/database/operations.dbs
        ${MEGA_SRC_DIR}/database/analysis.dbs
        ${MEGA_SRC_DIR}/database/automata.dbs
        )

set( COMPILER_OUTPUTS 
    ${MEGA_DATABASE_GENERATED_HEADERS}
    ${MEGA_DATABASE_GENERATED_SOURCES}
    ${MEGA_DATABASE_STAGE_HEADERS}
    ${MEGA_DATABASE_STAGE_SOURCES}
    )

set( TEMPLATES 
    ${MEGA_SRC_DIR}/database/compiler/templates/data.hxx.jinja
    ${MEGA_SRC_DIR}/database/compiler/templates/environment.hxx.jinja
    ${MEGA_SRC_DIR}/database/compiler/templates/file_info.hxx.jinja
    ${MEGA_SRC_DIR}/database/compiler/templates/manifest.hxx.jinja
    ${MEGA_SRC_DIR}/database/compiler/templates/view.hxx.jinja
    
    ${MEGA_SRC_DIR}/database/compiler/templates/data.cxx.jinja
    ${MEGA_SRC_DIR}/database/compiler/templates/environment.cxx.jinja
    ${MEGA_SRC_DIR}/database/compiler/templates/file_info.cxx.jinja
    ${MEGA_SRC_DIR}/database/compiler/templates/manifest.cxx.jinja
    ${MEGA_SRC_DIR}/database/compiler/templates/view.cxx.jinja
)

add_custom_target( GenerateDatabase
    COMMAND database_compiler_driver
        --api ${MEGA_API_DIR}/database/model/
        --src ${MEGA_SRC_DIR}/database/model/
        --dblib $<TARGET_FILE:database_compiler>
        --stash ${CMAKE_BINARY_DIR}/stash
        --data_dir ${MEGA_SRC_DIR}/database/data/
        --inja_dir ${MEGA_SRC_DIR}/database/compiler/templates/
        --input ${DATABASE_SCHEMAS}
    DEPENDS ${DATABASE_SCHEMAS} ${TEMPLATES} database_compiler_driver
    BYPRODUCTS  ${COMPILER_OUTPUTS}
    COMMENT "Generating Database" 
    SOURCES ${DATABASE_SCHEMAS}
    )

set_target_properties( GenerateDatabase PROPERTIES FOLDER database )
add_dependencies( GenerateDatabase database_compiler_driver )

# NOTE these files are duplicated in the database tests cmake
set( MEGA_DATABASE_COMMON
    ${MEGA_API_DIR}/database/common/api.hpp
    ${MEGA_API_DIR}/database/common/archive.hpp
    ${MEGA_API_DIR}/database/common/component_info.hpp
    ${MEGA_API_DIR}/database/common/data_pointer.hpp
    ${MEGA_API_DIR}/database/common/data_variant.hpp
    ${MEGA_API_DIR}/database/common/environment_archive.hpp
    ${MEGA_API_DIR}/database/common/environment_build.hpp
    ${MEGA_API_DIR}/database/common/environment_stash.hpp
    ${MEGA_API_DIR}/database/common/exception.hpp
    ${MEGA_API_DIR}/database/common/file_header.hpp
    ${MEGA_API_DIR}/database/common/file_store.hpp
    ${MEGA_API_DIR}/database/common/file_system.hpp
    ${MEGA_API_DIR}/database/common/file.hpp
    ${MEGA_API_DIR}/database/common/generics.hpp
    ${MEGA_API_DIR}/database/common/loader.hpp
    ${MEGA_API_DIR}/database/common/manifest_data.hpp
    ${MEGA_API_DIR}/database/common/object_info.hpp
    ${MEGA_API_DIR}/database/common/object_loader.hpp
    ${MEGA_API_DIR}/database/common/object.hpp
    ${MEGA_API_DIR}/database/common/serialisation.hpp
    ${MEGA_API_DIR}/database/common/storer.hpp
	)

set( MEGA_DATABASE_SOURCES
    ${MEGA_SRC_DIR}/database/common/archive.cpp
    ${MEGA_SRC_DIR}/database/common/environment_archive.cpp
    ${MEGA_SRC_DIR}/database/common/environment_build.cpp
    ${MEGA_SRC_DIR}/database/common/environment_stash.cpp
    ${MEGA_SRC_DIR}/database/common/component_info.cpp
    ${MEGA_SRC_DIR}/database/common/file.cpp
    ${MEGA_SRC_DIR}/database/common/loader.cpp
    ${MEGA_SRC_DIR}/database/common/object.cpp
    ${MEGA_SRC_DIR}/database/common/storer.cpp
    )

set( MEGA_TYPES_HEADERS
    
    ${MEGA_API_DIR}/database/types/cardinality.hpp
    ${MEGA_API_DIR}/database/types/clang_compilation.hpp
    ${MEGA_API_DIR}/database/types/component_type.hpp
    ${MEGA_API_DIR}/database/types/derivation.hpp
    ${MEGA_API_DIR}/database/types/modes.hpp
    ${MEGA_API_DIR}/database/types/operation.hpp
    ${MEGA_API_DIR}/database/types/ownership.hpp
    ${MEGA_API_DIR}/database/types/source_location.hpp
    ${MEGA_API_DIR}/database/types/sources.hpp
    ${MEGA_API_DIR}/database/types/type_id.hpp
    
    ${MEGA_API_DIR}/database/jit_database.hpp
    ${MEGA_API_DIR}/database/mpo_database.hpp
)

set( MEGA_TYPES_SOURCES
    ${MEGA_SRC_DIR}/database/types/cardinality.cpp
    ${MEGA_SRC_DIR}/database/types/operation.cpp
    ${MEGA_SRC_DIR}/database/types/ownership.cpp
    ${MEGA_SRC_DIR}/database/types/derivation.cpp
    ${MEGA_SRC_DIR}/database/types/modes.cpp
    ${MEGA_SRC_DIR}/database/types/component_type.cpp
    ${MEGA_SRC_DIR}/database/types/clang_compilation.cpp
    ${MEGA_SRC_DIR}/database/types/type_id.cpp
    
    ${MEGA_SRC_DIR}/database/jit_database.cpp
    ${MEGA_SRC_DIR}/database/mpo_database.cpp
)

add_library( database SHARED
                    ${MEGA_DATABASE_COMMON}
					${MEGA_DATABASE_SOURCES}
					${MEGA_DATABASE_GENERATED_HEADERS}
					${MEGA_DATABASE_GENERATED_SOURCES}
                    
                    ${MEGA_DATABASE_STAGE_HEADERS}
                    ${MEGA_DATABASE_STAGE_SOURCES}

                    ${MEGA_TYPES_HEADERS}
                    ${MEGA_TYPES_SOURCES}
				)
                
add_dependencies( database GenerateDatabase )

target_include_directories( database PUBLIC ${MEGA_API_DIR} )

add_definitions(-DEG_DATABASE_API_SHARED_MODULE)

IF( ${WIN32} )

target_compile_options(database PRIVATE /bigobj)
# for windows need to work around limit of 65K symbols - /Ob2 appears to achieve this
#target_compile_options(database PRIVATE /Ob2)
# symbols are manually exported so ensure this is OFF
set_property(TARGET database PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS OFF )

ELSE( ${WIN32} )

set_property(TARGET database PROPERTY C_VISIBILITY_PRESET hidden)
set_property(TARGET database PROPERTY VISIBILITY_INLINES_HIDDEN ON)
set_property(TARGET database PROPERTY POSITION_INDEPENDENT_CODE ON)

ENDIF( ${WIN32} )

set_target_properties( database PROPERTIES FOLDER database )
set_target_properties( database PROPERTIES VERSION "1.0.0" )

target_link_libraries( database mega_interface )

link_boost( database filesystem )
link_boost( database iostreams )
link_boost( database serialization )
link_boost( database system )
link_boost( database atomic )
link_json( database )
link_common( database )

install( FILES ${MEGA_DATABASE_COMMON} DESTINATION include/database/common )
install( FILES ${MEGA_TYPES_HEADERS} DESTINATION include/database/types )
install( FILES ${MEGA_DATABASE_GENERATED_HEADERS} DESTINATION include/database/model )
install( FILES ${MEGA_DATABASE_STAGE_HEADERS} DESTINATION include/database/model )

install( TARGETS database EXPORT database DESTINATION bin)
install( EXPORT database NAMESPACE MEGA:: FILE database-config.cmake DESTINATION share )
