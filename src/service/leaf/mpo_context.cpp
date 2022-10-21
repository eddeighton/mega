//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#include "service/mpo_context.hpp"

#include "common/assert_verify.hpp"

namespace
{
thread_local mega::MPOContext* g_pMPOContext = nullptr;
} // namespace

namespace mega
{

Cycle::~Cycle() { getMPOContext()->cycleComplete(); }

Context* Context::get() { return g_pMPOContext; }

MPOContext* getMPOContext() { return g_pMPOContext; }
void        resetMPOContext() { g_pMPOContext = nullptr; }
void        setMPOContext( MPOContext* pMPOContext )
{
    VERIFY_RTE( g_pMPOContext == nullptr );
    g_pMPOContext = pMPOContext;
}

void* MPOContext::get_this_shared_memory()
{
    VERIFY_RTE_MSG( m_pSharedMemory.get(), "Shared memory not initialised" );
    return m_pSharedMemory->get_address();
}

void* MPOContext::shared_offset_to_abs( U64 pSharedOffset, void* pMemory )
{
    return reinterpret_cast< char* >( pMemory ) + pSharedOffset;
}

U64 MPOContext::allocate_shared( U64 size, U64 alignment )
{
    VERIFY_RTE_MSG( m_pSharedMemory.get(), "Shared memory not initialised" );
    char* pMem = reinterpret_cast< char* >( m_pSharedMemory->allocate_aligned( size, alignment ) );
    return pMem - reinterpret_cast< char* >( m_pSharedMemory->get_address() );
}

void* MPOContext::allocate_heap( U64 size, U64 alignment )
{
    return new ( std::align_val_t( alignment ) ) char[ size ];
}

void MPOContext::get_object_shared_alloc( const char* pszUnitName, mega::TypeID objectTypeID,
                                          mega::runtime::SharedCtorFunction* ppFunction )
{
    getLeafJITRequest().GetObjectSharedAlloc(
        network::convert( pszUnitName ), objectTypeID, network::convert( ppFunction ) );
}
void MPOContext::get_object_shared_del( const char* pszUnitName, mega::TypeID objectTypeID,
                                        mega::runtime::SharedDtorFunction* ppFunction )
{
    getLeafJITRequest().GetObjectSharedDel(
        network::convert( pszUnitName ), objectTypeID, network::convert( ppFunction ) );
}
void MPOContext::get_object_heap_alloc( const char* pszUnitName, mega::TypeID objectTypeID,
                                        mega::runtime::HeapCtorFunction* ppFunction )
{
    getLeafJITRequest().GetObjectHeapAlloc(
        network::convert( pszUnitName ), objectTypeID, network::convert( ppFunction ) );
}
void MPOContext::get_object_heap_del( const char* pszUnitName, mega::TypeID objectTypeID,
                                      mega::runtime::HeapDtorFunction* ppFunction )
{
    getLeafJITRequest().GetObjectHeapDel(
        network::convert( pszUnitName ), objectTypeID, network::convert( ppFunction ) );
}
void MPOContext::get_call_getter( const char* pszUnitName, mega::TypeID objectTypeID,
                                  mega::runtime::TypeErasedFunction* ppFunction )
{
    getLeafJITRequest().GetCallGetter(
        network::convert( pszUnitName ), objectTypeID, network::convert( ppFunction ) );
}
void MPOContext::get_allocate( const char* pszUnitName, const mega::InvocationID& invocationID,
                               mega::runtime::AllocateFunction* ppFunction )
{
    getLeafJITRequest().GetAllocate(
        network::convert( pszUnitName ), invocationID, network::convert( ppFunction ) );
}
void MPOContext::get_read( const char* pszUnitName, const mega::InvocationID& invocationID,
                           mega::runtime::ReadFunction* ppFunction )
{
    getLeafJITRequest().GetRead(
        network::convert( pszUnitName ), invocationID, network::convert( ppFunction ) );
}
void MPOContext::get_write( const char* pszUnitName, const mega::InvocationID& invocationID,
                            mega::runtime::WriteFunction* ppFunction )
{
    getLeafJITRequest().GetWrite(
        network::convert( pszUnitName ), invocationID, network::convert( ppFunction ) );
}
void MPOContext::get_call( const char* pszUnitName, const mega::InvocationID& invocationID,
                           mega::runtime::CallFunction* ppFunction )
{
    getLeafJITRequest().GetCall(
        network::convert( pszUnitName ), invocationID, network::convert( ppFunction ) );
}
void MPOContext::get_start( const char* pszUnitName, const mega::InvocationID& invocationID,
                            mega::runtime::StartFunction* ppFunction )
{
    getLeafJITRequest().GetStart(
        network::convert( pszUnitName ), invocationID, network::convert( ppFunction ) );
}
void MPOContext::get_stop( const char* pszUnitName, const mega::InvocationID& invocationID,
                           mega::runtime::StopFunction* ppFunction )
{
    getLeafJITRequest().GetStop(
        network::convert( pszUnitName ), invocationID, network::convert( ppFunction ) );
}

} // namespace mega
