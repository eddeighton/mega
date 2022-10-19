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

#include "runtime/mpo_context.hpp"

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
    //
    THROW_TODO;
}
void* MPOContext::shared_offset_to_abs( U64 pSharedOffset, void* pMemory ) { THROW_TODO; }
void* MPOContext::allocate_shared( U64 size, U64 alignment, void** pMemory ) { THROW_TODO; }
void* MPOContext::allocate_heap( U64 size, U64 alignment ) { THROW_TODO; }
void  MPOContext::get_object_shared_alloc( const char* pszUnitName, mega::TypeID objectTypeID,
                                           mega::runtime::SharedCtorFunction* ppFunction )
{
    THROW_TODO;
}
void MPOContext::get_object_shared_del( const char* pszUnitName, mega::TypeID objectTypeID,
                                        mega::runtime::SharedCtorFunction* ppFunction )
{
    THROW_TODO;
}
void MPOContext::get_object_heap_alloc( const char* pszUnitName, mega::TypeID objectTypeID,
                                        mega::runtime::SharedCtorFunction* ppFunction )
{
    THROW_TODO;
}
void MPOContext::get_object_heap_del( const char* pszUnitName, mega::TypeID objectTypeID,
                                      mega::runtime::SharedCtorFunction* ppFunction )
{
    THROW_TODO;
}
void MPOContext::get_getter_call( const char* pszUnitName, mega::TypeID objectTypeID,
                                  mega::runtime::TypeErasedFunction* ppFunction )
{
    THROW_TODO;
}
void MPOContext::get_allocate( const char* pszUnitName, const mega::InvocationID& invocationID,
                               mega::runtime::AllocateFunction* ppFunction )
{
    THROW_TODO;
}
void MPOContext::get_read( const char* pszUnitName, const mega::InvocationID& invocationID,
                           mega::runtime::ReadFunction* ppFunction )
{
    THROW_TODO;
}
void MPOContext::get_write( const char* pszUnitName, const mega::InvocationID& invocationID,
                            mega::runtime::WriteFunction* ppFunction )
{
    THROW_TODO;
}
void MPOContext::get_call( const char* pszUnitName, const mega::InvocationID& invocationID,
                           mega::runtime::CallFunction* ppFunction )
{
    THROW_TODO;
}
void MPOContext::get_start( const char* pszUnitName, const mega::InvocationID& invocationID,
                            mega::runtime::StartFunction* ppFunction )
{
    THROW_TODO;
}
void MPOContext::get_stop( const char* pszUnitName, const mega::InvocationID& invocationID,
                           mega::runtime::StopFunction* ppFunction )
{
    THROW_TODO;
}

} // namespace mega
