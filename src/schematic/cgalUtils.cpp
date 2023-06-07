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

#include "schematic/cgalUtils.hpp"

#include <limits>

namespace schematic::Utils
{

Polygon getDefaultPolygon()
{
    Polygon defaultPolygon;
    {
        const double d = 16.0;
        defaultPolygon.push_back( Point( -d, -d ) );
        defaultPolygon.push_back( Point( d, -d ) );
        defaultPolygon.push_back( Point( d, d ) );
        defaultPolygon.push_back( Point( -d, d ) );
    }
    return defaultPolygon;
}

exact::Point getClosestPointOnSegment( const exact::Segment& segment, const exact::Point& pt )
{
    exact::Kernel::Construct_projected_point_2 project;

    const exact::Point projectedPoint = project( segment.supporting_line(), pt );

    if( segment.has_on( projectedPoint ) )
    {
        return projectedPoint;
    }
    else
    {
        if( CGAL::compare_distance_to_point( projectedPoint, segment[ 0 ], segment[ 1 ] ) == CGAL::SMALLER )
        {
            return segment[ 0 ];
        }
        else
        {
            return segment[ 1 ];
        }
    }
}

std::size_t getClosestPoint( const Polygon& poly, const Point& pt )
{
    if( poly.size() < 2U )
        return 0U;

    exact::InexactToExact converter;
    exact::ExactToInexact converterBack;

    // for each line segment find the closest point to the input point and record distance
    const std::size_t     szSize = poly.size();
    std::vector< double > distances;
    {
        for( std::size_t sz = 0U; sz != szSize; ++sz )
        {
            const Point& ptCur  = poly[ sz ];
            const Point& ptNext = poly[ ( sz + 1U ) % szSize ];

            if( CGAL::EQUAL == CGAL::compare_xy( ptNext, ptCur ) )
            {
                const double db = ( ptNext - pt ).squared_length();
                distances.push_back( db );
            }
            else
            {
                const Segment      segment( ptCur, ptNext );
                const exact::Point ptClosest = getClosestPointOnSegment( converter( segment ), converter( pt ) );
                const double       db        = ( converterBack( ptClosest ) - pt ).squared_length();
                distances.push_back( db );
            }
        }
    }

    // now find the smallest distance
    std::size_t uiLowest = 0U;
    {
        double dbLowest = std::numeric_limits< double >::max();
        for( std::size_t sz = 0U; sz != szSize; ++sz )
        {
            const double db = distances[ sz ];
            if( db < dbLowest )
            {
                dbLowest = db;
                uiLowest = sz;
            }
        }
    }
    return uiLowest;
}

void getSelectionBounds( const std::vector< Site* >& sites, Rect& transformBounds )
{
    std::vector< Rect > contours;
    for( const Site* pSite : sites )
    {
        // calcualte aabb instead of requiring the contour polygon
        const Rect boundingRect = pSite->getAABB();
        contours.push_back( boundingRect.transform( pSite->getTransform() ) );
    }
    if( !contours.empty() )
    {
        transformBounds = CGAL::bbox_2( contours.begin(), contours.end() );
    }
}

void getSelectionBounds( const Site::PtrVector& sites, Rect& transformBounds )
{
    std::vector< Site* > sites_;
    for( Site::Ptr pSite : sites )
    {
        sites_.push_back( pSite.get() );
    }
    getSelectionBounds( sites_, transformBounds );
}

exact::Polygon filterPolygon( const exact::Polygon& poly )
{
    if( poly.size() > 2U )
    {
        exact::Polygon filteredPoly = poly;
        bool           bFound       = true;
        while( bFound )
        {
            bFound                   = false;
            const std::size_t szSize = filteredPoly.size();
            for( std::size_t sz = 0U; sz != szSize; ++sz )
            {
                const std::size_t szPrev = ( sz + szSize - 1U ) % szSize;
                const std::size_t szNext = ( sz + szSize + 1U ) % szSize;

                const exact::Point& ptPrev = filteredPoly[ szPrev ];
                const exact::Point& pt     = filteredPoly[ sz ];
                const exact::Point& ptNext = filteredPoly[ szNext ];

                if( ( CGAL::EQUAL == CGAL::compare_xy( ptPrev, pt ) )
                    || ( CGAL::EQUAL == CGAL::compare_xy( pt, ptNext ) ) || ( CGAL::collinear( ptPrev, pt, ptNext ) ) )
                {
                    // remove point
                    filteredPoly.erase( filteredPoly.begin() + sz );
                    bFound = true;
                    break;
                }
            }
        }

        if( !filteredPoly.is_empty() && filteredPoly.is_simple() )
        {
            if( !filteredPoly.is_counterclockwise_oriented() )
            {
                filteredPoly.reverse_orientation();
            }
        }
        return filteredPoly;
    }
    else
    {
        return poly;
    }
}

Segment makeArrowHead( const Segment& segment, double size )
{
    static Transform rotation = rotate( AngleTraits::eEastSouthEast );

    if( segment.is_degenerate() ) return segment;

    const Vector v = rotation.transform( -segment.to_vector() );

    const double length = CGAL::approximate_sqrt( v.squared_length() );

    return Segment( segment[ 1 ], segment[ 1 ] + ( v * size / length ) );
}

} // namespace schematic::Utils
