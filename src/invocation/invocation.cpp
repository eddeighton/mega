
#include "invocation/invocation.hpp"
#include "database/model/OperationsStage.hxx"

namespace mega
{
namespace invocation
{

OperationsStage::Operations::Invocation* construct( const mega::invocation::ID& id, OperationsStage::Database& database,
                                                    const mega::io::megaFilePath& sourceFile )
{
    using namespace OperationsStage;
    using namespace OperationsStage::Operations;

    Invocations* pInvocations = database.one< Invocations >( sourceFile );

    Invocation* pInvocation = database.construct< Invocation >( Invocation::Args{} );

    pInvocations->insert_invocations( id, pInvocation );

    return pInvocation;
}

} // namespace invocation
} // namespace mega
