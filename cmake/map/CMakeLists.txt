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

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( MAP_API_DIR ${MEGA_API_DIR}/map )

set( MAP_HEADERS
    ${MAP_API_DIR}/base.hpp
    ${MAP_API_DIR}/basicFeature.hpp
    ${MAP_API_DIR}/buffer.hpp
    ${MAP_API_DIR}/cgalSettings.hpp
    ${MAP_API_DIR}/cgalUtils.hpp
    ${MAP_API_DIR}/clip.hpp
    ${MAP_API_DIR}/compilation.hpp
    ${MAP_API_DIR}/connection.hpp
    ${MAP_API_DIR}/container.hpp
    ${MAP_API_DIR}/dataBitmap.hpp
    ${MAP_API_DIR}/editBase.hpp
    ${MAP_API_DIR}/editInteractions.hpp
    ${MAP_API_DIR}/editSchematic.hpp
    ${MAP_API_DIR}/editNested.hpp
    ${MAP_API_DIR}/editRoot.hpp
    ${MAP_API_DIR}/factory.hpp
    ${MAP_API_DIR}/file.hpp
    ${MAP_API_DIR}/geometry.hpp
    ${MAP_API_DIR}/glyph.hpp
    ${MAP_API_DIR}/glyphSpec.hpp
    ${MAP_API_DIR}/glyphSpecProducer.hpp
    ${MAP_API_DIR}/location.hpp
    ${MAP_API_DIR}/markup.hpp
    ${MAP_API_DIR}/mission.hpp
    ${MAP_API_DIR}/node.hpp
    ${MAP_API_DIR}/object.hpp
    ${MAP_API_DIR}/property.hpp
    ${MAP_API_DIR}/rasteriser.hpp
    ${MAP_API_DIR}/readerWriter.hpp
    ${MAP_API_DIR}/schematic.hpp
    ${MAP_API_DIR}/sector.hpp
    ${MAP_API_DIR}/ship.hpp
    ${MAP_API_DIR}/site.hpp
    ${MAP_API_DIR}/space.hpp
    ${MAP_API_DIR}/svgUtils.hpp
    ${MAP_API_DIR}/transform.hpp
    ${MAP_API_DIR}/visibility.hpp
    ${MAP_API_DIR}/wall.hpp
)

set( MAP_SRC_DIR ${MEGA_SRC_DIR}/map )

set( MAP_SOURCE
    ${MAP_SRC_DIR}/base.cpp
    ${MAP_SRC_DIR}/basicFeature.cpp
    ${MAP_SRC_DIR}/cgalUtils.cpp
    ${MAP_SRC_DIR}/clip.cpp
    ${MAP_SRC_DIR}/compilation.cpp
    ${MAP_SRC_DIR}/compilationGetPolyInfo.cpp
    ${MAP_SRC_DIR}/connection.cpp
    ${MAP_SRC_DIR}/dataBitmap.cpp
    ${MAP_SRC_DIR}/editBase.cpp
    ${MAP_SRC_DIR}/editInteractions.cpp
    ${MAP_SRC_DIR}/editSchematic.cpp
    ${MAP_SRC_DIR}/editNested.cpp
    ${MAP_SRC_DIR}/editRoot.cpp
    ${MAP_SRC_DIR}/factory.cpp
    ${MAP_SRC_DIR}/file.cpp
    ${MAP_SRC_DIR}/glyph.cpp
    ${MAP_SRC_DIR}/location.cpp
    ${MAP_SRC_DIR}/markup.cpp
    ${MAP_SRC_DIR}/mission.cpp
    ${MAP_SRC_DIR}/node.cpp
    ${MAP_SRC_DIR}/object.cpp
    ${MAP_SRC_DIR}/property.cpp
    ${MAP_SRC_DIR}/schematic.cpp
    ${MAP_SRC_DIR}/sector.cpp
    ${MAP_SRC_DIR}/ship.cpp
    ${MAP_SRC_DIR}/site.cpp
    ${MAP_SRC_DIR}/space.cpp
    ${MAP_SRC_DIR}/svgUtils.cpp
    ${MAP_SRC_DIR}/visibility.cpp
    ${MAP_SRC_DIR}/wall.cpp
)


add_library( map
    ${MAP_HEADERS} 
    ${MAP_SOURCE}
)

set_target_properties( map PROPERTIES FOLDER map )

target_include_directories( map PUBLIC ${MEGA_API_DIR} )


IF( ${WIN32} )
set_property(TARGET map PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON )
ELSE( ${WIN32} )

ENDIF( ${WIN32} )

target_link_libraries( map mega_interface )

link_boost( map system )
link_boost( map filesystem )
link_boost( map iostreams )
link_boost( map serialization )

link_cgal( map )
link_common( map )

install( FILES ${MAP_HEADERS} DESTINATION include/map )
install( TARGETS map EXPORT map DESTINATION bin)
install( EXPORT map NAMESPACE MEGA:: FILE map-config.cmake DESTINATION share )