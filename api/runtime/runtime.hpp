#ifndef RUNTIME_18_JUNE_2022
#define RUNTIME_18_JUNE_2022

#include "runtime/runtime_functions.hpp"

#include "mega/common.hpp"
#include "mega/execution_context.hpp"
#include "mega/root.hpp"
#include "mega/invocation_id.hpp"

#include "service/protocol/common/header.hpp"
#include "service/protocol/common/megastructure_installation.hpp"
#include "service/protocol/common/project.hpp"

namespace mega
{
namespace runtime
{
void initialiseRuntime( const mega::network::MegastructureInstallation& megastructureInstallation,
                        const mega::network::Project&                   project );
bool isRuntimeInitialised();

class ExecutionRoot
{
public:
    ExecutionRoot( mega::MPEStorage mpe );
    ~ExecutionRoot();

    mega::reference root() const { return m_root; }

private:
    mega::reference m_root;
};

void get_allocate( const char* pszUnitName, const mega::InvocationID& invocationID, AllocateFunction* ppFunction );
void get_read( const char* pszUnitName, const mega::InvocationID& invocationID, ReadFunction* ppFunction );
void get_write( const char* pszUnitName, const mega::InvocationID& invocationID, WriteFunction* ppFunction );
void get_call( const char* pszUnitName, const mega::InvocationID& invocationID, CallFunction* ppFunction );

} // namespace runtime
} // namespace mega

#endif // RUNTIME_18_JUNE_2022
