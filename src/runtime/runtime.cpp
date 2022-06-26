
#include "runtime/runtime.hpp"
#include "runtime/runtime_api.hpp"
#include "runtime/component_manager.hpp"

#include <iostream>

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


extern "C"
{
    const int& madeUpFunction( mega::reference data )
    {
        std::cout << "madeUpFunction called" << std::endl;
        static int i = 321;
        return i;
    }

    void* resolveOperation( int typeID )
    {
        //
        return nullptr;
    }

    void resolveInvocation( mega::TypeID typeID, const mega::TypeID* typePath, mega::TypeID typePathSize,
                            mega::TypeID OperationID, void** ppFunction )
    {
        //
        std::cout << "Setting function pointer" << std::endl;
        *ppFunction = (void*)&madeUpFunction;
    }
}
