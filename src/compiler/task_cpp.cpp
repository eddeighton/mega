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

#include "database/model/OperationsStage.hxx"
#include "database/model/FinalStage.hxx"

#include "database/types/clang_compilation.hpp"

#include "database/types/operation.hpp"
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
} // namespace FinalStage

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
        using namespace FinalStage::Interface;

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
                    if ( str != "/" )
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

        const mega::io::GeneratedHPPSourceFilePath interfaceHeader
            = m_environment.Interface( pComponent->get_build_dir(), pComponent->get_name() );
        const mega::io::PrecompiledHeaderFile interfacePCHFilePath
            = m_environment.InterfacePCH( pComponent->get_build_dir(), pComponent->get_name() );

        start( taskProgress, "Task_CPPInterfaceAnalysis", m_strComponentName, interfacePCHFilePath.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( interfaceHeader ) } );

        const mega::Compilation compilationCMD
            = mega::Compilation::make_cpp_interfacePCH_compilation( m_environment, m_toolChain, pComponent );

        if ( m_environment.restore( interfacePCHFilePath, determinant ) )
        {
            //if ( !run_cmd( taskProgress, compilationCMD.generatePCHVerificationCMD() ) )
            {
                m_environment.setBuildHashCode( interfacePCHFilePath );
                cached( taskProgress );
                return;
            }
        }

        if ( run_cmd( taskProgress, compilationCMD.generateCompilationCMD() ) )
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

        const task::DeterminantHash determinant( { m_toolChain.toolChainHash,
                                                   m_environment.getBuildHashCode( m_environment.IncludePCH(
                                                       pComponent->get_build_dir(), pComponent->get_name() ) ),
                                                   m_environment.getBuildHashCode( m_environment.InterfacePCH(
                                                       pComponent->get_build_dir(), pComponent->get_name() ) ),
                                                   m_environment.FilePath( m_sourceFilePath ) } );

        const mega::Compilation compilationCMD
            = mega::Compilation::make_cpp_pch_compilation( m_environment, m_toolChain, pComponent, m_sourceFilePath );

        bool bRestoredHPP = m_environment.restore( tempHPPFile, determinant );
        if ( bRestoredHPP )
        {
            //if ( !run_cmd( taskProgress, compilationCMD.generatePCHVerificationCMD() ) )
            {
                m_environment.setBuildHashCode( tempHPPFile );
                bRestoredHPP = true;
            }
        }

        if ( m_environment.restore( cppPCHFile, determinant ) && m_environment.restore( compilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( cppPCHFile );
            m_environment.setBuildHashCode( compilationFile );
            cached( taskProgress );
            return;
        }

        if ( !bRestoredHPP )
        {
            const auto tempHppFilePath = m_environment.FilePath( tempHPPFile );
            const auto srcCppFilePath  = m_environment.FilePath( m_sourceFilePath );
            if ( boost::filesystem::exists( tempHppFilePath ) )
            {
                if ( common::Hash( tempHppFilePath ) != common::Hash( srcCppFilePath ) )
                {
                    boost::filesystem::remove( tempHppFilePath );
                    boost::filesystem::copy( srcCppFilePath, tempHppFilePath );
                }
            }
            else
            {
                boost::filesystem::copy( srcCppFilePath, tempHppFilePath );
            }
            m_environment.setBuildHashCode( tempHPPFile );
            m_environment.stash( tempHPPFile, determinant );
        }

        if ( run_cmd( taskProgress, compilationCMD.generateCompilationCMD() ) )
        {
            std::ostringstream os;
            os << "Error compiling C++ source file: " << m_sourceFilePath.path();
            throw std::runtime_error( os.str() );
        }

        if ( m_environment.exists( compilationFile ) && m_environment.exists( cppPCHFile ) )
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

    class TemplateEngine
    {
        const mega::io::StashEnvironment& m_environment;
        ::inja::Environment&              m_injaEnvironment;
        ::inja::Template                  m_implTemplate;

    public:
        TemplateEngine( const mega::io::StashEnvironment& buildEnvironment, ::inja::Environment& injaEnv )
            : m_environment( buildEnvironment )
            , m_injaEnvironment( injaEnv )
            , m_implTemplate( m_injaEnvironment.parse_template( m_environment.ImplementationTemplate().native() ) )
        {
        }

        void renderImpl( const nlohmann::json& data, std::ostream& os ) const
        {
            m_injaEnvironment.render_to( os, m_implTemplate, data );
        }
    };

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

        if ( m_environment.restore( implementationFile, determinant ) )
        {
            m_environment.setBuildHashCode( implementationFile );
            cached( taskProgress );
            return;
        }

        Database database( m_environment, m_environment.project_manifest() );

        {
            ::inja::Environment injaEnvironment;
            {
                injaEnvironment.set_trim_blocks( true );
            }

            TemplateEngine templateEngine( m_environment, injaEnvironment );

            nlohmann::json implData( { { "unitname", m_sourceFilePath.path().string() },
                                       { "invocations", nlohmann::json::array() },
                                       { "interfaces", nlohmann::json::array() } } );

            Operations::Invocations* pInvocations = database.one< Operations::Invocations >( m_sourceFilePath );

            std::vector< Interface::IContext* > contexts;
            {
                std::set< Interface::IContext* > uniqueContexts;
                for ( const auto& [ id, pInvocation ] : pInvocations->get_invocations() )
                {
                    for ( auto pElementVector : pInvocation->get_context()->get_vectors() )
                    {
                        for ( auto pElement : pElementVector->get_elements() )
                        {
                            if ( pElement->get_interface()->get_context().has_value() )
                            {
                                Interface::IContext* pContext = pElement->get_interface()->get_context().value();
                                if ( uniqueContexts.count( pContext ) == 0 )
                                {
                                    uniqueContexts.insert( pContext );
                                    contexts.push_back( pContext );
                                }
                            }
                        }
                    }
                }
            }

            {
                std::vector< std::string > typeNameStack;
                for ( Interface::IContext* pContext : contexts )
                {
                    Interface::IContext*       pIter = pContext;
                    std::vector< std::string > typeNamePath;
                    while ( pIter )
                    {
                        typeNamePath.push_back( pIter->get_identifier() );
                        pIter = dynamic_database_cast< Interface::IContext >( pIter->get_parent() );
                    }
                    std::reverse( typeNamePath.begin(), typeNamePath.end() );
                    std::ostringstream os;
                    common::delimit( typeNamePath.begin(), typeNamePath.end(), "::", os );
                    implData[ "interfaces" ].push_back( os.str() );
                }

                for ( auto& [ id, pInvocation ] : pInvocations->get_invocations() )
                {
                    std::ostringstream osTypePathIDs;
                    common::delimit( id.m_type_path.begin(), id.m_type_path.end(), ",", osTypePathIDs );

                    nlohmann::json invocation(
                        { { "return_type", pInvocation->get_return_type_str() },
                          { "runtime_return_type", pInvocation->get_runtime_return_type_str() },
                          { "context", pInvocation->get_context_str() },
                          { "type_path", pInvocation->get_type_path_str() },
                          { "operation", mega::getOperationString( pInvocation->get_operation() ) },
                          { "explicit_operation",
                            mega::getExplicitOperationString( pInvocation->get_explicit_operation() ) },
                          { "type_path_type_id_list", osTypePathIDs.str() },
                          { "type_path_size", id.m_type_path.size() },
                          { "impl", "" } } );

                    implData[ "invocations" ].push_back( invocation );
                }
            }

            std::ostringstream os;
            templateEngine.renderImpl( implData, os );

            boost::filesystem::updateFileIfChanged( m_environment.FilePath( implementationFile ), os.str() );
        }

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

        using namespace OperationsStage;
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

        if ( m_environment.restore( cppObjectFile, determinant ) )
        {
            m_environment.setBuildHashCode( cppObjectFile );
            cached( taskProgress );
            return;
        }

        const mega::Compilation compilationCMD
            = mega::Compilation::make_cpp_obj_compilation( m_environment, m_toolChain, pComponent, m_sourceFilePath );

        if ( run_cmd( taskProgress, compilationCMD.generateCompilationCMD() ) )
        {
            std::ostringstream os;
            os << "Error compiling C++ source file: " << m_sourceFilePath.path();
            msg( taskProgress, os.str() );
            failed( taskProgress );
            return;
        }

        if ( m_environment.exists( cppObjectFile ) )
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
} // namespace compiler
} // namespace mega
