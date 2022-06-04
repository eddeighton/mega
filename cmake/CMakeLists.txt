

cmake_minimum_required( VERSION 3.1...3.16 )

project( mega VERSION 1.0 LANGUAGES CXX )

#insist on ALL projects using c++ 17
set( CMAKE_CXX_STANDARD 17 )

######################################
#win32 specific settings
IF( ${WIN32} )

add_definitions(-D_WIN32_WINNT=0x0601)
add_definitions(-DNOMINMAX -D_NOMINMAX)

ENDIF( ${WIN32} )

set_property(GLOBAL PROPERTY LLVM_ENABLE_RTTI ON )
set_property(GLOBAL PROPERTY LLVM_ENABLE_EH ON )
set_property(GLOBAL PROPERTY POSITION_INDEPENDENT_CODE ON)

#setup some common directories from the root of the project
set( CMAKE_PARENT_PATH ${CMAKE_CURRENT_LIST_DIR}/.. )
find_path( MEGA_ROOT_DIR NAMES api PATHS ${CMAKE_PARENT_PATH} )
set( MEGA_API_DIR ${MEGA_ROOT_DIR}/api )
set( MEGA_SRC_DIR ${MEGA_ROOT_DIR}/src )

#enable folders within generated IDE build systems
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#ensure language runtimes and other system libs are copied to installation folder
include (InstallRequiredSystemLibraries)

add_subdirectory(version)
add_subdirectory(pipeline)
add_subdirectory(database)
add_subdirectory(clang_plugin)
add_subdirectory(parser)
add_subdirectory(compiler)
add_subdirectory(utilities)
add_subdirectory(service)
add_subdirectory(driver)
add_subdirectory(tests)
add_subdirectory(mega)
add_subdirectory(toolchain)

# install scripts
set( MEGA_SCRIPTS
    ${MEGA_ROOT_DIR}/scripts/run_service.sh 
)

install( FILES ${MEGA_SCRIPTS} DESTINATION bin )