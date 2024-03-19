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

#ifndef INVOCATION_ID_12_AUG_2022
#define INVOCATION_ID_12_AUG_2022

#include "mega/values/compilation/interface/symbol_id.hpp"
#include "mega/values/compilation/interface/type_id.hpp"
#include "mega/values/compilation/interface/symbol_id.hpp"

#include "common/assert_verify.hpp"
#include "common/serialisation.hpp"
#include "common/hash.hpp"

#include <utility>
#include <array>
#include <ostream>

namespace mega
{

class InvocationID
{
public:
    static constexpr auto MAX_ELEMENTS = 8U;

    using TypeIDArray   = std::array< interface::TypeID, MAX_ELEMENTS >;
    using SymbolIDArray = std::array< interface::SymbolID, MAX_ELEMENTS >;

    TypeIDArray   m_context;
    SymbolIDArray m_symbols;
    bool          m_bHasParams = false;

    struct Hash
    {
        inline std::size_t operator()( const InvocationID& value ) const
        {
            return common::Hash( value.m_context, value.m_symbols, value.m_bHasParams ).get();
        }
    };

    constexpr inline InvocationID() = default;

    constexpr inline explicit InvocationID( interface::TypeID context, SymbolIDArray symbols, bool bHasParams )
        : m_context( { context } )
        , m_symbols( std::move( symbols ) )
        , m_bHasParams( bHasParams )
    {
    }

    constexpr inline explicit InvocationID( TypeIDArray context, SymbolIDArray symbols, bool bHasParams )
        : m_context( std::move( context ) )
        , m_symbols( std::move( symbols ) )
        , m_bHasParams( bHasParams )
    {
    }

    constexpr inline bool operator==( const InvocationID& cmp ) const
    {
        return ( m_context == cmp.m_context ) && ( m_symbols == cmp.m_symbols ) && ( m_bHasParams == cmp.m_bHasParams );
    }

    constexpr inline bool operator<( const InvocationID& cmp ) const
    {
        return ( m_context != cmp.m_context )   ? ( m_context < cmp.m_context )
               : ( m_symbols != cmp.m_symbols ) ? ( m_symbols < cmp.m_symbols )
                                                : ( m_bHasParams < cmp.m_bHasParams );
    }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "context", m_context );
            archive& boost::serialization::make_nvp( "symbols", m_symbols );
            archive& boost::serialization::make_nvp( "bHasParams", m_bHasParams );
        }
        else
        {
            archive& m_context;
            archive& m_symbols;
            archive& m_bHasParams;
        }
    }
};

inline std::ostream& operator<<( std::ostream& os, const mega::InvocationID& invocationID )
{
    for( const auto& context : invocationID.m_context )
    {
        if( !context.valid() )
        {
            break;
        }
        os << context;
    }

    for( const auto& symbol : invocationID.m_symbols )
    {
        if( symbol == interface::NULL_SYMBOL_ID )
        {
            break;
        }
        os << symbol;
    }

    os << std::boolalpha << invocationID.m_bHasParams;

    return os;
}

} // namespace mega

#endif // INVOCATION_ID_12_AUG_2022
