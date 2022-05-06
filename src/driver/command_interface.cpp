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
#include "database/model/SymbolAnalysis.hxx"
#include "database/model/SymbolAnalysisView.hxx"
#include "database/model/SymbolRollout.hxx"
#include "database/model/InterfaceAnalysisStage.hxx"

#include "database/common/file.hpp"
#include "database/common/environments.hpp"

#include "utilities/clang_format.hpp"
#include "utilities/glob.hpp"
#include "utilities/clang_format.hpp"
#include "utilities/clang_compilation.hpp"

#include "common/scheduler.hpp"
#include "common/file.hpp"
#include "common/assert_verify.hpp"
#include "common/terminal.hpp"
#include "common/hash.hpp"
#include "common/string.hpp"
#include "common/stl.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include <boost/filesystem/file_status.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <boost/dll.hpp>
#pragma warning( push )
#pragma warning( disable : 4996 ) // iterator thing
#pragma warning( disable : 4244 ) // conversion to DWORD from system_clock::rep
#include <boost/process.hpp>
#pragma warning( pop )

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
    BaseTask( const RawPtrSet& dependencies, const mega::io::BuildEnvironment& environment,
              const task::DeterminantHash toolChainHash )
        : task::Task( dependencies )
        , m_environment( environment )
        , m_toolChainHash( toolChainHash )
    {
    }

protected:
    const mega::io::BuildEnvironment& m_environment;
    const task::DeterminantHash       m_toolChainHash;
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
    const boost::filesystem::path            m_parserDLL;
    const mega::io::megaFilePath&            m_sourceFilePath;
    const std::vector< mega::io::FileInfo >& m_fileInfos;
    using FileRootMap = std::map< boost::filesystem::path, ParserStage::Parser::SourceRoot* >;
    FileRootMap m_rootFiles;

public:
    Task_ParseAST( const mega::io::BuildEnvironment&        environment,
                   const task::DeterminantHash              toolChainHash,
                   const boost::filesystem::path&           parserDll,
                   const mega::io::megaFilePath&            sourceFilePath,
                   const std::vector< mega::io::FileInfo >& fileInfos )
        : BaseTask( {}, environment, toolChainHash )
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
            for ( Components::Component* pIter :
                  database.many< Components::Component >( m_environment.project_manifest() ) )
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
            VERIFY_RTE_MSG(
                pComponent, "Failed to locate component for source file: " << m_sourceFilePath.path().string() );
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
                        ContextDef* pIncludeContextDef
                            = parseInputFile( database, pComponent,
                                              m_environment.megaFilePath_fromPath( pInclude->get_megaSourceFilePath() ),
                                              osError, osWarn );

                        IncludeRoot* pIncludeRoot
                            = database.construct< IncludeRoot >( { IncludeRoot::Args{ SourceRoot::Args{
                                pInclude->get_megaSourceFilePath(), pComponent, pIncludeContextDef } } } );

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

                const task::DeterminantHash determinant( { m_toolChainHash, astHashCode } );

                if ( !m_environment.restore( astFile, determinant ) )
                {
                    m_environment.temp_to_real( astFile );
                    m_environment.stash( astFile, determinant );
                    bRestored = false;
                }
            }
            {
                const task::FileHash bodyHashCode = database.save_Body_to_temp();
                m_environment.setBuildHashCode( bodyFile, bodyHashCode );

                const task::DeterminantHash determinant( { m_toolChainHash, bodyHashCode } );

                if ( !m_environment.restore( bodyFile, determinant ) )
                {
                    m_environment.temp_to_real( bodyFile );
                    m_environment.stash( bodyFile, determinant );
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
};

class Task_Include : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_Include( task::Task::RawPtr pDependency, const mega::io::BuildEnvironment& environment,
                  const task::DeterminantHash toolChainHash, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( { pDependency }, environment, toolChainHash )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( task::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath        astFile         = m_environment.ParserStage_AST( m_sourceFilePath );
        const mega::io::GeneratedHPPSourceFilePath includeFilePath = m_environment.Include( m_sourceFilePath );
        taskProgress.start( "Task_Include", astFile.path(), includeFilePath.path() );

        const task::DeterminantHash determinant( { m_toolChainHash, m_environment.getBuildHashCode( astFile ) } );

        if ( m_environment.restore( includeFilePath, determinant ) )
        {
            m_environment.setBuildHashCode( includeFilePath );
            taskProgress.cached();
            return;
        }

        {
            using namespace InterfaceStage;
            using namespace InterfaceStage::Interface;

            Database database( m_environment, m_sourceFilePath );

            std::unique_ptr< boost::filesystem::ofstream > pOStream = m_environment.write( includeFilePath );

            for ( Parser::CPPInclude* pCPPInclude : database.many< Parser::CPPInclude >( m_sourceFilePath ) )
            {
                *pOStream << "#include \"" << pCPPInclude->get_cppSourceFilePath().string() << "\"\n";
            }

            // mega library includes
            *pOStream << "#include \"mega/include.hpp\"\n";

            for ( Parser::SystemInclude* pSystemInclude : database.many< Parser::SystemInclude >( m_sourceFilePath ) )
            {
                *pOStream << "#include <" << pSystemInclude->get_str() << ">\n";
            }
            *pOStream << "\n";
        }

        m_environment.setBuildHashCode( includeFilePath );
        m_environment.stash( includeFilePath, determinant );

        taskProgress.succeeded();
    }
};

class Task_IncludePCH : public BaseTask
{
    const boost::filesystem::path& m_compilerPath;
    const mega::io::megaFilePath&  m_sourceFilePath;

public:
    Task_IncludePCH( task::Task::RawPtr pDependency, const mega::io::BuildEnvironment& environment,
                     const task::DeterminantHash toolChainHash, const boost::filesystem::path& compilerPath,
                     const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( { pDependency }, environment, toolChainHash )
        , m_compilerPath( compilerPath )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( task::Progress& taskProgress )
    {
        const mega::io::GeneratedHPPSourceFilePath includeFilePath = m_environment.Include( m_sourceFilePath );
        const mega::io::PrecompiledHeaderFile      pchPath         = m_environment.PCH( m_sourceFilePath );
        taskProgress.start( "Task_IncludePCH", includeFilePath.path(), pchPath.path() );

        const task::DeterminantHash determinant(
            { m_toolChainHash, m_environment.getBuildHashCode( includeFilePath ) } );

        if ( m_environment.restore( pchPath, determinant ) )
        {
            m_environment.setBuildHashCode( pchPath );
            taskProgress.cached();
            return;
        }

        using namespace ParserStage;

        Database database( m_environment, m_sourceFilePath );

        Components::Component* pComponent = nullptr;
        {
            for ( Components::Component* pIter :
                  database.many< Components::Component >( m_environment.project_manifest() ) )
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
            VERIFY_RTE_MSG(
                pComponent, "Failed to locate component for source file: " << m_sourceFilePath.path().string() );
        }

        const std::string strCmd = mega::utilities::Compilation(
            m_compilerPath, pComponent->get_cpp_flags(), pComponent->get_cpp_defines(),
            pComponent->get_includeDirectories(), m_environment.FilePath( includeFilePath ),
            m_environment.FilePath( pchPath ) )();

        taskProgress.msg( strCmd );

        const int iResult = boost::process::system( strCmd );
        if ( iResult )
        {
            std::ostringstream os;
            os << "Error compiling include files to pch for source file: " << m_sourceFilePath.path();
            throw std::runtime_error( os.str() );
        }

        m_environment.setBuildHashCode( pchPath );
        m_environment.stash( pchPath, determinant );

        taskProgress.succeeded();
    }
};

class Task_ObjectInterfaceGen : public BaseTask
{
public:
    Task_ObjectInterfaceGen( task::Task::RawPtr                pDependency,
                             const mega::io::BuildEnvironment& environment,
                             const task::DeterminantHash       toolChainHash,
                             const mega::io::megaFilePath&     sourceFilePath )
        : BaseTask( { pDependency }, environment, toolChainHash )
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
                VERIFY_PARSER( pInterfaceContextGroup,
                               "Context definition of identifier: " << name << " with mixed types",
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
            if ( Parser::MegaIncludeNested* pNestedInclude
                 = dynamic_database_cast< Parser::MegaIncludeNested >( pInclude ) )
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
                        VERIFY_PARSER( pInterfaceContextGroup,
                                       "Context definition of identifier: " << name << " with mixed types",
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

                        pInterfaceContextGroup = database.construct< Namespace >( Namespace::Args(
                            Context::Args( ContextGroup::Args( std::vector< Context* >{} ), name.back(), pParent ),
                            bIsGlobalNamespace, { pIncludeContextDef } ) );
                        pParent->push_back_children( pInterfaceContextGroup );
                        namedContexts.insert( std::make_pair( name, pInterfaceContextGroup ) );
                    }
                }
                recurse( database, pRoot, pInterfaceContextGroup, pIncludeContextDef, name, namedContexts );
            }
            else if ( Parser::MegaIncludeInline* pInlineInclude
                      = dynamic_database_cast< Parser::MegaIncludeInline >( pInclude ) )
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
                        return database.construct< Namespace >( Namespace::Args(
                            Context::Args( ContextGroup::Args( std::vector< Context* >{} ), name, pParent ),
                            bIsGlobalNamespace, { pNamespaceDef } ) );
                    },
                    []( Namespace* pNamespace, Parser::ContextDef* pNamespaceDef )
                    { pNamespace->push_back_namespace_defs( pNamespaceDef ); } );
            }
            else if ( Parser::AbstractDef* pAbstractDef
                      = dynamic_database_cast< Parser::AbstractDef >( pChildContext ) )
            {
                constructOrAggregate< Parser::AbstractDef, Abstract >(
                    database, pRoot, pAbstractDef, currentName, namedContexts,
                    []( Database& database, const std::string& name, ContextGroup* pParent,
                        Parser::AbstractDef* pAbstractDef ) -> Abstract*
                    {
                        return database.construct< Abstract >( Abstract::Args(
                            Context::Args( ContextGroup::Args( std::vector< Context* >{} ), name, pParent ),
                            { pAbstractDef } ) );
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
                            Context::Args( ContextGroup::Args( std::vector< Context* >{} ), name, pParent ),
                            { pActionDef } ) );
                    },
                    []( Action* pAction, Parser::ActionDef* pActionDef )
                    { pAction->push_back_action_defs( pActionDef ); } );
            }
            else if ( Parser::EventDef* pEventDef = dynamic_database_cast< Parser::EventDef >( pChildContext ) )
            {
                constructOrAggregate< Parser::EventDef, Event >(
                    database, pRoot, pEventDef, currentName, namedContexts,
                    []( Database& database, const std::string& name, ContextGroup* pParent,
                        Parser::EventDef* pEventDef ) -> Event*
                    {
                        return database.construct< Event >( Event::Args(
                            Context::Args( ContextGroup::Args( std::vector< Context* >{} ), name, pParent ),
                            { pEventDef } ) );
                    },
                    []( Event* pEvent, Parser::EventDef* pEventDef ) { pEvent->push_back_event_defs( pEventDef ); } );
            }
            else if ( Parser::FunctionDef* pFunctionDef
                      = dynamic_database_cast< Parser::FunctionDef >( pChildContext ) )
            {
                constructOrAggregate< Parser::FunctionDef, Function >(
                    database, pRoot, pFunctionDef, currentName, namedContexts,
                    []( Database& database, const std::string& name, ContextGroup* pParent,
                        Parser::FunctionDef* pFunctionDef ) -> Function*
                    {
                        return database.construct< Function >( Function::Args(
                            Context::Args( ContextGroup::Args( std::vector< Context* >{} ), name, pParent ),
                            { pFunctionDef } ) );
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
                            Context::Args( ContextGroup::Args( std::vector< Context* >{} ), name, pParent ),
                            { pObjectDef } ) );
                    },
                    []( Object* pObject, Parser::ObjectDef* pObjectDef )
                    { pObject->push_back_object_defs( pObjectDef ); } );
            }
            else if ( Parser::LinkDef* pLinkDef = dynamic_database_cast< Parser::LinkDef >( pChildContext ) )
            {
                constructOrAggregate< Parser::LinkDef, Link >(
                    database, pRoot, pLinkDef, currentName, namedContexts,
                    []( Database& database, const std::string& name, ContextGroup* pParent,
                        Parser::LinkDef* pLinkDef ) -> Link*
                    {
                        return database.construct< Link >(
                            Link::Args( Context::Args( ContextGroup::Args( std::vector< Context* >{} ), name, pParent ),
                                        { pLinkDef } ) );
                    },
                    []( Link* pLink, Parser::LinkDef* pLinkDef ) { pLink->push_back_link_defs( pLinkDef ); } );
            }
            else
            {
                THROW_RTE( "Unknown context type" );
            }
        }
    }

    void collectDimensionTraits( InterfaceStage::Database& database, InterfaceStage::Parser::ContextDef* pDef,
                                 std::vector< InterfaceStage::Interface::DimensionTrait* >& dimensions )
    {
        using namespace InterfaceStage;

        for ( Parser::Dimension* pParserDim : pDef->get_dimensions() )
        {
            for ( Parser::Dimension* pExistingDimension : dimensions )
            {
                VERIFY_PARSER( pParserDim->get_id()->get_str() != pExistingDimension->get_id()->get_str(),
                               "Context has duplicate dimensions", pDef->get_id() );
            }
            dimensions.push_back(
                database.construct< Interface::DimensionTrait >( Interface::DimensionTrait::Args( pParserDim ) ) );
        }
    }

    template < typename TParserType >
    void collectInheritanceTrait( InterfaceStage::Database& database, TParserType* pContextDef,
                                  std::optional< InterfaceStage::Interface::InheritanceTrait* >& inheritance )
    {
        using namespace InterfaceStage;
        Parser::Inheritance*              pInheritanceDef = pContextDef->get_inheritance();
        const std::vector< std::string >& strings         = pInheritanceDef->get_strings();
        if ( !strings.empty() )
        {
            VERIFY_PARSER( !inheritance.has_value(), "Duplicate inheritance specified", pContextDef->get_id() );
            inheritance = database.construct< Interface::InheritanceTrait >(
                Interface::InheritanceTrait::Args{ pInheritanceDef } );
        }
    }

    template < typename TParserType >
    void collectSizeTrait( InterfaceStage::Database& database, TParserType* pContextDef,
                           std::optional< InterfaceStage::Interface::SizeTrait* >& size )
    {
        using namespace InterfaceStage;
        Parser::Size*      pSizeDef = pContextDef->get_size();
        const std::string& str      = pSizeDef->get_str();
        if ( !str.empty() )
        {
            VERIFY_PARSER( !size.has_value(), "Duplicate size specified", pContextDef->get_id() );
            size = database.construct< Interface::SizeTrait >( Interface::SizeTrait::Args{ pSizeDef } );
        }
    }

    void onNamespace( InterfaceStage::Database& database, InterfaceStage::Interface::Namespace* pNamespace )
    {
        using namespace InterfaceStage;
        std::vector< InterfaceStage::Interface::DimensionTrait* > dimensions;
        {
            for ( Parser::ContextDef* pDef : pNamespace->get_namespace_defs() )
            {
                if ( pNamespace->get_is_global() )
                {
                    VERIFY_PARSER( pDef->get_dimensions().empty(), "Global namespace has dimensions", pDef->get_id() );
                    VERIFY_PARSER(
                        pDef->get_dependencies().empty(), "Global namespace has dependencies", pDef->get_id() );
                }
                else
                {
                    collectDimensionTraits( database, pDef, dimensions );
                }

                VERIFY_PARSER( pDef->get_body().empty(), "Namespace has body", pDef->get_id() );
            }
        }

        pNamespace->set_dimension_traits( dimensions );
    }

    void onAbstract( InterfaceStage::Database& database, InterfaceStage::Interface::Abstract* pAbstract )
    {
        using namespace InterfaceStage;
        std::vector< InterfaceStage::Interface::DimensionTrait* > dimensions;
        std::optional< Interface::InheritanceTrait* >             inheritance;
        for ( Parser::AbstractDef* pDef : pAbstract->get_abstract_defs() )
        {
            collectDimensionTraits( database, pDef, dimensions );
            collectInheritanceTrait( database, pDef, inheritance );
        }

        pAbstract->set_dimension_traits( dimensions );
        pAbstract->set_inheritance_trait( inheritance );
    }
    void onAction( InterfaceStage::Database& database, InterfaceStage::Interface::Action* pAction )
    {
        using namespace InterfaceStage;

        std::vector< InterfaceStage::Interface::DimensionTrait* > dimensions;
        std::optional< Interface::InheritanceTrait* >             inheritance;
        std::optional< Interface::SizeTrait* >                    size;
        for ( Parser::ActionDef* pDef : pAction->get_action_defs() )
        {
            collectDimensionTraits( database, pDef, dimensions );
            collectInheritanceTrait( database, pDef, inheritance );
            collectSizeTrait( database, pDef, size );
        }

        pAction->set_dimension_traits( dimensions );
        pAction->set_inheritance_trait( inheritance );
        pAction->set_size_trait( size );
    }
    void onEvent( InterfaceStage::Database& database, InterfaceStage::Interface::Event* pEvent )
    {
        using namespace InterfaceStage;
        std::vector< InterfaceStage::Interface::DimensionTrait* > dimensions;
        std::optional< Interface::InheritanceTrait* >             inheritance;
        std::optional< Interface::SizeTrait* >                    size;
        for ( Parser::EventDef* pDef : pEvent->get_event_defs() )
        {
            collectDimensionTraits( database, pDef, dimensions );
            collectInheritanceTrait( database, pDef, inheritance );
            collectSizeTrait( database, pDef, size );
        }

        pEvent->set_dimension_traits( dimensions );
        pEvent->set_inheritance_trait( inheritance );
        pEvent->set_size_trait( size );
    }
    void onFunction( InterfaceStage::Database& database, InterfaceStage::Interface::Function* pFunction )
    {
        using namespace InterfaceStage;

        Interface::ArgumentListTrait* pArgumentListTrait = nullptr;
        Interface::ReturnTypeTrait*   pReturnTypeTrait   = nullptr;

        std::string strArguments, strReturnType;
        for ( Parser::FunctionDef* pDef : pFunction->get_function_defs() )
        {
            VERIFY_PARSER( pDef->get_dimensions().empty(), "Dimension has dimensions", pDef->get_id() );

            Parser::ArgumentList* pArguments = pDef->get_argumentList();
            // if ( !pArguments->get_str().empty() )
            {
                if ( !pArgumentListTrait )
                {
                    pArgumentListTrait = database.construct< Interface::ArgumentListTrait >(
                        Interface::ArgumentListTrait::Args{ pArguments } );
                    strArguments = pArguments->get_str();
                }
                else
                {
                    VERIFY_PARSER(
                        strArguments == pArguments->get_str(), "Function arguments mismatch", pDef->get_id() );
                }
            }

            Parser::ReturnType* pReturnType = pDef->get_returnType();
            // if ( !pReturnType->get_str().empty() )
            {
                if ( !pReturnTypeTrait )
                {
                    pReturnTypeTrait = database.construct< Interface::ReturnTypeTrait >(
                        Interface::ReturnTypeTrait::Args{ pReturnType } );
                    strReturnType = pReturnType->get_str();
                }
                else
                {
                    VERIFY_PARSER( strReturnType == pReturnType->get_str(), "Return type mismatch", pDef->get_id() );
                }
            }
        }

        VERIFY_PARSER(
            pArgumentListTrait, "Function missing argument list", pFunction->get_function_defs().front()->get_id() );
        VERIFY_PARSER(
            pReturnTypeTrait, "Function missing return type", pFunction->get_function_defs().front()->get_id() );

        pFunction->set_arguments_trait( pArgumentListTrait );
        pFunction->set_return_type_trait( pReturnTypeTrait );
    }
    void onObject( InterfaceStage::Database& database, InterfaceStage::Interface::Object* pObject )
    {
        using namespace InterfaceStage;
        std::vector< InterfaceStage::Interface::DimensionTrait* > dimensions;
        std::optional< Interface::InheritanceTrait* >             inheritance;
        for ( Parser::ObjectDef* pDef : pObject->get_object_defs() )
        {
            collectDimensionTraits( database, pDef, dimensions );
            collectInheritanceTrait( database, pDef, inheritance );
            VERIFY_PARSER( pDef->get_body().empty(), "Object has body", pDef->get_id() );
        }

        pObject->set_dimension_traits( dimensions );
        pObject->set_inheritance_trait( inheritance );
    }
    void onLink( InterfaceStage::Database& database, InterfaceStage::Interface::Link* pLink )
    {
        using namespace InterfaceStage;
        std::optional< Interface::InheritanceTrait* > inheritance;
        for ( Parser::LinkDef* pDef : pLink->get_link_defs() )
        {
            VERIFY_PARSER( pDef->get_dimensions().empty(), "Dimension has dimensions", pDef->get_id() );
            collectInheritanceTrait( database, pDef, inheritance );
            VERIFY_PARSER( pDef->get_body().empty(), "Link has body", pDef->get_id() );
        }
        VERIFY_PARSER( inheritance.has_value(), "Link missing inheritance specification",
                       pLink->get_link_defs().front()->get_id() );
        pLink->set_link_inheritance_trait( inheritance.value() );
    }

    virtual void run( task::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath treePath = m_environment.InterfaceStage_Tree( m_sourceFilePath );
        taskProgress.start( "Task_ObjectInterfaceGen", m_sourceFilePath.path(), treePath.path() );

        const task::FileHash parserStageASTHashCode
            = m_environment.getBuildHashCode( m_environment.ParserStage_AST( m_sourceFilePath ) );

        const task::DeterminantHash determinant( { m_toolChainHash, parserStageASTHashCode } );

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
                             const task::DeterminantHash toolChainHash, const mega::io::Manifest& manifest )
        : BaseTask( parserTasks, environment, toolChainHash )
        , m_manifest( manifest )
    {
    }

    using GlobVector = std::vector< mega::utilities::Glob >;
    using PathSet    = std::set< mega::io::megaFilePath >;

    struct CalculateDependencies
    {
        const mega::io::BuildEnvironment& m_environment;
        CalculateDependencies( const mega::io::BuildEnvironment& env )
            : m_environment( env )
        {
        }

        void collectDependencies( DependencyAnalysis::Parser::ContextDef* pContextDef,
                                  GlobVector&                             dependencyGlobs ) const
        {
            using namespace DependencyAnalysis;

            for ( Parser::Dependency* pDependency : pContextDef->get_dependencies() )
            {
                VERIFY_PARSER( !pDependency->get_str().empty(), "Empty dependency", pContextDef->get_id() );
                boost::filesystem::path sourceFilePath = pContextDef->get_id()->get_source_file();
                VERIFY_RTE( boost::filesystem::exists( sourceFilePath ) );
                VERIFY_RTE( sourceFilePath.has_parent_path() );
                dependencyGlobs.push_back(
                    mega::utilities::Glob{ sourceFilePath.parent_path(), pDependency->get_str(), pContextDef } );
            }

            for ( Parser::ContextDef* pContext : pContextDef->get_children() )
            {
                collectDependencies( pContext, dependencyGlobs );
            }
        }

        DependencyAnalysis::Dependencies::ObjectDependencies* operator()( DependencyAnalysis::Database& database,
                                                                          const mega::io::megaFilePath& sourceFilePath,
                                                                          const task::DeterminantHash   interfaceHash,
                                                                          const PathSet& sourceFiles ) const
        {
            using namespace DependencyAnalysis;
            using namespace DependencyAnalysis::Dependencies;

            GlobVector       dependencyGlobs;
            Interface::Root* pRoot = database.one< Interface::Root >( sourceFilePath );
            collectDependencies( pRoot->get_root()->get_ast(), dependencyGlobs );

            std::vector< Glob* >            globs;
            mega::utilities::FilePathVector matchedFilePaths;
            for ( const mega::utilities::Glob& glob : dependencyGlobs )
            {
                try
                {
                    mega::utilities::resolveGlob( glob, m_environment.rootSourceDir(), matchedFilePaths );
                }
                catch ( mega::utilities::GlobException& ex )
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

            mega::utilities::FilePathVector resolution;
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

            std::vector< Glob* >            globs;
            mega::utilities::FilePathVector matchedFilePaths;

            for ( DependencyAnalysisView::Dependencies::Glob* pOldGlob : pOldDependencies->get_globs() )
            {
                const mega::utilities::Glob glob{ pOldGlob->get_location(), pOldGlob->get_glob(), nullptr };
                try
                {
                    mega::utilities::resolveGlob( glob, m_environment.rootSourceDir(), matchedFilePaths );
                }
                catch ( mega::utilities::GlobException& ex )
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

            mega::utilities::FilePathVector resolution;
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

        task::DeterminantHash determinant( m_toolChainHash );
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

        struct InterfaceHashCodeGenerator
        {
            const mega::io::BuildEnvironment& env;
            task::DeterminantHash             toolChainHash;
            InterfaceHashCodeGenerator( const mega::io::BuildEnvironment& env, task::DeterminantHash toolChainHash )
                : env( env )
                , toolChainHash( toolChainHash )
            {
            }
            inline task::DeterminantHash operator()( const mega::io::megaFilePath& megaFilePath ) const
            {
                return task::DeterminantHash(
                    { toolChainHash, env.getBuildHashCode( env.ParserStage_AST( megaFilePath ) ) } );
            }

        } hashCodeGenerator( m_environment, m_toolChainHash );

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
                        [ &env = m_environment, &hashCodeGenerator, &newDependencies, &newDatabase, &sourceFiles,
                          &taskProgress ]( OldDependenciesVector::const_iterator i, PathSet::const_iterator j ) -> bool
                        {
                            const Old::Dependencies::ObjectDependencies* pDependencies = *i;
                            const task::DeterminantHash                  interfaceHash = hashCodeGenerator( *j );
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
                        [ &env = m_environment, &hashCodeGenerator, &newDatabase, &newDependencies, &sourceFiles,
                          &taskProgress ]( PathSet::const_iterator j )
                        {
                            // a new source file is added so must analysis from the ground up
                            const mega::io::megaFilePath megaFilePath  = *j;
                            const task::DeterminantHash  interfaceHash = hashCodeGenerator( megaFilePath );
                            newDependencies.push_back(
                                CalculateDependencies( env )( newDatabase, megaFilePath, interfaceHash, sourceFiles ) );
                            std::ostringstream os;
                            os << "\tAdding dependencies for: " << megaFilePath.path().string();
                            taskProgress.msg( os.str() );
                        },

                        // const Updated& updatesNeeded
                        [ &env = m_environment, &hashCodeGenerator, &newDatabase, &newDependencies,
                          &sourceFiles ]( OldDependenciesVector::const_iterator i )
                        {
                            // since the code is modified - must re analyse ALL dependencies from the ground up
                            const Old::Dependencies::ObjectDependencies* pDependencies = *i;
                            const mega::io::megaFilePath megaFilePath  = pDependencies->get_source_file();
                            const task::DeterminantHash  interfaceHash = hashCodeGenerator( megaFilePath );
                            newDependencies.push_back(
                                CalculateDependencies( env )( newDatabase, megaFilePath, interfaceHash, sourceFiles ) );
                        } );
                }

                newDatabase.construct< New::Dependencies::Analysis >(
                    New::Dependencies::Analysis::Args{ newDependencies } );
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
                        const task::DeterminantHash interfaceHash = hashCodeGenerator( sourceFilePath );
                        dependencies.push_back( CalculateDependencies( m_environment )(
                            database, sourceFilePath, interfaceHash, sourceFiles ) );
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

class Task_SymbolAnalysis : public BaseTask
{
    const mega::io::Manifest& m_manifest;

public:
    struct InterfaceHashCodeGenerator
    {
        const mega::io::BuildEnvironment& env;
        task::DeterminantHash             toolChainHash;
        InterfaceHashCodeGenerator( const mega::io::BuildEnvironment& env, task::DeterminantHash toolChainHash )
            : env( env )
            , toolChainHash( toolChainHash )
        {
        }
        inline task::DeterminantHash operator()( const mega::io::megaFilePath& megaFilePath ) const
        {
            return task::DeterminantHash(
                { toolChainHash, env.getBuildHashCode( env.InterfaceStage_Tree( megaFilePath ) ) } );
        }
    };

    Task_SymbolAnalysis( const task::Task::RawPtrSet& parserTasks, const mega::io::BuildEnvironment& environment,
                         const task::DeterminantHash toolChainHashCode, const mega::io::Manifest& manifest )
        : BaseTask( parserTasks, environment, toolChainHashCode )
        , m_manifest( manifest )
    {
    }

    using PathSet = std::set< mega::io::megaFilePath >;
    PathSet getSortedSourceFiles() const
    {
        PathSet sourceFiles;
        for ( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            sourceFiles.insert( sourceFilePath );
        }
        return sourceFiles;
    }

    using SymbolMap = std::map< std::string, ::SymbolAnalysis::Symbols::Symbol* >;
    struct ContextDimensionSymbolSet
    {
        SymbolAnalysis::Symbols::SymbolSet*        pSymbolSet = nullptr;
        SymbolAnalysis::Interface::Context*        pContext   = nullptr;
        SymbolAnalysis::Interface::DimensionTrait* pDimension = nullptr;
    };
    using TypeMap = std::multimap< int32_t, ContextDimensionSymbolSet >;

    struct SymbolCollector
    {
        SymbolAnalysis::Symbols::Symbol* findOrCreateSymbol( SymbolAnalysis::Database&           database,
                                                             SymbolAnalysis::Symbols::SymbolSet* pSymbolSet,
                                                             const std::string&                  strIdentifier,
                                                             SymbolMap&                          symbolMap ) const
        {
            using namespace SymbolAnalysis;

            Symbols::Symbol* pSymbol = nullptr;
            {
                SymbolMap::iterator iFind = symbolMap.find( strIdentifier );
                if ( iFind != symbolMap.end() )
                {
                    pSymbol = iFind->second;
                }
            }

            if ( !pSymbol )
            {
                // create the symbol in an un-labelled state i.e. with 0 for symbol id
                pSymbol = database.construct< Symbols::Symbol >( Symbols::Symbol::Args( strIdentifier, 0, {}, {} ) );
                symbolMap.insert( std::make_pair( strIdentifier, pSymbol ) );
            }

            // could already be there but insert anyway
            pSymbolSet->insert_symbols( strIdentifier, pSymbol );
            return pSymbol;
        }

        void operator()( SymbolAnalysis::Database& database, SymbolAnalysis::Symbols::SymbolSet* pSymbolSet,
                         SymbolMap& symbolMap, TypeMap& typeMap ) const
        {
            using namespace SymbolAnalysis;
            for ( Interface::Context* pContext : database.many< Interface::Context >( pSymbolSet->get_source_file() ) )
            {
                Symbols::Symbol* pSymbol
                    = findOrCreateSymbol( database, pSymbolSet, pContext->get_identifier(), symbolMap );
                pSymbolSet->insert_context_symbols( pContext, pSymbol );
                // create type id in un-labelled state
                typeMap.insert( std::make_pair( 0, ContextDimensionSymbolSet{ pSymbolSet, pContext, nullptr } ) );
            }
            for ( Interface::DimensionTrait* pDimension :
                  database.many< Interface::DimensionTrait >( pSymbolSet->get_source_file() ) )
            {
                Symbols::Symbol* pSymbol
                    = findOrCreateSymbol( database, pSymbolSet, pDimension->get_id()->get_str(), symbolMap );
                pSymbolSet->insert_dimension_symbols( pDimension, pSymbol );
                // create type id in un-labelled state
                typeMap.insert( std::make_pair( 0, ContextDimensionSymbolSet{ pSymbolSet, nullptr, pDimension } ) );
            }
        }

        void labelNewSymbols( SymbolMap& symbolMap ) const
        {
            using namespace SymbolAnalysis;

            std::set< std::int32_t > symbolLabels;
            // std::set< std::int32_t > paranoiCheck;
            for ( SymbolMap::iterator i = symbolMap.begin(); i != symbolMap.end(); ++i )
            {
                Symbols::Symbol* pSymbol = i->second;
                if ( pSymbol->get_id() != 0 )
                {
                    VERIFY_RTE( !symbolLabels.count( pSymbol->get_id() ) );
                    symbolLabels.insert( pSymbol->get_id() );
                    // paranoiCheck.insert( pSymbol->get_id() );
                }
            }
            std::set< std::int32_t >::iterator labelIter   = symbolLabels.begin();
            std::int32_t                       szNextLabel = 1;
            for ( SymbolMap::iterator i = symbolMap.begin(); i != symbolMap.end(); ++i )
            {
                Symbols::Symbol* pSymbol = i->second;
                if ( pSymbol->get_id() == 0 )
                {
                    while ( labelIter != symbolLabels.end() )
                    {
                        if ( *labelIter > szNextLabel )
                        {
                            break;
                        }
                        else
                        {
                            szNextLabel = *labelIter + 1;
                            ++labelIter;
                        }
                    }
                    pSymbol->set_id( szNextLabel );
                    // paranoiCheck.insert( szNextLabel );
                    ++szNextLabel;
                }
            }
            // VERIFY_RTE( paranoiCheck.size() == symbolMap.size() );
        }
        void labelNewTypes( const TypeMap& typeMap ) const
        {
            using namespace SymbolAnalysis;

            std::set< std::int32_t >                 existingTypeIDs;
            std::vector< ContextDimensionSymbolSet > unTyped;

            for ( TypeMap::const_iterator i = typeMap.begin(), iEnd = typeMap.end(); i != iEnd; ++i )
            {
                if ( i->first == 0 )
                {
                    unTyped.push_back( i->second );
                }
                else
                {
                    existingTypeIDs.insert( i->first );
                }
            }

            std::set< std::int32_t >::iterator labelIter  = existingTypeIDs.begin();
            std::int32_t                       szNextType = 1;

            for ( std::vector< ContextDimensionSymbolSet >::iterator i = unTyped.begin(); i != unTyped.end(); ++i )
            {
                while ( labelIter != existingTypeIDs.end() )
                {
                    if ( *labelIter > szNextType )
                    {
                        break;
                    }
                    else
                    {
                        szNextType = *labelIter + 1;
                        ++labelIter;
                    }
                }
                if ( i->pContext )
                    i->pSymbolSet->insert_context_type_ids( i->pContext, szNextType );
                else if ( i->pDimension )
                    i->pSymbolSet->insert_dimension_type_ids( i->pDimension, szNextType );
                else
                    THROW_RTE( "Invalid type for type id" );
                ++szNextType;
            }
        }

        void collate( std::map< mega::io::megaFilePath, SymbolAnalysis::Symbols::SymbolSet* >& symbolSetMap,
                      SymbolMap&                                                               symbolMap ) const
        {
            using namespace SymbolAnalysis;
            for ( auto& [ filePath, pSymbolSet ] : symbolSetMap )
            {
                for ( auto& [ pContext, pSymbol ] : pSymbolSet->get_context_symbols() )
                {
                    pSymbol->push_back_contexts( pContext );
                }
                for ( auto& [ pDimension, pSymbol ] : pSymbolSet->get_dimension_symbols() )
                {
                    pSymbol->push_back_dimensions( pDimension );
                }
            }
        }
    };

    virtual void run( task::Progress& taskProgress )
    {
        const mega::io::manifestFilePath    manifestFilePath = m_environment.project_manifest();
        const mega::io::CompilationFilePath symbolCompilationFile
            = m_environment.SymbolAnalysis_SymbolTable( manifestFilePath );
        taskProgress.start( "Task_SymbolAnalysis", manifestFilePath.path(), symbolCompilationFile.path() );

        task::DeterminantHash determinant( m_toolChainHash );
        for ( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            determinant ^= m_environment.getBuildHashCode( m_environment.InterfaceStage_Tree( sourceFilePath ) );
        }

        if ( m_environment.restore( symbolCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( symbolCompilationFile );
            taskProgress.cached();
            return;
        }

        InterfaceHashCodeGenerator hashCodeGenerator( m_environment, m_toolChainHash );

        // try loading previous one...
        if ( m_environment.exists( symbolCompilationFile ) )
        {
            // attempt to reuse previous symbol analysis
            namespace Old = SymbolAnalysisView;
            namespace New = SymbolAnalysis;

            New::Database newDatabase( m_environment, manifestFilePath );
            {
                Old::Database oldDatabase( m_environment, manifestFilePath );

                const Old::Symbols::SymbolTable* pOldAnalysis
                    = oldDatabase.one< Old::Symbols::SymbolTable >( manifestFilePath );
                VERIFY_RTE( pOldAnalysis );
                using OldSymbolsMap                = std::map< mega::io::megaFilePath, Old::Symbols::SymbolSet* >;
                const OldSymbolsMap oldSymbolsMaps = pOldAnalysis->get_symbol_sets();
                const PathSet       sourceFiles    = getSortedSourceFiles();

                // using SymbolMap = std::map< std::string, ::SymbolAnalysis::Symbols::Symbol* >;
                SymbolMap                                                    symbolMap;
                TypeMap                                                      typeMap;
                std::map< mega::io::megaFilePath, New::Symbols::SymbolSet* > symbolSetMap;

                {
                    struct Compare
                    {
                        const mega::io::Environment& env;
                        Compare( const mega::io::Environment& env )
                            : env( env )
                        {
                        }
                        bool operator()( OldSymbolsMap::const_iterator i, PathSet::const_iterator j ) const
                        {
                            return i->first < *j;
                        }
                        bool opposite( OldSymbolsMap::const_iterator i, PathSet::const_iterator j ) const
                        {
                            return *j < i->first;
                        }
                    } comparator( m_environment );

                    generics::matchGetUpdates(
                        oldSymbolsMaps.begin(), oldSymbolsMaps.end(), sourceFiles.begin(), sourceFiles.end(),
                        // const Compare& cmp
                        comparator,

                        // const Update& shouldUpdate
                        [ &env = m_environment, &hashCodeGenerator, &newDatabase, &taskProgress, &symbolMap,
                          &symbolSetMap,
                          &typeMap ]( OldSymbolsMap::const_iterator i, PathSet::const_iterator j ) -> bool
                        {
                            const task::DeterminantHash interfaceHash = hashCodeGenerator( *j );
                            if ( interfaceHash.get() == i->second->get_hash_code() )
                            {
                                // since the code is NOT modified - can re use
                                Old::Symbols::SymbolSet* pOldSymbolSet = i->second;
                                {
                                    // load the old symbol set maps - which only contain pointers to stuff from the same
                                    // compilation file - so they are safe to use

                                    New::Symbols::SymbolSet* pNewSymbolSet
                                        = newDatabase.construct< New::Symbols::SymbolSet >(
                                            New::Symbols::SymbolSet::Args( {}, pOldSymbolSet->get_source_file(),
                                                                           pOldSymbolSet->get_hash_code(), {}, {}, {},
                                                                           {} ) );
                                    {
                                        for ( const auto& [ strSymbol, pOldSymbol ] : pOldSymbolSet->get_symbols() )
                                        {
                                            New::Symbols::Symbol* pNewSymbol = nullptr;
                                            {
                                                SymbolMap::iterator iFind = symbolMap.find( strSymbol );
                                                if ( iFind == symbolMap.end() )
                                                {
                                                    pNewSymbol = newDatabase.construct< New::Symbols::Symbol >(
                                                        New::Symbols::Symbol::Args(
                                                            strSymbol, pOldSymbol->get_id(), {}, {} ) );
                                                    symbolMap.insert( std::make_pair( strSymbol, pNewSymbol ) );
                                                }
                                                else
                                                {
                                                    pNewSymbol = iFind->second;
                                                    // could have already encountered the symbol in modified file
                                                    // but then it could already exist in previous file
                                                    // so set the symbol id
                                                    if ( pNewSymbol->get_id() == 0 )
                                                        pNewSymbol->set_id( pOldSymbol->get_id() );
                                                    VERIFY_RTE( pNewSymbol->get_id() == pOldSymbol->get_id() );
                                                }
                                            }
                                            pNewSymbolSet->insert_symbols( strSymbol, pNewSymbol );
                                        }
                                    }
                                    {
                                        for ( const auto& [ pOldContext, typeID ] :
                                              pOldSymbolSet->get_context_type_ids() )
                                        {
                                            New::Interface::Context* pNewContext
                                                = newDatabase.convert< New::Interface::Context >( pOldContext );
                                            pNewSymbolSet->insert_context_type_ids( pNewContext, typeID );
                                            typeMap.insert( std::make_pair(
                                                typeID,
                                                ContextDimensionSymbolSet{ pNewSymbolSet, pNewContext, nullptr } ) );
                                        }
                                        for ( const auto& [ pOldDimension, typeID ] :
                                              pOldSymbolSet->get_dimension_type_ids() )
                                        {
                                            New::Interface::DimensionTrait* pNewDimension
                                                = newDatabase.convert< New::Interface::DimensionTrait >(
                                                    pOldDimension );
                                            pNewSymbolSet->insert_dimension_type_ids( pNewDimension, typeID );
                                            typeMap.insert( std::make_pair(
                                                typeID,
                                                ContextDimensionSymbolSet{ pNewSymbolSet, nullptr, pNewDimension } ) );
                                        }
                                    }
                                    for ( const auto& [ pOldContext, pOldSymbol ] :
                                          pOldSymbolSet->get_context_symbols() )
                                    {
                                        SymbolMap::iterator iFind = symbolMap.find( pOldSymbol->get_symbol() );
                                        VERIFY_RTE( iFind != symbolMap.end() );
                                        pNewSymbolSet->insert_context_symbols(
                                            newDatabase.convert< New::Interface::Context >( pOldContext ),
                                            iFind->second );
                                    }
                                    for ( const auto& [ pOldDimension, pOldSymbol ] :
                                          pOldSymbolSet->get_dimension_symbols() )
                                    {
                                        SymbolMap::iterator iFind = symbolMap.find( pOldSymbol->get_symbol() );
                                        VERIFY_RTE( iFind != symbolMap.end() );
                                        pNewSymbolSet->insert_dimension_symbols(
                                            newDatabase.convert< New::Interface::DimensionTrait >( pOldDimension ),
                                            iFind->second );
                                    }
                                    symbolSetMap.insert( std::make_pair( *j, pNewSymbolSet ) );
                                }

                                std::ostringstream os;
                                os << "\tPartially reusing symbols for: " << j->path().string();
                                taskProgress.msg( os.str() );
                                return false;
                            }
                            else
                            {
                                return true;
                            }
                        },

                        // const Removal& rem
                        []( OldSymbolsMap::const_iterator i )
                        {
                            // a source file has been removed - can ignor this since
                            // recreating the dependency analysis and only attempting to
                            // reuse the globs
                        },

                        // const Addition& add
                        [ &env = m_environment, &hashCodeGenerator, &newDatabase, &taskProgress, &symbolMap,
                          &symbolSetMap, &typeMap ]( PathSet::const_iterator j )
                        {
                            // a new source file is added so must analysis from the ground up
                            const mega::io::megaFilePath megaFilePath  = *j;
                            const task::DeterminantHash  interfaceHash = hashCodeGenerator( megaFilePath );

                            New::Symbols::SymbolSet* pSymbolSet
                                = newDatabase.construct< New::Symbols::SymbolSet >( New::Symbols::SymbolSet::Args(
                                    {}, megaFilePath, interfaceHash.get(), {}, {}, {}, {} ) );

                            SymbolCollector()( newDatabase, pSymbolSet, symbolMap, typeMap );
                            symbolSetMap.insert( std::make_pair( megaFilePath, pSymbolSet ) );

                            std::ostringstream os;
                            os << "\tAdded symbols for: " << megaFilePath.path().string();
                            taskProgress.msg( os.str() );
                        },

                        // const Updated& updatesNeeded
                        [ &env = m_environment, &hashCodeGenerator, &newDatabase, &taskProgress, &symbolMap,
                          &symbolSetMap, &typeMap ]( OldSymbolsMap::const_iterator i )
                        {
                            // Old::Symbols::SymbolSet* pOldSymbolSet = i->second;
                            //  a new source file is added so must analysis from the ground up
                            const mega::io::megaFilePath megaFilePath  = i->first;
                            const task::DeterminantHash  interfaceHash = hashCodeGenerator( megaFilePath );

                            New::Symbols::SymbolSet* pSymbolSet
                                = newDatabase.construct< New::Symbols::SymbolSet >( New::Symbols::SymbolSet::Args(
                                    {}, megaFilePath, interfaceHash.get(), {}, {}, {}, {} ) );

                            SymbolCollector()( newDatabase, pSymbolSet, symbolMap, typeMap );
                            symbolSetMap.insert( std::make_pair( megaFilePath, pSymbolSet ) );

                            std::ostringstream os;
                            os << "\tUpdated symbols for: " << megaFilePath.path().string();
                            taskProgress.msg( os.str() );
                        } );
                }
                SymbolCollector().labelNewSymbols( symbolMap );
                SymbolCollector().labelNewTypes( typeMap );
                SymbolCollector().collate( symbolSetMap, symbolMap );
                newDatabase.construct< New::Symbols::SymbolTable >(
                    New::Symbols::SymbolTable::Args( symbolSetMap, symbolMap ) );
            }

            const task::FileHash fileHashCode = newDatabase.save_SymbolTable_to_temp();
            m_environment.setBuildHashCode( symbolCompilationFile, fileHashCode );
            m_environment.temp_to_real( symbolCompilationFile );
            m_environment.stash( symbolCompilationFile, determinant );

            taskProgress.succeeded();
        }
        else
        {
            using namespace SymbolAnalysis;
            using namespace SymbolAnalysis::Symbols;

            Database database( m_environment, manifestFilePath );
            {
                SymbolMap                                               symbolMap;
                TypeMap                                                 typeMap;
                std::map< mega::io::megaFilePath, Symbols::SymbolSet* > symbolSetMap;
                {
                    for ( const mega::io::megaFilePath& sourceFilePath : getSortedSourceFiles() )
                    {
                        const task::DeterminantHash interfaceHash = hashCodeGenerator( sourceFilePath );
                        using OldSymbolsMap            = std::map< mega::io::megaFilePath, Symbols::SymbolSet* >;
                        Symbols::SymbolSet* pSymbolSet = database.construct< Symbols::SymbolSet >(
                            Symbols::SymbolSet::Args( {}, sourceFilePath, interfaceHash.get(), {}, {}, {}, {} ) );
                        SymbolCollector()( database, pSymbolSet, symbolMap, typeMap );
                        symbolSetMap.insert( std::make_pair( sourceFilePath, pSymbolSet ) );
                    }
                    SymbolCollector().labelNewSymbols( symbolMap );
                    SymbolCollector().labelNewTypes( typeMap );
                    SymbolCollector().collate( symbolSetMap, symbolMap );
                }
                database.construct< SymbolTable >( SymbolTable::Args( symbolSetMap, symbolMap ) );
            }

            const task::FileHash fileHashCode = database.save_SymbolTable_to_temp();
            m_environment.setBuildHashCode( symbolCompilationFile, fileHashCode );
            m_environment.temp_to_real( symbolCompilationFile );
            m_environment.stash( symbolCompilationFile, determinant );
            taskProgress.succeeded();
        }
    }
};

class Task_SymbolRollout : public BaseTask
{
public:
    Task_SymbolRollout( task::Task::RawPtrSet             dependencies,
                        const mega::io::BuildEnvironment& environment,
                        const task::DeterminantHash       toolChainHash,
                        const mega::io::megaFilePath&     sourceFilePath )
        : BaseTask( dependencies, environment, toolChainHash )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( task::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath symbolAnalysisFilePath
            = m_environment.SymbolAnalysis_SymbolTable( m_environment.project_manifest() );
        const mega::io::CompilationFilePath symbolRolloutFilePath
            = m_environment.SymbolRollout_PerSourceSymbols( m_sourceFilePath );
        taskProgress.start( "Task_SymbolRollout", symbolAnalysisFilePath.path(), symbolRolloutFilePath.path() );

        Task_SymbolAnalysis::InterfaceHashCodeGenerator hashGen( m_environment, m_toolChainHash );
        const task::DeterminantHash                     determinant = hashGen( m_sourceFilePath );

        if ( m_environment.restore( symbolRolloutFilePath, determinant ) )
        {
            m_environment.setBuildHashCode( symbolRolloutFilePath );
            taskProgress.cached();
            return;
        }

        using namespace SymbolRollout;

        Database database( m_environment, m_sourceFilePath );

        Symbols::SymbolSet* pSymbolSet = nullptr;
        {
            Symbols::SymbolTable* pSymbolTable
                = database.one< Symbols::SymbolTable >( m_environment.project_manifest() );
            std::map< mega::io::megaFilePath, Symbols::SymbolSet* > symbolSets = pSymbolTable->get_symbol_sets();
            std::map< mega::io::megaFilePath, Symbols::SymbolSet* >::const_iterator iFind
                = symbolSets.find( m_sourceFilePath );
            VERIFY_RTE( iFind != symbolSets.end() );
            pSymbolSet = iFind->second;
        }
        VERIFY_RTE( pSymbolSet->get_hash_code() == determinant.get() );

        for ( auto& [ pContext, pSymbol ] : pSymbolSet->get_context_symbols() )
        {
            database.construct< Interface::Context >( Interface::Context::Args( pContext, pSymbol->get_id() ) );
        }
        for ( auto& [ pDimension, pSymbol ] : pSymbolSet->get_dimension_symbols() )
        {
            database.construct< Interface::DimensionTrait >(
                Interface::DimensionTrait::Args( pDimension, pSymbol->get_id() ) );
        }
        for ( auto& [ pContext, typeID ] : pSymbolSet->get_context_type_ids() )
        {
            pContext->set_type_id( typeID );
        }
        for ( auto& [ pDimension, typeID ] : pSymbolSet->get_dimension_type_ids() )
        {
            pDimension->set_type_id( typeID );
        }

        const task::FileHash fileHashCode = database.save_PerSourceSymbols_to_temp();
        m_environment.setBuildHashCode( symbolRolloutFilePath, fileHashCode );
        m_environment.temp_to_real( symbolRolloutFilePath );
        m_environment.stash( symbolRolloutFilePath, determinant );
        taskProgress.succeeded();
    }
    const mega::io::megaFilePath& m_sourceFilePath;
};

class Task_ObjectInterfaceGeneration : public BaseTask
{
public:
    Task_ObjectInterfaceGeneration( task::Task::RawPtrSet             dependencies,
                                    const mega::io::BuildEnvironment& environment,
                                    const task::DeterminantHash       toolChainHash,
                                    const mega::io::megaFilePath&     sourceFilePath )
        : BaseTask( dependencies, environment, toolChainHash )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    class TemplateEngine
    {
        const mega::io::BuildEnvironment& m_environment;
        ::inja::Environment&              m_injaEnvironment;
        ::inja::Template                  m_contextTemplate;
        ::inja::Template                  m_namespaceTemplate;
        ::inja::Template                  m_interfaceTemplate;

    public:
        TemplateEngine( const mega::io::BuildEnvironment& buildEnvironment, ::inja::Environment& injaEnv )
            : m_environment( buildEnvironment )
            , m_injaEnvironment( injaEnv )
            , m_contextTemplate( m_injaEnvironment.parse_template( m_environment.ContextTemplate().native() ) )
            , m_namespaceTemplate( m_injaEnvironment.parse_template( m_environment.NamespaceTemplate().native() ) )
            , m_interfaceTemplate( m_injaEnvironment.parse_template( m_environment.InterfaceTemplate().native() ) )
        {
        }

        void renderContext( const nlohmann::json& data, std::ostream& os ) const
        {
            m_injaEnvironment.render_to( os, m_contextTemplate, data );
        }

        void renderNamespace( const nlohmann::json& data, std::ostream& os ) const
        {
            m_injaEnvironment.render_to( os, m_namespaceTemplate, data );
        }

        void renderInterface( const nlohmann::json& data, std::ostream& os ) const
        {
            m_injaEnvironment.render_to( os, m_interfaceTemplate, data );
        }
    };

    void recurse( TemplateEngine& templateEngine, InterfaceAnalysisStage::Interface::Context* pContext,
                  nlohmann::json& structs, const nlohmann::json& parentTypeNames, std::ostream& os )
    {
        using namespace InterfaceAnalysisStage;
        using namespace InterfaceAnalysisStage::Interface;

        nlohmann::json typenames = parentTypeNames;
        typenames.push_back( pContext->get_identifier() );

        std::ostringstream osNested;
        for ( Context* pNestedContext : pContext->get_children() )
        {
            recurse( templateEngine, pNestedContext, structs, typenames, osNested );
        }

        nlohmann::json contextData( { { "name", pContext->get_identifier() },
                                      { "typeid", pContext->get_type_id() },
                                      { "trait_structs", nlohmann::json::array() },
                                      { "nested", osNested.str() } } );

        if ( Namespace* pNamespace = dynamic_database_cast< Namespace >( pContext ) )
        {
            if ( pNamespace->get_is_global() )
            {
                templateEngine.renderNamespace( contextData, os );
            }
            else
            {
                templateEngine.renderContext( contextData, os );
            }
        }
        else if ( Abstract* pAbstract = dynamic_database_cast< Abstract >( pContext ) )
        {
            templateEngine.renderContext( contextData, os );
        }
        else if ( Action* pAction = dynamic_database_cast< Action >( pContext ) )
        {
            templateEngine.renderContext( contextData, os );
        }
        else if ( Event* pEvent = dynamic_database_cast< Event >( pContext ) )
        {
            templateEngine.renderContext( contextData, os );
        }
        else if ( Function* pFunction = dynamic_database_cast< Function >( pContext ) )
        {
            nlohmann::json trait_struct( { { "name", pContext->get_identifier() },
                                           { "typeid", pContext->get_type_id() },
                                           { "types", typenames },
                                           { "traits", nlohmann::json::array() } } );

            {
                std::ostringstream osTrait;
                osTrait << "using Type  = " << pFunction->get_return_type_trait()->get_str();
                trait_struct[ "traits" ].push_back( osTrait.str() );
            }
            {
                std::ostringstream osTrait;
                osTrait << "/// " << pFunction->get_arguments_trait()->get_str();
                trait_struct[ "traits" ].push_back( osTrait.str() );
            }

            contextData[ "trait_structs" ].push_back( trait_struct );

            templateEngine.renderContext( contextData, os );

            structs.push_back( trait_struct );
        }
        else if ( Object* pObject = dynamic_database_cast< Object >( pContext ) )
        {
            templateEngine.renderContext( contextData, os );
        }
        else if ( Link* pLink = dynamic_database_cast< Link >( pContext ) )
        {
            templateEngine.renderContext( contextData, os );
        }
        else
        {
            THROW_RTE( "Unknown context type" );
        }
    }

    virtual void run( task::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath interfaceTreeFile = m_environment.InterfaceStage_Tree( m_sourceFilePath );
        const mega::io::GeneratedHPPSourceFilePath interfaceHeader = m_environment.Interface( m_sourceFilePath );
        taskProgress.start( "Task_ObjectInterfaceGeneration", interfaceTreeFile.path(), interfaceHeader.path() );

        const task::DeterminantHash determinant( { m_toolChainHash, m_environment.getBuildHashCode( interfaceTreeFile ),
                                                   m_environment.ContextTemplate(), m_environment.NamespaceTemplate(),
                                                   m_environment.InterfaceTemplate() } );

        if ( m_environment.restore( interfaceHeader, determinant ) )
        {
            m_environment.setBuildHashCode( interfaceHeader );
            taskProgress.cached();
            return;
        }

        using namespace InterfaceAnalysisStage;
        using namespace InterfaceAnalysisStage::Interface;

        Database database( m_environment, m_sourceFilePath );

        ::inja::Environment injaEnvironment;
        {
            injaEnvironment.set_trim_blocks( true );
        }

        TemplateEngine templateEngine( m_environment, injaEnvironment );
        nlohmann::json structs   = nlohmann::json::array();
        nlohmann::json typenames = nlohmann::json::array();

        std::ostringstream os;
        {
            Interface::Root* pRoot = database.one< Interface::Root >( m_sourceFilePath );
            for ( Context* pContext : pRoot->get_children() )
            {
                recurse( templateEngine, pContext, structs, typenames, os );
            }
        }

        // clang-format
        std::string strInterface = os.str();
        mega::utilities::clang_format( strInterface, std::optional< boost::filesystem::path >() );

        // generate the interface header
        {
            nlohmann::json interfaceData(
                { { "interface", strInterface }, { "guard", determinant.get() }, { "structs", structs } } );

            std::unique_ptr< boost::filesystem::ofstream > pOStream = m_environment.write( interfaceHeader );
            templateEngine.renderInterface( interfaceData, *pOStream );
        }

        m_environment.setBuildHashCode( interfaceHeader );
        m_environment.stash( interfaceHeader, determinant );

        taskProgress.succeeded();
    }
    const mega::io::megaFilePath& m_sourceFilePath;
};

class Task_ObjectInterfaceAnalysis : public BaseTask
{
public:
    Task_ObjectInterfaceAnalysis( task::Task::RawPtrSet             dependencies,
                                  const mega::io::BuildEnvironment& environment,
                                  const task::DeterminantHash       toolChainHash,
                                  const mega::io::megaFilePath&     sourceFilePath )
        : BaseTask( dependencies, environment, toolChainHash )
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
        const mega::io::CompilationFilePath interfaceTreeFile = m_environment.InterfaceStage_Tree( m_sourceFilePath );
        const mega::io::CompilationFilePath interfaceAnalysisFile
            = m_environment.InterfaceAnalysisStage_Clang( m_sourceFilePath );
        taskProgress.start( "Task_ObjectInterfaceAnalysis", interfaceTreeFile.path(), interfaceAnalysisFile.path() );

        const task::DeterminantHash determinant(
            { m_toolChainHash, m_environment.getBuildHashCode( interfaceTreeFile ) } );

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
    boost::filesystem::path rootSourceDir, rootBuildDir, parserDll, megaCompiler, clangCompiler, templatesDir;
    std::string             projectName;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Compile Mega Project Interface" );
    {
        // clang-format off
                commandOptions.add_options()
                ( "root_src_dir",   po::value< boost::filesystem::path >( &rootSourceDir ),  "Root source directory" )
                ( "root_build_dir", po::value< boost::filesystem::path >( &rootBuildDir ),   "Root build directory" )
                ( "project",        po::value< std::string >( &projectName ),                "Mega Project Name" )
                ( "mega_compiler",  po::value< boost::filesystem::path >( &megaCompiler ),   "Mega Structure Compiler path" )
                ( "clang_compiler", po::value< boost::filesystem::path >( &clangCompiler ),  "Clang Compiler path" )
                ( "parser_dll",     po::value< boost::filesystem::path >( &parserDll ),      "Parser DLL Path" )
                ( "templates",      po::value< boost::filesystem::path >( &templatesDir ),   "Inja Templates directory" )
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
        mega::io::BuildEnvironment environment( rootSourceDir, rootBuildDir, templatesDir );

        VERIFY_RTE_MSG( boost::filesystem::exists( parserDll ), "Failed to locate parser DLL: " << parserDll );

        // load the manifest
        const mega::io::manifestFilePath projectManifestPath = environment.project_manifest();
        mega::io::Manifest               manifest( environment, projectManifestPath );

        const task::DeterminantHash toolChainHash(
            { task::FileHash( parserDll ), task::FileHash( megaCompiler ), task::FileHash( clangCompiler ) } );

        task::Task::PtrVector tasks;
        task::Task::RawPtrSet parserTasks;
        task::Task::RawPtrSet includeTasks, includePCHTasks;
        task::Task::RawPtrSet interfaceGenTasks;
        for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            Task_ParseAST* pTask = new Task_ParseAST(
                environment, toolChainHash, parserDll, sourceFilePath, manifest.getCompilationFileInfos() );
            parserTasks.insert( pTask );
            tasks.push_back( task::Task::Ptr( pTask ) );

            Task_Include* pTaskInclude = new Task_Include( pTask, environment, toolChainHash, sourceFilePath );
            includeTasks.insert( pTaskInclude );
            tasks.push_back( task::Task::Ptr( pTaskInclude ) );

            Task_IncludePCH* pTaskIncludePCH
                = new Task_IncludePCH( pTaskInclude, environment, toolChainHash, clangCompiler, sourceFilePath );
            includePCHTasks.insert( pTaskIncludePCH );
            tasks.push_back( task::Task::Ptr( pTaskIncludePCH ) );

            Task_ObjectInterfaceGen* pInterfaceGenTask
                = new Task_ObjectInterfaceGen( pTask, environment, toolChainHash, sourceFilePath );
            interfaceGenTasks.insert( pInterfaceGenTask );
            tasks.push_back( task::Task::Ptr( pInterfaceGenTask ) );
        }

        VERIFY_RTE_MSG( !tasks.empty(), "No input source code found" );

        Task_DependencyAnalysis* pDependencyAnalysisTask
            = new Task_DependencyAnalysis( interfaceGenTasks, environment, toolChainHash, manifest );
        tasks.push_back( task::Task::Ptr( pDependencyAnalysisTask ) );

        Task_SymbolAnalysis* pSymbolAnalysisTask
            = new Task_SymbolAnalysis( { pDependencyAnalysisTask }, environment, toolChainHash, manifest );
        tasks.push_back( task::Task::Ptr( pSymbolAnalysisTask ) );

        for ( const mega::io::megaFilePath& sourceFilePath : manifest.getMegaSourceFiles() )
        {
            Task_SymbolRollout* pSymbolRollout
                = new Task_SymbolRollout( { pSymbolAnalysisTask }, environment, toolChainHash, sourceFilePath );
            tasks.push_back( task::Task::Ptr( pSymbolRollout ) );

            Task_ObjectInterfaceGeneration* pInterfaceGen
                = new Task_ObjectInterfaceGeneration( { pSymbolRollout }, environment, toolChainHash, sourceFilePath );
            tasks.push_back( task::Task::Ptr( pInterfaceGen ) );

            Task_ObjectInterfaceAnalysis* pObjectInterfaceAnalysis
                = new Task_ObjectInterfaceAnalysis( { pInterfaceGen }, environment, toolChainHash, sourceFilePath );
            tasks.push_back( task::Task::Ptr( pObjectInterfaceAnalysis ) );
        }

        task::Schedule::Ptr pSchedule( new task::Schedule( tasks ) );
        task::run( pSchedule, std::cout );
    }
}
} // namespace interface
} // namespace driver
