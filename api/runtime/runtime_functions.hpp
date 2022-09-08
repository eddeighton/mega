#ifndef RUNTIME_FUNCTIONS_17_AUG_2022
#define RUNTIME_FUNCTIONS_17_AUG_2022

#include "mega/common.hpp"

namespace mega
{
namespace runtime
{
using GetHeapFunction      = void* ( * )( mega::PhysicalAddress address );
using GetSharedFunction    = void* ( * )( mega::PhysicalAddress address );
using AllocationFunction   = mega::PhysicalAddress ( * )( mega::ExecutionIndex executionIndex );
using DeAllocationFunction = void ( * )( mega::PhysicalAddress address );
using AllocateFunction     = mega::reference ( * )( const mega::reference& );
using ReadFunction         = void* ( * )( const mega::reference& );

} // namespace runtime
} // namespace mega

#endif // RUNTIME_FUNCTIONS_17_AUG_2022
