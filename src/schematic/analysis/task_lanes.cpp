
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
#include "constructions.hpp"
#include "geometry.hpp"
#include "polygon_tree.hpp"

#include "schematic/cgalUtils.hpp"
#include "schematic/analysis/analysis.hpp"
#include "schematic/analysis/polygon_with_holes.hpp"

#include "schematic/buffer.hpp"
#include "schematic/rasteriser.hpp"
#include "schematic/schematic.hpp"

#include "common/astar.hpp"

#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/create_straight_skeleton_from_polygon_with_holes_2.h>
#include <CGAL/create_offset_polygons_from_polygon_with_holes_2.h>
#include <CGAL/create_offset_polygons_2.h>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>

#include <tuple>

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
    static constexpr int  MaxIterations  = 100000;
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
            if( ( value.x < static_cast< int >( raster.getBuffer().getWidth() ) )
                && ( value.y < static_cast< int >( raster.getBuffer().getHeight() ) ) )
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

using FloorPolyMap = std::map< const Analysis::Partition*, Analysis::HalfEdgePolygonWithHoles >;

struct SchematicToBitmap
{
    const Rect& boundingBox;

    static constexpr int BITMAP_SCALING = 2;

    // const int x0 = Math::roundRealOutToInt( CGAL::to_double( p1.x() - boundingBox.xmin() ) );
    // const int y0 = Math::roundRealOutToInt( CGAL::to_double( p1.y() - boundingBox.ymin() ) );
    // const int x1 = Math::roundRealOutToInt( CGAL::to_double( p2.x() - boundingBox.xmin() ) );
    // const int y1 = Math::roundRealOutToInt( CGAL::to_double( p2.y() - boundingBox.ymin() ) );

    // used when convert arrangement edge paths to agg contours
    void operator()( Analysis::HalfEdgeCst e, double& x, double& y ) const
    {
        const auto& p = e->source()->point();
        x             = ( CGAL::to_double( p.x() - boundingBox.xmin() ) ) * BITMAP_SCALING;
        y             = ( CGAL::to_double( p.y() - boundingBox.ymin() ) ) * BITMAP_SCALING;
    }

    // used when convert doorstep points to precise pixels for bresenham lines
    void operator()( const exact::Point& pt, int& x, int& y ) const
    {
        x = static_cast< int >( ( CGAL::to_double( pt.x() - boundingBox.xmin() ) ) * BITMAP_SCALING );
        y = static_cast< int >( ( CGAL::to_double( pt.y() - boundingBox.ymin() ) ) * BITMAP_SCALING );
    }

    // basis to convert back bitmap segments to arrangement points
    exact::Point operator()( const int x, const int y ) const
    {
        return exact::Point{
            ( 0.75 + x + boundingBox.xmin() ) / BITMAP_SCALING, ( 0.75 + y + boundingBox.ymin() ) / BITMAP_SCALING };
    }
};

void renderPoly( const SchematicToBitmap& converter, agg::path_storage& path, const Analysis::Partition*,
                 const Analysis::HalfEdgeVector& poly )
{
    double x, y;
    for( auto i = poly.begin(), iEnd = poly.end(), iNext = poly.begin(); i != iEnd; ++i )
    {
        converter( *i, x, y );

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

void renderPolyWithHoles( const SchematicToBitmap& converter, schematic::Rasteriser& raster,
                          const Analysis::Partition*                pPartition,
                          const Analysis::HalfEdgePolygonWithHoles& polyWithHoles, agg::gray8 colour )
{
    schematic::Rasteriser::PolygonRasterizer ras;
    ras.gamma( agg::gamma_threshold( 1.0 ) );

    {
        agg::path_storage path;
        renderPoly( converter, path, pPartition, polyWithHoles.outer );
        ras.add_path( path );
    }
    for( const auto& hole : polyWithHoles.holes )
    {
        agg::path_storage path;
        renderPoly( converter, path, pPartition, hole );
        ras.add_path( path );
    }
    raster.render( ras, colour );
}

void renderPolyWithHolesClearance( const SchematicToBitmap& converter, schematic::Rasteriser& raster,
                                   const Analysis::Partition*                pPartition,
                                   const Analysis::HalfEdgePolygonWithHoles& polyWithHoles, agg::gray8 colour,
                                   float fClearance )
{
    schematic::Rasteriser::PolygonRasterizer ras;
    ras.gamma( agg::gamma_threshold( 0.5 ) );

    {
        agg::path_storage path;
        renderPoly( converter, path, pPartition, polyWithHoles.outer );
        agg::conv_stroke< agg::path_storage > contour( path );
        {
            contour.width( fClearance );
        }
        ras.add_path( contour );
    }
    for( const auto& hole : polyWithHoles.holes )
    {
        agg::path_storage path;
        renderPoly( converter, path, pPartition, hole );
        agg::conv_stroke< agg::path_storage > contour( path );
        {
            contour.width( 4.0 );
        }
        ras.add_path( contour );
    }
    raster.render( ras, colour );
}

void calculateLaneSegments( const SchematicToBitmap& converter, schematic::Rasteriser& raster,
                            const SearchCoeffs& coeffs, Adjacency& adjacency,
                            const Analysis::HalfEdgeCstVector& doorSteps, ValueSegmentVector& segments,
                            ValueSegmentVector& doorStepSegments )
{
    using DoorStep     = std::pair< int, int >;
    using DoorStepsVec = std::vector< DoorStep >;
    DoorStepsVec doorStepPoints;

    for( auto e : doorSteps )
    {
        const auto       v  = e->target()->point() - e->source()->point();
        const auto       n  = v.perpendicular( CGAL::Orientation::COUNTERCLOCKWISE );
        const auto       p1 = e->source()->point() + ( v / 2.0 );
        const exact::Ray r( p1, n );

        exact::Point p2;
        {
            schematic::Connection::PtrCst pConnection = e->data().pConnection;
            INVARIANT( pConnection, "Doorstep missing connection" );

            static ::exact::InexactToExact inexactToExactConverter;
            const exact::Transform         transform = pConnection->getAbsoluteExactTransform();
            const schematic::Segment       firstSeg  = pConnection->getFirstSegment();
            const schematic::Segment       secondSeg = pConnection->getSecondSegment();
            const exact::Point             ptFirstStart( transform( inexactToExactConverter( firstSeg[ 0 ] ) ) );
            const exact::Point             ptFirstEnd( transform( inexactToExactConverter( firstSeg[ 1 ] ) ) );
            const exact::Point             ptSecondStart( transform( inexactToExactConverter( secondSeg[ 0 ] ) ) );
            const exact::Point             ptSecondEnd( transform( inexactToExactConverter( secondSeg[ 1 ] ) ) );
            const exact::Segment           crossFirst( ptFirstStart, ptSecondStart );
            const exact::Segment           crossSecond( ptFirstEnd, ptSecondEnd );

            bool bSuccess = false;
            if( auto result1 = CGAL::intersection( r, crossFirst ) )
            {
                if( const exact::Point* pResult1 = boost::get< exact::Point >( &*result1 ) )
                {
                    p2       = *pResult1;
                    bSuccess = true;
                }
            }
            else if( auto result2 = CGAL::intersection( r, crossSecond ) )
            {
                if( const exact::Point* pResult2 = boost::get< exact::Point >( &*result2 ) )
                {
                    p2       = *pResult2;
                    bSuccess = true;
                }
            }
            INVARIANT( bSuccess, "Failed to resolve doorstep connection distance" );
        }

        int x0, y0;
        converter( p1, x0, y0 );
        int x1, y1;
        converter( p2, x1, y1 );

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

using ExtrusionSpec = std::tuple< double, EdgeMask::Type, EdgeMask::Type >;

struct Component
{
    std::vector< Analysis::Partition* > partitions;
    // Analysis::HalfEdgeSet               doorSteps;
    //  bInclude indicates that this component contains gutter partitions
    bool bInclude = false;
    using Vector  = std::vector< Component >;
};

template < typename PartitionEdgeFunctor, typename PartitionFunctor, std::size_t TotalExtrusions >
void generateLaneExtrusion( Analysis::Arrangement& arr, const Analysis::Partition::PtrVector& floors,
                            Analysis::HalfEdgeSet&                              doorSteps,
                            const std::array< ExtrusionSpec, TotalExtrusions >& extrusions,
                            PartitionEdgeFunctor&& isPartitionEdge, PartitionFunctor&& isPartition,
                            Component::Vector& components, const std::vector< EdgeMask::Type >& boundaryMasks )
{
    Analysis::HalfEdgeVectorVector doorStepGroups( floors.size() );
    Analysis::HalfEdgeCstSet       allUsedDoorSteps;

    // determine the connected components of partitions for gutters and pavements
    using Graph = boost::adjacency_list< boost::vecS, boost::vecS, boost::undirectedS >;
    Graph graph( floors.size() );
    {
        using PartitionEdges = std::vector< std::pair< int, int > >;
        PartitionEdges edges;
        for( auto e : doorSteps )
        {
            auto pPartition1 = e->data().pPartition;
            auto pPartition2 = e->twin()->data().pPartition;
            if( isPartitionEdge( pPartition1, pPartition2 ) )
            {
                edges.push_back( { pPartition1->uniqueID, pPartition2->uniqueID } );
                INVARIANT(
                    ( pPartition1->uniqueID >= 0 ) && ( pPartition1->uniqueID < static_cast< int >( floors.size() ) ),
                    "Invalid partition uniqueID" );
                INVARIANT(
                    ( pPartition2->uniqueID >= 0 ) && ( pPartition2->uniqueID < static_cast< int >( floors.size() ) ),
                    "Invalid partition uniqueID" );
                doorStepGroups[ pPartition1->uniqueID ].push_back( e );
                doorStepGroups[ pPartition2->uniqueID ].push_back( e->twin() );
                allUsedDoorSteps.insert( e );
                allUsedDoorSteps.insert( e->twin() );
            }
        }
        for( const auto& e : edges )
        {
            boost::add_edge( e.first, e.second, graph );
        }
    }

    std::vector< int > componentIndices( floors.size() );
    const auto         iNumComponents = boost::connected_components( graph, componentIndices.data() );
    components.resize( iNumComponents );
    using PartitionToComponentMap = std::map< Analysis::Partition*, typename Component::Vector::const_iterator >;
    PartitionToComponentMap partitionsToComponents;
    for( std::size_t iPartition = 0U; iPartition != floors.size(); ++iPartition )
    {
        const int            iComponentIndex = componentIndices[ iPartition ];
        Component&           component       = components[ iComponentIndex ];
        Analysis::Partition* pPartition      = floors[ iPartition ].get();
        component.partitions.push_back( pPartition );
        partitionsToComponents.insert( { pPartition, components.begin() + iComponentIndex } );
        if( isPartition( pPartition ) )
        {
            component.bInclude = true;
        }
        // for( auto e : doorStepGroups[ iPartition ] )
        //{
        // component.doorSteps.insert( e );
        //}
    }

    using ComponentFloorMap
        = std::map< typename Component::Vector::const_iterator, Analysis::HalfEdgePolygonWithHoles >;
    ComponentFloorMap componentFloors;
    {
        // like Analysis::getFloorPartitions but DO NOT USE doorsteps in allUsedDoorSteps
        using PolygonNode = PolygonNodeT< Analysis::HalfEdge, Analysis::Face, char >;
        using NodePtr     = typename PolygonNode::Ptr;
        using NodeVector  = std::vector< NodePtr >;
        NodeVector nodes;
        {
            Analysis::HalfEdgeVectorVector floorPolygons;
            {
                Analysis::HalfEdgeSet partitionFloorEdges;
                getEdges( arr, partitionFloorEdges,
                          [ &allUsedDoorSteps ]( Analysis::HalfEdge edge )
                          { return test( edge, EdgeMask::ePartitionFloor ) && !allUsedDoorSteps.contains( edge ); } );
                getPolygonsDir( partitionFloorEdges, floorPolygons, true );
            }
            for( auto& poly : floorPolygons )
            {
                nodes.push_back( std::make_shared< PolygonNode >( poly ) );
            }
        }

        auto rootNodes = sortPolygonNodes< Analysis::HalfEdge, Analysis::Face, char >( nodes );
        INVARIANT( rootNodes.size() == 1, "getFloorPartitions did not find singular root node" );

        struct Visitor
        {
            ComponentFloorMap&          floors;
            typename Component::Vector& components;
            PartitionToComponentMap&    partitionsToComponents;

            void floor( const PolygonNode& node )
            {
                // NOTE: does not matter WHICH partition is found i.e. multiple partitions
                // can belong to the same PolygonNode but they MUST ALL belong to the same component
                auto e          = node.polygon().front();
                auto pPartition = e->data().pPartition;
                INVARIANT( pPartition, "Floor edge missing partition" );
                INVARIANT( pPartition->pSite, "Floor partition missing site" );
                INVARIANT( node.face(), "Floor is not a face" );

                auto iFind = partitionsToComponents.find( pPartition );
                INVARIANT( iFind != partitionsToComponents.end(), "Failed to locate component" );

                Analysis::HalfEdgePolygonWithHoles polyWithHoles;
                polyWithHoles.outer = node.polygon();

                for( auto pContour : node.contained() )
                {
                    INVARIANT( !pContour->face(), "Hole is a face" );
                    polyWithHoles.holes.push_back( pContour->polygon() );
                    for( auto pInner : pContour->contained() )
                    {
                        floor( *pInner );
                    }
                }
                floors.insert( { iFind->second, polyWithHoles } );
            }
        } visitor{ componentFloors, components, partitionsToComponents };

        for( auto pNode : rootNodes )
        {
            visitor.floor( *pNode );
        }
    }

    // std::map< typename Component::Vector::const_iterator, Analysis::HalfEdgePolygonWithHoles >;
    for( const auto& [ componentIter, componentPolygonWithHoles ] : componentFloors )
    {
        const Component& component = *componentIter;
        if( component.bInclude )
        {
            // mask the actual polygon with holes
            for( auto m : boundaryMasks )
            {
                for( auto e : componentPolygonWithHoles.outer )
                {
                    classify( e, m );
                }
                for( const auto& hole : componentPolygonWithHoles.holes )
                {
                    for( auto e : hole )
                    {
                        classify( e, m );
                    }
                }
            }

            // calculate extrusion around the corridor for gutter style lanes
            const exact::Polygon_with_holes polygonWithHoles
                = exact::fromHalfEdgePolygonWithHoles( componentPolygonWithHoles );
            using StraightSkeleton    = CGAL::Straight_skeleton_2< exact::Kernel >;
            using StraightSkeletonPtr = boost::shared_ptr< StraightSkeleton >;

            StraightSkeletonPtr pStraightSkeleton = CGAL::create_interior_straight_skeleton_2(
                polygonWithHoles.outer_boundary().begin(), polygonWithHoles.outer_boundary().end(),
                polygonWithHoles.holes_begin(), polygonWithHoles.holes_end(), exact::Kernel() );

            for( const auto& [ fOffset, mask, maskBoundary ] : extrusions )
            {
                auto result = CGAL::create_offset_polygons_2( fOffset, *pStraightSkeleton );
                for( const auto& poly : result )
                {
                    auto i = poly->begin(), iNext = poly->begin(), iEnd = poly->end();
                    for( ; i != iEnd; ++i )
                    {
                        ++iNext;
                        if( iNext == iEnd )
                            iNext = poly->begin();
                        renderCurve( arr, Curve( Point( i->x(), i->y() ), Point( iNext->x(), iNext->y() ) ), mask,
                                     maskBoundary, {}, {} );
                    }
                }
            }
        }
    }
}

} // namespace

void Analysis::lanes()
{
    const schematic::Schematic::LaneConfig laneConfig = m_pSchematic->getLaneConfig();

    // first can generate the offset based gutters and pavements
    Component::Vector gutterComponents;
    {
        HalfEdgeSet doorSteps;
        getEdges( m_arr, doorSteps, []( HalfEdge e ) { return test( e, EdgeMask::eDoorStep ); } );

        const std::array< ExtrusionSpec, 3 > extrusions
            = { ExtrusionSpec{ laneConfig.laneRadius, EdgeMask::eLane, EdgeMask::eLane },
                ExtrusionSpec{ laneConfig.laneRadius * 2.0, EdgeMask::eLaneInnerBoundary, EdgeMask::eLaneInner },
                ExtrusionSpec{ laneConfig.laneRadius * 2.0 + laneConfig.laneLining, EdgeMask::eLaneOuterBoundary,
                               EdgeMask::eLaneOuter } };
        generateLaneExtrusion(
            m_arr, m_floors, doorSteps, extrusions,
            []( Partition* pLeft, Partition* pRight ) { return pLeft->bHasGutter && pRight->bHasGutter; },
            []( Partition* pPartition ) { return pPartition->bHasGutter; }, gutterComponents,
            { EdgeMask::eLaneInner, EdgeMask::eLaneOuter } );
    }

    // now generate the astar search based remaining lanes
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
    m_laneBitmap.resize( std::ceil( width ) * SchematicToBitmap::BITMAP_SCALING,
                         std::ceil( height ) * SchematicToBitmap::BITMAP_SCALING );
    m_laneBitmap.reset();
    m_pSchematic->setLaneBitmapOffset(
        schematic::Vector{ CGAL::to_double( boundingBox.xmin() ), CGAL::to_double( boundingBox.ymin() ) },
        SchematicToBitmap::BITMAP_SCALING );
    schematic::Rasteriser   raster( m_laneBitmap, false );
    const SchematicToBitmap converter{ boundingBox };

    // using FloorPolyMap = std::map< const Analysis::Partition*, Analysis::HalfEdgePolygonWithHoles >;
    FloorPolyMap floors;
    getFloorPartitions( floors );

    for( const auto& [ pPartition, polyWithHoles ] : floors )
    {
        renderPolyWithHoles( converter, raster, pPartition, polyWithHoles, colourSpace );
    }

    // TODO set clearance per partition via properties
    for( const auto& [ pPartition, polyWithHoles ] : floors )
    {
        renderPolyWithHolesClearance( converter, raster, pPartition, polyWithHoles, colourAvoid, laneConfig.clearance );
    }

    const SearchCoeffs coeffs;
    Adjacency          adjacency{ raster };

    const exact::Polygon octogonInner = makeOctogon( laneConfig.laneRadius );
    const exact::Polygon octogonOuter = makeOctogon( laneConfig.laneRadius + laneConfig.laneLining );

    // NOTE: can only write the partition lane segments AFTER all are calculated
    // because any doorstep edge intersection will break calculation
    struct PartitionLaneSegments
    {
        ValueSegmentVector segments;
        ValueSegmentVector doorStepSegments;
    };
    std::vector< PartitionLaneSegments > partitionSegments;

    // for each floor partition
    for( const auto& component : gutterComponents )
    {
        if( component.bInclude )
        {
            // empty doorsteps for gutter
            Analysis::HalfEdgeCstVector doorSteps;

            // locate all systems in the set of partitions
            // component.partitions

            if( !doorSteps.empty() )
            {
                PartitionLaneSegments segments;
                calculateLaneSegments(
                    converter, raster, coeffs, adjacency, doorSteps, segments.segments, segments.doorStepSegments );
                partitionSegments.emplace_back( segments );
            }
        }
        else
        {
            INVARIANT( component.partitions.size() == 1, "Invalid non-gutter partition" );

            auto iFind = floors.find( component.partitions.front() );
            INVARIANT( iFind != floors.end(), "Failed to locate floor" );

            const Analysis::HalfEdgePolygonWithHoles& polyWithHoles = iFind->second;
            Analysis::HalfEdgeCstVector               doorSteps;
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
            INVARIANT( !doorSteps.empty(), "No doorsteps in non-gutter partition" );
            if( !doorSteps.empty() )
            {
                PartitionLaneSegments segments;
                calculateLaneSegments(
                    converter, raster, coeffs, adjacency, doorSteps, segments.segments, segments.doorStepSegments );
                partitionSegments.emplace_back( segments );
            }
        }
    }

    // render generated lane segments into the arrangement
    for( const auto& segments : partitionSegments )
    {
        // render the octagonal lane geometry
        for( const auto& segment : segments.segments )
        {
            const Curve curve{
                converter( segment.first.x, segment.first.y ), converter( segment.second.x, segment.second.y ) };
            renderCurve( m_arr, curve, EdgeMask::eLane, EdgeMask::eLane );
            renderExtrudedCurve( m_arr, curve, octogonInner, EdgeMask::eLaneInner, EdgeMask::eLaneInnerBoundary );
            renderExtrudedCurve( m_arr, curve, octogonOuter, EdgeMask::eLaneOuter, EdgeMask::eLaneOuterBoundary );
        }
        for( const auto& segment : segments.doorStepSegments )
        {
            const Curve curve{
                converter( segment.first.x, segment.first.y ), converter( segment.second.x, segment.second.y ) };
            renderCurve( m_arr, curve, EdgeMask::eLane, EdgeMask::eLane );
            renderExtrudedCurve( m_arr, curve, octogonInner, EdgeMask::eLaneInner, EdgeMask::eLaneInnerBoundary );
            renderExtrudedCurve( m_arr, curve, octogonOuter, EdgeMask::eLaneOuter, EdgeMask::eLaneOuterBoundary );
        }
    }

    // generate pavements
    {
        HalfEdgeSet doorSteps;
        getEdges( m_arr, doorSteps, []( HalfEdge e ) { return test( e, EdgeMask::eDoorStep ); } );

        Component::Vector                    pavementComponents;
        const std::array< ExtrusionSpec, 2 > extrusions
            = { ExtrusionSpec{
                    laneConfig.pavementRadius * 2.0, EdgeMask::ePavementInnerBoundary, EdgeMask::ePavementInner },
                ExtrusionSpec{ laneConfig.pavementRadius * 2.0 + laneConfig.pavementLining,
                               EdgeMask::ePavementOuterBoundary, EdgeMask::ePavementOuter } };
        generateLaneExtrusion(
            m_arr, m_floors, doorSteps, extrusions,
            []( Partition* pLeft, Partition* pRight ) { return pLeft->bHasPavement && pRight->bHasPavement; },
            []( Partition* pPartition ) { return pPartition->bHasPavement; }, pavementComponents,
            { EdgeMask::ePavementInner, EdgeMask::ePavementOuter } );
    }

    // calculate the actual partition subdivisions
    {
        FaceVector laneInnerFaces;
        {
            FaceSet     laneInnerStartFaces;
            HalfEdgeSet laneInnerEdges;
            getEdges(
                m_arr, laneInnerEdges, []( Analysis::HalfEdge edge ) { return test( edge, EdgeMask::eLaneInner ); } );
            for( auto e : laneInnerEdges )
            {
                laneInnerStartFaces.insert( e->face() );
            }
            getSortedFaces< HalfEdge, Face >( laneInnerStartFaces, laneInnerFaces,
                                              [ &laneInnerEdges ]( HalfEdge e )
                                              { return !laneInnerEdges.contains( e ); } );
        }

        FaceVector laneOuterFaces;
        {
            FaceSet     laneOuterStartFaces;
            HalfEdgeSet laneOuterEdges;
            getEdges(
                m_arr, laneOuterEdges, []( Analysis::HalfEdge edge ) { return test( edge, EdgeMask::eLaneOuter ); } );
            for( auto e : laneOuterEdges )
            {
                laneOuterStartFaces.insert( e->face() );
            }
            getSortedFaces< HalfEdge, Face >( laneOuterStartFaces, laneOuterFaces,
                                              [ &laneOuterEdges ]( HalfEdge e )
                                              { return !laneOuterEdges.contains( e ); } );
        }

        FaceVector pavementInnerFaces;
        {
            FaceSet     pavementInnerStartFaces;
            HalfEdgeSet pavementInnerEdges;
            getEdges( m_arr, pavementInnerEdges,
                      []( Analysis::HalfEdge edge ) { return test( edge, EdgeMask::ePavementInner ); } );
            for( auto e : pavementInnerEdges )
            {
                pavementInnerStartFaces.insert( e->face() );
            }
            getSortedFaces< HalfEdge, Face >( pavementInnerStartFaces, pavementInnerFaces,
                                              [ &pavementInnerEdges ]( HalfEdge e )
                                              { return !pavementInnerEdges.contains( e ); } );
        }

        FaceVector pavementOuterFaces;
        {
            FaceSet     pavementOuterStartFaces;
            HalfEdgeSet pavementOuterEdges;
            getEdges( m_arr, pavementOuterEdges,
                      []( Analysis::HalfEdge edge ) { return test( edge, EdgeMask::ePavementOuter ); } );
            for( auto e : pavementOuterEdges )
            {
                pavementOuterStartFaces.insert( e->face() );
            }
            getSortedFaces< HalfEdge, Face >( pavementOuterStartFaces, pavementOuterFaces,
                                              [ &pavementOuterEdges ]( HalfEdge e )
                                              { return !pavementOuterEdges.contains( e ); } );
        }

        // like Analysis::getFloorPartitions but DO NOT USE doorsteps in allUsedDoorSteps
        using PolygonNode = PolygonNodeT< Analysis::HalfEdge, Analysis::Face, char >;
        using NodePtr     = typename PolygonNode::Ptr;
        using NodeVector  = std::vector< NodePtr >;
        NodeVector nodes;
        {
            HalfEdgeVectorVector floorPolygons;
            {
                HalfEdgeSet partitionFloorEdges;
                getEdges( m_arr, partitionFloorEdges,
                          []( HalfEdge edge ) { return test( edge, EdgeMask::ePartitionFloor ); } );
                getPolygonsDir( partitionFloorEdges, floorPolygons, true );
            }
            for( auto& poly : floorPolygons )
            {
                nodes.push_back( std::make_shared< PolygonNode >( poly ) );
            }
        }

        auto rootNodes = sortPolygonNodes< HalfEdge, Face, char >( nodes );
        INVARIANT( rootNodes.size() == 1, "sortPolygonNodes did not find singular root node" );

        using PartitionFaceMap = std::map< Partition*, FaceVector >;
        PartitionFaceMap partitionFaces;
        {
            struct Visitor
            {
                PartitionFaceMap& partitionFaces;

                void floor( NodePtr pNode )
                {
                    FaceVector faces = pNode->faces();

                    for( auto pContained : pNode->contained() )
                    {
                        FaceVector diff;
                        std::set_difference( faces.begin(), faces.end(), pContained->faces().begin(),
                                             pContained->faces().end(), std::back_inserter( diff ) );
                        faces.swap( diff );
                    }

                    INVARIANT( !faces.empty(), "No faces in partition" );
                    Face       f          = faces.front();
                    Partition* pPartition = f->data().pPartition;
                    INVARIANT( pPartition, "Face has no partition" );
                    auto ib = partitionFaces.insert( { pPartition, faces } );
                    INVARIANT( ib.second, "Found duplicate partitions" );

                    for( auto pContained : pNode->contained() )
                    {
                        for( auto pFloor : pContained->contained() )
                        {
                            floor( pFloor );
                        }
                    }
                }

            } visitor{ partitionFaces };
            for( auto n : rootNodes )
            {
                visitor.floor( n );
            }
        }

        for( auto& pPartition : m_floors )
        {
            auto iFind = partitionFaces.find( pPartition.get() );
            INVARIANT( iFind != partitionFaces.end(), "Parition missing faces" );

            FaceVector partitionFacesRemaining = iFind->second;
            auto       calculateRemaining      = [ &partitionFacesRemaining ]( const FaceVector& used )
            {
                FaceVector remaining;
                std::set_difference( partitionFacesRemaining.begin(), partitionFacesRemaining.end(), used.begin(),
                                     used.end(), std::back_inserter( remaining ) );
                partitionFacesRemaining.swap( remaining );
            };

            // determine the laneInnerFaces for this partition
            {
                FaceVector partitionLaneInnerFaces;
                std::set_intersection( partitionFacesRemaining.begin(), partitionFacesRemaining.end(),
                                       laneInnerFaces.begin(), laneInnerFaces.end(),
                                       std::back_inserter( partitionLaneInnerFaces ) );
                calculateRemaining( partitionLaneInnerFaces );

                HalfEdgeVector boundary;
                getFacesBoundary( partitionLaneInnerFaces, boundary );
                for( auto e : boundary )
                {
                    classify( e, EdgeMask::eLaneSpace );
                    e->data().pPartition = pPartition.get();
                }
            }

            // determine the laneOuterFaces for this partition
            {
                FaceVector partitionLaneOuterFaces;
                FaceVector partitionLaneOuterFacesTemp;
                std::set_intersection( partitionFacesRemaining.begin(), partitionFacesRemaining.end(),
                                       laneOuterFaces.begin(), laneOuterFaces.end(),
                                       std::back_inserter( partitionLaneOuterFaces ) );
                calculateRemaining( partitionLaneOuterFaces );

                HalfEdgeVector boundary;
                getFacesBoundary( partitionLaneOuterFaces, boundary );
                for( auto e : boundary )
                {
                    classify( e, EdgeMask::eLaneLining );
                    e->data().pPartition = pPartition.get();
                }
            }

            {
                FaceVector partitionPavementInnerFaces;
                std::set_intersection( partitionFacesRemaining.begin(), partitionFacesRemaining.end(),
                                       pavementInnerFaces.begin(), pavementInnerFaces.end(),
                                       std::back_inserter( partitionPavementInnerFaces ) );
                calculateRemaining( partitionPavementInnerFaces );

                HalfEdgeVector boundary;
                getFacesBoundary( partitionPavementInnerFaces, boundary );
                for( auto e : boundary )
                {
                    classify( e, EdgeMask::ePavementSpace );
                    e->data().pPartition = pPartition.get();
                }
            }

            {
                FaceVector partitionPavementOuterFaces;
                std::set_intersection( partitionFacesRemaining.begin(), partitionFacesRemaining.end(),
                                       pavementOuterFaces.begin(), pavementOuterFaces.end(),
                                       std::back_inserter( partitionPavementOuterFaces ) );
                calculateRemaining( partitionPavementOuterFaces );

                HalfEdgeVector boundary;
                getFacesBoundary( partitionPavementOuterFaces, boundary );
                for( auto e : boundary )
                {
                    classify( e, EdgeMask::ePavementLining );
                    e->data().pPartition = pPartition.get();
                }
            }

            // everything left is road
            {
                HalfEdgeVector boundary;
                getFacesBoundary( partitionFacesRemaining, boundary );
                for( auto e : boundary )
                {
                    classify( e, EdgeMask::eRoad );
                    e->data().pPartition = pPartition.get();
                }
            }
        }
    }
}

} // namespace exact
