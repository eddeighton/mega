
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

#include "schematic/buffer.hpp"
#include "schematic/rasteriser.hpp"

#include "common/astar.hpp"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/segment.hpp>
#include <boost/geometry/index/rtree.hpp>

namespace exact
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
};

struct SearchCoeffs
{
    float turns    = 8.0f;
    float distance = 1.0f;
};

using Angle = Math::Angle< 8 >;

struct Value
{
    int x, y;
    int direction = Angle::TOTAL_ANGLES;
    int turns     = 0;

    inline bool operator<( const Value& value ) const
    {
        //
        return ( x != value.x ) ? ( x < value.x ) : ( y < value.y );
    }
    inline bool operator==( const Value& value ) const
    {
        //
        return ( x == value.x ) && ( y == value.y );
    }

    struct Hash
    {
        inline std::size_t operator()( const Value& value ) const { return value.x + value.y; }
    };
};

int pointLineSegmentDistance( const Value& point, const Value& lineStart, const Value& lineEnd )
{
    using IntPoint   = boost::geometry::model::point< int, 2, boost::geometry::cs::cartesian >;
    using IntSegment = boost::geometry::model::segment< IntPoint >;

    // does the line segment face the point?
    const IntPoint v1{ lineStart.x, lineStart.y };
    const IntPoint v2{ lineEnd.x, lineEnd.y };
    const IntPoint pt{ point.x, point.y };

    // clang-format off
    const IntPoint e1{
        v2.get< 0 >() - v1.get< 0 >(),
        v2.get< 1 >() - v1.get< 1 >() };

    const IntPoint e2{
        pt.get< 0 >() - v1.get< 0 >(),
        pt.get< 1 >() - v1.get< 1 >() };

    const double r1 = boost::geometry::dot_product( e1, e1 );
    const double r2 = boost::geometry::dot_product( e1, e2 );

    if( ( r2 > 0.0 ) && ( r2 < r1 ) )
    {
        // get squared length of e1
        const double len2 = ( e1.get<0>() * e1.get<0>() ) + ( e1.get<1>() * e1.get<1>() );

        const IntPoint pointOnSegment
        {
            static_cast< int >( static_cast< double >( v1.get< 0 >() ) + (r2 * static_cast< double >( e1.get<0>() ) ) / len2 ),
            static_cast< int >( static_cast< double >( v1.get< 1 >() ) + (r2 * static_cast< double >( e1.get<1>() ) ) / len2 )
        };

        // calculate the manhatten distance
        return abs( pointOnSegment.get<0>() - pt.get<0>() ) + abs( pointOnSegment.get<1>() - pt.get<1>() );
    }
    else
    {
        return std::min( 
            abs( pt.get<0>() - v1.get<0>() ) + abs( pt.get<1>() - v1.get<1>() ),
            abs( pt.get<0>() - v2.get<0>() ) + abs( pt.get<1>() - v2.get<1>() )
            );
    }
}

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

bool drawPath( const Value& vStart, const Value& vGoal, const SearchCoeffs& coeffs, Adjacency& adjacency,
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

            //const float fDist = std::sqrt( x * x + y * y );
            const float fDist = x + y;
            const float fTurns
                = previous.turns + Adjacency::turns( previous.direction, value.direction ) * coeffs.turns;

            return previousCostEstimate.cost + fDist + fTurns;
        },

        // Estimate
        [ &vGoal, &coeffs ]( const Value& value ) -> float
        {
            const int   x      = abs( vGoal.x - value.x );
            const int   y      = abs( vGoal.y - value.y );
            //const float fDist  = std::sqrt( x * x + y * y ) * coeffs.distance;
            const float fDist  = ( x + y ) * coeffs.distance;
            const float fTurns = ( value.turns ) * coeffs.turns;
            return fDist + fTurns;
        },

        result );
}

using ValueSegment = std::pair< Value, Value >;
using ValueSegmentVector = std::vector< ValueSegment >;

bool drawPath( const Value& vStart, Value& vEnd, const ValueSegmentVector& goalSegments, const SearchCoeffs& coeffs, Adjacency& adjacency,
               AStarTraits::PredecessorMap& result )
{
    return astar::search< AStarTraits >(
        vStart, 
        
        [ &goalSegments, &vEnd ]( const Value& best )
        {
            int iMinDist = std::numeric_limits< int >::max();
            for( const auto& segment : goalSegments )
            {
                const int m = pointLineSegmentDistance( best, segment.first, segment.second );
                if( abs( m )  < 2 )
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

            //const float fDist = std::sqrt( x * x + y * y );
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

using FloorPolyMap = std::map< const Analysis::Partition*, Analysis::HalfEdgePolygonWithHoles >;

void renderPoly( agg::path_storage& path, const Rect& boundingBox, const Analysis::Partition* pPartition,
                 const Analysis::HalfEdgeVector& poly )
{
    auto convert = [ &boundingBox ]( Analysis::HalfEdge e, double& x, double& y )
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
                          const Analysis::HalfEdgePolygonWithHoles& polyWithHoles, agg::gray8 colour )
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
                                   const Analysis::HalfEdgePolygonWithHoles& polyWithHoles, agg::gray8 colour,
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

void Analysis::lanes()
{
    HalfEdgeVector perimeter;
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

    // for each floor partition
    for( const auto& [ pPartition, polyWithHoles ] : floors )
    {
        if( pPartition->bIsCorridor )
        {
        }
        else
        {
            // collect all doorsteps
            HalfEdgeVector doorSteps;
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

            using DoorStep = std::pair< int, int >;
            using DoorStepsVec = std::vector< DoorStep >;
            DoorStepsVec doorStepPoints;

            for( auto e : doorSteps )
            {
                const auto   v      = e->target()->point() - e->source()->point();
                const auto   n      = v.perpendicular( CGAL::Orientation::COUNTERCLOCKWISE );
                const double length = CGAL::approximate_sqrt( CGAL::to_double( n.squared_length() ) );
                const auto   p1     = e->source()->point() + ( v / 2.0 );
                const auto   p2     = p1 + ( n * 4.0 / length );

                const int x = static_cast< int >( CGAL::to_double( p2.x() - boundingBox.xmin() ) );
                const int y = static_cast< int >( CGAL::to_double( p2.y() - boundingBox.ymin() ) );

                doorStepPoints.push_back( DoorStep{ x, y } );

                raster.line( CGAL::to_double( p1.x() - boundingBox.xmin() ),
                             CGAL::to_double( p1.y() - boundingBox.ymin() ), x, y, colourLine );
            }

            // now solve the lane paths
            if( doorStepPoints.size() > 1 )
            {
                ValueSegmentVector segments;

                std::set< DoorStepsVec::const_iterator > connected;
                {
                    for( DoorStepsVec::const_iterator i = doorStepPoints.begin(), iEnd = doorStepPoints.end();
                         i != iEnd;
                         ++i )
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
                    if( drawPath( vStart, vGoal, coeffs, adjacency, result ) )
                    {
                        connected.erase( furthestDoorStepPair.first );
                        connected.erase( furthestDoorStepPair.second );

                        // draw solution
                        Value v = vGoal;
                        Value vLast = v;
                        while( v != vStart )
                        {
                            auto iFind = result.find( v );
                            INVARIANT( iFind != result.end(), "AStar result error" );
                            raster.setPixel( iFind->second.x, iFind->second.y, colourLine );
                            v = iFind->second;

                            if( vLast.direction != v.direction )
                            {
                                segments.push_back( ValueSegment{ vLast, v } );
                                vLast = v;
                            }
                        }
                    }
                    else
                    {
                        INVARIANT( false, "AStar failed" );
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
                        int iMinDist = std::numeric_limits< int >::max();
                        for( const auto& segment : segments )
                        {
                            int m = pointLineSegmentDistance( Value{ doorStep.first, doorStep.second },
                                segment.first, segment.second );
                            if( m < iMinDist )
                                iMinDist = m;
                        }
                        doorStepDistances.insert( { -iMinDist, i } );
                    }

                    auto doorStepIter = doorStepDistances.begin()->second;

                    const Value vStart{ doorStepIter->first, doorStepIter->second };
                    AStarTraits::PredecessorMap result;
                    Value vEnd;
                    if( drawPath( vStart, vEnd, segments, coeffs, adjacency, result ) )
                    {
                        connected.erase( doorStepIter );

                        // draw solution
                        Value v = vEnd;
                        Value vLast = v;
                        while( v != vStart )
                        {
                            auto iFind = result.find( v );
                            INVARIANT( iFind != result.end(), "AStar result error" );
                            raster.setPixel( iFind->second.x, iFind->second.y, colourLine );
                            v = iFind->second;

                            if( vLast.direction != v.direction )
                            {
                                segments.push_back( { vLast, v } );
                                vLast = v;
                            }
                        }
                        raster.setPixel( vStart.x, vStart.y, colourLine );
                    }
                    else
                    {
                        INVARIANT( false, "AStar failed" );
                    }

                }
            }
        }
    }
}

} // namespace exact
