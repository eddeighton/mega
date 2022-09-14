#ifndef RUNTIME_FUNCTIONS_17_AUG_2022
#define RUNTIME_FUNCTIONS_17_AUG_2022

#include "mega/common.hpp"

namespace mega
{
namespace runtime
{
using TypeErasedFunction = const void*;
using GetHeapFunction    = void* ( * )( MachineAddress address );
using GetSharedFunction  = void* ( * )( MachineAddress address );

using SharedCtorFunction = void ( * )( void* );
using HeapCtorFunction   = void ( * )( void* );
using SharedDtorFunction = void ( * )( void* );
using HeapDtorFunction   = void ( * )( void* );

using AllocationFunction   = MachineAddress ( * )( ExecutorID mpe );
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

NetworkAddress allocateNetworkAddress( MPEStorage mpe, TypeID objectTypeID );
reference      networkToMachine( MPEStorage mpe, TypeID objectTypeID, NetworkAddress networkAddress );
void           get_getter_heap( const char* pszUnitName, TypeID objectTypeID, GetHeapFunction* ppFunction );
void           get_getter_shared( const char* pszUnitName, TypeID objectTypeID, GetSharedFunction* ppFunction );
void           get_getter_call( const char* pszUnitName, TypeID objectTypeID, TypeErasedFunction* ppFunction );


struct Allocator
{
    virtual void* get_shared( mega::MachineAddress address ) = 0;
    virtual void* get_heap( mega::MachineAddress address ) = 0;
};
using SetAllocatorFunction = void (*)( Allocator* );

} // namespace runtime
} // namespace mega

#endif // RUNTIME_FUNCTIONS_17_AUG_2022
