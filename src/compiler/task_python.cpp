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

#include "compiler/generator_utility.hpp"
#include "compiler/clang_compilation.hpp"

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

#include "database/component_type.hpp"
#include "database/sources.hpp"

#include "common/file.hpp"
#include "common/stash.hpp"
#include "common/requireSemicolon.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include <boost/filesystem/operations.hpp>

namespace mega::compiler
{

class Task_PythonWrapper : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

    class TemplateEngine
    {
        const mega::io::StashEnvironment& m_environment;
        ::inja::Environment&              m_injaEnvironment;
        ::inja::Template                  m_pythonWrapperTemplate;

    public:
        TemplateEngine( const mega::io::StashEnvironment& buildEnvironment, ::inja::Environment& injaEnv )
            : m_environment( buildEnvironment )
            , m_injaEnvironment( injaEnv )
            , m_pythonWrapperTemplate(
                  m_injaEnvironment.parse_template( m_environment.PythonWrapperTemplate().string() ) )
        {
        }

        void renderPythonWrappers( const nlohmann::json& data, std::ostream& os ) const
        {
            m_injaEnvironment.render_to( os, m_pythonWrapperTemplate, data );
        }
    };

public:
    Task_PythonWrapper( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    void recurse( ConcreteStage::Interface::IContext* pContext,
                  nlohmann::json&                     data,
                  CleverUtility::IDList&              namespaces,
                  CleverUtility::IDList&              types )
    {
        using namespace ConcreteStage;
        using namespace ConcreteStage::Interface;

        if( auto pNamespace = db_cast< Namespace >( pContext ) )
        {
            /*if ( pNamespace->get_is_global() )
            {
                CleverUtility c( namespaces, pNamespace->get_identifier() );
                for ( IContext* pNestedContext : pNamespace->get_children() )
                {
                    recurse( pNestedContext, data, namespaces, types );
                }
            }
            else*/
            {
                CleverUtility c( types, pNamespace->get_identifier() );
                for( IContext* pNestedContext : pNamespace->get_children() )
                {
                    recurse( pNestedContext, data, namespaces, types );
                }
            }
        }
        else if( auto pAbstract = db_cast< Abstract >( pContext ) )
        {
            CleverUtility c( types, pAbstract->get_identifier() );
            for( IContext* pNestedContext : pAbstract->get_children() )
            {
                recurse( pNestedContext, data, namespaces, types );
            }
        }
        else if( auto pState = db_cast< State >( pContext ) )
        {
            CleverUtility c( types, pState->get_identifier() );
            for( IContext* pNestedContext : pState->get_children() )
            {
                recurse( pNestedContext, data, namespaces, types );
            }
        }
        else if( auto pEvent = db_cast< Event >( pContext ) )
        {
        }
        else if( auto pInterupt = db_cast< Interupt >( pContext ) )
        {
        }
        else if( auto pDecider = db_cast< Decider >( pContext ) )
        {
        }
        else if( auto pFunction = db_cast< Function >( pContext ) )
        {
            CleverUtility c( types, pFunction->get_identifier() );

            common::Hash hash{ pFunction->get_return_type_trait()->get_str() };
            for( const std::string& strParamType : pFunction->get_arguments_trait()->get_canonical_types() )
            {
                hash ^= strParamType;
            }

            std::ostringstream osArgs;
            osArgs << pFunction->get_arguments_trait()->get_arguments();

            nlohmann::json operation( { { "return_type", pFunction->get_return_type_trait()->get_str() },
                                        { "hash", hash.toHexString() },
                                        { "typeID", pFunction->get_interface_id().getSymbolID() },
                                        { "has_namespaces", !namespaces.empty() },
                                        { "namespaces", namespaces },
                                        { "types", types },
                                        { "params_string", osArgs.str() },
                                        { "params", nlohmann::json::array() } } );
            {
                int iParamCounter = 1;
                for( const std::string& strParamType : pFunction->get_arguments_trait()->get_canonical_types() )
                {
                    std::ostringstream osParamName;
                    osParamName << "p_" << iParamCounter++;
                    nlohmann::json param( { { "type", strParamType }, { "name", osParamName.str() } } );
                    operation[ "params" ].push_back( param );
                }
            }

            data[ "operations" ].push_back( operation );
        }
        else if( auto pObject = db_cast< Object >( pContext ) )
        {
            CleverUtility c( types, pObject->get_identifier() );
            for( IContext* pNestedContext : pObject->get_children() )
            {
                recurse( pNestedContext, data, namespaces, types );
            }
        }
        else
        {
            THROW_RTE( "Unknown context type" );
        }
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath concreteFile = m_environment.ConcreteStage_Concrete( m_sourceFilePath );
        const mega::io::GeneratedHPPSourceFilePath includeFilePath = m_environment.Include( m_sourceFilePath );
        const mega::io::GeneratedCPPSourceFilePath operationsFile  = m_environment.PythonWrapper( m_sourceFilePath );
        start( taskProgress, "Task_PythonWrapper", m_sourceFilePath.path(), operationsFile.path() );

        task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.PythonWrapperTemplate(),
              m_environment.getBuildHashCode( includeFilePath ),
              m_environment.getBuildHashCode( m_environment.ParserStage_Body( m_sourceFilePath ) ),
              m_environment.getBuildHashCode(
                  m_environment.ConcreteTypeRollout_PerSourceConcreteTable( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( concreteFile ) } );

        if( m_environment.restore( operationsFile, determinant ) )
        {
            m_environment.setBuildHashCode( operationsFile );
            cached( taskProgress );
            return;
        }

        bool bModified = false;
        {
            using namespace ConcreteStage;
            using namespace ConcreteStage::Interface;

            Database database( m_environment, m_sourceFilePath );

            auto pComponent = getComponent< Components::Component >( database, m_sourceFilePath );

            const bool bFirst = m_sourceFilePath == pComponent->get_mega_source_files().front();

            std::ostringstream os;
            {
                ::inja::Environment injaEnvironment;
                {
                    injaEnvironment.set_trim_blocks( true );
                }
                TemplateEngine templateEngine( m_environment, injaEnvironment );

                nlohmann::json data( { { "component_name", pComponent->get_name() },
                                       { "define_setPythonCaster", bFirst },
                                       { "operations", nlohmann::json::array() },
                                       { "include", m_environment.FilePath( includeFilePath ).string() } } );

                Interface::Root*      pRoot = database.one< Interface::Root >( m_sourceFilePath );
                CleverUtility::IDList namespaces, types;
                for( IContext* pContext : pRoot->get_children() )
                {
                    recurse( pContext, data, namespaces, types );
                }

                templateEngine.renderPythonWrappers( data, os );
            }
            std::string strOperations = os.str();
            bModified = boost::filesystem::updateFileIfChanged( m_environment.FilePath( operationsFile ), strOperations );
        }

        if( bModified )
        {
            m_environment.setBuildHashCode( operationsFile );
            m_environment.stash( operationsFile, determinant );
            succeeded( taskProgress );
        }
        else
        {
            m_environment.setBuildHashCode( operationsFile );
            cached( taskProgress );
        }

    }
};

BaseTask::Ptr create_Task_PythonWrapper( const TaskArguments&          taskArguments,
                                         const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_PythonWrapper >( taskArguments, sourceFilePath );
}

#define PYTHON_DUMB_STAGE( Stage, File )                                                                  \
    DO_STUFF_AND_REQUIRE_SEMI_COLON(                                                                      \
        const mega::io::CompilationFilePath compilationFile = m_environment.Stage##_##File( m_manifest ); \
                                                                                                          \
        const task::DeterminantHash determinant = { m_toolChain.toolChainHash };                          \
                                                                                                          \
        if( m_environment.restore( compilationFile, determinant ) ) {                                     \
            m_environment.setBuildHashCode( compilationFile );                                            \
            cached( taskProgress );                                                                       \
            return;                                                                                       \
        }                                                                                                 \
                                                                                                          \
        using namespace Stage;                                                                            \
                                                                                                          \
        Database database( m_environment, m_manifest );                                                   \
                                                                                                          \
        const task::FileHash fileHashCode = database.save_##File##_to_temp();                             \
        m_environment.setBuildHashCode( compilationFile, fileHashCode );                                  \
        m_environment.temp_to_real( compilationFile );                                                    \
        m_environment.stash( compilationFile, determinant ); )

class Task_PythonStages : public BaseTask
{
    const mega::io::manifestFilePath& m_manifest;

    // This just creates the database stage used for dynamic python invocations
public:
    Task_PythonStages( const TaskArguments& taskArguments, const mega::io::manifestFilePath& manifest )
        : BaseTask( taskArguments )
        , m_manifest( manifest )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        start( taskProgress, "Task_PythonStages", m_manifest.path(), m_manifest.path() );
        PYTHON_DUMB_STAGE( ParserStage, AST );
        PYTHON_DUMB_STAGE( ParserStage, Body );
        PYTHON_DUMB_STAGE( InterfaceStage, Tree );
        PYTHON_DUMB_STAGE( SymbolRollout, PerSourceSymbols );
        PYTHON_DUMB_STAGE( MetaStage, MetaAnalysis );
        PYTHON_DUMB_STAGE( InterfaceAnalysisStage, Clang );
        PYTHON_DUMB_STAGE( ConcreteStage, Concrete );
        PYTHON_DUMB_STAGE( InheritanceAnalysisRollout, PerSourceDerivations );
        PYTHON_DUMB_STAGE( HyperGraphAnalysisRollout, PerSourceModel );
        PYTHON_DUMB_STAGE( AliasAnalysisRollout, PerSourceModel );
        PYTHON_DUMB_STAGE( ConcreteTypeRollout, PerSourceConcreteTable );
        PYTHON_DUMB_STAGE( MemoryStage, MemoryLayout );
        PYTHON_DUMB_STAGE( GlobalMemoryStageRollout, GlobalMemoryRollout );
        PYTHON_DUMB_STAGE( AutomataStage, AutomataAnalysis );
        PYTHON_DUMB_STAGE( OperationsStage, Operations );
        PYTHON_DUMB_STAGE( OperationsLocs, Locations );
        PYTHON_DUMB_STAGE( ValueSpaceStage, ValueSpace );
        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_PythonStages( const TaskArguments&              taskArguments,
                                        const mega::io::manifestFilePath& manifestFilePath )
{
    return std::make_unique< Task_PythonStages >( taskArguments, manifestFilePath );
}

class Task_PythonObject : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_PythonObject( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        using namespace FinalStage;
        using namespace FinalStage::Interface;

        const mega::io::GeneratedCPPSourceFilePath implementationFile = m_environment.PythonWrapper( m_sourceFilePath );

        const mega::io::ObjectFilePath implementationObj = m_environment.PythonObj( m_sourceFilePath );

        start( taskProgress, "Task_PythonObject", m_sourceFilePath.path(), implementationObj.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( implementationFile ) } );

        if( m_environment.restore( implementationObj, determinant ) )
        {
            m_environment.setBuildHashCode( implementationObj );
            cached( taskProgress );
            return;
        }

        Database database( m_environment, m_environment.project_manifest() );

        const mega::Compilation compilationCMD = mega::Compilation::make_python_obj_compilation(
            m_environment,
            m_toolChain,
            getComponent< Components::Component >( database, m_sourceFilePath ),
            m_sourceFilePath );

        if( run_cmd( taskProgress, compilationCMD.generateCompilationCMD() ) )
        {
            failed( taskProgress );
            return;
        }

        if( m_environment.exists( implementationObj ) )
        {
            m_environment.setBuildHashCode( implementationObj );
            m_environment.stash( implementationObj, determinant );
            succeeded( taskProgress );
        }
        else
        {
            failed( taskProgress );
        }
    }
};

BaseTask::Ptr create_Task_PythonObject( const TaskArguments&          taskArguments,
                                        const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_PythonObject >( taskArguments, sourceFilePath );
}
} // namespace mega::compiler
