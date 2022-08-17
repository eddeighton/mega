

#include "database/types/invocation_id.hpp"
#include "database/types/operation.hpp"

#include "common/string.hpp"

#include "nlohmann/json.hpp"

namespace
{
    template< class Iter >
    void delimit( Iter pBegin, Iter pEnd, const std::string& delimiter, std::ostream& os )
    {
        for( Iter p = pBegin, pNext = pBegin; p!=pEnd; ++p )
        {
            ++pNext;
            if( *p >= 0 )
            {
                // type id
                os << 't' << *p;
            }
            else
            {
                // symbol id
                os << 's' << -*p;
            }
            if( pNext != pEnd )
            {
                os << delimiter;
            }
        }
    }
}

std::ostream& operator<<( std::ostream& os, const mega::InvocationID& invocationID )
{
    os << "c";
    delimit( invocationID.m_context.begin(), invocationID.m_context.end(), "_", os );
    os << 'p';
    delimit( invocationID.m_type_path.begin(), invocationID.m_type_path.end(), "_", os );
    return os << mega::getOperationString( invocationID.m_operation );
}
