
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

#get the source code
set( DRIVER_SOURCE_DIR ${MEGA_SRC_DIR}/driver )

set( DRIVER_SOURCES
	${DRIVER_SOURCE_DIR}/environment.hpp
	${DRIVER_SOURCE_DIR}/environment.cpp
	${DRIVER_SOURCE_DIR}/command_list.cpp
	${DRIVER_SOURCE_DIR}/command_interface.cpp
	${DRIVER_SOURCE_DIR}/main.cpp
	)
	
source_group( src FILES ${DRIVER_SOURCES} )

add_executable( mega ${DRIVER_SOURCES} ${GENERATOR_SOURCES} )

target_link_libraries(mega database)

set_target_properties( mega PROPERTIES FOLDER driver )

link_boost( mega atomic )
link_boost( mega program_options )
link_boost( mega timer )
link_boost( mega filesystem )
link_boost( mega iostreams )
link_boost( mega serialization )
link_common( mega )

install( TARGETS mega DESTINATION bin)