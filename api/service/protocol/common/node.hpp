#ifndef NODE_17_JUNE_2022
#define NODE_17_JUNE_2022

namespace mega
{
namespace network
{

class Node
{
public:
    enum Type
    {
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

} // namespace network
} // namespace mega

#endif // NODE_17_JUNE_2022
