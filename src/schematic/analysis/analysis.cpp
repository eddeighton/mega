
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

#include <vector>

namespace exact
{
Analysis::Analysis( schematic::Schematic::Ptr pSchematic )
    : m_pSchematic( pSchematic )
    , m_observer( m_arr )
{
    for( schematic::Site::Ptr pSite : pSchematic->getSites() )
    {
        recurse( pSite );
    }
    for( schematic::Site::Ptr pSite : pSchematic->getSites() )
    {
        connect( pSite );
    }

    for( schematic::Site::Ptr pSite : pSchematic->getSites() )
    {
        recursePost( pSite );
    }
}

void Analysis::classify( Arrangement::Halfedge_handle h, EdgeMask::Type mask )
{
    auto data = h->data();
    data.flags.set( mask );
    h->set_data( data );
}

void Analysis::renderContour( const exact::Transform& transform, const exact::Polygon& polyOriginal,
                              EdgeMask::Type innerMask, EdgeMask::Type outerMask )
{
    // transform to absolute coordinates
    exact::Polygon exactPoly;
    for( const exact::Point& pt : polyOriginal )
    {
        exactPoly.push_back( transform( pt ) );
    }

    // render the line segments
    for( auto i = exactPoly.begin(), iNext = exactPoly.begin(), iEnd = exactPoly.end(); i != iEnd; ++i )
    {
        ++iNext;
        if( iNext == iEnd )
            iNext = exactPoly.begin();

        Arrangement::Curve_handle firstCurve = CGAL::insert( m_arr, exact::Curve( *i, *iNext ) );
        for( auto i = m_arr.induced_edges_begin( firstCurve ); i != m_arr.induced_edges_end( firstCurve ); ++i )
        {
            Arrangement::Halfedge_handle h = *i;
            classify( h, innerMask );
            classify( h->twin(), outerMask );
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
            renderContour( transform, pSpace->getInteriorPolygon(), EdgeMask::eInterior, EdgeMask::eInteriorBoundary );
        }

        // render the exterior polygons
        {
            for( const exact::Polygon& p : pSpace->getInnerExteriorUnions() )
            {
                renderContour( transform, p, EdgeMask::eExterior, EdgeMask::eExteriorBoundary );
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
        renderContour( transform, pSpace->getSitePolygon(), EdgeMask::eSite, EdgeMask::eSite );
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
        VERIFY_RTE( bFound );
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
        VERIFY_RTE( bFound );
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
                    classify( firstBisectorEdge->twin(), EdgeMask::eConnectionBoundary );
                    if( bFoundFirst )
                        throw std::runtime_error( "Failed in connect" );
                    VERIFY_RTE( !bFoundFirst );
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
                    classify( secondBisectorEdge, EdgeMask::eConnectionBisector );
                    classify( secondBisectorEdge->twin(), EdgeMask::eConnectionBoundary );
                    if( bFoundSecond )
                        throw std::runtime_error( "Failed in connect" );
                    VERIFY_RTE( !bFoundSecond );
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

        VERIFY_RTE_MSG(
            bFoundFirst && bFoundSecond, "Failed to construct connection: " << pConnection->Node::getName() );
        constructConnectionEdges( pConnection, firstBisectorEdge, secondBisectorEdge );

        // VERIFY_RTE_MSG( toRemove.size() == 4, "Bad connection" );
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

void Analysis::getEdges( std::vector< schematic::Segment >& edges, EdgeMask::Set include, EdgeMask::Set exclude )
{
    exact::ExactToInexact convert;
    for( auto i = m_arr.halfedges_begin(); i != m_arr.halfedges_end(); ++i )
    {
        const auto& data = i->data();

        auto inclusive = data.flags & include;
        auto exclusive = data.flags & exclude;

        if( inclusive.any() && exclusive.none() )
        {
            edges.emplace_back( convert( i->source()->point() ), convert( i->target()->point() ) );
        }

        /*if( data.flags.test( eInterior ) || data.flags.test( eExterior ) || data.flags.test( eDoorStep )
            || data.flags.test( eConnectionBisector ) )
        {
            if( !data.flags.test( eConnectionBreak ) )
            {
            }
        }*/
    }
}
} // namespace exact
