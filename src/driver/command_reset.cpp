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

#include "database/io/manifest.hpp"
#include "database/io/environment.hpp"

#include "database/stages/parser.hpp"

#include "database/io/database.hpp"

#include "common/scheduler.hpp"
#include "common/file.hpp"
#include "common/assert_verify.hpp"
#include "common/terminal.hpp"
#include "common/stash.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>

#include <string>
#include <vector>
#include <iostream>

namespace driver
{
namespace reset
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

    class Task_ResetSourceListings : public BaseTask
    {
    public:
        Task_ResetSourceListings( const mega::io::Environment& environment, task::Stash& stash )
            : BaseTask( environment, stash, {} )
        {
        }
        virtual void run( task::Progress& taskProgress )
        {
            taskProgress.start( "Task_ResetSourceListings",
                                m_environment.buildDir(),
                                m_environment.buildDir() );

            const boost::filesystem::path projectManifestFilePath = m_environment.project_manifest();
            if ( boost::filesystem::exists( projectManifestFilePath ) )
            {
                boost::filesystem::remove( projectManifestFilePath );
            }

            mega::io::Manifest::reset( m_environment.sourceDir(), m_environment.buildDir() );

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
            mega::io::Environment environment( rootSourceDir, rootBuildDir, rootSourceDir, rootBuildDir );

            task::Stash stash( environment.stashDir() );

            task::Task::PtrVector tasks;

            Task_ResetSourceListings* pTask = new Task_ResetSourceListings( environment, stash );
            tasks.push_back( task::Task::Ptr( pTask ) );

            task::Schedule::Ptr pSchedule( new task::Schedule( tasks ) );
            task::run( pSchedule, std::cout );
        }
    }
} // namespace reset
} // namespace driver
