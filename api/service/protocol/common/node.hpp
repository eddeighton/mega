


#ifndef NODE_17_JUNE_2022
#define NODE_17_JUNE_2022

#include <string>

namespace mega
{
namespace network
{

class Node
{
public:
    enum Type
    {
        Leaf,
        Terminal,
        Tool,
        Executor,
        Daemon,
        Root,
        TOTAL_NODE_TYPES
    };

    static const char* toStr( Type type );
    static Type        fromStr( const char* pszStr );
};

std::string makeProcessName( Node::Type type );

} // namespace network
} // namespace mega

#endif // NODE_17_JUNE_2022
