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

#get flatbuffers
include( ${WORKSPACE_ROOT_PATH}/thirdparty/flatbuffers/flatbuffers_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

#get ed
include( ${WORKSPACE_ROOT_PATH}/src/ed/ed_include.cmake )

set( SCHEMATIC_API_DIR ${MEGA_API_DIR}/schematic )

set( SCHEMATIC_HEADERS
    ${SCHEMATIC_API_DIR}/analysis/edge_mask.hpp
    ${SCHEMATIC_API_DIR}/analysis/analysis.hpp
    ${SCHEMATIC_API_DIR}/analysis/invariant.hpp
    
    ${SCHEMATIC_API_DIR}/format/format.hpp

    ${SCHEMATIC_API_DIR}/buffer.hpp
    ${SCHEMATIC_API_DIR}/cgalSettings.hpp
    ${SCHEMATIC_API_DIR}/cgalUtils.hpp
    ${SCHEMATIC_API_DIR}/compilation_stage.hpp
    ${SCHEMATIC_API_DIR}/connection.hpp
    ${SCHEMATIC_API_DIR}/container.hpp
    ${SCHEMATIC_API_DIR}/cut.hpp
    ${SCHEMATIC_API_DIR}/dataBitmap.hpp
    ${SCHEMATIC_API_DIR}/editBase.hpp
    ${SCHEMATIC_API_DIR}/editInteractions.hpp
    ${SCHEMATIC_API_DIR}/editNested.hpp
    ${SCHEMATIC_API_DIR}/editRoot.hpp
    ${SCHEMATIC_API_DIR}/editSchematic.hpp
    ${SCHEMATIC_API_DIR}/factory.hpp
    ${SCHEMATIC_API_DIR}/feature.hpp
    ${SCHEMATIC_API_DIR}/file.hpp
    ${SCHEMATIC_API_DIR}/geometry.hpp
    ${SCHEMATIC_API_DIR}/glyph.hpp
    ${SCHEMATIC_API_DIR}/glyphSpec.hpp
    ${SCHEMATIC_API_DIR}/glyphSpecProducer.hpp
    ${SCHEMATIC_API_DIR}/markup.hpp
    ${SCHEMATIC_API_DIR}/node.hpp
    ${SCHEMATIC_API_DIR}/object.hpp
    ${SCHEMATIC_API_DIR}/property.hpp
    ${SCHEMATIC_API_DIR}/rasteriser.hpp
    ${SCHEMATIC_API_DIR}/schematic.hpp
    ${SCHEMATIC_API_DIR}/site.hpp
    ${SCHEMATIC_API_DIR}/space.hpp
    ${SCHEMATIC_API_DIR}/svgUtils.hpp
    ${SCHEMATIC_API_DIR}/transform.hpp
    ${SCHEMATIC_API_DIR}/wall.hpp
)

set( SCHEMATIC_SRC_DIR ${MEGA_SRC_DIR}/schematic )

set( SCHEMATIC_SOURCE

    ${SCHEMATIC_SRC_DIR}/analysis/algorithms.hpp
    ${SCHEMATIC_SRC_DIR}/analysis/analysis.cpp
    ${SCHEMATIC_SRC_DIR}/analysis/constructions.hpp
    ${SCHEMATIC_SRC_DIR}/analysis/edge_mask.cpp
    ${SCHEMATIC_SRC_DIR}/analysis/polygon_with_holes.hpp
    ${SCHEMATIC_SRC_DIR}/analysis/task_connections.cpp
    ${SCHEMATIC_SRC_DIR}/analysis/task_contours.cpp
    ${SCHEMATIC_SRC_DIR}/analysis/task_partitions.cpp
    ${SCHEMATIC_SRC_DIR}/analysis/task_properties.cpp
    ${SCHEMATIC_SRC_DIR}/analysis/task_skeleton.cpp

    ${SCHEMATIC_SRC_DIR}/format/format.cpp

    ${SCHEMATIC_SRC_DIR}/cgalUtils.cpp
    ${SCHEMATIC_SRC_DIR}/connection.cpp
    ${SCHEMATIC_SRC_DIR}/cut.cpp
    ${SCHEMATIC_SRC_DIR}/dataBitmap.cpp
    ${SCHEMATIC_SRC_DIR}/editBase.cpp
    ${SCHEMATIC_SRC_DIR}/editInteractions.cpp
    ${SCHEMATIC_SRC_DIR}/editNested.cpp
    ${SCHEMATIC_SRC_DIR}/editRoot.cpp
    ${SCHEMATIC_SRC_DIR}/editSchematic.cpp
    ${SCHEMATIC_SRC_DIR}/factory.cpp
    ${SCHEMATIC_SRC_DIR}/feature.cpp
    ${SCHEMATIC_SRC_DIR}/file.cpp
    ${SCHEMATIC_SRC_DIR}/glyph.cpp
    ${SCHEMATIC_SRC_DIR}/markup.cpp
    ${SCHEMATIC_SRC_DIR}/node.cpp
    ${SCHEMATIC_SRC_DIR}/object.cpp
    ${SCHEMATIC_SRC_DIR}/property.cpp
    ${SCHEMATIC_SRC_DIR}/schematic.cpp
    ${SCHEMATIC_SRC_DIR}/site.cpp
    ${SCHEMATIC_SRC_DIR}/space.cpp
    ${SCHEMATIC_SRC_DIR}/svgUtils.cpp
    ${SCHEMATIC_SRC_DIR}/wall.cpp
)


add_library( schematic
    ${SCHEMATIC_HEADERS} 
    ${SCHEMATIC_SOURCE}
)

set_target_properties( schematic PROPERTIES FOLDER schematic )

target_include_directories( schematic PUBLIC ${MEGA_API_DIR} )


IF( ${WIN32} )
set_property(TARGET schematic PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON )
ELSE( ${WIN32} )

ENDIF( ${WIN32} )

target_link_libraries( schematic mega_interface )

add_dependencies( schematic map_format )

link_boost( schematic system )
link_boost( schematic filesystem )
link_boost( schematic iostreams )
#link_boost( schematic serialization )

link_cgal( schematic )
link_common( schematic )
link_ed( schematic )
link_flatbuffers( schematic )

install( FILES ${SCHEMATIC_HEADERS} DESTINATION include/schematic )
install( TARGETS schematic EXPORT schematic DESTINATION bin)
install( EXPORT schematic NAMESPACE MEGA:: FILE schematic-config.cmake DESTINATION share )