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

#include "database/model/ConcreteStage.hxx"
#include <common/stash.hpp>
#include <optional>

namespace mega::compiler
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

        using LinkVector = std::vector< ConcreteStage::Interface::LinkTrait* >;
        LinkVector links;

        void addContext( ConcreteStage::Interface::IContext* pContext )
        {
            const std::string& strIdentifier = pContext->get_identifier();
            if( !identifiers.count( strIdentifier ) )
            {
                contexts.push_back( pContext );
                identifiers.insert( strIdentifier );
            }
        }
        void addDimension( ConcreteStage::Interface::DimensionTrait* pDimension )
        {
            const std::string& strIdentifier = pDimension->get_id()->get_str();
            if( !identifiers.count( strIdentifier ) )
            {
                dimensions.push_back( pDimension );
                identifiers.insert( strIdentifier );
            }
        }
        void addLink( ConcreteStage::Interface::LinkTrait* pLink )
        {
            const std::string& strIdentifier = pLink->get_id()->get_str();
            if( !identifiers.count( strIdentifier ) )
            {
                links.push_back( pLink );
                identifiers.insert( strIdentifier );
            }
        }
    };

    template < typename TContextType >
    void collectDimensions( ConcreteStage::Database& database, TContextType* pInterfaceContext,
                            IdentifierMap& identifierMap )
    {
        using namespace ConcreteStage;
        VERIFY_RTE( pInterfaceContext );
        for( Interface::DimensionTrait* pDimension : pInterfaceContext->get_dimension_traits() )
        {
            identifierMap.addDimension( pDimension );
        }
    }

    template < typename TContextType >
    void collectLinks( ConcreteStage::Database& database, TContextType* pInterfaceContext,
                       IdentifierMap& identifierMap )
    {
        using namespace ConcreteStage;
        VERIFY_RTE( pInterfaceContext );
        for( Interface::LinkTrait* pLink : pInterfaceContext->get_link_traits() )
        {
            identifierMap.addLink( pLink );
        }
    }

    template < typename TContextType >
    void collectContexts( TContextType* pInterfaceContext, IdentifierMap& identifierMap )
    {
        using namespace ConcreteStage;
        VERIFY_RTE( pInterfaceContext );
        for( Interface::IContext* pContext : pInterfaceContext->get_children() )
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

        if( auto pNamespace = db_cast< Interface::Namespace >( pContext ) )
        {
            // do nothing
            collectDimensions( database, pNamespace, identifierMap );
        }
        else if( auto pAbstract = db_cast< Interface::Abstract >( pContext ) )
        {
            collectDimensions( database, pAbstract, identifierMap );
            collectLinks( database, pAbstract, identifierMap );
            if( std::optional< Interface::InheritanceTrait* > inheritanceOpt = pAbstract->get_inheritance_trait() )
            {
                for( Interface::IContext* pInheritedContext : inheritanceOpt.value()->get_contexts() )
                {
                    recurseInheritance( database, pConcreteRoot, pInheritedContext, identifierMap );
                }
            }
        }
        else if( auto pState = db_cast< Interface::State >( pContext ) )
        {
            collectDimensions( database, pState, identifierMap );
            collectLinks( database, pState, identifierMap );
            if( std::optional< Interface::InheritanceTrait* > inheritanceOpt = pState->get_inheritance_trait() )
            {
                for( Interface::IContext* pInheritedContext : inheritanceOpt.value()->get_contexts() )
                {
                    recurseInheritance( database, pConcreteRoot, pInheritedContext, identifierMap );
                }
            }
        }
        else if( auto pEvent = db_cast< Interface::Event >( pContext ) )
        {
            collectDimensions( database, pEvent, identifierMap );
            if( std::optional< Interface::InheritanceTrait* > inheritanceOpt = pEvent->get_inheritance_trait() )
            {
                for( Interface::IContext* pInheritedContext : inheritanceOpt.value()->get_contexts() )
                {
                    recurseInheritance( database, pConcreteRoot, pInheritedContext, identifierMap );
                }
            }
        }
        else if( auto pInterupt = db_cast< Interface::Interupt >( pContext ) )
        {
            // do nothing
        }
        else if( auto pFunction = db_cast< Interface::Function >( pContext ) )
        {
            // do nothing
        }
        else if( auto pObject = db_cast< Interface::Object >( pContext ) )
        {
            collectDimensions( database, pObject, identifierMap );
            collectLinks( database, pObject, identifierMap );
            if( std::optional< Interface::InheritanceTrait* > inheritanceOpt = pObject->get_inheritance_trait() )
            {
                for( Interface::IContext* pInheritedContext : inheritanceOpt.value()->get_contexts() )
                {
                    recurseInheritance( database, pConcreteRoot, pInheritedContext, identifierMap );
                }
            }
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
                            std::vector< ConcreteStage::Concrete::Dimensions::User* >& dimensions,
                            std::vector< ConcreteStage::Concrete::Dimensions::Link* >& links,
                            std::optional< ConcreteStage::Concrete::Object* >&         concreteObjectOpt,
                            ConcreteStage::Components::Component*                      pComponent )
    {
        using namespace ConcreteStage;
        using namespace ConcreteStage::Concrete;

        for( Interface::IContext* pChildContext : inheritedContexts.contexts )
        {
            if( Context* pContext
                = recurse( database, parentConcreteContextGroup, pChildContext, concreteObjectOpt, pComponent ) )
            {
                childContexts.push_back( pContext );
            }
        }

        for( Interface::DimensionTrait* pInterfaceDimension : inheritedContexts.dimensions )
        {
            auto              pParentConcreteContext = db_cast< Concrete::Context >( parentConcreteContextGroup );
            Dimensions::User* pConcreteDimension     = database.construct< Dimensions::User >(
                Dimensions::User::Args{ pParentConcreteContext, pInterfaceDimension } );
            dimensions.push_back( pConcreteDimension );
        }

        for( Interface::LinkTrait* pInterfaceLink : inheritedContexts.links )
        {
            auto              pParentConcreteContext = db_cast< Concrete::Context >( parentConcreteContextGroup );
            Dimensions::Link* pConcreteLink          = database.construct< Dimensions::Link >(
                Dimensions::Link::Args{ pParentConcreteContext, pInterfaceLink } );
            links.push_back( pConcreteLink );
        }
    }

    ConcreteStage::Concrete::Context* recurse( ConcreteStage::Database&                           database,
                                               ConcreteStage::Concrete::ContextGroup*             pParentContextGroup,
                                               ConcreteStage::Interface::IContext*                pContext,
                                               std::optional< ConcreteStage::Concrete::Object* >& concreteObjectOpt,
                                               ConcreteStage::Components::Component*              pComponent )
    {
        using namespace ConcreteStage;
        using namespace ConcreteStage::Concrete;

        if( auto pNamespace = db_cast< Interface::Namespace >( pContext ) )
        {
            Namespace* pConcrete = database.construct< Namespace >( Namespace::Args{
                UserDimensionContext::Args{
                    Context::Args{ ContextGroup::Args{ {} }, pComponent, pParentContextGroup, pNamespace, {} },
                    {},
                    {} },
                pNamespace } );
            pParentContextGroup->push_back_children( pConcrete );
            pConcrete->set_concrete_object( concreteObjectOpt );

            IdentifierMap inheritedContexts;
            {
                recurseInheritance( database, pConcrete, pNamespace, inheritedContexts );
                pConcrete->set_inheritance( inheritedContexts.inherited );
            }

            std::vector< ConcreteStage::Concrete::Context* >          childContexts;
            std::vector< ConcreteStage::Concrete::Dimensions::User* > dimensions;
            std::vector< ConcreteStage::Concrete::Dimensions::Link* > links;
            constructElements( database, pConcrete, inheritedContexts, childContexts, dimensions, links,
                               concreteObjectOpt, pComponent );

            VERIFY_RTE( links.empty() );

            pConcrete->set_dimensions( dimensions );
            pConcrete->set_children( childContexts );

            return pConcrete;
        }
        else if( auto pAbstract = db_cast< Interface::Abstract >( pContext ) )
        {
            // do nothing
            return nullptr;
        }
        else if( auto pState = db_cast< Interface::State >( pContext ) )
        {
            if( concreteObjectOpt.has_value() )
            {
                State* pConcrete = nullptr;

                if( auto pAction = db_cast< Interface::Action >( pState ) )
                {
                    // clang-format off
                    pConcrete = database.construct< Action >
                    (
                        Action::Args
                        { 
                            State::Args
                            {
                                UserDimensionContext::Args
                                {
                                    Context::Args
                                    { 
                                        ContextGroup::Args{ {} },
                                        pComponent, 
                                        pParentContextGroup, 
                                        pAction, 
                                        {} 
                                    },
                                    {},
                                    {}
                                },
                                pState
                            },
                            pAction 
                        } 
                    );
                    // clang-format on
                }
                else if( auto pInterfaceComponent = db_cast< Interface::Component >( pState ) )
                {
                    // clang-format off
                    pConcrete = database.construct< Component >
                    ( 
                        Component::Args
                        {
                            State::Args
                            {
                                UserDimensionContext::Args
                                {
                                    Context::Args
                                    {
                                        ContextGroup::Args{ {} },
                                        pComponent,
                                        pParentContextGroup,
                                        pInterfaceComponent,
                                        {} 
                                    },
                                    {},
                                    {} 
                                },
                                pState 
                            },
                            pInterfaceComponent 
                        }
                    );
                    // clang-format on
                }
                else
                {
                    pConcrete = database.construct< State >( State::Args{
                        UserDimensionContext::Args{
                            Context::Args{ ContextGroup::Args{ {} }, pComponent, pParentContextGroup, pAction, {} },
                            {},
                            {} },
                        pAction } );
                }

                pParentContextGroup->push_back_children( pConcrete );
                pConcrete->set_concrete_object( concreteObjectOpt );

                IdentifierMap inheritedContexts;
                {
                    recurseInheritance( database, pConcrete, pState, inheritedContexts );
                    pConcrete->set_inheritance( inheritedContexts.inherited );
                }

                std::vector< ConcreteStage::Concrete::Context* >          childContexts;
                std::vector< ConcreteStage::Concrete::Dimensions::User* > dimensions;
                std::vector< ConcreteStage::Concrete::Dimensions::Link* > links;
                constructElements( database, pConcrete, inheritedContexts, childContexts, dimensions, links,
                                   concreteObjectOpt, pComponent );

                pConcrete->set_dimensions( dimensions );
                pConcrete->set_links( links );
                pConcrete->set_children( childContexts );

                return pConcrete;
            }
            else
            {
                return nullptr;
            }
        }
        else if( auto pEvent = db_cast< Interface::Event >( pContext ) )
        {
            if( concreteObjectOpt.has_value() )
            {
                Event* pConcrete = database.construct< Event >( Event::Args{
                    UserDimensionContext::Args{
                        Context::Args{ ContextGroup::Args{ {} }, pComponent, pParentContextGroup, pEvent, {} },
                        {},
                        {} },
                    pEvent } );
                pParentContextGroup->push_back_children( pConcrete );
                pConcrete->set_concrete_object( concreteObjectOpt );

                IdentifierMap inheritedContexts;
                {
                    recurseInheritance( database, pConcrete, pEvent, inheritedContexts );
                    pConcrete->set_inheritance( inheritedContexts.inherited );
                }

                std::vector< ConcreteStage::Concrete::Context* >          childContexts;
                std::vector< ConcreteStage::Concrete::Dimensions::User* > dimensions;
                std::vector< ConcreteStage::Concrete::Dimensions::Link* > links;
                constructElements( database, pConcrete, inheritedContexts, childContexts, dimensions, links,
                                   concreteObjectOpt, pComponent );

                VERIFY_RTE( links.empty() );

                pConcrete->set_dimensions( dimensions );
                pConcrete->set_children( childContexts );

                return pConcrete;
            }
            else
            {
                return nullptr;
            }
        }
        else if( auto pInterupt = db_cast< Interface::Interupt >( pContext ) )
        {
            if( concreteObjectOpt.has_value() )
            {
                Interupt* pConcrete = database.construct< Interupt >( Interupt::Args{
                    Context::Args{ ContextGroup::Args{ {} }, pComponent, pParentContextGroup, pInterupt, {} },
                    pInterupt } );
                pParentContextGroup->push_back_children( pConcrete );
                pConcrete->set_concrete_object( concreteObjectOpt );

                IdentifierMap inheritedContexts;
                {
                    recurseInheritance( database, pConcrete, pInterupt, inheritedContexts );
                    pConcrete->set_inheritance( inheritedContexts.inherited );
                }

                std::vector< ConcreteStage::Concrete::Context* >          childContexts;
                std::vector< ConcreteStage::Concrete::Dimensions::User* > dimensions;
                std::vector< ConcreteStage::Concrete::Dimensions::Link* > links;
                constructElements( database, pConcrete, inheritedContexts, childContexts, dimensions, links,
                                   concreteObjectOpt, pComponent );
                VERIFY_RTE( dimensions.empty() );
                VERIFY_RTE( links.empty() );

                pConcrete->set_children( childContexts );

                return pConcrete;
            }
            else
            {
                return nullptr;
            }
        }
        else if( auto pFunction = db_cast< Interface::Function >( pContext ) )
        {
            if( concreteObjectOpt.has_value() )
            {
                Function* pConcrete = database.construct< Function >( Function::Args{
                    Context::Args{ ContextGroup::Args{ {} }, pComponent, pParentContextGroup, pFunction, {} },
                    pFunction } );
                pParentContextGroup->push_back_children( pConcrete );
                pConcrete->set_concrete_object( concreteObjectOpt );

                IdentifierMap inheritedContexts;
                {
                    recurseInheritance( database, pConcrete, pFunction, inheritedContexts );
                    pConcrete->set_inheritance( inheritedContexts.inherited );
                }

                std::vector< ConcreteStage::Concrete::Context* >          childContexts;
                std::vector< ConcreteStage::Concrete::Dimensions::User* > dimensions;
                std::vector< ConcreteStage::Concrete::Dimensions::Link* > links;
                constructElements( database, pConcrete, inheritedContexts, childContexts, dimensions, links,
                                   concreteObjectOpt, pComponent );
                VERIFY_RTE( dimensions.empty() );
                VERIFY_RTE( links.empty() );

                pConcrete->set_children( childContexts );

                return pConcrete;
            }
            else
            {
                return nullptr;
            }
        }
        else if( auto pObject = db_cast< Interface::Object >( pContext ) )
        {
            Object* pConcrete = database.construct< Object >( Object::Args{
                UserDimensionContext::Args{
                    Context::Args{ ContextGroup::Args{ {} }, pComponent, pParentContextGroup, pObject, {} }, {}, {} },
                pObject } );
            pParentContextGroup->push_back_children( pConcrete );
            concreteObjectOpt = pConcrete;
            pConcrete->set_concrete_object( concreteObjectOpt );

            IdentifierMap inheritedContexts;
            {
                recurseInheritance( database, pConcrete, pObject, inheritedContexts );
                pConcrete->set_inheritance( inheritedContexts.inherited );
            }

            std::vector< ConcreteStage::Concrete::Context* >          childContexts;
            std::vector< ConcreteStage::Concrete::Dimensions::User* > dimensions;
            std::vector< ConcreteStage::Concrete::Dimensions::Link* > links;
            constructElements( database, pConcrete, inheritedContexts, childContexts, dimensions, links,
                               concreteObjectOpt, pComponent );

            pConcrete->set_dimensions( dimensions );
            pConcrete->set_links( links );
            pConcrete->set_children( childContexts );

            return pConcrete;
        }
        else
        {
            THROW_RTE( "Unknown context type" );
        }
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath interfaceTreeFile = m_environment.InterfaceStage_Tree( m_sourceFilePath );
        const mega::io::CompilationFilePath interfaceAnalysisFile
            = m_environment.InterfaceAnalysisStage_Clang( m_sourceFilePath );
        const mega::io::CompilationFilePath concreteFile = m_environment.ConcreteStage_Concrete( m_sourceFilePath );
        start( taskProgress, "Task_ConcreteTree", m_sourceFilePath.path(), concreteFile.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash,
              m_environment.getBuildHashCode( m_environment.ParserStage_AST( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( interfaceTreeFile ),
              m_environment.getBuildHashCode( interfaceAnalysisFile ) } );

        if( m_environment.restore( concreteFile, determinant ) )
        {
            m_environment.setBuildHashCode( concreteFile );
            cached( taskProgress );
            return;
        }

        using namespace ConcreteStage;
        using namespace ConcreteStage::Concrete;

        Database database( m_environment, m_sourceFilePath );

        Interface::Root*       pInterfaceRoot = database.one< Interface::Root >( m_sourceFilePath );
        Components::Component* pComponent     = pInterfaceRoot->get_root()->get_component();
        VERIFY_RTE( pComponent );

        Concrete::Root* pConcreteRoot
            = database.construct< Root >( Root::Args{ ContextGroup::Args{ {} }, pInterfaceRoot } );
        std::optional< Concrete::Object* > concreteObjectOpt;

        for( Interface::IContext* pChildContext : pInterfaceRoot->get_children() )
        {
            recurse( database, pConcreteRoot, pChildContext, concreteObjectOpt, pComponent );
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

} // namespace mega::compiler
