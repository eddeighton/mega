#ifndef RUNTIME_18_JUNE_2022
#define RUNTIME_18_JUNE_2022

#include "boost/filesystem/path.hpp"

extern "C"
{
void* resolveOperation( int typeID );
void* resolveInvocation( int typeID, int* typePath, int typePathSize, int OperationID );
}

void initialiseRuntime( const boost::filesystem::path& archiveDatabase );

#endif // RUNTIME_18_JUNE_2022
