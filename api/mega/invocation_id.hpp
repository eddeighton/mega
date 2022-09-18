
#ifndef INVOCATION_ID_12_AUG_2022
#define INVOCATION_ID_12_AUG_2022

#include "mega/common.hpp"

#include <vector>
#include <array>

namespace mega
{

class InvocationID
{
public:
    using SymbolIDVector = std::vector< mega::SymbolID >;

    SymbolIDVector    m_context;
    SymbolIDVector    m_type_path;
    mega::OperationID m_operation;

    inline bool operator==( const InvocationID& cmp ) const
    {
        return ( m_context == cmp.m_context ) && ( m_type_path == cmp.m_type_path )
               && ( m_operation == cmp.m_operation );
    }

    inline bool operator<( const InvocationID& cmp ) const
    {
        return ( m_context != cmp.m_context )       ? ( m_context < cmp.m_context )
               : ( m_type_path != cmp.m_type_path ) ? ( m_type_path < cmp.m_type_path )
                                                    : ( m_operation < cmp.m_operation );
    }

    InvocationID() {}

    InvocationID( const SymbolIDVector& context, const SymbolIDVector& typePath, mega::OperationID operationID )
        : m_context( context )
        , m_type_path( typePath )
        , m_operation( operationID )
    {
    }

    template < mega::U64 Size >
    InvocationID( mega::TypeID context, const std::array< mega::TypeID, Size >& typePath, mega::TypeID operation )
        : m_context{ context }
        , m_type_path( typePath.begin(), typePath.end() )
        , m_operation( static_cast< mega::OperationID >( operation ) )
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

}

#endif //INVOCATION_ID_12_AUG_2022
