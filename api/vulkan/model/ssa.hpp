
#ifndef SSA_25_JULY_2022
#define SSA_25_JULY_2022

#include <cstdint>
#include <vector>
#include <type_traits>
#include <optional>

#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <vulkan/vulkan.hpp>

namespace ssa
{

struct Variable
{
public:
    using Ptr      = Variable*;
    using PtrArray = std::vector< Ptr >;
    std::uint32_t      id;
    Variable::PtrArray parameters;
};

template < typename T >
struct Value : public Variable
{
    using Ptr = Value< T >*;
    std::optional< T > value;
};

struct Command : public Variable
{
    using Ptr = Command*;
    Variable::PtrArray results;
};

template < typename... T >
using Chain = vk::StructureChain< T... >;

template < typename T >
struct Lazy
{
    typename Variable::Ptr pVariable;
};

template < typename T >
struct NonLazy
{
    typename Variable::Ptr pVariable;
};

template < typename T >
struct Null
{
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

} // namespace ssa

#endif // SSA_25_JULY_2022
