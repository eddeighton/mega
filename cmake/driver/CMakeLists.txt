
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

#get the source code
set( DRIVER_SOURCE_DIR ${MEGA_SRC_DIR}/driver )

set( DRIVER_SOURCES
	#${DRIVER_SOURCE_DIR}/command_build.cpp
	#${DRIVER_SOURCE_DIR}/command_clean.cpp
	#${DRIVER_SOURCE_DIR}/command_create.cpp
	#${DRIVER_SOURCE_DIR}/command_info.cpp
	#${DRIVER_SOURCE_DIR}/command_log.cpp
	#${DRIVER_SOURCE_DIR}/command_run.cpp
	${DRIVER_SOURCE_DIR}/main.cpp
	#${DRIVER_SOURCE_DIR}/stacktrace.cpp
	#${DRIVER_SOURCE_DIR}/build_tools.hpp
	#${DRIVER_SOURCE_DIR}/build_tools.cpp
	#${DRIVER_SOURCE_DIR}/build_interface.hpp
	#${DRIVER_SOURCE_DIR}/build_interface.cpp
	#${DRIVER_SOURCE_DIR}/build_implementation.hpp
	#${DRIVER_SOURCE_DIR}/build_implementation.cpp
	)
	
#set( GENERATOR_SOURCE_DIR ${MEGA_ROOT_DIR}/src/generators )

#set( GENERATOR_SOURCES 
#	${GENERATOR_SOURCE_DIR}/generator.cpp 
#    ${GENERATOR_SOURCE_DIR}/python_generator.cpp
#    ${GENERATOR_SOURCE_DIR}/instructionCodeGenerator.cpp
#    ${GENERATOR_SOURCE_DIR}/unreal_generator.cpp
#    ${GENERATOR_SOURCE_DIR}/geometry_generator.cpp
#    ${GENERATOR_SOURCE_DIR}/configio_generator.cpp
#    )

source_group( src FILES ${DRIVER_SOURCES} )

add_executable( mega ${DRIVER_SOURCES} ${GENERATOR_SOURCES} )

set_target_properties( mega PROPERTIES FOLDER driver )

link_boost( mega atomic )
link_boost( mega program_options )
link_boost( mega timer )
link_boost( mega filesystem )
link_boost( mega iostreams )
link_boost( mega serialization )
link_common( mega )

install( TARGETS mega DESTINATION bin)