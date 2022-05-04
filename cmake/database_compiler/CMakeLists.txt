
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get inja
include( ${WORKSPACE_ROOT_PATH}/thirdparty/inja/inja_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

include_directories( ${MEGA_API_DIR} )
include_directories( ${MEGA_SRC_DIR} )

set( MEGA_DATABASE_COMPILER
    ${MEGA_SRC_DIR}/database/compiler/generator.hpp
    ${MEGA_SRC_DIR}/database/compiler/generator.cpp
    ${MEGA_SRC_DIR}/database/compiler/grammar.hpp
    ${MEGA_SRC_DIR}/database/compiler/grammar.cpp
    ${MEGA_SRC_DIR}/database/compiler/json_converter.hpp
    ${MEGA_SRC_DIR}/database/compiler/json_converter.cpp
    ${MEGA_SRC_DIR}/database/compiler/model.hpp
    ${MEGA_SRC_DIR}/database/compiler/model.cpp
	)
	
add_library( database_compiler
                    ${MEGA_DATABASE_COMPILER}
				)

set_target_properties( database_compiler PROPERTIES FOLDER database_compiler )

link_boost( database_compiler filesystem )
link_boost( database_compiler iostreams )
link_boost( database_compiler serialization )
link_boost( database_compiler system )
link_boost( database_compiler atomic )

link_json( database_compiler )
link_inja( database_compiler )

link_common( database_compiler ) 

install( TARGETS database_compiler DESTINATION bin)