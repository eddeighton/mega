



#ifndef SSA_25_JULY_2022
#define SSA_25_JULY_2022

#include "mega/native_types.hpp"

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include <type_traits>
#include <optional>
#include <vector>
#include <list>
#include <memory>

namespace ssa
{

struct Variable
{
public:
    virtual ~Variable() {}

    using Ptr      = Variable*;
    using PtrArray = std::vector< Ptr >;
    mega::U32 id;
    PtrArray      parameters;
};

struct Command : public Variable
{
    using Ptr = Command*;
    Variable::PtrArray results;
    bool               evaluated = false;
};

// template < typename... T >
// using Chain = vk::StructureChain< T... >;

template < typename T >
struct Value : public Variable
{
    using Ptr = Value< T >*;

    virtual T&   get() const = 0;
    virtual bool is() const  = 0;
};

template < typename T >
struct ValueInstance : public Value< T >
{
    using Ptr = ValueInstance< T >*;

    virtual T&   get() const { return *m_value; }
    virtual bool is() const { return m_value.get(); }

    inline void set( const T& value ) { m_value = std::make_unique< T >( value ); }

private:
    std::unique_ptr< T > m_value;
};

template < typename THead, typename... T >
struct typelist_head
{
    using Type = THead;
};

template < typename... T >
struct Chain : public Value< typename typelist_head< T... >::Type >
{
    using Ptr       = Chain< T... >*;
    using BaseType  = typename typelist_head< T... >::Type;
    using Base      = Value< BaseType >;
    using ChainType = vk::StructureChain< T... >;

    virtual BaseType& get() const { return std::get< 0 >( *m_value.get() ); }
    virtual bool      is() const { return m_value.get(); }

    inline ChainType& getChain() const { return *m_value.get(); }
    inline void       setChain( const ChainType& value ) { m_value = std::make_unique< ChainType >( value ); }

private:
    std::unique_ptr< ChainType > m_value;
};

template < typename T >
struct make_variable_impl
{
    using Type = ValueInstance< T >;
};

template < typename... Ts >
struct make_variable_impl< Chain< Ts... > >
{
    using Type = Chain< Ts... >;
};

template < typename TValueType >
struct make_variable
{
    using Type = typename make_variable_impl< TValueType >::Type;
};

template < typename T >
struct get_pointer_type
{
    using Type = typename T::Ptr;
};

template < typename T >
struct Lazy
{
    /*Lazy( Command* pCommand )
        : pVariable( pCommand )
    {
    }
    Lazy( typename make_variable< T >::Type* pValue )
        : pVariable( pValue )
    {
    }*/
    typename Variable::Ptr pVariable;
};

template < typename T >
struct NonLazy
{
    /*NonLazy( Command* pCommand )
        : pVariable( pCommand )
    {
    }
    NonLazy( typename make_variable< T >::Type* pValue )
        : pVariable( pValue )
    {
    }*/
    typename Variable::Ptr pVariable;
};

template < typename T >
struct Null
{
};

using Void = char; // for Lazy<Void>

template < template < typename T0 > class Wrapper, typename T >
inline Variable::Ptr get_param_optional( const Wrapper< T >& param )
{
    return param.pVariable;
}

template < typename T >
inline Variable::Ptr get_param_optional( const Null< T >& param )
{
    return nullptr;
}

template < template < typename T0 > class Wrapper, typename T >
inline Variable::Ptr get_param( const Wrapper< T >& param )
{
    return param.pVariable;
}

template < typename TExpected, typename TActual >
struct check_param
{
    static_assert( false && typeid( TExpected ).name(), "Invalid type used in ssa function" );
    static constexpr bool value = false;
};

template < typename TExpected >
struct check_param< TExpected, TExpected >
{
    static constexpr bool value = true;
};

template < typename TExpected >
struct check_param< Chain< TExpected >, TExpected >
{
    static constexpr bool value = true;
};

class Allocator
{
public:
    Allocator() {}
    ~Allocator()
    {
        for ( Variable* p : m_values )
            delete p;
    }

    static Allocator& get()
    {
        static Allocator allocator;
        return allocator;
    }

    template < typename T >
    typename get_pointer_type< typename make_variable< T >::Type >::Type make()
    {
        using VariableType           = typename make_variable< T >::Type;
        typename VariableType::Ptr p = new VariableType;
        m_values.push_back( p );
        return p;
    }

    template < typename T >
    typename Value< T >::Ptr make_value()
    {
        typename Value< T >::Ptr p = new ValueInstance< T >;
        m_values.push_back( p );
        return p;
    }

    template < typename T >
    typename T::Ptr make_chain()
    {
        typename T::Ptr p = new T;
        m_values.push_back( p );
        return p;
    }

    Command::Ptr make_command()
    {
        Command c;
        m_commands.push_back( c );
        return &m_commands.back();
    }

private:
    std::list< Variable* > m_values;
    std::list< Command >   m_commands;
};

// first parameter - accumulator
// second parameter - input Chain
template < class T, class U >
struct make_chain_impl;

// first case - the head of the Chain is Chain too
// expand this Chain and continue
template < class... Ts, class... Heads, class... Tail >
struct make_chain_impl< Chain< Ts... >, Chain< Chain< Heads... >, Tail... > >
{
    using Type = typename make_chain_impl< Chain< Ts... >, Chain< Heads..., Tail... > >::Type;
};

// second case - the head of the Chain is not a Chain
// append it to our new, flattened Chain
template < class... Ts, class Head, class... Tail >
struct make_chain_impl< Chain< Ts... >, Chain< Head, Tail... > >
{
    using Type = typename make_chain_impl< Chain< Ts..., Head >, Chain< Tail... > >::Type;
};

// base case - input Chain is empty
// return our flattened Chain
template < class... Ts >
struct make_chain_impl< Chain< Ts... >, Chain<> >
{
    using Type = Chain< Ts... >;
    // static constexpr const auto ID = std::array< mega::TypeID, std::size( Ts::ID... ) >{ Ts::ID... };
};

template < typename T >
struct make_chain
{
    using Type = Chain< T >;
    // static constexpr const auto ID = std::array< mega::TypeID, 1U >{ T::ID };
};

template < typename... Ts >
struct make_chain< Chain< Ts... > >
{
    using Type = typename make_chain_impl< Chain<>, Chain< Ts... > >::Type;
    // static constexpr const auto ID = std::array< mega::TypeID, std::size( Ts::ID... ) >{ Ts::ID... };
};

template < typename T >
struct chain_head_impl
{
    using Type = T;
};

template < typename... Ts >
struct chain_head_impl< Chain< Ts... > >
{
    using Type = typename typelist_head< Ts... >::Type;
};

template < typename TChain >
struct chain_head
{
    using Type = typename chain_head_impl< TChain >::Type;
};

} // namespace ssa

#endif // SSA_25_JULY_2022
