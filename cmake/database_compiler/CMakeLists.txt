
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get inja
include( ${WORKSPACE_ROOT_PATH}/thirdparty/inja/inja_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

#get handy function to preprocess files for the xmacros stuff
include( ../preprocess_cpp.cmake )

#set( MEGA_DATABASE_TEMPLATES_DIR ${MEGA_SRC_DIR}/database_compiler/templates )
#set( MEGA_DATABASE_TEMPLATES
#    ${MEGA_DATABASE_TEMPLATES_DIR}/view.inja
#)

set( MEGA_DATABASE_COMPILER
    ${MEGA_SRC_DIR}/database_compiler/json_converter.hpp
    ${MEGA_SRC_DIR}/database_compiler/json_converter.cpp
    ${MEGA_SRC_DIR}/database_compiler/generator.hpp
    ${MEGA_SRC_DIR}/database_compiler/generator.cpp
    ${MEGA_SRC_DIR}/database_compiler/model.hpp
    ${MEGA_SRC_DIR}/database_compiler/model.cpp
    ${MEGA_SRC_DIR}/database_compiler/main.cpp
	)
	
add_executable( database_compiler
                    ${MEGA_DATABASE_COMPILER}
				)

target_compile_definitions(database_compiler PUBLIC -DBOOST_STACKTRACE_USE_ADDR2LINE)

set_target_properties( database_compiler PROPERTIES FOLDER database_compiler )

link_boost( database_compiler filesystem )
link_boost( database_compiler iostreams )
link_boost( database_compiler serialization )
link_boost( database_compiler system )
link_boost( database_compiler atomic )
link_boost( database_compiler program_options )

link_json( database_compiler )
link_inja( database_compiler )

link_common( database_compiler ) 

install( TARGETS database_compiler DESTINATION bin)
#install( FILES ${MEGA_DATABASE_TEMPLATES} DESINATION templates )

