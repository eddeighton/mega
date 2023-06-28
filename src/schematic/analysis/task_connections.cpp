
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
#include <list>

namespace exact
{

void Analysis::ports()
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

void Analysis::constructConnectionEdges( schematic::Connection::Ptr pConnection,
                                         HalfEdgeVector&            firstBisectors,
                                         HalfEdgeVector&            secondBisectors )
{
    // ensure the input sequences are sorted correctly
    {
        auto sortSequence = []( HalfEdgeVector& sequence )
        {
            std::list< HalfEdge > sorted;

            HalfEdgeSet edges( sequence.begin(), sequence.end() );

            bool bProgress = true;
            while( bProgress && !edges.empty() )
            {
                bProgress = false;

                if( sorted.empty() )
                {
                    auto iFirst = edges.begin();
                    sorted.push_back( *iFirst );
                    edges.erase( iFirst );
                    bProgress = true;
                }
                else
                {
                    for( auto i = edges.begin(), iEnd = edges.end(); i != iEnd; ++i )
                    {
                        auto e = *i;
                        if( e->target() == sorted.front()->source() )
                        {
                            sorted.push_front( e );
                            edges.erase( i );
                            bProgress = true;
                            break;
                        }
                        else if( e->source() == sorted.back()->target() )
                        {
                            sorted.push_back( e );
                            edges.erase( i );
                            bProgress = true;
                            break;
                        }
                    }
                }
            }
            INVARIANT( edges.empty(), "Failed to sort input edge sequence" );
            sequence.assign( sorted.begin(), sorted.end() );
        };

        INVARIANT( !firstBisectors.empty(), "First bisectors empty: " << pConnection->Node::getName() );
        INVARIANT( !secondBisectors.empty(), "Second bisectors empty: " << pConnection->Node::getName() );

        sortSequence( firstBisectors );
        sortSequence( secondBisectors );

        {
            HalfEdge eLast;
            for( auto e : firstBisectors )
            {
                INVARIANT( ( eLast == HalfEdge{} ) || ( eLast->target() == e->source() ),
                           "First bisectors not sequence: " << pConnection->Node::getName() );
                eLast = e;
            }
        }
        {
            HalfEdge eLast;
            for( auto e : secondBisectors )
            {
                INVARIANT( ( eLast == HalfEdge{} ) || ( eLast->target() == e->source() ),
                           "Second bisectors not sequence: " << pConnection->Node::getName() );
                eLast = e;
            }
        }
    }

    // find all edges between bisectors and label as connection break edges UNLESS actual doorstep
    // if there is a single site edge then it is used as the doorstep
    HalfEdgeSet breakEdges;
    HalfEdgeSet siteEdges;
    {
        VertexSet first, second;
        for( auto e : firstBisectors )
        {
            first.insert( e->source() );
            first.insert( e->target() );
        }
        for( auto e : secondBisectors )
        {
            second.insert( e->source() );
            second.insert( e->target() );
        }

        for( auto v : first )
        {
            Arrangement::Halfedge_around_vertex_circulator first, iter;
            first = iter = v->incident_halfedges();
            do
            {
                if( second.contains( iter->source() ) )
                {
                    if( test( iter, EdgeMask::eSite ) )
                    {
                        siteEdges.insert( iter );
                        siteEdges.insert( iter->twin() );
                    }
                    else
                    {
                        breakEdges.insert( iter );
                        breakEdges.insert( iter->twin() );
                    }
                }
                ++iter;
            } while( iter != first );
        }
    }

    for( auto e : breakEdges )
    {
        classify( e, EdgeMask::eConnectionBreak );
    }

    // now determine the doorStep
    std::optional< HalfEdge > hDoorStep;

    if( siteEdges.size() == 2 )
    {
        for( auto e : siteEdges )
        {
            hDoorStep = e;
            break;
        }
    }
    else
    {
        for( auto e : siteEdges )
        {
            classify( e, EdgeMask::eConnectionBreak );
        }

        auto findEdge = []( const HalfEdgeVector& edges, const exact::Kernel::FT& distance ) -> HalfEdge
        {
            exact::Kernel::FT firstDist = 0.0;
            for( auto e : edges )
            {
                const auto d = firstDist + CGAL::squared_distance( e->source()->point(), e->target()->point() );
                if( distance <= d )
                {
                    return e;
                }
                else
                {
                    firstDist = firstDist + d;
                }
            }
            INVARIANT( false, "Failed to locate mid point edge" );
        };

        // construct doorstep from midpoints
        Vertex      vFirstStart  = firstBisectors.front()->source();
        Vertex      vFirstEnd    = firstBisectors.back()->target();
        const Point ptFirstStart = vFirstStart->point();
        const Point ptFirstEnd   = vFirstEnd->point();
        HalfEdge    firstEdge    = findEdge( firstBisectors, CGAL::squared_distance( ptFirstStart, ptFirstEnd ) / 2.0 );
        const Point ptFirstMid   = ptFirstStart + ( ptFirstEnd - ptFirstStart ) / 2.0;
        HalfEdge    hFirstStartToMid = m_arr.split_edge( firstEdge, ptFirstMid );
        Vertex      vFirstMid        = hFirstStartToMid->target();

        Vertex      vSecondStart  = secondBisectors.front()->source();
        Vertex      vSecondEnd    = secondBisectors.back()->target();
        const Point ptSecondStart = vSecondStart->point();
        const Point ptSecondEnd   = vSecondEnd->point();
        HalfEdge secondEdge = findEdge( secondBisectors, CGAL::squared_distance( ptSecondStart, ptSecondEnd ) / 2.0 );
        const Point ptSecondMid       = ptSecondStart + ( ptSecondEnd - ptSecondStart ) / 2.0;
        HalfEdge    hSecondStartToMid = m_arr.split_edge( secondEdge, ptSecondMid );
        Vertex      vSecondMid        = hSecondStartToMid->target();

        const ::exact::Curve segDoorStep( ptFirstMid, ptSecondMid );
        hDoorStep = m_arr.insert_at_vertices( segDoorStep, vFirstMid, vSecondMid );
    }

    // classify the doorStep
    INVARIANT( hDoorStep.has_value(), "Failed to construct doorstep: " << pConnection->Node::getName() );
    {
        classify( hDoorStep.value(), EdgeMask::eDoorStep );
        classify( hDoorStep.value()->twin(), EdgeMask::eDoorStep );
        hDoorStep.value()->data().pConnection         = pConnection;
        hDoorStep.value()->twin()->data().pConnection = pConnection;
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
        HalfEdgeVector toRemove, firstBisectors, secondBisectors;
        {
            Curve_handle firstCurve = CGAL::insert( m_arr, Curve( ptFirstStart, ptFirstEnd ) );

            for( auto i = m_arr.induced_edges_begin( firstCurve ); i != m_arr.induced_edges_end( firstCurve ); ++i )
            {
                HalfEdge h = *i;

                if( ( h->source()->point() == ptFirstStart ) || ( h->source()->point() == ptFirstEnd )
                    || ( h->target()->point() == ptFirstStart ) || ( h->target()->point() == ptFirstEnd ) )
                {
                    toRemove.push_back( h );
                }
                else
                {
                    firstBisectors.push_back( h );

                    const exact::Direction halfEdgeDir( h->target()->point() - h->source()->point() );
                    const exact::Direction halfEdgeOrthoDir
                        = halfEdgeDir.perpendicular( CGAL::Orientation::COUNTERCLOCKWISE );

                    if( halfEdgeOrthoDir == edgeDir )
                    {
                        classify( h->twin(), EdgeMask::eConnectionBisector );
                        classify( h, EdgeMask::eConnectionBisectorBoundary );
                    }
                    else
                    {
                        classify( h, EdgeMask::eConnectionBisector );
                        classify( h->twin(), EdgeMask::eConnectionBisectorBoundary );
                    }
                }
            }
        }

        {
            Curve_handle secondCurve = CGAL::insert( m_arr, Curve( ptSecondStart, ptSecondEnd ) );

            for( auto i = m_arr.induced_edges_begin( secondCurve ); i != m_arr.induced_edges_end( secondCurve ); ++i )
            {
                HalfEdge h = *i;

                if( ( h->source()->point() == ptSecondStart ) || ( h->source()->point() == ptSecondEnd )
                    || ( h->target()->point() == ptSecondStart ) || ( h->target()->point() == ptSecondEnd ) )
                {
                    toRemove.push_back( h );
                }
                else
                {
                    secondBisectors.push_back( h );

                    const exact::Direction halfEdgeDir( h->target()->point() - h->source()->point() );
                    const exact::Direction halfEdgeOrthoDir
                        = halfEdgeDir.perpendicular( CGAL::Orientation::COUNTERCLOCKWISE );

                    if( halfEdgeOrthoDir == edgeDir )
                    {
                        classify( h, EdgeMask::eConnectionBisector );
                        classify( h->twin(), EdgeMask::eConnectionBisectorBoundary );
                    }
                    else
                    {
                        classify( h->twin(), EdgeMask::eConnectionBisector );
                        classify( h, EdgeMask::eConnectionBisectorBoundary );
                    }
                }
            }
        }

        if( firstBisectors.empty() || secondBisectors.empty() )
        {
            std::ostringstream os;
            os << "Failed to find bisectors for connection: " << pConnection->Node::getName();
            throw std::runtime_error( os.str() );
        }

        constructConnectionEdges( pConnection, firstBisectors, secondBisectors );

        for( HalfEdge h : toRemove )
        {
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

        // std::vector< HalfEdge > toRemove;
        for( auto i = m_arr.induced_edges_begin( curve ); i != m_arr.induced_edges_end( curve ); ++i )
        {
            HalfEdge h = *i;

            // TODO - possibly remove cut edges that are NOT within boundaries

            // cut must be inside boundary face

            // if( test( h, EdgeMask::eInterior

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

        /*for( HalfEdge h : toRemove )
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
