#ifndef RUNTIME_API_18_JUNE_2022
#define RUNTIME_API_18_JUNE_2022

#include "mega/common.hpp"

extern "C"
{
    void* resolveOperation( int typeID );
    void resolveInvocation( mega::TypeID typeID, const mega::TypeID* typePath, mega::TypeID typePathSize, mega::TypeID OperationID, void* ppFunction );
}

#endif // RUNTIME_API_18_JUNE_2022
