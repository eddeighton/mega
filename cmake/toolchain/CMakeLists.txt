

cmake_minimum_required(VERSION 3.2)


file( GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/installation_version.cmake INPUT ${CMAKE_CURRENT_LIST_DIR}/installation_version.cmake.in  )

install( SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/installation_version.cmake )
