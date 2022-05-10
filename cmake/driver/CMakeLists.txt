
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get inja
include( ${WORKSPACE_ROOT_PATH}/thirdparty/inja/inja_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

#get handy function to preprocess files for the xmacros stuff
include( ../preprocess_cpp.cmake )

include_directories( ${MEGA_API_DIR} )
include_directories( ${MEGA_SRC_DIR} )

#get the source code
set( DRIVER_SOURCE_DIR ${MEGA_SRC_DIR}/driver )

set( JINJA_TEMPLATES 
	${DRIVER_SOURCE_DIR}/graphviz.jinja 
	${DRIVER_SOURCE_DIR}/context.jinja 
	${DRIVER_SOURCE_DIR}/namespace.jinja 
	${DRIVER_SOURCE_DIR}/interface.jinja 
	)

set( MEGA_LIBRARY
	${MEGA_API_DIR}/mega/allocator.hpp
	${MEGA_API_DIR}/mega/backtrace.hpp
	${MEGA_API_DIR}/mega/basic_scheduler.cpp 
	${MEGA_API_DIR}/mega/clock.cpp
	${MEGA_API_DIR}/mega/clock.hpp
	${MEGA_API_DIR}/mega/common_strings.hpp 
	${MEGA_API_DIR}/mega/common.hpp
	${MEGA_API_DIR}/mega/coroutine.hpp
	${MEGA_API_DIR}/mega/default_traits.hpp
	${MEGA_API_DIR}/mega/eg_common_strings.hpp
	${MEGA_API_DIR}/mega/event_log.cpp
	${MEGA_API_DIR}/mega/event.hpp
	${MEGA_API_DIR}/mega/frame.cpp
	${MEGA_API_DIR}/mega/frame.hpp
	${MEGA_API_DIR}/mega/include.hpp
	${MEGA_API_DIR}/mega/iterators.hpp
	${MEGA_API_DIR}/mega/macros.hpp
	${MEGA_API_DIR}/mega/operations.hpp
	${MEGA_API_DIR}/mega/resource.cpp
	${MEGA_API_DIR}/mega/resource.hpp
	${MEGA_API_DIR}/mega/result_type.hpp
	${MEGA_API_DIR}/mega/scheduler.hpp
	${MEGA_API_DIR}/mega/type_path.hpp
	${MEGA_API_DIR}/mega/variant.hpp
)


set( DRIVER_SOURCES
		${DRIVER_SOURCE_DIR}/command_archive.cpp
		${DRIVER_SOURCE_DIR}/command_graph.cpp
		${DRIVER_SOURCE_DIR}/command_inja.cpp
		${DRIVER_SOURCE_DIR}/command_json.cpp
		${DRIVER_SOURCE_DIR}/command_list.cpp
		${DRIVER_SOURCE_DIR}/command_manifest.cpp

		${DRIVER_SOURCE_DIR}/command_interface/base_task.hpp  
		${DRIVER_SOURCE_DIR}/command_interface/task_dependencies.hpp 
		${DRIVER_SOURCE_DIR}/command_interface/task_includes.hpp  
		${DRIVER_SOURCE_DIR}/command_interface/task_interface_analysis.hpp  
		${DRIVER_SOURCE_DIR}/command_interface/task_interface.hpp  
		${DRIVER_SOURCE_DIR}/command_interface/task_parse_ast.hpp  
		${DRIVER_SOURCE_DIR}/command_interface/task_symbols.hpp

		${DRIVER_SOURCE_DIR}/command_interface.cpp
		
		${DRIVER_SOURCE_DIR}/main.cpp
	)
	
source_group( src FILES ${DRIVER_SOURCES} )

add_c_preprocessor_command( OUTPUT main.cpp.i SOURCE ${DRIVER_SOURCE_DIR}/main.cpp TARGET mega )
add_custom_target( PreprocessMain 
	SOURCES main.cpp.i 
	COMMENT "Proprocessing main.cpp")

add_executable( mega ${DRIVER_SOURCES} ${GENERATOR_SOURCES} )

target_link_libraries( mega database )
target_link_libraries( mega utilities )

set_target_properties( mega PROPERTIES FOLDER driver )

link_boost( mega atomic )
link_boost( mega program_options )
link_boost( mega timer )
link_boost( mega filesystem )
link_boost( mega iostreams )
link_boost( mega serialization )
link_spdlog( mega )
link_json( mega )
link_inja( mega )
link_common( mega )

install( TARGETS mega DESTINATION bin)
install( FILES ${JINJA_TEMPLATES} DESTINATION templates )
install( FILES ${MEGA_LIBRARY} DESTINATION include/mega )
