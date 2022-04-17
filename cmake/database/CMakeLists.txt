
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get graphviz
#include( ${WORKSPACE_ROOT_PATH}/thirdparty/graphviz/graphviz_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

#get handy function to preprocess files for the xmacros stuff
include( ../preprocess_cpp.cmake )

include_directories( ${MEGA_API_DIR} )
include_directories( ${MEGA_SRC_DIR} )

set( MEGA_DATABASE_GENERATED_HEADERS

    ${MEGA_API_DIR}/database/model/data.hxx
    ${MEGA_API_DIR}/database/model/environment.hxx
    ${MEGA_API_DIR}/database/model/manifest.hxx
    ${MEGA_API_DIR}/database/model/file_info.hxx

    ${MEGA_API_DIR}/database/model/ComponentListing.hxx
    ${MEGA_API_DIR}/database/model/ParserStage.hxx
    ${MEGA_API_DIR}/database/model/InterfaceStage.hxx
    ${MEGA_API_DIR}/database/model/FinalStage.hxx
	)

set( MEGA_DATABASE_GENERATED_SOURCES

    ${MEGA_SRC_DIR}/database/model/data.cxx
    ${MEGA_SRC_DIR}/database/model/environment.cxx
    ${MEGA_SRC_DIR}/database/model/manifest.cxx
    ${MEGA_SRC_DIR}/database/model/file_info.cxx

    ${MEGA_SRC_DIR}/database/model/ComponentListing.cxx
    ${MEGA_SRC_DIR}/database/model/ParserStage.cxx
    ${MEGA_SRC_DIR}/database/model/InterfaceStage.cxx
    ${MEGA_SRC_DIR}/database/model/FinalStage.cxx
)

set( DATABASE_SCHEMAS 
        ${MEGA_SRC_DIR}/database/parser.dbs
        ${MEGA_SRC_DIR}/database/interface.dbs
        ${MEGA_SRC_DIR}/database/final.dbs
        )

set( COMPILER_OUTPUTS 
    ${MEGA_DATABASE_GENERATED_HEADERS} 
    ${MEGA_DATABASE_GENERATED_SOURCES} )

set( COMPILER_BYPRODUCTS
    data.json 
    stages.json
    ComponentListing.json
    ParserStage.json
    InterfaceStage.json
    FinalStage.json
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

set( MEGA_DATABASE_COMMON
    ${MEGA_API_DIR}/database/common/archive.hpp
    ${MEGA_API_DIR}/database/common/component_info.hpp
    ${MEGA_API_DIR}/database/common/data_pointer.hpp
    ${MEGA_API_DIR}/database/common/file.hpp
    ${MEGA_API_DIR}/database/common/generics.hpp
    ${MEGA_API_DIR}/database/common/loader.hpp
    ${MEGA_API_DIR}/database/common/object_info.hpp
    ${MEGA_API_DIR}/database/common/object_loader.hpp
    ${MEGA_API_DIR}/database/common/object.hpp
    ${MEGA_API_DIR}/database/common/storer.hpp
    ${MEGA_API_DIR}/database/common/sources.hpp
	)

set( MEGA_DATABASE_SOURCES
    ${MEGA_SRC_DIR}/database/common/sources.cpp
    ${MEGA_SRC_DIR}/database/common/component_info.cpp
    ${MEGA_SRC_DIR}/database/common/file.cpp
    ${MEGA_SRC_DIR}/database/common/loader.cpp
    ${MEGA_SRC_DIR}/database/common/object.cpp
    ${MEGA_SRC_DIR}/database/common/storer.cpp
    )

add_library( database
                    ${MEGA_DATABASE_COMMON}
					${MEGA_DATABASE_SOURCES}
					${MEGA_DATABASE_GENERATED_HEADERS}
					${MEGA_DATABASE_GENERATED_SOURCES}
				)

add_dependencies( database database_compiler_driver )
#add_dependencies( database database_generation )


target_compile_definitions(database PUBLIC -DBOOST_STACKTRACE_USE_ADDR2LINE)
set_target_properties( database PROPERTIES FOLDER database )
set_property(TARGET database PROPERTY POSITION_INDEPENDENT_CODE ON)

link_boost( database filesystem )
link_boost( database iostreams )
link_boost( database serialization )
link_boost( database system )
link_boost( database atomic )
link_json( database )
link_common( database )

#install( FILES ${MEGA_LIBRARY} DESTINATION include/database/mega )
install( FILES ${MEGA_DATABASE_COMMON} DESTINATION include/database/io )
install( FILES ${MEGA_DATABASE_GENERATED_HEADERS} DESTINATION include/database/model )
install( TARGETS database EXPORT database DESTINATION bin)
install( EXPORT database NAMESPACE MEGA:: FILE database-config.cmake DESTINATION share )

add_c_preprocessor_command( OUTPUT stages.hpp.i SOURCE ${MEGA_API_DIR}/database/io/stages.hpp TARGET database )
add_custom_target( PreprocessStages
	SOURCES stages.hpp.i
	COMMENT "Proprocessing stages.hpp")

add_c_preprocessor_command( OUTPUT file_info.hxx.i SOURCE ${MEGA_API_DIR}/database/io/file_info.hxx TARGET database )
add_custom_target( PreprocessFileInfo
    SOURCES file_info.hxx.i
    COMMENT "Proprocessing file_info.hxx")
