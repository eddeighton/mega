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

#ifndef TRANSFORM_07_NOV_2020
#define TRANSFORM_07_NOV_2020

#include "format/format.hpp"

#include "schematic/cgalSettings.hpp"
#include "schematic/geometry.hpp"

#include "common/angle.hpp"

#include <string>
#include <sstream>

namespace schematic
{
inline Transform fromFormat( const format::Transform& transform )
{
    return Transform( transform.m00, transform.m01, transform.m02, transform.m10, transform.m11, transform.m12 );
}

inline exact::Transform inexactToExact( const Transform& transform )
{
    static const exact::InexactToExact converter;
    return exact::Transform( converter( transform.m( 0, 0 ) ),
                             converter( transform.m( 0, 1 ) ),
                             converter( transform.m( 0, 2 ) ),
                             converter( transform.m( 1, 0 ) ),
                             converter( transform.m( 1, 1 ) ),
                             converter( transform.m( 1, 2 ) ) );
}

inline Transform exactToInexact( const exact::Transform& transform )
{
    static const exact::ExactToInexact converter;
    return Transform( converter( transform.m( 0, 0 ) ),
                      converter( transform.m( 0, 1 ) ),
                      converter( transform.m( 0, 2 ) ),
                      converter( transform.m( 1, 0 ) ),
                      converter( transform.m( 1, 1 ) ),
                      converter( transform.m( 1, 2 ) ) );
}

inline void toFormat( const Transform& transform, format::Transform& formatTransform )
{
    formatTransform
        = format::Transform{ CGAL::to_double( transform.m( 0, 0 ) ), CGAL::to_double( transform.m( 0, 1 ) ),
                             CGAL::to_double( transform.m( 0, 2 ) ), CGAL::to_double( transform.m( 1, 0 ) ),
                             CGAL::to_double( transform.m( 1, 1 ) ), CGAL::to_double( transform.m( 1, 2 ) ) };
}

inline Transform translate( const Vector v )
{
    return Transform( CGAL::TRANSLATION, v );
}

inline Transform rotate( AngleTraits::Value angle )
{
    const double a = ( static_cast< double >( angle ) * MY_PI * 2.0 ) / AngleTraits::TOTAL_ANGLES;
    return Transform( CGAL::ROTATION, sin( a ), cos( a ) );
}

inline const Transform& mirrorX()
{
    static Transform reflectX( CGAL::REFLECTION, Line( Point( 0, 0 ), Point( 1, 0 ) ) );
    return reflectX;
}

inline const Transform& mirrorY()
{
    static Transform reflectY( CGAL::REFLECTION, Line( Point( 0, 0 ), Point( 0, 1 ) ) );
    return reflectY;
}

inline Vector getTranslation( const Transform& transform )
{
    return Vector( transform.m( 0, 2 ), transform.m( 1, 2 ) );
}

inline void setTranslation( Transform& transform, Float fTranslateX, Float fTranslateY )
{
    transform = Transform(
        transform.m( 0, 0 ), transform.m( 0, 1 ), fTranslateX, transform.m( 1, 0 ), transform.m( 1, 1 ), fTranslateY );
}

inline Transform transformWithinBounds( const Transform& existing, const Transform& transform,
                                        const Rect& transformBounds )
{
    Transform result = existing;
    {
        const auto fCentreX = transformBounds.xmin() + ( transformBounds.xmax() - transformBounds.xmin() ) / 2.0f;
        const auto fCentreY = transformBounds.ymin() + ( transformBounds.ymax() - transformBounds.ymin() ) / 2.0f;

        const Transform translateToBoundsCentre = translate( Vector( -fCentreX, -fCentreY ) );
        const Transform translateBack           = translate( Vector( fCentreX, fCentreY ) );

        // pre multiply
        result = translateToBoundsCentre * result;
        result = transform * result;
        result = translateBack * result;
    }
    return result;
}

static Transform rotateLeft( const Transform& existing, const Rect& transformBounds )
{
    static const auto leftTurn = Math::rotate< AngleTraits >( AngleTraits::eEast, 1 );
    return transformWithinBounds( existing, rotate( leftTurn ), transformBounds );
}

static Transform rotateRight( const Transform& existing, const Rect& transformBounds )
{
    static const auto rightTurn = Math::rotate< AngleTraits >( AngleTraits::eEast, -1 );
    return transformWithinBounds( existing, rotate( rightTurn ), transformBounds );
}

static Transform flipHorizontally( const Transform& existing, const Rect& transformBounds )
{
    return transformWithinBounds( existing, mirrorY(), transformBounds );
}

static Transform flipVertically( const Transform& existing, const Rect& transformBounds )
{
    return transformWithinBounds( existing, mirrorX(), transformBounds );
}

} // namespace schematic

#endif // TRANSFORM_07_NOV_2020