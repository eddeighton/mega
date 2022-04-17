
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

include_directories( ${MEGA_API_DIR} )
include_directories( ${MEGA_SRC_DIR} )

add_executable( database_compiler_driver ${MEGA_SRC_DIR}/database/compiler/main.cpp )

target_compile_definitions(database_compiler_driver PUBLIC -DBOOST_STACKTRACE_USE_ADDR2LINE)

set_target_properties( database_compiler_driver PROPERTIES FOLDER database_compiler_driver )

link_boost( database_compiler_driver filesystem )
link_boost( database_compiler_driver system )
link_boost( database_compiler_driver atomic )
link_boost( database_compiler_driver program_options )
link_common( database_compiler_driver ) 

target_link_libraries(database_compiler_driver database_compiler)

install( TARGETS database_compiler_driver DESTINATION bin)

add_dependencies( database_compiler_driver database_compiler )