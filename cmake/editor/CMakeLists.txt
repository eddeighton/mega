##  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
##  Author: Edward Deighton
##  License: Please see license.txt in the project root folder.

##  Use and copying of this software and preparation of derivative works
##  based upon this software are permitted. Any copy of this software or
##  of any derivative work must include the above copyright notice, this
##  paragraph and the one after it.  Any distribution of this software or
##  derivative works must comply with all applicable laws.

##  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
##  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
##  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
##  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
##  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
##  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
##  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
##  OF THE POSSIBILITY OF SUCH DAMAGES.

cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get cgal
include( ${WORKSPACE_ROOT_PATH}/thirdparty/cgal/cgal_include.cmake )

#get ads
include( ${WORKSPACE_ROOT_PATH}/thirdparty/qtdock2/qtdock2_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

find_package(Qt6 COMPONENTS Core Widgets Gui REQUIRED )

set( EDITOR_API_DIR ${MEGA_API_DIR}/editor )
set( EDITOR_SRC_DIR ${MEGA_SRC_DIR}/editor )

set( EDITOR_HEADERS

    ${EDITOR_SRC_DIR}/document.hpp
    ${EDITOR_SRC_DIR}/glyphs.hpp
    ${EDITOR_SRC_DIR}/glyphView.hpp
    ${EDITOR_SRC_DIR}/gridView.hpp
    ${EDITOR_SRC_DIR}/mainWindow.hpp
    ${EDITOR_SRC_DIR}/messages.hpp
    ${EDITOR_SRC_DIR}/qtUtils.hpp
    ${EDITOR_SRC_DIR}/schematicView.hpp
    ${EDITOR_SRC_DIR}/toolbox.hpp
    ${EDITOR_SRC_DIR}/toolboxPanel.hpp
    ${EDITOR_SRC_DIR}/tools.hpp
    ${EDITOR_SRC_DIR}/voronoi.hpp

)

set( EDITOR_SOURCE

    ${EDITOR_SRC_DIR}/document.cpp
    ${EDITOR_SRC_DIR}/glyphs.cpp
    ${EDITOR_SRC_DIR}/glyphView.cpp
    ${EDITOR_SRC_DIR}/gridView.cpp
    ${EDITOR_SRC_DIR}/main.cpp 
    ${EDITOR_SRC_DIR}/mainWindow.cpp
    ${EDITOR_SRC_DIR}/mainWindow.ui
    ${EDITOR_SRC_DIR}/resources.qrc
    ${EDITOR_SRC_DIR}/schematicView.cpp
    ${EDITOR_SRC_DIR}/toolbox.cpp
    ${EDITOR_SRC_DIR}/toolboxPanel.cpp
    ${EDITOR_SRC_DIR}/tools.cpp
)

add_executable( editor
    ${EDITOR_HEADERS} 
    ${EDITOR_SOURCE}
)

target_include_directories( editor PRIVATE ${MEGA_API_DIR} )
target_include_directories( editor PRIVATE ${EDITOR_SRC_DIR} )

set_target_properties( editor PROPERTIES FOLDER editor )

IF( ${WIN32} )
set_property(TARGET editor PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON )
ENDIF( ${WIN32} )

set_target_properties( editor PROPERTIES 
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
    CXX_STANDARD 17
    #CXX_STANDARD_REQUIRED ON
    #CXX_EXTENSIONS OFF
    #VERSION ${VERSION_SHORT}
    EXPORT_NAME "Editor"
    #ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
    #LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
    #RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/bin"
)

target_link_libraries( editor Qt6::Core) 
target_link_libraries( editor Qt6::Gui) 
target_link_libraries( editor Qt6::Widgets) 
target_link_libraries( editor mega_interface )
target_link_libraries( editor map )

link_cgal( editor )
link_qtdock2( editor )
link_common( editor )
#link_boost( editor serialization )


install( FILES ${EDITOR_HEADERS} DESTINATION include/editor )
install( FILES ${EDITOR_SRC_DIR}/config.ed DESTINATION bin )
install( TARGETS editor EXPORT editor DESTINATION bin)
install( EXPORT editor NAMESPACE MEGA:: FILE editor-config.cmake DESTINATION share )