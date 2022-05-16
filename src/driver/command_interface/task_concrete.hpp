#ifndef TASK_CONCRETE_12_MAY_2022
#define TASK_CONCRETE_12_MAY_2022

#include "base_task.hpp"

#include "database/model/ConcreteStage.hxx"
#include <common/stash.hpp>

namespace driver
{
namespace interface
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
        using ContextVector = std::vector< ConcreteStage::Interface::Context* >;
        ContextVector inherited;
        ContextVector contexts;

        using DimensionVector = std::vector< ConcreteStage::Interface::DimensionTrait* >;
        DimensionVector dimensions;

        void addContext( ConcreteStage::Interface::Context* pContext )
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

    template< typename TContextType >
    void collectDimensions( TContextType* pInterfaceContext, IdentifierMap& identifierMap )
    {
        using namespace ConcreteStage;
        VERIFY_RTE( pInterfaceContext );
        for( Interface::DimensionTrait* pDimension : pInterfaceContext->get_dimension_traits() )
        {
            identifierMap.addDimension( pDimension );
        }
    }

    template< typename TContextType >
    void collectContexts( TContextType* pInterfaceContext, IdentifierMap& identifierMap )
    {
        using namespace ConcreteStage;
        VERIFY_RTE( pInterfaceContext );
        for( Interface::Context* pContext : pInterfaceContext->get_children() )
        {
            identifierMap.addContext( pContext );
        }
    }

    void recurseInheritance( ConcreteStage::Database& database, ConcreteStage::Interface::Context* pContext,
                             IdentifierMap& identifierMap )
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
                for ( Interface::Context* pInheritedContext : inheritanceOpt.value()->get_contexts() )
                {
                    recurseInheritance( database, pInheritedContext, identifierMap );
                }
            }
        }
        else if ( Interface::Action* pAction = dynamic_database_cast< Interface::Action >( pContext ) )
        {
            collectDimensions( pAction, identifierMap );
            if ( std::optional< Interface::InheritanceTrait* > inheritanceOpt = pAction->get_inheritance_trait() )
            {
                for ( Interface::Context* pInheritedContext : inheritanceOpt.value()->get_contexts() )
                {
                    recurseInheritance( database, pInheritedContext, identifierMap );
                }
            }
        }
        else if ( Interface::Event* pEvent = dynamic_database_cast< Interface::Event >( pContext ) )
        {
            collectDimensions( pEvent, identifierMap );
            if ( std::optional< Interface::InheritanceTrait* > inheritanceOpt = pEvent->get_inheritance_trait() )
            {
                for ( Interface::Context* pInheritedContext : inheritanceOpt.value()->get_contexts() )
                {
                    recurseInheritance( database, pInheritedContext, identifierMap );
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
                for ( Interface::Context* pInheritedContext : inheritanceOpt.value()->get_contexts() )
                {
                    recurseInheritance( database, pInheritedContext, identifierMap );
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
        else
        {
            THROW_RTE( "Unknown context type" );
        }
    }

    void constructElements( ConcreteStage::Database& database, const IdentifierMap& inheritedContexts,
                            std::vector< ConcreteStage::Concrete::Context* >&   childContexts,
                            std::vector< ConcreteStage::Concrete::Dimension* >& dimensions )
    {
        using namespace ConcreteStage;
        using namespace ConcreteStage::Concrete;

        for ( Interface::Context* pChildContext : inheritedContexts.contexts )
        {
            if( Context* pContext = recurse( database, pChildContext ) )
                childContexts.push_back( pContext );
        }

        for ( Interface::DimensionTrait* pInterfaceDimension : inheritedContexts.dimensions )
        {
            Dimension* pConcreteDimension = database.construct< Dimension >( Dimension::Args{ pInterfaceDimension } );
            dimensions.push_back( pConcreteDimension );
        }
    }

    ConcreteStage::Concrete::Context* recurse( ConcreteStage::Database&           database,
                                               ConcreteStage::Interface::Context* pContext )
    {
        using namespace ConcreteStage;
        using namespace ConcreteStage::Concrete;

        if ( Interface::Namespace* pNamespace = dynamic_database_cast< Interface::Namespace >( pContext ) )
        {
            IdentifierMap inheritedContexts;
            recurseInheritance( database, pNamespace, inheritedContexts );

            std::vector< ConcreteStage::Concrete::Context* >   childContexts;
            std::vector< ConcreteStage::Concrete::Dimension* > dimensions;
            constructElements( database, inheritedContexts, childContexts, dimensions );

            Namespace* pConcrete = database.construct< Namespace >(
                Namespace::Args{ Context::Args{ inheritedContexts.inherited, childContexts }, pNamespace, dimensions } );
            return pConcrete;
        }
        else if ( Interface::Abstract* pAbstract = dynamic_database_cast< Interface::Abstract >( pContext ) )
        {
            // do nothing
            return nullptr;
        }
        else if ( Interface::Action* pAction = dynamic_database_cast< Interface::Action >( pContext ) )
        {
            IdentifierMap inheritedContexts;
            recurseInheritance( database, pAction, inheritedContexts );

            std::vector< ConcreteStage::Concrete::Context* >   childContexts;
            std::vector< ConcreteStage::Concrete::Dimension* > dimensions;
            constructElements( database, inheritedContexts, childContexts, dimensions );

            Action* pConcrete = database.construct< Action >(
                Action::Args{ Context::Args{ inheritedContexts.inherited, childContexts }, pAction, dimensions } );
            return pConcrete;
        }
        else if ( Interface::Event* pEvent = dynamic_database_cast< Interface::Event >( pContext ) )
        {
            IdentifierMap inheritedContexts;
            recurseInheritance( database, pEvent, inheritedContexts );

            std::vector< ConcreteStage::Concrete::Context* >   childContexts;
            std::vector< ConcreteStage::Concrete::Dimension* > dimensions;
            constructElements( database, inheritedContexts, childContexts, dimensions );

            Event* pConcrete = database.construct< Event >(
                Event::Args{ Context::Args{ inheritedContexts.inherited, childContexts }, pEvent, dimensions } );
            return pConcrete;
        }
        else if ( Interface::Function* pFunction = dynamic_database_cast< Interface::Function >( pContext ) )
        {
            IdentifierMap inheritedContexts;
            recurseInheritance( database, pFunction, inheritedContexts );

            std::vector< ConcreteStage::Concrete::Context* >   childContexts;
            std::vector< ConcreteStage::Concrete::Dimension* > dimensions;
            constructElements( database, inheritedContexts, childContexts, dimensions );
            VERIFY_RTE( dimensions.empty() );

            Function* pConcrete = database.construct< Function >(
                Function::Args{ Context::Args{ inheritedContexts.inherited, childContexts }, pFunction } );
            return pConcrete;
        }
        else if ( Interface::Object* pObject = dynamic_database_cast< Interface::Object >( pContext ) )
        {
            IdentifierMap inheritedContexts;
            recurseInheritance( database, pObject, inheritedContexts );

            std::vector< ConcreteStage::Concrete::Context* >   childContexts;
            std::vector< ConcreteStage::Concrete::Dimension* > dimensions;
            constructElements( database, inheritedContexts, childContexts, dimensions );

            Object* pConcrete = database.construct< Object >(
                Object::Args{ Context::Args{ inheritedContexts.inherited, childContexts }, pObject, dimensions } );
            return pConcrete;
        }
        else if ( Interface::Link* pLink = dynamic_database_cast< Interface::Link >( pContext ) )
        {
            IdentifierMap inheritedContexts;
            recurseInheritance( database, pLink, inheritedContexts );

            std::vector< ConcreteStage::Concrete::Context* >   childContexts;
            std::vector< ConcreteStage::Concrete::Dimension* > dimensions;
            constructElements( database, inheritedContexts, childContexts, dimensions );
            VERIFY_RTE( dimensions.empty() );

            Link* pConcrete = database.construct< Link >(
                Link::Args{ Context::Args{ inheritedContexts.inherited, childContexts }, pLink } );
            return pConcrete;
        }
        else if ( Interface::Table* pTable = dynamic_database_cast< Interface::Table >( pContext ) )
        {
            IdentifierMap inheritedContexts;
            recurseInheritance( database, pTable, inheritedContexts );

            std::vector< ConcreteStage::Concrete::Context* >   childContexts;
            std::vector< ConcreteStage::Concrete::Dimension* > dimensions;
            constructElements( database, inheritedContexts, childContexts, dimensions );
            VERIFY_RTE( dimensions.empty() );

            Table* pConcrete = database.construct< Table >(
                Table::Args{ Context::Args{ inheritedContexts.inherited, childContexts }, pTable } );
            return pConcrete;
        }
        else
        {
            THROW_RTE( "Unknown context type" );
        }
    }

    virtual void run( task::Progress& taskProgress )
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

        Interface::Root* pRoot = database.one< Interface::Root >( m_sourceFilePath );

        std::vector< Context* > contexts;
        for ( Interface::Context* pChildContext : pRoot->get_children() )
        {
            if( Context* pConcreteContext = recurse( database, pChildContext ) )
                contexts.push_back( pConcreteContext );
        }

        database.construct< Root >( Root::Args{ Context::Args{ {}, contexts } } );

        const task::FileHash buildHash = database.save_Concrete_to_temp();
        m_environment.temp_to_real( concreteFile );
        m_environment.setBuildHashCode( concreteFile, buildHash );
        m_environment.stash( concreteFile, determinant );

        succeeded( taskProgress );
    }

    const mega::io::megaFilePath& m_sourceFilePath;
};

} // namespace interface
} // namespace driver

#endif // TASK_CONCRETE_12_MAY_2022
