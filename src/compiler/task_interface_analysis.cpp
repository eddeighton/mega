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

#include "database/InterfaceAnalysisStage.hxx"

#include "compiler/clang_compilation.hpp"

#include "mega/common_strings.hpp"
#include "mega/values/runtime/reference.hpp"
#include "mega/mangle/traits.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include <memory>

namespace InterfaceAnalysisStage
{
#include "compiler/interface_printer.hpp"
using namespace InterfaceAnalysisStage::Interface;
#include "compiler/generator_interface.hpp"
} // namespace InterfaceAnalysisStage

namespace mega::compiler
{

class Task_InterfaceGeneration : public BaseTask
{
public:
    Task_InterfaceGeneration( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath interfaceTreeFile = m_environment.InterfaceStage_Tree( m_sourceFilePath );
        const mega::io::CompilationFilePath symbolTableFile
            = m_environment.SymbolAnalysis_SymbolTable( m_environment.project_manifest() );
        const mega::io::GeneratedHPPSourceFilePath interfaceHeader = m_environment.Interface( m_sourceFilePath );
        start( taskProgress, "Task_InterfaceGeneration", m_sourceFilePath.path(), interfaceHeader.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash,
              m_environment.getBuildHashCode( m_environment.ParserStage_AST( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( interfaceTreeFile ), m_environment.getBuildHashCode( symbolTableFile ),
              m_environment.ContextTemplate(), m_environment.InterfaceTemplate() } );

        if( m_environment.restore( interfaceHeader, determinant ) )
        {
            m_environment.setBuildHashCode( interfaceHeader );
            cached( taskProgress );
            return;
        }

        std::ostringstream osInterface;
        {
            using namespace InterfaceAnalysisStage;
            using namespace InterfaceAnalysisStage::Interface;

            Database database( m_environment, m_sourceFilePath );

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
                auto megaFileDependencies = pDependencyAnalysis->get_mega_dependencies();
                auto megaIter             = megaFileDependencies.find( m_sourceFilePath );
                VERIFY_RTE( megaIter != megaFileDependencies.end() );
                Dependencies::TransitiveDependencies* pTransitiveDep = megaIter->second;

                for( const mega::io::megaFilePath& megaFile : pTransitiveDep->get_mega_source_files() )
                {
                    Interface::Root* pRoot = database.one< Interface::Root >( megaFile );
                    for( IContext* pContext : pRoot->get_children() )
                    {
                        InterfaceGen::buildInterfaceTree( pInterfaceRoot, pContext );
                    }
                }

                Interface::Root* pRoot = database.one< Interface::Root >( m_sourceFilePath );
                for( IContext* pContext : pRoot->get_children() )
                {
                    InterfaceGen::buildInterfaceTree( pInterfaceRoot, pContext );
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
                    for( auto filePart : m_sourceFilePath.path() )
                    {
                        if( bFirst )
                            bFirst = false;
                        else
                            osGuard << "_";
                        osGuard << filePart.replace_extension( "" ).string();
                    }
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

                templateEngine.renderInterface( interfaceData, osInterface );
            }
        }

        if( boost::filesystem::updateFileIfChanged( m_environment.FilePath( interfaceHeader ), osInterface.str() ) )
        {
            m_environment.setBuildHashCode( interfaceHeader );
            m_environment.stash( interfaceHeader, determinant );
            succeeded( taskProgress );
        }
        else
        {
            m_environment.setBuildHashCode( interfaceHeader );
            cached( taskProgress );
        }
    }
    const mega::io::megaFilePath& m_sourceFilePath;
};

BaseTask::Ptr create_Task_InterfaceGeneration( const TaskArguments&          taskArguments,
                                               const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_InterfaceGeneration >( taskArguments, sourceFilePath );
}

class Task_InterfaceAnalysis : public BaseTask
{
public:
    Task_InterfaceAnalysis( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath interfaceTreeFile = m_environment.InterfaceStage_Tree( m_sourceFilePath );
        const mega::io::GeneratedHPPSourceFilePath interfaceHeader = m_environment.Interface( m_sourceFilePath );
        const mega::io::PrecompiledHeaderFile interfacePCHFilePath = m_environment.InterfacePCH( m_sourceFilePath );
        const mega::io::CompilationFilePath   interfaceAnalysisFile
            = m_environment.InterfaceAnalysisStage_Clang( m_sourceFilePath );

        start( taskProgress, "Task_InterfaceAnalysis", m_sourceFilePath.path(), interfacePCHFilePath.path() );

        using namespace InterfaceAnalysisStage;
        using namespace InterfaceAnalysisStage::Interface;
        Database               database( m_environment, m_sourceFilePath );
        Components::Component* pComponent = getComponent< Components::Component >( database, m_sourceFilePath );

        const mega::Compilation compilationCMD = mega::Compilation::make_interfacePCH_compilation(
            m_environment, m_toolChain, pComponent, m_sourceFilePath );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( interfaceHeader ),
              m_environment.getBuildHashCode( m_environment.IncludePCH( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( m_environment.ParserStage_AST( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( interfaceTreeFile ), compilationCMD.generateCompilationCMD().str() } );

        if( m_environment.restore( interfacePCHFilePath, determinant )
            && m_environment.restore( interfaceAnalysisFile, determinant ) )
        {
            //if ( EXIT_SUCCESS == run_cmd( taskProgress, compilationCMD.generatePCHVerificationCMD(), false ) )
            {
                m_environment.setBuildHashCode( interfacePCHFilePath );
                m_environment.setBuildHashCode( interfaceAnalysisFile );
                cached( taskProgress );
                return;
            }
            /*else
            {
                std::ostringstream os;
                os << "Verification of cached pch: " << interfacePCHFilePath.path().string() << " failed";
                msg( taskProgress, os.str() );
            }*/
        }
        
        if ( EXIT_SUCCESS == run_cmd( taskProgress, compilationCMD.generateCompilationCMD() ) )
        {
            m_environment.setBuildHashCode( interfacePCHFilePath );
            m_environment.stash( interfacePCHFilePath, determinant );

            m_environment.setBuildHashCode( interfaceAnalysisFile );
            m_environment.stash( interfaceAnalysisFile, determinant );

            succeeded( taskProgress );
            return;
        }
        else
        {
            failed( taskProgress );
        }

    }

    const mega::io::megaFilePath& m_sourceFilePath;
};

BaseTask::Ptr create_Task_InterfaceAnalysis( const TaskArguments&          taskArguments,
                                             const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_InterfaceAnalysis >( taskArguments, sourceFilePath );
}

} // namespace mega::compiler
