
cmake_minimum_required( VERSION 3.1...3.16 )

project( megatest )

set( CMAKE_CXX_STANDARD 17 )

include( ${WORKSPACE_ROOT_PATH}/src/mega/mega_include.cmake )

MegaProjectBegin( megatest "cube.mega;square.mega" )

add_subdirectory(stuff)

MegaProjectEnd( megatest )
