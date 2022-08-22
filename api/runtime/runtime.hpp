#ifndef RUNTIME_18_JUNE_2022
#define RUNTIME_18_JUNE_2022

#include "runtime/runtime_functions.hpp"

#include "mega/common.hpp"
#include "mega/root.hpp"
#include "mega/invocation_id.hpp"

#include "service/protocol/common/header.hpp"
#include "service/protocol/common/megastructure_installation.hpp"
#include "service/protocol/common/project.hpp"

namespace mega
{
namespace runtime
{
extern void initialiseRuntime( const mega::network::MegastructureInstallation& megastructureInstallation,
                               const mega::network::Project&                   project );

extern ::Root* allocateRoot( mega::ExecutionContext&              executionContext,
                             const mega::network::ConversationID& simulationID );
extern void    releaseRoot( mega::ExecutionContext& executionContext, ::Root* pRoot );

extern void get_allocate( const char* pszUnitName, mega::ExecutionContext& executionContext,
                          const mega::InvocationID& invocationID, AllocateFunction* ppFunction );

extern void get_read( const char* pszUnitName, mega::ExecutionContext& executionContext,
                      const mega::InvocationID& invocationID, ReadFunction* ppFunction );

} // namespace runtime
} // namespace mega

#endif // RUNTIME_18_JUNE_2022
