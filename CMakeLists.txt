
cmake_minimum_required( VERSION 3.1...3.16 )

project( megatest )

set( CMAKE_CXX_STANDARD 17 )

set( MEGA_SRC_ROOT ${CMAKE_SOURCE_DIR} )
set( MEGA_BUILD_ROOT ${CMAKE_BINARY_DIR} )

add_subdirectory(retail_test)

include( ${WORKSPACE_ROOT_PATH}/src/mega/mega_include.cmake )
include( ${WORKSPACE_ROOT_PATH}/thirdparty/spdlog/spdlog_include.cmake )
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

MegaProject( MegaTest )

MegaComponent( MegaTestComponent1 CPP test.cpp MEGA cube.mega square.mega )

link_spdlog( MegaTestComponent1 )
link_boost( MegaTestComponent1 filesystem )

add_subdirectory(stuff)

