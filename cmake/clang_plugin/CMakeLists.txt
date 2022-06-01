
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get clang
include( ${WORKSPACE_ROOT_PATH}/thirdparty/llvm/clang_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

include_directories( ${MEGA_API_DIR} )
include_directories( ${MEGA_SRC_DIR} )

set( CLANG_PLUGIN_API ${MEGA_API_DIR}/clang_plugin/clang_plugin.hpp )

set( CLANG_PLUGIN_SOURCES
    ${MEGA_SRC_DIR}/clang_plugin/clang_plugin.cpp
    ${MEGA_SRC_DIR}/clang_plugin/clang_utils.hpp
    ${MEGA_SRC_DIR}/clang_plugin/clang_utils.cpp
    ${MEGA_SRC_DIR}/clang_plugin/session.hpp
    ${MEGA_SRC_DIR}/clang_plugin/interface_analysis.cpp
    )

add_definitions(-DEG_DATABASE_SHARED_MODULE)

# use the LLVM provided cmake macro - need the plugin to IMPORT the llvm symbols
#add_llvm_library( clang_plugin MODULE 
#    ${CLANG_PLUGIN_API}
#    ${CLANG_PLUGIN_SOURCES} 
#    PLUGIN_TOOL clang )

add_library( clang_plugin MODULE
                    ${CLANG_PLUGIN_API}
                    ${CLANG_PLUGIN_SOURCES} )

set_target_properties( clang_plugin PROPERTIES FOLDER clang_plugin )

link_boost( clang_plugin filesystem )
link_boost( clang_plugin iostreams )
link_boost( clang_plugin serialization )
link_common( clang_plugin )

# import clang - symbols linked dynamically when load plugin
import_clang( clang_plugin )

target_link_libraries( clang_plugin database )

install( FILES ${CLANG_PLUGIN_API} DESTINATION include/clang_plugin )
install( TARGETS clang_plugin DESTINATION bin)
