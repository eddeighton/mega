
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

#include "schematic/analysis/analysis.hpp"
#include "schematic/schematic.hpp"
#include "schematic/cut.hpp"

#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/create_straight_skeleton_from_polygon_with_holes_2.h>
#include <CGAL/create_offset_polygons_from_polygon_with_holes_2.h>
#include <CGAL/create_offset_polygons_2.h>

#include <vector>
#include <tuple>

namespace exact
{

Analysis::Analysis( schematic::Schematic::Ptr pSchematic )
    : m_pSchematic( pSchematic )
    , m_observer( m_arr )
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

void Analysis::getVertices( VertexVector& vertices ) const
{
    for( auto i = m_arr.vertices_begin(); i != m_arr.vertices_end(); ++i )
    {
        vertices.push_back( i );
    }
}

void Analysis::getPerimeterPolygon( HalfEdgeVector& polygon ) const
{
    HalfEdgeSet perimeterEdges;
    getEdges( m_arr, perimeterEdges,
              []( Arrangement::Halfedge_const_handle edge )
              { return edge->data().flags.test( EdgeMask::ePerimeter ); } );

    HalfEdgeVectorVector polygons;
    getPolygons( perimeterEdges, polygons );

    INVARIANT( polygons.size() == 1, "Did not find single perimeter polygon" );
    polygon = polygons.front();
}

Analysis::Floor::Vector Analysis::getFloors()
{
    Floor::Vector floors;

    struct Visitor
    {
        Floor::Vector& floors;

        void floor( PolygonNode& node ) const
        {
            INVARIANT( node.outer(), "Found hole when expecting contour" );

            Floor theFloor;
            auto  f              = node.faces().front();
            theFloor.pPartition  = f->data().pPartition;
            theFloor.floor.outer = node.polygon();

            for( auto pHole : node.contained() )
            {
                floorInner( *pHole, theFloor, theFloor.floor );
            }

            floors.emplace_back( theFloor );
        }

        void floorOrHole( PolygonNode& node, Floor& theFloor, HalfEdgePolygonWithHoles& polyWithHoles ) const
        {
            for( auto pContour : node.contained() )
            {
                if( pContour->polygon().front()->data().flags.test( EdgeMask::ePartitionFloor ) )
                {
                    floor( *pContour );
                }
                else if( pContour->polygon().front()->data().flags.test( EdgeMask::ePartitionBoundary )
                         || pContour->polygon().front()->data().flags.test( EdgeMask::eExtrusionOneBoundary )
                         || pContour->polygon().front()->data().flags.test( EdgeMask::eExtrusionTwoBoundary )
                         || pContour->polygon().front()->data().flags.test( EdgeMask::eExtrusionThreeBoundary )
                         || pContour->polygon().front()->data().flags.test( EdgeMask::eExtrusionFourBoundary ) )
                {
                    floorInner( *pContour, theFloor, polyWithHoles );
                }
                else
                {
                    INVARIANT( false, "Unexpected edge type in polygon" );
                }
            }
        }

        void floorEx1( PolygonNode& node, Floor& theFloor ) const
        {
            INVARIANT( node.outer(), "Found hole when expecting contour" );
            HalfEdgePolygonWithHoles polyWithHoles;
            polyWithHoles.outer = node.polygon();
            floorOrHole( node, theFloor, polyWithHoles );
            theFloor.ex1.push_back( polyWithHoles );
        }

        void floorEx2( PolygonNode& node, Floor& theFloor ) const
        {
            INVARIANT( node.outer(), "Found hole when expecting contour" );
            HalfEdgePolygonWithHoles polyWithHoles;
            polyWithHoles.outer = node.polygon();
            floorOrHole( node, theFloor, polyWithHoles );
            theFloor.ex2.push_back( polyWithHoles );
        }

        void floorEx3( PolygonNode& node, Floor& theFloor ) const
        {
            INVARIANT( node.outer(), "Found hole when expecting contour" );
            HalfEdgePolygonWithHoles polyWithHoles;
            polyWithHoles.outer = node.polygon();
            floorOrHole( node, theFloor, polyWithHoles );
            theFloor.ex3.push_back( polyWithHoles );
        }

        void floorEx4( PolygonNode& node, Floor& theFloor ) const
        {
            INVARIANT( node.outer(), "Found hole when expecting contour" );
            HalfEdgePolygonWithHoles polyWithHoles;
            polyWithHoles.outer = node.polygon();
            floorOrHole( node, theFloor, polyWithHoles );
            theFloor.ex4.push_back( polyWithHoles );
        }

        void floorInner( PolygonNode& node, Floor& theFloor, HalfEdgePolygonWithHoles& poly ) const
        {
            INVARIANT( node.inner(), "Found contour when expecting hole" );

            poly.holes.push_back( node.polygon() );

            for( auto pContour : node.contained() )
            {
                // classify the outer
                INVARIANT( pContour->outer(), "Inner within inner" );

                if( pContour->face() )
                {
                    auto e = pContour->polygon().front();
                    if( e->data().flags.test( EdgeMask::ePartitionFloor ) )
                    {
                        floor( *pContour );
                    }
                    else if( e->data().flags.test( EdgeMask::eExtrusionOne ) )
                    {
                        floorEx1( *pContour, theFloor );
                    }
                    else if( e->data().flags.test( EdgeMask::eExtrusionTwo ) )
                    {
                        floorEx2( *pContour, theFloor );
                    }
                    else if( e->data().flags.test( EdgeMask::eExtrusionThree ) )
                    {
                        floorEx3( *pContour, theFloor );
                    }
                    else if( e->data().flags.test( EdgeMask::eExtrusionFour ) )
                    {
                        floorEx4( *pContour, theFloor );
                    }
                    else
                    {
                        INVARIANT( false, "Unexpected edge type in polygon" );
                    }
                }
                else
                {
                    auto e = pContour->polygon().front();
                    if( e->data().flags.test( EdgeMask::ePartitionFloor ) )
                    {
                        // ignore
                    }
                    else if( e->data().flags.test( EdgeMask::eExtrusionOne ) )
                    {
                        floor( *pContour );
                    }
                    else if( e->data().flags.test( EdgeMask::eExtrusionTwo ) )
                    {
                        floorEx1( *pContour, theFloor );
                    }
                    else if( e->data().flags.test( EdgeMask::eExtrusionThree ) )
                    {
                        floorEx2( *pContour, theFloor );
                    }
                    else if( e->data().flags.test( EdgeMask::eExtrusionFour ) )
                    {
                        floorEx3( *pContour, theFloor );
                    }
                    else
                    {
                        INVARIANT( false, "Unexpected edge type in polygon" );
                    }
                }
            }
        }
    };

    auto rootNodes = getPolygonNodes(
        m_arr,

        // OuterEdgePredicate
        []( Arrangement::Halfedge_const_handle edge )
        {
            //
            return !edge->data().flags.test( EdgeMask::eDoorStep )
                   && ( edge->data().flags.test( EdgeMask::ePerimeter )
                        || edge->data().flags.test( EdgeMask::ePartitionFloor )

                        || edge->data().flags.test( EdgeMask::eExtrusionOne )
                        || edge->data().flags.test( EdgeMask::eExtrusionTwo )
                        || edge->data().flags.test( EdgeMask::eExtrusionThree )
                        || edge->data().flags.test( EdgeMask::eExtrusionFour )

                        //
                   );
        },
        // InnerEdgePredicate
        []( Arrangement::Halfedge_const_handle edge )
        {
            //
            return // edge->data().flags.test( EdgeMask::eDoorStep ) ||
                edge->data().flags.test( EdgeMask::ePartitionBoundary )
                || edge->data().flags.test( EdgeMask::eExtrusionOneBoundary )
                || edge->data().flags.test( EdgeMask::eExtrusionTwoBoundary )
                || edge->data().flags.test( EdgeMask::eExtrusionThreeBoundary )
                || edge->data().flags.test( EdgeMask::eExtrusionFourBoundary );
        } );

    Visitor visitor{ floors };
    for( auto pNode : rootNodes )
    {
        visitor.floor( *pNode );
    }

    return floors;
}

Analysis::Boundary::Vector Analysis::getBoundaries()
{
    using Edge = Arrangement::Halfedge_const_handle;

    HalfEdgeSet boundaryEdges;
    getEdges(
        m_arr, boundaryEdges, []( Edge edge ) { return edge->data().flags.test( EdgeMask::ePartitionBoundary ); } );

    HalfEdgeVectorVector boundaryPolygons;
    getPolygons( boundaryEdges, boundaryPolygons );

    HalfEdgeSet boundarySegmentEdges;
    getEdges( m_arr, boundarySegmentEdges,
              []( Edge edge ) { return edge->data().flags.test( EdgeMask::ePartitionBoundarySegment ); } );

    HalfEdgeVectorVector boundarySegmentPolygons;
    getPolygons( boundarySegmentEdges, boundarySegmentPolygons );

    // Partition::PtrVector        m_floors, m_boundaries;
    // PartitionSegment::PtrVector m_boundarySegments;

    using SegmentEdgeMap = std::map< PartitionSegment*, HalfEdgeVector >;
    SegmentEdgeMap segmentMap;

    for( const auto& segmentPoly : boundarySegmentPolygons )
    {
        INVARIANT( !segmentPoly.empty(), "Empty boundary segment polygon" );
        const auto& firstEdge = segmentPoly.front();
        INVARIANT( firstEdge->data().pPartitionSegment, "Boundary segment edge with no partition segment" );
        segmentMap.insert( { segmentPoly.front()->data().pPartitionSegment, segmentPoly } );
    }

    struct Sequence
    {
        using Vector                               = std::vector< Sequence >;
        using EdgeSequence                         = std::list< Edge >;
        Partition*                     m_pBoundary = nullptr;
        Partition*                     m_pFloor    = nullptr;
        std::list< PartitionSegment* > m_segments;
        EdgeSequence                   m_edges;
        enum Plane
        {
            eHole,
            eGround,
            eMid,
            eCeiling
        } m_plane
            = eCeiling;

        bool pushBack( Edge edge )
        {
            Partition*        pBoundary         = edge->data().pPartition;
            PartitionSegment* pPartitionSegment = edge->data().pPartitionSegment;
            Partition*        pFloor            = edge->twin()->data().pPartition;
            INVARIANT( pBoundary, "Boundary edge with no boundary" );
            INVARIANT( pPartitionSegment, "Boundary edge with no boundary segment" );
            INVARIANT( pFloor, "Floor missing from boundary" );
            INVARIANT( m_pBoundary == pBoundary, "Inconsistent boundary" );

            if( pFloor == m_pFloor )
            {
                if( pPartitionSegment == m_segments.back() )
                {
                    m_edges.push_back( edge );
                    return true;
                }
                else
                {
                    // TODO
                    INVARIANT( false, "Not implemented" );
                }
            }
            else
            {
                return false;
            }
        }

        bool pushFront( Edge edge )
        {
            Partition*        pBoundary         = edge->data().pPartition;
            PartitionSegment* pPartitionSegment = edge->data().pPartitionSegment;
            Partition*        pFloor            = edge->twin()->data().pPartition;
            INVARIANT( pBoundary, "Boundary edge with no boundary" );
            INVARIANT( pPartitionSegment, "Boundary edge with no boundary segment" );
            INVARIANT( pFloor, "Floor missing from boundary" );
            INVARIANT( m_pBoundary == pBoundary, "Inconsistent boundary" );

            if( m_edges.front()->source() == edge->target() )
            {
                if( pFloor == m_pFloor )
                {
                    if( pPartitionSegment == m_segments.front() )
                    {
                        m_edges.push_front( edge );
                        return true;
                    }
                    else
                    {
                        // TODO
                        INVARIANT( false, "Not implemented" );
                    }
                }
            }
            return false;
        }
    };

    Boundary::Vector result;

    for( const auto& boundaryPolygon : boundaryPolygons )
    {
        Sequence::Vector sequences;

        Partition* pBoundary = nullptr;

        for( const auto& edge : boundaryPolygon )
        {
            INVARIANT( pBoundary == nullptr || pBoundary == edge->data().pPartition, "Inconsistent boundary" );
            pBoundary = edge->data().pPartition;
            INVARIANT( pBoundary, "Boundary edge has no partition" );
            PartitionSegment* pPartitionSegment = edge->data().pPartitionSegment;
            INVARIANT( pPartitionSegment, "Boundary edge has no partition segment" );
            INVARIANT( std::find( pBoundary->segments.begin(), pBoundary->segments.end(), pPartitionSegment )
                           != pBoundary->segments.end(),
                       "Boundary does not contain segment" );
            Partition* pFloor = edge->twin()->data().pPartition;
            INVARIANT( pFloor, "Floor missing from boundary" );

            if( !sequences.empty() && sequences.back().pushBack( edge ) )
            {
                continue;
            }
            else if( !sequences.empty() && sequences.front().pushFront( edge ) )
            {
                continue;
            }
            else
            {
                Sequence seq;
                seq.m_pBoundary = pBoundary;
                seq.m_pFloor    = pFloor;
                seq.m_segments.push_back( pPartitionSegment );
                seq.m_edges.push_back( edge );
                sequences.emplace_back( seq );
            }
        }

        // construct the boundary
        Boundary boundary;

        boundary.contour = boundaryPolygon;

        for( const auto& seq : sequences )
        {
            for( PartitionSegment* pSegment : seq.m_segments )
            {
                auto iFind = segmentMap.find( pSegment );
                INVARIANT( iFind != segmentMap.end(), "Could not find segment" );

                auto findSiteContours
                    = [ pSegment ]( const Analysis::HalfEdgeVector& boundary, Analysis::HalfEdgeSet& siteEdges )
                {
                    Analysis::FaceVector facesWithinSegment;
                    getSortedFacesInsidePolygon( boundary, facesWithinSegment );
                    for( auto f : facesWithinSegment )
                    {
                        visitEdgesOfFace( f,
                                          [ &siteEdges ]( Analysis::HalfEdge edge )
                                          {
                                              if( !siteEdges.contains( edge ) && !siteEdges.contains( edge->twin() ) )
                                              {
                                                  siteEdges.insert( edge );
                                              }
                                              return true;
                                          } );
                    }
                };

                switch( seq.m_plane )
                {
                    case Sequence::eHole:
                    {
                        Boundary::Pane pane;
                        findSiteContours( iFind->second, pane.edges );
                        boundary.panes.push_back( pane );
                        boundary.hori_holes.push_back( iFind->second );
                    }
                    break;
                    case Sequence::eGround:
                    {
                        Boundary::Pane pane;
                        findSiteContours( iFind->second, pane.edges );
                        boundary.panes.push_back( pane );
                        boundary.hori_floors.push_back( iFind->second );
                    }
                    break;
                    case Sequence::eMid:
                    {
                        Boundary::Pane pane;
                        findSiteContours( iFind->second, pane.edges );
                        boundary.panes.push_back( pane );
                        boundary.hori_mids.push_back( iFind->second );
                    }
                    break;
                    case Sequence::eCeiling:
                    {
                        boundary.hori_ceilings.push_back( iFind->second );
                    }
                    break;
                }
            }

            switch( seq.m_plane )
            {
                case Sequence::eHole:
                {
                    Boundary::WallSection wall;
                    wall.edges.assign( seq.m_edges.begin(), seq.m_edges.end() );
                    boundary.wall_hole.emplace_back( wall );
                }
                break;
                case Sequence::eGround:
                {
                    Boundary::WallSection wall;
                    wall.edges.assign( seq.m_edges.begin(), seq.m_edges.end() );
                    boundary.wall_ground.emplace_back( wall );
                }
                break;
                case Sequence::eMid:
                {
                    Boundary::WallSection wall;
                    wall.edges.assign( seq.m_edges.begin(), seq.m_edges.end() );
                    boundary.wall_lower.emplace_back( wall );
                }
                break;
                case Sequence::eCeiling:
                {
                    Boundary::WallSection wall;
                    wall.edges.assign( seq.m_edges.begin(), seq.m_edges.end() );
                    boundary.wall_upper.emplace_back( wall );
                }
                break;
            }
        }

        result.emplace_back( boundary );
    }

    return result;
}

} // namespace exact
