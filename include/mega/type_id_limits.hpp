
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

#ifndef GUARD_2023_February_08_type_id_limits
#define GUARD_2023_February_08_type_id_limits

#include "type_id.hpp"

#include <limits>

namespace mega
{

static constexpr auto max_object_id     = static_cast< TypeID::SubValueType >( std::numeric_limits< TypeID::SubValueType >::max() >> 1 );
static constexpr auto max_sub_object_id = std::numeric_limits< TypeID::SubValueType >::max();
static constexpr auto min_object_id     = std::numeric_limits< TypeID::SubValueType >::min();
static constexpr auto min_sub_object_id = std::numeric_limits< TypeID::SubValueType >::min();

static_assert( max_object_id == 32767, "Maths is wrong" );
static_assert( max_sub_object_id == 65535, "Maths is wrong" );

static constexpr auto max_typeID_context = TypeID::make_context( max_object_id, max_sub_object_id );
static constexpr auto min_typeID_context = TypeID::make_context( min_object_id, min_sub_object_id );

static constexpr auto max_symbol_id = -1;
static constexpr auto min_symbol_id = std::numeric_limits< TypeID::ValueType >::min();

static_assert( TypeID::LOWEST_SYMBOL_ID == min_symbol_id, "Lowest symbol ID incorrect" );

static constexpr auto max_typeID_symbol = TypeID{ max_symbol_id };
static constexpr auto min_typeID_symbol = TypeID{ min_symbol_id };

} // namespace mega

#endif // GUARD_2023_February_08_type_id_limits
