#ifndef TASK_INTERFACE_ANALYSIS_10_MAY_2022
#define TASK_INTERFACE_ANALYSIS_10_MAY_2022

#include "base_task.hpp"

#include "database/model/InterfaceAnalysisStage.hxx"

#include "database/types/clang_compilation.hpp"

#include "mega/common_strings.hpp"

#include "utilities/clang_format.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include <memory>

namespace driver
{
namespace interface
{

class Task_ObjectInterfaceGeneration : public BaseTask
{
public:
    Task_ObjectInterfaceGeneration( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    class TemplateEngine
    {
        const mega::io::StashEnvironment& m_environment;
        ::inja::Environment&              m_injaEnvironment;
        ::inja::Template                  m_contextTemplate;
        ::inja::Template                  m_namespaceTemplate;
        ::inja::Template                  m_interfaceTemplate;

    public:
        TemplateEngine( const mega::io::StashEnvironment& buildEnvironment, ::inja::Environment& injaEnv )
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

    template < typename TContextType >
    std::vector< nlohmann::json >
    getInheritanceTraits( const nlohmann::json& typenames, TContextType* pContext,
                          InterfaceAnalysisStage::Interface::InheritanceTrait* pInheritanceTrait )
    {
        using namespace InterfaceAnalysisStage;
        using namespace InterfaceAnalysisStage::Interface;

        std::vector< nlohmann::json > traits;
        int                           iCounter = 0;
        for ( const std::string& strType : pInheritanceTrait->get_strings() )
        {
            nlohmann::json     traitNames = typenames;
            std::ostringstream os;
            os << mega::EG_BASE_PREFIX_TRAIT_TYPE << iCounter++;
            traitNames.push_back( os.str() );

            nlohmann::json trait_struct( { { "name", os.str() },
                                           { "typeid", pContext->get_type_id() },
                                           { "types", traitNames },
                                           { "traits", nlohmann::json::array() } } );

            {
                std::ostringstream osTrait;
                osTrait << "using Type  = " << strType;
                trait_struct[ "traits" ].push_back( osTrait.str() );
            }
            traits.push_back( trait_struct );
        }
        return traits;
    }

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
            if ( auto opt = pAbstract->get_inheritance_trait() )
            {
                for ( const nlohmann::json& trait : getInheritanceTraits( typenames, pAbstract, opt.value() ) )
                {
                    contextData[ "trait_structs" ].push_back( trait );
                    structs.push_back( trait );
                }
            }
            templateEngine.renderContext( contextData, os );
        }
        else if ( Action* pAction = dynamic_database_cast< Action >( pContext ) )
        {
            if ( auto opt = pAction->get_inheritance_trait() )
            {
                for ( const nlohmann::json& trait : getInheritanceTraits( typenames, pAction, opt.value() ) )
                {
                    contextData[ "trait_structs" ].push_back( trait );
                    structs.push_back( trait );
                }
            }
            templateEngine.renderContext( contextData, os );
        }
        else if ( Event* pEvent = dynamic_database_cast< Event >( pContext ) )
        {
            if ( auto opt = pEvent->get_inheritance_trait() )
            {
                for ( const nlohmann::json& trait : getInheritanceTraits( typenames, pEvent, opt.value() ) )
                {
                    contextData[ "trait_structs" ].push_back( trait );
                    structs.push_back( trait );
                }
            }
            templateEngine.renderContext( contextData, os );
        }
        else if ( Function* pFunction = dynamic_database_cast< Function >( pContext ) )
        {
            nlohmann::json functionTraitTypeNames = typenames;
            functionTraitTypeNames.push_back( mega::EG_FUNCTION_TRAIT_TYPE );

            nlohmann::json trait_struct( { { "name", mega::EG_FUNCTION_TRAIT_TYPE },
                                           { "typeid", pContext->get_type_id() },
                                           { "types", functionTraitTypeNames },
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
            if ( auto opt = pObject->get_inheritance_trait() )
            {
                for ( const nlohmann::json& trait : getInheritanceTraits( typenames, pObject, opt.value() ) )
                {
                    contextData[ "trait_structs" ].push_back( trait );
                    structs.push_back( trait );
                }
            }
            templateEngine.renderContext( contextData, os );
        }
        else if ( Link* pLink = dynamic_database_cast< Link >( pContext ) )
        {
            for ( const nlohmann::json& trait : getInheritanceTraits( typenames, pLink, pLink->get_link_target() ) )
            {
                contextData[ "trait_structs" ].push_back( trait );
                structs.push_back( trait );
            }

            templateEngine.renderContext( contextData, os );
        }
        else if ( Table* pTable = dynamic_database_cast< Table >( pContext ) )
        {
            templateEngine.renderContext( contextData, os );
        }
        else
        {
            THROW_RTE( "Unknown context type" );
        }
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath interfaceTreeFile = m_environment.InterfaceStage_Tree( m_sourceFilePath );
        const mega::io::GeneratedHPPSourceFilePath interfaceHeader = m_environment.Interface( m_sourceFilePath );
        start( taskProgress, "Task_ObjectInterfaceGeneration", interfaceTreeFile.path(), interfaceHeader.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( interfaceTreeFile ),
              m_environment.ContextTemplate(), m_environment.NamespaceTemplate(), m_environment.InterfaceTemplate() } );

        if ( m_environment.restore( interfaceHeader, determinant ) )
        {
            m_environment.setBuildHashCode( interfaceHeader );
            cached( taskProgress );
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
                { { "interface", strInterface }, { "guard", determinant.toHexString() }, { "structs", structs } } );

            std::unique_ptr< boost::filesystem::ofstream > pOStream = m_environment.write( interfaceHeader );
            templateEngine.renderInterface( interfaceData, *pOStream );
        }

        m_environment.setBuildHashCode( interfaceHeader );
        m_environment.stash( interfaceHeader, determinant );

        succeeded( taskProgress );
    }
    const mega::io::megaFilePath& m_sourceFilePath;
};

BaseTask::Ptr create_Task_ObjectInterfaceGeneration( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_ObjectInterfaceGeneration >( taskArguments, sourceFilePath );
}

class Task_ObjectInterfaceAnalysis : public BaseTask
{
public:
    Task_ObjectInterfaceAnalysis( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath interfaceTreeFile = m_environment.InterfaceStage_Tree( m_sourceFilePath );
        const mega::io::GeneratedHPPSourceFilePath interfaceHeader = m_environment.Interface( m_sourceFilePath );
        const mega::io::PrecompiledHeaderFile      includePCH      = m_environment.PCH( m_sourceFilePath );
        const mega::io::PrecompiledHeaderFile interfacePCHFilePath = m_environment.InterfacePCH( m_sourceFilePath );
        const mega::io::CompilationFilePath   interfaceAnalysisFile
            = m_environment.InterfaceAnalysisStage_Clang( m_sourceFilePath );

        start( taskProgress, "Task_ObjectInterfaceAnalysis", interfaceHeader.path(), interfacePCHFilePath.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( interfaceHeader ),
              m_environment.getBuildHashCode( includePCH ), m_environment.getBuildHashCode( interfaceTreeFile ) } );

        if ( m_environment.restore( interfacePCHFilePath, determinant )
             && m_environment.restore( interfaceAnalysisFile, determinant ) )
        {
            m_environment.setBuildHashCode( interfacePCHFilePath );
            m_environment.setBuildHashCode( interfaceAnalysisFile );
            cached( taskProgress );
            return;
        }

        using namespace InterfaceAnalysisStage;
        using namespace InterfaceAnalysisStage::Interface;

        Database               database( m_environment, m_sourceFilePath );
        Components::Component* pComponent = getComponent< Components::Component >( database, m_sourceFilePath );

        const std::string strCmd = mega::Compilation(
            m_toolChain.clangCompilerPath, m_toolChain.clangPluginPath, pComponent->get_cpp_flags(),
            pComponent->get_cpp_defines(), pComponent->get_includeDirectories(), mega::CompilationMode::eInterface,
            m_environment.rootSourceDir(), m_environment.rootBuildDir(), m_environment.FilePath( m_sourceFilePath ),
            m_environment.FilePath( includePCH ), m_environment.FilePath( interfaceHeader ),
            m_environment.FilePath( interfacePCHFilePath ) )();

        msg( taskProgress, strCmd );

        const int iResult = boost::process::system( strCmd );
        if ( iResult )
        {
            failed( taskProgress );
            return;
        }

        if ( m_environment.exists( interfaceAnalysisFile ) && m_environment.exists( interfacePCHFilePath ) )
        {
            m_environment.setBuildHashCode( interfacePCHFilePath );
            m_environment.stash( interfacePCHFilePath, determinant );

            m_environment.setBuildHashCode( interfaceAnalysisFile );
            m_environment.stash( interfaceAnalysisFile, determinant );

            succeeded( taskProgress );
        }
        else
        {
            failed( taskProgress );
        }
    }

    const mega::io::megaFilePath& m_sourceFilePath;
};

BaseTask::Ptr create_Task_ObjectInterfaceAnalysis( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_ObjectInterfaceAnalysis >( taskArguments, sourceFilePath );
}

} // namespace interface
} // namespace driver

#endif // TASK_INTERFACE_ANALYSIS_10_MAY_2022
