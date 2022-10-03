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

#ifndef RUNTIME_FUNCTIONS_17_AUG_2022
#define RUNTIME_FUNCTIONS_17_AUG_2022

#include "/workspace/root/src/mega/src/api/mega/common.hpp"

namespace mega::runtime
{
using TypeErasedFunction = const void*;
using GetHeapFunction    = void* ( * )( MachineAddress address );
using GetSharedFunction  = void* ( * )( MachineAddress address );

using SharedCtorFunction = void ( * )( void*, void* );
using HeapCtorFunction   = void ( * )( void* );
using SharedDtorFunction = void ( * )( void* );
using HeapDtorFunction   = void ( * )( void* );

using AllocationFunction   = MachineAddress ( * )( OwnerID mpo );
using DeAllocationFunction = void ( * )( MachineAddress address );
using AllocateFunction     = reference ( * )( reference );
using ReadFunction         = void* ( * )( reference );

struct WriteResult
{
    void*     pData;
    reference context;
};
using WriteFunction = WriteResult ( * )( reference );

struct CallResult
{
    TypeErasedFunction pFunction;
    reference          context;
};
using CallFunction = CallResult ( * )( reference );

NetworkAddress allocateNetworkAddress( MPO mpo, TypeID objectTypeID );
reference      allocateMachineAddress( MPO mpo, TypeID objectTypeID, NetworkAddress networkAddress );
reference      networkToMachine( TypeID objectTypeID, NetworkAddress networkAddress );

void get_getter_heap( const char* pszUnitName, TypeID objectTypeID, GetHeapFunction* ppFunction );
void get_getter_shared( const char* pszUnitName, TypeID objectTypeID, GetSharedFunction* ppFunction );
void get_getter_call( const char* pszUnitName, TypeID objectTypeID, TypeErasedFunction* ppFunction );

struct Allocator
{
    virtual void* get_shared( mega::MachineAddress address ) = 0;
    virtual void* get_heap( mega::MachineAddress address )   = 0;
};
using SetAllocatorFunction = void ( * )( Allocator* );

} // namespace mega::runtime

#endif // RUNTIME_FUNCTIONS_17_AUG_2022
