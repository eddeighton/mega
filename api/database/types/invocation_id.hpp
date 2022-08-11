#ifndef INVOCATION_ID_7_JUNE_2022
#define INVOCATION_ID_7_JUNE_2022

#include "mega/common.hpp"

#include "common/requireSemicolon.hpp"

#include "nlohmann/json.hpp"

#include <vector>

namespace mega
{
namespace invocation
{
class ID
{
public:
    using SymbolIDVector = std::vector< mega::SymbolID >;

    SymbolIDVector    m_context;
    SymbolIDVector    m_type_path;
    mega::OperationID m_operation;

    inline bool operator==( const ID& cmp ) const
    {
        return ( m_context == cmp.m_context ) && ( m_type_path == cmp.m_type_path )
               && ( m_operation == cmp.m_operation );
    }

    inline bool operator<( const ID& cmp ) const
    {
        return ( m_context != cmp.m_context )       ? ( m_context < cmp.m_context )
               : ( m_type_path != cmp.m_type_path ) ? ( m_type_path < cmp.m_type_path )
                                                    : ( m_operation < cmp.m_operation );
    }

    ID() {}

    ID( const SymbolIDVector& context, const SymbolIDVector& typePath, mega::OperationID operationID )
        : m_context( context )
        , m_type_path( typePath )
        , m_operation( operationID )
    {
    }

    template < std::size_t Size >
    ID( mega::TypeID context, const std::array< mega::TypeID, Size >& typePath, mega::OperationID operation )
        : m_context{ context }
        , m_type_path( typePath.begin(), typePath.end() )
        , m_operation( operation )
    {
    }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_context;
        archive& m_type_path;
        archive& m_operation;
    }
};

inline void to_json( nlohmann::json& j, const mega::invocation::ID& invocationID )
{
    j = nlohmann::json{ { "context", invocationID.m_context },
                        { "type_path", invocationID.m_type_path },
                        { "operation", invocationID.m_operation } };
}

} // namespace invocation
} // namespace mega

std::ostream& operator<<( std::ostream& os, const mega::invocation::ID& invocationID );

#endif // INVOCATION_ID_7_JUNE_2022
