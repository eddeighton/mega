

#include <gtest/gtest.h>

#include "vulkan/model/ssa.hpp"

#include <tuple>
#include <type_traits>
#include <typeinfo>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace ssa
{

struct A
{
};

struct B
{
};

struct Result
{
};

struct Device
{
};


// clang-format off
template 
< 
    // output parameter type params with defaults
    typename TResult = Device,

    // deduced input parameter type wrapper types
    template < typename T1 > class W1,

    // deduced type params
    typename T1, 
    
    // input param type parameter constraints
    std::enable_if_t< check_param< A, typename chain_head< T1 >::Type >::value, bool > = true,

    // output param type parameter constraints
    std::enable_if_t< check_param< Device, typename chain_head< TResult >::Type >::value, bool > = true     
>
// clang-format on
inline std::tuple< Lazy< Command >, Lazy< TResult > > vkTestCommand( const W1< T1 >& pCreateInfo )
{
    typename Command::Ptr pCommand = Allocator::get().make_command();
    pCommand->id                   = 1141;
    pCommand->parameters.push_back( get_param( pCreateInfo ) );

    // ValueInstance< Device >* pDeviceValueInstance = ;

    const Lazy< TResult > device{ Allocator::get().make< TResult >() };
    pCommand->results.push_back( get_param( device ) );
    return std::make_tuple( Lazy< Command >{ pCommand }, device );
}

} // namespace ssa

TEST( Vulkan, InputParameterTypeChecks )
{
    using namespace ssa;

    // non chain input
    {
        Lazy< A >                                     a{ Allocator::get().make_value< A >() };
        std::tuple< Lazy< Command >, Lazy< Device > > result = vkTestCommand( a );
        ASSERT_EQ( get_param( std::get< 0 >( result ) )->parameters[ 0 ], get_param( a ) );
    }
    // mono chain input
    {
        Lazy< Chain< A > >                            a{ Allocator::get().make_chain< Chain< A > >() };
        std::tuple< Lazy< Command >, Lazy< Device > > result = vkTestCommand( a );
        ASSERT_EQ( get_param( std::get< 0 >( result ) )->parameters[ 0 ], get_param( a ) );
    }
    // poly chain input
    {
        Lazy< Chain< A, B > >                         a{ Allocator::get().make_chain< Chain< A, B > >() };
        std::tuple< Lazy< Command >, Lazy< Device > > result = vkTestCommand( a );
        ASSERT_EQ( get_param( std::get< 0 >( result ) )->parameters[ 0 ], get_param( a ) );
    }
    // longer poly chain input
    {
        Lazy< Chain< A, B, A > >                      a{ Allocator::get().make_chain< Chain< A, B, A > >() };
        std::tuple< Lazy< Command >, Lazy< Device > > result = vkTestCommand( a );
        ASSERT_EQ( get_param( std::get< 0 >( result ) )->parameters[ 0 ], get_param( a ) );
    }
}

TEST( Vulkan, OutputParameterTypeChecks )
{
    using namespace ssa;

    // implicit default
    {
        Lazy< A >                                     a{ Allocator::get().make_value< A >() };
        std::tuple< Lazy< Command >, Lazy< Device > > result = vkTestCommand( a );
        ASSERT_EQ( get_param( std::get< 0 >( result ) )->parameters[ 0 ], get_param( a ) );
    }
    // explicit non-chain
    {
        Lazy< A >                                     a{ Allocator::get().make_value< A >() };
        std::tuple< Lazy< Command >, Lazy< Device > > result = vkTestCommand< Device >( a );
        ASSERT_EQ( get_param( std::get< 0 >( result ) )->parameters[ 0 ], get_param( a ) );
    }
    // explicit mono chain
    {
        Lazy< A >                                              a{ Allocator::get().make_value< A >() };
        std::tuple< Lazy< Command >, Lazy< Chain< Device > > > result = vkTestCommand< Chain< Device > >( a );
        ASSERT_EQ( get_param( std::get< 0 >( result ) )->parameters[ 0 ], get_param( a ) );
    }
    // explicit poly chain
    {
        Lazy< A >                                                 a{ Allocator::get().make_value< A >() };
        std::tuple< Lazy< Command >, Lazy< Chain< Device, B > > > result = vkTestCommand< Chain< Device, B > >( a );
        ASSERT_EQ( get_param( std::get< 0 >( result ) )->parameters[ 0 ], get_param( a ) );
    }
}

namespace ssa
{

// clang-format off
template 
< 
    // output parameter type params with defaults

    // deduced input parameter type wrapper types
    template < typename T1 > class W1,
    template < typename T2 > class W2,

    // deduced type params
    typename T1, 
    typename T2, 
    
    // input param type parameter constraints
    std::enable_if_t< check_param< A, typename chain_head< T1 >::Type >::value, bool > = true,
    std::enable_if_t< check_param< B, typename chain_head< T2 >::Type >::value, bool > = true
 
>
// clang-format on
inline std::tuple< Lazy< Command > > vkTestCommand2( const W1< T1 >& pCreateInfo, const W2< T2 >& pOptionalThing )
{
    typename Command::Ptr pCommand = Allocator::get().make_command();
    pCommand->id                   = 1142;
    pCommand->parameters.push_back( get_param( pCreateInfo ) );
    pCommand->parameters.push_back( get_param_optional( pOptionalThing ) );
    return std::make_tuple( Lazy< Command >{ pCommand } );
}

} // namespace ssa

TEST( Vulkan, OptionalInputParameterChecks )
{
    using namespace ssa;

    // non-null
    {
        Lazy< A >                     a{ Allocator::get().make_value< A >() };
        Lazy< B >                     b{ Allocator::get().make_value< B >() };
        std::tuple< Lazy< Command > > result = vkTestCommand2( a, b );
        ASSERT_EQ( get_param( std::get< 0 >( result ) )->parameters[ 0 ], get_param( a ) );
        ASSERT_EQ( get_param( std::get< 0 >( result ) )->parameters[ 1 ], get_param_optional( b ) );
    }
    // null
    {
        Lazy< A >                     a{ Allocator::get().make_value< A >() };
        Null< B >                     b;
        std::tuple< Lazy< Command > > result = vkTestCommand2( a, b );
        ASSERT_EQ( get_param( std::get< 0 >( result ) )->parameters[ 0 ], get_param( a ) );
        ASSERT_EQ( get_param( std::get< 0 >( result ) )->parameters[ 1 ], get_param_optional( b ) );
    }
}
