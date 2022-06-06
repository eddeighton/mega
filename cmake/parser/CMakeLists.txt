
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get clang
include( ${WORKSPACE_ROOT_PATH}/thirdparty/llvm/clang_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

include_directories( ${MEGA_API_DIR} )
include_directories( ${MEGA_SRC_DIR} )

set( CLANG_PLUGIN_API ${MEGA_API_DIR}/parser/parser.hpp )

set( CLANG_PLUGIN_SOURCES
    ${MEGA_SRC_DIR}/parser/clang.hpp
    ${MEGA_SRC_DIR}/parser/clang.cpp
    ${MEGA_SRC_DIR}/parser/parser.cpp
    )

add_definitions(-DEG_DATABASE_SHARED_MODULE)

add_library( parser SHARED
                    ${CLANG_PLUGIN_API}
                    ${CLANG_PLUGIN_SOURCES} )

set_property(TARGET parser PROPERTY LLVM_ENABLE_RTTI ON )
set_property(TARGET parser PROPERTY LLVM_ENABLE_EH ON )
set_property(TARGET parser PROPERTY POSITION_INDEPENDENT_CODE ON)

set_target_properties( parser PROPERTIES FOLDER parser )

target_link_libraries( parser utilities )
target_link_libraries( parser database )
target_link_libraries( parser pipeline )

link_boost( parser filesystem )
link_boost( parser iostreams )
link_boost( parser serialization )
link_boost( parser system )
link_clang( parser )
link_json( parser )
link_common( parser )

install( FILES ${CLANG_PLUGIN_API} DESTINATION include/parser )
install( TARGETS parser DESTINATION bin)
