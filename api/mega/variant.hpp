//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
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

#ifndef EG_VARIANT
#define EG_VARIANT

#include "common.hpp"
#include "event.hpp"
//#include "result_type.hpp"

namespace mega
{
template < typename from, typename to >
struct is_convertible
{
    static constexpr const bool value = false;
};

template < typename... sequence >
struct boolean_or
{
    static constexpr const bool value = ( sequence::value || ... );
};

template < typename from, typename... toRest >
struct is_convertible_many
{
    static constexpr const bool value = boolean_or< is_convertible< from, toRest >... >::value;
};

// everything is convertible to and from an Event
template < typename to >
struct is_convertible< Event, to >
{
    static constexpr const bool value = true;
};

template < typename from >
struct is_convertible< from, Event >
{
    static constexpr const bool value = true;
};

template < typename T >
struct variant_runtime_type_check_helper
{
    static inline bool test( const Event& from )
    {
        T check( from );
        return check;
    }
};

template < typename... sequence >
struct variant_runtime_type_check
{
    static inline bool test( const Event& from )
    {
        return ( variant_runtime_type_check_helper< sequence >::test( from ) || ... );
    }
};

} // namespace mega

template < typename... Ts >
struct [[clang::eg_type( mega::id_Variant )]] __eg_variant
{
    inline __eg_variant() {}

    inline __eg_variant( const Event& from )
    {
        // when convert from event need to check runtime type against all __eg_variant types
        if ( ( from.data.is_valid() ) || !mega::variant_runtime_type_check< Ts... >::test( from ) )
        {
            data = mega::reference{};
        }
        else
        {
            data = from.data;
        }
    }

    template < class T >
    inline __eg_variant( const T& from )
    {
        static_assert( mega::is_convertible_many< T, Ts... >::value, "Incompatible mega type conversion" );
        data = from.data;
    }

    inline __eg_variant& operator=( const Event& from )
    {
        // when convert from event need to check runtime type against all __eg_variant types
        if ( ( from.data.is_valid() ) || !mega::variant_runtime_type_check< Ts... >::test( from ) )
        {
            data = mega::reference{};
        }
        else
        {
            data = from.data;
        }
        return *this;
    }

    template < class T >
    inline __eg_variant& operator=( const T& from )
    {
        static_assert( mega::is_convertible_many< T, Ts... >::value, "Incompatible mega type conversion" );
        data = from.data;
        return *this;
    }

    template < typename TComp >
    inline bool operator==( const TComp& cmp ) const
    {
        return data == cmp.data;
    }

    template < typename TComp >
    inline bool operator!=( const TComp& cmp ) const
    {
        return !( data == cmp.data );
    }

    template < typename TComp >
    inline bool operator<( const TComp& cmp ) const
    {
        return data < cmp.data;
    }

    inline operator const void*() const
    {
        if ( data.is_valid() )
        {
            return reinterpret_cast< const void* >( &data );
        }
        else
        {
            return nullptr;
        }
    }

    template < typename TypePath, typename Operation, typename... Args >
    typename mega::result_type< __eg_variant< Ts... >, TypePath, Operation >::Type invoke( Args... args ) const;

    mega::reference data;
};

template < typename... Args >
using Var = __eg_variant< Args... >;

namespace mega
{
//__eg_variant conversion is reflexive so just reuse is_convertible_many in reverse
template < typename... varArgs, typename to >
struct is_convertible< __eg_variant< varArgs... >, to >
{
    static constexpr const bool value = is_convertible_many< to, varArgs... >::value;
};
} // namespace mega

#endif // EG_VARIANT