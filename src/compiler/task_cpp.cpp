


#include "base_task.hpp"

#include "database/model/OperationsStage.hxx"
#include "database/model/FinalStage.hxx"

#include "database/types/clang_compilation.hpp"

#include "database/types/sources.hpp"

#include "utilities/clang_format.hpp"

#include "mega/common.hpp"
#include "mega/common_strings.hpp"

#include "common/file.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include <common/stash.hpp>

#include <vector>
#include <string>


namespace FinalStage
{
    using namespace FinalStage::Interface;
    #include "interface.hpp"
}

namespace mega
{
namespace compiler
{
    

class Task_CPPInterfaceGeneration : public BaseTask
{
    const std::string& m_strComponentName;

public:
    Task_CPPInterfaceGeneration( const TaskArguments& taskArguments, const std::string& strComponentName )
        : BaseTask( taskArguments )
        , m_strComponentName( strComponentName )
    {
    }


    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        using namespace FinalStage;

        Database database( m_environment, m_environment.project_manifest() );

        Components::Component* pComponent = nullptr;
        for ( Components::Component* pIter :
              database.template many< Components::Component >( m_environment.project_manifest() ) )
        {
            if ( pIter->get_name() == m_strComponentName )
            {
                pComponent = pIter;
                break;
            }
        }
        VERIFY_RTE( pComponent );

        const mega::io::CompilationFilePath symbolTableFile
            = m_environment.SymbolAnalysis_SymbolTable( m_environment.project_manifest() );

        const mega::io::GeneratedHPPSourceFilePath interfaceHeader
            = m_environment.Interface( pComponent->get_build_dir(), pComponent->get_name() );
        start( taskProgress, "Task_CPPInterfaceGeneration", pComponent->get_name(), interfaceHeader.path() );

        /*const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash,
              m_environment.getBuildHashCode( symbolTableFile ), m_environment.ContextTemplate(),
              m_environment.InterfaceTemplate() } );

        if ( m_environment.restore( interfaceHeader, determinant ) )
        {
            m_environment.setBuildHashCode( interfaceHeader );
            cached( taskProgress );
            return;
        }*/

        // using namespace OperationsStage;
        using namespace FinalStage::Interface;

        ::inja::Environment injaEnvironment;
        {
            injaEnvironment.set_trim_blocks( true );
        }


        InterfaceGen::TemplateEngine templateEngine( m_environment, injaEnvironment );
        nlohmann::json structs   = nlohmann::json::array();
        nlohmann::json typenames = nlohmann::json::array();

        Dependencies::Analysis* pDependencyAnalysis
            = database.one< Dependencies::Analysis >( m_environment.project_manifest() );

        // build the interface view for this translation unit
        InterfaceGen::InterfaceNode::Ptr pInterfaceRoot = std::make_shared< InterfaceGen::InterfaceNode >();
        {
            auto cppFileDependencies = pDependencyAnalysis->get_cpp_dependencies();

            for ( const mega::io::cppFilePath& cppFile : pComponent->get_cpp_source_files() )
            {
                auto megaIter = cppFileDependencies.find( cppFile );
                VERIFY_RTE( megaIter != cppFileDependencies.end() );
                Dependencies::TransitiveDependencies* pTransitiveDep = megaIter->second;

                for ( const mega::io::megaFilePath& megaFile : pTransitiveDep->get_mega_source_files() )
                {
                    Interface::Root* pRoot = database.one< Interface::Root >( megaFile );
                    for ( IContext* pContext : pRoot->get_children() )
                    {
                        InterfaceGen::buildInterfaceTree( pInterfaceRoot, pContext );
                    }
                }
            }
        }

        std::ostringstream os;
        {
            for ( InterfaceGen::InterfaceNode::Ptr pInterfaceNode : pInterfaceRoot->children )
            {
                InterfaceGen::recurse( templateEngine, pInterfaceNode, structs, typenames, os );
            }
        }

        // clang-format
        std::string strInterface = os.str();
        mega::utilities::clang_format( strInterface, std::optional< boost::filesystem::path >() );

        // generate the interface header
        {
            std::ostringstream osGuard;
            {
                bool bFirst = true;
                for ( auto filePart : pComponent->get_build_dir() )
                {
                    if ( bFirst )
                        bFirst = false;
                    else
                        osGuard << "_";
                    std::string str = filePart.replace_extension( "" ).string();
                    if( str != "/" )
                        osGuard << str;
                }
                osGuard << "_" << pComponent->get_name();
            }

            nlohmann::json interfaceData( { { "interface", strInterface },
                                            { "guard", osGuard.str() },
                                            { "structs", structs },
                                            { "forward_decls", nlohmann::json::array() } } );

            Symbols::SymbolTable* pSymbolTable
                = database.one< Symbols::SymbolTable >( m_environment.project_manifest() );
            for ( auto& [ symbolName, pSymbol ] : pSymbolTable->get_symbols() )
            {
                nlohmann::json forwardDecl( { { "symbol", pSymbol->get_id() }, { "name", symbolName } } );

                bool bIsGlobal = false;
                for ( auto pContext : pSymbol->get_contexts() )
                {
                    if ( dynamic_database_cast< Interface::Root >( pContext->get_parent() ) )
                    {
                        bIsGlobal = true;
                    }
                }

                if ( !bIsGlobal )
                {
                    interfaceData[ "forward_decls" ].push_back( forwardDecl );
                }
            }

            std::unique_ptr< boost::filesystem::ofstream > pOStream = m_environment.write( interfaceHeader );
            templateEngine.renderInterface( interfaceData, *pOStream );
        }

        m_environment.setBuildHashCode( interfaceHeader );
        // m_environment.stash( interfaceHeader, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_CPPInterfaceGeneration( const TaskArguments& taskArguments,
                                                  const std::string&   strComponentName )
{
    return std::make_unique< Task_CPPInterfaceGeneration >( taskArguments, strComponentName );
}

class Task_CPPInterfaceAnalysis : public BaseTask
{
    const std::string& m_strComponentName;

public:
    Task_CPPInterfaceAnalysis( const TaskArguments& taskArguments, const std::string& strComponentName )
        : BaseTask( taskArguments )
        , m_strComponentName( strComponentName )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        using namespace FinalStage;

        Database database( m_environment, m_environment.project_manifest() );

        Components::Component* pComponent = nullptr;
        for ( Components::Component* pIter :
              database.template many< Components::Component >( m_environment.project_manifest() ) )
        {
            if ( pIter->get_name() == m_strComponentName )
            {
                pComponent = pIter;
                break;
            }
        }
        VERIFY_RTE( pComponent );

        //const mega::io::CompilationFilePath interfaceTreeFile = m_environment.InterfaceStage_Tree( m_sourceFilePath );
        const mega::io::GeneratedHPPSourceFilePath interfaceHeader = m_environment.Interface( pComponent->get_build_dir(), pComponent->get_name() );
        const mega::io::PrecompiledHeaderFile interfacePCHFilePath = m_environment.InterfacePCH( pComponent->get_build_dir(), pComponent->get_name() );
        //const mega::io::CompilationFilePath   interfaceAnalysisFile
        //    = m_environment.InterfaceAnalysisStage_Clang( m_sourceFilePath );

        start( taskProgress, "Task_CPPInterfaceAnalysis", interfaceHeader.path(), interfacePCHFilePath.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( interfaceHeader ) } );

        if ( m_environment.restore( interfacePCHFilePath, determinant ) )
        {
            m_environment.setBuildHashCode( interfacePCHFilePath );
            cached( taskProgress );
            return;
        }

        const std::string strCmd = mega::Compilation::make_interfacePCH_compilation(
            m_environment, m_toolChain, pComponent )();

        msg( taskProgress, strCmd );

        const int iResult = boost::process::system( strCmd );
        if ( iResult )
        {
            failed( taskProgress );
            return;
        }

        if ( m_environment.exists( interfacePCHFilePath ) )
        {
            m_environment.setBuildHashCode( interfacePCHFilePath );
            m_environment.stash( interfacePCHFilePath, determinant );

            succeeded( taskProgress );
        }
        else
        {
            failed( taskProgress );
        }
    }
};

BaseTask::Ptr create_Task_CPPInterfaceAnalysis( const TaskArguments& taskArguments,
                                                const std::string&   strComponentName )
{
    return std::make_unique< Task_CPPInterfaceAnalysis >( taskArguments, strComponentName );
}

class Task_CPP : public BaseTask
{
    const mega::io::cppFilePath& m_sourceFilePath;

public:
    Task_CPP( const TaskArguments& taskArguments, const mega::io::cppFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath compilationFile
            = m_environment.OperationsStage_Operations( m_sourceFilePath );
        const mega::io::ObjectFilePath cppObjectFile = m_environment.Obj( m_sourceFilePath );
        start( taskProgress, "Task_CPP", m_sourceFilePath.path(), compilationFile.path() );

        using namespace OperationsStage;
        Database               database( m_environment, m_sourceFilePath );
        Components::Component* pComponent = getComponent< Components::Component >( database, m_sourceFilePath );

        const task::DeterminantHash determinant( { m_toolChain.toolChainHash,
                                                   m_environment.getBuildHashCode( m_environment.IncludePCH(
                                                       pComponent->get_build_dir(), pComponent->get_name() ) ),
                                                   m_environment.getBuildHashCode( m_environment.InterfacePCH(
                                                       pComponent->get_build_dir(), pComponent->get_name() ) ),
                                                   m_environment.FilePath( m_sourceFilePath ) } );

        if ( m_environment.restore( cppObjectFile, determinant )
             && m_environment.restore( compilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( cppObjectFile );
            m_environment.setBuildHashCode( compilationFile );
            cached( taskProgress );
            return;
        }

        const std::string strCmd
            = mega::Compilation::make_cpp_compilation( m_environment, m_toolChain, pComponent, m_sourceFilePath )();

        msg( taskProgress, strCmd );

        const int iResult = boost::process::system( strCmd );
        if ( iResult )
        {
            std::ostringstream os;
            os << "Error compiling C++ source file: " << m_sourceFilePath.path();
            throw std::runtime_error( os.str() );
        }

        if ( m_environment.exists( compilationFile ) && m_environment.exists( cppObjectFile ) )
        {
            m_environment.setBuildHashCode( compilationFile );
            m_environment.stash( compilationFile, determinant );

            m_environment.setBuildHashCode( cppObjectFile );
            m_environment.stash( cppObjectFile, determinant );

            succeeded( taskProgress );
        }
        else
        {
            failed( taskProgress );
        }
    }
};

BaseTask::Ptr create_Task_CPP( const TaskArguments& taskArguments, const mega::io::cppFilePath& sourceFilePath )
{
    return std::make_unique< Task_CPP >( taskArguments, sourceFilePath );
}

} // namespace compiler
} // namespace mega
