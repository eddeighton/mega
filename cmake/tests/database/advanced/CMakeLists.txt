
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get gtest
include( ${WORKSPACE_ROOT_PATH}/thirdparty/gtest/gtest_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )


set( ADVANCED_DIR ${MEGA_TEST_DIR}/database/advanced )

set( BASIC_DATABASE_GENERATED_HEADERS

    ${ADVANCED_DIR}/database/model/data_FirstFile.hxx
    ${ADVANCED_DIR}/database/model/data_SecondFile.hxx
    ${ADVANCED_DIR}/database/model/data_ThirdFile.hxx
    ${ADVANCED_DIR}/database/model/data_FourthFile.hxx

    ${ADVANCED_DIR}/database/model/environment.hxx
    ${ADVANCED_DIR}/database/model/manifest.hxx
    ${ADVANCED_DIR}/database/model/file_info.hxx

    ${ADVANCED_DIR}/database/model/FirstStage.hxx
    ${ADVANCED_DIR}/database/model/SecondStage.hxx
    ${ADVANCED_DIR}/database/model/ThirdStage.hxx
    ${ADVANCED_DIR}/database/model/FourthStage.hxx
	)

set( BASIC_DATABASE_GENERATED_SOURCES

    ${ADVANCED_DIR}/database/model/data.cxx
    ${ADVANCED_DIR}/database/model/environment.cxx
    ${ADVANCED_DIR}/database/model/manifest.cxx
    ${ADVANCED_DIR}/database/model/file_info.cxx

    ${ADVANCED_DIR}/database/model/FirstStage.cxx
    ${ADVANCED_DIR}/database/model/SecondStage.cxx
    ${ADVANCED_DIR}/database/model/ThirdStage.cxx
    ${ADVANCED_DIR}/database/model/FourthStage.cxx
)

set( DATABASE_SCHEMAS 
        ${ADVANCED_DIR}/schema.dbs
        )

set( COMPILER_OUTPUTS 
    ${BASIC_DATABASE_GENERATED_HEADERS} 
    ${BASIC_DATABASE_GENERATED_SOURCES} )

set( COMPILER_BYPRODUCTS
    ${ADVANCED_DIR}/database/data/data.json 
    ${ADVANCED_DIR}/database/data/stages.json
    ${ADVANCED_DIR}/database/data/FirstStage.json
    ${ADVANCED_DIR}/database/data/SecondStage.json
    ${ADVANCED_DIR}/database/data/ThirdStage.json
    ${ADVANCED_DIR}/database/data/FourthStage.json
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
        --api ${ADVANCED_DIR}/database/model/
        --src ${ADVANCED_DIR}/database/model/
        --dblib $<TARGET_FILE:database_compiler>
        --stash ${CMAKE_CURRENT_BINARY_DIR}/stash
        --data_dir ${ADVANCED_DIR}/database/data/
        --inja_dir ${MEGA_SRC_DIR}/database/compiler/templates/
        --input ${DATABASE_SCHEMAS}
    DEPENDS ${DATABASE_SCHEMAS} ${TEMPLATES} database_compiler_driver
    COMMENT "Generating Database" )


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
    ${MEGA_API_DIR}/database/types/clang_compilation.hpp
    ${MEGA_API_DIR}/database/types/invocation_id.hpp
)

set( MEGA_TYPES_SOURCES
    ${MEGA_SRC_DIR}/database/types/cardinality.cpp
    ${MEGA_SRC_DIR}/database/types/operation.cpp
    ${MEGA_SRC_DIR}/database/types/ownership.cpp
    ${MEGA_SRC_DIR}/database/types/sources.cpp
    ${MEGA_SRC_DIR}/database/types/modes.cpp
    ${MEGA_SRC_DIR}/database/types/clang_compilation.cpp
    ${MEGA_SRC_DIR}/database/types/invocation_id.cpp
)

set( MEGA_UNIT_TESTS
	${ADVANCED_DIR}/advanced_tests.cpp 
    )

enable_testing()

add_executable( database_advanced_test 

                ${MEGA_DATABASE_COMMON}
                ${MEGA_DATABASE_SOURCES}
                ${COMPILER_OUTPUTS}
                
                ${MEGA_TYPES_HEADERS}
                ${MEGA_TYPES_SOURCES}

                ${MEGA_UNIT_TESTS} 
                ${ADVANCED_DIR}/main.cpp 
                )

# allow access to source folder for tests - NOTE order sensitive here
target_include_directories( database_advanced_test PUBLIC ${ADVANCED_DIR} )
target_include_directories( database_advanced_test PUBLIC ${MEGA_API_DIR} )

set_target_properties( database_advanced_test PROPERTIES FOLDER mega )

link_boost( database_advanced_test filesystem )
link_boost( database_advanced_test system )
link_boost( database_advanced_test date_time )
link_boost( database_advanced_test program_options )
link_boost( database_advanced_test iostreams )
link_boost( database_advanced_test atomic )
link_boost( database_advanced_test serialization )
link_json( database_advanced_test )
link_gtest( database_advanced_test )
link_common( database_advanced_test )

gtest_add_tests( TARGET database_advanced_test SOURCES ${MEGA_UNIT_TESTS}  )

install( TARGETS database_advanced_test DESTINATION bin )

add_custom_command( TARGET database_advanced_test POST_BUILD COMMAND ctest --output-on-failure )