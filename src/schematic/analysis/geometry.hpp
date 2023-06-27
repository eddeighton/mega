
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

#ifndef GUARD_2023_June_27_geometry
#define GUARD_2023_June_27_geometry

#include "common/angle.hpp"

namespace exact
{

using Angle = Math::Angle< 8 >;

struct Value
{
    int x, y;
    int direction = Angle::TOTAL_ANGLES;
    int turns     = 0;

    inline bool operator<( const Value& value ) const
    {
        //
        return ( x != value.x ) ? ( x < value.x ) : ( y < value.y );
    }
    inline bool operator==( const Value& value ) const
    {
        //
        return ( x == value.x ) && ( y == value.y );
    }

    struct Hash
    {
        inline std::size_t operator()( const Value& value ) const { return value.x + value.y; }
    };
};

int pointLineSegmentDistance( const Value& point, const Value& lineStart, const Value& lineEnd );

}

#endif //GUARD_2023_June_27_geometry
