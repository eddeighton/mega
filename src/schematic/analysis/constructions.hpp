
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

#ifndef GUARD_2023_June_17_constructions
#define GUARD_2023_June_17_constructions

#include "schematic/analysis/edge_mask.hpp"
#include "schematic/analysis/invariant.hpp"
#include "schematic/analysis/analysis.hpp"

#include <CGAL/ch_graham_andrew.h>

namespace exact
{
    
inline void renderCurve( Analysis::Arrangement& arr, const exact::Curve& curve, EdgeMask::Type innerMask,
                         EdgeMask::Type outerMask = EdgeMask::TOTAL_MASK_TYPES )
{
    Analysis::Arrangement::Curve_handle firstCurve = CGAL::insert( arr, curve );
    for( auto h = arr.induced_edges_begin( firstCurve ); h != arr.induced_edges_end( firstCurve ); ++h )
    {
        Analysis::Arrangement::Halfedge_handle edge = *h;

        const bool bSameDir = firstCurve->is_directed_right()
                                  ? ( edge->direction() == CGAL::Arr_halfedge_direction::ARR_LEFT_TO_RIGHT )
                                  : ( edge->direction() == CGAL::Arr_halfedge_direction::ARR_RIGHT_TO_LEFT );

        if( bSameDir )
        {
            classify( edge, innerMask );
            if( outerMask != EdgeMask::TOTAL_MASK_TYPES )
            {
                classify( edge->twin(), outerMask );
            }
        }
        else
        {
            classify( edge->twin(), innerMask );
            if( outerMask != EdgeMask::TOTAL_MASK_TYPES )
            {
                classify( edge, outerMask );
            }
        }
    }
}

inline void renderExtrudedCurve( Analysis::Arrangement& arr, const exact::Curve& curve,
                                 const exact::Polygon& convexShape, EdgeMask::Type innerMask, EdgeMask::Type outerMask )
{
    const Vector vSource = curve.source() - Point{ 0.0, 0.0 };
    const Vector vTarget = curve.target() - Point{ 0.0, 0.0 };

    // generate all points of convex shape around curve end points
    std::vector< Point > points;
    for( const auto& p : convexShape )
    {
        points.emplace_back( p + vSource );
        points.emplace_back( p + vTarget );
    }

    // calculate the monotone convex hull of result
    std::vector< Point > hull;
    CGAL::ch_graham_andrew( points.begin(), points.end(), std::back_inserter( hull ) );

    // render the convex hull line segments
    for( auto i = hull.begin(), iNext = hull.begin(), iEnd = hull.end(); i != iEnd; ++i )
    {
        ++iNext;
        if( iNext == iEnd )
        {
            iNext = hull.begin();
        }
        renderCurve( arr, Curve( *i, *iNext ), innerMask, outerMask );
    }
}

inline void renderCurve( Analysis::Arrangement& arr, const exact::Curve& curve, EdgeMask::Type innerMask,
                         EdgeMask::Type outerMask, schematic::Site::PtrCst pInnerSite,
                         schematic::Site::PtrCst pOuterSite )
{
    Analysis::Arrangement::Curve_handle firstCurve = CGAL::insert( arr, curve );
    for( auto h = arr.induced_edges_begin( firstCurve ); h != arr.induced_edges_end( firstCurve ); ++h )
    {
        Analysis::Arrangement::Halfedge_handle edge = *h;

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

inline void renderContour( Analysis::Arrangement& arr, const exact::Transform& transform,
                           const exact::Polygon& polyOriginal, EdgeMask::Type innerMask, EdgeMask::Type outerMask,
                           schematic::Site::PtrCst pInnerSite, schematic::Site::PtrCst pOuterSite )
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
        renderCurve( arr, exact::Curve( *i, *iNext ), innerMask, outerMask, pInnerSite, pOuterSite );
    }
}

} // namespace exact

#endif // GUARD_2023_June_17_constructions
