
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
    cut( pRootSpace );
}

void Analysis::renderCurve( const exact::Curve& curve, EdgeMask::Type mask )
{
    Arrangement::Curve_handle firstCurve = CGAL::insert( m_arr, curve );
    for( auto h = m_arr.induced_edges_begin( firstCurve ); h != m_arr.induced_edges_end( firstCurve ); ++h )
    {
        Arrangement::Halfedge_handle edge = *h;

        const bool bSameDir = firstCurve->is_directed_right()
                                  ? ( edge->direction() == CGAL::Arr_halfedge_direction::ARR_LEFT_TO_RIGHT )
                                  : ( edge->direction() == CGAL::Arr_halfedge_direction::ARR_RIGHT_TO_LEFT );

        if( bSameDir )
        {
            classify( edge, mask );
        }
        else
        {
            classify( edge->twin(), mask );
        }
    }
}

void Analysis::renderCurve( const exact::Curve& curve, EdgeMask::Type innerMask, EdgeMask::Type outerMask,
                            schematic::Site::PtrCst pInnerSite, schematic::Site::PtrCst pOuterSite )
{
    Arrangement::Curve_handle firstCurve = CGAL::insert( m_arr, curve );
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
            classify( edge, innerMask );

            if( pOuterSite )
                edge->twin()->data().appendSiteUnique( pOuterSite );
            classify( edge->twin(), outerMask );
        }
        else
        {
            if( pOuterSite )
                edge->data().appendSiteUnique( pOuterSite );
            classify( edge, outerMask );

            if( pInnerSite )
                edge->twin()->data().appendSiteUnique( pInnerSite );
            classify( edge->twin(), innerMask );
        }
    }
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
        renderCurve( exact::Curve( *i, *iNext ), innerMask, outerMask, pInnerSite, pOuterSite );
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
                renderContour( transform, p, EdgeMask::eExteriorBoundary, EdgeMask::eExterior, {}, pSpace );
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

        const schematic::Segment& firstSeg  = pConnection->getFirstSegment();
        const schematic::Segment& secondSeg = pConnection->getSecondSegment();
        const exact::Point        ptFirstStart( transform( converter( firstSeg[ 0 ] ) ) );
        const exact::Point        ptFirstEnd( transform( converter( firstSeg[ 1 ] ) ) );
        const exact::Point        ptSecondStart( transform( converter( secondSeg[ 1 ] ) ) );
        const exact::Point        ptSecondEnd( transform( converter( secondSeg[ 0 ] ) ) );
        const exact::Direction    edgeDir( ptFirstStart - ptSecondEnd );

        // attempt to find the four connection vertices
        std::vector< Arrangement::Halfedge_handle > toRemove;
        Arrangement::Halfedge_handle                firstBisectorEdge, secondBisectorEdge;
        bool                                        bFoundFirst = false, bFoundSecond = false;
        {
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

                    const exact::Direction halfEdgeDir( firstBisectorEdge->target()->point()
                                                        - firstBisectorEdge->source()->point() );
                    const exact::Direction halfEdgeOrthoDir
                        = halfEdgeDir.perpendicular( CGAL::Orientation::COUNTERCLOCKWISE );

                    if( halfEdgeOrthoDir == edgeDir )
                    {
                        classify( firstBisectorEdge->twin(), EdgeMask::eConnectionBisector );
                        classify( firstBisectorEdge, EdgeMask::eConnectionBisectorBoundary );
                    }
                    else
                    {
                        classify( firstBisectorEdge, EdgeMask::eConnectionBisector );
                        classify( firstBisectorEdge->twin(), EdgeMask::eConnectionBisectorBoundary );
                    }

                    if( bFoundFirst )
                        throw std::runtime_error( "Failed in connect" );
                    INVARIANT( !bFoundFirst, "Connect failed" );
                    bFoundFirst = true;
                }
            }
        }

        {
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

                    const exact::Direction halfEdgeDir( secondBisectorEdge->target()->point()
                                                        - secondBisectorEdge->source()->point() );
                    const exact::Direction halfEdgeOrthoDir
                        = halfEdgeDir.perpendicular( CGAL::Orientation::COUNTERCLOCKWISE );

                    if( halfEdgeOrthoDir == edgeDir )
                    {
                        classify( secondBisectorEdge, EdgeMask::eConnectionBisector );
                        classify( secondBisectorEdge->twin(), EdgeMask::eConnectionBisectorBoundary );
                    }
                    else
                    {
                        classify( secondBisectorEdge->twin(), EdgeMask::eConnectionBisector );
                        classify( secondBisectorEdge, EdgeMask::eConnectionBisectorBoundary );
                    }

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
            // classify( h, EdgeMask::eConnectionEnd );
            // classify( h->twin(), EdgeMask::eConnectionEnd );
            CGAL::remove_edge( m_arr, h );
        }
    }

    for( schematic::Site::Ptr pNestedSite : pSite->getSites() )
    {
        connect( pNestedSite );
    }
}

void Analysis::cut( schematic::Site::Ptr pSite )
{
    static ::exact::InexactToExact converter;

    if( schematic::Cut::Ptr pCut = boost::dynamic_pointer_cast< schematic::Cut >( pSite ) )
    {
        const exact::Transform transform = pCut->getAbsoluteExactTransform();
        const exact::Point     ptStart( transform( converter( pCut->getSegment()[ 0 ] ) ) );
        const exact::Point     ptEnd( transform( converter( pCut->getSegment()[ 1 ] ) ) );

        Curve_handle curve = CGAL::insert( m_arr, Curve( ptStart, ptEnd ) );

        std::vector< Arrangement::Halfedge_handle > toRemove;
        for( auto i = m_arr.induced_edges_begin( curve ); i != m_arr.induced_edges_end( curve ); ++i )
        {
            Arrangement::Halfedge_handle h = *i;

            // cut must be inside boundary face

            // if( h->data().flags.test( EdgeMask::eInterior

            /*if( ( h->source()->point() == ptStart ) || ( h->source()->point() == ptEnd )
                || ( h->target()->point() == ptStart ) || ( h->target()->point() == ptEnd ) )
            {
                toRemove.push_back( h );
            }
            else*/
            {
                classify( h, EdgeMask::eCut );
                classify( h->twin(), EdgeMask::eCut );
            }
        }

        for( Arrangement::Halfedge_handle h : toRemove )
        {
            CGAL::remove_edge( m_arr, h );
        }
    }

    for( schematic::Site::Ptr pNestedSite : pSite->getSites() )
    {
        cut( pNestedSite );
    }
}

void Analysis::partition()
{
    std::vector< Arrangement::Vertex_handle >          doorStepVertices;
    std::set< Analysis::Arrangement::Halfedge_handle > doorSteps;
    {
        getEdges( m_arr, doorSteps,
                  []( Arrangement::Halfedge_handle edge ) { return edge->data().flags.test( EdgeMask::eDoorStep ); } );
        for( auto d : doorSteps )
        {
            doorStepVertices.push_back( d->source() );
        }
    }

    {
        using HalfEdgeSet = std::set< Analysis::Arrangement::Halfedge_handle >;
        HalfEdgeSet floorEdges;
        getEdges( m_arr, floorEdges,
                  []( Arrangement::Halfedge_handle edge )
                  {
                      auto& flags = edge->data().flags;
                      return ( flags.test( EdgeMask::eInterior ) || flags.test( EdgeMask::eExterior )
                               || flags.test( EdgeMask::eConnectionBisector ) || flags.test( EdgeMask::eDoorStep ) )

                             && !flags.test( EdgeMask::eConnectionBreak );
                  } );

        std::vector< std::vector< Arrangement::Halfedge_handle > > floorPolygons;
        searchPolygons( doorStepVertices, floorEdges, true, floorPolygons );

        for( auto floorBoundary : floorPolygons )
        {
            Partition::Ptr                      pPartition = std::make_unique< Partition >();
            std::set< schematic::Site::PtrCst > sites;

            // classify the outer floor boundary that has been reachable from the doorstep
            HalfEdgeSet floorDoorSteps;
            for( auto e : floorBoundary )
            {
                if( e->data().flags.test( EdgeMask::eInterior ) || e->data().flags.test( EdgeMask::eExterior ) )
                {
                    INVARIANT( !e->data().sites.empty(), "Interior or exterior edge missing site" );
                    sites.insert( e->data().sites.back() );
                }
                e->data().pPartition = pPartition.get();
                classify( e, EdgeMask::ePartitionFloor );

                if( e->data().flags.test( EdgeMask::eDoorStep ) )
                {
                    floorDoorSteps.insert( e );
                }
            }

            // now search WITHIN the door steps of the floor boundary
            for( auto e : floorDoorSteps )
            {
                std::set< Analysis::Arrangement::Halfedge_handle > holeBoundaries;
                // first locate all reachable faces from the door step that are contained in the floor boundary
                // the algorithm returns the set of halfedges in holeBoundaries that
                // are reachable through the faces but are NOT in floorBoundary

                // NOTE: MUST pass ALL floorEdges here not just the floorBoundary
                locateHoleBoundariesFromDoorStep( e, floorEdges, floorBoundary, holeBoundaries );

                std::vector< std::vector< Arrangement::Halfedge_handle > > floorHoles;
                getPolygons( holeBoundaries, floorHoles );

                for( auto innerWallBoundary : floorHoles )
                {
                    for( auto innerWallEdge : innerWallBoundary )
                    {
                        innerWallEdge->data().pPartition = pPartition.get();
                        classify( innerWallEdge, EdgeMask::ePartitionFloor );
                        if( !innerWallEdge->data().flags.test( EdgeMask::eDoorStep ) )
                        {
                            classify( innerWallEdge->twin(), EdgeMask::ePartitionBoundary );
                        }
                    }
                }
            }

            INVARIANT( sites.size() != 0, "Floor has no sites" );
            INVARIANT( sites.size() == 1, "Floor has multiple sites" );
            pPartition->pSite = *sites.begin();
            m_floors.emplace_back( std::move( pPartition ) );
        }
    }

    // use similar algorithm to determine the boundary polygons which CANNOT have holes
    {
        std::set< Analysis::Arrangement::Halfedge_handle > boundaryEdges;
        getEdges( m_arr, boundaryEdges,
                  []( Arrangement::Halfedge_handle edge )
                  {
                      auto& flags = edge->data().flags;
                      return ( flags.test( EdgeMask::eInteriorBoundary ) || flags.test( EdgeMask::eExteriorBoundary )
                               || flags.test( EdgeMask::eConnectionBisectorBoundary ) )

                             && !flags.test( EdgeMask::eConnectionBreak );
                  } );

        std::vector< std::vector< Arrangement::Halfedge_handle > > boundaryPolygons;
        searchPolygons( doorStepVertices, boundaryEdges, false, boundaryPolygons );

        for( auto b : boundaryPolygons )
        {
            Partition::Ptr pPartition = std::make_unique< Partition >();
            for( auto e : b )
            {
                classify( e, EdgeMask::ePartitionBoundary );
                e->data().pPartition = pPartition.get();
            }
            m_boundaries.emplace_back( std::move( pPartition ) );
        }
    }

    // now determine all boundary segments
    {
        // Get all boundary edges COMBINED with the cut edges
        // NOTE that cut edges are on both sides of the cut curve
        std::set< Analysis::Arrangement::Halfedge_handle > boundaryEdges;
        getEdges( m_arr, boundaryEdges,
                  []( Arrangement::Halfedge_handle edge )
                  { return edge->data().flags.test( EdgeMask::ePartitionBoundary ); } );

        std::set< Analysis::Arrangement::Halfedge_handle > cutEdges;
        getEdges( m_arr, cutEdges,
                  []( Arrangement::Halfedge_handle edge ) { return edge->data().flags.test( EdgeMask::eCut ); } );

        // just add ALL vertices from the boundary as start vertices
        std::vector< Arrangement::Vertex_handle > boundarySegmentStartVertices;
        {
            for( auto e : boundaryEdges )
            {
                boundarySegmentStartVertices.push_back( e->source() );
            }
        }

        std::set< Analysis::Arrangement::Halfedge_handle > boundaryAndCutEdges = boundaryEdges;
        boundaryAndCutEdges.insert( cutEdges.begin(), cutEdges.end() );

        std::vector< std::vector< Arrangement::Halfedge_handle > > boundarySegmentPolygons;
        searchPolygons( boundarySegmentStartVertices, boundaryAndCutEdges, true, boundarySegmentPolygons );

        for( auto b : boundarySegmentPolygons )
        {
            // only admit boundary segments that have atleast three edges AND
            // ALWAYS contain atleast one partition boundary edge
            if( b.size() > 2 )
            {
                bool bContainsBoundaryEdge = false;
                for( auto e : b )
                {
                    if( e->data().flags.test( EdgeMask::ePartitionBoundary ) )
                    {
                        bContainsBoundaryEdge = true;
                        break;
                    }
                }
                if( bContainsBoundaryEdge )
                {
                    PartitionSegment::Ptr pPartitionSegment = std::make_unique< PartitionSegment >();
                    for( auto e : b )
                    {
                        classify( e, EdgeMask::ePartitionBoundarySegment );
                        e->data().pPartitionSegment         = pPartitionSegment.get();
                        e->face()->data().pPartitionSegment = pPartitionSegment.get();
                    }
                    m_boundarySegments.emplace_back( std::move( pPartitionSegment ) );
                }
            }
        }
    }
}

void Analysis::skeleton()
{
    HalfEdgePolygonWithHoles::Vector floors;

    getPolyonsWithHoles(
        m_arr,
        // OuterEdgePredicate
        []( Arrangement::Halfedge_const_handle edge ) { return edge->data().flags.test( EdgeMask::ePerimeter ); },
        // InnerEdgePredicate
        []( Arrangement::Halfedge_const_handle edge )
        {
            return edge->data().flags.test( EdgeMask::ePartitionBoundary )
                   && !edge->data().flags.test( EdgeMask::ePerimeterBoundary );
        },
        floors );

    INVARIANT( floors.size() == 1, "Failed to locate single floor" );
    const exact::Polygon_with_holes polygonWithHoles = fromHalfEdgePolygonWithHoles( floors.front() );

    using StraightSkeleton    = CGAL::Straight_skeleton_2< exact::Kernel >;
    using StraightSkeletonPtr = boost::shared_ptr< StraightSkeleton >;

    // for( auto& h : polygonWithHoles.holes() )
    //     std::reverse( h.begin(), h.end() );

    StraightSkeletonPtr pStraightSkeleton = CGAL::create_interior_straight_skeleton_2(
        polygonWithHoles.outer_boundary().begin(), polygonWithHoles.outer_boundary().end(),
        polygonWithHoles.holes_begin(), polygonWithHoles.holes_end(), exact::Kernel() );

    for( auto i = pStraightSkeleton->halfedges_begin(), iEnd = pStraightSkeleton->halfedges_end(); i != iEnd; ++i )
    {
        StraightSkeleton::Halfedge_handle h = i;
        if( h->is_inner_bisector() )
        {
            renderCurve(
                Curve( h->vertex()->point(), h->next()->vertex()->point() ), EdgeMask::eSkeletonInnerBisector );
        }
        else if( h->is_bisector() )
        {
            renderCurve( Curve( h->vertex()->point(), h->next()->vertex()->point() ), EdgeMask::eSkeletonBisector );
        }
    }

    using ExtrusionSpec = std::tuple< double, EdgeMask::Type, EdgeMask::Type >;
    static constexpr std::array< ExtrusionSpec, 4 > extrusions
        = { ExtrusionSpec{ 1.0, EdgeMask::eExtrusionOne, EdgeMask::eExtrusionOneBoundary },
            ExtrusionSpec{ 2.0, EdgeMask::eExtrusionTwo, EdgeMask::eExtrusionTwoBoundary },
            ExtrusionSpec{ 3.0, EdgeMask::eExtrusionThree, EdgeMask::eExtrusionThreeBoundary },
            ExtrusionSpec{ 4.0, EdgeMask::eExtrusionFour, EdgeMask::eExtrusionFourBoundary } };
    for( const auto& [ fOffset, mask, maskBoundary ] : extrusions )
    {
        auto result = CGAL::create_offset_polygons_2( fOffset, *pStraightSkeleton );
        for( const auto poly : result )
        {
            auto i = poly->begin(), iNext = poly->begin(), iEnd = poly->end();
            for( ; i != iEnd; ++i )
            {
                ++iNext;
                if( iNext == iEnd )
                    iNext = poly->begin();
                renderCurve(
                    Curve( Point( i->x(), i->y() ), Point( iNext->x(), iNext->y() ) ), mask, maskBoundary, {}, {} );
            }
        }
    }
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
                   && ( 
                            edge->data().flags.test( EdgeMask::ePerimeter )
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
            return edge->data().flags.test( EdgeMask::ePartitionBoundary )
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
    Boundary::Vector boundaries;

    return boundaries;
}

} // namespace exact
