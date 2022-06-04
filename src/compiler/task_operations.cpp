#ifndef TASK_OPERATIONS_4_JUNE_2022
#define TASK_OPERATIONS_4_JUNE_2022

#include "base_task.hpp"

#include "database/model/ConcreteStage.hxx"

#include "database/types/clang_compilation.hpp"

#include "utilities/clang_format.hpp"

#include "common/file.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"
#include <common/stash.hpp>

#include <vector>
#include <string>

namespace driver
{
namespace interface
{
struct CleverUtility
{
    using IDList = std::vector< std::string >;
    IDList& theList;
    CleverUtility( IDList& theList, const std::string& strID )
        : theList( theList )
    {
        theList.push_back( strID );
    }
    ~CleverUtility() { theList.pop_back(); }
};

class Task_Operations : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

    class TemplateEngine
    {
        const mega::io::StashEnvironment& m_environment;
        ::inja::Environment&              m_injaEnvironment;
        ::inja::Template                  m_operationsTemplate;

    public:
        TemplateEngine( const mega::io::StashEnvironment& buildEnvironment, ::inja::Environment& injaEnv )
            : m_environment( buildEnvironment )
            , m_injaEnvironment( injaEnv )
            , m_operationsTemplate( m_injaEnvironment.parse_template( m_environment.OperationsTemplate().native() ) )
        {
        }

        void renderOperations( const nlohmann::json& data, std::ostream& os ) const
        {
            m_injaEnvironment.render_to( os, m_operationsTemplate, data );
        }
    };

public:
    Task_Operations( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    void recurse( ConcreteStage::Interface::Context* pContext,
                  nlohmann::json&                    data,
                  CleverUtility::IDList&             namespaces,
                  CleverUtility::IDList&             types )
    {
        using namespace ConcreteStage;
        using namespace ConcreteStage::Interface;

        if ( Namespace* pNamespace = dynamic_database_cast< Namespace >( pContext ) )
        {
            if ( pNamespace->get_is_global() )
            {
                CleverUtility c( namespaces, pNamespace->get_identifier() );
                for ( Context* pNestedContext : pNamespace->get_children() )
                {
                    recurse( pNestedContext, data, namespaces, types );
                }
            }
            else
            {
                CleverUtility c( types, pNamespace->get_identifier() );
                for ( Context* pNestedContext : pNamespace->get_children() )
                {
                    recurse( pNestedContext, data, namespaces, types );
                }
            }
        }
        else if ( Abstract* pAbstract = dynamic_database_cast< Abstract >( pContext ) )
        {
            CleverUtility c( types, pAbstract->get_identifier() );
            for ( Context* pNestedContext : pAbstract->get_children() )
            {
                recurse( pNestedContext, data, namespaces, types );
            }
        }
        else if ( Action* pAction = dynamic_database_cast< Action >( pContext ) )
        {
            CleverUtility c( types, pAction->get_identifier() );

            std::ostringstream osBody;
            for ( auto pDef : pAction->get_action_defs() )
            {
                if ( !pDef->get_body().empty() )
                {
                    osBody << pDef->get_body();
                    break;
                }
            }
            osBody << "\nco_return mega::done();";

            nlohmann::json operation( { { "return_type", "mega::ActionCoroutine" },
                                        { "body", osBody.str() },
                                        { "has_namespaces", !namespaces.empty() },
                                        { "namespaces", namespaces },
                                        { "types", types },
                                        { "params", "" } } );
            data[ "operations" ].push_back( operation );

            for ( Context* pNestedContext : pAction->get_children() )
            {
                recurse( pNestedContext, data, namespaces, types );
            }
        }
        else if ( Event* pEvent = dynamic_database_cast< Event >( pContext ) )
        {
        }
        else if ( Function* pFunction = dynamic_database_cast< Function >( pContext ) )
        {
            CleverUtility c( types, pFunction->get_identifier() );

            std::string strBody;
            {
                for ( auto pDef : pFunction->get_function_defs() )
                {
                    if ( !pDef->get_body().empty() )
                    {
                        strBody = pDef->get_body();
                        break;
                    }
                }
            }
            nlohmann::json operation( { { "return_type", pFunction->get_return_type_trait()->get_str() },
                                        { "body", strBody },
                                        { "has_namespaces", !namespaces.empty() },
                                        { "namespaces", namespaces },
                                        { "types", types },
                                        { "params", pFunction->get_arguments_trait()->get_str() } } );
            data[ "operations" ].push_back( operation );
        }
        else if ( Object* pObject = dynamic_database_cast< Object >( pContext ) )
        {
            CleverUtility c( types, pObject->get_identifier() );
            for ( Context* pNestedContext : pObject->get_children() )
            {
                recurse( pNestedContext, data, namespaces, types );
            }
        }
        else if ( Link* pLink = dynamic_database_cast< Link >( pContext ) )
        {
            CleverUtility c( types, pLink->get_identifier() );
            for ( Context* pNestedContext : pLink->get_children() )
            {
                recurse( pNestedContext, data, namespaces, types );
            }
        }
        else if ( Table* pTable = dynamic_database_cast< Table >( pContext ) )
        {
        }
        else
        {
            THROW_RTE( "Unknown context type" );
        }
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath concreteFile = m_environment.ConcreteStage_Concrete( m_sourceFilePath );
        const mega::io::GeneratedHPPSourceFilePath operationsFile = m_environment.Operations( m_sourceFilePath );
        start( taskProgress, "Task_Operations", concreteFile.path(), operationsFile.path() );

        task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.OperationsTemplate(),
              m_environment.getBuildHashCode( m_environment.ParserStage_Body( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( concreteFile ) } );

        if ( m_environment.restore( operationsFile, determinant ) )
        {
            m_environment.setBuildHashCode( operationsFile );
            cached( taskProgress );
            return;
        }

        {
            using namespace ConcreteStage;
            using namespace ConcreteStage::Interface;

            Database database( m_environment, m_sourceFilePath );

            std::ostringstream os;
            {
                ::inja::Environment injaEnvironment;
                {
                    injaEnvironment.set_trim_blocks( true );
                }
                TemplateEngine templateEngine( m_environment, injaEnvironment );

                nlohmann::json data( { { "operations", nlohmann::json::array() } } );

                Interface::Root*      pRoot = database.one< Interface::Root >( m_sourceFilePath );
                CleverUtility::IDList namespaces, types;
                for ( Context* pContext : pRoot->get_children() )
                {
                    recurse( pContext, data, namespaces, types );
                }

                templateEngine.renderOperations( data, os );
            }
            std::string strOperations = os.str();
            mega::utilities::clang_format( strOperations, std::optional< boost::filesystem::path >() );
            boost::filesystem::updateFileIfChanged( m_environment.FilePath( operationsFile ), strOperations );
        }

        m_environment.setBuildHashCode( operationsFile );
        m_environment.stash( operationsFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_Operations( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_Operations >( taskArguments, sourceFilePath );
}

class Task_OperationsPCH : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_OperationsPCH( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::GeneratedHPPSourceFilePath operationsFile = m_environment.Operations( m_sourceFilePath );
        const mega::io::PrecompiledHeaderFile      operationsPCH  = m_environment.OperationsPCH( m_sourceFilePath );
        start( taskProgress, "Task_OperationsPCH", operationsFile.path(), operationsPCH.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, 123, m_environment.getBuildHashCode( operationsFile ) } );

        if ( m_environment.restore( operationsPCH, determinant ) )
        {
            m_environment.setBuildHashCode( operationsPCH );
            cached( taskProgress );
            return;
        }

        using namespace ConcreteStage;

        Database database( m_environment, m_sourceFilePath );

        Components::Component* pComponent = getComponent< Components::Component >( database, m_sourceFilePath );

        const std::string strCmd = mega::Compilation::make_operationsPCH_compilation(
            m_environment, m_toolChain, pComponent, m_sourceFilePath )();

        msg( taskProgress, strCmd );

        const int iResult = boost::process::system( strCmd );
        if ( iResult )
        {
            std::ostringstream os;
            os << "Error compiling operations pch file for source file: " << m_sourceFilePath.path();
            throw std::runtime_error( os.str() );
        }

        m_environment.setBuildHashCode( operationsPCH );
        m_environment.stash( operationsPCH, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_OperationsPCH( const TaskArguments&          taskArguments,
                                         const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_OperationsPCH >( taskArguments, sourceFilePath );
}

} // namespace interface
} // namespace driver

#endif // TASK_OPERATIONS_4_JUNE_2022
