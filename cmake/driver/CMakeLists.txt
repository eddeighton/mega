
#  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
#  Author: Edward Deighton
#  License: Please see license.txt in the project root folder.
#
#  Use and copying of this software and preparation of derivative works
#  based upon this software are permitted. Any copy of this software or
#  of any derivative work must include the above copyright notice, this
#  paragraph and the one after it.  Any distribution of this software or
#  derivative works must comply with all applicable laws.
#
#  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
#  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
#  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
#  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
#  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
#  OF THE POSSIBILITY OF SUCH DAMAGES.


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
	)

set( DRIVER_SOURCES
		${DRIVER_SOURCE_DIR}/command_archive.cpp
		${DRIVER_SOURCE_DIR}/command_graph.cpp
		${DRIVER_SOURCE_DIR}/command_inja.cpp
		${DRIVER_SOURCE_DIR}/command_json.cpp
		${DRIVER_SOURCE_DIR}/command_component_listing.cpp
		${DRIVER_SOURCE_DIR}/command_make_pipeline.cpp
		${DRIVER_SOURCE_DIR}/command_execute_pipeline.cpp
		${DRIVER_SOURCE_DIR}/command_tool_chain.cpp
		${DRIVER_SOURCE_DIR}/command_new_install.cpp
		${DRIVER_SOURCE_DIR}/command_project.cpp
		${DRIVER_SOURCE_DIR}/command_sim.cpp
		${DRIVER_SOURCE_DIR}/command_status.cpp
		${DRIVER_SOURCE_DIR}/command_stash.cpp
		
		${DRIVER_SOURCE_DIR}/main.cpp

		## ensure the version is baked into the shared object
		${MEGA_SRC_DIR}/version/version.cxx
	)
	
source_group( src FILES ${DRIVER_SOURCES} )

add_c_preprocessor_command( OUTPUT main.cpp.i SOURCE ${DRIVER_SOURCE_DIR}/main.cpp TARGET mega )
add_custom_target( PreprocessMain 
	SOURCES main.cpp.i 
	COMMENT "Proprocessing main.cpp")

add_executable( mega ${DRIVER_SOURCES} ${GENERATOR_SOURCES} )

target_link_libraries( mega compiler_config )
target_link_libraries( mega database )
target_link_libraries( mega utilities )
target_link_libraries( mega protocol )
target_link_libraries( mega network )
target_link_libraries( mega terminal )

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

