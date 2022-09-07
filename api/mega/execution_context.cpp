
#include "mega/execution_context.hpp"

#include "common/assert_verify.hpp"

namespace
{
thread_local mega::ExecutionContext* g_pExecutionContext = nullptr;
} // namespace

namespace mega
{

void ExecutionContext::resume( ExecutionContext* pExecutionContext )
{
    VERIFY_RTE( g_pExecutionContext == nullptr );
    g_pExecutionContext = pExecutionContext;
}

void ExecutionContext::suspend()
{
    g_pExecutionContext = nullptr;
}

ExecutionContext* ExecutionContext::get()
{
    return g_pExecutionContext;
}

} // namespace mega