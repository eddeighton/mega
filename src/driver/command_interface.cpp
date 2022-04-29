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
#include "database/model/DependencyAnalysis.hxx"
#include "database/model/DependencyAnalysisView.hxx"
#include "database/model/InterfaceAnalysisStage.hxx"

#include "database/common/file.hpp"
#include "database/common/glob.hpp"
#include "database/common/environments.hpp"

#include "common/scheduler.hpp"
#include "common/file.hpp"
#include "common/assert_verify.hpp"
#include "common/terminal.hpp"
#include "common/hash.hpp"
#include "common/string.hpp"
#include "common/stl.hpp"

#include <boost/filesystem/file_status.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <boost/dll.hpp>

#include <common/stash.hpp>
#include <string>
#include <vector>
#include <iostream>

std::ostream& operator<<( std::ostream& os, const std::vector< std::string >& name )
{
    common::delimit( name.begin(), name.end(), ".", os );
    return os;
}

#define VERIFY_PARSER( expression, msg, scoped_id )                                                             \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( if ( !( expression ) ) {                                                   \
        std::ostringstream _os2;                                                                                \
        _os2 << common::COLOUR_RED_BEGIN << msg << ". Source Location: " << scoped_id->get_source_file() << ":" \
             << scoped_id->get_line_number() << common::COLOUR_END;                                             \
        throw std::runtime_error( _os2.str() );                                                                 \
    } )

namespace driver
{
    namespace interface
    {
        class BaseTask : public task::Task
        {
        public:
            BaseTask( const RawPtrSet& dependencies, const mega::io::BuildEnvironment& environment )
                : task::Task( dependencies )
                , m_environment( environment )
            {
            }

        protected:
            const mega::io::BuildEnvironment& m_environment;
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
            Task_ParseAST( const mega::io::BuildEnvironment&        environment,
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
                                    SourceRoot::Args{ pInclude->get_megaSourceFilePath(), pComponent, pIncludeContextDef } } } );

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

                    bool bRestored = false;
                    {
                        const task::FileHash astHashCode = database.save_AST_to_temp();
                        m_environment.setBuildHashCode( astFile, astHashCode );
                        if ( !m_environment.restore( astFile, astHashCode ) )
                        {
                            m_environment.temp_to_real( astFile );
                            m_environment.stash( astFile, astHashCode );
                            bRestored = false;
                        }
                    }
                    {
                        const task::FileHash bodyHashCode = database.save_Body_to_temp();
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
            Task_ObjectInterfaceGen( task::Task::RawPtr                pDependency,
                                     const mega::io::BuildEnvironment& environment,
                                     const mega::io::megaFilePath&     sourceFilePath )
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
                        VERIFY_PARSER( pInterfaceContextGroup, "Context definition of identifier: " << name << " with mixed types",
                                       pContextDef->get_id() );
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
                            VERIFY_PARSER( pParent, "Failed to locate parent for: " << name, pContextDef->get_id() );
                        }
                        pInterfaceContextGroup = constructorFunctor( database, name.back(), pParent, pContextDef );
                        pParent->push_back_children( pInterfaceContextGroup );
                        namedContexts.insert( std::make_pair( name, pInterfaceContextGroup ) );
                    }
                }
                recurse( database, pRoot, pInterfaceContextGroup, pContextDef, name, namedContexts );
            }

            struct isGlobalNamespace
            {
                bool operator()( InterfaceStage::Interface::ContextGroup* pParent )
                {
                    using namespace InterfaceStage;
                    using namespace InterfaceStage::Interface;

                    if ( Root* pParentContext = dynamic_database_cast< Root >( pParent ) )
                    {
                        return true;
                    }
                    else if ( Namespace* pParentNamespace = dynamic_database_cast< Namespace >( pParent ) )
                    {
                        return isGlobalNamespace()( pParentNamespace->get_parent() );
                    }
                    else
                    {
                        return false;
                    }
                }
            };

            void recurse( InterfaceStage::Database&                database,
                          InterfaceStage::Interface::Root*         pRoot,
                          InterfaceStage::Interface::ContextGroup* pGroup,
                          InterfaceStage::Parser::ContextDef*      pContextDef,
                          const Name&                              currentName,
                          NameResMap&                              namedContexts )
            {
                using namespace InterfaceStage;
                using namespace InterfaceStage::Interface;

                VERIFY_RTE( pContextDef );

                for ( Parser::Include* pInclude : pContextDef->get_includes() )
                {
                    if ( Parser::MegaIncludeNested* pNestedInclude = dynamic_database_cast< Parser::MegaIncludeNested >( pInclude ) )
                    {
                        Parser::IncludeRoot* pIncludeRoot       = pNestedInclude->get_root();
                        Parser::ContextDef*  pIncludeContextDef = pIncludeRoot->get_ast();
                        // NOTE: use of ContextDef here instead of NamespaceDef due to how
                        //  include root only has ContextDef*

                        const Name name                   = fromParserID( currentName, pNestedInclude->get_id()->get_ids() );
                        Namespace* pInterfaceContextGroup = nullptr;
                        {
                            if ( Interface::ContextGroup* pContextGroup = findContextGroup( name, namedContexts ) )
                            {
                                pInterfaceContextGroup = dynamic_database_cast< Namespace >( pContextGroup );
                                VERIFY_PARSER( pInterfaceContextGroup, "Context definition of identifier: " << name << " with mixed types",
                                               pContextDef->get_id() );
                                pInterfaceContextGroup->push_back_namespace_defs( pIncludeContextDef );
                            }
                            else
                            {
                                Interface::ContextGroup* pParent = pRoot;
                                if ( name.size() > 1U )
                                {
                                    Name parentName = name;
                                    parentName.pop_back();
                                    pParent = findContextGroup( parentName, namedContexts );
                                    VERIFY_PARSER( pParent, "Failed to locate parent for: " << name, pContextDef->get_id() );
                                }

                                const bool bIsGlobalNamespace = isGlobalNamespace()( pParent );

                                pInterfaceContextGroup = database.construct< Namespace >(
                                    Namespace::Args( Context::Args( ContextGroup::Args( std::vector< Context* >{} ), name.back(), pParent ),
                                                     bIsGlobalNamespace, { pIncludeContextDef }, {} ) );
                                pParent->push_back_children( pInterfaceContextGroup );
                                namedContexts.insert( std::make_pair( name, pInterfaceContextGroup ) );
                            }
                        }
                        recurse( database, pRoot, pInterfaceContextGroup, pIncludeContextDef, name, namedContexts );
                    }
                    else if ( Parser::MegaIncludeInline* pInlineInclude = dynamic_database_cast< Parser::MegaIncludeInline >( pInclude ) )
                    {
                        Parser::IncludeRoot* pIncludeRoot = pInlineInclude->get_root();
                        recurse( database, pRoot, pGroup, pIncludeRoot->get_ast(), currentName, namedContexts );
                    }
                }

                for ( Parser::ContextDef* pChildContext : pContextDef->get_children() )
                {
                    if ( Parser::NamespaceDef* pNamespaceDef = dynamic_database_cast< Parser::NamespaceDef >( pChildContext ) )
                    {
                        constructOrAggregate< Parser::NamespaceDef, Namespace >(
                            database, pRoot, pNamespaceDef, currentName, namedContexts,
                            []( Database& database, const std::string& name, ContextGroup* pParent,
                                Parser::ContextDef* pNamespaceDef ) -> Namespace*
                            {
                                const bool bIsGlobalNamespace = isGlobalNamespace()( pParent );
                                return database.construct< Namespace >(
                                    Namespace::Args( Context::Args( ContextGroup::Args( std::vector< Context* >{} ), name, pParent ),
                                                     bIsGlobalNamespace, { pNamespaceDef }, {} ) );
                            },
                            []( Namespace* pNamespace, Parser::ContextDef* pNamespaceDef )
                            { pNamespace->push_back_namespace_defs( pNamespaceDef ); } );
                    }
                    else if ( Parser::AbstractDef* pAbstractDef = dynamic_database_cast< Parser::AbstractDef >( pChildContext ) )
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
                    std::vector< Interface::DimensionInitial* > existing = pGroup->get_dimensions();
                    for ( Interface::DimensionInitial* pExistingDimension : existing )
                    {
                        VERIFY_PARSER( pParserDim->get_id()->get_str() != pExistingDimension->get_id()->get_str(),
                                       "Context has duplicate dimensions", pDef->get_id() );
                    }
                    Interface::DimensionInitial* pDimension
                        = database.construct< Interface::DimensionInitial >( Interface::DimensionInitial::Args( pParserDim ) );
                    pGroup->push_back_dimensions( pDimension );
                }
            }

            void onNamespace( InterfaceStage::Database& database, InterfaceStage::Interface::Namespace* pNamespace )
            {
                using namespace InterfaceStage;
                for ( Parser::ContextDef* pDef : pNamespace->get_namespace_defs() )
                {
                    if ( pNamespace->get_is_global() )
                    {
                        VERIFY_PARSER( pDef->get_dimensions().empty(), "Global namespace has dimensions", pDef->get_id() );
                        VERIFY_PARSER( pDef->get_dependencies().empty(), "Global namespace has dependencies", pDef->get_id() );
                    }
                    else
                    {
                        collectDimensions< InterfaceStage::Interface::Namespace, Parser::ContextDef >( database, pNamespace, pDef );
                    }

                    VERIFY_PARSER( pDef->get_body().empty(), "Namespace has body", pDef->get_id() );
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

                std::optional< std::size_t > szSizeOpt;
                for ( Parser::ActionDef* pDef : pAction->get_action_defs() )
                {
                    collectDimensions< InterfaceStage::Interface::Action, Parser::ActionDef >( database, pAction, pDef );
                }
            }
            void onEvent( InterfaceStage::Database& database, InterfaceStage::Interface::Event* pEvent )
            {
                using namespace InterfaceStage;
                std::optional< std::size_t > szSizeOpt;
                for ( Parser::EventDef* pDef : pEvent->get_event_defs() )
                {
                    collectDimensions< InterfaceStage::Interface::Event, Parser::EventDef >( database, pEvent, pDef );
                }
            }
            void onFunction( InterfaceStage::Database& database, InterfaceStage::Interface::Function* pFunction )
            {
                using namespace InterfaceStage;

                for ( Parser::FunctionDef* pDef : pFunction->get_function_defs() )
                {
                    VERIFY_PARSER( pDef->get_dimensions().empty(), "Dimension has dimensions", pDef->get_id() );
                }
            }
            void onObject( InterfaceStage::Database& database, InterfaceStage::Interface::Object* pObject )
            {
                using namespace InterfaceStage;
                for ( Parser::ObjectDef* pDef : pObject->get_object_defs() )
                {
                    collectDimensions< InterfaceStage::Interface::Object, Parser::ObjectDef >( database, pObject, pDef );
                }
            }
            void onLink( InterfaceStage::Database& database, InterfaceStage::Interface::Link* pLink )
            {
                using namespace InterfaceStage;
                for ( Parser::LinkDef* pDef : pLink->get_link_defs() )
                {
                    VERIFY_PARSER( pDef->get_dimensions().empty(), "Dimension has dimensions", pDef->get_id() );
                }
            }

            virtual void run( task::Progress& taskProgress )
            {
                const mega::io::CompilationFilePath treePath = m_environment.InterfaceStage_Tree( m_sourceFilePath );
                taskProgress.start( "Task_ObjectInterfaceGen", m_sourceFilePath.path(), treePath.path() );

                const task::FileHash parserStageASTHashCode = 
                    m_environment.getBuildHashCode( m_environment.ParserStage_AST( m_sourceFilePath ) );

                const task::DeterminantHash determinant( parserStageASTHashCode );

                if ( m_environment.restore( treePath, determinant ) )
                {
                    m_environment.setBuildHashCode( treePath );
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

                for ( Interface::Namespace* pNamespace : database.many< Interface::Namespace >( m_sourceFilePath ) )
                {
                    onNamespace( database, pNamespace );
                }
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

                const task::FileHash fileHashCode = database.save_Tree_to_temp();
                m_environment.setBuildHashCode( treePath, fileHashCode );
                m_environment.temp_to_real( treePath );
                m_environment.stash( treePath, determinant );

                taskProgress.succeeded();
            }
            const mega::io::megaFilePath& m_sourceFilePath;
        };

        class Task_DependencyAnalysis : public BaseTask
        {
            const mega::io::Manifest& m_manifest;

        public:
            Task_DependencyAnalysis( const task::Task::RawPtrSet& parserTasks, const mega::io::BuildEnvironment& environment,
                                     const mega::io::Manifest& manifest )
                : BaseTask( parserTasks, environment )
                , m_manifest( manifest )
            {
            }

            using GlobVector = std::vector< mega::io::Glob >;
            using PathSet    = std::set< mega::io::megaFilePath >;

            struct CalculateDependencies
            {
                const mega::io::BuildEnvironment& m_environment;
                CalculateDependencies( const mega::io::BuildEnvironment& env )
                    : m_environment( env )
                {
                }

                void collectDependencies( DependencyAnalysis::Parser::ContextDef* pContextDef, GlobVector& dependencyGlobs ) const
                {
                    using namespace DependencyAnalysis;

                    for ( Parser::Dependency* pDependency : pContextDef->get_dependencies() )
                    {
                        VERIFY_PARSER( !pDependency->get_str().empty(), "Empty dependency", pContextDef->get_id() );
                        boost::filesystem::path sourceFilePath = pContextDef->get_id()->get_source_file();
                        VERIFY_RTE( boost::filesystem::exists( sourceFilePath ) );
                        VERIFY_RTE( sourceFilePath.has_parent_path() );
                        dependencyGlobs.push_back( mega::io::Glob{ sourceFilePath.parent_path(), pDependency->get_str(), pContextDef } );
                    }

                    for ( Parser::ContextDef* pContext : pContextDef->get_children() )
                    {
                        collectDependencies( pContext, dependencyGlobs );
                    }
                }

                DependencyAnalysis::Dependencies::ObjectDependencies* operator()( DependencyAnalysis::Database& database,
                                                                                  const mega::io::megaFilePath& sourceFilePath,
                                                                                  const task::FileHash        interfaceHash,
                                                                                  const PathSet&                sourceFiles ) const
                {
                    using namespace DependencyAnalysis;
                    using namespace DependencyAnalysis::Dependencies;

                    GlobVector       dependencyGlobs;
                    Interface::Root* pRoot = database.one< Interface::Root >( sourceFilePath );
                    collectDependencies( pRoot->get_root()->get_ast(), dependencyGlobs );

                    std::vector< Glob* >     globs;
                    mega::io::FilePathVector matchedFilePaths;
                    for ( const mega::io::Glob& glob : dependencyGlobs )
                    {
                        try
                        {
                            mega::io::resolveGlob( glob, m_environment.rootSourceDir(), matchedFilePaths );
                        }
                        catch ( mega::io::GlobException& ex )
                        {
                            VERIFY_PARSER( false, "Dependency error: " << ex.what(),
                                           reinterpret_cast< Parser::ContextDef* >( glob.pDiagnostic )->get_id() );
                        }
                        catch ( boost::filesystem::filesystem_error& ex )
                        {
                            VERIFY_PARSER( false, "Dependency error: " << ex.what(),
                                           reinterpret_cast< Parser::ContextDef* >( glob.pDiagnostic )->get_id() );
                        }
                        globs.push_back( database.construct< Glob >( Glob::Args{ glob.source_file, glob.glob } ) );
                    }

                    mega::io::FilePathVector resolution;
                    for ( const boost::filesystem::path& filePath : matchedFilePaths )
                    {
                        if ( sourceFiles.count( m_environment.megaFilePath_fromPath( filePath ) ) )
                            resolution.push_back( filePath );
                    }

                    ObjectDependencies* pDependencies = database.construct< ObjectDependencies >(
                        ObjectDependencies::Args{ sourceFilePath, interfaceHash.get(), globs, resolution } );

                    return pDependencies;
                }

                DependencyAnalysis::Dependencies::ObjectDependencies*
                operator()( DependencyAnalysis::Database&                                   database,
                            const DependencyAnalysisView::Dependencies::ObjectDependencies* pOldDependencies,
                            const PathSet&                                                  sourceFiles ) const
                {
                    using namespace DependencyAnalysis;
                    using namespace DependencyAnalysis::Dependencies;

                    std::vector< Glob* >     globs;
                    mega::io::FilePathVector matchedFilePaths;

                    for ( DependencyAnalysisView::Dependencies::Glob* pOldGlob : pOldDependencies->get_globs() )
                    {
                        const mega::io::Glob glob{ pOldGlob->get_location(), pOldGlob->get_glob(), nullptr };
                        try
                        {
                            mega::io::resolveGlob( glob, m_environment.rootSourceDir(), matchedFilePaths );
                        }
                        catch ( mega::io::GlobException& ex )
                        {
                            VERIFY_PARSER( false, "Dependency error: " << ex.what(),
                                           reinterpret_cast< Parser::ContextDef* >( glob.pDiagnostic )->get_id() );
                        }
                        catch ( boost::filesystem::filesystem_error& ex )
                        {
                            VERIFY_PARSER( false, "Dependency error: " << ex.what(),
                                           reinterpret_cast< Parser::ContextDef* >( glob.pDiagnostic )->get_id() );
                        }
                        globs.push_back( database.construct< Glob >( Glob::Args{ glob.source_file, glob.glob } ) );
                    }

                    mega::io::FilePathVector resolution;
                    for ( const boost::filesystem::path& filePath : matchedFilePaths )
                    {
                        if ( sourceFiles.count( m_environment.megaFilePath_fromPath( filePath ) ) )
                            resolution.push_back( filePath );
                    }

                    ObjectDependencies* pDependencies = database.construct< ObjectDependencies >( ObjectDependencies::Args{
                        pOldDependencies->get_source_file(), pOldDependencies->get_hash_code(), globs, resolution } );

                    return pDependencies;
                }
            };

            PathSet getSortedSourceFiles() const
            {
                PathSet sourceFiles;
                for ( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
                {
                    sourceFiles.insert( sourceFilePath );
                }
                return sourceFiles;
            }

            virtual void run( task::Progress& taskProgress )
            {
                const mega::io::manifestFilePath    manifestFilePath = m_environment.project_manifest();
                const mega::io::CompilationFilePath dependencyCompilationFilePath
                    = m_environment.DependencyAnalysis_DPGraph( manifestFilePath );
                taskProgress.start( "Task_DependencyAnalysis", manifestFilePath.path(), dependencyCompilationFilePath.path() );

                task::DeterminantHash determinant;
                for ( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
                {
                    determinant ^= m_environment.getBuildHashCode( m_environment.ParserStage_AST( sourceFilePath ) );
                }

                if ( m_environment.restore( dependencyCompilationFilePath, determinant ) )
                {
                    m_environment.setBuildHashCode( dependencyCompilationFilePath );
                    taskProgress.cached();
                    return;
                }

                // try loading previous one...
                if ( m_environment.exists( dependencyCompilationFilePath ) )
                {
                    // attempt to load previous dependency analysis
                    namespace Old = DependencyAnalysisView;
                    namespace New = DependencyAnalysis;

                    New::Database newDatabase( m_environment, manifestFilePath );
                    {
                        Old::Database oldDatabase( m_environment, manifestFilePath );

                        const Old::Dependencies::Analysis* pOldAnalysis
                            = oldDatabase.one< Old::Dependencies::Analysis >( manifestFilePath );
                        VERIFY_RTE( pOldAnalysis );
                        using OldDependenciesVector              = std::vector< Old::Dependencies::ObjectDependencies* >;
                        const OldDependenciesVector dependencies = pOldAnalysis->get_objects();
                        const PathSet               sourceFiles  = getSortedSourceFiles();

                        struct Compare
                        {
                            const mega::io::Environment& env;
                            Compare( const mega::io::Environment& env )
                                : env( env )
                            {
                            }
                            bool operator()( OldDependenciesVector::const_iterator i, PathSet::const_iterator j ) const
                            {
                                const Old::Dependencies::ObjectDependencies* pDependencies = *i;
                                return pDependencies->get_source_file() < *j;
                            }
                            bool opposite( OldDependenciesVector::const_iterator i, PathSet::const_iterator j ) const
                            {
                                const Old::Dependencies::ObjectDependencies* pDependencies = *i;
                                return *j < pDependencies->get_source_file();
                            }
                        } comparator( m_environment );

                        using NewDependenciesVector = std::vector< New::Dependencies::ObjectDependencies* >;
                        NewDependenciesVector newDependencies;
                        {
                            generics::matchGetUpdates(
                                dependencies.begin(), dependencies.end(), sourceFiles.begin(), sourceFiles.end(),
                                // const Compare& cmp
                                comparator,

                                // const Update& shouldUpdate
                                [ &env = m_environment, &newDependencies, &newDatabase, &sourceFiles, &taskProgress ](
                                    OldDependenciesVector::const_iterator i, PathSet::const_iterator j ) -> bool
                                {
                                    const Old::Dependencies::ObjectDependencies* pDependencies = *i;
                                    const task::FileHash interfaceHash = env.getBuildHashCode( env.ParserStage_AST( *j ) );
                                    if ( interfaceHash == pDependencies->get_hash_code() )
                                    {
                                        // since the code is NOT modified - can re use the globs from previous result
                                        newDependencies.push_back(
                                            CalculateDependencies( env )( newDatabase, pDependencies, sourceFiles ) );

                                        std::ostringstream os;
                                        os << "\tPartially reusing dependencies for: " << j->path().string();
                                        taskProgress.msg( os.str() );
                                        return false;
                                    }
                                    else
                                    {
                                        std::ostringstream os;
                                        os << "\tRecalculating dependencies for: " << j->path().string();
                                        taskProgress.msg( os.str() );
                                        return true;
                                    }
                                },

                                // const Removal& rem
                                []( OldDependenciesVector::const_iterator i )
                                {
                                    // a source file has been removed - can ignor this since
                                    // recreating the dependency analysis and only attempting to
                                    // reuse the globs
                                },

                                // const Addition& add
                                [ &env = m_environment, &newDatabase, &newDependencies, &sourceFiles,
                                  &taskProgress ]( PathSet::const_iterator j )
                                {
                                    // a new source file is added so must analysis from the ground up
                                    const mega::io::megaFilePath megaFilePath = *j;
                                    const task::FileHash interfaceHash      = env.getBuildHashCode( env.ParserStage_AST( megaFilePath ) );
                                    newDependencies.push_back(
                                        CalculateDependencies( env )( newDatabase, megaFilePath, interfaceHash, sourceFiles ) );

                                    std::ostringstream os;
                                    os << "\tAdding dependencies for: " << megaFilePath.path().string();
                                    taskProgress.msg( os.str() );
                                },

                                // const Updated& updatesNeeded
                                [ &env = m_environment, &newDatabase, &newDependencies,
                                  &sourceFiles ]( OldDependenciesVector::const_iterator i )
                                {
                                    // since the code is modified - must re analyse ALL dependencies from the ground up
                                    const Old::Dependencies::ObjectDependencies* pDependencies = *i;
                                    const mega::io::megaFilePath                 megaFilePath  = pDependencies->get_source_file();
                                    const task::FileHash interfaceHash = env.getBuildHashCode( env.ParserStage_AST( megaFilePath ) );
                                    newDependencies.push_back(
                                        CalculateDependencies( env )( newDatabase, megaFilePath, interfaceHash, sourceFiles ) );
                                } );
                        }

                        newDatabase.construct< New::Dependencies::Analysis >( New::Dependencies::Analysis::Args{ newDependencies } );
                    }

                    const task::FileHash fileHashCode = newDatabase.save_DPGraph_to_temp();
                    m_environment.setBuildHashCode( dependencyCompilationFilePath, fileHashCode );
                    m_environment.temp_to_real( dependencyCompilationFilePath );
                    m_environment.stash( dependencyCompilationFilePath, determinant );

                    taskProgress.succeeded();
                }
                else
                {
                    using namespace DependencyAnalysis;
                    using namespace DependencyAnalysis::Dependencies;

                    Database database( m_environment, manifestFilePath );
                    {
                        std::vector< ObjectDependencies* > dependencies;
                        {
                            const PathSet sourceFiles = getSortedSourceFiles();
                            for ( const mega::io::megaFilePath& sourceFilePath : sourceFiles )
                            {
                                const task::FileHash interfaceHash
                                    = m_environment.getBuildHashCode( m_environment.ParserStage_AST( sourceFilePath ) );
                                dependencies.push_back(
                                    CalculateDependencies( m_environment )( database, sourceFilePath, interfaceHash, sourceFiles ) );
                            }
                        }
                        database.construct< Analysis >( Analysis::Args{ dependencies } );
                    }

                    const task::FileHash fileHashCode = database.save_DPGraph_to_temp();
                    m_environment.setBuildHashCode( dependencyCompilationFilePath, fileHashCode );
                    m_environment.temp_to_real( dependencyCompilationFilePath );
                    m_environment.stash( dependencyCompilationFilePath, determinant );

                    taskProgress.succeeded();
                }
            }
        };

        class Task_ObjectInterfaceAnalysis : public BaseTask
        {
        public:
            Task_ObjectInterfaceAnalysis( task::Task::RawPtrSet             dependencies,
                                          const mega::io::BuildEnvironment& environment,
                                          const mega::io::megaFilePath&     sourceFilePath )
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
                const mega::io::CompilationFilePath interfaceTreeFile     = m_environment.InterfaceStage_Tree( m_sourceFilePath );
                const mega::io::CompilationFilePath interfaceAnalysisFile = m_environment.InterfaceAnalysisStage_Clang( m_sourceFilePath );
                taskProgress.start( "Task_ObjectInterfaceAnalysis", interfaceTreeFile.path(), interfaceAnalysisFile.path() );

                const task::DeterminantHash determinant = m_environment.getBuildHashCode( interfaceTreeFile );

                if ( m_environment.restore( interfaceAnalysisFile, determinant ) )
                {
                    m_environment.setBuildHashCode( interfaceAnalysisFile );
                    taskProgress.cached();
                    return;
                }

                using namespace InterfaceAnalysisStage;
                using namespace InterfaceAnalysisStage::Interface;

                Database database( m_environment, m_sourceFilePath );

                // generate interface

                const task::FileHash fileHashCode = database.save_Clang_to_temp();
                m_environment.setBuildHashCode( interfaceAnalysisFile, fileHashCode );
                m_environment.temp_to_real( interfaceAnalysisFile );
                m_environment.stash( interfaceAnalysisFile, determinant );

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

                Task_DependencyAnalysis* pDependencyAnalysisTask = new Task_DependencyAnalysis( interfaceGenTasks, environment, manifest );
                tasks.push_back( task::Task::Ptr( pDependencyAnalysisTask ) );

                for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
                {
                    Task_ObjectInterfaceAnalysis* pObjectInterfaceAnalysis
                        = new Task_ObjectInterfaceAnalysis( { pDependencyAnalysisTask }, environment, sourceFilePath );
                    tasks.push_back( task::Task::Ptr( pObjectInterfaceAnalysis ) );
                }

                task::Schedule::Ptr pSchedule( new task::Schedule( tasks ) );
                task::run( pSchedule, std::cout );
            }
        }
    } // namespace interface
} // namespace driver
