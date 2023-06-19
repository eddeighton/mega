
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

#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>

#include "schematic/schematic.hpp"
#include "schematic/analysis/analysis.hpp"

TEST( Schematic, Basic )
{
    using namespace schematic;

    Schematic test( "test" );

    ASSERT_EQ( test.getName(), "test" );
}

TEST( Schematic, Arrangement_BasicInsertSegment )
{
    using namespace exact;

    Analysis::Arrangement arr;

    Point   p1( 1, 1 ), p2( 1, 2 ), p3( 2, 1 );
    Segment cv[] = { Segment( p1, p2 ), Segment( p2, p3 ), Segment( p3, p1 ) };

    CGAL::insert( arr, &cv[ 0 ], &cv[ sizeof( cv ) / sizeof( Segment ) ] );
    ASSERT_EQ( arr.number_of_faces(), 2 );
}

TEST( Schematic, Arrangement_InsertCurveAndGetInducedEdges )
{
    using namespace exact;

    using Arrangement = Analysis::Arrangement;
    Arrangement arr;

    const Point p1( 1, 1 ), p2( 1, 2 ), p3( 1, 3 );

    {
        Arrangement::Curve_handle firstCurve = CGAL::insert( arr, Curve( p1, p2 ) );

        int iCounter = 0;
        for( auto i = arr.induced_edges_begin( firstCurve ); i != arr.induced_edges_end( firstCurve ); ++i )
        {
            Arrangement::Halfedge_handle h = *i;
            ++iCounter;
        }

        ASSERT_EQ( iCounter, 1 );
    }

    {
        Arrangement::Curve_handle firstCurve = CGAL::insert( arr, Curve( p1, p3 ) );

        int iCounter = 0;
        for( auto i = arr.induced_edges_begin( firstCurve ); i != arr.induced_edges_end( firstCurve ); ++i )
        {
            Arrangement::Halfedge_handle h = *i;
            ++iCounter;
        }

        ASSERT_EQ( iCounter, 2 );
    }

    {
        // ASSERT_TRUE( CGAL::do_intersect( arr, Curve( p2, p3 ) ) );
    }

    {
        std::vector< CGAL::Object >  objects;
        Arrangement::Face_handle     face;
        Arrangement::Halfedge_handle halfedge;
        Arrangement::Vertex_handle   vertex;
        CGAL::zone( arr, Curve( p1, p3 ), std::back_inserter( objects ) );

        int iFaces = 0, iHalfEdges = 0, iVertices = 0;
        for( auto& obj : objects )
        {
            if( CGAL::assign( face, obj ) )
            {
                iFaces++;
            }
            if( CGAL::assign( halfedge, obj ) )
            {
                iHalfEdges++;
            }
            if( CGAL::assign( vertex, obj ) )
            {
                iVertices++;
            }
        }
        ASSERT_EQ( iFaces, 0 );
        ASSERT_EQ( iHalfEdges, 2 );
        ASSERT_EQ( iVertices, 3 );
    }
}

TEST( Schematic, Arrangement_EdgeIntersect )
{
    using namespace exact;

    using Arrangement = Analysis::Arrangement;
    Arrangement        arr;
    Analysis::Observer observer( arr );

    const Point p1( 1, 1 ), p2( 3, 1 );
    const Point p3( 2, 0 ), p4( 2, 2 );

    {
        Arrangement::Curve_handle firstCurve = CGAL::insert( arr, Curve( p1, p2 ) );

        int iCounter = 0;
        for( auto i = arr.induced_edges_begin( firstCurve ); i != arr.induced_edges_end( firstCurve ); ++i )
        {
            Arrangement::Halfedge_handle h = *i;
            classify( h, exact::EdgeMask::eInterior );
            classify( h->twin(), exact::EdgeMask::eInterior );
            ++iCounter;
        }
        ASSERT_EQ( iCounter, 1 );
    }

    {
        Arrangement::Curve_handle firstCurve = CGAL::insert( arr, Curve( p3, p4 ) );
        int                       iCounter   = 0;
        for( auto i = arr.induced_edges_begin( firstCurve ); i != arr.induced_edges_end( firstCurve ); ++i )
        {
            Arrangement::Halfedge_handle h = *i;
            classify( h, exact::EdgeMask::eSite );
            classify( h->twin(), exact::EdgeMask::eSite );
            ++iCounter;
        }
        ASSERT_EQ( iCounter, 2 );
    }

    {
        std::vector< CGAL::Object >        objects;
        Arrangement::Face_handle           face;
        Arrangement::Halfedge_handle       halfedge;
        Arrangement::Vertex_handle         vertex;
        CGAL::zone( arr, Curve( p1, p2 ), std::back_inserter( objects ) );

        int iFaces = 0, iHalfEdges = 0, iVertices = 0;
        for( auto& obj : objects )
        {
            if( CGAL::assign( face, obj ) )
            {
                iFaces++;
            }
            else if( CGAL::assign( halfedge, obj ) )
            {
                iHalfEdges++;
                ASSERT_TRUE( test( halfedge, exact::EdgeMask::eInterior ) );
                ASSERT_FALSE( test( halfedge, exact::EdgeMask::eSite ) );
            }
            else if( CGAL::assign( vertex, obj ) )
            {
                iVertices++;
            }
        }
        ASSERT_EQ( iFaces, 0 );
        ASSERT_EQ( iHalfEdges, 2 );
        ASSERT_EQ( iVertices, 3 );
    }
}

TEST( Schematic, Arrangement_FaceIntersect )
{
    using namespace exact;

    using Arrangement = Analysis::Arrangement;
    Arrangement        arr;
    Analysis::Observer observer( arr );

    Analysis::Partition partition;

    // create triangle
    {
        const Point p1( 0, 0 ), p2( 3, 0 ), p3( 3, 3 );

        Arrangement::Curve_handle firstCurve  = CGAL::insert( arr, Curve( p1, p2 ) );
        Arrangement::Curve_handle secondCurve = CGAL::insert( arr, Curve( p2, p3 ) );
        Arrangement::Curve_handle thirdCurve  = CGAL::insert( arr, Curve( p3, p1 ) );

        for( auto i = arr.induced_edges_begin( firstCurve ); i != arr.induced_edges_end( firstCurve ); ++i )
        {
            Arrangement::Halfedge_handle h = *i;
            h->face()->set_data( Analysis::FaceData{ &partition } );
            break;
        }
    }
    ASSERT_EQ( arr.number_of_faces(), 2 );

    {
        int iCounter = 0;
        for( auto i = arr.faces_begin(), iEnd = arr.faces_end(); i != iEnd; ++i )
        {
            auto& data = i->data();
            if( data.pPartition == &partition )
            {
                ++iCounter;
            }
        }
        ASSERT_EQ( iCounter, 1 );
    }

    // now split the face

    {
        const Point               p1( 1, -1 ), p2( 1, 4 );
        Arrangement::Curve_handle firstCurve = CGAL::insert( arr, Curve( p1, p2 ) );
    }

    ASSERT_EQ( arr.number_of_faces(), 3 );

    // test that the partition pointer has been propagated to the two new faces from the old one
    {
        int iCounter = 0;
        for( auto i = arr.faces_begin(), iEnd = arr.faces_end(); i != iEnd; ++i )
        {
            auto& data = i->data();
            if( data.pPartition == &partition )
            {
                ++iCounter;
            }
        }
        ASSERT_EQ( iCounter, 2 );
    }
}
