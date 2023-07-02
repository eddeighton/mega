
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

#ifndef GUARD_2023_June_14_polygon_with_holes
#define GUARD_2023_June_14_polygon_with_holes

#include "invariant.hpp"

#include "schematic/cgalSettings.hpp"

#include <vector>
#include <algorithm>

namespace exact
{

template < typename HalfEdgeType >
std::vector< HalfEdgeType > reverse( const std::vector< HalfEdgeType >& path )
{
    std::vector< HalfEdgeType > result;
    for( auto e : path )
    {
        result.push_back( e->twin() );
    }
    std::reverse( result.begin(), result.end() );
    return result;
}

template < typename VertexType, typename HalfEdgeType >
std::vector< VertexType > filterColinearEdges( const std::vector< HalfEdgeType >& edges )
{
    std::vector< VertexType > result;

    if( edges.size() <= 2 )
    {
        if( edges.empty() )
        {
            // do nothing
        }
        else if( edges.size() == 1 )
        {
            result.push_back( edges.back()->source() );
            result.push_back( edges.back()->target() );
        }
        else
        {
            INVARIANT( edges.back()->target() != edges.front()->source(), "Edge sequence of only two edges in loop" );
            // always add the start and end
            result.push_back( edges.front()->source() );
            // but only add the middle if NOT colinear
            switch( CGAL::orientation(
                edges.front()->source()->point(), edges.back()->source()->point(), edges.back()->target()->point() ) )
            {
                case CGAL::RIGHT_TURN:
                case CGAL::LEFT_TURN:
                    result.push_back( edges.front()->target() );
                    break;
                case CGAL::COLLINEAR:
                default:
                    break;
            }
            result.push_back( edges.back()->target() );
        }
    }
    else if( edges.back()->target() == edges.front()->source() )
    {
        bool bFoundLoop = false;
        for( auto i = edges.end() - 1, iNext = i; !bFoundLoop; i = iNext )
        {
            // skip colinear points
            bool bFoundTurn = false;
            while( !bFoundTurn && !bFoundLoop )
            {
                ++iNext;
                if( iNext == edges.end() )
                {
                    iNext = edges.begin();
                }
                switch( CGAL::orientation(
                    ( *i )->source()->point(), ( *iNext )->source()->point(), ( *iNext )->target()->point() ) )
                {
                    case CGAL::RIGHT_TURN:
                    case CGAL::LEFT_TURN:
                        if( !result.empty() && ( result.front() == ( *iNext )->source() ) )
                        {
                            bFoundLoop = true;
                        }
                        else
                        {
                            result.push_back( ( *iNext )->source() );
                        }
                        bFoundTurn = true;
                        break;
                    case CGAL::COLLINEAR:
                    default:
                        break;
                }
            }
        }
    }
    else
    {
        // if not always start at start vertex
        auto iPrev = edges.end();
        for( auto i = edges.begin(), iEnd = edges.end(); i != iEnd; ++i )
        {
            if( iPrev == edges.end() )
            {
                iPrev = edges.begin();
                result.push_back( ( *i )->source() );
            }
            else
            {
                switch( CGAL::orientation(
                    ( *iPrev )->source()->point(), ( *i )->source()->point(), ( *i )->target()->point() ) )
                {
                    case CGAL::RIGHT_TURN:
                    case CGAL::LEFT_TURN:
                        result.push_back( ( *i )->source() );
                        iPrev = i;
                        break;
                    case CGAL::COLLINEAR:
                    default:
                        break;
                }
            }
        }
        result.push_back( edges.back()->target() );
    }

    return result;
}

template < typename HalfEdgeType >
struct HalfEdgePolygonWithHolesT
{
    std::vector< HalfEdgeType >                outer;
    std::vector< std::vector< HalfEdgeType > > holes;
    using Vector = std::vector< HalfEdgePolygonWithHolesT >;
};

template < typename HalfEdgeType >
static inline exact::Polygon fromHalfEdgePolygon( const std::vector< HalfEdgeType >& poly )
{
    exact::Polygon outer;
    for( auto& e : poly )
    {
        outer.push_back( e->source()->point() );
    }
    // ensure it is a not a hole
    if( !outer.is_counterclockwise_oriented() )
    {
        std::reverse( outer.begin(), outer.end() );
    }
    return outer;
}

template < typename HalfEdgeType >
static inline exact::Polygon_with_holes
fromHalfEdgePolygonWithHoles( const HalfEdgePolygonWithHolesT< HalfEdgeType >& poly )
{
    exact::Polygon_with_holes polygonWithHoles( fromHalfEdgePolygon< HalfEdgeType >( poly.outer ) );
    for( auto& h : poly.holes )
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
    return polygonWithHoles;
}

template < typename VertexType >
static inline exact::Polygon fromVertexPolygon( const std::vector< VertexType >& poly, bool ccw = true )
{
    exact::Polygon outer;
    for( auto& e : poly )
    {
        outer.push_back( e->point() );
    }
    // ensure it is a not a hole
    if( ccw != outer.is_counterclockwise_oriented() )
    {
        std::reverse( outer.begin(), outer.end() );
    }
    return outer;
}

template < typename VertexType, typename HalfEdgeType >
static inline exact::Polygon_with_holes
fromHalfEdgePolygonWithHolesFiltered( const HalfEdgePolygonWithHolesT< HalfEdgeType >& poly )
{
    exact::Polygon_with_holes polygonWithHoles(
        fromVertexPolygon< VertexType >( exact::filterColinearEdges< VertexType, HalfEdgeType >( poly.outer ) ) );
    for( auto& hole : poly.holes )
    {
        polygonWithHoles.add_hole(
            fromVertexPolygon( exact::filterColinearEdges< VertexType, HalfEdgeType >( hole ), false ) );
    }
    return polygonWithHoles;
}

} // namespace exact

#endif // GUARD_2023_June_14_polygon_with_holes
