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

#include "parser/parser.hpp"

#include "database/io/file.hpp"
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
#include <boost/filesystem/path.hpp>
#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <boost/dll.hpp>

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

    const mega::io::FileInfo& findFileInfo( const boost::filesystem::path& filePath, const std::vector< mega::io::FileInfo >& fileInfos )
    {
        for ( const mega::io::FileInfo& fileInfo : fileInfos )
        {
            if ( fileInfo.m_filePath == filePath )
            {
                return fileInfo;
            }
        }
        THROW_RTE( "Failed to locate file: " << filePath << " in manifest" );
    }

    class Task_ParseAST : public BaseTask
    {
    public:
        Task_ParseAST( const mega::io::Environment& environment, task::Stash& stash,
                       const boost::filesystem::path&           parserDll,
                       const mega::io::FileInfo&                fileInfo,
                       const std::vector< mega::io::FileInfo >& fileInfos )
            : BaseTask( environment, stash, {} )
            , m_parserDLL( parserDll )
            , m_fileInfo( fileInfo )
            , m_fileInfos( fileInfos )
        {
        }
        virtual void run( task::Progress& taskProgress )
        {
            static boost::shared_ptr< mega::EG_PARSER_INTERFACE > pParserInterface;
            if ( !pParserInterface )
            {
                pParserInterface = boost::dll::import_symbol< mega::EG_PARSER_INTERFACE >(
                    m_parserDLL, "g_parserSymbol" );
            }

            const mega::io::FileInfo& objectAST = findFileInfo( m_environment.objectAST( m_fileInfo.m_filePath ), m_fileInfos );
            const mega::io::FileInfo& objectBody = findFileInfo( m_environment.objectBody( m_fileInfo.m_filePath ), m_fileInfos );

            taskProgress.start( "Task_ParseAST",
                                m_fileInfo.m_filePath,
                                objectAST.m_filePath );

            std::ostringstream osError;

            pParserInterface->parseEGSourceFile( m_environment.rootSourceDir(),
                                                 m_environment.rootBuildDir(),
                                                 m_fileInfo,
                                                 objectAST,
                                                 objectBody,
                                                 osError );

            if ( !osError.str().empty() )
            {
                // Error
            }

            taskProgress.succeeded();
        }

        const mega::io::FileInfo&                m_fileInfo;
        const std::vector< mega::io::FileInfo >& m_fileInfos;
        const boost::filesystem::path            m_parserDLL;
    };

    void command( bool bHelp, const std::vector< std::string >& args )
    {
        boost::filesystem::path rootSourceDir, rootBuildDir, parserDll;
        std::string             projectName;

        namespace po = boost::program_options;
        po::options_description commandOptions( " Compile Mega Project Interface" );
        {
            // clang-format off
            commandOptions.add_options()
                ( "root_src_dir",   po::value< boost::filesystem::path >( &rootSourceDir ), "Root source directory" )
                ( "root_build_dir", po::value< boost::filesystem::path >( &rootBuildDir ),  "Root build directory" )
                ( "project",        po::value< std::string >( &projectName ),               "Mega Project Name" )
                ( "parser_dll",     po::value< boost::filesystem::path >( &parserDll ),     "Parser DLL Path" )
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
            mega::io::Environment environment( rootSourceDir, rootBuildDir );

            task::Stash stash( environment.stashDir() );

            VERIFY_RTE_MSG( boost::filesystem::exists( parserDll ),
                            "Failed to locate parser DLL: " << parserDll );

            // load the manifest
            const mega::io::Environment::Path projectManifestPath = environment.project_manifest();
            mega::io::Manifest                manifest( projectManifestPath );

            std::vector< mega::io::FileInfo > fileInfos;
            manifest.collectFileInfos( fileInfos );

            std::cout << "Project Manifest contains: " << fileInfos.size() << " files" << std::endl;

            task::Task::PtrVector tasks;

            for ( const mega::io::FileInfo& fileInfo : fileInfos )
            {
                std::cout << "Manifest file: " << fileInfo << std::endl;

                switch ( fileInfo.m_fileType )
                {
                case mega::io::FileInfo::ObjectSourceFile:
                {
                    Task_ParseAST* pTask = new Task_ParseAST( environment, stash, parserDll, fileInfo, fileInfos );
                    tasks.push_back( task::Task::Ptr( pTask ) );
                }
                break;
                case mega::io::FileInfo::ObjectAST:
                    break;
                case mega::io::FileInfo::ObjectBody:
                    break;
                case mega::io::FileInfo::TOTAL_FILE_TYPES:
                default:
                    THROW_RTE( "Unknown file type" );
                }
            }

            task::Schedule::Ptr pSchedule( new task::Schedule( tasks ) );
            task::run( pSchedule, std::cout );
        }
    }
} // namespace interface
} // namespace driver
