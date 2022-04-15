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


#include "command_utils.hpp"

#include "database/common/component_info.hpp"
#include "database/common/archive.hpp"

#include "database/model/manifest.hxx"
#include "database/model/environment.hxx"
#include "database/model/ComponentListing.hxx"

#include "common/scheduler.hpp"
#include "common/file.hpp"
#include "common/assert_verify.hpp"
#include "common/terminal.hpp"
#include "common/stash.hpp"

#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>

#include <string>
#include <vector>
#include <iostream>

namespace driver
{
    namespace manifest
    {
        class BaseTask : public task::Task
        {
        public:
            BaseTask( const mega::io::Environment& environment,
                      task::Stash&                 stash,
                      const RawPtrSet&             dependencies )
                : task::Task( dependencies )
                , m_environment( environment )
                , m_stash( stash )
            {
            }

        protected:
            const mega::io::Environment& m_environment;
            task::Stash&                 m_stash;
        };

        class Task_GenerateManifest : public BaseTask
        {
            const std::vector< boost::filesystem::path >& m_componentInfoPaths;

        public:
            Task_GenerateManifest(
                const mega::io::Environment&                  environment,
                task::Stash&                                  stash,
                const std::vector< boost::filesystem::path >& componentInfoPaths )
                : BaseTask( environment, stash, {} )
                , m_componentInfoPaths( componentInfoPaths )
            {
            }
            virtual void run( task::Progress& taskProgress )
            {
                const mega::io::Environment::Path projectManifestPath
                    = m_environment.project_manifest();

                taskProgress.start(
                    "Task_GenerateManifest", m_environment.rootBuildDir(), projectManifestPath );

                const mega::io::Manifest manifest( m_environment, m_componentInfoPaths );
                manifest.save( projectManifestPath );

                VERIFY_RTE_MSG(
                    boost::filesystem::exists( projectManifestPath ),
                    "Failed to create manifest file: " << projectManifestPath.string() );

                taskProgress.succeeded();
            }
        };

        class Task_GenerateComponents : public BaseTask
        {
            const std::vector< boost::filesystem::path >& m_componentInfoPaths;

        public:
            Task_GenerateComponents(
                Task_GenerateManifest*                        pTask,
                const mega::io::Environment&                  environment,
                task::Stash&                                  stash,
                const std::vector< boost::filesystem::path >& componentInfoPaths )
                : BaseTask( environment, stash, { pTask } )
                , m_componentInfoPaths( componentInfoPaths )
            {
            }

            virtual void run( task::Progress& taskProgress )
            {
                using namespace mega;

                const io::Environment::Path projectManifestPath = m_environment.project_manifest();

                taskProgress.start( "Task_GenerateComponents",
                                    m_environment.rootBuildDir(),
                                    m_environment.ComponentListing_Components( mega::io::manifest{ projectManifestPath } ) );

                using namespace ComponentListing;
                Database database( m_environment, projectManifestPath );

                for ( const boost::filesystem::path& componentInfoPath : m_componentInfoPaths )
                {
                    io::ComponentInfo componentInfo;
                    {
                        VERIFY_RTE_MSG( boost::filesystem::exists( componentInfoPath ),
                                        "Failed to locate file: " << componentInfoPath.string() );
                        std::ifstream inputFileStream( componentInfoPath.native().c_str(),
                                                       std::ios::in | std::ios_base::binary );
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
                                                     componentInfo.getIncludeDirectories(),
                                                     componentInfo.getSourceFiles() ) );

                    VERIFY_RTE( pComponent->get_name() == componentInfo.getName() );
                }

                database.store();

                taskProgress.succeeded();
            }
        };

        void command( bool bHelp, const std::vector< std::string >& args )
        {
            boost::filesystem::path rootSourceDir, rootBuildDir;
            std::string             projectName, strComponentInfoPaths;

            namespace po = boost::program_options;
            po::options_description commandOptions( " Compile Mega Project Interface" );
            {
                // clang-format off
            commandOptions.add_options()
                ( "root_src_dir",   po::value< boost::filesystem::path >( &rootSourceDir ), "Root source directory" )
                ( "root_build_dir", po::value< boost::filesystem::path >( &rootBuildDir ),  "Root build directory" )
                ( "project",        po::value< std::string >( &projectName ),               "Mega Project Name" )
                ( "components",     po::value< std::string >( &strComponentInfoPaths ),     "Component info files" )
                ;
                // clang-format on
            }

            po::positional_options_description p;
            p.add( "components", -1 );

            po::variables_map vm;
            po::store(
                po::command_line_parser( args ).options( commandOptions ).positional( p ).run(),
                vm );
            po::notify( vm );

            if ( bHelp )
            {
                std::cout << commandOptions << "\n";
            }
            else
            {
                mega::io::Environment environment( rootSourceDir, rootBuildDir );

                task::Stash stash( environment.stashDir() );

                const std::vector< boost::filesystem::path > componentInfoPaths
                    = pathListToFiles( parsePathList( strComponentInfoPaths ) );

                task::Task::PtrVector tasks;

                Task_GenerateManifest* pTask
                    = new Task_GenerateManifest( environment, stash, componentInfoPaths );
                tasks.push_back( task::Task::Ptr( pTask ) );

                Task_GenerateComponents* pTaskComponents
                    = new Task_GenerateComponents( pTask, environment, stash, componentInfoPaths );
                tasks.push_back( task::Task::Ptr( pTaskComponents ) );

                task::Schedule::Ptr pSchedule( new task::Schedule( tasks ) );
                task::run( pSchedule, std::cout );
            }
        }
    } // namespace manifest
} // namespace driver
