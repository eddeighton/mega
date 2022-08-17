cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get clang
include( ${WORKSPACE_ROOT_PATH}/thirdparty/llvm/clang_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( RUNTIME_SOURCE
    ${MEGA_SRC_DIR}/runtime/jit.hpp
    ${MEGA_SRC_DIR}/runtime/jit.cpp
)

add_library( jit SHARED
    ${RUNTIME_HEADERS} 
    ${RUNTIME_SOURCE}
)

#target_link_options( jit PUBLIC -export-dynamic )
#set_property(TARGET jit PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories( jit PUBLIC ${MEGA_API_DIR} )

link_boost( jit system )
link_common( jit )
link_orc( jit )

install( FILES ${RUNTIME_HEADERS} DESTINATION include/runtime )
install( TARGETS jit EXPORT jit DESTINATION bin)
install( EXPORT jit NAMESPACE MEGA:: FILE jit-config.cmake DESTINATION share )
