#ifndef RUNTIME_FUNCTIONS_17_AUG_2022
#define RUNTIME_FUNCTIONS_17_AUG_2022

#include "mega/common.hpp"

namespace mega
{
namespace runtime
{
using TypeErasedFunction   = const void*;
using GetHeapFunction      = void* ( * )( mega::PhysicalAddress address );
using GetSharedFunction    = void* ( * )( mega::PhysicalAddress address );
using AllocationFunction   = mega::PhysicalAddress ( * )( mega::ExecutionIndex executionIndex );
using DeAllocationFunction = void ( * )( mega::PhysicalAddress address );
using AllocateFunction     = mega::reference ( * )( mega::reference );
using ReadFunction         = void* ( * )( mega::reference );

struct WriteResult
{
    void*           pData;
    mega::reference context;
};
using WriteFunction = WriteResult ( * )( mega::reference );

struct CallResult
{
    TypeErasedFunction pFunction;
    mega::reference    context;
};
using CallFunction = CallResult ( * )( mega::reference );

} // namespace runtime
} // namespace mega

#endif // RUNTIME_FUNCTIONS_17_AUG_2022
