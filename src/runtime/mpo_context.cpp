
#include "runtime/mpo_context.hpp"

#include "common/assert_verify.hpp"

namespace
{
thread_local mega::MPOContext* g_pMPOContext = nullptr;
} // namespace

namespace mega
{

void MPOContext::resume( MPOContext* pMPOContext )
{
    VERIFY_RTE( g_pMPOContext == nullptr );
    g_pMPOContext = pMPOContext;
}

void MPOContext::suspend() { g_pMPOContext = nullptr; }

MPOContext* MPOContext::get() { return g_pMPOContext; }

} // namespace mega