#ifndef RUNTIME_FUNCTIONS_17_AUG_2022
#define RUNTIME_FUNCTIONS_17_AUG_2022

#include "mega/common.hpp"

namespace mega
{
namespace runtime
{

using AllocateFunction = mega::reference ( * )();
using ReadFunction = void* ( * )( const mega::reference& );

} // namespace runtime
} // namespace mega

#endif // RUNTIME_FUNCTIONS_17_AUG_2022
