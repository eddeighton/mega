//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#include "base_task.hpp"

#include "mega_mangle_meta.hxx"

#include "database/common/component_info.hpp"
#include "database/common/serialisation.hpp"

#include "database/model/manifest.hxx"
#include "database/model/environment.hxx"
#include "database/model/ComponentListing.hxx"

#include "utilities/glob.hpp"

#include <common/stash.hpp>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

namespace mega::compiler
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

        start( taskProgress, "Task_GenerateManifest", projectManifestPath.path(), projectManifestPath.path() );

        const mega::io::Manifest manifest( m_environment, m_componentInfoPaths );
        const task::FileHash     hashCode = manifest.save_temp( m_environment, projectManifestPath );
        // const task::DeterminantHash determinant( hashCode );

        /*if ( m_environment.restore( projectManifestPath, determinant ) )
        {
            m_environment.setBuildHashCode( projectManifestPath, hashCode );
            cached( taskProgress );
            return;
        }
        else*/
        {
            m_environment.temp_to_real( projectManifestPath );
            // m_environment.setBuildHashCode( projectManifestPath, hashCode );
            // m_environment.stash( projectManifestPath, determinant );
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

        /*task::DeterminantHash determinant = m_environment.getBuildHashCode( projectManifestPath );
        for ( const boost::filesystem::path& componentInfoPath : m_componentInfoPaths )
        {
            determinant ^= componentInfoPath;
        }

        if ( m_environment.restore( componentsListing, determinant ) )
        {
            m_environment.setBuildHashCode( componentsListing );
            cached( taskProgress );
            return;
        }*/

        std::set< std::string > componentNames;

        using namespace ComponentListing;
        Database database( m_environment, projectManifestPath );

        constructMegaMangle( database );

        for( const boost::filesystem::path& componentInfoPath : m_componentInfoPaths )
        {
            io::ComponentInfo componentInfo;
            {
                VERIFY_RTE_MSG( boost::filesystem::exists( componentInfoPath ),
                                "Failed to locate file: " << componentInfoPath.string() );
                std::ifstream inputFileStream(
                    componentInfoPath.string().c_str(), std::ios::in | std::ios_base::binary );
                if( !inputFileStream.good() )
                {
                    THROW_RTE( "Failed to open file: " << componentInfoPath.string() );
                }
                boost::archive::xml_iarchive ia( inputFileStream );
                ia >> boost::serialization::make_nvp( "componentInfo", componentInfo );
            };

            VERIFY_RTE_MSG( componentNames.count( componentInfo.getName() ) == 0,
                            "Duplicate component name: " << componentInfo.getName() );
            componentNames.insert( componentInfo.getName() );

            const mega::io::ComponentFilePath componentFilePath
                = m_environment.ComponentPath_fromPath( componentInfo.getFilePath() );
            const mega::io::ComponentFilePath pythonFilePath
                = m_environment.PythonComponentPath_fromPath( componentInfo.getFilePath() );
            const mega::io::ComponentFilePath initFilePath
                = m_environment.InitComponentPath_fromPath( componentInfo.getFilePath() );

            std::vector< mega::io::megaFilePath > megaSourceFiles;
            std::vector< mega::io::cppFilePath >  cppSourceFiles;
            std::vector< mega::io::schFilePath >  schSourceFiles;
            for( const boost::filesystem::path& filePath : componentInfo.getSourceFiles() )
            {
                if( filePath.extension() == mega::io::megaFilePath::extension() )
                {
                    megaSourceFiles.push_back( m_environment.megaFilePath_fromPath( filePath ) );
                }
                else if( filePath.extension() == mega::io::cppFilePath::extension() )
                {
                    cppSourceFiles.push_back( m_environment.cppFilePath_fromPath( filePath ) );
                }
                else if( filePath.extension() == mega::io::schFilePath::extension() )
                {
                    schSourceFiles.push_back( m_environment.schFilePath_fromPath( filePath ) );
                }
                else
                {
                    THROW_RTE( "Unknown file type: " << filePath.string() );
                }
            }

            std::vector< mega::io::megaFilePath > dependencies;
            {
                for( const boost::filesystem::path& dependency : componentInfo.getDependencyFiles() )
                {
                    mega::utilities::Glob           glob{ dependency.parent_path(), dependency.filename().string() };
                    mega::utilities::FilePathVector matchedFilePaths;
                    mega::utilities::resolveGlob( glob, m_environment.srcDir(), matchedFilePaths );
                    for( const boost::filesystem::path& matchedFilePath : matchedFilePaths )
                    {
                        dependencies.push_back( m_environment.megaFilePath_fromPath( matchedFilePath ) );
                    }
                }
            }

            Components::Component* pComponent
                = database.construct< Components::Component >( Components::Component::Args(
                    componentInfo.getComponentType(), componentInfo.getName(), componentFilePath, pythonFilePath,
                    initFilePath, componentInfo.getSrcDir(), componentInfo.getBuildDir(), componentInfo.getCPPFlags(),
                    componentInfo.getCPPDefines(), componentInfo.getIncludeDirectories(), dependencies, megaSourceFiles,
                    cppSourceFiles, schSourceFiles ) );

            VERIFY_RTE( pComponent->get_name() == componentInfo.getName() );
        }

        const task::FileHash fileHashCode = database.save_Components_to_temp();
        // m_environment.setBuildHashCode( componentsListing, fileHashCode );
        m_environment.temp_to_real( componentsListing );
        // m_environment.stash( componentsListing, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_GenerateComponents( const TaskArguments&                          taskArguments,
                                              const std::vector< boost::filesystem::path >& componentInfoPaths )
{
    return std::make_unique< Task_GenerateComponents >( taskArguments, componentInfoPaths );
}

} // namespace mega::compiler
