
#ifndef COMPONENT_MANAGER_20_JUNE_2022
#define COMPONENT_MANAGER_20_JUNE_2022

#include "database.hpp"

#include "runtime/runtime_functions.hpp"

#include "service/protocol/common/project.hpp"

#include "boost/filesystem/path.hpp"
#include "boost/dll/import.hpp"
#include "boost/dll/alias.hpp"
#include "boost/dll/library_info.hpp"

#include <map>
#include <memory>
#include <unordered_map>

namespace mega
{
namespace runtime
{

class ComponentManager
{
    using ComponentPath = boost::dll::fs::path;

    using FunctionPtr    = boost::shared_ptr< TypeErasedFunction* >;
    using FunctionPtrMap = std::unordered_map< mega::TypeID, FunctionPtr >;

    struct InterfaceComponent
    {
        using Ptr = std::shared_ptr< InterfaceComponent >;

        static ComponentPath makeTempComponent( const ComponentPath& path );
        InterfaceComponent( const ComponentPath& path, FunctionPtrMap& functions );

        const ComponentPath& getPath() const { return m_path; }

    private:
        ComponentPath              m_path;
        ComponentPath              m_tempPath;
        boost::dll::library_info   m_libraryInfo;
        boost::dll::shared_library m_library;
    };

    using InterfaceComponentMap = std::map< ComponentPath, InterfaceComponent::Ptr >;

public:
    using Ptr = std::unique_ptr< ComponentManager >;

    ComponentManager( const mega::network::Project& project, DatabaseInstance& database );

    TypeErasedFunction getOperationFunctionPtr( mega::TypeID concreteTypeID );

private:
    void loadComponent() {}

    const mega::network::Project& m_project;
    DatabaseInstance&             m_database;
    InterfaceComponentMap         m_interfaceComponents;
    FunctionPtrMap                m_functions;
};

} // namespace runtime
} // namespace mega

#endif // COMPONENT_MANAGER_20_JUNE_2022
