#include "base_task.hpp"

#include "database/model/FinalStage.hxx"

#include "database/types/clang_compilation.hpp"

#include <common/file.hpp>
#include <common/string.hpp>

#include "database/types/operation.hpp"
#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

namespace mega
{
namespace compiler
{

class Task_Implementation : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

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
    Task_Implementation( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        using namespace FinalStage;
        using namespace FinalStage::Interface;

        const mega::io::GeneratedCPPSourceFilePath implementationFile
            = m_environment.Implementation( m_sourceFilePath );

        start( taskProgress, "Task_Implementation", m_sourceFilePath.path(), implementationFile.path() );

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

            nlohmann::json implData(
                { { "invocations", nlohmann::json::array() }, { "interfaces", nlohmann::json::array() } } );

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
                    nlohmann::json invocation(
                        { { "return_type", pInvocation->get_return_type_str() },
                          { "context", pInvocation->get_context_str() },
                          { "type_path", pInvocation->get_type_path_str() },
                          { "operation", mega::getOperationString( pInvocation->get_operation() ) },
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

BaseTask::Ptr create_Task_Implementation( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_Implementation >( taskArguments, sourceFilePath );
}

class Task_ImplementationObj : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_ImplementationObj( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        using namespace FinalStage;
        using namespace FinalStage::Interface;

        const mega::io::GeneratedCPPSourceFilePath implementationFile
            = m_environment.Implementation( m_sourceFilePath );

        const mega::io::ObjectFilePath implementationObj = m_environment.ImplementationObj( m_sourceFilePath );

        start( taskProgress, "Task_ImplementationObj", implementationFile.path(), implementationObj.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( implementationFile ) } );

        if ( m_environment.restore( implementationObj, determinant ) )
        {
            m_environment.setBuildHashCode( implementationObj );
            cached( taskProgress );
            return;
        }

        Database database( m_environment, m_environment.project_manifest() );

        const std::string strCmd = mega::Compilation::make_implementationObj_compilation(
            m_environment, m_toolChain, getComponent< Components::Component >( database, m_sourceFilePath ),
            m_sourceFilePath )();

        if ( run_cmd( taskProgress, strCmd ) )
        {
            failed( taskProgress );
            return;
        }

        if ( m_environment.exists( implementationObj ) )
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

BaseTask::Ptr create_Task_ImplementationObj( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_ImplementationObj >( taskArguments, sourceFilePath );
}

/*

class Task_Component_Impl : public BaseTask
{
    const std::string& m_strComponentName;

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
    Task_Component_Impl( const TaskArguments& taskArguments, const std::string& strComponentName )
        : BaseTask( taskArguments )
        , m_strComponentName( strComponentName )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        using namespace FinalStage;
        using namespace FinalStage::Interface;

        Database database( m_environment, m_environment.project_manifest() );

        Components::Component* pComponent = nullptr;
        {
            for ( Components::Component* pIter :
                  database.template many< Components::Component >( m_environment.project_manifest() ) )
            {
                if ( pIter->get_name() == m_strComponentName )
                {
                    pComponent = pIter;
                    break;
                }
            }
        }
        VERIFY_RTE( pComponent );

        const mega::io::GeneratedCPPSourceFilePath implementationFile
            = m_environment.Implementation( pComponent->get_build_dir(), pComponent->get_name() );

        start( taskProgress, "Task_Component_Impl", pComponent->get_name(), implementationFile.path() );

        task::DeterminantHash determinant( { m_toolChain.toolChainHash, m_environment.ImplTemplate() } );
        for ( const auto& srcFile : pComponent->get_mega_source_files() )
        {
            determinant ^= m_environment.getBuildHashCode( m_environment.OperationsStage_Operations( srcFile ) );
        }
        for ( const auto& srcFile : pComponent->get_cpp_source_files() )
        {
            determinant ^= m_environment.getBuildHashCode( m_environment.OperationsStage_Operations( srcFile ) );
        }

        if ( m_environment.restore( implementationFile, determinant ) )
        {
            m_environment.setBuildHashCode( implementationFile );
            cached( taskProgress );
            return;
        }

        {
            ::inja::Environment injaEnvironment;
            {
                injaEnvironment.set_trim_blocks( true );
            }

            TemplateEngine templateEngine( m_environment, injaEnvironment );

            nlohmann::json implData(
                { { "invocations", nlohmann::json::array() }, { "interfaces", nlohmann::json::array() } } );

            std::vector< Operations::Invocations* > invocations;
            {
                for ( const auto& srcFile : pComponent->get_mega_source_files() )
                {
                    invocations.push_back( database.one< Operations::Invocations >( srcFile ) );
                }
                for ( const auto& srcFile : pComponent->get_cpp_source_files() )
                {
                    invocations.push_back( database.one< Operations::Invocations >( srcFile ) );
                }
            }
            std::vector< Interface::IContext* > contexts;
            {
                std::set< Interface::IContext* > uniqueContexts;
                for ( Operations::Invocations* pInvocations : invocations )
                {
                    for ( auto& [ id, pInvocation ] : pInvocations->get_invocations() )
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

                for ( Operations::Invocations* pInvocations : invocations )
                {
                    for ( auto& [ id, pInvocation ] : pInvocations->get_invocations() )
                    {
                        nlohmann::json invocation(
                            { { "return_type", pInvocation->get_return_type_str() },
                              { "context", pInvocation->get_context_str() },
                              { "type_path", pInvocation->get_type_path_str() },
                              { "operation", mega::getOperationString( pInvocation->get_operation() ) },
                              { "impl", "" } } );

                        implData[ "invocations" ].push_back( invocation );
                    }
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

BaseTask::Ptr create_Task_Component_Impl( const TaskArguments& taskArguments, const std::string& strComponentName )
{
    return std::make_unique< Task_Component_Impl >( taskArguments, strComponentName );
}

class Task_Component_ImplObj : public BaseTask
{
    const std::string& m_strComponentName;

public:
    Task_Component_ImplObj( const TaskArguments& taskArguments, const std::string& strComponentName )
        : BaseTask( taskArguments )
        , m_strComponentName( strComponentName )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        using namespace FinalStage;
        using namespace FinalStage::Interface;

        Database database( m_environment, m_environment.project_manifest() );

        Components::Component* pComponent = nullptr;
        {
            for ( Components::Component* pIter :
                  database.template many< Components::Component >( m_environment.project_manifest() ) )
            {
                if ( pIter->get_name() == m_strComponentName )
                {
                    pComponent = pIter;
                    break;
                }
            }
        }
        VERIFY_RTE( pComponent );

        const mega::io::GeneratedCPPSourceFilePath implementationFile
            = m_environment.Implementation( pComponent->get_build_dir(), pComponent->get_name() );

        const mega::io::ObjectFilePath implementationObj
            = m_environment.ImplementationObj( pComponent->get_build_dir(), pComponent->get_name() );

        start( taskProgress, "Task_Component_ImplObj", implementationFile.path(), implementationObj.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( implementationFile ) } );

        if ( m_environment.restore( implementationObj, determinant ) )
        {
            m_environment.setBuildHashCode( implementationObj );
            cached( taskProgress );
            return;
        }

        const std::string strCmd = mega::Compilation::make_implementationObj_compilation(
            m_environment, m_toolChain, pComponent )();

        if ( run_cmd( taskProgress, strCmd ) )
        {
            failed( taskProgress );
            return;
        }

        if ( m_environment.exists( implementationObj ) )
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

BaseTask::Ptr create_Task_Component_ImplObj( const TaskArguments& taskArguments, const std::string& strComponentName )
{
    return std::make_unique< Task_Component_ImplObj >( taskArguments, strComponentName );
}
*/
} // namespace compiler
} // namespace mega
