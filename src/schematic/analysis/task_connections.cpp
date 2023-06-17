
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

#include "constructions.hpp"

#include "schematic/analysis/analysis.hpp"
#include "schematic/cut.hpp"
#include "schematic/schematic.hpp"

#include <vector>
#include <tuple>

namespace exact
{

void Analysis::connections()
{
    auto sites = m_pSchematic->getSites();
    INVARIANT( sites.size() < 2, "Schematic contains more than one root site" );
    INVARIANT( sites.size() == 1, "Schematic missing root site" );

    schematic::Space::Ptr pRootSpace = boost::dynamic_pointer_cast< schematic::Space >( sites.front() );
    INVARIANT( pRootSpace, "Root site is not a space" );

    connect( pRootSpace );
    renderSiteContours( pRootSpace );
    cut( pRootSpace );
}

void Analysis::renderSiteContours( schematic::Site::Ptr pSite )
{
    if( schematic::Space::Ptr pSpace = boost::dynamic_pointer_cast< schematic::Space >( pSite ) )
    {
        const exact::Transform transform = pSpace->getAbsoluteExactTransform();

        // render the site polygon
        renderContour( m_arr, transform, pSpace->getSitePolygon(), EdgeMask::eSite, EdgeMask::eSite, pSpace, {} );
    }

    for( schematic::Site::Ptr pNestedSite : pSite->getSites() )
    {
        renderSiteContours( pNestedSite );
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

        //std::vector< Arrangement::Halfedge_handle > toRemove;
        for( auto i = m_arr.induced_edges_begin( curve ); i != m_arr.induced_edges_end( curve ); ++i )
        {
            Arrangement::Halfedge_handle h = *i;

            // TODO - possibly remove cut edges that are NOT within boundaries

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

        /*for( Arrangement::Halfedge_handle h : toRemove )
        {
            CGAL::remove_edge( m_arr, h );
        }*/
    }

    for( schematic::Site::Ptr pNestedSite : pSite->getSites() )
    {
        cut( pNestedSite );
    }
}

} // namespace exact
