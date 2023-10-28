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

#include "base_task.hpp"

#include "database/AliasAnalysisRollout.hxx"
#include "database/AutomataStage.hxx"
#include "database/ComponentListing.hxx"
#include "database/ComponentListingView.hxx"
#include "database/ConcreteStage.hxx"
#include "database/ConcreteTypeAnalysis.hxx"
#include "database/ConcreteTypeAnalysisView.hxx"
#include "database/ConcreteTypeRollout.hxx"
#include "database/DependencyAnalysis.hxx"
#include "database/DependencyAnalysisView.hxx"
#include "database/FinalStage.hxx"
#include "database/GlobalMemoryStageRollout.hxx"
#include "database/HyperGraphAnalysis.hxx"
#include "database/HyperGraphAnalysisRollout.hxx"
#include "database/HyperGraphAnalysisView.hxx"
#include "database/InheritanceAnalysis.hxx"
#include "database/InheritanceAnalysisRollout.hxx"
#include "database/InheritanceAnalysisView.hxx"
#include "database/InterfaceAnalysisStage.hxx"
#include "database/InterfaceStage.hxx"
#include "database/MemoryStage.hxx"
#include "database/MetaStage.hxx"
#include "database/OperationsLocs.hxx"
#include "database/OperationsStage.hxx"
#include "database/ParserStage.hxx"
#include "database/SymbolAnalysis.hxx"
#include "database/SymbolAnalysisView.hxx"
#include "database/SymbolRollout.hxx"
#include "database/ValueSpaceStage.hxx"

#include "compiler/clang_compilation.hpp"

#include "mega/values/compilation/operation_id.hpp"
#include "database/sources.hpp"

#include "mega/common_strings.hpp"
#include "mega/mangle/traits.hpp"

#include "common/file.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include <common/stash.hpp>

#include <vector>
#include <string>

namespace InheritanceAnalysisView
{
#include "compiler/interface_printer.hpp"
using namespace InheritanceAnalysisView::Interface;
#include "compiler/generator_interface.hpp"
} // namespace InheritanceAnalysisView

namespace FinalStage
{
#include "compiler/generator_implementation.hpp"
}

namespace mega::compiler
{

#define CPP_DUMB_STAGE( Stage, File )                                                                           \
    DO_STUFF_AND_REQUIRE_SEMI_COLON(                                                                            \
        const mega::io::CompilationFilePath compilationFile = m_environment.Stage##_##File( m_sourceFilePath ); \
                                                                                                                \
        const task::DeterminantHash determinant = { m_toolChain.toolChainHash };                                \
                                                                                                                \
        if( m_environment.restore( compilationFile, determinant ) ) {                                           \
            m_environment.setBuildHashCode( compilationFile );                                                  \
            cached( taskProgress );                                                                             \
            return;                                                                                             \
        }                                                                                                       \
                                                                                                                \
        using namespace Stage;                                                                                  \
                                                                                                                \
        Database database( m_environment, m_sourceFilePath );                                                   \
                                                                                                                \
        const task::FileHash fileHashCode = database.save_##File##_to_temp();                                   \
        m_environment.setBuildHashCode( compilationFile, fileHashCode );                                        \
        m_environment.temp_to_real( compilationFile );                                                          \
        m_environment.stash( compilationFile, determinant ); )

class Task_CPPStages : public BaseTask
{
    const mega::io::cppFilePath& m_sourceFilePath;

public:
    Task_CPPStages( const TaskArguments& taskArguments, const mega::io::cppFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        start( taskProgress, "Task_CPPStages", m_sourceFilePath.path(), m_sourceFilePath.path() );
        CPP_DUMB_STAGE( ParserStage, AST );
        CPP_DUMB_STAGE( ParserStage, Body );
        CPP_DUMB_STAGE( InterfaceStage, Tree );
        CPP_DUMB_STAGE( SymbolRollout, PerSourceSymbols );
        CPP_DUMB_STAGE( MetaStage, MetaAnalysis );
        CPP_DUMB_STAGE( InterfaceAnalysisStage, Clang );
        CPP_DUMB_STAGE( ConcreteStage, Concrete );
        CPP_DUMB_STAGE( InheritanceAnalysisRollout, PerSourceDerivations );
        CPP_DUMB_STAGE( HyperGraphAnalysisRollout, PerSourceModel );
        CPP_DUMB_STAGE( AliasAnalysisRollout, PerSourceModel );
        CPP_DUMB_STAGE( ConcreteTypeRollout, PerSourceConcreteTable );
        CPP_DUMB_STAGE( MemoryStage, MemoryLayout );
        CPP_DUMB_STAGE( GlobalMemoryStageRollout, GlobalMemoryRollout );
        CPP_DUMB_STAGE( AutomataStage, AutomataAnalysis );
        CPP_DUMB_STAGE( OperationsStage, Operations );
        CPP_DUMB_STAGE( OperationsLocs, Locations );
        CPP_DUMB_STAGE( ValueSpaceStage, ValueSpace );
        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_CPPStages( const TaskArguments& taskArguments, const mega::io::cppFilePath& sourceFilePath )
{
    return std::make_unique< Task_CPPStages >( taskArguments, sourceFilePath );
}

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
        using namespace InheritanceAnalysisView;

        Database database( m_environment, m_environment.project_manifest() );

        Components::Component* pComponent = nullptr;
        for( Components::Component* pIter :
             database.template many< Components::Component >( m_environment.project_manifest() ) )
        {
            if( pIter->get_name() == m_strComponentName )
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
        start( taskProgress, "Task_CPPInterfaceGeneration", m_strComponentName, interfaceHeader.path() );

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
        using namespace InheritanceAnalysisView::Interface;

        ::inja::Environment injaEnvironment;
        {
            injaEnvironment.set_trim_blocks( true );
        }

        InterfaceGen::TemplateEngine templateEngine( m_environment, injaEnvironment );
        nlohmann::json               structs   = nlohmann::json::array();
        nlohmann::json               typenames = nlohmann::json::array();

        Dependencies::Analysis* pDependencyAnalysis
            = database.one< Dependencies::Analysis >( m_environment.project_manifest() );

        // build the interface view for this translation unit
        InterfaceGen::InterfaceNode::Ptr pInterfaceRoot = std::make_shared< InterfaceGen::InterfaceNode >();
        {
            auto cppFileDependencies = pDependencyAnalysis->get_cpp_dependencies();

            for( const mega::io::cppFilePath& cppFile : pComponent->get_cpp_source_files() )
            {
                auto megaIter = cppFileDependencies.find( cppFile );
                VERIFY_RTE( megaIter != cppFileDependencies.end() );
                Dependencies::TransitiveDependencies* pTransitiveDep = megaIter->second;

                for( const mega::io::megaFilePath& megaFile : pTransitiveDep->get_mega_source_files() )
                {
                    // ensure meta analysis loaded
                    Interface::Root* pRoot = database.one< Interface::Root >( megaFile );
                    for( IContext* pContext : pRoot->get_children() )
                    {
                        InterfaceGen::buildInterfaceTree( pInterfaceRoot, pContext );
                    }
                }
            }
        }

        std::ostringstream os;
        {
            for( InterfaceGen::InterfaceNode::Ptr pInterfaceNode : pInterfaceRoot->children )
            {
                InterfaceGen::recurse( templateEngine, pInterfaceNode, structs, typenames, os );
            }
        }

        // clang-format
        std::string strInterface = os.str();

        // generate the interface header
        {
            std::ostringstream osGuard;
            {
                bool bFirst = true;
                for( auto filePart : pComponent->get_build_dir() )
                {
                    if( bFirst )
                        bFirst = false;
                    else
                        osGuard << "_";
                    std::string str = filePart.replace_extension( "" ).string();
                    std::replace_if(
                        str.begin(), str.end(), []( char ch ) { return !std::isalnum( ch ); }, '_' );
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
            for( auto& [ symbolName, pSymbol ] : pSymbolTable->get_symbol_names() )
            {
                nlohmann::json forwardDecl( { { "symbol", pSymbol->get_id().getSymbolID() }, { "name", symbolName } } );

                bool bIsGlobal = false;
                for( auto pContext : pSymbol->get_contexts() )
                {
                    if( db_cast< Interface::Root >( pContext->get_parent() ) )
                    {
                        bIsGlobal = true;
                    }
                }

                if( !bIsGlobal )
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
        using namespace InheritanceAnalysisView; // just need a stage that uses project_manifest

        Database database( m_environment, m_environment.project_manifest() );

        Components::Component* pComponent = nullptr;
        for( Components::Component* pIter :
             database.template many< Components::Component >( m_environment.project_manifest() ) )
        {
            if( pIter->get_name() == m_strComponentName )
            {
                pComponent = pIter;
                break;
            }
        }
        VERIFY_RTE( pComponent );

        const mega::io::GeneratedHPPSourceFilePath interfaceHeader
            = m_environment.Interface( pComponent->get_build_dir(), pComponent->get_name() );
        const mega::io::PrecompiledHeaderFile interfacePCHFilePath
            = m_environment.InterfacePCH( pComponent->get_build_dir(), pComponent->get_name() );

        start( taskProgress, "Task_CPPInterfaceAnalysis", m_strComponentName, interfacePCHFilePath.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( interfaceHeader ) } );

        const mega::Compilation compilationCMD
            = mega::Compilation::make_cpp_interfacePCH_compilation( m_environment, m_toolChain, pComponent );

        if( m_environment.restore( interfacePCHFilePath, determinant ) )
        {
            // if ( !run_cmd( taskProgress, compilationCMD.generatePCHVerificationCMD() ) )
            {
                m_environment.setBuildHashCode( interfacePCHFilePath );
                cached( taskProgress );
                return;
            }
        }

        if( run_cmd( taskProgress, compilationCMD.generateCompilationCMD() ) )
        {
            failed( taskProgress );
            return;
        }

        if( m_environment.exists( interfacePCHFilePath ) )
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

class Task_CPPPCH : public BaseTask
{
    const mega::io::cppFilePath& m_sourceFilePath;

public:
    Task_CPPPCH( const TaskArguments& taskArguments, const mega::io::cppFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::GeneratedHPPSourceFilePath tempHPPFile = m_environment.CPPTempHpp( m_sourceFilePath );
        const mega::io::CompilationFilePath        compilationFile
            = m_environment.OperationsStage_Operations( m_sourceFilePath );
        const mega::io::PrecompiledHeaderFile cppPCHFile = m_environment.CPPPCH( m_sourceFilePath );
        start( taskProgress, "Task_CPPPCH", m_sourceFilePath.path(), cppPCHFile.path() );

        using namespace OperationsStage;
        Database               database( m_environment, m_sourceFilePath );
        Components::Component* pComponent = getComponent< Components::Component >( database, m_sourceFilePath );

        const mega::Compilation compilationCMD
            = mega::Compilation::make_cpp_pch_compilation( m_environment, m_toolChain, pComponent, m_sourceFilePath );

        const task::DeterminantHash determinant( { m_toolChain.toolChainHash, compilationCMD.generateCompilationCMD(),
                                                   m_environment.getBuildHashCode( m_environment.IncludePCH(
                                                       pComponent->get_build_dir(), pComponent->get_name() ) ),
                                                   m_environment.getBuildHashCode( m_environment.InterfacePCH(
                                                       pComponent->get_build_dir(), pComponent->get_name() ) ),
                                                   m_environment.FilePath( m_sourceFilePath ) } );

        bool bRestoredHPP = m_environment.restore( tempHPPFile, determinant );
        if( bRestoredHPP )
        {
            // if ( !run_cmd( taskProgress, compilationCMD.generatePCHVerificationCMD() ) )
            {
                m_environment.setBuildHashCode( tempHPPFile );
                bRestoredHPP = true;
            }
        }

        if( m_environment.restore( cppPCHFile, determinant ) && m_environment.restore( compilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( cppPCHFile );
            m_environment.setBuildHashCode( compilationFile );
            cached( taskProgress );
            return;
        }

        if( !bRestoredHPP )
        {
            const auto tempHppFilePath = m_environment.FilePath( tempHPPFile );
            const auto srcCppFilePath  = m_environment.FilePath( m_sourceFilePath );
            if( boost::filesystem::exists( tempHppFilePath ) )
            {
                if( common::Hash( tempHppFilePath ) != common::Hash( srcCppFilePath ) )
                {
                    boost::filesystem::remove( tempHppFilePath );
                    boost::filesystem::copy_file(
                        srcCppFilePath, tempHppFilePath, boost::filesystem::copy_options::synchronize );
                }
            }
            else
            {
                boost::filesystem::copy_file(
                    srcCppFilePath, tempHppFilePath, boost::filesystem::copy_options::synchronize );
            }
            m_environment.setBuildHashCode( tempHPPFile );
            m_environment.stash( tempHPPFile, determinant );
        }

        if( run_cmd( taskProgress, compilationCMD.generateCompilationCMD() ) )
        {
            std::ostringstream os;
            os << "Error compiling C++ source file: " << m_sourceFilePath.path();
            throw std::runtime_error( os.str() );
        }

        if( m_environment.exists( compilationFile ) && m_environment.exists( cppPCHFile ) )
        {
            m_environment.setBuildHashCode( compilationFile );
            m_environment.stash( compilationFile, determinant );

            m_environment.setBuildHashCode( cppPCHFile );
            m_environment.stash( cppPCHFile, determinant );

            succeeded( taskProgress );
        }
        else
        {
            failed( taskProgress );
        }
    }
};

BaseTask::Ptr create_Task_CPPPCH( const TaskArguments& taskArguments, const mega::io::cppFilePath& sourceFilePath )
{
    return std::make_unique< Task_CPPPCH >( taskArguments, sourceFilePath );
}

class Task_CPPImplementation : public BaseTask
{
    const mega::io::cppFilePath& m_sourceFilePath;

public:
    Task_CPPImplementation( const TaskArguments& taskArguments, const mega::io::cppFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        using namespace FinalStage;
        using namespace FinalStage::Interface;

        const mega::io::GeneratedCPPSourceFilePath implementationFile
            = m_environment.CPPImplementation( m_sourceFilePath );

        start( taskProgress, "Task_CPPImplementation", m_sourceFilePath.path(), implementationFile.path() );

        task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.ImplementationTemplate(),
              m_environment.getBuildHashCode( m_environment.OperationsStage_Operations( m_sourceFilePath ) ) } );

        if( m_environment.restore( implementationFile, determinant ) )
        {
            m_environment.setBuildHashCode( implementationFile );
            cached( taskProgress );
            return;
        }

        Database database( m_environment, m_environment.project_manifest() );

        std::ostringstream os;
        {
            ImplementationGen implGen( m_environment, m_sourceFilePath.path().string() );
            implGen.generate( database.one< Operations::Invocations >( m_sourceFilePath ), os );
        }
        boost::filesystem::updateFileIfChanged( m_environment.FilePath( implementationFile ), os.str() );

        m_environment.setBuildHashCode( implementationFile );
        m_environment.stash( implementationFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_CPPImplementation( const TaskArguments&         taskArguments,
                                             const mega::io::cppFilePath& sourceFilePath )
{
    return std::make_unique< Task_CPPImplementation >( taskArguments, sourceFilePath );
}

class Task_CPPObj : public BaseTask
{
    const mega::io::cppFilePath& m_sourceFilePath;

public:
    Task_CPPObj( const TaskArguments& taskArguments, const mega::io::cppFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::ObjectFilePath cppObjectFile = m_environment.CPPObj( m_sourceFilePath );
        start( taskProgress, "Task_CPPObj", m_sourceFilePath.path(), cppObjectFile.path() );

        using namespace FinalStage;
        Database               database( m_environment, m_sourceFilePath );
        Components::Component* pComponent = getComponent< Components::Component >( database, m_sourceFilePath );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash,
              m_environment.getBuildHashCode(
                  m_environment.IncludePCH( pComponent->get_build_dir(), pComponent->get_name() ) ),
              m_environment.getBuildHashCode(
                  m_environment.InterfacePCH( pComponent->get_build_dir(), pComponent->get_name() ) ),
              m_environment.getBuildHashCode( m_environment.CPPPCH( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( m_environment.CPPImplementation( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( m_environment.OperationsStage_Operations( m_sourceFilePath ) ) } );

        if( m_environment.restore( cppObjectFile, determinant ) )
        {
            m_environment.setBuildHashCode( cppObjectFile );
            cached( taskProgress );
            return;
        }

        const mega::Compilation compilationCMD
            = mega::Compilation::make_cpp_obj_compilation( m_environment, m_toolChain, pComponent, m_sourceFilePath );

        if( run_cmd( taskProgress, compilationCMD.generateCompilationCMD() ) )
        {
            std::ostringstream os;
            os << "Error compiling C++ source file: " << m_sourceFilePath.path();
            msg( taskProgress, os.str() );
            failed( taskProgress );
            return;
        }

        if( m_environment.exists( cppObjectFile ) )
        {
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

BaseTask::Ptr create_Task_CPPObj( const TaskArguments& taskArguments, const mega::io::cppFilePath& sourceFilePath )
{
    return std::make_unique< Task_CPPObj >( taskArguments, sourceFilePath );
}

} // namespace mega::compiler
