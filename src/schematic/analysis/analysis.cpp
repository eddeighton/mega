
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
#include "schematic/schematic.hpp"
#include "schematic/analysis/invariant.hpp"

#include <vector>

namespace exact
{
Analysis::Analysis( schematic::Schematic::Ptr pSchematic )
    : m_pSchematic( pSchematic )
    , m_observer( m_arr )
{
    auto sites = pSchematic->getSites();
    INVARIANT( sites.size() < 2, "Schematic contains more than one root site" );
    INVARIANT( sites.size() == 1, "Schematic missing root site" );

    schematic::Space::Ptr pRootSpace = boost::dynamic_pointer_cast< schematic::Space >( sites.front() );
    INVARIANT( pRootSpace, "Root site is not a space" );
    renderContour( pRootSpace->getAbsoluteExactTransform(), pRootSpace->getInteriorPolygon(), EdgeMask::ePerimeter,
                   EdgeMask::ePerimeterBoundary, pRootSpace, {} );

    recurse( pRootSpace );
    connect( pRootSpace );
    recursePost( pRootSpace );
    partition();
}

void Analysis::renderContour( const exact::Transform& transform, const exact::Polygon& polyOriginal,
                              EdgeMask::Type innerMask, EdgeMask::Type outerMask, schematic::Site::PtrCst pInnerSite,
                              schematic::Site::PtrCst pOuterSite )
{
    // transform to absolute coordinates
    exact::Polygon exactPoly;
    for( const exact::Point& pt : polyOriginal )
    {
        exactPoly.push_back( transform( pt ) );
    }

    if( !exactPoly.is_counterclockwise_oriented() )
    {
        std::reverse( exactPoly.begin(), exactPoly.end() );
    }

    // render the line segments
    for( auto i = exactPoly.begin(), iNext = exactPoly.begin(), iEnd = exactPoly.end(); i != iEnd; ++i )
    {
        ++iNext;
        if( iNext == iEnd )
            iNext = exactPoly.begin();

        Arrangement::Curve_handle firstCurve = CGAL::insert( m_arr, exact::Curve( *i, *iNext ) );
        for( auto h = m_arr.induced_edges_begin( firstCurve ); h != m_arr.induced_edges_end( firstCurve ); ++h )
        {
            Arrangement::Halfedge_handle edge = *h;

            const bool bSameDir = firstCurve->is_directed_right()
                                      ? ( edge->direction() == CGAL::Arr_halfedge_direction::ARR_LEFT_TO_RIGHT )
                                      : ( edge->direction() == CGAL::Arr_halfedge_direction::ARR_RIGHT_TO_LEFT );

            if( bSameDir )
            {
                if( pInnerSite )
                    edge->data().appendSiteUnique( pInnerSite );
                if( pOuterSite )
                    edge->twin()->data().appendSiteUnique( pOuterSite );
                classify( edge, innerMask );
                classify( edge->twin(), outerMask );
            }
            else
            {
                if( pInnerSite )
                    edge->twin()->data().appendSiteUnique( pInnerSite );
                if( pOuterSite )
                    edge->data().appendSiteUnique( pOuterSite );
                classify( edge->twin(), innerMask );
                classify( edge, outerMask );
            }
        }
    }
}

void Analysis::recurse( schematic::Site::Ptr pSite )
{
    if( schematic::Space::Ptr pSpace = boost::dynamic_pointer_cast< schematic::Space >( pSite ) )
    {
        const exact::Transform transform = pSpace->getAbsoluteExactTransform();

        // render the interior polygon
        {
            renderContour(
                transform, pSpace->getInteriorPolygon(), EdgeMask::eInterior, EdgeMask::eInteriorBoundary, pSpace, {} );
        }

        // render the exterior polygons
        {
            for( const exact::Polygon& p : pSpace->getInnerExteriorUnions() )
            {
                renderContour( transform, p, EdgeMask::eExterior, EdgeMask::eExteriorBoundary, {}, pSpace );
            }
        }
    }

    for( schematic::Site::Ptr pNestedSite : pSite->getSites() )
    {
        recurse( pNestedSite );
    }
}

void Analysis::recursePost( schematic::Site::Ptr pSite )
{
    if( schematic::Space::Ptr pSpace = boost::dynamic_pointer_cast< schematic::Space >( pSite ) )
    {
        const exact::Transform transform = pSpace->getAbsoluteExactTransform();

        // render the site polygon
        renderContour( transform, pSpace->getSitePolygon(), EdgeMask::eSite, EdgeMask::eSite, pSpace, {} );
    }

    for( schematic::Site::Ptr pNestedSite : pSite->getSites() )
    {
        recursePost( pNestedSite );
    }
}

void Analysis::constructConnectionEdges( schematic::Connection::Ptr   pConnection,
                                         Arrangement::Halfedge_handle firstBisectorEdge,
                                         Arrangement::Halfedge_handle secondBisectorEdge )
{
    Arrangement::Vertex_handle vFirstStart  = firstBisectorEdge->source();
    Arrangement::Vertex_handle vFirstEnd    = firstBisectorEdge->target();
    Arrangement::Vertex_handle vSecondStart = secondBisectorEdge->source();
    Arrangement::Vertex_handle vSecondEnd   = secondBisectorEdge->target();

    const Point ptFirstStart  = vFirstStart->point();
    const Point ptFirstEnd    = vFirstEnd->point();
    const Point ptSecondStart = vSecondStart->point();
    const Point ptSecondEnd   = vSecondEnd->point();
    const Point ptFirstMid    = ptFirstStart + ( ptFirstEnd - ptFirstStart ) / 2.0;
    const Point ptSecondMid   = ptSecondStart + ( ptSecondEnd - ptSecondStart ) / 2.0;

    Arrangement::Halfedge_handle hFirstStartToMid = m_arr.split_edge( firstBisectorEdge, ptFirstMid );
    Arrangement::Vertex_handle   vFirstMid        = hFirstStartToMid->target();

    Arrangement::Halfedge_handle hSecondStartToMid = m_arr.split_edge( secondBisectorEdge, ptSecondMid );
    Arrangement::Vertex_handle   vSecondMid        = hSecondStartToMid->target();

    // create edge between mid points
    Arrangement::Halfedge_handle m_hDoorStep;
    {
        const ::exact::Curve segDoorStep( ptFirstMid, ptSecondMid );
        m_hDoorStep = m_arr.insert_at_vertices( segDoorStep, vFirstMid, vSecondMid );
        classify( m_hDoorStep, EdgeMask::eDoorStep );
        classify( m_hDoorStep->twin(), EdgeMask::eDoorStep );
    }

    {
        bool                                           bFound = false;
        Arrangement::Halfedge_around_vertex_circulator first, iter;
        first = iter = vFirstStart->incident_halfedges();
        do
        {
            if( ( iter->source() == vSecondStart ) || ( iter->source() == vSecondEnd ) )
            {
                classify( iter, EdgeMask::eConnectionBreak );
                classify( iter->twin(), EdgeMask::eConnectionBreak );
                bFound = true;
                break;
            }
            ++iter;
        } while( iter != first );
        if( !bFound )
        {
            throw std::runtime_error( "Failed in constructConnectionEdges" );
        }
        INVARIANT( bFound, "Connection failed" );
    }
    {
        bool                                           bFound = false;
        Arrangement::Halfedge_around_vertex_circulator first, iter;
        first = iter = vFirstEnd->incident_halfedges();
        do
        {
            if( ( iter->source() == vSecondStart ) || ( iter->source() == vSecondEnd ) )
            {
                classify( iter, EdgeMask::eConnectionBreak );
                classify( iter->twin(), EdgeMask::eConnectionBreak );
                bFound = true;
                break;
            }
            ++iter;
        } while( iter != first );
        if( !bFound )
        {
            throw std::runtime_error( "Failed in constructConnectionEdges" );
        }
        INVARIANT( bFound, "Connection failed" );
    }
}

void Analysis::connect( schematic::Site::Ptr pSite )
{
    static ::exact::InexactToExact converter;

    if( schematic::Connection::Ptr pConnection = boost::dynamic_pointer_cast< schematic::Connection >( pSite ) )
    {
        const exact::Transform transform = pConnection->getAbsoluteExactTransform();

        // attempt to find the four connection vertices
        std::vector< Arrangement::Halfedge_handle > toRemove;
        Arrangement::Halfedge_handle                firstBisectorEdge, secondBisectorEdge;
        bool                                        bFoundFirst = false, bFoundSecond = false;
        {
            const schematic::Segment& firstSeg = pConnection->getFirstSegment();
            const exact::Point        ptFirstStart( transform( converter( firstSeg[ 0 ] ) ) );
            const exact::Point        ptFirstEnd( transform( converter( firstSeg[ 1 ] ) ) );

            Curve_handle firstCurve = CGAL::insert( m_arr, Curve( ptFirstStart, ptFirstEnd ) );

            for( auto i = m_arr.induced_edges_begin( firstCurve ); i != m_arr.induced_edges_end( firstCurve ); ++i )
            {
                Arrangement::Halfedge_handle h = *i;

                if( ( h->source()->point() == ptFirstStart ) || ( h->source()->point() == ptFirstEnd )
                    || ( h->target()->point() == ptFirstStart ) || ( h->target()->point() == ptFirstEnd ) )
                {
                    toRemove.push_back( h );
                }
                else
                {
                    firstBisectorEdge = h;
                    classify( firstBisectorEdge, EdgeMask::eConnectionBisector );
                    classify( firstBisectorEdge->twin(), EdgeMask::eConnectionBisectorBoundary );
                    if( bFoundFirst )
                        throw std::runtime_error( "Failed in connect" );
                    INVARIANT( !bFoundFirst, "Connect failed" );
                    bFoundFirst = true;
                }
            }
        }

        {
            const schematic::Segment& secondSeg = pConnection->getSecondSegment();
            const exact::Point        ptSecondStart( transform( converter( secondSeg[ 1 ] ) ) );
            const exact::Point        ptSecondEnd( transform( converter( secondSeg[ 0 ] ) ) );

            Curve_handle secondCurve = CGAL::insert( m_arr, Curve( ptSecondStart, ptSecondEnd ) );

            for( auto i = m_arr.induced_edges_begin( secondCurve ); i != m_arr.induced_edges_end( secondCurve ); ++i )
            {
                Arrangement::Halfedge_handle h = *i;

                if( ( h->source()->point() == ptSecondStart ) || ( h->source()->point() == ptSecondEnd )
                    || ( h->target()->point() == ptSecondStart ) || ( h->target()->point() == ptSecondEnd ) )
                {
                    toRemove.push_back( h );
                }
                else
                {
                    secondBisectorEdge = h;
                    classify( secondBisectorEdge, EdgeMask::eConnectionBisectorBoundary );
                    classify( secondBisectorEdge->twin(), EdgeMask::eConnectionBisector );
                    if( bFoundSecond )
                        throw std::runtime_error( "Failed in connect" );
                    INVARIANT( !bFoundSecond, "Connect failed" );
                    bFoundSecond = true;
                }
            }
        }

        if( !( bFoundFirst && bFoundSecond ) )
        {
            std::ostringstream os;
            os << "Failed to construct connection: " << pConnection->Node::getName();
            throw std::runtime_error( os.str() );
        }

        INVARIANT( bFoundFirst && bFoundSecond, "Failed to construct connection: " << pConnection->Node::getName() );
        constructConnectionEdges( pConnection, firstBisectorEdge, secondBisectorEdge );

        // INVARIANT( toRemove.size() == 4, "Bad connection" );
        for( Arrangement::Halfedge_handle h : toRemove )
        {
            classify( h, EdgeMask::eConnectionEnd );
            classify( h->twin(), EdgeMask::eConnectionEnd );
        }
    }

    for( schematic::Site::Ptr pNestedSite : pSite->getSites() )
    {
        connect( pNestedSite );
    }
}

bool Analysis::areFacesAdjacent( Arrangement::Face_const_handle hFace1, Arrangement::Face_const_handle hFace2 ) const
{
    INVARIANT( !hFace1->is_unbounded(), "Invalid face passed to areFacesAdjacent" );

    Arrangement::Ccb_halfedge_const_circulator iter  = hFace1->outer_ccb();
    Arrangement::Ccb_halfedge_const_circulator start = iter;
    do
    {
        if( iter->twin()->face() == hFace2 )
        {
            return true;
        }
        ++iter;
    } while( iter != start );

    // search through all holes
    for( Arrangement::Hole_const_iterator holeIter = hFace1->holes_begin(), holeIterEnd = hFace1->holes_end();
         holeIter != holeIterEnd; ++holeIter )
    {
        Arrangement::Ccb_halfedge_const_circulator iter  = *holeIter;
        Arrangement::Ccb_halfedge_const_circulator start = iter;
        do
        {
            if( iter->twin()->face() == hFace2 )
            {
                return true;
            }
            ++iter;
        } while( iter != start );
    }

    return false;
}

void Analysis::partition()
{
    using FaceHandle    = Arrangement::Face_handle;
    using FaceHandleSet = std::set< FaceHandle >;

    FaceHandleSet floorFaces, doorStepFloorFaces, otherFloorFaces;
    FaceHandleSet boundaryFaces;
    int           iUnboundedCount = 0;
    for( auto i = m_arr.faces_begin(), iEnd = m_arr.faces_end(); i != iEnd; ++i )
    {
        Arrangement::Face_handle hFace = i;
        if( !hFace->is_unbounded() )
        {
            // if face contains a doorstep then it is ALWAYS a floor
            if( anyFaceEdge(
                    hFace, []( const HalfEdgeData& edge ) { return edge.flags.test( EdgeMask::eDoorStep ); } ) )
            {
                doorStepFloorFaces.insert( hFace );
                floorFaces.insert( hFace );
            }
            // if the face contains an interior edge WHICH IS NOT also and exterior boundary edge
            // then it must be a floor
            else if( allFaceEdges( hFace,
                                   []( const HalfEdgeData& edge )
                                   {
                                       return edge.flags.test( EdgeMask::eInterior )
                                              || edge.flags.test( EdgeMask::eExteriorBoundary )
                                              || edge.flags.test( EdgeMask::eConnectionEnd );
                                   } ) )
            {
                otherFloorFaces.insert( hFace );
                floorFaces.insert( hFace );
            }
            else
            {
                boundaryFaces.insert( hFace );
            }
        }
        else
        {
            ++iUnboundedCount;
        }
    }
    INVARIANT( iUnboundedCount == 1, "Unbounded faces count not one: " << iUnboundedCount );

    // classify all floor face halfedges as ePartitionFloor
    for( auto& hFace : floorFaces )
    {
        INVARIANT( !hFace->is_unbounded(), "Unbounded floor face" );

        Arrangement::Ccb_halfedge_circulator iter  = hFace->outer_ccb();
        Arrangement::Ccb_halfedge_circulator start = iter;
        do
        {
            if( !iter->data().flags.test( EdgeMask::eConnectionBreak )
                && !iter->data().flags.test( EdgeMask::eConnectionEnd ) )
            {
                classify( iter, EdgeMask::ePartitionFloor );
            }
            ++iter;
        } while( iter != start );

        // search through all holes
        for( Arrangement::Hole_iterator holeIter = hFace->holes_begin(), holeIterEnd = hFace->holes_end();
             holeIter != holeIterEnd; ++holeIter )
        {
            Arrangement::Ccb_halfedge_circulator iter  = *holeIter;
            Arrangement::Ccb_halfedge_circulator start = iter;
            do
            {
                if( !iter->data().flags.test( EdgeMask::eConnectionBreak )
                    && !iter->data().flags.test( EdgeMask::eConnectionEnd ) )
                {
                    classify( iter, EdgeMask::ePartitionFloor );
                }
                ++iter;
            } while( iter != start );
        }
    }

    // boundary edges MUST be opposite of a floor edge
    // NOTE floor edge can be opposite of floor edge i.e. doorstep
    for( auto i = m_arr.halfedges_begin(); i != m_arr.halfedges_end(); ++i )
    {
        if( i->data().flags.test( EdgeMask::ePartitionFloor ) )
        {
            if( !i->twin()->data().flags.test( EdgeMask::ePartitionFloor ) )
            {
                classify( i->twin(), EdgeMask::ePartitionBoundary );
            }
        }
    }

    // all other edges are NOT parition edges
    for( auto i = m_arr.halfedges_begin(); i != m_arr.halfedges_end(); ++i )
    {
        if( !i->data().flags.test( EdgeMask::ePartitionFloor )
            && !i->data().flags.test( EdgeMask::ePartitionBoundary ) )
        {
            classify( i->twin(), EdgeMask::ePartitionNone );
        }
    }

    // solve the face connected components that consistutute the floor and boundary partitions
    FaceHandleSet foundDoorStepFaces;

    using FloorToDoorStepFaceMap = std::multimap< FaceHandle, FaceHandle >;
    FloorToDoorStepFaceMap floorPartitions;

    for( auto hFloorFace : otherFloorFaces )
    {
        for( auto hDoorStepFace : doorStepFloorFaces )
        {
            if( areFacesAdjacent( hFloorFace, hDoorStepFace ) )
            {
                INVARIANT(
                    !foundDoorStepFaces.contains( hDoorStepFace ), "Door step face found adjacent to two floor faces" );
                foundDoorStepFaces.insert( hDoorStepFace );
                floorPartitions.insert( { hFloorFace, hDoorStepFace } );
            }
        }
    }

    using BoundaryDisjointSets = std::list< FaceHandleSet >;
    BoundaryDisjointSets boundaries;
    for( auto hBoundaryFace : boundaryFaces )
    {
        bool bFound = false;

        FaceHandleSet newSet;
        newSet.insert( hBoundaryFace );

        std::vector< BoundaryDisjointSets::iterator > connections;
        for( auto i = boundaries.begin(), iEnd = boundaries.end(); i != iEnd; ++i )
        {
            for( auto hExisting : *i )
            {
                if( areFacesAdjacent( hBoundaryFace, hExisting ) )
                {
                    newSet.insert( i->begin(), i->end() );
                    connections.push_back( i );
                    break;
                }
            }
        }
        for( auto i : connections )
            boundaries.erase( i );
        boundaries.push_back( newSet );
    }

    // FaceHandleSet floorFaces, doorStepFloorFaces, otherFloorFaces;
    // FaceHandleSet boundaryFaces;

    /*
        HalfEdgeVectorVector boundaryPolygons;
        {
            HalfEdgeSet partitionEdges;
            getEdges( partitionEdges,
                    []( Arrangement::Halfedge_const_handle edge )
                    {
                        return edge->data().flags.test( EdgeMask::ePartitionBoundary )
                                && !edge->twin()->data().flags.test( EdgeMask::ePerimeter );
                    } );

            getPolygons( partitionEdges, boundaryPolygons );
        }
    */
    // extrapolate the partitions

    /*

            Partition::Ptr pPartition = std::make_unique< Partition >();
            iter->data().pPartition = pPartition.get();

            // INVARIANT( !iter->data().sites.empty(), "No site for floor edge" );
            if( !iter->data().sites.empty() )
            {
                auto p = iter->data().sites.back();
                INVARIANT(
                    !pPartition->pSite || ( pPartition->pSite == p ), "Differing site detected for partition" );
                if( !pPartition->pSite )
                    pPartition->pSite = p;
            }
        iter->data().pPartition = pPartition.get();

        // INVARIANT( !iter->data().sites.empty(), "No site for floor edge" );
        if( !iter->data().sites.empty() )
        {
            auto p = iter->data().sites.back();
            INVARIANT(
                !pPartition->pSite || ( pPartition->pSite == p ), "Differing site detected for partition" );
            if( !pPartition->pSite )
                pPartition->pSite = p;
            hFace->data().pPartition = pPartition.get();

            m_partitions.emplace_back( std::move( pPartition ) );

        }*/
}

void Analysis::getEdges( std::vector< std::pair< schematic::Segment, EdgeMask::Set > >& edges )
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
    getEdges( perimeterEdges, []( Arrangement::Halfedge_const_handle edge )
              { return edge->data().flags.test( EdgeMask::ePerimeter ); } );

    HalfEdgeVectorVector polygons;
    getPolygons( perimeterEdges, polygons );

    INVARIANT( polygons.size() == 1, "Did not find single perimeter polygon" );
    polygon = polygons.front();
}

void Analysis::getBoundaryPolygons( HalfEdgeVectorVector& polygons ) const
{
    HalfEdgeSet partitionEdges;
    getEdges( partitionEdges,
              []( Arrangement::Halfedge_const_handle edge )
              {
                  return edge->data().flags.test( EdgeMask::ePartitionBoundary )
                         && !edge->twin()->data().flags.test( EdgeMask::ePerimeter );
              } );

    getPolygons( partitionEdges, polygons );
}

} // namespace exact
