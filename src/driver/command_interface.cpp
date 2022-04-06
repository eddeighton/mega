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

//#include "database/model/interface.hpp"
#include "parser/parser.hpp"

//#include "database/model/input.hpp"
//#include "database/model/component.hpp"
//#include "database/model/dependencies.hpp"

#include "database/io/file.hpp"
#include "database/io/manifest.hpp"
#include "database/io/environment.hpp"
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
        BaseTask( const RawPtrSet&             dependencies,
                  const mega::io::Environment& environment,
                  task::Stash&                 stash )
            : task::Task( dependencies )
            , m_environment( environment )
            , m_stash( stash )
        {
        }

    protected:
        const mega::io::Environment& m_environment;
        task::Stash&                 m_stash;
    };

    const mega::io::FileInfo& findFileInfo( const boost::filesystem::path&           filePath,
                                            const std::vector< mega::io::FileInfo >& fileInfos )
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
                       const boost::filesystem::path&           sourceFilePath,
                       const std::vector< mega::io::FileInfo >& fileInfos )
            : BaseTask( {}, environment, stash )
            , m_parserDLL( parserDll )
            , m_sourceFilePath( sourceFilePath )
            , m_fileInfos( fileInfos )
        {
        }
/*
        void parseInputFile( mega::io::Database< mega::io::stage::Stage_ObjectParse >& database,
                             const mega::Component*                                    pComponent,
                             mega::input::Root*                                        pRoot,
                             std::ostream&                                             osError,
                             std::ostream&                                             osWarn )
        {
            static boost::shared_ptr< mega::EG_PARSER_INTERFACE > pParserInterface;
            if ( !pParserInterface )
            {
                pParserInterface = boost::dll::import_symbol< mega::EG_PARSER_INTERFACE >(
                    m_parserDLL, "g_parserSymbol" );
            }

            pParserInterface->parseEGSourceFile(
                database, pRoot, pComponent->getIncludeDirectories(), osError, osWarn );
        }
*/
        virtual void run( task::Progress& taskProgress )
        {
            mega::io::Database< mega::io::stage::STAGE_InputParse > database(
                m_environment, m_sourceFilePath );

            taskProgress.start(
                "Task_ParseAST", m_sourceFilePath, m_environment.FILE_InputParse_AST( m_sourceFilePath ) );

            std::ostringstream osError, osWarn;
/*
            const mega::Component* pComponent = nullptr;
            {
                for ( const mega::Component* pIter : database.many_cst< mega::Component >() )
                {
                    for ( const boost::filesystem::path& sourceFile : pIter->getSourceFiles() )
                    {
                        if ( sourceFile == m_sourceFilePath )
                        {
                            pComponent = pIter;
                            break;
                        }
                    }
                }
                VERIFY_RTE_MSG(
                    pComponent,
                    "Failed to locate component for source file: " << m_sourceFilePath.string() );
            }

            mega::input::Root* pObjectSrcRoot = database.construct< mega::input::Root >(
                pComponent, m_sourceFilePath, mega::eObjectSrcRoot );
            parseInputFile( database, pComponent, pObjectSrcRoot, osError, osWarn );
            m_rootFiles.insert( std::make_pair( m_sourceFilePath, pObjectSrcRoot ) );

            // greedy algorithm to parse transitive closure of include files
            {
                bool bExhaustedAll = false;
                while ( !bExhaustedAll )
                {
                    bExhaustedAll = true;

                    for ( mega::input::MegaInclude* pInclude :
                          database.many< mega::input::MegaInclude >() )
                    {
                        FileRootMap::const_iterator iFind
                            = m_rootFiles.find( pInclude->getIncludeFilePath() );
                        if ( iFind == m_rootFiles.end() )
                        {
                            pInclude->m_pIncludeRoot = database.construct< mega::input::Root >(
                                pComponent, pInclude->getIncludeFilePath(), mega::eIncludeSrcRoot );
                            parseInputFile(
                                database, pComponent, pInclude->m_pIncludeRoot, osError, osWarn );
                            m_rootFiles.insert( std::make_pair(
                                pInclude->getIncludeFilePath(), pInclude->m_pIncludeRoot ) );
                            bExhaustedAll = false;
                            break;
                        }
                        else
                        {
                            pInclude->m_pIncludeRoot = iFind->second;
                        }
                    }
                }
            }
*/
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

        const boost::filesystem::path&           m_sourceFilePath;
        const std::vector< mega::io::FileInfo >& m_fileInfos;
        const boost::filesystem::path            m_parserDLL;
        //using FileRootMap = std::map< boost::filesystem::path, mega::input::Root* >;
        //FileRootMap m_rootFiles;
    };

    class Task_ObjectInterfaceGen : public BaseTask
    {
    public:
        Task_ObjectInterfaceGen( task::Task::RawPtr             pDependency,
                                 const mega::io::Environment&   environment,
                                 task::Stash&                   stash,
                                 const boost::filesystem::path& sourceFilePath )
            : BaseTask( { pDependency }, environment, stash )
            , m_sourceFilePath( sourceFilePath )
        {
        }

        //using DatabaseType = mega::io::Database< mega::io::stage::STAGE_Interface >;
/*
        void buildInterface( DatabaseType&               database,
                             const mega::input::Element* pInput,
                             mega::interface::Element*   pInterface )
        {
            using namespace mega;
            switch ( pInput->getType() )
            {
                case eInputOpaque:
                case eInputDimension:
                case eInputUsing:
                case eInputExport:
                case eInputVisibility:
                case eInputMegaInclude:
                case eInputCPPInclude:
                case eInputSystemInclude:
                case eInputDependency:
                    break;
                case eInputContext:
                case eInputRoot:
                {
                    const input::Context* pContext
                        = dynamic_cast< const input::Context* >( pInput );
                    VERIFY_RTE( pContext );

                    for ( const input::Element* pChildElement : pContext->getElements() )
                    {
                        buildInterface( database, pChildElement, pInterface );
                    }
                }
                break;
            }
        }
*/
        virtual void run( task::Progress& taskProgress )
        {
            //mega::io::Database< mega::io::stage::STAGE_Interface > database(
            //    m_environment, m_sourceFilePath );

            taskProgress.start( "Task_ObjectInterfaceGen",
                                m_sourceFilePath,
                                m_environment.FILE_Interface_Tree( m_sourceFilePath ) );
/*
            const mega::input::Root* pRoot = nullptr;
            for ( const mega::input::Root* pIter : database.many_cst< mega::input::Root >() )
            {
                if ( pIter->getFilePath() == m_sourceFilePath )
                {
                    pRoot = pIter;
                    break;
                }
            }
            VERIFY_RTE( pRoot );
*/
            //mega::interface::Root* pInterfaceRoot
            //    = database.construct< mega::interface::Root >( pRoot, mega::eVisPublic );

            //buildInterface( database, pRoot, pInterfaceRoot );

            //database.store();

            taskProgress.succeeded();
        }
        const boost::filesystem::path& m_sourceFilePath;
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
/*
        using DependencyMap = std::map< const mega::input::Dependency*, const mega::input::Root*,
                                        mega::io::CompareIndexedObjects >;

        const mega::input::Root*
        resolveDependency( const std::vector< const mega::input::Root* >& roots,
                           const boost::filesystem::path&                 filepath )
        {
            struct FindRoot
            {
                const mega::input::Root*
                operator()( const std::vector< const mega::input::Root* >& roots,
                            const boost::filesystem::path&                 filepath ) const
                {
                    auto iFind = std::find_if( roots.begin(), roots.end(),
                                               [ &filepath ]( const mega::input::Root* pRoot )
                                               { return pRoot->getFilePath() == filepath; } );
                    if ( iFind != roots.end() )
                        return *iFind;
                    else
                        return nullptr;
                }
            };

            if ( boost::filesystem::exists( filepath ) )
            {
                const mega::input::Root* pRoot = FindRoot()( roots, filepath );
                VERIFY_RTE_MSG( pRoot, "Failed to locate dependency file: " << filepath.string() );
                return pRoot;
            }
            return nullptr;
        }

        void resolveDependencies( const std::vector< const mega::input::Root* >& roots,
                                  const mega::input::Root*                       pRoot,
                                  const mega::input::Context*                    pContext,
                                  DependencyMap&                                 dependencyMap )
        {
            using namespace mega::input;

            for ( const Element* pChild : pContext->getElements() )
            {
                if ( const Dependency* pDependency = dynamic_cast< const Dependency* >( pChild ) )
                {
                    const boost::filesystem::path relativePath
                        = m_environment.dependency( pDependency->getOpaque()->getStr() );

                    // try current directory of root
                    boost::filesystem::path curDir = pRoot->getFilePath();
                    curDir.remove_filename();

                    if ( const mega::input::Root* pDependencyRoot
                         = resolveDependency( roots, curDir / relativePath ) )
                    {
                        dependencyMap.insert( std::make_pair( pDependency, pDependencyRoot ) );
                    }
                    else if ( const mega::input::Root* pDependencyRoot = resolveDependency(
                                  roots, m_environment.rootSourceDir() / relativePath ) )
                    {
                        dependencyMap.insert( std::make_pair( pDependency, pDependencyRoot ) );
                    }
                    else
                    {
                        // try the include directories
                        bool bFound = false;
                        for ( const boost::filesystem::path& includeDir :
                              pRoot->m_pComponent->getIncludeDirectories() )
                        {
                            if ( const mega::input::Root* pDependencyRoot
                                 = resolveDependency( roots, includeDir / relativePath ) )
                            {
                                dependencyMap.insert(
                                    std::make_pair( pDependency, pDependencyRoot ) );
                                bFound = true;
                                break;
                            }
                        }
                        VERIFY_RTE_MSG(
                            bFound, "Failed to locate dependency: "
                                        << pDependency->getOpaque()->getStr()
                                        << " in file: " << pRoot->getFilePath().string() );
                    }
                }
                else if ( const Context* pChildContext = dynamic_cast< const Context* >( pChild ) )
                {
                    resolveDependencies( roots, pRoot, pChildContext, dependencyMap );
                }
            }
        }

        using DependenciesMap
            = std::multimap< const mega::input::Root*, const mega::input::Dependency*,
                             mega::io::CompareIndexedObjects >;

        void collectDependencies( const std::vector< const mega::input::Root* >& roots,
                                  const mega::input::Root*                       pRoot,
                                  const mega::input::Context*                    pContext,
                                  DependenciesMap&                               dependencies )
        {
            using namespace mega::input;

            for ( const Element* pChild : pContext->getElements() )
            {
                if ( const Dependency* pDependency = dynamic_cast< const Dependency* >( pChild ) )
                {
                    dependencies.insert( std::make_pair( pRoot, pDependency ) );
                }
                else if ( const MegaInclude* pInclude
                          = dynamic_cast< const MegaInclude* >( pChild ) )
                {
                    VERIFY_RTE( pInclude->m_pIncludeRoot );
                    collectDependencies( roots, pRoot, pInclude->m_pIncludeRoot, dependencies );
                }
                else if ( const Context* pChildContext = dynamic_cast< const Context* >( pChild ) )
                {
                    collectDependencies( roots, pRoot, pChildContext, dependencies );
                }
            }
        }
*/
        virtual void run( task::Progress& taskProgress )
        {
           // mega::io::Database< mega::io::stage::Stage_DependencyAnalysis > database(
           //     m_environment );

            taskProgress.start( "Task_DependencyAnalysis",
                                m_environment.FILE_ComponentListing_Components(),
                                m_environment.FILE_Dependencies_DependencyAnalysis() );

            using namespace mega;
/*
            mega::DependencyAnalysis* pDA = database.construct< mega::DependencyAnalysis >();

            const std::vector< const mega::input::Root* > roots
                = database.many_cst< mega::input::Root >();

            DependencyMap dependencyMap;
            for ( const mega::input::Root* pRoot : roots )
            {
                resolveDependencies( roots, pRoot, pRoot, dependencyMap );
            }

            DependenciesMap dependenciesMap;
            for ( const mega::input::Root* pRoot : roots )
            {
                if ( pRoot->getRootType() == eObjectSrcRoot )
                {
                    collectDependencies( roots, pRoot, pRoot, dependenciesMap );
                }
            }

            for ( const mega::input::Root* pRoot : roots )
            {
                if ( pRoot->getRootType() == eObjectSrcRoot )
                {
                    for ( DependenciesMap::const_iterator i = dependenciesMap.lower_bound( pRoot ),
                                                          iUpper
                                                          = dependenciesMap.upper_bound( pRoot );
                          i != iUpper;
                          ++i )
                    {
                        auto iFind = dependencyMap.find( i->second );
                        VERIFY_RTE( iFind != dependencyMap.end() );
                        pDA->m_dependencies.insert( std::make_pair( pRoot, iFind->second ) );
                        // std::ostringstream os;
                        // os << pRoot->getFilePath() << "->" << iFind->second->getFilePath();
                        // taskProgress.msg( os.str() );
                    }
                }
            }

            database.store();*/
            taskProgress.succeeded();
        }
    };

    class Task_ObjectInterfaceAnalysis : public BaseTask
    {
    public:
        Task_ObjectInterfaceAnalysis( task::Task::RawPtrSet          dependencies,
                                      const mega::io::Environment&   environment,
                                      task::Stash&                   stash,
                                      const boost::filesystem::path& sourceFilePath )
            : BaseTask( dependencies, environment, stash )
            , m_sourceFilePath( sourceFilePath )
        {
        }

        virtual bool isReady( const RawPtrSet& finished )
        {
            if ( BaseTask::isReady( finished ) )
            {
                // determine if dependency tasks are completed...

                return true;
            }

            return false;
        }

        virtual void run( task::Progress& taskProgress )
        {
            //mega::io::Database< mega::io::stage::Stage_ObjectAnalysis > database(
            //    m_environment, m_sourceFilePath );

            taskProgress.start( "Task_ObjectInterfaceAnalysis",
                                m_sourceFilePath,
                                m_environment.FILE_Analysis_ObjectAnalysis( m_sourceFilePath ) );

            //database.store();

            taskProgress.succeeded();
        }
        const boost::filesystem::path& m_sourceFilePath;
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
        po::store(
            po::command_line_parser( args ).options( commandOptions ).positional( p ).run(), vm );
        po::notify( vm );

        if ( bHelp )
        {
            std::cout << commandOptions << "\n";
        }
        else
        {
            mega::io::Environment environment( rootSourceDir, rootBuildDir );
            task::Stash           stash( environment.stashDir() );

            VERIFY_RTE_MSG( boost::filesystem::exists( parserDll ),
                            "Failed to locate parser DLL: " << parserDll );

            // load the manifest
            const mega::io::Environment::Path projectManifestPath = environment.project_manifest();
            mega::io::Manifest                manifest( projectManifestPath );

            task::Task::PtrVector tasks;
            task::Task::RawPtrSet parserTasks;
            task::Task::RawPtrSet interfaceGenTasks;
            for ( const boost::filesystem::path& sourceFilePath : manifest.getSourceFiles() )
            {
                Task_ParseAST* pTask
                    = new Task_ParseAST( environment, stash, parserDll, sourceFilePath,
                                         manifest.getCompilationFileInfos() );
                parserTasks.insert( pTask );
                tasks.push_back( task::Task::Ptr( pTask ) );

                Task_ObjectInterfaceGen* pInterfaceGenTask
                    = new Task_ObjectInterfaceGen( pTask, environment, stash, sourceFilePath );
                interfaceGenTasks.insert( pInterfaceGenTask );
                tasks.push_back( task::Task::Ptr( pInterfaceGenTask ) );
            }

            VERIFY_RTE_MSG( !tasks.empty(), "No input source code found" );

            Task_DependencyAnalysis* pDependencyAnalysisTask
                = new Task_DependencyAnalysis( interfaceGenTasks, environment, stash );
            tasks.push_back( task::Task::Ptr( pDependencyAnalysisTask ) );

            for ( const boost::filesystem::path& sourceFilePath : manifest.getSourceFiles() )
            {
                Task_ObjectInterfaceAnalysis* pObjectInterfaceAnalysis
                    = new Task_ObjectInterfaceAnalysis(
                        { pDependencyAnalysisTask }, environment, stash, sourceFilePath );
                tasks.push_back( task::Task::Ptr( pObjectInterfaceAnalysis ) );
            }

            task::Schedule::Ptr pSchedule( new task::Schedule( tasks ) );
            task::run( pSchedule, std::cout );
        }
    }
} // namespace interface
} // namespace driver
