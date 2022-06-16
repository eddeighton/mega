
#include "service/protocol/common/node.hpp"

#include <string>
#include <array>
#include <algorithm>

namespace mega
{
namespace network
{
namespace
{
using NodeStringArray = std::array< std::string, Node::TOTAL_NODE_TYPES >;
// clang-format off
static const NodeStringArray g_nodeTypeStrings = 
{ 
    std::string{ "Terminal" }, 
    std::string{ "Tool" }, 
    std::string{ "Executor" }, 
    std::string{ "Daemon" },
    std::string{ "Root" } 
};
// clang-format on
} // namespace

const char* Node::toStr( Node::Type type ) { return g_nodeTypeStrings[ type ].c_str(); }

Node::Type Node::fromStr( const char* pszStr )
{
    auto iFind = std::find( g_nodeTypeStrings.begin(), g_nodeTypeStrings.end(), pszStr );
    if ( iFind == g_nodeTypeStrings.end() )
        return TOTAL_NODE_TYPES;
    else
        return static_cast< Node::Type >( std::distance( g_nodeTypeStrings.begin(), iFind ) );
}

} // namespace network
} // namespace mega