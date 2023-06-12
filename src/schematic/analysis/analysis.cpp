
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
#include "schematic/cut.hpp"

#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/create_straight_skeleton_from_polygon_with_holes_2.h>
#include <CGAL/create_offset_polygons_from_polygon_with_holes_2.h>
#include <CGAL/create_offset_polygons_2.h>

#include <vector>

namespace exact
{

namespace
{

template < typename TFunctor >
inline void visit( Analysis::Arrangement::Face_const_handle hFace, TFunctor&& visitor )
{
    if( !hFace->is_unbounded() )
    {
        Analysis::Arrangement::Ccb_halfedge_const_circulator iter  = hFace->outer_ccb();
        Analysis::Arrangement::Ccb_halfedge_const_circulator start = iter;
        do
        {
            visitor( iter->data() );
            ++iter;
        } while( iter != start );
    }

    // search through all holes
    for( Analysis::Arrangement::Hole_const_iterator holeIter = hFace->holes_begin(), holeIterEnd = hFace->holes_end();
         holeIter != holeIterEnd; ++holeIter )
    {
        Analysis::Arrangement::Ccb_halfedge_const_circulator iter  = *holeIter;
        Analysis::Arrangement::Ccb_halfedge_const_circulator start = iter;
        do
        {
            visitor( iter->data() );
            ++iter;
        } while( iter != start );
    }
}

template < typename TFunctor >
inline void visit( Analysis::Arrangement::Face_handle hFace, TFunctor&& visitor )
{
    if( !hFace->is_unbounded() )
    {
        Analysis::Arrangement::Ccb_halfedge_circulator iter  = hFace->outer_ccb();
        Analysis::Arrangement::Ccb_halfedge_circulator start = iter;
        do
        {
            visitor( iter->data() );
            ++iter;
        } while( iter != start );
    }

    // search through all holes
    for( Analysis::Arrangement::Hole_iterator holeIter = hFace->holes_begin(), holeIterEnd = hFace->holes_end();
         holeIter != holeIterEnd; ++holeIter )
    {
        Analysis::Arrangement::Ccb_halfedge_circulator iter  = *holeIter;
        Analysis::Arrangement::Ccb_halfedge_circulator start = iter;
        do
        {
            visitor( iter->data() );
            ++iter;
        } while( iter != start );
    }
}

template < typename TFunctor >
inline bool anyFaceEdge( Analysis::Arrangement::Face_const_handle hFace, TFunctor&& predicate )
{
    bool bSuccess = false;
    visit( hFace,
           [ &bSuccess, &predicate ]( const Analysis::HalfEdgeData& edge )
           {
               if( predicate( edge ) )
               {
                   bSuccess = true;
               };
           } );
    return bSuccess;
}

template < typename TFunctor >
inline bool allFaceEdges( Analysis::Arrangement::Face_const_handle hFace, TFunctor&& predicate )
{
    bool bSuccess = true;
    visit( hFace,
           [ &bSuccess, &predicate ]( const Analysis::HalfEdgeData& edge )
           {
               if( !predicate( edge ) )
               {
                   bSuccess = false;
               };
           } );
    return bSuccess;
}

template < typename HalfEdgeType, typename FaceType, typename EdgePredicate >
inline void getFaces( std::set< FaceType >& startFaces, const std::vector< HalfEdgeType >& polygon,
                      std::vector< FaceType >& result, EdgePredicate&& predicate )
{
    std::set< FaceType > faces = startFaces, open = startFaces, temp;

    INVARIANT( !polygon.empty(), "Polygon empty" );

    // determine the edge boundary as lookup set
    const std::set< HalfEdgeType > boundary( polygon.begin(), polygon.end() );

    // greedy algorithm to exaust open set until no new faces found
    while( !open.empty() )
    {
        for( auto f : open )
        {
            // go through all edges inside face
            auto iter  = f->outer_ccb();
            auto start = iter;
            do
            {
                // if the edge IS NOT on the polygon boundary then
                // it MUST connect to another face WITHIN the polygon
                if( !boundary.contains( iter ) && predicate( iter ) )
                {
                    auto newFace = iter->twin()->face();
                    if( !faces.contains( newFace ) )
                    {
                        temp.insert( newFace );
                        faces.insert( newFace );
                    }
                }
                ++iter;
            } while( iter != start );

            // search through all holes
            for( auto holeIter = f->holes_begin(), holeIterEnd = f->holes_end(); holeIter != holeIterEnd; ++holeIter )
            {
                auto iter  = *holeIter;
                auto start = iter;
                do
                {
                    if( !boundary.contains( iter ) && predicate( iter ) )
                    {
                        auto newFace = iter->twin()->face();
                        if( !faces.contains( newFace ) )
                        {
                            temp.insert( newFace );
                            faces.insert( newFace );
                        }
                    }
                    ++iter;
                } while( iter != start );
            }
        }
        open.clear();
        open.swap( temp );
    }
    result.assign( faces.begin(), faces.end() );
}

inline void getFaces( const Analysis::HalfEdgeVector& polygon, Analysis::FaceVector& result )
{
    Analysis::FaceSet startFaces;
    for( auto e : polygon )
    {
        startFaces.insert( e->face() );
    }
    getFaces( startFaces, polygon, result, []( Analysis::HalfEdge ) { return true; } );
}

inline void locateFloorFacesFromDoorStep( Analysis::Arrangement::Halfedge_handle                       doorStep,
                                          const std::vector< Analysis::Arrangement::Halfedge_handle >& boundary,
                                          std::set< Analysis::Arrangement::Halfedge_handle >&          innerBoundaries )
{
    std::vector< Analysis::Arrangement::Face_handle > result;
    std::set< Analysis::Arrangement::Face_handle >    startFaces;
    startFaces.insert( doorStep->face() );
    getFaces< Analysis::Arrangement::Halfedge_handle, Analysis::Arrangement::Face_handle >(
        startFaces, boundary, result,
        [ &boundary, &innerBoundaries ]( Analysis::Arrangement::Halfedge_handle edge )
        {
            auto& flags = edge->data().flags;

            const bool bIsEdgeNextToBoundary
                = ( ( flags.test( EdgeMask::eInterior ) || flags.test( EdgeMask::eExterior )
                      || flags.test( EdgeMask::eConnectionBisector ) || flags.test( EdgeMask::eDoorStep ) )

                    && !flags.test( EdgeMask::eConnectionBreak ) );

            if( bIsEdgeNextToBoundary )
            {
                if( std::find( boundary.begin(), boundary.end(), edge ) == boundary.end() )
                {
                    innerBoundaries.insert( edge );
                }
            }

            return !bIsEdgeNextToBoundary;
        } );
}

bool areFacesAdjacent( Analysis::Arrangement::Face_const_handle hFace1,
                       Analysis::Arrangement::Face_const_handle hFace2 )
{
    INVARIANT( !hFace1->is_unbounded(), "Invalid face passed to areFacesAdjacent" );

    Analysis::Arrangement::Ccb_halfedge_const_circulator iter  = hFace1->outer_ccb();
    Analysis::Arrangement::Ccb_halfedge_const_circulator start = iter;
    do
    {
        if( iter->twin()->face() == hFace2 )
        {
            return true;
        }
        ++iter;
    } while( iter != start );

    // search through all holes
    for( Analysis::Arrangement::Hole_const_iterator holeIter = hFace1->holes_begin(), holeIterEnd = hFace1->holes_end();
         holeIter != holeIterEnd; ++holeIter )
    {
        Analysis::Arrangement::Ccb_halfedge_const_circulator iter  = *holeIter;
        Analysis::Arrangement::Ccb_halfedge_const_circulator start = iter;
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

// internal query implementation helper functions

template < typename Predicate >
inline void getEdges( const Analysis::Arrangement& arr, Analysis::HalfEdgeSet& edges, Predicate&& predicate )
{
    for( auto i = arr.halfedges_begin(); i != arr.halfedges_end(); ++i )
    {
        if( predicate( i ) )
        {
            edges.insert( i );
        }
    }
}

template < typename Predicate >
inline void getEdges( Analysis::Arrangement& arr, std::set< Analysis::Arrangement::Halfedge_handle >& edges,
                      Predicate&& predicate )
{
    for( auto i = arr.halfedges_begin(); i != arr.halfedges_end(); ++i )
    {
        if( predicate( i ) )
        {
            edges.insert( i );
        }
    }
}

template < typename HalfEdgeSetType >
inline void getPolygons( const HalfEdgeSetType& edges, Analysis::HalfEdgeVectorVector& polygons )
{
    HalfEdgeSetType open = edges;

    using HalfEdge = typename HalfEdgeSetType::value_type;

    while( !open.empty() )
    {
        HalfEdge currentEdge = *open.begin();

        Analysis::HalfEdgeVector polygon;
        while( currentEdge != HalfEdge{} )
        {
            polygon.push_back( currentEdge );
            open.erase( currentEdge );

            HalfEdge nextEdge = {};
            {
                auto edgeIter = currentEdge->target()->incident_halfedges(), edgeIterEnd = edgeIter;
                do
                {
                    HalfEdge outEdge = edgeIter->twin();
                    if( ( currentEdge != outEdge ) && open.contains( outEdge ) )
                    {
                        INVARIANT( nextEdge == HalfEdge{}, "Duplicate next edge found" );
                        nextEdge = outEdge;
                    }
                    ++edgeIter;
                } while( edgeIter != edgeIterEnd );
            }
            currentEdge = nextEdge;
        }
        INVARIANT( polygon.size() >= 3, "Invalid polygon found with less than three edges" );
        polygons.push_back( polygon );
    }
}

template < typename VertexVector, typename HalfEdgeSetType >
inline void searchPolygons( const VertexVector& startVertices, const HalfEdgeSetType& edges, bool bDir,
                            std::vector< std::vector< Analysis::Arrangement::Halfedge_handle > >& polygons )
{
    using HalfEdge = typename HalfEdgeSetType::value_type;

    HalfEdgeSetType open = edges;

    for( auto startVertex : startVertices )
    {
        for( auto e : open )
        {
            if( e->source() == startVertex )
            {
                // attempt to solve polygon from here
                std::vector< Analysis::Arrangement::Halfedge_handle > polygon;

                open.erase( e );
                polygon.push_back( e );

                while( e->target() != startVertex )
                {
                    INVARIANT( !open.empty(), "Failed to find polygon" );
                    // https://doc.cgal.org/latest/Arrangement_on_surface_2/index.html
                    // The call v->incident_halfedges() returns a circulator of the nested type
                    // Halfedge_around_vertex_circulator that enables the traversal of this circular list around a given
                    // vertex v in a clockwise order.

                    // So - the most acute angle first - so take the last one
                    bool bFoundNextVertex = false;
                    auto edgeIter         = e->target()->incident_halfedges();
                    while( edgeIter != e )
                        ++edgeIter;
                    auto edgeIterEnd = edgeIter;
                    do
                    {
                        if( bDir )
                            ++edgeIter;
                        else
                            --edgeIter;
                        HalfEdge nextEdge = edgeIter->twin();
                        if( open.contains( nextEdge ) )
                        {
                            e                = nextEdge;
                            bFoundNextVertex = true;
                            break;
                        }
                    } while( edgeIter != edgeIterEnd );

                    INVARIANT( bFoundNextVertex, "Failed to find next vertex" );

                    open.erase( e );
                    polygon.push_back( e );
                }
                polygons.push_back( polygon );
                break;
            }
        }
    }
}

template < typename OuterEdgePredicate, typename HoleEdgePredicate >
inline void getPolyonsWithHoles( Analysis::Arrangement& arr, OuterEdgePredicate&& outerEdgePredicate,
                                 HoleEdgePredicate&&               holeEdgePredicate,
                                 Analysis::PolygonWithHolesVector& polygonsWithHoles )
{
    struct PolygonNode
    {
        using Ptr    = std::shared_ptr< PolygonNode >;
        using PtrSet = std::set< Ptr >;

        bool                     hole = false;
        Analysis::HalfEdgeVector polygon;
        Analysis::FaceVector     faces;
        PtrSet                   contained;

        enum ContainmentResult
        {
            eContained,
            eContainsThis,
            eDisjoint
        };

        PolygonNode( bool isHole, const Analysis::HalfEdgeVector& polygon )
            : hole( isHole )
            , polygon( polygon )
        {
            getFaces( polygon, faces );
        }

        void collate( Analysis::PolygonWithHolesVector& polygonsWithHoles ) const
        {
            INVARIANT( !hole, "Found hole when expecting contour" );
            Analysis::PolygonWithHoles p;
            p.outer = polygon;
            for( auto pHole : contained )
            {
                pHole->collateHole( polygonsWithHoles, p );
            }
            polygonsWithHoles.push_back( p );
        }

        void collateHole( Analysis::PolygonWithHolesVector& polygonsWithHoles, Analysis::PolygonWithHoles& p ) const
        {
            INVARIANT( hole, "Found contour when expecting hole" );
            p.holes.push_back( polygon );
            for( auto pContour : contained )
            {
                pContour->collate( polygonsWithHoles );
            }
        }

        ContainmentResult tryContain( Ptr pPolygon )
        {
            if( std::includes( faces.begin(), faces.end(), pPolygon->faces.begin(), pPolygon->faces.end() ) )
            {
                ContainmentResult nestedResult = eDisjoint;
                for( auto pChild : contained )
                {
                    switch( pChild->tryContain( pPolygon ) )
                    {
                        case eContained:
                        {
                            nestedResult = eContained;
                        }
                        break;
                        case eContainsThis:
                        {
                            contained.erase( pChild );
                            pPolygon->contained.insert( pChild );
                        }
                        break;
                        case eDisjoint:
                            break;
                    }
                    if( nestedResult == eContained )
                        break;
                }
                if( nestedResult == eDisjoint )
                {
                    contained.insert( pPolygon );
                }
                return eContained;
            }
            else if( std::includes( pPolygon->faces.begin(), pPolygon->faces.end(), faces.begin(), faces.end() ) )
            {
                return eContainsThis;
            }
            else
            {
                Analysis::FaceVector intersection;
                std::set_intersection( faces.begin(), faces.end(), pPolygon->faces.begin(), pPolygon->faces.end(),
                                       std::back_inserter( intersection ) );
                INVARIANT( intersection.empty(), "Non superset face set intersects" );
                return eDisjoint;
            }
        }
    };

    // construct a node for all polygons collecting their faces
    std::vector< typename PolygonNode::Ptr > nodes;
    {
        Analysis::HalfEdgeVectorVector outerPolygons;
        {
            Analysis::HalfEdgeSet outerEdges;
            getEdges( arr, outerEdges, outerEdgePredicate );
            getPolygons( outerEdges, outerPolygons );
        }

        Analysis::HalfEdgeVectorVector holePolygons;
        {
            Analysis::HalfEdgeSet holeEdges;
            getEdges( arr, holeEdges, holeEdgePredicate );
            getPolygons( holeEdges, holePolygons );
        }

        for( auto& poly : outerPolygons )
        {
            nodes.push_back( std::make_shared< PolygonNode >( false, poly ) );
        }
        for( auto& poly : holePolygons )
        {
            nodes.push_back( std::make_shared< PolygonNode >( true, poly ) );
        }
    }

    // determine the tree of polygons based on the rule that
    // 1. Either polygons have completely disjoint set of faces
    // 2. Or a polygon contains another where it has a super set of faces
    using NodeVector = std::set< typename PolygonNode::Ptr >;
    NodeVector rootNodes;

    for( auto pNode : nodes )
    {
        typename PolygonNode::ContainmentResult result = PolygonNode::eDisjoint;
        {
            NodeVector roots = rootNodes;
            for( auto pRoot : roots )
            {
                switch( pRoot->tryContain( pNode ) )
                {
                    case PolygonNode::eContained:
                    {
                        result = PolygonNode::eContained;
                    }
                    break;
                    case PolygonNode::eContainsThis:
                    {
                        // make pNode a root containing Root
                        rootNodes.erase( pRoot );
                        pNode->contained.insert( pRoot );
                    }
                    break;
                    case PolygonNode::eDisjoint:
                        break;
                }
            }
        }
        if( result == PolygonNode::eDisjoint )
        {
            rootNodes.insert( pNode );
        }
    }

    for( auto pNode : rootNodes )
    {
        pNode->collate( polygonsWithHoles );
    }
}
} // namespace

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

            //if( h->data().flags.test( EdgeMask::eInterior

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
    std::vector< Arrangement::Vertex_handle >          startVertices;
    std::set< Analysis::Arrangement::Halfedge_handle > doorSteps;
    {
        getEdges( m_arr, doorSteps,
                  []( Arrangement::Halfedge_handle edge ) { return edge->data().flags.test( EdgeMask::eDoorStep ); } );
        for( auto d : doorSteps )
        {
            startVertices.push_back( d->source() );
        }
    }

    std::vector< std::vector< Arrangement::Halfedge_handle > > floorPolygons;
    {
        std::set< Analysis::Arrangement::Halfedge_handle > floorEdges;
        getEdges( m_arr, floorEdges,
                  []( Arrangement::Halfedge_handle edge )
                  {
                      auto& flags = edge->data().flags;
                      return ( flags.test( EdgeMask::eInterior ) || flags.test( EdgeMask::eExterior )
                               || flags.test( EdgeMask::eConnectionBisector ) || flags.test( EdgeMask::eDoorStep ) )

                             && !flags.test( EdgeMask::eConnectionBreak );
                  } );

        searchPolygons( startVertices, floorEdges, true, floorPolygons );

        for( auto floorBoundary : floorPolygons )
        {
            Partition::Ptr                      pPartition = std::make_unique< Partition >();
            std::set< schematic::Site::PtrCst > sites;
            for( auto e : floorBoundary )
            {
                if( e->data().flags.test( EdgeMask::eInterior ) || e->data().flags.test( EdgeMask::eExterior ) )
                {
                    INVARIANT( !e->data().sites.empty(), "Interior or exterior edge missing site" );
                    sites.insert( e->data().sites.back() );
                }
                e->data().pPartition = pPartition.get();

                classify( e, EdgeMask::ePartitionFloor );

                // need to search for inner walls
                if( e->data().flags.test( EdgeMask::eDoorStep ) )
                {
                    std::set< Analysis::Arrangement::Halfedge_handle > innerBoundaries;
                    locateFloorFacesFromDoorStep( e, floorBoundary, innerBoundaries );
                    while( !innerBoundaries.empty() )
                    {
                        std::vector< std::vector< Arrangement::Halfedge_handle > > innerWallPolygons;
                        std::vector< Arrangement::Vertex_handle >                  innerStartVerts
                            = { ( *innerBoundaries.begin() )->source() };
                        innerBoundaries.erase( *innerBoundaries.begin() );
                        searchPolygons( innerStartVerts, floorEdges, false, innerWallPolygons );

                        bool bMadeProgress = false;
                        for( auto innerWallBoundary : innerWallPolygons )
                        {
                            for( auto innerWallEdge : innerWallBoundary )
                            {
                                // if( innerWallEdge->data().flags.test( EdgeMask::eInterior )
                                //     || innerWallEdge->data().flags.test( EdgeMask::eExterior ) )
                                //{
                                //     INVARIANT( !innerWallEdge->data().sites.empty(),
                                //                "Interior or exterior edge missing site" );
                                //     sites.insert( innerWallEdge->data().sites.back() );
                                // }
                                innerWallEdge->data().pPartition = pPartition.get();
                                classify( innerWallEdge, EdgeMask::ePartitionFloor );
                                if( !innerWallEdge->data().flags.test( EdgeMask::eDoorStep ) )
                                {
                                    classify( innerWallEdge->twin(), EdgeMask::ePartitionBoundary );
                                }

                                innerBoundaries.erase( innerWallEdge );
                                bMadeProgress = true;
                            }
                        }
                        INVARIANT( bMadeProgress, "Failed to make progress on inner walL" );
                    }
                }
            }
            INVARIANT( sites.size() != 0, "Floor has no sites" );
            INVARIANT( sites.size() == 1, "Floor has multiple sites" );
            pPartition->pSite = *sites.begin();
            m_floors.emplace_back( std::move( pPartition ) );
        }
    }

    std::vector< std::vector< Arrangement::Halfedge_handle > > boundaryPolygons;
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

        searchPolygons( startVertices, boundaryEdges, false, boundaryPolygons );

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
}

void Analysis::skeleton()
{
    PolygonWithHolesVector floors;

    getPolyonsWithHoles(
        m_arr,
        // OuterEdgePredicate
        []( Arrangement::Halfedge_const_handle edge ) { return edge->data().flags.test( EdgeMask::ePerimeter ); },
        // HoleEdgePredicate
        []( Arrangement::Halfedge_const_handle edge )
        {
            return edge->data().flags.test( EdgeMask::ePartitionBoundary )
                   && !edge->data().flags.test( EdgeMask::ePerimeterBoundary );
        },
        floors );

    INVARIANT( floors.size() == 1, "Failed to locate single floor" );
    const PolygonWithHoles& floor = floors.front();

    using Point               = exact::Point;
    using Polygon             = exact::Polygon;
    using PolygonWithHoles    = exact::Polygon_with_holes;
    using StraightSkeleton    = CGAL::Straight_skeleton_2< exact::Kernel >;
    using StraightSkeletonPtr = boost::shared_ptr< StraightSkeleton >;

    Polygon outer;
    for( auto& e : floor.outer )
    {
        outer.push_back( e->source()->point() );
    }
    // ensure it is a not a hole
    if( !outer.is_counterclockwise_oriented() )
    {
        std::reverse( outer.begin(), outer.end() );
    }
    PolygonWithHoles polygonWithHoles( outer );

    for( auto& h : floor.holes )
    {
        Polygon hole;
        for( auto& e : h )
        {
            hole.push_back( e->source()->point() );
        }
        // ensure it is a hole
        if( !hole.is_clockwise_oriented() )
        {
            std::reverse( hole.begin(), hole.end() );
        }
        polygonWithHoles.add_hole( hole );
    }

    StraightSkeletonPtr pStraightSkeleton = CGAL::create_interior_straight_skeleton_2(
        polygonWithHoles.outer_boundary().begin(), polygonWithHoles.outer_boundary().end(),
        polygonWithHoles.holes_begin(), polygonWithHoles.holes_end(), exact::Kernel() );

    for( auto i = pStraightSkeleton->halfedges_begin(), iEnd = pStraightSkeleton->halfedges_end(); i != iEnd; ++i )
    {
        StraightSkeleton::Halfedge_handle h = i;
        // h->defining_contour_edge()

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

    static constexpr std::array< std::pair< double, EdgeMask::Type >, 4 > extrusions
        = { std::pair< double, EdgeMask::Type >{ 1.0, EdgeMask::eExtrusionOne },
            std::pair< double, EdgeMask::Type >{ 2.0, EdgeMask::eExtrusionTwo },
            std::pair< double, EdgeMask::Type >{ 3.0, EdgeMask::eExtrusionThree },
            std::pair< double, EdgeMask::Type >{ 4.0, EdgeMask::eExtrusionFour } };
    for( const auto& [ fOffset, mask ] : extrusions )
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
                renderCurve( Curve( Point( i->x(), i->y() ), Point( iNext->x(), iNext->y() ) ), mask );
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

void Analysis::getBoundaryPolygons( HalfEdgeVectorVector& polygons ) const
{
    HalfEdgeSet partitionEdges;
    getEdges( m_arr, partitionEdges,
              []( Arrangement::Halfedge_const_handle edge )
              {
                  return edge->data().flags.test( EdgeMask::ePartitionBoundary )
                         && !edge->twin()->data().flags.test( EdgeMask::ePerimeter );
              } );

    getPolygons( partitionEdges, polygons );
}

} // namespace exact
