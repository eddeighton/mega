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

#include "database/model/component.hpp"
#include "database/model/input.hpp"
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
        BaseTask( const RawPtrSet& dependencies, const mega::io::Environment& environment, task::Stash& stash )
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
            if ( fileInfo.getFilePath() == filePath )
            {
                return fileInfo;
            }
        }
        THROW_RTE( "Failed to locate file: " << filePath << " in manifest" );
    }

    class Task_ParseAST : public BaseTask
    {
    public:
        Task_ParseAST( const mega::io::Environment&             environment,
                       task::Stash&                             stash,
                       const boost::filesystem::path&           parserDll,
                       const mega::io::FileInfo&                fileInfo,
                       const std::vector< mega::io::FileInfo >& fileInfos )
            : BaseTask( {}, environment, stash )
            , m_parserDLL( parserDll )
            , m_fileInfo( fileInfo )
            , m_fileInfos( fileInfos )
        {
        }

        void parseInputFile( mega::io::Database< mega::io::stage::ObjectParse >& database,
                const mega::Component* pComponent, 
                const boost::filesystem::path& sourceFilePath, 
                std::ostream& osError, 
                std::ostream& osWarn )
        {
            static boost::shared_ptr< mega::EG_PARSER_INTERFACE > pParserInterface;
            if ( !pParserInterface )
            {
                pParserInterface = boost::dll::import_symbol< mega::EG_PARSER_INTERFACE >(
                    m_parserDLL, "g_parserSymbol" );
            }

            boost::filesystem::path sourceFileFolder = sourceFilePath;
            sourceFileFolder.remove_filename();

            mega::input::Root* pRoot = pParserInterface->parseEGSourceFile( database,
                                                                            sourceFileFolder,
                                                                            sourceFilePath,
                                                                            pComponent->getIncludeDirectories(),
                                                                            osError,
                                                                            osWarn );

            m_rootFiles.insert( std::make_pair( sourceFilePath, pRoot ) );
        }

        virtual void run( task::Progress& taskProgress )
        {
            mega::io::Database< mega::io::stage::ObjectParse > database( m_environment, m_fileInfo.getFilePath() );

            taskProgress.start( "Task_ParseAST",
                                m_fileInfo.getFilePath(),
                                m_environment.objectAST( m_fileInfo.getFilePath() ) );

            std::ostringstream osError, osWarn;

            const mega::Component* pComponent = nullptr;
            {
                for ( const mega::Component* pIter : database.many_cst< mega::Component >() )
                {
                    for ( const boost::filesystem::path& sourceFile : pIter->getSourceFiles() )
                    {
                        if ( sourceFile == m_fileInfo.getFilePath() )
                        {
                            pComponent = pIter;
                            break;
                        }
                    }
                }
                VERIFY_RTE_MSG( pComponent, "Failed to locate component for source file: " << m_fileInfo.getFilePath() );
            }

            parseInputFile( database, pComponent, m_fileInfo.getFilePath(), osError, osWarn );

            // greedy algorithm to parse transitive closure of include files
            {
                bool bExhaustedAll = false;
                while ( !bExhaustedAll )
                {
                    bExhaustedAll = true;

                    for ( mega::input::MegaInclude* pInclude : database.many< mega::input::MegaInclude >() )
                    {
                        FileRootMap::const_iterator iFind = m_rootFiles.find( pInclude->getIncludeFilePath() );
                        if ( iFind == m_rootFiles.end() )
                        {
                            parseInputFile( database, pComponent, pInclude->getIncludeFilePath(), osError, osWarn );
                            bExhaustedAll = false;
                            break;
                        }
                    }
                }
            }

            if ( !osError.str().empty() )
            {
                // Error
                taskProgress.msg( osError.str() );
                taskProgress.failed();
            }
            else
            {
                if ( !osWarn.str().empty() )
                {
                    // Warning
                    taskProgress.msg( osWarn.str() );
                }
                database.store();
                taskProgress.succeeded();
            }
        }

        const mega::io::FileInfo&                m_fileInfo;
        const std::vector< mega::io::FileInfo >& m_fileInfos;
        const boost::filesystem::path            m_parserDLL;
        using FileRootMap = std::map< boost::filesystem::path, mega::input::Root* >;
        FileRootMap m_rootFiles;
    };

    class Task_DependencyAnalysis : public BaseTask
    {
    public:
        Task_DependencyAnalysis( const task::Task::RawPtrSet& parserTasks,
                                 const mega::io::Environment& environment,
                                 task::Stash&                 stash )
            : BaseTask( parserTasks, environment, stash )
        {
        }

        virtual void run( task::Progress& taskProgress )
        {
            mega::io::Database< mega::io::stage::DependencyAnalysis > database( m_environment );

            taskProgress.start( "Task_DependencyAnalysis",
                                m_environment.component(),
                                m_environment.dependencyAnalysis() );

            using namespace mega;

            std::vector< const mega::Component* > components = database.many_cst< mega::Component >();

            std::ostringstream os;
            os << "Found " << components.size() << " components";
            taskProgress.msg( os.str() );

            database.store();
            taskProgress.succeeded();
        }

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

            task::Task::PtrVector tasks;
            task::Task::RawPtrSet parserTasks;
            for ( const mega::io::FileInfo& fileInfo : manifest.getFileInfos() )
            {
                switch ( fileInfo.getFileType() )
                {
                case mega::io::FileInfo::ObjectSourceFile:
                {
                    Task_ParseAST* pTask = new Task_ParseAST( environment, stash, parserDll, fileInfo, manifest.getFileInfos() );
                    parserTasks.insert( pTask );
                    tasks.push_back( task::Task::Ptr( pTask ) );
                }
                break;
                default:
                    break;
                }
            }

            VERIFY_RTE_MSG( !tasks.empty(), "No input source code found" );

            Task_DependencyAnalysis* pDependencyAnalysisTask = new Task_DependencyAnalysis( parserTasks, environment, stash );
            tasks.push_back( task::Task::Ptr( pDependencyAnalysisTask ) );

            task::Schedule::Ptr pSchedule( new task::Schedule( tasks ) );
            task::run( pSchedule, std::cout );
        }
    }
} // namespace interface
} // namespace driver
