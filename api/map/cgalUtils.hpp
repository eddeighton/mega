
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

#ifndef SPACEUTILS_15_09_2013
#define SPACEUTILS_15_09_2013

#include "map/cgalSettings.hpp"
#include "map/site.hpp"

namespace map::Utils
{

template < typename KernelTo, typename KernelFrom >
inline CGAL::Polygon_2< KernelTo > convert( const CGAL::Polygon_2< KernelFrom >& polyFrom )
{
    CGAL::Cartesian_converter< KernelFrom, KernelTo > converter;

    CGAL::Polygon_2< KernelTo > result;
    for( const typename KernelFrom::Point_2& p : polyFrom )
    {
        result.push_back( converter( p ) );
    }
    return result;
}

Polygon getDefaultPolygon();

Point getClosestPointOnSegment( const Segment& segment, const Point& pt );

std::size_t getClosestPoint( const Polygon& poly, const Point& pt );
void        getSelectionBounds( const std::vector< Site* >& sites, Rect& transformBounds );
void        getSelectionBounds( const Site::PtrVector& sites, Rect& transformBounds );

Polygon getConvexHull( const Site::PtrVector& sites );

exact::Polygon filterPolygon( const exact::Polygon& poly );

} // namespace map::Utils

#endif // SPACEUTILS_15_09_2013