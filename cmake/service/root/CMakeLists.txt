
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get spdlog
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )


#get the source code
set( ROOT_SOURCE_DIR ${MEGA_SRC_DIR}/service/root )

set( ROOT_SOURCE 
    ${ROOT_SOURCE_DIR}/logical_address_space.hpp
    ${ROOT_SOURCE_DIR}/mpo_manager.hpp
    ${ROOT_SOURCE_DIR}/root.hpp
    ${ROOT_SOURCE_DIR}/root.cpp
    ${ROOT_SOURCE_DIR}/pipeline.hpp
    ${ROOT_SOURCE_DIR}/pipeline.cpp
    ${ROOT_SOURCE_DIR}/job.hpp
    ${ROOT_SOURCE_DIR}/job.cpp
    ${ROOT_SOURCE_DIR}/simulation.hpp
    ${ROOT_SOURCE_DIR}/simulation.cpp
    ${ROOT_SOURCE_DIR}/main.cpp
    ${ROOT_SOURCE_DIR}/request.hpp
    ${ROOT_SOURCE_DIR}/routing.cpp
    ${ROOT_SOURCE_DIR}/project.cpp
    ${ROOT_SOURCE_DIR}/enrole.cpp
    ${ROOT_SOURCE_DIR}/status.cpp
    ${ROOT_SOURCE_DIR}/stash.cpp
 )

add_executable( root ${ROOT_SOURCE} )

target_compile_options( root PUBLIC "-fcoroutines" )

target_link_libraries( root utilities )
target_link_libraries( root protocol )
target_link_libraries( root network )
target_link_libraries( root pipeline )

set_target_properties( root PROPERTIES FOLDER driver )

link_boost( root program_options )
link_boost( root filesystem )
link_boost( root iostreams )
link_boost( root serialization )
link_boost( root system )
link_boost( root atomic )
link_spdlog( root )
link_common( root )

install( TARGETS root DESTINATION bin)
