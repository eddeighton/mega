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

#include "database/io/component_info.hpp"
#include "database/io/environment.hpp"

#include "common/scheduler.hpp"
#include "common/assert_verify.hpp"
#include "common/stash.hpp"

#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <string>
#include <vector>
#include <iostream>

namespace driver
{
namespace list
{
    class BaseTask : public task::Task
    {
    public:
        BaseTask( const mega::io::Environment& environment, task::Stash& stash, const RawPtrSet& dependencies )
            : task::Task( dependencies )
            , m_environment( environment )
            , m_stash( stash )
        {
        }

    protected:
        const mega::io::Environment& m_environment;
        task::Stash&                 m_stash;
    };

    class Task_ComponentInfoToManifest : public BaseTask
    {
    public:
        Task_ComponentInfoToManifest( const mega::io::Environment&                  environment,
                                      task::Stash&                                  stash,
                                      const std::string&                            strComponentName,
                                      const boost::filesystem::path&                srcDir,
                                      const boost::filesystem::path&                buildDir,
                                      const std::vector< boost::filesystem::path >& inputMegaSourceFiles,
                                      const std::vector< boost::filesystem::path >& includeDirectories )
            : BaseTask( environment, stash, {} )
            , m_srcDir( srcDir )
            , m_buildDir( buildDir )
            , m_componentInfo( strComponentName, srcDir, inputMegaSourceFiles, includeDirectories )
        {
        }

        virtual void run( task::Progress& taskProgress )
        {
            // inputMegaSourceFiles
            const mega::io::Environment::Path sourceListPath = m_environment.source_list( m_buildDir );

            taskProgress.start( "Task_ComponentInfoToManifest",
                                m_srcDir,
                                sourceListPath );

            std::ofstream outputFileStream( sourceListPath.native().c_str(), std::ios_base::trunc | std::ios_base::out );
            if ( !outputFileStream.good() )
            {
                THROW_RTE( "Failed to write to file: " << sourceListPath.string() );
            }
            outputFileStream << m_componentInfo;

            taskProgress.succeeded();
        }

    private:
        const boost::filesystem::path m_srcDir;
        const boost::filesystem::path m_buildDir;
        mega::io::ComponentInfo       m_componentInfo;
    };

    void command( bool bHelp, const std::vector< std::string >& args )
    {
        boost::filesystem::path    rootSourceDir, rootBuildDir, sourceDir, buildDir;
        std::string                strComponentName, strIncludeDirectories;
        std::vector< std::string > objectSourceFileNames;

        namespace po = boost::program_options;
        po::options_description commandOptions( " List input mega source files" );
        {
            // clang-format off
            commandOptions.add_options()
                ( "root_src_dir",   po::value< boost::filesystem::path >( &rootSourceDir ),             "Root source directory" )
                ( "root_build_dir", po::value< boost::filesystem::path >( &rootBuildDir ),              "Root build directory" )
                ( "src_dir",        po::value< boost::filesystem::path >( &sourceDir ),                 "Source directory" )
                ( "build_dir",      po::value< boost::filesystem::path >( &buildDir ),                  "Build Directory" )
                ( "name",           po::value< std::string >( &strComponentName ),                      "Component name" )       
                ( "include_dirs",   po::value< std::string >( &strIncludeDirectories ),                 "Include directories ( semicolon delimited )" )
                ( "src",            po::value< std::vector< std::string > >( &objectSourceFileNames ),  "Mega source file names" )
                ;
            // clang-format on
        }

        po::positional_options_description p;
        p.add( "src", -1 );

        po::variables_map vm;
        po::store( po::command_line_parser( args ).options( commandOptions ).positional( p ).run(), vm );
        po::notify( vm );

        if ( bHelp )
        {
            std::cout << commandOptions << "\n";
        }
        else
        {
            // tokenize semi colon delimited names into absolute mega source file paths
            const std::vector< boost::filesystem::path > inputSourceFiles = pathListToFiles( sourceDir, objectSourceFileNames );
            const std::vector< boost::filesystem::path > includeDirectories = pathListToFolders( parsePathList( strIncludeDirectories ) );

            mega::io::Environment environment( rootSourceDir, rootBuildDir );

            task::Stash stash( environment.stashDir() );

            task::Task::PtrVector tasks;

            Task_ComponentInfoToManifest* pTask = new Task_ComponentInfoToManifest(
                environment,
                stash,
                strComponentName,
                sourceDir,
                buildDir,
                inputSourceFiles,
                includeDirectories );
            tasks.push_back( task::Task::Ptr( pTask ) );

            task::Schedule::Ptr pSchedule( new task::Schedule( tasks ) );
            task::run( pSchedule, std::cout );
        }
    }
} // namespace list
} // namespace driver
