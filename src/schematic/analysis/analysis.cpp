
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
#include "polygon_tree.hpp"

#include "schematic/analysis/polygon_with_holes.hpp"
#include "schematic/analysis/analysis.hpp"
#include "schematic/schematic.hpp"

#include <vector>
#include <tuple>

namespace exact
{

Analysis::Analysis( schematic::Schematic::Ptr pSchematic )
    : m_pSchematic( pSchematic )
    , m_observer( m_arr )
    , m_laneBitmap( pSchematic->getLaneBitmap() )
{
}

void Analysis::getAllEdges( std::vector< std::pair< schematic::Segment, EdgeMask::Set > >& edges ) const
{
    exact::ExactToInexact convert;
    for( auto i = m_arr.halfedges_begin(); i != m_arr.halfedges_end(); ++i )
    {
        edges.emplace_back(
            schematic::Segment{ convert( i->source()->point() ), convert( i->target()->point() ) }, i->data().flags );
    }
}

namespace
{
template < typename ArrangementType, typename HalfEdgeType, typename FaceType >
void getFloorPartitions(
    ArrangementType&                                                                          arr,
    std::map< const Analysis::Partition*, exact::HalfEdgePolygonWithHolesT< HalfEdgeType > >& floors )
{
    using HalfEdge      = HalfEdgeType;
    using Face          = FaceType;
    using PolygonNode   = PolygonNodeT< HalfEdgeType, FaceType, bool >;
    using NodePtr       = typename PolygonNode::Ptr;
    using NodeVector    = std::vector< NodePtr >;
    using PolyWithHoles = exact::HalfEdgePolygonWithHolesT< HalfEdge >;

    NodeVector nodes;
    {
        std::vector< std::vector< HalfEdgeType > > floorPolygons;
        {
            std::set< HalfEdge > outerEdges;
            getEdges( arr, outerEdges, []( HalfEdge edge ) { return test( edge, EdgeMask::ePartitionFloor ); } );
            getPolygonsDir( outerEdges, floorPolygons, true );
        }
        for( auto& poly : floorPolygons )
        {
            nodes.push_back( std::make_shared< PolygonNode >( poly ) );
        }
    }

    auto rootNodes = sortPolygonNodes< HalfEdge, Face, bool >( nodes );
    INVARIANT( rootNodes.size() == 1, "getFloorPartitions did not find singular root node" );

    struct Visitor
    {
        std::map< const Analysis::Partition*, PolyWithHoles >& floors;

        void floor( const PolygonNode& node )
        {
            auto e          = node.polygon().front();
            auto pPartition = e->data().pPartition;
            INVARIANT( pPartition, "Floor edge missing partition" );
            INVARIANT( pPartition->pSite, "Floor partition missing site" );
            INVARIANT( node.face(), "Floor is not a face" );

            PolyWithHoles polyWithHoles;
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
            floors.insert( { pPartition, polyWithHoles } );
        }
    } visitor{ floors };

    for( auto pNode : rootNodes )
    {
        visitor.floor( *pNode );
    }
}
} // namespace

void Analysis::getFloorPartitions( std::map< const Analysis::Partition*, Analysis::HalfEdgePolygonWithHoles >& floors )
{
    exact::getFloorPartitions< Arrangement, HalfEdge, Face >( m_arr, floors );
}

void Analysis::getFloorPartitions(
    std::map< const Analysis::Partition*, Analysis::HalfEdgeCstPolygonWithHoles >& floors ) const
{
    exact::getFloorPartitions< const Arrangement, HalfEdgeCst, FaceCst >( m_arr, floors );
}

void Analysis::getFloorPartitions( std::map< const Analysis::Partition*, exact::Polygon_with_holes >& floors ) const
{
    std::map< const Analysis::Partition*, Analysis::HalfEdgeCstPolygonWithHoles > halfEdgeFloors;
    getFloorPartitions( halfEdgeFloors );

    for( const auto& [ pPartition, polyWithHoles ] : halfEdgeFloors )
    {
        floors.insert( { pPartition, fromHalfEdgePolygonWithHoles( polyWithHoles ) } );
    }
}

void Analysis::getBoundaryPartitions( Analysis::HalfEdgeCstVectorVector& boundarySegments ) const
{
    Analysis::HalfEdgeCstSet edges;
    getEdges( m_arr, edges,
              []( Arrangement::Halfedge_const_handle edge )
              { return test( edge, EdgeMask::ePartitionBoundarySegment ); } );
    getPolygonsDir( edges, boundarySegments, true );
}

void Analysis::getBoundaryPartitions( std::map< const Analysis::PartitionSegment*, exact::Polygon >& boundaries ) const
{
    Analysis::HalfEdgeCstVectorVector boundarySegments;
    getBoundaryPartitions( boundarySegments );
    for( const auto& boundarySegment : boundarySegments )
    {
        auto pSegment = boundarySegment.front()->data().pPartitionSegment;
        for( auto& e : boundarySegment )
        {
            INVARIANT( e->data().pPartitionSegment == pSegment, "Inconsistent partition segment" );
        }

        INVARIANT( pSegment, "Failed to locate boundary segment" );
        boundaries.insert( { pSegment, fromHalfEdgePolygon( boundarySegment ) } );
    }
}

void Analysis::getVertices( VertexCstVector& vertices ) const
{
    for( auto i = m_arr.vertices_begin(); i != m_arr.vertices_end(); ++i )
    {
        vertices.push_back( i );
    }
}

void Analysis::getPerimeterPolygon( HalfEdgeCstVector& polygon ) const
{
    HalfEdgeCstSet perimeterEdges;
    getEdges( m_arr, perimeterEdges,
              []( Arrangement::Halfedge_const_handle edge ) { return test( edge, EdgeMask::ePerimeter ); } );

    HalfEdgeCstVectorVector polygons;
    getPolygons( perimeterEdges, polygons );

    INVARIANT( polygons.size() == 1, "Did not find single perimeter polygon" );
    polygon = polygons.front();
}

Analysis::SkeletonRegionQuery::SkeletonRegionQuery( Analysis& analysis )
{
    getEdges( analysis.m_arr, m_skeletonEdges,
              []( Analysis::HalfEdgeCst edge )
              {
                  // clang-format off
                return test( edge, EdgeMask::eSkeleton ) && 
                !(
                    test( edge, EdgeMask::eLaneSpace ) || 
                    test( edge, EdgeMask::eLaneLining ) || 
                    test( edge, EdgeMask::ePavementSpace ) || 
                    test( edge, EdgeMask::ePavementLining ) || 
                    test( edge, EdgeMask::eRoad )
                );
                  // clang-format on
              } );
}

Analysis::VertexCstVector Analysis::SkeletonRegionQuery::getRegion( Analysis::VertexCst   v,
                                                                    Analysis::HalfEdgeCst eNext ) const
{
    Analysis::VertexCstVector region;
    searchSkeletonRegion< Analysis::VertexCst, Analysis::HalfEdgeCst >( v, eNext, m_skeletonEdges, region );
    return region;
}

} // namespace exact
