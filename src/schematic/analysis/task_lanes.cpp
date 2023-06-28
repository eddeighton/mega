
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
#include "geometry.hpp"

#include "schematic/cgalUtils.hpp"
#include "schematic/analysis/analysis.hpp"

#include "schematic/buffer.hpp"
#include "schematic/rasteriser.hpp"

#include "common/astar.hpp"

namespace exact
{
namespace
{
static const agg::gray8 colourLine  = agg::gray8{ 0x03 };
static const agg::gray8 colourAvoid = agg::gray8{ 0x02 };
static const agg::gray8 colourSpace = agg::gray8{ 0x01 };
static const agg::gray8 colourClear = agg::gray8{ 0x00 };


bool operator==( const agg::gray8& left, const agg::gray8& right )
{
    return left.v == right.v; // && left.a == right.a;
}

struct Policies
{
    static constexpr bool TerminateEarly = false;
    static constexpr int  MaxIterations  = 40000;
};

struct SearchCoeffs
{
    float turns    = 8.0f;
    float distance = 1.0f;
};

using AStarTraits = astar::AStarTraits< Value, Policies >;

struct Adjacency
{
    schematic::Rasteriser& raster;

    inline bool isValidAStarValue( const AStarTraits::Value& value )
    {
        if( ( value.x >= 0 ) && ( value.y >= 0 ) )
        {
            if( ( value.x < raster.getBuffer().getWidth() ) && ( value.y < raster.getBuffer().getHeight() ) )
            {
                if( raster.getPixel( value.x, value.y ) == colourSpace
                    || raster.getPixel( value.x, value.y ) == colourLine )
                {
                    return true;
                }
            }
        }
        return false;
    };

    static inline int turns( int from, int to )
    {
        if( from == Angle::TOTAL_ANGLES )
            return 0;
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
};

astar::ErrorCode drawPath( const Value& vStart, const Value& vGoal, const SearchCoeffs& coeffs, Adjacency& adjacency,
                           AStarTraits::PredecessorMap& result )
{
    return astar::search< AStarTraits >(
        vStart,

        [ &vGoal ]( const Value& best ) { return best == vGoal; },

        adjacency,

        // Cost
        [ &coeffs ](
            const Value& value, const Value& previous, const AStarTraits::CostEstimate& previousCostEstimate ) -> float
        {
            const int x = abs( value.x - previous.x );
            const int y = abs( value.y - previous.y );

            // const float fDist = std::sqrt( x * x + y * y );
            const float fDist = x + y;
            const float fTurns
                = previous.turns + Adjacency::turns( previous.direction, value.direction ) * coeffs.turns;

            return previousCostEstimate.cost + fDist + fTurns;
        },

        // Estimate
        [ &vGoal, &coeffs ]( const Value& value ) -> float
        {
            const int x = abs( vGoal.x - value.x );
            const int y = abs( vGoal.y - value.y );
            // const float fDist  = std::sqrt( x * x + y * y ) * coeffs.distance;
            const float fDist  = ( x + y ) * coeffs.distance;
            const float fTurns = ( value.turns ) * coeffs.turns;
            return fDist + fTurns;
        },

        result );
}

using ValueSegment       = std::pair< Value, Value >;
using ValueSegmentVector = std::vector< ValueSegment >;

astar::ErrorCode drawPath( const Value& vStart, Value& vEnd, const ValueSegmentVector& goalSegments,
                           const SearchCoeffs& coeffs, Adjacency& adjacency, AStarTraits::PredecessorMap& result )
{
    return astar::search< AStarTraits >(
        vStart,

        [ &goalSegments, &vEnd ]( const Value& best )
        {
            int iMinDist = std::numeric_limits< int >::max();
            for( const auto& segment : goalSegments )
            {
                const int m = pointLineSegmentDistance( best, segment.first, segment.second );
                if( m == 0 )
                {
                    vEnd = best;
                    return true;
                }
            }
            return false;
        },

        adjacency,

        // Cost
        [ &coeffs ](
            const Value& value, const Value& previous, const AStarTraits::CostEstimate& previousCostEstimate ) -> float
        {
            const int x = abs( value.x - previous.x );
            const int y = abs( value.y - previous.y );

            // const float fDist = std::sqrt( x * x + y * y );
            const float fDist = x + y;
            const float fTurns
                = previous.turns + Adjacency::turns( previous.direction, value.direction ) * coeffs.turns;

            return previousCostEstimate.cost + fDist + fTurns;
        },

        // Estimate
        [ &goalSegments, &coeffs ]( const Value& value ) -> float
        {
            int iMinDist = std::numeric_limits< int >::max();
            for( const auto& segment : goalSegments )
            {
                const int m = pointLineSegmentDistance( value, segment.first, segment.second );
                if( m < iMinDist )
                    iMinDist = m;
            }
            const float fDist  = iMinDist * coeffs.distance;
            const float fTurns = value.turns * coeffs.turns;
            return fDist + fTurns;
        },

        result );
}

using FloorPolyMap = std::map< const Analysis::Partition*, Analysis::HalfEdgeCstPolygonWithHoles >;

void renderPoly( agg::path_storage& path, const Rect& boundingBox, const Analysis::Partition* pPartition,
                 const Analysis::HalfEdgeCstVector& poly )
{
    auto convert = [ &boundingBox ]( Analysis::HalfEdgeCst e, double& x, double& y )
    {
        const auto& p = e->source()->point();
        x             = ( CGAL::to_double( p.x() - boundingBox.xmin() ) );
        y             = ( CGAL::to_double( p.y() - boundingBox.ymin() ) );
    };

    double x, y;
    for( auto i = poly.begin(), iEnd = poly.end(), iNext = poly.begin(); i != iEnd; ++i )
    {
        convert( *i, x, y );

        ++iNext;
        if( i == poly.begin() )
        {
            path.move_to( x, y );
        }
        else
        {
            path.line_to( x, y );
            if( iNext == iEnd )
            {
                path.close_polygon();
            }
        }
    }
}

void renderPolyWithHoles( schematic::Rasteriser& raster, const Rect& boundingBox, const Analysis::Partition* pPartition,
                          const Analysis::HalfEdgeCstPolygonWithHoles& polyWithHoles, agg::gray8 colour )
{
    schematic::Rasteriser::PolygonRasterizer ras;
    ras.gamma( agg::gamma_threshold( 1.0 ) );

    {
        agg::path_storage path;
        renderPoly( path, boundingBox, pPartition, polyWithHoles.outer );
        ras.add_path( path );
    }
    for( const auto& hole : polyWithHoles.holes )
    {
        agg::path_storage path;
        renderPoly( path, boundingBox, pPartition, hole );
        ras.add_path( path );
    }
    raster.render( ras, colour );
}

void renderPolyWithHolesClearance( schematic::Rasteriser& raster, const Rect& boundingBox,
                                   const Analysis::Partition*                pPartition,
                                   const Analysis::HalfEdgeCstPolygonWithHoles& polyWithHoles, agg::gray8 colour,
                                   float fClearance )
{
    schematic::Rasteriser::PolygonRasterizer ras;
    ras.gamma( agg::gamma_threshold( 0.5 ) );

    {
        agg::path_storage path;
        renderPoly( path, boundingBox, pPartition, polyWithHoles.outer );
        agg::conv_stroke< agg::path_storage > contour( path );
        {
            contour.width( fClearance );
        }
        ras.add_path( contour );
    }
    for( const auto& hole : polyWithHoles.holes )
    {
        agg::path_storage path;
        renderPoly( path, boundingBox, pPartition, hole );
        agg::conv_stroke< agg::path_storage > contour( path );
        {
            contour.width( 4.0 );
        }
        ras.add_path( contour );
    }
    raster.render( ras, colour );
}

void calculateLaneSegments( schematic::Rasteriser& raster, const Rect& boundingBox,
                            const Analysis::Partition* pFloorPartition, const SearchCoeffs& coeffs,
                            Adjacency& adjacency, const Analysis::HalfEdgeCstPolygonWithHoles& polyWithHoles,
                            ValueSegmentVector& segments, ValueSegmentVector& doorStepSegments )
{
    // collect all doorsteps
    Analysis::HalfEdgeCstVector doorSteps;
    for( auto e : polyWithHoles.outer )
    {
        if( test( e, EdgeMask::eDoorStep ) )
        {
            doorSteps.push_back( e );
        }
    }
    for( const auto& hole : polyWithHoles.holes )
    {
        for( auto e : hole )
        {
            if( test( e, EdgeMask::eDoorStep ) )
            {
                doorSteps.push_back( e );
            }
        }
    }

    using DoorStep     = std::pair< int, int >;
    using DoorStepsVec = std::vector< DoorStep >;
    DoorStepsVec doorStepPoints;

    for( auto e : doorSteps )
    {
        const auto   v      = e->target()->point() - e->source()->point();
        const auto   n      = v.perpendicular( CGAL::Orientation::COUNTERCLOCKWISE );
        const double length = CGAL::approximate_sqrt( CGAL::to_double( n.squared_length() ) );
        const auto   p1     = e->source()->point() + ( v / 2.0 );
        const auto   p2     = p1 + ( n * 5.0 / length );

        const int x0 = Math::roundRealOutToInt( CGAL::to_double( p1.x() - boundingBox.xmin() ) );
        const int y0 = Math::roundRealOutToInt( CGAL::to_double( p1.y() - boundingBox.ymin() ) );
        const int x1 = Math::roundRealOutToInt( CGAL::to_double( p2.x() - boundingBox.xmin() ) );
        const int y1 = Math::roundRealOutToInt( CGAL::to_double( p2.y() - boundingBox.ymin() ) );

        const auto dir = Math::fromVector< Angle >( x1 - x0, y1 - y0 );

        doorStepPoints.push_back( DoorStep{ x1, y1 } );
        doorStepSegments.push_back( ValueSegment{ Value{ x0, y0, dir }, Value{ x1, y1, dir } } );

        raster.line( x0, y0, x1, y1, colourLine );
    }

    // now solve the lane paths
    if( doorStepPoints.size() > 1 )
    {
        std::set< DoorStepsVec::const_iterator > connected;
        {
            for( DoorStepsVec::const_iterator i = doorStepPoints.begin(), iEnd = doorStepPoints.end(); i != iEnd; ++i )
            {
                connected.insert( i );
            }
        }

        // first connect the two furthest door steps
        {
            using DoorStepVecIterPair = std::pair< DoorStepsVec::const_iterator, DoorStepsVec::const_iterator >;
            DoorStepVecIterPair furthestDoorStepPair;
            {
                std::multimap< int, DoorStepVecIterPair > doorStepPairDistances;
                for( DoorStepsVec::const_iterator i    = doorStepPoints.begin(),
                                                  iEnd = doorStepPoints.end() - 1,
                                                  jEnd = doorStepPoints.end();
                     i != iEnd;
                     ++i )
                {
                    for( DoorStepsVec::const_iterator j = i + 1; j != jEnd; ++j )
                    {
                        const int x = i->first - j->first;
                        const int y = i->second - j->second;
                        // order by largest distance first
                        doorStepPairDistances.insert( { -( x * x + y * y ), { i, j } } );
                    }
                }
                furthestDoorStepPair = doorStepPairDistances.begin()->second;
            }

            const Value vStart{ furthestDoorStepPair.first->first, furthestDoorStepPair.first->second };
            const Value vGoal{ furthestDoorStepPair.second->first, furthestDoorStepPair.second->second };
            AStarTraits::PredecessorMap result;
            const auto                  astarResult = drawPath( vStart, vGoal, coeffs, adjacency, result );
            if( astarResult == astar::eSuccess )
            {
                connected.erase( furthestDoorStepPair.first );
                connected.erase( furthestDoorStepPair.second );

                raster.setPixel( vGoal.x, vGoal.y, colourLine );
                // draw solution
                Value v     = vGoal;
                Value vLast = v;
                while( v != vStart )
                {
                    auto iFind = result.find( v );
                    INVARIANT( iFind != result.end(), "AStar result error" );
                    v = iFind->second;
                    raster.setPixel( v.x, v.y, colourLine );

                    if( vLast.direction != v.direction )
                    {
                        segments.push_back( ValueSegment{ vLast, v } );
                        vLast = v;
                    }
                }
            }
            else
            {
                INVARIANT( false, "AStar failed: " << astarResult );
            }
        }

        // then go through the rest in order of distance from constructed line segments
        // and connect them all to the nearest line segment
        while( !connected.empty() )
        {
            // find door step FURTHEST from any segment
            std::multimap< int, DoorStepsVec::const_iterator > doorStepDistances;
            for( auto i : connected )
            {
                const DoorStep& doorStep = *i;
                int             iMinDist = std::numeric_limits< int >::max();
                for( const auto& segment : segments )
                {
                    int m = pointLineSegmentDistance(
                        Value{ doorStep.first, doorStep.second }, segment.first, segment.second );
                    if( m < iMinDist )
                        iMinDist = m;
                }
                doorStepDistances.insert( { -iMinDist, i } );
            }

            auto doorStepIter = doorStepDistances.begin()->second;

            const Value                 vStart{ doorStepIter->first, doorStepIter->second };
            AStarTraits::PredecessorMap result;
            Value                       vEnd;
            auto                        astarResult = drawPath( vStart, vEnd, segments, coeffs, adjacency, result );
            if( astarResult == astar::eSuccess )
            {
                connected.erase( doorStepIter );

                raster.setPixel( vEnd.x, vEnd.y, colourLine );

                // draw solution
                Value v     = vEnd;
                Value vLast = v;
                while( v != vStart )
                {
                    auto iFind = result.find( v );
                    INVARIANT( iFind != result.end(), "AStar result error" );
                    v = iFind->second;
                    raster.setPixel( v.x, v.y, colourLine );

                    if( vLast.direction != v.direction )
                    {
                        segments.push_back( { vLast, v } );
                        vLast = v;
                    }
                }
            }
            else
            {
                INVARIANT( false, "AStar failed: " << astarResult );
            }
        }
    }
}

static exact::Polygon makeOctogon( double x )
{
    // 1.414213562
    const double y     = x / 1.41; // approx square root of 2
    const double halfx = x / 2.0;

    const double x0 = -halfx - y;
    const double x1 = -halfx;
    const double x2 = halfx;
    const double x3 = halfx + y;

    const double y0 = -halfx - y;
    const double y1 = -halfx;
    const double y2 = halfx;
    const double y3 = halfx + y;

    // clang-format off
    //
    //       x0  x1       x2  x3     
    //  y3        #########
    //           #         #
    //          #           #
    //         #             #
    //  y2    #               #
    //        #               #
    //        #               #
    //        #               #
    //  y1    #               #
    //        #              #
    //         #            #
    //          #          #
    //  y0       ##########
    //
    // clang-format on

    // counter clockwise starting from extreme left top
    const std::vector< Point > points = { Point{ x0, y2 }, Point{ x0, y1 }, Point{ x1, y0 }, Point{ x2, y0 },
                                          Point{ x3, y1 }, Point{ x3, y2 }, Point{ x2, y3 }, Point{ x1, y3 } };
    return exact::Polygon{ points.begin(), points.end() };
}

} // namespace

void Analysis::lanes()
{
    HalfEdgeCstVector perimeter;
    getPerimeterPolygon( perimeter );

    std::vector< exact::Point > perimeterPoints;
    for( auto e : perimeter )
    {
        perimeterPoints.push_back( e->source()->point() );
    }
    const Rect   boundingBox = CGAL::bbox_2( perimeterPoints.begin(), perimeterPoints.end() );
    const double width       = CGAL::to_double( boundingBox.xmax() - boundingBox.xmin() );
    const double height      = CGAL::to_double( boundingBox.ymax() - boundingBox.ymin() );
    m_laneBitmap.resize( std::ceil( width ), std::ceil( height ) );
    m_laneBitmap.reset();
    m_pSchematic->setLaneBitmapOffset(
        schematic::Vector{ CGAL::to_double( boundingBox.xmin() ), CGAL::to_double( boundingBox.ymin() ) } );
    schematic::Rasteriser raster( m_laneBitmap, false );

    FloorPolyMap floors;
    getFloorPartitions( floors );

    for( const auto& [ pPartition, polyWithHoles ] : floors )
    {
        renderPolyWithHoles( raster, boundingBox, pPartition, polyWithHoles, colourSpace );
    }

    // TODO set clearance per partition via properties
    for( const auto& [ pPartition, polyWithHoles ] : floors )
    {
        renderPolyWithHolesClearance( raster, boundingBox, pPartition, polyWithHoles, colourAvoid, 4.0f );
    }

    const SearchCoeffs coeffs;
    Adjacency          adjacency{ raster };

    static const exact::Polygon octogonInner = makeOctogon( 1 );
    static const exact::Polygon octogonOuter = makeOctogon( 1.125 );

    // NOTE: can only write the partition lane segments AFTER all are calculated
    // because any doorstep edge intersection will break calculation
    struct PartitionLaneSegments
    {
        ValueSegmentVector segments;
        ValueSegmentVector doorStepSegments;
    };
    std::map< const Analysis::Partition*, PartitionLaneSegments > partitionSegments;

    // for each floor partition
    for( const auto& [ pPartition, polyWithHoles ] : floors )
    {
        if( pPartition->bIsCorridor )
        {
        }
        else
        {
            PartitionLaneSegments segments;
            calculateLaneSegments( raster, boundingBox, pPartition, coeffs, adjacency, polyWithHoles, segments.segments,
                                   segments.doorStepSegments );
            partitionSegments.insert( { pPartition, std::move( segments ) } );
        }
    }

    for( const auto& [ pPartition, segments ] : partitionSegments )
    {
        // render the octagonal lane geometry
        for( const auto& segment : segments.segments )
        {
            const Curve curve{
                Point{ 0.5 + segment.first.x + boundingBox.xmin(), 0.5 + segment.first.y + boundingBox.ymin() },
                Point{ 0.5 + segment.second.x + boundingBox.xmin(), 0.5 + segment.second.y + boundingBox.ymin() } };
            renderCurve( m_arr, curve, EdgeMask::eLane, EdgeMask::eLane );
            renderExtrudedCurve( m_arr, curve, octogonInner, EdgeMask::eLaneInner, EdgeMask::eLaneInnerBoundary );
            renderExtrudedCurve( m_arr, curve, octogonOuter, EdgeMask::eLaneOuter, EdgeMask::eLaneOuterBoundary );
        }
        for( const auto& segment : segments.doorStepSegments )
        {
            const Curve curve{
                Point{ 0.5 + segment.first.x + boundingBox.xmin(), 0.5 + segment.first.y + boundingBox.ymin() },
                Point{ 0.5 + segment.second.x + boundingBox.xmin(), 0.5 + segment.second.y + boundingBox.ymin() } };
            renderCurve( m_arr, curve, EdgeMask::eLane, EdgeMask::eLane );
            renderExtrudedCurve( m_arr, curve, octogonInner, EdgeMask::eLaneInner, EdgeMask::eLaneInnerBoundary );
            renderExtrudedCurve( m_arr, curve, octogonOuter, EdgeMask::eLaneOuter, EdgeMask::eLaneOuterBoundary );
        }
    }
}

} // namespace exact
