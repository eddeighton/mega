
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( MEGA_DATABASE_GENERATED_HEADERS

    ${MEGA_API_DIR}/database/model/data.hxx
    ${MEGA_API_DIR}/database/model/environment.hxx
    ${MEGA_API_DIR}/database/model/manifest.hxx
    ${MEGA_API_DIR}/database/model/file_info.hxx
    
    ${MEGA_API_DIR}/database/model/ComponentListing.hxx
    ${MEGA_API_DIR}/database/model/ComponentListingView.hxx
    ${MEGA_API_DIR}/database/model/ConcreteStage.hxx
    ${MEGA_API_DIR}/database/model/DependencyAnalysis.hxx
    ${MEGA_API_DIR}/database/model/DependencyAnalysisView.hxx
    ${MEGA_API_DIR}/database/model/DerivationAnalysis.hxx
    ${MEGA_API_DIR}/database/model/DerivationAnalysisView.hxx
    ${MEGA_API_DIR}/database/model/FinalStage.hxx    
    ${MEGA_API_DIR}/database/model/HyperGraphAnalysis.hxx
    ${MEGA_API_DIR}/database/model/HyperGraphAnalysisView.hxx
    ${MEGA_API_DIR}/database/model/InterfaceAnalysisStage.hxx
    ${MEGA_API_DIR}/database/model/InterfaceStage.hxx
    ${MEGA_API_DIR}/database/model/OperationsStage.hxx
    ${MEGA_API_DIR}/database/model/ParserStage.hxx
    ${MEGA_API_DIR}/database/model/SymbolAnalysis.hxx
    ${MEGA_API_DIR}/database/model/SymbolAnalysisView.hxx
    ${MEGA_API_DIR}/database/model/SymbolRollout.hxx
	)

set( MEGA_DATABASE_GENERATED_SOURCES

    ${MEGA_SRC_DIR}/database/model/data.cxx
    ${MEGA_SRC_DIR}/database/model/environment.cxx
    ${MEGA_SRC_DIR}/database/model/manifest.cxx
    ${MEGA_SRC_DIR}/database/model/file_info.cxx

    ${MEGA_SRC_DIR}/database/model/ComponentListing.cxx
    ${MEGA_SRC_DIR}/database/model/ComponentListingView.cxx
    ${MEGA_SRC_DIR}/database/model/ConcreteStage.cxx
    ${MEGA_SRC_DIR}/database/model/DependencyAnalysis.cxx
    ${MEGA_SRC_DIR}/database/model/DependencyAnalysisView.cxx
    ${MEGA_SRC_DIR}/database/model/DerivationAnalysis.cxx
    ${MEGA_SRC_DIR}/database/model/DerivationAnalysisView.cxx
    ${MEGA_SRC_DIR}/database/model/FinalStage.cxx
    ${MEGA_SRC_DIR}/database/model/HyperGraphAnalysis.cxx
    ${MEGA_SRC_DIR}/database/model/HyperGraphAnalysisView.cxx
    ${MEGA_SRC_DIR}/database/model/InterfaceAnalysisStage.cxx
    ${MEGA_SRC_DIR}/database/model/InterfaceStage.cxx
    ${MEGA_SRC_DIR}/database/model/OperationsStage.cxx
    ${MEGA_SRC_DIR}/database/model/ParserStage.cxx
    ${MEGA_SRC_DIR}/database/model/SymbolAnalysis.cxx
    ${MEGA_SRC_DIR}/database/model/SymbolAnalysisView.cxx
    ${MEGA_SRC_DIR}/database/model/SymbolRollout.cxx
)

set( DATABASE_SCHEMAS 
        ${MEGA_SRC_DIR}/database/stages.dbs
        ${MEGA_SRC_DIR}/database/parser.dbs
        ${MEGA_SRC_DIR}/database/interface.dbs
        ${MEGA_SRC_DIR}/database/invocation.dbs
        ${MEGA_SRC_DIR}/database/concrete.dbs
        ${MEGA_SRC_DIR}/database/operations.dbs
        ${MEGA_SRC_DIR}/database/analysis.dbs
        )

set( COMPILER_OUTPUTS 
    ${MEGA_DATABASE_GENERATED_HEADERS} 
    ${MEGA_DATABASE_GENERATED_SOURCES} )

set( COMPILER_BYPRODUCTS
    data.json 
    stages.json
    ComponentListing.json
    ComponentListingView.json
    ConcreteStage.json
    DependencyAnalysis.json
    DependencyAnalysisView.json
    DerivationAnalysis.json
    DerivationAnalysisView.json
    FinalStage.json
    HyperGraphAnalysis.json
    HyperGraphAnalysisView.json
    InterfaceAnalysisStage.json
    InterfaceStage.json
    OperationsStage.json
    ParserStage.json
    SymbolAnalysis.json
    SymbolAnalysisView.json
    SymbolRollout.json
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

add_custom_command( 
    OUTPUT ${COMPILER_OUTPUTS}
    COMMAND database_compiler_driver
        --api ${MEGA_API_DIR}/database/model/
        --src ${MEGA_SRC_DIR}/database/model/
        --data_dir ${MEGA_SRC_DIR}/database/data/
        --inja_dir ${MEGA_SRC_DIR}/database/compiler/templates/
        --json
        --template
        --input ${DATABASE_SCHEMAS}
    DEPENDS ${DATABASE_SCHEMAS} ${TEMPLATES} database_compiler_driver
    COMMENT "Generating Database" 
    )

# NOTE these files are duplicated in the database tests cmake
set( MEGA_DATABASE_COMMON
    ${MEGA_API_DIR}/database/common/archive.hpp
    ${MEGA_API_DIR}/database/common/component_info.hpp
    ${MEGA_API_DIR}/database/common/data_pointer.hpp
    ${MEGA_API_DIR}/database/common/environment_archive.hpp
    ${MEGA_API_DIR}/database/common/environment_build.hpp
    ${MEGA_API_DIR}/database/common/environment_stash.hpp
    ${MEGA_API_DIR}/database/common/exception.hpp
    ${MEGA_API_DIR}/database/common/file_header.hpp
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
    ${MEGA_API_DIR}/database/types/operation.hpp
    ${MEGA_API_DIR}/database/types/ownership.hpp
    ${MEGA_API_DIR}/database/types/sources.hpp
    ${MEGA_API_DIR}/database/types/modes.hpp
    ${MEGA_API_DIR}/database/types/component_type.hpp
    ${MEGA_API_DIR}/database/types/clang_compilation.hpp
    ${MEGA_API_DIR}/database/types/invocation_id.hpp
)

set( MEGA_TYPES_SOURCES
    ${MEGA_SRC_DIR}/database/types/cardinality.cpp
    ${MEGA_SRC_DIR}/database/types/operation.cpp
    ${MEGA_SRC_DIR}/database/types/ownership.cpp
    ${MEGA_SRC_DIR}/database/types/sources.cpp
    ${MEGA_SRC_DIR}/database/types/modes.cpp
    ${MEGA_SRC_DIR}/database/types/component_type.cpp
    ${MEGA_SRC_DIR}/database/types/clang_compilation.cpp
    ${MEGA_SRC_DIR}/database/types/invocation_id.cpp
)

add_library( database SHARED
                    ${MEGA_DATABASE_COMMON}
					${MEGA_DATABASE_SOURCES}
					${MEGA_DATABASE_GENERATED_HEADERS}
					${MEGA_DATABASE_GENERATED_SOURCES}
                    ${MEGA_TYPES_HEADERS}
                    ${MEGA_TYPES_SOURCES}
				)

add_dependencies( database database_compiler_driver )

target_include_directories( database PUBLIC ${MEGA_API_DIR} )

set_target_properties( database PROPERTIES FOLDER database )
set_property(TARGET database PROPERTY POSITION_INDEPENDENT_CODE ON)

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
install( TARGETS database EXPORT database DESTINATION bin)
install( EXPORT database NAMESPACE MEGA:: FILE database-config.cmake DESTINATION share )
