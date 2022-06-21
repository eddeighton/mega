
#ifndef COMPONENT_MANAGER_20_JUNE_2022
#define COMPONENT_MANAGER_20_JUNE_2022


#include "service/protocol/common/project.hpp"

#include "database/common/environment_archive.hpp"

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

    struct FrontEndComponent
    {
        using Ptr = std::shared_ptr< FrontEndComponent >;

        FrontEndComponent( const ComponentPath& path )
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
        using Ptr = std::shared_ptr< FrontEndComponent >;

        LinkComponent( const ComponentPath& path )
            : m_path( path )
            , m_library( path )
        {
        }

    private:
        ComponentPath              m_path;
        boost::dll::shared_library m_library;
    };

    using FrontEndComponentMap = std::map< ComponentPath, FrontEndComponent::Ptr >;
    using LinkComponentMap     = std::map< ComponentPath, LinkComponent::Ptr >;

public:
    using Ptr = std::unique_ptr< ComponentManager >;

    ComponentManager( const mega::network::Project& project );
    ~ComponentManager();

    void reinitialise( const mega::network::Project& project );

private:
    std::unique_ptr< mega::io::ArchiveEnvironment > m_pArchive;
    FrontEndComponentMap                            m_frontEndComponents;
    LinkComponentMap                                m_linkComponents;
};

} // namespace runtime
} // namespace mega

#endif // COMPONENT_MANAGER_20_JUNE_2022
