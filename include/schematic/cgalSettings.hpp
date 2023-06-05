
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

#ifndef CGAL_SETINGS_26_NOV_2020
#define CGAL_SETINGS_26_NOV_2020

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
// #include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
// #include <CGAL/Simple_cartesian.h>
// #include <CGAL/Exact_rational.h>
#include <CGAL/ch_graham_andrew.h>
#include <CGAL/minkowski_sum_2.h>

#include <CGAL/Cartesian.h>
#include <CGAL/Bbox_2.h>
#include <CGAL/Vector_2.h>
#include <CGAL/Ray_2.h>
#include <CGAL/Line_2.h>
#include <CGAL/Iso_rectangle_2.h>

#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arrangement_on_surface_with_history_2.h>
#include <CGAL/Arrangement_with_history_2.h>
#include <CGAL/Arr_extended_dcel.h>
#include <CGAL/Arr_simple_point_location.h>

#include <CGAL/IO/Arr_text_formatter.h>
#include <CGAL/IO/Arr_iostream.h>

#include <CGAL/create_offset_polygons_2.h>
#include <CGAL/Boolean_set_operations_2.h>

#include <CGAL/Cartesian_converter.h>

#include <ostream>
#include <istream>

namespace exact
{
struct DefaultedBool
{
    DefaultedBool() = default;
    DefaultedBool( bool b )
        : m_bValue( b )
    {
    }
    bool get() const { return m_bValue; }
    void set( bool b ) { m_bValue = b; }

private:
    bool m_bValue = false;
};

inline std::ostream& operator<<( std::ostream& os, const exact::DefaultedBool& defBool )
{
    return os << defBool.get();
}

inline std::istream& operator>>( std::istream& is, exact::DefaultedBool& defBool )
{
    bool bValue;
    is >> bValue;
    defBool.set( bValue );
    return is;
}
} // namespace exact

namespace schematic
{
using Float = double;

using Number_type = double;
// typedef CGAL::MP_Float                                        Number_type;
// typedef CGAL::Quotient< CGAL::MP_Float >                      Number_type;
// typedef CGAL::Quotient< double >                              Number_type;
// typedef CGAL::Exact_rational                                  Number_type;

// typedef CGAL::Cartesian< Number_type >                        Kernel;
using Kernel = CGAL::Simple_cartesian< Number_type >;
// typedef CGAL::Exact_predicates_inexact_constructions_kernel   Kernel;
// typedef CGAL::Exact_predicates_exact_constructions_kernel     Kernel;

using Transform = Kernel::Aff_transformation_2;
using Line      = Kernel::Line_2;
using Point     = Kernel::Point_2;
using Ray       = Kernel::Ray_2;
using Segment   = Kernel::Segment_2;
using Vector    = Kernel::Vector_2;
using Rect      = Kernel::Iso_rectangle_2;
using Bbox      = CGAL::Bbox_2;
using Circle    = Kernel::Circle_2;
using Direction = Kernel::Direction_2;

using Polygon            = CGAL::Polygon_2< Kernel >;
using Polygon_with_holes = CGAL::Polygon_with_holes_2< Kernel >;
// typedef CGAL::Arr_segment_traits_2< Kernel >                  Traits;
// typedef Traits::X_monotone_curve_2                            Curve;

// typedef CGAL::Arr_extended_dcel< Traits, DefaultedBool, DefaultedBool, DefaultedBool > Dcel;
// typedef CGAL::Arrangement_with_history_2< Traits, Dcel >      Arrangement;
// typedef Arrangement::Curve_handle                             Curve_handle;
// typedef CGAL::Arr_simple_point_location< Arrangement >        Point_location;
// typedef CGAL::Arr_extended_dcel_text_formatter< Arrangement > Formatter;
} // namespace schematic

namespace exact
{
using Float = double;

// typedef double                                                Number_type;
// typedef CGAL::MP_Float                                        Number_type;
// typedef CGAL::Quotient< CGAL::MP_Float >                      Number_type;
// typedef CGAL::Quotient< double >                              Number_type;
// typedef CGAL::Exact_rational                                  Number_type;

// typedef CGAL::Cartesian< Number_type >                        Kernel;
// typedef CGAL::Simple_cartesian< Number_type >                 Kernel;
// typedef CGAL::Exact_predicates_inexact_constructions_kernel   Kernel;
using Kernel = CGAL::Exact_predicates_exact_constructions_kernel;

using Transform = Kernel::Aff_transformation_2;
using Line      = Kernel::Line_2;
using Point     = Kernel::Point_2;
using Ray       = Kernel::Ray_2;
using Segment   = Kernel::Segment_2;
using Vector    = Kernel::Vector_2;
using Rect      = Kernel::Iso_rectangle_2;
using Bbox      = CGAL::Bbox_2;
using Circle    = Kernel::Circle_2;
using Direction = Kernel::Direction_2;

using Polygon            = CGAL::Polygon_2< Kernel >;
using Polygon_with_holes = CGAL::Polygon_with_holes_2< Kernel >;
using Traits             = CGAL::Arr_segment_traits_2< Kernel >;
using Curve              = Traits::X_monotone_curve_2;

using InexactToExact = CGAL::Cartesian_converter< ::schematic::Kernel, Kernel >;
using ExactToInexact = CGAL::Cartesian_converter< Kernel, ::schematic::Kernel >;

} // namespace exact

#endif // CGAL_SETINGS_26_NOV_2020