
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

#ifndef GUARD_2023_December_08_symbol_id
#define GUARD_2023_December_08_symbol_id

#include "mega/values/compilation/interface/inline.h"

#include "mega/values/native_types.hpp"

#include "common/serialisation.hpp"

#include <limits>
#include <ostream>
#include <iomanip>

namespace mega::interface
{

class SymbolID : public c_symbol_id
{
public:
    using ValueType = U32;

    struct Hash
    {
        inline U64 operator()( const SymbolID& ref ) const noexcept { return ref.getValue(); }
    };

    constexpr inline SymbolID()
        : c_symbol_id{ 0 }
    {
    }
    constexpr inline explicit SymbolID( c_symbol_id value )
        : c_symbol_id( value )
    {
    }
    constexpr inline explicit SymbolID( ValueType v )
        : c_symbol_id{ v }
    {
    }

    constexpr inline ValueType getValue() const { return value; }

    constexpr inline bool operator==( const SymbolID& cmp ) const { return value == cmp.value; }
    constexpr inline bool operator!=( const SymbolID& cmp ) const { return !( this->operator==( cmp ) ); }
    constexpr inline bool operator<( const SymbolID& cmp ) const { return value < cmp.value; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "symbol_id", value );
        }
        else
        {
            archive& value;
        }
    }
};

inline constexpr SymbolID operator ""_S( unsigned long long int value )
{
    return SymbolID{ static_cast< SymbolID::ValueType >( value ) };
}

static constexpr SymbolID NULL_SYMBOL_ID = 0x0_S;
static constexpr SymbolID ROOT_SYMBOL_ID = 0x1_S;

static constexpr const char* ROOT_SYMBOL = "Root";

inline std::ostream& operator<<( std::ostream& os, const SymbolID& value )
{
    return os << "0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << value.getValue() << std::dec << "_S";
}

} // namespace mega::interface

#endif // GUARD_2023_December_08_symbol_id
