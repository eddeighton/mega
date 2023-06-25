
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

#include "common/astar.hpp"

// #include <CGAL/Polygon_with_holes_2.h>
// #include <CGAL/create_straight_skeleton_from_polygon_with_holes_2.h>
// #include <CGAL/create_offset_polygons_from_polygon_with_holes_2.h>
// #include <CGAL/create_offset_polygons_2.h>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/segment.hpp>
#include <boost/geometry/index/rtree.hpp>

#include "schematic/buffer.hpp"
#include "schematic/rasteriser.hpp"

#include <vector>
#include <tuple>

namespace exact
{

// TODO: better encapsulate this...
static const double imageScale = 2.0;

bool operator==( const agg::gray8& left, const agg::gray8& right )
{
    return left.v == right.v; // && left.a == right.a;
}

static const agg::gray8 colourLine  = agg::gray8{ 0x02 };
static const agg::gray8 colourSet   = agg::gray8{ 0x01 };
static const agg::gray8 colourClear = agg::gray8{ 0x00 };

void drawPath( int x1, int y1, int x2, int y2, float turnCoef, schematic::Rasteriser& raster )
{
    using Angle = Math::Angle< 8 >;

    struct Value
    {
        int x, y;
        int direction = Angle::TOTAL_ANGLES;
        int turns     = 0;

        inline bool operator<=( const Value& value ) const
        {
            return ( x != value.x ) ? ( x < value.x ) : ( y < value.y );
        }
        inline bool operator==( const Value& value ) const { return ( x == value.x ) && ( y == value.y ); }

        struct Hash
        {
            inline std::size_t operator()( const Value& value ) const { return value.x + value.y; }
        };
    };
    using AStarTraits = astar::AStarTraits< Value >;

    struct Adjacency
    {
        schematic::Rasteriser& raster;

        inline bool isValidAStarValue( const AStarTraits::Value& value )
        {
            if( ( value.x >= 0 ) && ( value.y >= 0 ) )
            {
                if( ( value.x < raster.getBuffer().getWidth() ) && ( value.y < raster.getBuffer().getHeight() ) )
                {
                    if( raster.getPixel( value.x, value.y ) == colourClear )
                    {
                        return true;
                    }
                }
            }
            return false;
        };

        static inline int turns( int from, int to )
        {
            return Math::difference< Angle >( static_cast< Angle::Value >( from ), static_cast< Angle::Value >( to ) );
        }

        inline const std::vector< AStarTraits::Value >& operator()( const AStarTraits::Value& value )
        {
            static std::vector< AStarTraits::Value > values;
            values.clear();
            for( int i = 0; i != Angle::TOTAL_ANGLES; ++i )
            {
                const Angle::Value angle = static_cast< Angle::Value >( i );
                int                x = 0, y = 0;
                Math::toVectorDiscrete( angle, x, y );

                const AStarTraits::Value adjacentValue{
                    value.x + x, value.y + y, angle, value.turns + turns( value.direction, angle ) };

                if( isValidAStarValue( adjacentValue ) )
                {
                    values.emplace_back( adjacentValue );
                }
            }
            return values;
        }
    } adjacency{ raster };

    AStarTraits::PredecessorMap result;

    const Value vStart{ x1, y1 };
    const Value vGoal{ x2, y2 };

    const bool bResult = astar::search< AStarTraits >(
        vStart, vGoal, adjacency,

        // Cost
        [ turnCoef ](
            const Value& value, const Value& previous, const AStarTraits::CostEstimate& previousCostEstimate ) -> float
        {
            const int x = abs( value.x - previous.x );
            const int y = abs( value.y - previous.y );

            const float fDist = std::sqrt( x * x + y * y );
            //const float fDist = x + y;

            return previousCostEstimate.cost + fDist
                   + ( Adjacency::turns( previous.direction, value.direction ) * turnCoef );
            
        },

        // Estimate
        [ &vGoal, turnCoef ]( const Value& value ) -> float
        {
            const int x = abs( vGoal.x - value.x );
            const int y = abs( vGoal.y - value.y );

            const float fDist = std::sqrt( x * x + y * y );
            //const float fDist = x + y;

            return fDist + ( value.turns * turnCoef );
        },

        result );

    for( const auto& [ v, _ ] : result )
    {
        raster.setPixel( v.x, v.y, colourSet );
    }

    if( bResult )
    {
        // draw solution
        Value v = vGoal;
        while( v != vStart )
        {
            auto iFind = result.find( v );
            INVARIANT( iFind != result.end(), "AStar result error" );
            raster.setPixel( iFind->second.x, iFind->second.y, colourLine );
            v = iFind->second;
        }
    }
    else
    {
        INVARIANT( false, "AStar failed" );
    }
}

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

    std::vector< exact::Point > perimeterPoints;
    for( auto e : perimeter )
    {
        perimeterPoints.push_back( e->source()->point() );
    }
    const Rect boundingBox = CGAL::bbox_2( perimeterPoints.begin(), perimeterPoints.end() );

    const double width  = CGAL::to_double( boundingBox.xmax() - boundingBox.xmin() ) * imageScale;
    const double height = CGAL::to_double( boundingBox.ymax() - boundingBox.ymin() ) * imageScale;

    m_laneBitmap.resize( std::ceil( width ) + imageScale, std::ceil( height ) + imageScale );
    m_laneBitmap.reset();

    schematic::Rasteriser raster( m_laneBitmap, false );

    INVARIANT( raster.getPixel( 36, 36 ) == colourClear, "0,0 not clear after reset" );

    Analysis::HalfEdgeVectorVector boundarySegments;
    getBoundaryPartitions( boundarySegments );

    struct HalfEdgeVectorVertexSource
    {
        const Rect&                              m_boundingBox;
        const Analysis::HalfEdgeVector&          m_boundarySegments;
        Analysis::HalfEdgeVector::const_iterator m_iter;
        bool                                     m_bFirst      = true;
        bool                                     m_bPolyClosed = false;

        HalfEdgeVectorVertexSource( const Rect& boundingBox, const Analysis::HalfEdgeVector& boundarySegments )
            : m_boundingBox( boundingBox )
            , m_boundarySegments( boundarySegments )
            , m_iter( m_boundarySegments.begin() )
        {
        }
        void rewind( unsigned path_id = 0 ) { m_iter = m_boundarySegments.begin() + path_id; }

        unsigned vertex( double* x, double* y )
        {
            if( m_iter != m_boundarySegments.end() )
            {
                const auto& p = ( *m_iter )->source()->point();
                *x            = ( CGAL::to_double( p.x() - m_boundingBox.xmin() ) ) * imageScale;
                *y            = ( CGAL::to_double( p.y() - m_boundingBox.ymin() ) ) * imageScale;
                ++m_iter;

                if( m_bFirst )
                {
                    m_bFirst = false;
                    return agg::path_cmd_move_to;
                }
                else
                {
                    return agg::path_cmd_line_to;
                }
            }
            if( !m_bPolyClosed )
            {
                m_bPolyClosed = true;
                return agg::get_close_flag( agg::path_cmd_end_poly );
            }
            else
            {
                return agg::path_cmd_stop;
            }
        }
    };

    // render the boundaries into the bitmap
    for( const auto& boundary : boundarySegments )
    {
        HalfEdgeVectorVertexSource visitor{ boundingBox, boundary };
        raster.renderPath( visitor, colourSet, 1.0f );
    }

    float turnCoef = 1.0f;

    std::vector< schematic::Feature_Pin::Ptr > pathPins;
    m_pSchematic->for_each_recursive(

        [ &pathPins, &turnCoef ]( schematic::Node::Ptr pNode )
        {
            if( schematic::Feature_Pin::Ptr pPin = boost::dynamic_pointer_cast< schematic::Feature_Pin >( pNode ) )
            {
                if( auto pOpt = pPin->getPropertyString( "path" ); pOpt.has_value() )
                {
                    try
                    {
                        std::istringstream is( pOpt.value() );
                        is >> turnCoef;
                    }
                    catch( ... )
                    {
                    }
                    pathPins.push_back( pPin );
                }
            }
        },
        schematic::Node::ConvertPtrType< schematic::Property >() );

    // HalfEdgeSet doorSteps;
    // getEdges( m_arr, doorSteps, []( HalfEdge edge ) { return test( edge, EdgeMask::eDoorStep ); } );

    // for( auto i = doorSteps.begin(), iNext = doorSteps.begin(), iEnd = doorSteps.end(); i != iEnd; ++i )
    for( auto i = pathPins.begin(), iNext = pathPins.begin(), iEnd = pathPins.end(); i != iEnd; ++i )
    {
        ++iNext;
        if( iNext == pathPins.end() )
        {
            iNext = pathPins.begin();
        }

        /*auto calcDoorPos = [ &boundingBox, &imageScale ]( HalfEdge e ) -> astar::Value
        {
            const auto   v      = e->target()->point() - e->source()->point();
            const auto   n      = v.perpendicular( CGAL::Orientation::COUNTERCLOCKWISE );
            const double length = CGAL::approximate_sqrt( CGAL::to_double( n.squared_length() ) );
            const auto   p      = e->source()->point() + ( v / 2.0 ) + ( n * 4.0 / length );

            return astar::Value{ ( CGAL::to_double( p.x() - boundingBox.xmin() ) ) * imageScale,
                                 ( CGAL::to_double( p.y() - boundingBox.ymin() ) ) * imageScale };
        };*/

        auto calcPinPos = [ &boundingBox, imageScale ]( schematic::Feature_Pin::Ptr pPin ) -> astar::BasicValue
        {
            static const exact::InexactToExact converter;
            exact::Point                       p            = converter( pPin->getPoint( 0 ) );
            std::optional< exact::Transform >  transformOpt = pPin->getAbsoluteExactTransform();
            if( transformOpt.has_value() )
            {
                p = transformOpt.value()( p );
            }
            return astar::BasicValue{
                static_cast< int >( CGAL::to_double( p.x() - boundingBox.xmin() ) * imageScale ),
                static_cast< int >( CGAL::to_double( p.y() - boundingBox.ymin() ) * imageScale ) };
        };
        const astar::BasicValue vStart = calcPinPos( *i );
        const astar::BasicValue vGoal  = calcPinPos( *iNext );

        // INVARIANT( isValidAStarValue( vStart ), "AStar start point invalid: " << vStart.x << "," << vStart.y );
        // INVARIANT( isValidAStarValue( vGoal ), "AStar end point invalid: " << vGoal.x << "," << vGoal.y );

        drawPath( vStart.x, vStart.y, vGoal.x, vGoal.y, turnCoef, raster );

        return;
    }
}

} // namespace exact
