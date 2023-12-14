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

#include "database/component_info.hpp"
#include "database/serialisation.hpp"

#include "database/manifest.hxx"
#include "database/environment.hxx"
#include "database/ComponentListing.hxx"

#include "compiler/glob.hpp"

#include <common/stash.hpp>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

namespace mega::compiler
{

class Task_GenerateComponents : public BaseTask
{
    const std::vector< mega::io::ComponentInfo >& m_componentInfoPaths;

public:
    Task_GenerateComponents( const TaskArguments&                          taskArguments,
                             const std::vector< mega::io::ComponentInfo >& componentInfos )
        : BaseTask( taskArguments )
        , m_componentInfoPaths( componentInfos )
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
        for ( const mega::io::ComponentInfo& componentInfo : m_componentInfoPaths )
        {
            determinant ^= componentInfo.something;
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

        for( const mega::io::ComponentInfo& componentInfo : m_componentInfoPaths )
        {
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
                                              const std::vector< mega::io::ComponentInfo >& componentInfos )
{
    return std::make_unique< Task_GenerateComponents >( taskArguments, componentInfos );
}

} // namespace mega::compiler
