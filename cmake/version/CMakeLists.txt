


cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

string( TIMESTAMP CMAKE_TIMESTAMP_STRING "%Y-%m-%d %H:%M:%S" UTC )

set( CMAKE_VERSION_MAJOR        0 )
set( CMAKE_VERSION_MINOR        0 )
set( CMAKE_VERSION_REVISION     0 )
set( CMAKE_VERSION_BUILD        ${CMAKE_TIMESTAMP_STRING} )

configure_file( ${MEGA_SRC_DIR}/version/version.cpp.in ${MEGA_SRC_DIR}/version/version.cxx )

set( VERSION_HEADERS
    ${MEGA_API_DIR}/version/version.hpp )

#add_library( version STATIC
#    ${VERSION_HEADERS} 
#    ${MEGA_SRC_DIR}/version/version.cxx )

#set_property(TARGET version PROPERTY POSITION_INDEPENDENT_CODE ON)

#target_include_directories( version PUBLIC ${MEGA_API_DIR} )

#link_boost( version serialization )

install( FILES ${VERSION_HEADERS} DESTINATION include/version )
#install( TARGETS version EXPORT version DESTINATION lib)
#install( EXPORT version NAMESPACE MEGA:: FILE version-config.cmake DESTINATION share )
