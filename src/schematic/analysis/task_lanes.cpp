
//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#include "algorithms.hpp"
#include "polygon_with_holes.hpp"
#include "constructions.hpp"

#include "schematic/cgalUtils.hpp"
#include "schematic/analysis/analysis.hpp"

// #include <CGAL/Polygon_with_holes_2.h>
// #include <CGAL/create_straight_skeleton_from_polygon_with_holes_2.h>
// #include <CGAL/create_offset_polygons_from_polygon_with_holes_2.h>
// #include <CGAL/create_offset_polygons_2.h>

#include "schematic/buffer.hpp"
#include "schematic/rasteriser.hpp"

#include <vector>
#include <tuple>

namespace exact
{

void Analysis::lanes()
{
    static const exact::ExactToInexact converter;

    // std::map< const Analysis::Partition*, exact::Polygon_with_holes > floors;
    // getFloorPartitions( floors );
    //
    // std::map< const Analysis::PartitionSegment*, exact::Polygon > boundaries;
    // getBoundaryPartitions( boundaries );

    HalfEdgeVector perimeter;
    getPerimeterPolygon( perimeter );

    schematic::NavBitmap buffer( 128, 128 );

    schematic::Rasteriser raster( buffer, true );

    // /workspace/root/thirdparty/agg/src/include/agg_basics.h:336
    //---------------------------------------------------------path_commands_e
    /*enum path_commands_e
     {
         path_cmd_stop     = 0,        //----path_cmd_stop
         path_cmd_move_to  = 1,        //----path_cmd_move_to
         path_cmd_line_to  = 2,        //----path_cmd_line_to
         path_cmd_curve3   = 3,        //----path_cmd_curve3
         path_cmd_curve4   = 4,        //----path_cmd_curve4
         path_cmd_curveN   = 5,        //----path_cmd_curveN
         path_cmd_catrom   = 6,        //----path_cmd_catrom
         path_cmd_ubspline = 7,        //----path_cmd_ubspline
         path_cmd_end_poly = 0x0F,     //----path_cmd_end_poly
         path_cmd_mask     = 0x0F      //----path_cmd_mask
     };*/

    Analysis::HalfEdgeVectorVector boundarySegments;
    getBoundaryPartitions( boundarySegments );

    struct HalfEdgeVectorVertexSource
    {
        const Analysis::HalfEdgeVector&          m_boundarySegments;
        Analysis::HalfEdgeVector::const_iterator iter;
        bool                                     m_bFirst;

        HalfEdgeVectorVertexSource( const Analysis::HalfEdgeVector& boundarySegments )
            : m_boundarySegments( boundarySegments )
            , iter( m_boundarySegments.begin() )
            , m_bFirst( true )
        {
        }
        void     rewind( unsigned path_id = 0 ) { iter = m_boundarySegments.begin(); }
        unsigned vertex( double* x, double* y )
        {
            if( iter != m_boundarySegments.end() )
            {
                const auto& p = ( *iter )->source()->point();
                *x            = CGAL::to_double( p.x() );
                *y            = CGAL::to_double( p.y() );

                ++iter;
                if( m_bFirst )
                {
                    return agg::path_cmd_line_to;
                }
                else
                {
                    m_bFirst = false;
                    return agg::path_cmd_move_to;
                }
            }
            return agg::path_cmd_stop;
        }
    };

    for( const auto& boundary : boundarySegments )
    {
        HalfEdgeVectorVertexSource        visitor{ boundary };
        schematic::Rasteriser::ColourType colourSet{ 0xFF };
        raster.renderPath( visitor, colourSet );
    }

    /*std::vector< exact::Polygon > polygons;
    for( const auto& [ pPartitionSegment, polygon ] : boundaries )
    {
        polygons.push_back( polygon );
    }
    const Rect boundingBox = CGAL::bbox_2( polygons.begin(), polygons.end() );*/
}

} // namespace exact
