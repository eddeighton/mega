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
            , m_implTemplate( m_injaEnvironment.parse_template( m_environment.ImplTemplate().native() ) )
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

    void recurse( TemplateEngine& templateEngine, FinalStage::Interface::IContext* pContext, nlohmann::json& implData,
                  std::vector< std::string >& typeNameStack )
    {
        using namespace FinalStage;
        using namespace FinalStage::Interface;

        std::ostringstream os;
        {
            common::delimit( typeNameStack.begin(), typeNameStack.end(), "::", os );
            if ( !typeNameStack.empty() )
                os << "::";
            os << pContext->get_identifier();
        }

        implData[ "interfaces" ].push_back( os.str() );

        for ( IContext* pNestedContext : pContext->get_children() )
        {
            typeNameStack.push_back( pContext->get_identifier() );
            recurse( templateEngine, pNestedContext, implData, typeNameStack );
            typeNameStack.pop_back();
        }
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        using namespace FinalStage;
        using namespace FinalStage::Interface;

        const mega::io::CompilationFilePath operationsStage
            = m_environment.OperationsStage_Operations( m_sourceFilePath );
        const mega::io::GeneratedCPPSourceFilePath implementationFile
            = m_environment.Implementation( m_sourceFilePath );
        start( taskProgress, "Task_Implementation", operationsStage.path(), implementationFile.path() );

        const task::DeterminantHash determinant( { m_toolChain.toolChainHash,
                                                   m_environment.getBuildHashCode( operationsStage ),
                                                   m_environment.ImplTemplate() } );

        if ( m_environment.restore( implementationFile, determinant ) )
        {
            m_environment.setBuildHashCode( implementationFile );
            cached( taskProgress );
            return;
        }

        {
            Database database( m_environment, m_sourceFilePath );

            ::inja::Environment injaEnvironment;
            {
                injaEnvironment.set_trim_blocks( true );
            }

            TemplateEngine templateEngine( m_environment, injaEnvironment );

            nlohmann::json implData(
                { { "invocations", nlohmann::json::array() }, { "interfaces", nlohmann::json::array() } } );

            {
                std::vector< std::string > typeNameStack;
                Interface::Root*           pRoot = database.one< Interface::Root >( m_sourceFilePath );
                for ( Interface::IContext* pContext : pRoot->get_children() )
                {
                    recurse( templateEngine, pContext, implData, typeNameStack );
                }

                Operations::Invocations* pInvocations = database.one< Operations::Invocations >( m_sourceFilePath );
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
        const mega::io::GeneratedCPPSourceFilePath implementationFile
            = m_environment.Implementation( m_sourceFilePath );
        const mega::io::ObjectFilePath implementationObj = m_environment.ImplementationObj( m_sourceFilePath );
        start( taskProgress, "Task_ImplementationObj", implementationFile.path(), implementationObj.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( implementationFile ),

              m_environment.getBuildHashCode( m_environment.IncludePCH( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( m_environment.InterfacePCH( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( m_environment.GenericsPCH( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( m_environment.OperationsPCH( m_sourceFilePath ) ) } );

        if ( m_environment.restore( implementationObj, determinant ) )
        {
            m_environment.setBuildHashCode( implementationObj );
            cached( taskProgress );
            return;
        }

        using namespace FinalStage;

        Database database( m_environment, m_sourceFilePath );

        Components::Component* pComponent = getComponent< Components::Component >( database, m_sourceFilePath );

        const std::string strCmd = mega::Compilation::make_implementationObj_compilation(
            m_environment, m_toolChain, pComponent, m_sourceFilePath )();

        msg( taskProgress, strCmd );

        const int iResult = boost::process::system( strCmd );
        if ( iResult )
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
