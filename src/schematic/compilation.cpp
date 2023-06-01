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

#include "schematic/compilation.hpp"
#include "schematic/svgUtils.hpp"
#include "schematic/connection.hpp"
#include "schematic/schematic.hpp"

#include <algorithm>

namespace
{
bool doesFaceHaveDoorstep( exact::Arrangement::Face_const_handle hFace )
{
    if( !hFace->is_unbounded() )
    {
        exact::Arrangement::Ccb_halfedge_const_circulator iter  = hFace->outer_ccb();
        exact::Arrangement::Ccb_halfedge_const_circulator start = iter;
        do
        {
            if( iter->data().get() )
            {
                return true;
            }
            ++iter;
        } while( iter != start );
    }

    // search through all holes
    for( exact::Arrangement::Hole_const_iterator holeIter = hFace->holes_begin(), holeIterEnd = hFace->holes_end();
         holeIter != holeIterEnd; ++holeIter )
    {
        exact::Arrangement::Ccb_halfedge_const_circulator iter  = *holeIter;
        exact::Arrangement::Ccb_halfedge_const_circulator start = iter;
        do
        {
            if( iter->data().get() )
            {
                return true;
            }
            ++iter;
        } while( iter != start );
    }
    return false;
}
} // namespace

namespace analysis
{

Compilation::Compilation()
{
}

Compilation::Compilation( schematic::Schematic::Ptr pSchematic )
{
    for( schematic::Site::Ptr pSite : pSchematic->getSites() )
    {
        recurse( pSite );
    }
    for( schematic::Site::Ptr pSite : pSchematic->getSites() )
    {
        connect( pSite );
    }

    // record ALL doorsteps
    std::vector< Arrangement::Halfedge_handle > edges;
    for( auto i = m_arr.edges_begin(); i != m_arr.edges_end(); ++i )
    {
        if( i->data().get() )
        {
            edges.push_back( i );
        }
    }

    for( schematic::Site::Ptr pSite : pSchematic->getSites() )
    {
        recursePost( pSite );
    }

    for( Arrangement::Halfedge_handle i : edges )
    {
        if( !i->data().get() )
        {
            THROW_RTE( "Doorstep edge lost after recursePost" );
        }
    }
}

void Compilation::renderContour( Arrangement&            arr,
                                 const exact::Transform& transform,
                                 const exact::Polygon&   polyOriginal )
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
        CGAL::insert( arr, exact::Curve( *i, *iNext ) );
    }
}

void Compilation::recurse( schematic::Site::Ptr pSite )
{
    if( schematic::Space::Ptr pSpace = boost::dynamic_pointer_cast< schematic::Space >( pSite ) )
    {
        const exact::Transform transform = pSpace->getAbsoluteExactTransform();

        // render the interior polygon
        {
            renderContour( m_arr, transform, pSpace->getInteriorPolygon() );
        }

        // render the exterior polygons
        {
            for( const exact::Polygon& p : pSpace->getInnerExteriorUnions() )
            {
                renderContour( m_arr, transform, p );
            }
        }
    }

    for( schematic::Site::Ptr pNestedSite : pSite->getSites() )
    {
        recurse( pNestedSite );
    }
}

void Compilation::recursePost( schematic::Site::Ptr pSite )
{
    if( schematic::Space::Ptr pSpace = boost::dynamic_pointer_cast< schematic::Space >( pSite ) )
    {
        const exact::Transform transform = pSpace->getAbsoluteExactTransform();

        // render the site polygon
        renderContour( m_arr, transform, pSpace->getSitePolygon() );
    }

    for( schematic::Site::Ptr pNestedSite : pSite->getSites() )
    {
        recursePost( pNestedSite );
    }
}

void constructConnectionEdges( ::exact::Arrangement&                 arr,
                               schematic::Connection::Ptr            pConnection,
                               ::exact::Arrangement::Halfedge_handle firstBisectorEdge,
                               ::exact::Arrangement::Halfedge_handle secondBisectorEdge )
{
    using Arrangement = ::exact::Arrangement;
    using Point       = ::exact::Point;

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

    Arrangement::Halfedge_handle hFirstStartToMid = arr.split_edge( firstBisectorEdge, ptFirstMid );
    Arrangement::Vertex_handle   vFirstMid        = hFirstStartToMid->target();

    Arrangement::Halfedge_handle hSecondStartToMid = arr.split_edge( secondBisectorEdge, ptSecondMid );
    Arrangement::Vertex_handle   vSecondMid        = hSecondStartToMid->target();

    // create edge between mid points
    Arrangement::Halfedge_handle m_hDoorStep;
    {
        const ::exact::Curve segDoorStep( ptFirstMid, ptSecondMid );
        m_hDoorStep = arr.insert_at_vertices( segDoorStep, vFirstMid, vSecondMid );
    }

    m_hDoorStep->set_data( ( ::exact::DefaultedBool( true ) ) );
    m_hDoorStep->twin()->set_data( ( ::exact::DefaultedBool( true ) ) );

    {
        bool                                           bFound = false;
        Arrangement::Halfedge_around_vertex_circulator first, iter;
        first = iter = vFirstStart->incident_halfedges();
        do
        {
            if( ( iter->source() == vSecondStart ) || ( iter->source() == vSecondEnd ) )
            {
                arr.remove_edge( iter );
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
                arr.remove_edge( iter );
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

void Compilation::connect( schematic::Site::Ptr pSite )
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
        constructConnectionEdges( m_arr, pConnection, firstBisectorEdge, secondBisectorEdge );

        // VERIFY_RTE_MSG( toRemove.size() == 4, "Bad connection" );
        for( Arrangement::Halfedge_handle h : toRemove )
        {
            m_arr.remove_edge( h );
        }
    }

    for( schematic::Site::Ptr pNestedSite : pSite->getSites() )
    {
        connect( pNestedSite );
    }
}

void Compilation::getFaces( FaceHandleSet& floorFaces, FaceHandleSet& fillerFaces )
{
    for( auto i = m_arr.faces_begin(), iEnd = m_arr.faces_end(); i != iEnd; ++i )
    {
        Arrangement::Face_const_handle hFace = i;
        // if( !hFace->is_unbounded() && !hFace->is_fictitious() )
        {
            if( doesFaceHaveDoorstep( hFace ) )
                floorFaces.insert( hFace );
            else
            {
                // fillers cannot have holes
                if( hFace->holes_begin() == hFace->holes_end() )
                {
                    fillerFaces.insert( hFace );
                }
            }
        }
    }
}

void Compilation::getEdges( std::vector< schematic::Segment >& edges )
{
    exact::ExactToInexact convert;
    for( auto i = m_arr.edges_begin(); i != m_arr.edges_end(); ++i )
    {
        edges.emplace_back( convert( i->source()->point() ), convert( i->target()->point() ) );
    }
}

/*
void Compilation::render( const boost::filesystem::path& filepath )
{
    EdgeVectorVector edgeGroups;
    std::vector< Arrangement::Halfedge_const_handle > edges;
    for( auto i = m_arr.edges_begin(); i != m_arr.edges_end(); ++i )
        edges.push_back( i );
    edgeGroups.push_back( edges );

    SVGStyle style;
    generateHTML( filepath, m_arr, edgeGroups, style );
}


void Compilation::renderFloors( const boost::filesystem::path& filepath )
{
    EdgeVectorVector edgeGroups;

    using EdgeVector = std::vector< Arrangement::Halfedge_const_handle >;
    for( auto i = m_arr.faces_begin(),
        iEnd = m_arr.faces_end(); i!=iEnd; ++i )
    {
        EdgeVector edges;
        //for each face determine if it has a doorstep - including only in the holes
        bool bDoesFaceHaveDoorstep = false;
        if( !i->is_unbounded() )
        {
            Arrangement::Ccb_halfedge_const_circulator iter = i->outer_ccb();
            Arrangement::Ccb_halfedge_const_circulator start = iter;
            do
            {
                edges.push_back( iter );
                if( iter->data().get() )
                {
                    bDoesFaceHaveDoorstep = true;
                }
                ++iter;
            }
            while( iter != start );

            if( !bDoesFaceHaveDoorstep )
            {
                //search through all holes
                for( Arrangement::Hole_const_iterator
                    holeIter = i->holes_begin(),
                    holeIterEnd = i->holes_end();
                        holeIter != holeIterEnd; ++holeIter )
                {
                    Arrangement::Ccb_halfedge_const_circulator iter = *holeIter;
                    Arrangement::Ccb_halfedge_const_circulator start = iter;
                    do
                    {
                        if( iter->data().get() )
                        {
                            bDoesFaceHaveDoorstep = true;
                            break;
                        }
                        ++iter;
                    }
                    while( iter != start );
                }
            }
        }

        if( bDoesFaceHaveDoorstep )
        {
            for( Arrangement::Hole_const_iterator
                holeIter = i->holes_begin(),
                holeIterEnd = i->holes_end();
                    holeIter != holeIterEnd; ++holeIter )
            {
                Arrangement::Ccb_halfedge_const_circulator iter = *holeIter;
                Arrangement::Ccb_halfedge_const_circulator start = iter;
                do
                {
                    edges.push_back( iter );
                    ++iter;
                }
                while( iter != start );
            }
            edgeGroups.push_back( edges );
        }
    }

    SVGStyle style;
    generateHTML( filepath, m_arr, edgeGroups, style );
}


void Compilation::renderFillers( const boost::filesystem::path& filepath )
{
    EdgeVectorVector edgeGroups;

    for( auto i = m_arr.faces_begin(),
        iEnd = m_arr.faces_end(); i!=iEnd; ++i )
    {
        EdgeVector edges;
        //for each face determine if it has a doorstep - including only in the holes
        bool bDoesFaceHaveDoorstep = false;
        if( !i->is_unbounded() )
        {
            Arrangement::Ccb_halfedge_const_circulator iter = i->outer_ccb();
            Arrangement::Ccb_halfedge_const_circulator start = iter;
            do
            {
                edges.push_back( iter );
                if( iter->data().get() )
                {
                    bDoesFaceHaveDoorstep = true;
                }
                ++iter;
            }
            while( iter != start );

            if( !bDoesFaceHaveDoorstep )
            {
                //search through all holes
                for( Arrangement::Hole_const_iterator
                    holeIter = i->holes_begin(),
                    holeIterEnd = i->holes_end();
                        holeIter != holeIterEnd; ++holeIter )
                {
                    Arrangement::Ccb_halfedge_const_circulator iter = *holeIter;
                    Arrangement::Ccb_halfedge_const_circulator start = iter;
                    do
                    {
                        if( iter->data().get() )
                        {
                            bDoesFaceHaveDoorstep = true;
                            break;
                        }
                        ++iter;
                    }
                    while( iter != start );
                }
            }
        }

        if( !bDoesFaceHaveDoorstep && !i->is_unbounded() )
        {
            for( Arrangement::Hole_const_iterator
                holeIter = i->holes_begin(),
                holeIterEnd = i->holes_end();
                    holeIter != holeIterEnd; ++holeIter )
            {
                Arrangement::Ccb_halfedge_const_circulator iter = *holeIter;
                Arrangement::Ccb_halfedge_const_circulator start = iter;
                do
                {
                    edges.push_back( iter );
                    ++iter;
                }
                while( iter != start );
            }
            edgeGroups.push_back( edges );
        }
    }

    SVGStyle style;
    generateHTML( filepath, m_arr, edgeGroups, style );
}*/

void Compilation::save( std::ostream& os ) const
{
    exact::Formatter formatter;
    CGAL::write( m_arr, os, formatter );
}

void Compilation::load( std::istream& is )
{
    exact::Formatter formatter;
    CGAL::read( m_arr, is, formatter );
}
} // namespace analysis
