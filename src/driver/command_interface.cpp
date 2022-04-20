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
#include "database/common/sources.hpp"
#include "parser/parser.hpp"

#include "database/model/manifest.hxx"
#include "database/model/ParserStage.hxx"
#include "database/model/InterfaceStage.hxx"

#include "database/common/file.hpp"
#include "database/common/environments.hpp"

#include "common/scheduler.hpp"
#include "common/file.hpp"
#include "common/assert_verify.hpp"
#include "common/terminal.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <boost/dll.hpp>

#include <common/hash.hpp>
#include <common/string.hpp>
#include <string>
#include <vector>
#include <iostream>

std::ostream& operator<<( std::ostream& os, const std::vector< std::string >& name )
{
    common::delimit( name.begin(), name.end(), ".", os );
    return os;
}

namespace driver
{
    namespace interface
    {
        class BaseTask : public task::Task
        {
        public:
            BaseTask( const RawPtrSet& dependencies, const mega::io::Environment& environment )
                : task::Task( dependencies )
                , m_environment( environment )
            {
            }

        protected:
            const mega::io::Environment& m_environment;
        };

        const mega::io::FileInfo& findFileInfo( const boost::filesystem::path&           filePath,
                                                const std::vector< mega::io::FileInfo >& fileInfos )
        {
            for ( const mega::io::FileInfo& fileInfo : fileInfos )
            {
                if ( fileInfo.getFilePath().path() == filePath )
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
                           const boost::filesystem::path&           parserDll,
                           const mega::io::megaFilePath&            sourceFilePath,
                           const std::vector< mega::io::FileInfo >& fileInfos )
                : BaseTask( {}, environment )
                , m_parserDLL( parserDll )
                , m_sourceFilePath( sourceFilePath )
                , m_fileInfos( fileInfos )
            {
            }

            ParserStage::Parser::ContextDef* parseInputFile( ParserStage::Database&                    database,
                                                             const ParserStage::Components::Component* pComponent,
                                                             const mega::io::megaFilePath&             sourceFilePath,
                                                             std::ostream&                             osError,
                                                             std::ostream&                             osWarn )
            {
                static boost::shared_ptr< EG_PARSER_INTERFACE > pParserInterface;
                if ( !pParserInterface )
                {
                    pParserInterface = boost::dll::import_symbol< EG_PARSER_INTERFACE >( m_parserDLL, "g_parserSymbol" );
                }

                ParserStage::Parser::ContextDef* pContextDef
                    = pParserInterface->parseEGSourceFile( database, m_environment.rootSourceDir() / sourceFilePath.path(),
                                                           pComponent->get_includeDirectories(), osError, osWarn );
                return pContextDef;
            }

            virtual void run( task::Progress& taskProgress )
            {
                // ParserStage::Parser::Root
                const mega::io::CompilationFilePath astFile  = m_environment.ParserStage_AST( m_sourceFilePath );
                const mega::io::CompilationFilePath bodyFile = m_environment.ParserStage_Body( m_sourceFilePath );

                taskProgress.start( "Task_ParseAST", m_sourceFilePath.path(), astFile.path() );

                using namespace ParserStage;
                Database database( m_environment, m_sourceFilePath );

                std::ostringstream osError, osWarn;

                Components::Component* pComponent = nullptr;
                {
                    for ( Components::Component* pIter : database.many< Components::Component >( m_environment.project_manifest() ) )
                    {
                        for ( const boost::filesystem::path& sourceFile : pIter->get_sourceFiles() )
                        {
                            if ( m_environment.megaFilePath_fromPath( sourceFile ) == m_sourceFilePath )
                            {
                                pComponent = pIter;
                                break;
                            }
                        }
                    }
                    VERIFY_RTE_MSG( pComponent, "Failed to locate component for source file: " << m_sourceFilePath.path().string() );
                }

                using namespace ParserStage::Parser;

                ContextDef* pContextDef = parseInputFile( database, pComponent, m_sourceFilePath, osError, osWarn );

                ObjectSourceRoot* pObjectSrcRoot = database.construct< ObjectSourceRoot >(
                    ObjectSourceRoot::Args{ SourceRoot::Args{ m_sourceFilePath.path(), pComponent, pContextDef } } );

                m_rootFiles.insert( std::make_pair( m_sourceFilePath.path(), pObjectSrcRoot ) );

                // greedy algorithm to parse transitive closure of include files
                {
                    bool bExhaustedAll = false;
                    while ( !bExhaustedAll )
                    {
                        bExhaustedAll = true;

                        for ( MegaInclude* pInclude : database.many< MegaInclude >( m_sourceFilePath ) )
                        {
                            FileRootMap::const_iterator iFind = m_rootFiles.find( pInclude->get_megaSourceFilePath() );
                            if ( iFind == m_rootFiles.end() )
                            {
                                ContextDef* pIncludeContextDef = parseInputFile(
                                    database, pComponent, m_environment.megaFilePath_fromPath( pInclude->get_megaSourceFilePath() ),
                                    osError, osWarn );

                                IncludeRoot* pIncludeRoot = database.construct< IncludeRoot >( { IncludeRoot::Args{
                                    SourceRoot::Args{ pInclude->get_megaSourceFilePath(), pComponent, pIncludeContextDef }, pInclude } } );

                                pInclude->set_root( pIncludeRoot );

                                m_rootFiles.insert( std::make_pair( pInclude->get_megaSourceFilePath(), pIncludeRoot ) );
                                bExhaustedAll = false;
                                break;
                            }
                            else
                            {
                                IncludeRoot* pIncludeRoot = dynamic_database_cast< IncludeRoot >( iFind->second );
                                VERIFY_RTE( pIncludeRoot );
                                pInclude->set_root( pIncludeRoot );
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

                    bool bRestored = true;
                    {
                        const common::HashCode astHashCode = database.save_AST_to_temp();
                        m_environment.setBuildHashCode( astFile, astHashCode );
                        if ( !m_environment.restore( astFile, astHashCode ) )
                        {
                            m_environment.temp_to_real( astFile );
                            m_environment.stash( astFile, astHashCode );
                            bRestored = false;
                        }
                    }
                    {
                        const common::HashCode bodyHashCode = database.save_Body_to_temp();
                        m_environment.setBuildHashCode( bodyFile, bodyHashCode );
                        if ( !m_environment.restore( bodyFile, bodyHashCode ) )
                        {
                            m_environment.temp_to_real( bodyFile );
                            m_environment.stash( bodyFile, bodyHashCode );
                            bRestored = false;
                        }
                    }

                    if ( bRestored )
                    {
                        taskProgress.cached();
                    }
                    else
                    {
                        taskProgress.succeeded();
                    }
                }
            }

            const mega::io::megaFilePath&            m_sourceFilePath;
            const std::vector< mega::io::FileInfo >& m_fileInfos;
            const boost::filesystem::path            m_parserDLL;
            using FileRootMap = std::map< boost::filesystem::path, ParserStage::Parser::SourceRoot* >;
            FileRootMap m_rootFiles;
        };

        class Task_ObjectInterfaceGen : public BaseTask
        {
        public:
            Task_ObjectInterfaceGen( task::Task::RawPtr            pDependency,
                                     const mega::io::Environment&  environment,
                                     const mega::io::megaFilePath& sourceFilePath )
                : BaseTask( { pDependency }, environment )
                , m_sourceFilePath( sourceFilePath )
            {
            }

            using Name       = std::vector< std::string >;
            using NameResMap = std::map< Name, InterfaceStage::Interface::ContextGroup* >;

            Name fromParserID( const Name& currentName, const std::vector< InterfaceStage::Parser::Identifier* >& ids ) const
            {
                VERIFY_RTE( !ids.empty() );
                using namespace InterfaceStage;
                Name name = currentName;
                for ( const Parser::Identifier* pID : ids )
                {
                    name.push_back( pID->get_str() );
                }
                return name;
            }

            InterfaceStage::Interface::ContextGroup* findContextGroup( const Name& name, const NameResMap& namedContexts )
            {
                NameResMap::const_iterator iFind = namedContexts.find( name );
                if ( iFind != namedContexts.end() )
                {
                    return iFind->second;
                }
                return nullptr;
            }

            template < typename TParserContextDefType, typename TInterfaceContextType, typename ConstructorLambda,
                       typename AggregateLambda >
            void constructOrAggregate( InterfaceStage::Database&        database,
                                       InterfaceStage::Interface::Root* pRoot,
                                       TParserContextDefType*           pContextDef,
                                       const Name&                      currentName,
                                       NameResMap&                      namedContexts,
                                       const ConstructorLambda&         constructorFunctor,
                                       const AggregateLambda&           aggregateFunctor )
            {
                using namespace InterfaceStage;
                using namespace InterfaceStage::Interface;

                const Name             name                   = fromParserID( currentName, pContextDef->get_id()->get_ids() );
                TInterfaceContextType* pInterfaceContextGroup = nullptr;
                {
                    if ( Interface::ContextGroup* pContextGroup = findContextGroup( name, namedContexts ) )
                    {
                        pInterfaceContextGroup = dynamic_database_cast< TInterfaceContextType >( pContextGroup );
                        VERIFY_RTE_MSG( pInterfaceContextGroup, "Context definition of identifier: " << name << " with mixed types" );
                        aggregateFunctor( pInterfaceContextGroup, pContextDef );
                    }
                    else
                    {
                        Interface::ContextGroup* pParent = pRoot;
                        if ( name.size() > 1U )
                        {
                            Name parentName = name;
                            parentName.pop_back();
                            pParent = findContextGroup( parentName, namedContexts );
                            VERIFY_RTE_MSG( pParent, "Failed to locate parent for: " << name );
                        }
                        pInterfaceContextGroup = constructorFunctor( database, name.back(), pParent, pContextDef );
                        namedContexts.insert( std::make_pair( name, pInterfaceContextGroup ) );
                    }
                }
                recurse( database, pRoot, pInterfaceContextGroup, pContextDef, name, namedContexts );
            }

            void recurse( InterfaceStage::Database&                database,
                          InterfaceStage::Interface::Root*         pRoot,
                          InterfaceStage::Interface::ContextGroup* pGroup,
                          InterfaceStage::Parser::ContextDef*      pContext,
                          const Name&                              currentName,
                          NameResMap&                              namedContexts )
            {
                using namespace InterfaceStage;
                using namespace InterfaceStage::Interface;

                VERIFY_RTE( pContext );

                for ( Parser::ContextDef* pChildContext : pContext->get_children() )
                {
                    if ( Parser::AbstractDef* pAbstractDef = dynamic_database_cast< Parser::AbstractDef >( pChildContext ) )
                    {
                        constructOrAggregate< Parser::AbstractDef, Abstract >(
                            database, pRoot, pAbstractDef, currentName, namedContexts,
                            []( Database& database, const std::string& name, ContextGroup* pParent,
                                Parser::AbstractDef* pAbstractDef ) -> Abstract*
                            {
                                return database.construct< Abstract >(
                                    Abstract::Args( Context::Args( ContextGroup::Args( std::vector< Context* >{} ), name, pParent ),
                                                    { pAbstractDef }, {} ) );
                            },
                            []( Abstract* pAbstract, Parser::AbstractDef* pAbstractDef )
                            { pAbstract->push_back_abstract_defs( pAbstractDef ); } );
                    }
                    else if ( Parser::ActionDef* pActionDef = dynamic_database_cast< Parser::ActionDef >( pChildContext ) )
                    {
                        constructOrAggregate< Parser::ActionDef, Action >(
                            database, pRoot, pActionDef, currentName, namedContexts,
                            []( Database& database, const std::string& name, ContextGroup* pParent,
                                Parser::ActionDef* pActionDef ) -> Action*
                            {
                                return database.construct< Action >( Action::Args(
                                    Context::Args( ContextGroup::Args( std::vector< Context* >{} ), name, pParent ), { pActionDef }, {} ) );
                            },
                            []( Action* pAction, Parser::ActionDef* pActionDef ) { pAction->push_back_action_defs( pActionDef ); } );
                    }
                    else if ( Parser::EventDef* pEventDef = dynamic_database_cast< Parser::EventDef >( pChildContext ) )
                    {
                        constructOrAggregate< Parser::EventDef, Event >(
                            database, pRoot, pEventDef, currentName, namedContexts,
                            []( Database& database, const std::string& name, ContextGroup* pParent, Parser::EventDef* pEventDef ) -> Event*
                            {
                                return database.construct< Event >( Event::Args(
                                    Context::Args( ContextGroup::Args( std::vector< Context* >{} ), name, pParent ), { pEventDef }, {} ) );
                            },
                            []( Event* pEvent, Parser::EventDef* pEventDef ) { pEvent->push_back_event_defs( pEventDef ); } );
                    }
                    else if ( Parser::FunctionDef* pFunctionDef = dynamic_database_cast< Parser::FunctionDef >( pChildContext ) )
                    {
                        constructOrAggregate< Parser::FunctionDef, Function >(
                            database, pRoot, pFunctionDef, currentName, namedContexts,
                            []( Database& database, const std::string& name, ContextGroup* pParent,
                                Parser::FunctionDef* pFunctionDef ) -> Function*
                            {
                                return database.construct< Function >( Function::Args(
                                    Context::Args( ContextGroup::Args( std::vector< Context* >{} ), name, pParent ), { pFunctionDef } ) );
                            },
                            []( Function* pFunction, Parser::FunctionDef* pFunctionDef )
                            { pFunction->push_back_function_defs( pFunctionDef ); } );
                    }
                    else if ( Parser::ObjectDef* pObjectDef = dynamic_database_cast< Parser::ObjectDef >( pChildContext ) )
                    {
                        constructOrAggregate< Parser::ObjectDef, Object >(
                            database, pRoot, pObjectDef, currentName, namedContexts,
                            []( Database& database, const std::string& name, ContextGroup* pParent,
                                Parser::ObjectDef* pObjectDef ) -> Object*
                            {
                                return database.construct< Object >( Object::Args(
                                    Context::Args( ContextGroup::Args( std::vector< Context* >{} ), name, pParent ), { pObjectDef }, {} ) );
                            },
                            []( Object* pObject, Parser::ObjectDef* pObjectDef ) { pObject->push_back_object_defs( pObjectDef ); } );
                    }
                    else if ( Parser::LinkDef* pLinkDef = dynamic_database_cast< Parser::LinkDef >( pChildContext ) )
                    {
                        constructOrAggregate< Parser::LinkDef, Link >(
                            database, pRoot, pLinkDef, currentName, namedContexts,
                            []( Database& database, const std::string& name, ContextGroup* pParent, Parser::LinkDef* pLinkDef ) -> Link*
                            {
                                return database.construct< Link >( Link::Args(
                                    Context::Args( ContextGroup::Args( std::vector< Context* >{} ), name, pParent ), { pLinkDef } ) );
                            },
                            []( Link* pLink, Parser::LinkDef* pLinkDef ) { pLink->push_back_link_defs( pLinkDef ); } );
                    }
                    else
                    {
                        THROW_RTE( "Unknown context type" );
                    }
                }
            }

            template < typename TContextGroup, typename TParserDef >
            void collectDimensions( InterfaceStage::Database& database, TContextGroup* pGroup, TParserDef* pDef )
            {
                using namespace InterfaceStage;

                for ( Parser::Dimension* pParserDim : pDef->get_dimensions() )
                {
                    // Interface::Dimension* pDimension = database.construct< Interface::Dimension >(
                    //     Interface::Dimension::Args( pParserDim, pParserDim->get_id()->get_str() ) );
                    // pGroup->push_back_dimensions( pDimension );
                }
            }

            void onAbstract( InterfaceStage::Database& database, InterfaceStage::Interface::Abstract* pAbstract )
            {
                using namespace InterfaceStage;
                for ( Parser::AbstractDef* pDef : pAbstract->get_abstract_defs() )
                {
                    collectDimensions< InterfaceStage::Interface::Abstract, Parser::AbstractDef >( database, pAbstract, pDef );
                }
            }
            void onAction( InterfaceStage::Database& database, InterfaceStage::Interface::Action* pAction )
            {
                using namespace InterfaceStage;
                for ( Parser::ActionDef* pDef : pAction->get_action_defs() )
                {
                    collectDimensions< InterfaceStage::Interface::Action, Parser::ActionDef >( database, pAction, pDef );

                    const std::string& strBody = pDef->get_body();
                    if ( !strBody.empty() )
                    {
                    }
                }
            }
            void onEvent( InterfaceStage::Database& database, InterfaceStage::Interface::Event* pEvent )
            {
                using namespace InterfaceStage;
                for ( Parser::EventDef* pDef : pEvent->get_event_defs() )
                {
                    collectDimensions< InterfaceStage::Interface::Event, Parser::EventDef >( database, pEvent, pDef );
                }
            }
            void onFunction( InterfaceStage::Database& database, InterfaceStage::Interface::Function* pFunction )
            {
                using namespace InterfaceStage;
            }
            void onObject( InterfaceStage::Database& database, InterfaceStage::Interface::Object* pObject )
            {
                using namespace InterfaceStage;
                for ( Parser::ObjectDef* pDef : pObject->get_object_defs() )
                {
                    collectDimensions< InterfaceStage::Interface::Object, Parser::ObjectDef >( database, pObject, pDef );
                }
            }
            void onLink( InterfaceStage::Database& database, InterfaceStage::Interface::Link* pLink ) { using namespace InterfaceStage; }

            virtual void run( task::Progress& taskProgress )
            {
                const mega::io::CompilationFilePath treePath = m_environment.InterfaceStage_Tree( m_sourceFilePath );
                taskProgress.start( "Task_ObjectInterfaceGen", m_sourceFilePath.path(), treePath.path() );

                const common::HashCode hashCode = m_environment.getBuildHashCode( m_environment.ParserStage_AST( m_sourceFilePath ) );

                if ( m_environment.restore( treePath, hashCode ) )
                {
                    m_environment.setBuildHashCode( treePath, hashCode );
                    taskProgress.cached();
                    return;
                }

                using namespace InterfaceStage;
                Database database( m_environment, m_sourceFilePath );

                Parser::ObjectSourceRoot* pRoot = database.one< Parser::ObjectSourceRoot >( m_sourceFilePath );
                VERIFY_RTE( pRoot );

                using namespace InterfaceStage::Interface;
                Root* pInterfaceRoot = database.construct< Root >( Root::Args{ ContextGroup::Args{ {} }, pRoot } );

                Name       currentName;
                NameResMap namedContexts;

                recurse( database, pInterfaceRoot, pInterfaceRoot, pRoot->get_ast(), currentName, namedContexts );

                for ( Interface::Abstract* pAbstract : database.many< Interface::Abstract >( m_sourceFilePath ) )
                {
                    onAbstract( database, pAbstract );
                }
                for ( Interface::Action* pAction : database.many< Interface::Action >( m_sourceFilePath ) )
                {
                    onAction( database, pAction );
                }
                for ( Interface::Event* pEvent : database.many< Interface::Event >( m_sourceFilePath ) )
                {
                    onEvent( database, pEvent );
                }
                for ( Interface::Function* pFunction : database.many< Interface::Function >( m_sourceFilePath ) )
                {
                    onFunction( database, pFunction );
                }
                for ( Interface::Object* pObject : database.many< Interface::Object >( m_sourceFilePath ) )
                {
                    onObject( database, pObject );
                }
                for ( Interface::Link* pLink : database.many< Interface::Link >( m_sourceFilePath ) )
                {
                    onLink( database, pLink );
                }

                const common::HashCode fileHashCode = database.save_Tree_to_temp();
                m_environment.setBuildHashCode( treePath, fileHashCode );
                m_environment.temp_to_real( treePath );
                m_environment.stash( treePath, hashCode );

                taskProgress.succeeded();
            }
            const mega::io::megaFilePath& m_sourceFilePath;
        };

        class Task_DependencyAnalysis : public BaseTask
        {
        public:
            Task_DependencyAnalysis( const task::Task::RawPtrSet& parserTasks, const mega::io::Environment& environment )
                : BaseTask( parserTasks, environment )
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

                /*
                            taskProgress.start( "Task_DependencyAnalysis",
                                                m_environment.ComponentListing_Components(),
                                                m_environment.Dependencies_DependencyAnalysis() );

                            using namespace mega;
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
            Task_ObjectInterfaceAnalysis( task::Task::RawPtrSet         dependencies,
                                          const mega::io::Environment&  environment,
                                          const mega::io::megaFilePath& sourceFilePath )
                : BaseTask( dependencies, environment )
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
                // mega::io::Database< mega::io::stage::Stage_ObjectAnalysis > database(
                //     m_environment, m_sourceFilePath );

                // taskProgress.start( "Task_ObjectInterfaceAnalysis",
                //                     m_sourceFilePath,
                //                     m_environment.Analysis_ObjectAnalysis( m_sourceFilePath ) );

                // database.store();

                taskProgress.succeeded();
            }
            const mega::io::megaFilePath& m_sourceFilePath;
        };

        void command( bool bHelp, const std::vector< std::string >& args )
        {
            boost::filesystem::path rootSourceDir, rootBuildDir, parserDll, tempDir = boost::filesystem::temp_directory_path();
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
                mega::io::BuildEnvironment environment( rootSourceDir, rootBuildDir, tempDir );

                VERIFY_RTE_MSG( boost::filesystem::exists( parserDll ), "Failed to locate parser DLL: " << parserDll );

                // load the manifest
                const mega::io::manifestFilePath projectManifestPath = environment.project_manifest();
                mega::io::Manifest               manifest( environment, projectManifestPath );

                task::Task::PtrVector tasks;
                task::Task::RawPtrSet parserTasks;
                task::Task::RawPtrSet interfaceGenTasks;
                for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
                {
                    Task_ParseAST* pTask = new Task_ParseAST( environment, parserDll, sourceFilePath, manifest.getCompilationFileInfos() );
                    parserTasks.insert( pTask );
                    tasks.push_back( task::Task::Ptr( pTask ) );

                    Task_ObjectInterfaceGen* pInterfaceGenTask = new Task_ObjectInterfaceGen( pTask, environment, sourceFilePath );
                    interfaceGenTasks.insert( pInterfaceGenTask );
                    tasks.push_back( task::Task::Ptr( pInterfaceGenTask ) );
                }

                VERIFY_RTE_MSG( !tasks.empty(), "No input source code found" );

                /*Task_DependencyAnalysis* pDependencyAnalysisTask = new Task_DependencyAnalysis( interfaceGenTasks, environment );
                tasks.push_back( task::Task::Ptr( pDependencyAnalysisTask ) );

                for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
                {
                    Task_ObjectInterfaceAnalysis* pObjectInterfaceAnalysis
                        = new Task_ObjectInterfaceAnalysis( { pDependencyAnalysisTask }, environment, sourceFilePath );
                    tasks.push_back( task::Task::Ptr( pObjectInterfaceAnalysis ) );
                }*/

                task::Schedule::Ptr pSchedule( new task::Schedule( tasks ) );
                task::run( pSchedule, std::cout );
            }
        }
    } // namespace interface
} // namespace driver
