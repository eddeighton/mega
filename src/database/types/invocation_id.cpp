

#include "database/types/invocation_id.hpp"
#include "database/types/operation.hpp"

#include "common/string.hpp"

std::ostream& operator<<( std::ostream& os, const mega::invocation::ID& invocationID )
{
    common::delimit( invocationID.m_context.begin(), invocationID.m_context.end(), ".", os );
    os << '_';
    common::delimit( invocationID.m_type_path.begin(), invocationID.m_type_path.end(), ".", os );
    os << '_';
    return os << mega::getOperationString( invocationID.m_operation );
}
