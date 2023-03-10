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

#ifndef GEOMETRY_TYPES_01_DEC_2020
#define GEOMETRY_TYPES_01_DEC_2020

#include "common/angle.hpp"

#include <cmath>

namespace schematic
{
using AngleTraits              = Math::Angle< 16 >;
using Angle                    = AngleTraits::Value;
static const Angle QuarterTurn = static_cast< Angle >( 4 );

struct Map_FloorAverage
{
    float operator()( float fValue ) const { return floorf( 0.5f + fValue ); }
};

struct Map_FloorAverageMin
{
    const float fMin;
    Map_FloorAverageMin( float _fMin )
        : fMin( _fMin )
    {
    }
    float operator()( float fValue ) const { return std::max( fMin, floorf( 0.5f + fValue ) ); }
};

} // namespace schematic

#endif // GEOMETRY_TYPES_01_DEC_2020