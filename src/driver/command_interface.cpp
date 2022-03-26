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

#include "environment.hpp"

#include "database/stages/parser.hpp"

#include "database/io/database.hpp"

#include "common/scheduler.hpp"
#include "common/file.hpp"
#include "common/assert_verify.hpp"
#include "common/terminal.hpp"

#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>

#include <string>
#include <vector>
#include <iostream>

namespace driver
{
namespace interface
{
    class BaseTask : public task::Task
    {
    public:
        BaseTask( const Environment& environment, const RawPtrSet& dependencies )
            : task::Task( dependencies )
            , m_environment( environment )
        {
        }

    protected:
        const Environment& m_environment;
    };

    class Task_CollateSourceFiles : public BaseTask
    {
    public:
        Task_CollateSourceFiles( const Environment& environment )
            : BaseTask( environment, {} )
        {
        }
        virtual void run( task::Progress& taskProgress )
        {
            // inputMegaSourceFiles
            const Environment::Path projectManifestPath = m_environment.project_manifest();

            taskProgress.start( "Task_CollateSourceFiles",
                                m_environment.sourceDir(),
                                projectManifestPath );

            using namespace mega::io;

            mega::io::Manifest::PtrCst pManifest = std::make_shared< mega::io::Manifest >( m_environment.buildDir() );

            mega::io::Database< mega::io::stage::Test > database( 
                m_environment.sourceDir(), m_environment.buildDir(), pManifest );

            database.store();

            //Database< stage::Test > testStageDatabase( m_environment.sourceDir(), m_environment.buildDir() );

            //mega::Stages::Parser parserStage( "", m_environment.sourceDir(), m_environment.buildDir() );

            /*TestObject* pTestObject = testStageDatabase.construct<TestObject>();
            
            //mega::io::Database database( m_environment.sourceDir(), m_environment.buildDir() );
            //database.saveProjectManifest();

            TestObject* pAgain = testStageDatabase.one< TestObject >();
            VERIFY_RTE(pAgain);*/


            taskProgress.succeeded();
        }

    };

    void command( bool bHelp, const std::vector< std::string >& args )
    {
        boost::filesystem::path rootSourceDir, rootBuildDir;
        std::string             projectName;

        namespace po = boost::program_options;
        po::options_description commandOptions( " Compile Mega Project Interface" );
        {
            // clang-format off
            commandOptions.add_options()
                ( "root_src_dir",   po::value< boost::filesystem::path >( &rootSourceDir ),     "Root source directory" )
                ( "root_build_dir", po::value< boost::filesystem::path >( &rootBuildDir ),      "Root build directory" )
                ( "project",        po::value< std::string >( &projectName ),                   "Mega Project Name" )
                ;
            // clang-format on
        }

        po::positional_options_description p;
        p.add( "dir", -1 );

        po::variables_map vm;
        po::store( po::command_line_parser( args ).options( commandOptions ).positional( p ).run(), vm );
        po::notify( vm );

        if ( bHelp )
        {
            std::cout << commandOptions << "\n";
        }
        else
        {

            Environment environment( rootSourceDir, rootBuildDir, rootSourceDir, rootBuildDir );

            task::Task::PtrVector tasks;

            Task_CollateSourceFiles* pTask = new Task_CollateSourceFiles( environment );
            tasks.push_back( task::Task::Ptr( pTask ) );

            task::Schedule::Ptr pSchedule( new task::Schedule( tasks ) );
            task::run( pSchedule, std::cout );
        }
    }
} // namespace list
} // namespace driver
