
#include "runtime/runtime.hpp"
#include "runtime/component_manager.hpp"

namespace
{
    mega::runtime::ComponentManager::Ptr g_componentManager;

}

void initialiseRuntime( const boost::filesystem::path& archiveDatabase )
{
    if( !g_componentManager )
    {
        g_componentManager = std::make_unique< mega::runtime::ComponentManager >( archiveDatabase );
    }
    else
    {
        g_componentManager->reinitialise( archiveDatabase );
    }
}

void* resolveOperation( int typeID )
{

    return nullptr;
}

void* resolveInvocation( int typeID, int* typePath, int typePathSize, int OperationID )
{

    return nullptr;
}
