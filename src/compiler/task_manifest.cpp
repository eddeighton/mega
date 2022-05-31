
#include "base_task.hpp"

#include "database/common/component_info.hpp"
#include "database/common/serialisation.hpp"
#include "database/common/environments.hpp"

#include "database/model/manifest.hxx"
#include "database/model/environment.hxx"
#include "database/model/ComponentListing.hxx"

#include <common/stash.hpp>

namespace driver
{
namespace interface
{

class Task_GenerateManifest : public BaseTask
{
    const std::vector< boost::filesystem::path >& m_componentInfoPaths;

public:
    Task_GenerateManifest( const TaskArguments&                          taskArguments,
                           const std::vector< boost::filesystem::path >& componentInfoPaths )
        : BaseTask( taskArguments )
        , m_componentInfoPaths( componentInfoPaths )
    {
    }
    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::manifestFilePath projectManifestPath = m_environment.project_manifest();

        start( taskProgress, "Task_GenerateManifest", boost::filesystem::path{}, projectManifestPath.path() );
        // make a note of the schema version
        //{
        //    std::ostringstream os;
        //    os << "Schema version: " << mega::io::Environment::VERSION;
        //    msg( taskProgress, os.str() );
        //}

        const mega::io::Manifest    manifest( m_environment, m_componentInfoPaths );
        const task::FileHash        hashCode = manifest.save_temp( m_environment, projectManifestPath );
        const task::DeterminantHash determinant( hashCode );

        if ( m_environment.restore( projectManifestPath, determinant ) )
        {
            m_environment.setBuildHashCode( projectManifestPath, hashCode );
            cached( taskProgress );
            return;
        }
        else
        {
            m_environment.temp_to_real( projectManifestPath );
            m_environment.setBuildHashCode( projectManifestPath, hashCode );
            m_environment.stash( projectManifestPath, determinant );
            succeeded( taskProgress );
        }
    }
};

BaseTask::Ptr create_Task_GenerateManifest( const TaskArguments&                          taskArguments,
                                           const std::vector< boost::filesystem::path >& componentInfoPaths )
{
    return std::make_unique< Task_GenerateManifest >( taskArguments, componentInfoPaths );
}

class Task_GenerateComponents : public BaseTask
{
    const std::vector< boost::filesystem::path >& m_componentInfoPaths;

public:
    Task_GenerateComponents( const TaskArguments&                          taskArguments,
                             const std::vector< boost::filesystem::path >& componentInfoPaths )
        : BaseTask( taskArguments )
        , m_componentInfoPaths( componentInfoPaths )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        using namespace mega;

        const io::manifestFilePath    projectManifestPath = m_environment.project_manifest();
        const io::CompilationFilePath componentsListing
            = m_environment.ComponentListing_Components( projectManifestPath );

        start( taskProgress, "Task_GenerateComponents", projectManifestPath.path(), componentsListing.path() );

        task::DeterminantHash determinant = m_environment.getBuildHashCode( projectManifestPath );
        for ( const boost::filesystem::path& componentInfoPath : m_componentInfoPaths )
        {
            determinant ^= componentInfoPath;
        }

        if ( m_environment.restore( componentsListing, determinant ) )
        {
            m_environment.setBuildHashCode( componentsListing );
            cached( taskProgress );
            return;
        }

        using namespace ComponentListing;
        Database database( m_environment, projectManifestPath );

        for ( const boost::filesystem::path& componentInfoPath : m_componentInfoPaths )
        {
            io::ComponentInfo componentInfo;
            {
                VERIFY_RTE_MSG( boost::filesystem::exists( componentInfoPath ),
                                "Failed to locate file: " << componentInfoPath.string() );
                std::ifstream inputFileStream(
                    componentInfoPath.native().c_str(), std::ios::in | std::ios_base::binary );
                if ( !inputFileStream.good() )
                {
                    THROW_RTE( "Failed to open file: " << componentInfoPath.string() );
                }
                InputArchiveType ia( inputFileStream );
                ia >> boost::serialization::make_nvp( "componentInfo", componentInfo );
            };

            Components::Component* pComponent = database.construct< Components::Component >(
                Components::Component::Args( componentInfo.getName(),
                                             componentInfo.getDirectory(),
                                             componentInfo.getCPPFlags(),
                                             componentInfo.getCPPDefines(),
                                             componentInfo.getIncludeDirectories(),
                                             componentInfo.getSourceFiles() ) );

            VERIFY_RTE( pComponent->get_name() == componentInfo.getName() );
        }

        const task::FileHash fileHashCode = database.save_Components_to_temp();
        m_environment.setBuildHashCode( componentsListing, fileHashCode );
        m_environment.temp_to_real( componentsListing );
        m_environment.stash( componentsListing, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_GenerateComponents( const TaskArguments&                          taskArguments,
                                             const std::vector< boost::filesystem::path >& componentInfoPaths )
{
    return std::make_unique< Task_GenerateComponents >( taskArguments, componentInfoPaths );
}

} // namespace interface
} // namespace driver
