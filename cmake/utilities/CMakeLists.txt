cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get clang
include( ${WORKSPACE_ROOT_PATH}/thirdparty/llvm/clang_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get inja
include( ${WORKSPACE_ROOT_PATH}/thirdparty/inja/inja_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( UTILITIES_HEADERS
    ${MEGA_API_DIR}/utilities/glob.hpp
    ${MEGA_API_DIR}/utilities/clang_format.hpp
    ${MEGA_API_DIR}/utilities/cmake.hpp
    ${MEGA_API_DIR}/utilities/tool_chain_hash.hpp
    ${MEGA_API_DIR}/utilities/serialization_helpers.hpp
)

set( UTILITIES_SOURCE
    ${MEGA_SRC_DIR}/utilities/glob.cpp
    ${MEGA_SRC_DIR}/utilities/clang_format.cpp
    ${MEGA_SRC_DIR}/utilities/tool_chain_hash.cpp
)


add_library( utilities
    ${UTILITIES_HEADERS} 
    ${UTILITIES_SOURCE}
)

#target_link_libraries( utilities version )

target_include_directories( utilities PUBLIC ${MEGA_API_DIR} )

set_property(TARGET utilities PROPERTY POSITION_INDEPENDENT_CODE ON)

link_boost( utilities atomic )
link_boost( utilities program_options )
link_boost( utilities timer )
link_boost( utilities filesystem )
link_boost( utilities iostreams )
link_boost( utilities serialization )
link_clang( utilities )
link_json( utilities )
link_inja( utilities )
link_common( utilities )

install( FILES ${UTILITIES_HEADERS} DESTINATION include/utilities )
install( TARGETS utilities EXPORT utilities DESTINATION bin)
install( EXPORT utilities NAMESPACE MEGA:: FILE utilities-config.cmake DESTINATION share )