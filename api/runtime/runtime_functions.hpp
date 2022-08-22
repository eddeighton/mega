#ifndef RUNTIME_FUNCTIONS_17_AUG_2022
#define RUNTIME_FUNCTIONS_17_AUG_2022

#include "mega/common.hpp"

namespace mega
{
namespace runtime
{

using AllocateFunction = mega::reference ( * )( mega::ExecutionContext& executionContext, const mega::reference& );
using ReadFunction = void* ( * )( mega::ExecutionContext& executionContext, const mega::reference& );

} // namespace runtime
} // namespace mega

#endif // RUNTIME_FUNCTIONS_17_AUG_2022
