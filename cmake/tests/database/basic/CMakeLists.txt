
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get gtest
include( ${WORKSPACE_ROOT_PATH}/thirdparty/gtest/gtest_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )


set( BASIC_DIR ${MEGA_TEST_DIR}/database/basic )

set( BASIC_DATABASE_GENERATED_HEADERS

    ${BASIC_DIR}/database/model/data.hxx
    ${BASIC_DIR}/database/model/environment.hxx
    ${BASIC_DIR}/database/model/manifest.hxx
    ${BASIC_DIR}/database/model/file_info.hxx

    ${BASIC_DIR}/database/model/BasicStage.hxx
    ${BASIC_DIR}/database/model/SecondStage.hxx
    ${BASIC_DIR}/database/model/ThirdStage.hxx
    ${BASIC_DIR}/database/model/FourthStage.hxx
	)

set( BASIC_DATABASE_GENERATED_SOURCES

    ${BASIC_DIR}/database/model/data.cxx
    ${BASIC_DIR}/database/model/environment.cxx
    ${BASIC_DIR}/database/model/manifest.cxx
    ${BASIC_DIR}/database/model/file_info.cxx

    ${BASIC_DIR}/database/model/BasicStage.cxx
    ${BASIC_DIR}/database/model/SecondStage.cxx
    ${BASIC_DIR}/database/model/ThirdStage.cxx
    ${BASIC_DIR}/database/model/FourthStage.cxx
)

set( DATABASE_SCHEMAS 
        ${BASIC_DIR}/schema.dbs
        )

set( COMPILER_OUTPUTS 
    ${BASIC_DATABASE_GENERATED_HEADERS} 
    ${BASIC_DATABASE_GENERATED_SOURCES} )

set( COMPILER_BYPRODUCTS
    ${BASIC_DIR}/database/data/data.json 
    ${BASIC_DIR}/database/data/stages.json
    ${BASIC_DIR}/database/data/BasicStage.json
    ${BASIC_DIR}/database/data/SecondStage.json
    ${BASIC_DIR}/database/data/ThirdStage.json
    ${BASIC_DIR}/database/data/FourthStage.json
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
        --api ${BASIC_DIR}/database/model/
        --src ${BASIC_DIR}/database/model/
        --data_dir ${BASIC_DIR}/database/data/
        --inja_dir ${MEGA_SRC_DIR}/database/compiler/templates/
        --json
        --template
        --input ${DATABASE_SCHEMAS}
    DEPENDS ${DATABASE_SCHEMAS} ${TEMPLATES} database_compiler_driver
    COMMENT "Generating Database" )

set( MEGA_DATABASE_COMMON
${MEGA_API_DIR}/database/common/serialisation.hpp
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

set( MEGA_UNIT_TESTS
	${BASIC_DIR}/basic_tests.cpp 
    )

enable_testing()

add_executable( database_basic_tests 

                ${MEGA_DATABASE_COMMON}
                ${MEGA_DATABASE_SOURCES}
                ${COMPILER_OUTPUTS}

                ${MEGA_UNIT_TESTS} 
                ${BASIC_DIR}/main.cpp 
                )

# allow access to source folder for tests - NOTE order sensitive here
target_include_directories( database_basic_tests PUBLIC ${BASIC_DIR} )
target_include_directories( database_basic_tests PUBLIC ${MEGA_API_DIR} )

set_target_properties( database_basic_tests PROPERTIES FOLDER mega )

link_boost( database_basic_tests filesystem )
link_boost( database_basic_tests system )
link_boost( database_basic_tests date_time )
link_boost( database_basic_tests program_options )
link_boost( database_basic_tests iostreams )
link_boost( database_basic_tests atomic )
link_boost( database_basic_tests serialization )
link_json( database_basic_tests )
link_gtest( database_basic_tests )
link_common( database_basic_tests )

gtest_add_tests( TARGET database_basic_tests SOURCES ${MEGA_UNIT_TESTS}  )

install( TARGETS database_basic_tests DESTINATION bin )

add_custom_command( TARGET database_basic_tests POST_BUILD COMMAND ctest --output-on-failure )