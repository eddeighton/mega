
#ifndef COMPONENT_MANAGER_20_JUNE_2022
#define COMPONENT_MANAGER_20_JUNE_2022


#include "boost/filesystem/path.hpp"
#include "boost/dll/import.hpp"
#include "boost/dll/alias.hpp"
#include "boost/dll/library_info.hpp"

#include <map>
#include <memory>

namespace mega
{
namespace runtime
{

class ComponentManager
{
    using ComponentPath = boost::dll::fs::path;

    struct InterfaceComponent
    {
        using Ptr = std::shared_ptr< InterfaceComponent >;

        InterfaceComponent( const ComponentPath& path )
            : m_path( path )
            , m_library( path )
        {
        }

    private:
        ComponentPath              m_path;
        boost::dll::shared_library m_library;
    };

    struct LinkComponent
    {
        using Ptr = std::shared_ptr< LinkComponent >;

        LinkComponent( const ComponentPath& path )
            : m_path( path )
            , m_library( path )
        {
        }

    private:
        ComponentPath              m_path;
        boost::dll::shared_library m_library;
    };

    using InterfaceComponentMap = std::map< ComponentPath, InterfaceComponent::Ptr >;
    using LinkComponentMap      = std::map< ComponentPath, LinkComponent::Ptr >;

public:
    using Ptr = std::unique_ptr< ComponentManager >;

    ComponentManager();
    ~ComponentManager();

private:
    InterfaceComponentMap                           m_interfaceComponents;
    LinkComponentMap                                m_linkComponents;
};

} // namespace runtime
} // namespace mega

#endif // COMPONENT_MANAGER_20_JUNE_2022
