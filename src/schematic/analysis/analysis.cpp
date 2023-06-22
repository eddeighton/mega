
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

void Analysis::getFloorPartitions( std::map< const Analysis::Partition*, exact::Polygon_with_holes >& floors )
{
    using NodePtr    = typename PolygonNode::Ptr;
    using NodeVector = std::vector< NodePtr >;
    NodeVector nodes;
    {
        Analysis::HalfEdgeVectorVector floorPolygons;
        {
            Analysis::HalfEdgeSet outerEdges;
            getEdges( m_arr, outerEdges,
                      []( Arrangement::Halfedge_const_handle edge )
                      { return test( edge, EdgeMask::ePartitionFloor ); } );
            getPolygonsDir( outerEdges, floorPolygons, true );
        }
        for( auto& poly : floorPolygons )
        {
            nodes.push_back( std::make_shared< PolygonNode >( poly ) );
        }
    }

    auto rootNodes = getPolygonNodes( nodes );

    struct Visitor
    {
        std::map< const Analysis::Partition*, exact::Polygon_with_holes >& floors;

        void floor( const PolygonNode& node )
        {
            auto e          = node.polygon().front();
            auto pPartition = e->data().pPartition;
            INVARIANT( pPartition, "Floor edge missing partition" );
            INVARIANT( pPartition->pSite, "Floor partition missing site" );

            HalfEdgePolygonWithHoles polyWithHoles;
            polyWithHoles.outer = node.polygon();

            for( auto pContour : node.contained() )
            {
                polyWithHoles.holes.push_back( pContour->polygon() );
                for( auto pInner : pContour->contained() )
                {
                    floor( *pInner );
                }
            }
            floors.insert( { pPartition, fromHalfEdgePolygonWithHoles( polyWithHoles ) } );
        }
    } visitor{ floors };

    for( auto pNode : rootNodes )
    {
        visitor.floor( *pNode );
    }
}

void Analysis::getBoundaryPartitions( Analysis::HalfEdgeVectorVector& boundarySegments )
{
    Analysis::HalfEdgeSet edges;
    getEdges( m_arr, edges,
              []( Arrangement::Halfedge_const_handle edge )
              { return test( edge, EdgeMask::ePartitionBoundarySegment ); } );
    getPolygonsDir( edges, boundarySegments, true );
}

void Analysis::getBoundaryPartitions( std::map< const Analysis::PartitionSegment*, exact::Polygon >& boundaries )
{
    Analysis::HalfEdgeVectorVector boundarySegments;
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
              []( Arrangement::Halfedge_const_handle edge ) { return test( edge, EdgeMask::ePerimeter ); } );

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
                auto e = pContour->polygon().front();
                if( test( e, EdgeMask::ePartitionFloor ) )
                {
                    floor( *pContour );
                }
                else if( test( e, EdgeMask::ePartitionBoundary ) || test( e, EdgeMask::eExtrusionOneBoundary )
                         || test( e, EdgeMask::eExtrusionTwoBoundary ) || test( e, EdgeMask::eExtrusionThreeBoundary )
                         || test( e, EdgeMask::eExtrusionFourBoundary ) )
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
                    if( test( e, EdgeMask::ePartitionFloor ) )
                    {
                        floor( *pContour );
                    }
                    else if( test( e, EdgeMask::eExtrusionOne ) )
                    {
                        floorEx1( *pContour, theFloor );
                    }
                    else if( test( e, EdgeMask::eExtrusionTwo ) )
                    {
                        floorEx2( *pContour, theFloor );
                    }
                    else if( test( e, EdgeMask::eExtrusionThree ) )
                    {
                        floorEx3( *pContour, theFloor );
                    }
                    else if( test( e, EdgeMask::eExtrusionFour ) )
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
                    if( test( e, EdgeMask::ePartitionFloor ) )
                    {
                        // ignore
                    }
                    else if( test( e, EdgeMask::eExtrusionOne ) )
                    {
                        floor( *pContour );
                    }
                    else if( test( e, EdgeMask::eExtrusionTwo ) )
                    {
                        floorEx1( *pContour, theFloor );
                    }
                    else if( test( e, EdgeMask::eExtrusionThree ) )
                    {
                        floorEx2( *pContour, theFloor );
                    }
                    else if( test( e, EdgeMask::eExtrusionFour ) )
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
            return !test( edge, EdgeMask::eDoorStep )
                   && ( test( edge, EdgeMask::ePartitionFloor )

                        || test( edge, EdgeMask::eExtrusionOne ) || test( edge, EdgeMask::eExtrusionTwo )
                        || test( edge, EdgeMask::eExtrusionThree ) || test( edge, EdgeMask::eExtrusionFour )

                        //
                   );
        },
        // InnerEdgePredicate
        []( Arrangement::Halfedge_const_handle edge )
        {
            //
            return !test( edge, EdgeMask::ePerimeterBoundary )
                   && ( test( edge, EdgeMask::ePartitionBoundary ) || test( edge, EdgeMask::eExtrusionOneBoundary )
                        || test( edge, EdgeMask::eExtrusionTwoBoundary )
                        || test( edge, EdgeMask::eExtrusionThreeBoundary )
                        || test( edge, EdgeMask::eExtrusionFourBoundary ) );
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
    getEdges( m_arr, boundaryEdges, []( Edge edge ) { return test( edge, EdgeMask::ePartitionBoundary ); } );

    HalfEdgeVectorVector boundaryPolygons;
    getPolygons( boundaryEdges, boundaryPolygons );

    HalfEdgeSet boundarySegmentEdges;
    getEdges(
        m_arr, boundarySegmentEdges, []( Edge edge ) { return test( edge, EdgeMask::ePartitionBoundarySegment ); } );

    HalfEdgeVectorVector boundarySegmentPolygons;
    getPolygonsDir( boundarySegmentEdges, boundarySegmentPolygons, true );

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
        PartitionSegment::Plane        m_plane = PartitionSegment::eCeiling;

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
                else if( m_plane == pPartitionSegment->plane )
                {
                    m_edges.push_back( edge );
                    m_segments.push_back( pPartitionSegment );
                    return true;
                }
            }
            return false;
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
                    else if( m_plane == pPartitionSegment->plane )
                    {
                        m_edges.push_front( edge );
                        m_segments.push_front( pPartitionSegment );
                        return true;
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
                seq.m_plane     = pPartitionSegment->plane;
                seq.m_segments.push_back( pPartitionSegment );
                seq.m_edges.push_back( edge );
                sequences.emplace_back( seq );
            }
        }

        // construct the boundary
        Boundary boundary;

        boundary.contour = boundaryPolygon;

        using CutEdgeSet       = Analysis::HalfEdgeSet;
        using CutEdgeSeq       = Analysis::HalfEdgeVector;
        using CutEdgeSeqVector = std::vector< CutEdgeSeq >;
        using CutEdgeSeqMap    = std::map< PartitionSegment*, CutEdgeSeqVector >;
        struct CutEdgeSeqInfo
        {
            PartitionSegment*          pSegment;
            CutEdgeSeq::const_iterator iter, iterEnd;
        };
        using CutEdgeIterMap = std::map< Analysis::HalfEdge, CutEdgeSeqInfo >;
        CutEdgeSet     cutEdges;
        CutEdgeSeqMap  segmentCutEdges;
        CutEdgeIterMap cutEdgeIters;

        // add all segment horizontal surfaces and collect cut edges
        INVARIANT( !pBoundary->segments.empty(), "Boundary contains no segments" );
        for( PartitionSegment* pSegment : pBoundary->segments )
        {
            auto iFind = segmentMap.find( pSegment );
            INVARIANT( iFind != segmentMap.end(), "Could not find segment" );
            const Analysis::HalfEdgeVector& boundarySegmentPoly = iFind->second;

            CutEdgeSeqVector cutEdgeSequences;
            getSubSequences< Analysis::HalfEdge >( boundarySegmentPoly,
                                                   cutEdgeSequences,
                                                   [ &cutEdges ]( Analysis::HalfEdge e )
                                                   {
                                                       if( test( e, EdgeMask::eCut ) )
                                                       {
                                                           cutEdges.insert( e );
                                                           return true;
                                                       }
                                                       return false;
                                                   } );
            INVARIANT( !cutEdgeSequences.empty() || cutEdges.empty(), "No subsequences" );

            // record the cut edge subsequences
            auto ib = segmentCutEdges.insert( { pSegment, cutEdgeSequences } );
            // then record the iterator associated with each cut edge into its associated sequence
            // along with the end of the sequence - every cut half edge should occur only once
            for( CutEdgeSeqVector::const_iterator i = ib.first->second.begin(), iEnd = ib.first->second.end();
                 i != iEnd; ++i )
            {
                for( CutEdgeSeq::const_iterator j = i->begin(), jEnd = i->end(); j != jEnd; ++j )
                {
                    Analysis::HalfEdge e = *j;
                    auto               r = cutEdgeIters.insert( { e, CutEdgeSeqInfo{ pSegment, j, jEnd } } );
                    INVARIANT( r.second, "Cut half edge occured twice" );
                }
            }

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
                                          if( test( edge, EdgeMask::eSite ) )
                                          {
                                              siteEdges.insert( edge );
                                          }
                                      } );
                }
            };

            switch( pSegment->plane )
            {
                case PartitionSegment::eHole:
                {
                    Boundary::Pane pane;
                    pane.lower = PartitionSegment::eHole;
                    pane.upper = PartitionSegment::eCeiling;
                    findSiteContours( boundarySegmentPoly, pane.edges );
                    boundary.panes.push_back( pane );
                    boundary.hori_holes.push_back( boundarySegmentPoly );
                }
                break;
                case PartitionSegment::eGround:
                {
                    Boundary::Pane pane;
                    pane.lower = PartitionSegment::eGround;
                    pane.upper = PartitionSegment::eCeiling;
                    findSiteContours( boundarySegmentPoly, pane.edges );
                    boundary.panes.push_back( pane );
                    boundary.hori_floors.push_back( boundarySegmentPoly );
                }
                break;
                case PartitionSegment::eMid:
                {
                    Boundary::Pane pane;
                    pane.lower = PartitionSegment::eMid;
                    pane.upper = PartitionSegment::eCeiling;
                    findSiteContours( boundarySegmentPoly, pane.edges );
                    boundary.panes.push_back( pane );
                    boundary.hori_mids.push_back( boundarySegmentPoly );
                }
                break;
                case PartitionSegment::eCeiling:
                {
                    boundary.hori_ceilings.push_back( boundarySegmentPoly );
                }
                break;
            }
        }

        // build graph of all adjacent PartitionSegments
        // Each adjacency occurs along a sequence of cut edges and their associated twin edges
        // For each of these cut edge twin sequences construct the correct wall section given
        // the plane of edge partition segment
        {
            struct CutEdgeTwinSeqeunce
            {
                PartitionSegment *pSegmentOne, *pSegmentTwo;
                CutEdgeSeq        sequence;
            };
            std::vector< CutEdgeTwinSeqeunce > cutEdgeTwinSequences;

            for( const auto& [ pSegment, cutEdgeSequences ] : segmentCutEdges )
            {
                for( const auto& subSeq : cutEdgeSequences )
                {
                    std::optional< CutEdgeSeq::const_iterator > iterLast;
                    for( auto e : subSeq )
                    {
                        if( cutEdges.contains( e ) )
                        {
                            cutEdges.erase( e );

                            auto iFind = cutEdgeIters.find( e->twin() );
                            INVARIANT( iFind != cutEdgeIters.end(), "Failed to locate cut edge twin" );
                            const CutEdgeSeqInfo& twinCutEdgeInfo = iFind->second;

                            if( iterLast.has_value() && twinCutEdgeInfo.iter == iterLast.value() )
                            {
                                INVARIANT( !cutEdgeTwinSequences.empty(), "No twin sequence" );
                                INVARIANT( cutEdgeTwinSequences.back().pSegmentOne == pSegment, "Segment mismatch" );
                                INVARIANT( cutEdgeTwinSequences.back().pSegmentTwo == twinCutEdgeInfo.pSegment,
                                           "Twin Segment mismatch" );

                                cutEdgeTwinSequences.back().sequence.push_back( e );
                            }
                            else
                            {
                                // start new twin sequence
                                CutEdgeTwinSeqeunce twinSeq;
                                twinSeq.pSegmentOne = pSegment;
                                twinSeq.pSegmentTwo = twinCutEdgeInfo.pSegment;
                                twinSeq.sequence.push_back( e );
                                cutEdgeTwinSequences.push_back( twinSeq );
                            }

                            auto twinNext = twinCutEdgeInfo.iter + 1;
                            if( twinNext != twinCutEdgeInfo.iterEnd )
                            {
                                iterLast = twinNext;
                            }
                            else
                            {
                                iterLast.reset();
                            }
                        }
                    }
                }
            }

            INVARIANT( cutEdges.empty(), "Cut edges not empty: " << cutEdges.size() );

            // finally generate the wall sections
            for( const auto& twinSeq : cutEdgeTwinSequences )
            {
                if( twinSeq.pSegmentOne->plane != twinSeq.pSegmentTwo->plane )
                {
                    Boundary::WallSection wall;
                    if( twinSeq.pSegmentOne->plane < twinSeq.pSegmentTwo->plane )
                    {
                        wall.lower = twinSeq.pSegmentOne->plane;
                        wall.upper = twinSeq.pSegmentTwo->plane;
                        for( auto e : twinSeq.sequence )
                        {
                            wall.edges.push_back( e->twin() );
                            std::reverse( wall.edges.begin(), wall.edges.end() );
                        }
                    }
                    else
                    {
                        wall.lower = twinSeq.pSegmentTwo->plane;
                        wall.upper = twinSeq.pSegmentOne->plane;
                        for( auto e : twinSeq.sequence )
                            wall.edges.push_back( e );
                    }
                    boundary.walls.emplace_back( wall );
                }
            }
        }

        INVARIANT( !sequences.empty(), "No sequences for boundary" );

        for( const auto& seq : sequences )
        {
            switch( seq.m_plane )
            {
                case PartitionSegment::eHole:
                {
                    Boundary::WallSection wall;
                    wall.lower = PartitionSegment::eHole;
                    wall.upper = PartitionSegment::eGround;
                    for( auto e : seq.m_edges )
                    {
                        wall.edges.push_back( e->twin() );
                        std::reverse( wall.edges.begin(), wall.edges.end() );
                    }
                    boundary.walls.emplace_back( wall );
                }
                break;
                case PartitionSegment::eGround:
                    break;
                case PartitionSegment::eMid:
                {
                    Boundary::WallSection wall;
                    wall.lower = PartitionSegment::eGround;
                    wall.upper = PartitionSegment::eMid;
                    for( auto e : seq.m_edges )
                        wall.edges.push_back( e );
                    boundary.walls.emplace_back( wall );
                }
                break;
                case PartitionSegment::eCeiling:
                {
                    Boundary::WallSection wall;
                    wall.lower = PartitionSegment::eGround;
                    wall.upper = PartitionSegment::eCeiling;
                    for( auto e : seq.m_edges )
                        wall.edges.push_back( e );
                    boundary.walls.emplace_back( wall );
                }
                break;
            }
        }

        result.emplace_back( boundary );
    }

    return result;
}

} // namespace exact
