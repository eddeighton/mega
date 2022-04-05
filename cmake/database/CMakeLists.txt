
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

#get handy function to preprocess files for the xmacros stuff
include( ../preprocess_cpp.cmake )

set( DATABASE_SCHEMA ${MEGA_SRC_DIR}/database_compiler/data/schema.txt )
add_custom_target( database_generation
    COMMAND database_compiler 
        --input ${DATABASE_SCHEMA}
        --api ${MEGA_API_DIR}/database/model/
        --src ${MEGA_SRC_DIR}/database/model/
        --data_dir ${MEGA_SRC_DIR}/database_compiler/data/
        --inja_dir ${MEGA_SRC_DIR}/database_compiler/templates/
        --json
        --template
    DEPENDS ${DATABASE_SCHEMA}
    SOURCES ${DATABASE_SCHEMA}
)
add_dependencies( database_generation database_compiler )

#set( MEGA_LIBRARY
#	${MEGA_API_DIR}/mega/allocator.hpp
#	${MEGA_API_DIR}/mega/backtrace.hpp
#	${MEGA_API_DIR}/mega/clock.cpp
#	${MEGA_API_DIR}/mega/clock.hpp
#	${MEGA_API_DIR}/mega/common.hpp
#	${MEGA_API_DIR}/mega/coroutine.hpp
#	${MEGA_API_DIR}/mega/default_traits.hpp
#	${MEGA_API_DIR}/mega/eg_common_strings.hpp
#	${MEGA_API_DIR}/mega/event.hpp
#	${MEGA_API_DIR}/mega/event_log.cpp
#	${MEGA_API_DIR}/mega/frame.hpp
#	${MEGA_API_DIR}/mega/frame.cpp
#	${MEGA_API_DIR}/mega/include.hpp
#	${MEGA_API_DIR}/mega/iterators.hpp
#	${MEGA_API_DIR}/mega/macros.hpp
#	${MEGA_API_DIR}/mega/operations.hpp
#	${MEGA_API_DIR}/mega/resource.cpp
#	${MEGA_API_DIR}/mega/resource.hpp
#	${MEGA_API_DIR}/mega/result_type.hpp
#	${MEGA_API_DIR}/mega/scheduler.hpp
#	${MEGA_API_DIR}/mega/basic_scheduler.cpp
#	${MEGA_API_DIR}/mega/type_path.hpp
#	${MEGA_API_DIR}/mega/variant.hpp
#	)
	
set( MEGA_DATABASE_MODEL

    ${MEGA_API_DIR}/database/model/data.hxx
    ${MEGA_API_DIR}/database/model/view.hxx

	)
	
set( MEGA_DATABASE_IO
    ${MEGA_API_DIR}/database/io/object_info.hpp
    ${MEGA_API_DIR}/database/io/file_info.hpp
    ${MEGA_API_DIR}/database/io/component_info.hpp
    ${MEGA_API_DIR}/database/io/environment.hpp
    ${MEGA_API_DIR}/database/io/manifest.hpp
    ${MEGA_API_DIR}/database/io/stages.hpp
    ${MEGA_API_DIR}/database/io/file_system.hpp
    ${MEGA_API_DIR}/database/io/archive.hpp
    ${MEGA_API_DIR}/database/io/database.hpp
    ${MEGA_API_DIR}/database/io/factory.hpp
    ${MEGA_API_DIR}/database/io/file.hpp
    ${MEGA_API_DIR}/database/io/generics.hpp
    ${MEGA_API_DIR}/database/io/loader.hpp
    ${MEGA_API_DIR}/database/io/object.hpp
    ${MEGA_API_DIR}/database/io/storer.hpp
	)
	
set( MEGA_DATABASE_SOURCES
    ${MEGA_SRC_DIR}/database/io/file_info.cpp
    ${MEGA_SRC_DIR}/database/io/component_info.cpp
    ${MEGA_SRC_DIR}/database/io/environment.cpp
    ${MEGA_SRC_DIR}/database/io/manifest.cpp
    ${MEGA_SRC_DIR}/database/io/database.cpp
    ${MEGA_SRC_DIR}/database/io/factory.cpp
    ${MEGA_SRC_DIR}/database/io/file.cpp
    ${MEGA_SRC_DIR}/database/io/loader.cpp
    ${MEGA_SRC_DIR}/database/io/object.cpp
    ${MEGA_SRC_DIR}/database/io/storer.cpp
    
    ${MEGA_SRC_DIR}/database/model/data.cxx
    ${MEGA_SRC_DIR}/database/model/view.cxx
    
    
    )
    
add_library( database
                    ${MEGA_DATABASE_IO}
					${MEGA_DATABASE_MODEL}
					${MEGA_DATABASE_SOURCES}
				)

add_dependencies( database database_generation )
                

target_compile_definitions(database PUBLIC -DBOOST_STACKTRACE_USE_ADDR2LINE)
set_target_properties( database PROPERTIES FOLDER database )
set_property(TARGET database PROPERTY POSITION_INDEPENDENT_CODE ON)

link_boost( database filesystem )
link_boost( database iostreams )
link_boost( database serialization )
link_boost( database system )
link_boost( database atomic )
link_common( database ) 

#install( FILES ${MEGA_LIBRARY} DESTINATION include/database/mega )
install( FILES ${MEGA_DATABASE_IO} DESTINATION include/database/io )
install( FILES ${MEGA_DATABASE_MODEL} DESTINATION include/database/model )
install( TARGETS database EXPORT database DESTINATION bin)
install( EXPORT database NAMESPACE MEGA:: FILE database-config.cmake DESTINATION share )


add_c_preprocessor_command( OUTPUT stages.hpp.i SOURCE ${MEGA_API_DIR}/database/io/stages.hpp TARGET database )
add_custom_target( PreprocessStages
	SOURCES stages.hpp.i
	COMMENT "Proprocessing stages.hpp")

add_c_preprocessor_command( OUTPUT file_system.hpp.i SOURCE ${MEGA_API_DIR}/database/io/file_system.hpp TARGET database )
add_custom_target( PreprocessFileSystem
    SOURCES file_system.hpp.i
    COMMENT "Proprocessing file_system.hpp")
