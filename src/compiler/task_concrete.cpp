#include "base_task.hpp"

#include "database/model/ConcreteStage.hxx"
#include <common/stash.hpp>
#include <optional>

namespace mega
{
namespace compiler
{

class Task_ConcreteTree : public BaseTask
{
public:
    Task_ConcreteTree( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    struct IdentifierMap
    {
        std::set< std::string > identifiers;
        using ContextVector = std::vector< ConcreteStage::Interface::IContext* >;
        ContextVector inherited;
        ContextVector contexts;

        using DimensionVector = std::vector< ConcreteStage::Interface::DimensionTrait* >;
        DimensionVector dimensions;

        void addContext( ConcreteStage::Interface::IContext* pContext )
        {
            const std::string& strIdentifier = pContext->get_identifier();
            if ( !identifiers.count( strIdentifier ) )
            {
                contexts.push_back( pContext );
                identifiers.insert( strIdentifier );
            }
        }
        void addDimension( ConcreteStage::Interface::DimensionTrait* pDimension )
        {
            const std::string& strIdentifier = pDimension->get_id()->get_str();
            if ( !identifiers.count( strIdentifier ) )
            {
                dimensions.push_back( pDimension );
                identifiers.insert( strIdentifier );
            }
        }
    };

    template < typename TContextType >
    void collectDimensions( TContextType* pInterfaceContext, IdentifierMap& identifierMap )
    {
        using namespace ConcreteStage;
        VERIFY_RTE( pInterfaceContext );
        for ( Interface::DimensionTrait* pDimension : pInterfaceContext->get_dimension_traits() )
        {
            identifierMap.addDimension( pDimension );
        }
    }

    template < typename TContextType >
    void collectContexts( TContextType* pInterfaceContext, IdentifierMap& identifierMap )
    {
        using namespace ConcreteStage;
        VERIFY_RTE( pInterfaceContext );
        for ( Interface::IContext* pContext : pInterfaceContext->get_children() )
        {
            identifierMap.addContext( pContext );
        }
    }

    void recurseInheritance( ConcreteStage::Database& database, ConcreteStage::Concrete::Context* pConcreteRoot,
                             ConcreteStage::Interface::IContext* pContext, IdentifierMap& identifierMap )
    {
        using namespace ConcreteStage;
        using namespace ConcreteStage::Concrete;

        identifierMap.inherited.push_back( pContext );

        collectContexts( pContext, identifierMap );

        if ( Interface::Namespace* pNamespace = dynamic_database_cast< Interface::Namespace >( pContext ) )
        {
            // do nothing
            collectDimensions( pNamespace, identifierMap );
        }
        else if ( Interface::Abstract* pAbstract = dynamic_database_cast< Interface::Abstract >( pContext ) )
        {
            collectDimensions( pAbstract, identifierMap );
            if ( std::optional< Interface::InheritanceTrait* > inheritanceOpt = pAbstract->get_inheritance_trait() )
            {
                for ( Interface::IContext* pInheritedContext : inheritanceOpt.value()->get_contexts() )
                {
                    recurseInheritance( database, pConcreteRoot, pInheritedContext, identifierMap );
                }
            }
        }
        else if ( Interface::Action* pAction = dynamic_database_cast< Interface::Action >( pContext ) )
        {
            collectDimensions( pAction, identifierMap );
            if ( std::optional< Interface::InheritanceTrait* > inheritanceOpt = pAction->get_inheritance_trait() )
            {
                for ( Interface::IContext* pInheritedContext : inheritanceOpt.value()->get_contexts() )
                {
                    recurseInheritance( database, pConcreteRoot, pInheritedContext, identifierMap );
                }
            }
        }
        else if ( Interface::Event* pEvent = dynamic_database_cast< Interface::Event >( pContext ) )
        {
            collectDimensions( pEvent, identifierMap );
            if ( std::optional< Interface::InheritanceTrait* > inheritanceOpt = pEvent->get_inheritance_trait() )
            {
                for ( Interface::IContext* pInheritedContext : inheritanceOpt.value()->get_contexts() )
                {
                    recurseInheritance( database, pConcreteRoot, pInheritedContext, identifierMap );
                }
            }
        }
        else if ( Interface::Function* pFunction = dynamic_database_cast< Interface::Function >( pContext ) )
        {
            // do nothing
        }
        else if ( Interface::Object* pObject = dynamic_database_cast< Interface::Object >( pContext ) )
        {
            collectDimensions( pObject, identifierMap );
            if ( std::optional< Interface::InheritanceTrait* > inheritanceOpt = pObject->get_inheritance_trait() )
            {
                for ( Interface::IContext* pInheritedContext : inheritanceOpt.value()->get_contexts() )
                {
                    recurseInheritance( database, pConcreteRoot, pInheritedContext, identifierMap );
                }
            }
        }
        else if ( Interface::Link* pLink = dynamic_database_cast< Interface::Link >( pContext ) )
        {
            // do nothing
        }
        else if ( Interface::Table* pTable = dynamic_database_cast< Interface::Table >( pContext ) )
        {
            // do nothing
        }
        else if ( Interface::Buffer* pBuffer = dynamic_database_cast< Interface::Buffer >( pContext ) )
        {
            collectDimensions( pBuffer, identifierMap );
        }
        else
        {
            THROW_RTE( "Unknown context type" );
        }
    }

    void constructElements( ConcreteStage::Database&                                   database,
                            ConcreteStage::Concrete::ContextGroup*                     parentConcreteContextGroup,
                            const IdentifierMap&                                       inheritedContexts,
                            std::vector< ConcreteStage::Concrete::Context* >&          childContexts,
                            std::vector< ConcreteStage::Concrete::Dimensions::User* >& dimensions )
    {
        using namespace ConcreteStage;
        using namespace ConcreteStage::Concrete;

        for ( Interface::IContext* pChildContext : inheritedContexts.contexts )
        {
            if ( Context* pContext = recurse( database, parentConcreteContextGroup, pChildContext ) )
                childContexts.push_back( pContext );
        }

        for ( Interface::DimensionTrait* pInterfaceDimension : inheritedContexts.dimensions )
        {
            Concrete::Context* pParentConcreteContext
                = dynamic_database_cast< Concrete::Context >( parentConcreteContextGroup );

            Dimensions::User* pConcreteDimension = database.construct< Dimensions::User >(
                Dimensions::User::Args{ pParentConcreteContext, pInterfaceDimension } );
            dimensions.push_back( pConcreteDimension );

            // set the pointer in interface to concrete dimension
            database.construct< Interface::DimensionTrait >(
                Interface::DimensionTrait::Args{ pInterfaceDimension, pConcreteDimension } );
        }
    }

    ConcreteStage::Concrete::Context* recurse( ConcreteStage::Database&               database,
                                               ConcreteStage::Concrete::ContextGroup* pParentContextGroup,
                                               ConcreteStage::Interface::IContext*    pContext )
    {
        using namespace ConcreteStage;
        using namespace ConcreteStage::Concrete;

        if ( Interface::Namespace* pNamespace = dynamic_database_cast< Interface::Namespace >( pContext ) )
        {
            Namespace* pConcrete = database.construct< Namespace >( Namespace::Args{
                Context::Args{ ContextGroup::Args{ {} }, pParentContextGroup, pNamespace, {} }, pNamespace, {} } );
            pParentContextGroup->push_back_children( pConcrete );

            IdentifierMap inheritedContexts;
            {
                recurseInheritance( database, pConcrete, pNamespace, inheritedContexts );
                pConcrete->set_inheritance( inheritedContexts.inherited );
            }

            std::vector< ConcreteStage::Concrete::Context* >          childContexts;
            std::vector< ConcreteStage::Concrete::Dimensions::User* > dimensions;
            constructElements( database, pConcrete, inheritedContexts, childContexts, dimensions );

            pConcrete->set_dimensions( dimensions );
            pConcrete->set_children( childContexts );

            database.construct< Interface::IContext >( Interface::IContext::Args{ pContext, { pConcrete } } );

            return pConcrete;
        }
        else if ( Interface::Abstract* pAbstract = dynamic_database_cast< Interface::Abstract >( pContext ) )
        {
            database.construct< Interface::IContext >( Interface::IContext::Args{ pContext, { std::nullopt } } );

            // do nothing
            return nullptr;
        }
        else if ( Interface::Action* pAction = dynamic_database_cast< Interface::Action >( pContext ) )
        {
            Action* pConcrete = database.construct< Action >( Action::Args{
                Context::Args{ ContextGroup::Args{ {} }, pParentContextGroup, pAction, {} }, pAction, {} } );
            pParentContextGroup->push_back_children( pConcrete );

            IdentifierMap inheritedContexts;
            {
                recurseInheritance( database, pConcrete, pAction, inheritedContexts );
                pConcrete->set_inheritance( inheritedContexts.inherited );
            }

            std::vector< ConcreteStage::Concrete::Context* >          childContexts;
            std::vector< ConcreteStage::Concrete::Dimensions::User* > dimensions;
            constructElements( database, pConcrete, inheritedContexts, childContexts, dimensions );

            pConcrete->set_dimensions( dimensions );
            pConcrete->set_children( childContexts );

            database.construct< Interface::IContext >( Interface::IContext::Args{ pContext, { pConcrete } } );

            return pConcrete;
        }
        else if ( Interface::Event* pEvent = dynamic_database_cast< Interface::Event >( pContext ) )
        {
            Event* pConcrete = database.construct< Event >(
                Event::Args{ Context::Args{ ContextGroup::Args{ {} }, pParentContextGroup, pEvent, {} }, pEvent, {} } );
            pParentContextGroup->push_back_children( pConcrete );

            IdentifierMap inheritedContexts;
            {
                recurseInheritance( database, pConcrete, pEvent, inheritedContexts );
                pConcrete->set_inheritance( inheritedContexts.inherited );
            }

            std::vector< ConcreteStage::Concrete::Context* >          childContexts;
            std::vector< ConcreteStage::Concrete::Dimensions::User* > dimensions;
            constructElements( database, pConcrete, inheritedContexts, childContexts, dimensions );

            pConcrete->set_dimensions( dimensions );
            pConcrete->set_children( childContexts );

            database.construct< Interface::IContext >( Interface::IContext::Args{ pContext, { pConcrete } } );

            return pConcrete;
        }
        else if ( Interface::Function* pFunction = dynamic_database_cast< Interface::Function >( pContext ) )
        {
            Function* pConcrete = database.construct< Function >( Function::Args{
                Context::Args{ ContextGroup::Args{ {} }, pParentContextGroup, pFunction, {} }, pFunction } );
            pParentContextGroup->push_back_children( pConcrete );

            IdentifierMap inheritedContexts;
            {
                recurseInheritance( database, pConcrete, pFunction, inheritedContexts );
                pConcrete->set_inheritance( inheritedContexts.inherited );
            }

            std::vector< ConcreteStage::Concrete::Context* >          childContexts;
            std::vector< ConcreteStage::Concrete::Dimensions::User* > dimensions;
            constructElements( database, pConcrete, inheritedContexts, childContexts, dimensions );
            VERIFY_RTE( dimensions.empty() );

            pConcrete->set_children( childContexts );

            database.construct< Interface::IContext >( Interface::IContext::Args{ pContext, { pConcrete } } );

            return pConcrete;
        }
        else if ( Interface::Object* pObject = dynamic_database_cast< Interface::Object >( pContext ) )
        {
            Object* pConcrete = database.construct< Object >( Object::Args{
                Context::Args{ ContextGroup::Args{ {} }, pParentContextGroup, pObject, {} }, pObject, {} } );
            pParentContextGroup->push_back_children( pConcrete );

            IdentifierMap inheritedContexts;
            {
                recurseInheritance( database, pConcrete, pObject, inheritedContexts );
                pConcrete->set_inheritance( inheritedContexts.inherited );
            }

            std::vector< ConcreteStage::Concrete::Context* >          childContexts;
            std::vector< ConcreteStage::Concrete::Dimensions::User* > dimensions;
            constructElements( database, pConcrete, inheritedContexts, childContexts, dimensions );

            pConcrete->set_dimensions( dimensions );
            pConcrete->set_children( childContexts );

            database.construct< Interface::IContext >( Interface::IContext::Args{ pContext, { pConcrete } } );

            return pConcrete;
        }
        else if ( Interface::Link* pLink = dynamic_database_cast< Interface::Link >( pContext ) )
        {
            Link* pConcrete = database.construct< Link >(
                Link::Args{ Context::Args{ ContextGroup::Args{ {} }, pParentContextGroup, pLink, {} }, pLink } );
            pParentContextGroup->push_back_children( pConcrete );

            IdentifierMap inheritedContexts;
            {
                recurseInheritance( database, pConcrete, pLink, inheritedContexts );
                pConcrete->set_inheritance( inheritedContexts.inherited );
            }

            std::vector< ConcreteStage::Concrete::Context* >          childContexts;
            std::vector< ConcreteStage::Concrete::Dimensions::User* > dimensions;
            constructElements( database, pConcrete, inheritedContexts, childContexts, dimensions );
            VERIFY_RTE( dimensions.empty() );

            pConcrete->set_children( childContexts );

            database.construct< Interface::IContext >( Interface::IContext::Args{ pContext, { pConcrete } } );

            return pConcrete;
        }
        else if ( Interface::Table* pTable = dynamic_database_cast< Interface::Table >( pContext ) )
        {
            Table* pConcrete = database.construct< Table >(
                Table::Args{ Context::Args{ ContextGroup::Args{ {} }, pParentContextGroup, pTable, {} }, pTable } );
            pParentContextGroup->push_back_children( pConcrete );

            IdentifierMap inheritedContexts;
            {
                recurseInheritance( database, pConcrete, pTable, inheritedContexts );
                pConcrete->set_inheritance( inheritedContexts.inherited );
            }

            std::vector< ConcreteStage::Concrete::Context* >          childContexts;
            std::vector< ConcreteStage::Concrete::Dimensions::User* > dimensions;
            constructElements( database, pConcrete, inheritedContexts, childContexts, dimensions );
            VERIFY_RTE( dimensions.empty() );

            pConcrete->set_children( childContexts );

            database.construct< Interface::IContext >( Interface::IContext::Args{ pContext, { pConcrete } } );

            return pConcrete;
        }
        else if ( Interface::Buffer* pBuffer = dynamic_database_cast< Interface::Buffer >( pContext ) )
        {
            Buffer* pConcrete = database.construct< Buffer >(
                Buffer::Args{ Context::Args{ ContextGroup::Args{ {} }, pParentContextGroup, pBuffer, {} }, pBuffer, {} } );
            pParentContextGroup->push_back_children( pConcrete );

            IdentifierMap inheritedContexts;
            {
                recurseInheritance( database, pConcrete, pBuffer, inheritedContexts );
                pConcrete->set_inheritance( inheritedContexts.inherited );
            }

            std::vector< ConcreteStage::Concrete::Context* >          childContexts;
            std::vector< ConcreteStage::Concrete::Dimensions::User* > dimensions;
            constructElements( database, pConcrete, inheritedContexts, childContexts, dimensions );
            VERIFY_RTE( dimensions.empty() );

            pConcrete->set_children( childContexts );

            database.construct< Interface::IContext >( Interface::IContext::Args{ pContext, { pConcrete } } );

            return pConcrete;
        }
        else
        {
            THROW_RTE( "Unknown context type" );
        }
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath interfaceAnalysisFile
            = m_environment.InterfaceAnalysisStage_Clang( m_sourceFilePath );
        const mega::io::CompilationFilePath concreteFile = m_environment.ConcreteStage_Concrete( m_sourceFilePath );
        start( taskProgress, "Task_ConcreteTree", interfaceAnalysisFile.path(), concreteFile.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( interfaceAnalysisFile ) } );

        if ( m_environment.restore( concreteFile, determinant ) )
        {
            m_environment.setBuildHashCode( concreteFile );
            cached( taskProgress );
            return;
        }

        using namespace ConcreteStage;
        using namespace ConcreteStage::Concrete;

        Database database( m_environment, m_sourceFilePath );

        Interface::Root* pInterfaceRoot = database.one< Interface::Root >( m_sourceFilePath );

        Concrete::Root* pConcreteRoot
            = database.construct< Root >( Root::Args{ ContextGroup::Args{ {} }, pInterfaceRoot } );

        for ( Interface::IContext* pChildContext : pInterfaceRoot->get_children() )
        {
            recurse( database, pConcreteRoot, pChildContext );
        }

        const task::FileHash buildHash = database.save_Concrete_to_temp();
        m_environment.temp_to_real( concreteFile );
        m_environment.setBuildHashCode( concreteFile, buildHash );
        m_environment.stash( concreteFile, determinant );

        succeeded( taskProgress );
    }

    const mega::io::megaFilePath& m_sourceFilePath;
};

BaseTask::Ptr create_Task_ConcreteTree( const TaskArguments&          taskArguments,
                                        const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_ConcreteTree >( taskArguments, sourceFilePath );
}

} // namespace compiler
} // namespace mega
