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

#get gtest
include( ${WORKSPACE_ROOT_PATH}/thirdparty/gtest/gtest_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )


set( MULTI_STAGE_BASE_DIR ${MEGA_TEST_DIR}/database/multi_stage_base )

set( MULTI_STAGE_BASE_DATABASE_GENERATED_HEADERS

    ${MULTI_STAGE_BASE_DIR}/database/model/data.hxx

    ${MULTI_STAGE_BASE_DIR}/database/model/environment.hxx
    ${MULTI_STAGE_BASE_DIR}/database/model/manifest.hxx
    ${MULTI_STAGE_BASE_DIR}/database/model/file_info.hxx

    ${MULTI_STAGE_BASE_DIR}/database/model/FirstStage.hxx
    ${MULTI_STAGE_BASE_DIR}/database/model/SecondStage.hxx
	)

set( MULTI_STAGE_BASE_DATABASE_GENERATED_SOURCES

    ${MULTI_STAGE_BASE_DIR}/database/model/data.cxx
    ${MULTI_STAGE_BASE_DIR}/database/model/environment.cxx
    ${MULTI_STAGE_BASE_DIR}/database/model/manifest.cxx
    ${MULTI_STAGE_BASE_DIR}/database/model/file_info.cxx

    ${MULTI_STAGE_BASE_DIR}/database/model/FirstStage.cxx
    ${MULTI_STAGE_BASE_DIR}/database/model/SecondStage.cxx
)

set( DATABASE_SCHEMAS 
        ${MULTI_STAGE_BASE_DIR}/schema.dbs
        )

set( COMPILER_OUTPUTS 
    ${MULTI_STAGE_BASE_DATABASE_GENERATED_HEADERS} 
    ${MULTI_STAGE_BASE_DATABASE_GENERATED_SOURCES} )

set( COMPILER_BYPRODUCTS
    ${MULTI_STAGE_BASE_DIR}/database/data/data.json 
    ${MULTI_STAGE_BASE_DIR}/database/data/stages.json
    ${MULTI_STAGE_BASE_DIR}/database/data/FirstStage.json

    ${MULTI_STAGE_BASE_DIR}/database/data/SecondStage.json
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
        --api ${MULTI_STAGE_BASE_DIR}/database/model/
        --src ${MULTI_STAGE_BASE_DIR}/database/model/
        --dblib $<TARGET_FILE:database_compiler>
        --stash ${CMAKE_CURRENT_BINARY_DIR}/stash
        --data_dir ${MULTI_STAGE_BASE_DIR}/database/data/
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
	${MULTI_STAGE_BASE_DIR}/multi_stage_base_tests.cpp 
    )

enable_testing()

add_executable( database_multi_stage_base_tests 

                ${MEGA_DATABASE_COMMON}
                ${MEGA_DATABASE_SOURCES}
                ${COMPILER_OUTPUTS}
                
                ${MEGA_TYPES_HEADERS}
                ${MEGA_TYPES_SOURCES}

                ${MEGA_UNIT_TESTS} 
                ${MULTI_STAGE_BASE_DIR}/main.cpp 
                )

# allow access to source folder for tests - NOTE order sensitive here
target_include_directories( database_multi_stage_base_tests PUBLIC ${MULTI_STAGE_BASE_DIR} )
target_include_directories( database_multi_stage_base_tests PUBLIC ${MEGA_API_DIR} )

set_target_properties( database_multi_stage_base_tests PROPERTIES FOLDER mega )

link_boost( database_multi_stage_base_tests filesystem )
link_boost( database_multi_stage_base_tests system )
link_boost( database_multi_stage_base_tests date_time )
link_boost( database_multi_stage_base_tests program_options )
link_boost( database_multi_stage_base_tests iostreams )
link_boost( database_multi_stage_base_tests atomic )
link_boost( database_multi_stage_base_tests serialization )
link_json( database_multi_stage_base_tests )
link_gtest( database_multi_stage_base_tests )
link_common( database_multi_stage_base_tests )

gtest_add_tests( TARGET database_multi_stage_base_tests SOURCES ${MEGA_UNIT_TESTS}  )

install( TARGETS database_multi_stage_base_tests DESTINATION bin )

add_custom_command( TARGET database_multi_stage_base_tests POST_BUILD COMMAND ctest --output-on-failure )