
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
#include "constructions.hpp"

#include "schematic/analysis/analysis.hpp"

#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/create_straight_skeleton_from_polygon_with_holes_2.h>
#include <CGAL/create_offset_polygons_from_polygon_with_holes_2.h>
#include <CGAL/create_offset_polygons_2.h>

#include <vector>
#include <tuple>

namespace exact
{
/*
void Analysis::skeleton()
{
    HalfEdgeCstPolygonWithHoles::Vector floors;

    getPolyonsWithHoles(
        m_arr,
        // OuterEdgePredicate
        []( Arrangement::Halfedge_const_handle edge )
        { return test( edge, EdgeMask::ePerimeter ) || test( edge, EdgeMask::ePartitionFloor ); },
        // InnerEdgePredicate
        []( Arrangement::Halfedge_const_handle edge )
        { return test( edge, EdgeMask::ePartitionFloor ) && !test( edge, EdgeMask::ePerimeterBoundary ); },
        floors );

    INVARIANT( floors.size() == 1, "Failed to locate single floor" );
    const exact::Polygon_with_holes polygonWithHoles = fromHalfEdgePolygonWithHoles( floors.front() );

    using StraightSkeleton    = CGAL::Straight_skeleton_2< exact::Kernel >;
    using StraightSkeletonPtr = boost::shared_ptr< StraightSkeleton >;

    StraightSkeletonPtr pStraightSkeleton = CGAL::create_interior_straight_skeleton_2(
        polygonWithHoles.outer_boundary().begin(), polygonWithHoles.outer_boundary().end(),
        polygonWithHoles.holes_begin(), polygonWithHoles.holes_end(), exact::Kernel() );

    for( auto i = pStraightSkeleton->halfedges_begin(), iEnd = pStraightSkeleton->halfedges_end(); i != iEnd; ++i )
    {
        StraightSkeleton::Halfedge_handle h = i;
        if( h->is_inner_bisector() )
        {
            renderCurve(
                m_arr, Curve( h->vertex()->point(), h->next()->vertex()->point() ), EdgeMask::eSkeletonInnerBisector );
        }
        else if( h->is_bisector() )
        {
            renderCurve(
                m_arr, Curve( h->vertex()->point(), h->next()->vertex()->point() ), EdgeMask::eSkeletonBisector );
        }
    }

    using ExtrusionSpec = std::tuple< double, EdgeMask::Type, EdgeMask::Type >;
    static constexpr std::array< ExtrusionSpec, 4 > extrusions
        = { ExtrusionSpec{ 0.5, EdgeMask::eExtrusionOne, EdgeMask::eExtrusionOneBoundary },
            ExtrusionSpec{ 1.0, EdgeMask::eExtrusionTwo, EdgeMask::eExtrusionTwoBoundary },
            ExtrusionSpec{ 2.0, EdgeMask::eExtrusionThree, EdgeMask::eExtrusionThreeBoundary },
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
                renderCurve( m_arr, Curve( Point( i->x(), i->y() ), Point( iNext->x(), iNext->y() ) ), mask,
                             maskBoundary, {}, {} );
            }
        }
    }
}*/

} // namespace exact
