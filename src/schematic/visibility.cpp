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

#include "schematic/visibility.hpp"
#include "schematic/svgUtils.hpp"
#include "schematic/object.hpp"

#include "CGAL/Arr_landmarks_point_location.h"
/*
namespace
{
    void renderFloorFace( schematic::Arrangement& arr, schematic::Arrangement::Face_const_handle hFace )
    {
        if( !hFace->is_unbounded() )
        {
            schematic::Arrangement::Ccb_halfedge_const_circulator iter = hFace->outer_ccb();
            schematic::Arrangement::Ccb_halfedge_const_circulator start = iter;
            do
            {
                //test if the edge is a doorstep
                if( !iter->data().get() )
                {
                    CGAL::insert( arr,
                        schematic::Curve( iter->source()->point(),
                                              iter->target()->point() ) );
                }
                ++iter;
            }
            while( iter != start );
        }

        //search through all holes
        for( schematic::Arrangement::Hole_const_iterator
            holeIter = hFace->holes_begin(),
            holeIterEnd = hFace->holes_end();
                holeIter != holeIterEnd; ++holeIter )
        {
            schematic::Arrangement::Ccb_halfedge_const_circulator iter = *holeIter;
            schematic::Arrangement::Ccb_halfedge_const_circulator start = iter;
            do
            {
                //test if the edge is a doorstep
                if( !iter->data().get() )
                {
                    CGAL::insert( arr,
                        schematic::Curve( iter->source()->point(),
                                              iter->target()->point() ) );
                }
                ++iter;
            }
            while( iter != start );
        }
    }

    class ArrObserver : public CGAL::Arr_observer< schematic::Arrangement >
    {
    public:
        ArrObserver( schematic::Arrangement& arr )
        :     CGAL::Arr_observer< schematic::Arrangement >( arr )
        {
        }

        virtual void after_split_face(
            schematic::Arrangement::Face_handle f,
            schematic::Arrangement::Face_handle new_f, bool is_hole )
        {
            new_f->set_data( f->data() );
        }
    };
}

namespace schematic
{


//class FloorAnalysis::FloorTest : public CGAL::Arr_landmarks_point_location< Arrangement >
//{
//public:
//    using VertexHandle =
//        Arrangement::Vertex_const_handle;
//
//    FloorTest( const Arrangement& arr )
//        :   Arr_landmarks_point_location< Arrangement >( arr )
//    {
//
//    }
//
//    bool testSegment( VertexHandle v1, VertexHandle v2 )
//    {
//        Halfedge_set crossed_edges;
//
//        result_type result =
//            _walk_from_vertex( v1, v2->point(), crossed_edges );
//
//        VERIFY_RTE( boost::get< VertexHandle >( result ) == v2 );
//
//        return crossed_edges.empty();
//    }
//
//    ::schematic::Segment getBisector( VertexHandle v1, VertexHandle v2)
//    {
//        ::schematic::Segment result( v1->point(), v2->point() );
//        return result;
//    }
//};

FloorAnalysis::FloorAnalysis()
    :   m_hFloorFace( nullptr )
{
}

FloorAnalysis::FloorAnalysis( Compilation& compilation, boost::shared_ptr< Schematic > pSchematic )
    :   m_hFloorFace( nullptr )
{
    Compilation::FaceHandleSet floorFaces;
    Compilation::FaceHandleSet fillerFaces;
    compilation.getFaces( floorFaces, fillerFaces );

    for( Compilation::FaceHandle hFace : floorFaces )
    {
        renderFloorFace( m_arr, hFace );
    }

    for( Site::Ptr pNestedSite : pSchematic->getSites() )
    {
        recurseObjects( pNestedSite );
    }

    findFloorFace();

    m_hFloorFace->set_data( (DefaultedBool( true )) );

    VERIFY_RTE( m_arr.is_valid() );

    //calculate the bounding box
    calculateBounds();
}

void FloorAnalysis::findFloorFace()
{
    int iFloorFaceCount = 0;
    Arrangement::Face_handle hOuterFace = m_arr.unbounded_face();
    for( Arrangement::Hole_iterator
        holeIter = hOuterFace->holes_begin(),
        holeIterEnd = hOuterFace->holes_end();
            holeIter != holeIterEnd; ++holeIter )
    {
        Arrangement::Ccb_halfedge_circulator iter = *holeIter;
        m_hFloorFace = iter->twin()->face();
        ++iFloorFaceCount;
    }
    VERIFY_RTE_MSG( iFloorFaceCount == 1, "Invalid number of floors: " << iFloorFaceCount );

    VERIFY_RTE_MSG( !m_hFloorFace->is_unbounded(), "Floor face is unbounded" );

    //paranoia check
    {
        for( auto i = m_arr.faces_begin(),
            iEnd = m_arr.faces_end(); i!=iEnd; ++i )
        {
            if( i != m_hFloorFace )
            {
                VERIFY_RTE( !i->data().get() );
            }
        }
    }
}

void FloorAnalysis::calculateBounds()
{
    std::vector< Segment > outerSegments;
    Arrangement::Ccb_halfedge_const_circulator iter = m_hFloorFace->outer_ccb();
    Arrangement::Ccb_halfedge_const_circulator start = iter;
    do
    {
        outerSegments.push_back( iter->curve() );
        ++iter;
    }
    while( iter != start );

    m_boundingBox = CGAL::bbox_2( outerSegments.begin(), outerSegments.end() );
}

void FloorAnalysis::recurseObjects( Site::Ptr pSite )
{
    if( Object::Ptr pObject = boost::dynamic_pointer_cast< Object >( pSite ) )
    {
        Compilation::renderContour( m_arr,
            pObject->getAbsoluteTransform(),
            pObject->getContourPolygon() );
    }
    //else if( Wall::Ptr pWall = boost::dynamic_pointer_cast< Wall >( pSite ) )
    //{
    //
    //}

    for( Site::Ptr pNestedSite : pSite->getSites() )
    {
        recurseObjects( pNestedSite );
    }
}

inline bool isInFloor( Arrangement::Halfedge_const_handle h )
{
    const bool bIsFloorEdge =
        h->face()->data().get() ||
        h->twin()->face()->data().get();
    return bIsFloorEdge;
}

bool FloorAnalysis::isWithinFloor( VertexHandle v1, VertexHandle v2 ) const
{
    ArrObserver observer( m_arr );

    Curve_handle hCurve =
        CGAL::insert( m_arr, Curve( v1->point(), v2->point() ) );

    bool bIsOnlyFloorFaces = true;
    for( auto i = m_arr.induced_edges_begin( hCurve ),
              iEnd = m_arr.induced_edges_end( hCurve );
              i != iEnd; ++i )
    {
        Arrangement::Halfedge_handle h = *i;
        if( !isInFloor( h ) )
        {
            bIsOnlyFloorFaces = false;
            break;
        }
    }

    CGAL::remove_curve( m_arr, hCurve );

    return bIsOnlyFloorFaces;
}

Segment makeBisector( const Segment& edge, const Rect& rect )
{
    const Line line( edge.source(), edge.target() );

    VERIFY_RTE_MSG( CGAL::do_intersect( rect, line ),
        "bisector does not intersect bounds" );

    boost::optional< boost::variant< Point, Segment > >
        result = CGAL::intersection< Kernel >( rect, line );
    VERIFY_RTE_MSG( result, "Bisector intersection failed" );

    return boost::get< Segment >( result.get() );
}

boost::optional< Curve > FloorAnalysis::getFloorBisector( VertexHandle v1, VertexHandle v2, bool bKeepSingleEnded )
const
{
    const Segment segment( v1->point(), v2->point() );
    boost::optional< Curve > result;

    if( segment.squared_length() > 0.0 )
    {
        const Segment bisector = makeBisector( segment, m_boundingBox );

        ArrObserver observer( m_arr );

        Curve_handle hCurveSegment =
            CGAL::insert( m_arr, Curve( segment.source(), segment.target() ) );

        Curve_handle hCurve =
            CGAL::insert( m_arr, Curve( bisector.source(), bisector.target() ) );

        using HalfEdgeHandle = Arrangement::Halfedge_const_handle;
        using HalfEdgeHandleSet = std::set< HalfEdgeHandle >;

        HalfEdgeHandleSet edgesSet;
        {
            for( auto i = m_arr.induced_edges_begin( hCurveSegment ),
                      iEnd = m_arr.induced_edges_end( hCurveSegment );
                      i != iEnd; ++i )
            {
                edgesSet.insert( *i );
            }
            for( auto i = m_arr.induced_edges_begin( hCurve ),
                      iEnd = m_arr.induced_edges_end( hCurve );
                      i != iEnd; ++i )
            {
                edgesSet.insert( *i );
            }
        }

        using HalfEdgeHandleVector = std::vector< HalfEdgeHandle >;
        HalfEdgeHandleVector orderedEdges( edgesSet.begin(), edgesSet.end() );
        {
            std::transform( orderedEdges.begin(), orderedEdges.end(), orderedEdges.begin(),
                [ bisector]( HalfEdgeHandle edge ) -> HalfEdgeHandle
                {
                    if( CGAL::has_smaller_distance_to_point(
                        bisector.source(),
                        edge->source()->point(),
                        edge->target()->point() ) )
                    {
                        return edge;
                    }
                    else
                    {
                        return edge->twin();
                    }
                });

            std::sort( orderedEdges.begin(), orderedEdges.end(),
                [ bisector ]( HalfEdgeHandle left, HalfEdgeHandle right )
                {
                    return CGAL::has_smaller_distance_to_point(
                        bisector.source(),
                        left->source()->point(),
                        right->source()->point() );
                } );
        }

        //find region within original segment
        HalfEdgeHandleVector left, inside, right;
        {
            int iState = 0;
            for( HalfEdgeHandleVector::iterator
                    i = orderedEdges.begin(),
                    iEnd = orderedEdges.end(); i!=iEnd; ++i )
            {
                HalfEdgeHandle h = *i;
                VERIFY_RTE( h->source()->point() != h->target()->point() );
                switch( iState )
                {
                    case 0:
                        if( h->source()->point() == segment.source() )
                        {
                            iState = 1;
                            inside.push_back( h );
                        }
                        else
                        {
                            left.push_back( h );
                        }
                        break;
                    case 1:
                        if( h->source()->point() == segment.target() )
                        {
                            iState = 2;
                            right.push_back( h );
                        }
                        else
                        {
                            inside.push_back( h );
                        }
                        break;
                    case 2:
                        {
                            right.push_back( h );
                        }
                        break;
                }
            }
        }
        std::ostringstream os;
        //{
        //    for( HalfEdgeHandleVector::iterator
        //            i = orderedEdges.begin(),
        //            iEnd = orderedEdges.end(); i!=iEnd; ++i )
        //    {
        //        HalfEdgeHandle h = *i;
        //        os << "halfedge: " << h->source()->point() << " : " << h->target()->point() << "\n";
        //    }
        //}
        VERIFY_RTE_MSG( !left.empty() || ( segment.source() == bisector.source() ),
            "Empty left segment: " << segment.source() << " bisector: " << bisector.source() << "\n" << os.str() );
        VERIFY_RTE_MSG( !inside.empty(),
            "Empty inside segment: " << segment << " bisector: " << bisector << "\n" << os.str() );
        VERIFY_RTE_MSG( !right.empty() || ( segment.target() == bisector.target() ),
            "Empty right segment: " << segment.target() << " bisector: " << bisector.target() << "\n" << os.str() );

        //test the inside is full within the floor
        bool bInvalid = true;
        for( HalfEdgeHandleVector::iterator
                i = inside.begin(),
                iEnd = inside.end(); i!=iEnd; ++i )
        {
            HalfEdgeHandle h = *i;
            if( !isInFloor( h ) )
            {
                bInvalid = true;
                break;
            }
        }

        if( bInvalid )
        {
            //find the last edges within floor on both sides
            HalfEdgeHandle hFirst = inside.front(), hLast = inside.back();
            for( HalfEdgeHandleVector::reverse_iterator
                    i = left.rbegin(),
                    iEnd = left.rend(); i!=iEnd; ++i )
            {
                HalfEdgeHandle h = *i;
                if( isInFloor( h ) )
                {
                    hFirst = h;
                }
                else
                {
                    break;
                }
            }

            for( HalfEdgeHandleVector::iterator
                    i = right.begin(),
                    iEnd = right.end(); i!=iEnd; ++i )
            {
                HalfEdgeHandle h = *i;
                if( isInFloor( h ) )
                {
                    hLast = h;
                }
                else
                {
                    break;
                }
            }

            //insiste on the bisector extending beyond the originating segment
            if( bKeepSingleEnded )
            {
                if( ( hFirst != inside.front() ) || ( hLast != inside.back() ) )
                {
                    result = Curve( hFirst->source()->point(), hLast->target()->point() );
                }
            }
            else
            {
                if( ( hFirst != inside.front() ) && ( hLast != inside.back() ) )
                {
                    result = Curve( hFirst->source()->point(), hLast->target()->point() );
                }
            }
        }

        CGAL::remove_curve( m_arr, hCurve );
        CGAL::remove_curve( m_arr, hCurveSegment );
    }


    return result;
}

boost::optional< Curve > FloorAnalysis::getFloorBisector( const Segment& segment, bool bKeepSingleEnded ) const
{
    VertexHandle v1, v2;
    bool bFoundSource = false, bFoundTarget = false;
    for( auto i = m_arr.vertices_begin(),
        iEnd = m_arr.vertices_end(); i!=iEnd; ++i )
    {
        if( i->point() == segment.source() )
        {
            VERIFY_RTE( !bFoundSource );
            v1 = i;
            bFoundSource = true;
        }
        if( i->point() == segment.target() )
        {
            VERIFY_RTE( !bFoundTarget );
            v2 = i;
            bFoundTarget = true;
        }
    }
    VERIFY_RTE( bFoundSource );
    VERIFY_RTE( bFoundTarget );
    VERIFY_RTE( v1 != v2 );

    return getFloorBisector( v1, v2, bKeepSingleEnded );
}

void FloorAnalysis::render( const boost::filesystem::path& filepath )
{
    EdgeVectorVector edgeGroups;

    {
        EdgeVector edges;
        Arrangement::Ccb_halfedge_const_circulator iter = m_hFloorFace->outer_ccb();
        Arrangement::Ccb_halfedge_const_circulator start = iter;
        do
        {
            edges.push_back( iter );
            ++iter;
        }
        while( iter != start );
        edgeGroups.push_back( edges );
    }

    {
        for( Arrangement::Hole_const_iterator
            holeIter = m_hFloorFace->holes_begin(),
            holeIterEnd = m_hFloorFace->holes_end();
                holeIter != holeIterEnd; ++holeIter )
        {
            EdgeVector edges;
            Arrangement::Ccb_halfedge_const_circulator iter = *holeIter;
            Arrangement::Ccb_halfedge_const_circulator start = iter;
            do
            {
                edges.push_back( iter );
                ++iter;
            }
            while( iter != start );
            if( !edges.empty() )
                edgeGroups.push_back( edges );
        }
    }

    SVGStyle style;
    generateHTML( filepath, m_arr, edgeGroups, style );
}

void FloorAnalysis::save( std::ostream& os ) const
{
    Formatter formatter;
    CGAL::write( m_arr, os, formatter );
}

void FloorAnalysis::load( std::istream& is )
{
    Formatter formatter;
    CGAL::read( m_arr, is, formatter );

    findFloorFace();

    //VERIFY_RTE( m_arr.is_valid() );

    calculateBounds();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
Visibility::Visibility()
{

}

Visibility::Visibility( FloorAnalysis& floor )
{
    Arrangement::Face_const_handle hFloor = floor.getFloorFace();

    std::vector< Curve > segments;

    using VertexVector =
        std::vector< Arrangement::Vertex_const_handle >;
    VertexVector interiorPoints, allPoints;

    {
        Arrangement::Ccb_halfedge_const_circulator iter = hFloor->outer_ccb();
        Arrangement::Ccb_halfedge_const_circulator start = iter;
        do
        {
            segments.push_back( Curve(  iter->source()->point(),
                                            iter->target()->point() ) );

            Arrangement::Ccb_halfedge_const_circulator prev = iter;
            ++iter;

            const CGAL::Orientation turn =
                CGAL::orientation(
                    prev->source()->point(),
                    prev->target()->point(),
                    iter->target()->point() );
            if( turn == CGAL::NEGATIVE )
            {
                interiorPoints.push_back( prev->target() );
            }
            allPoints.push_back( prev->target() );
        }
        while( iter != start );
    }

    {
        for( Arrangement::Hole_const_iterator
            holeIter = hFloor->holes_begin(),
            holeIterEnd = hFloor->holes_end();
                holeIter != holeIterEnd; ++holeIter )
        {
            Arrangement::Ccb_halfedge_const_circulator iter = *holeIter;
            Arrangement::Ccb_halfedge_const_circulator start = iter;
            do
            {
                segments.push_back( Curve(  iter->source()->point(),
                                                iter->target()->point() ) );

                Arrangement::Ccb_halfedge_const_circulator prev = iter;
                ++iter;

                const CGAL::Orientation turn =
                    CGAL::orientation(
                        prev->source()->point(),
                        prev->target()->point(),
                        iter->target()->point() );
                if( turn == CGAL::NEGATIVE )
                {
                    interiorPoints.push_back( prev->target() );
                }
                allPoints.push_back( prev->target() );
            }
            while( iter != start );
        }
    }

    for( const Curve& segment : segments )
    {
        CGAL::insert( m_arr, segment );
    }

    for( const Curve& segment : segments )
    {
        if( boost::optional< Curve > bisectorOpt =
            floor.getFloorBisector( segment, true ) )
        {
            CGAL::insert( m_arr, bisectorOpt.get() );
        }
    }

    //determine the set of interior vertices VertexVector
    auto    i       = interiorPoints.begin(),
            iEnd    = interiorPoints.end() - 1U,
            j       = interiorPoints.begin(),
            jEnd    = interiorPoints.end();

    for( ; i != iEnd; ++i )
    {
        for( j = i + 1U; j != jEnd; ++j )
        {
            Arrangement::Vertex_const_handle v1 = *i;
            Arrangement::Vertex_const_handle v2 = *j;
            if( v1 != v2 )
            {
                if( floor.isWithinFloor( v1, v2 ) )
                {
                    if( boost::optional< Curve > bisectorOpt =
                        floor.getFloorBisector( v1, v2, false ) )
                    {
                        CGAL::insert( m_arr, bisectorOpt.get() );
                    }
                }
            }

        }
    }
}

void Visibility::render( const boost::filesystem::path& filepath )
{
    EdgeVectorVector edgeGroups;
    std::vector< Arrangement::Halfedge_const_handle > edges;
    for( auto i = m_arr.edges_begin(); i != m_arr.edges_end(); ++i )
        edges.push_back( i );
    edgeGroups.push_back( edges );

    SVGStyle style;
    {
        style.bDots = false;
        style.bArrows = false;
    }
    generateHTML( filepath, m_arr, edgeGroups, style );
}

void Visibility::save( std::ostream& os ) const
{
    Formatter formatter;
    CGAL::write( m_arr, os, formatter );
}

void Visibility::load( std::istream& is )
{
    Formatter formatter;
    CGAL::read( m_arr, is, formatter );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
Analysis::Analysis()
{
}

Analysis::Analysis( boost::shared_ptr< Schematic > pSchematic )
    :   m_compilation( pSchematic ),
        m_floor( m_compilation, pSchematic ),
        m_visibility( m_floor )
{

}

Analysis::Ptr Analysis::constructFrommission( boost::shared_ptr< Schematic > pSchematic )
{
    Analysis::Ptr pAnalysis( new Analysis( pSchematic ) );
    return pAnalysis;
}

Analysis::Ptr Analysis::constructFromStream( std::istream& is )
{
    Analysis::Ptr pAnalysis( new Analysis );

    pAnalysis->m_compilation.load( is );
    pAnalysis->m_floor.load( is );
    pAnalysis->m_visibility.load( is );

    return pAnalysis;
}

void Analysis::save( std::ostream& os ) const
{
    m_compilation.save( os );
    m_floor.save( os );
    m_visibility.save( os );
}

void Analysis::renderFloor( IPainter& painter ) const
{
    const Arrangement& floor = m_floor.getFloor();
    for( auto i = floor.edges_begin(); i != floor.edges_end(); ++i )
    {
        Arrangement::Halfedge_const_handle h = i;

        painter.moveTo(
            CGAL::to_double( h->source()->point().x() ),
            CGAL::to_double( h->source()->point().y() ) );

        painter.lineTo(
            CGAL::to_double( h->target()->point().x() ),
            CGAL::to_double( h->target()->point().y() ) );
    }
}

}
*/