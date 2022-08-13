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

BaseTask::Ptr create_Task_Implementation( const TaskArguments&          taskArguments,
                                          const mega::io::megaFilePath& sourceFilePath )
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

        start( taskProgress, "Task_ImplementationObj", m_sourceFilePath.path(), implementationObj.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( implementationFile ) } );

        if ( m_environment.restore( implementationObj, determinant ) )
        {
            m_environment.setBuildHashCode( implementationObj );
            cached( taskProgress );
            return;
        }

        Database database( m_environment, m_environment.project_manifest() );

        const mega::Compilation compilationCMD = mega::Compilation::make_implementationObj_compilation(
            m_environment, m_toolChain, getComponent< Components::Component >( database, m_sourceFilePath ), m_sourceFilePath );

        if ( run_cmd( taskProgress, compilationCMD.generateCompilationCMD() ) )
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

BaseTask::Ptr create_Task_ImplementationObj( const TaskArguments&          taskArguments,
                                             const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_ImplementationObj >( taskArguments, sourceFilePath );
}

} // namespace compiler
} // namespace mega
