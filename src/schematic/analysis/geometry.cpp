
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

#include "geometry.hpp"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/segment.hpp>
#include <boost/geometry/index/rtree.hpp>

namespace exact
{

int pointLineSegmentDistance( const Value& point, const Value& lineStart, const Value& lineEnd )
{
    using IntPoint = boost::geometry::model::point< int, 2, boost::geometry::cs::cartesian >;

    // does the line segment face the point?
    const IntPoint v1{ lineStart.x, lineStart.y };
    const IntPoint v2{ lineEnd.x, lineEnd.y };
    const IntPoint pt{ point.x, point.y };

    // clang-format off
    const IntPoint e1{
        v2.get< 0 >() - v1.get< 0 >(),
        v2.get< 1 >() - v1.get< 1 >() };

    const IntPoint e2{
        pt.get< 0 >() - v1.get< 0 >(),
        pt.get< 1 >() - v1.get< 1 >() };

    const double r1 = boost::geometry::dot_product( e1, e1 );
    const double r2 = boost::geometry::dot_product( e1, e2 );

    if( ( r2 > 0.0 ) && ( r2 < r1 ) )
    {
        // get squared length of e1
        const double len2 = ( e1.get<0>() * e1.get<0>() ) + ( e1.get<1>() * e1.get<1>() );

        const IntPoint pointOnSegment
        {
            static_cast< int >( static_cast< double >( v1.get< 0 >() ) + (r2 * static_cast< double >( e1.get<0>() ) ) / len2 ),
            static_cast< int >( static_cast< double >( v1.get< 1 >() ) + (r2 * static_cast< double >( e1.get<1>() ) ) / len2 )
        };

        // calculate the manhatten distance
        return abs( pointOnSegment.get<0>() - pt.get<0>() ) + abs( pointOnSegment.get<1>() - pt.get<1>() );
    }
    else
    {
        return std::min( 
            abs( pt.get<0>() - v1.get<0>() ) + abs( pt.get<1>() - v1.get<1>() ),
            abs( pt.get<0>() - v2.get<0>() ) + abs( pt.get<1>() - v2.get<1>() )
            );
    }
}
}

