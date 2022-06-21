
#include "runtime/runtime.hpp"
#include "runtime/component_manager.hpp"

namespace
{
mega::runtime::ComponentManager::Ptr g_componentManager;

}

namespace mega
{
namespace runtime
{
void initialiseRuntime( const mega::network::Project& project )
{
    if ( !g_componentManager )
    {
        g_componentManager = std::make_unique< mega::runtime::ComponentManager >( project );
    }
    else
    {
        g_componentManager->reinitialise( project );
    }
}
} // namespace runtime
} // namespace mega

void* resolveOperation( int typeID ) { return nullptr; }

void* resolveInvocation( int typeID, int* typePath, int typePathSize, int OperationID ) { return nullptr; }
