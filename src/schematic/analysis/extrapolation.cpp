
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

#include "schematic/analysis/analysis.hpp"
#include "schematic/analysis/polygon_with_holes.hpp"

#include "algorithms.hpp"
#include "polygon_tree.hpp"

namespace exact
{

Analysis::Room::Vector Analysis::getRooms()
{
    enum NodeType
    {
        eNodeLaneSpace,
        eNodeLaneLining,
        eNodePavementSpace,
        eNodePavementLining,
        eNodeRoad,
        TOTAL_NODE_TYPES
    };
    using Node       = exact::PolygonNodeT< HalfEdgeCst, FaceCst, NodeType >;
    using NodePtr    = typename Node::Ptr;
    using NodeVector = std::vector< NodePtr >;
    NodeVector nodes;
    {
        {
            HalfEdgeCstVectorVector polygons;
            HalfEdgeCstSet          edges;
            getEdges( m_arr, edges, []( HalfEdgeCst edge ) { return test( edge, EdgeMask::eLaneSpace ); } );
            getPolygonsDir( edges, polygons, true );
            for( auto& poly : polygons )
            {
                nodes.push_back( std::make_shared< Node >( eNodeLaneSpace, poly ) );
            }
        }
        {
            HalfEdgeCstVectorVector polygons;
            HalfEdgeCstSet          edges;
            getEdges( m_arr, edges, []( HalfEdgeCst edge ) { return test( edge, EdgeMask::eLaneLining ); } );
            getPolygonsDir( edges, polygons, true );
            for( auto& poly : polygons )
            {
                nodes.push_back( std::make_shared< Node >( eNodeLaneLining, poly ) );
            }
        }
        {
            HalfEdgeCstVectorVector polygons;
            HalfEdgeCstSet          edges;
            getEdges( m_arr, edges, []( HalfEdgeCst edge ) { return test( edge, EdgeMask::ePavementSpace ); } );
            getPolygonsDir( edges, polygons, true );
            for( auto& poly : polygons )
            {
                nodes.push_back( std::make_shared< Node >( eNodePavementSpace, poly ) );
            }
        }
        {
            HalfEdgeCstVectorVector polygons;
            HalfEdgeCstSet          edges;
            getEdges( m_arr, edges, []( HalfEdgeCst edge ) { return test( edge, EdgeMask::ePavementLining ); } );
            getPolygonsDir( edges, polygons, true );
            for( auto& poly : polygons )
            {
                nodes.push_back( std::make_shared< Node >( eNodePavementLining, poly ) );
            }
        }
        {
            HalfEdgeCstVectorVector polygons;
            HalfEdgeCstSet          edges;
            getEdges( m_arr, edges, []( HalfEdgeCst edge ) { return test( edge, EdgeMask::eRoad ); } );
            getPolygonsDir( edges, polygons, true );
            for( auto& poly : polygons )
            {
                nodes.push_back( std::make_shared< Node >( eNodeRoad, poly ) );
            }
        }
    }

    auto rootNodes = sortPolygonNodes< HalfEdgeCst, FaceCst, NodeType >( nodes );
    INVARIANT( rootNodes.size() == 1, "getRooms did not find singular root node" );

    using PartitionRoomMap = std::map< Partition*, Room >;
    PartitionRoomMap roomMap;

    struct Visitor
    {
        PartitionRoomMap& roomMap;

        void visit( NodePtr pNode )
        {
            if( pNode->face() )
            {
                auto edge       = pNode->polygon().front();
                auto pPartition = edge->data().pPartition;
                INVARIANT( pPartition, "getRooms Floor edge missing partition" );

                Room& room      = roomMap[ pPartition ];
                room.pPartition = pPartition;

                INVARIANT( room.pPartition->pSite, "Floor partition missing site" );

                switch( pNode->type() )
                {
                    case eNodeLaneSpace:
                    {
                        HalfEdgeCstPolygonWithHoles lane;
                        lane.outer = pNode->polygon();
                        for( auto pContour : pNode->contained() )
                        {
                            lane.holes.push_back( pContour->polygon() );
                        }
                        room.lanes.push_back( lane );
                    }
                    break;
                    case eNodeLaneLining:
                    {
                        HalfEdgeCstPolygonWithHoles laneLining;
                        laneLining.outer = pNode->polygon();
                        for( auto pContour : pNode->contained() )
                        {
                            laneLining.holes.push_back( pContour->polygon() );
                        }
                        room.laneLinings.push_back( laneLining );
                    }
                    break;
                    case eNodePavementSpace:
                    {
                        HalfEdgeCstPolygonWithHoles pavement;
                        pavement.outer = pNode->polygon();
                        for( auto pContour : pNode->contained() )
                        {
                            pavement.holes.push_back( pContour->polygon() );
                        }
                        room.pavements.push_back( pavement );
                    }
                    break;
                    case eNodePavementLining:
                    {
                        HalfEdgeCstPolygonWithHoles pavementLining;
                        pavementLining.outer = pNode->polygon();
                        for( auto pContour : pNode->contained() )
                        {
                            pavementLining.holes.push_back( pContour->polygon() );
                        }
                        room.pavementLinings.push_back( pavementLining );
                    }
                    break;
                    case eNodeRoad:
                    {
                        HalfEdgeCstPolygonWithHoles road;
                        road.outer = pNode->polygon();
                        for( auto pContour : pNode->contained() )
                        {
                            road.holes.push_back( pContour->polygon() );
                        }
                        room.roads.push_back( road );
                    }
                    break;
                    case TOTAL_NODE_TYPES:
                    default:
                    {
                        INVARIANT( false, "Unknown node type" );
                    }
                    break;
                }
            }

            for( auto pContour : pNode->contained() )
            {
                visit( pContour );
            }
        }
    } visitor{ roomMap };

    for( auto r : rootNodes )
    {
        INVARIANT( r->face(), "Root node is not a face" );
        visitor.visit( r );
    }

    Analysis::Room::Vector rooms;
    for( auto& [ pPartition, room ] : roomMap )
    {
        rooms.emplace_back( room );
    }
    return rooms;
}

Analysis::Boundary::Vector Analysis::getBoundaries()
{
    using Edge = Arrangement::Halfedge_const_handle;

    HalfEdgeCstSet boundaryEdges;
    getEdges( m_arr, boundaryEdges, []( Edge edge ) { return test( edge, EdgeMask::ePartitionBoundary ); } );

    HalfEdgeCstVectorVector boundaryPolygons;
    getPolygons( boundaryEdges, boundaryPolygons );

    HalfEdgeCstSet boundarySegmentEdges;
    getEdges(
        m_arr, boundarySegmentEdges, []( Edge edge ) { return test( edge, EdgeMask::ePartitionBoundarySegment ); } );

    HalfEdgeCstVectorVector boundarySegmentPolygons;
    getPolygonsDir( boundarySegmentEdges, boundarySegmentPolygons, true );

    // Partition::PtrVector        m_floors, m_boundaries;
    // PartitionSegment::PtrVector m_boundarySegments;

    using SegmentEdgeMap = std::map< PartitionSegment*, HalfEdgeCstVector >;
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
            INVARIANT( test( edge, EdgeMask::ePartitionBoundarySegment ), "Boundary is not partition segment boundary" );
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

        using CutEdgeSet       = Analysis::HalfEdgeCstSet;
        using CutEdgeSeq       = Analysis::HalfEdgeCstVector;
        using CutEdgeSeqVector = std::vector< CutEdgeSeq >;
        using CutEdgeSeqMap    = std::map< PartitionSegment*, CutEdgeSeqVector >;
        struct CutEdgeSeqInfo
        {
            PartitionSegment*          pSegment;
            CutEdgeSeq::const_iterator iter, iterEnd;
        };
        using CutEdgeIterMap = std::map< Analysis::HalfEdgeCst, CutEdgeSeqInfo >;
        CutEdgeSet     cutEdges;
        CutEdgeSeqMap  segmentCutEdges;
        CutEdgeIterMap cutEdgeIters;

        // add all segment horizontal surfaces and collect cut edges
        INVARIANT( !pBoundary->segments.empty(), "Boundary contains no segments" );
        for( PartitionSegment* pSegment : pBoundary->segments )
        {
            auto iFind = segmentMap.find( pSegment );
            INVARIANT( iFind != segmentMap.end(), "Could not find segment" );
            const Analysis::HalfEdgeCstVector& boundarySegmentPoly = iFind->second;

            CutEdgeSeqVector cutEdgeSequences;
            getSubSequences< Analysis::HalfEdgeCst >( boundarySegmentPoly,
                                                      cutEdgeSequences,
                                                      [ &cutEdges ]( Analysis::HalfEdgeCst e )
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
                    Analysis::HalfEdgeCst e = *j;
                    auto                  r = cutEdgeIters.insert( { e, CutEdgeSeqInfo{ pSegment, j, jEnd } } );
                    INVARIANT( r.second, "Cut half edge occured twice" );
                }
            }

            auto findSiteContours
                = [ pSegment ]( const Analysis::HalfEdgeCstVector& boundary, Analysis::HalfEdgeCstSet& siteEdges )
            {
                Analysis::FaceCstVector facesWithinSegment;
                getSortedFacesInsidePolygon( boundary, facesWithinSegment );
                for( auto f : facesWithinSegment )
                {
                    visitEdgesOfFace( f,
                                      [ &siteEdges ]( Analysis::HalfEdgeCst edge )
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