#ifndef RUNTIME_18_JUNE_2022
#define RUNTIME_18_JUNE_2022


#include "service/protocol/common/project.hpp"

extern "C"
{
    void* resolveOperation( int typeID );
    void* resolveInvocation( int typeID, int* typePath, int typePathSize, int OperationID );
}

namespace mega
{
namespace runtime
{
void initialiseRuntime( const mega::network::Project& project );
}
} // namespace mega

#endif // RUNTIME_18_JUNE_2022
