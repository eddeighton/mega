
#ifndef INVOCATION_6_JUNE_2022
#define INVOCATION_6_JUNE_2022

#include "database/types/invocation_id.hpp"

#include "database/model/OperationsStage.hxx"


namespace mega
{
namespace invocation
{
    OperationsStage::Operations::Invocation* construct( const mega::invocation::ID& id, OperationsStage::Database& database, const mega::io::megaFilePath& sourceFile );

} // namespace invocation
} // namespace mega

#endif // INVOCATION_6_JUNE_2022
