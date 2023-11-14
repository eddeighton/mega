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

#include "mega/common_strings.hpp"

#include "database/ConcreteStage.hxx"

#include <common/stash.hpp>

#include <optional>

namespace ConcreteStage
{
#include "compiler/interface_printer.hpp"
} // namespace ConcreteStage

namespace mega::compiler
{

using namespace ConcreteStage;

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
        using ContextVector = std::vector< Interface::IContext* >;
        ContextVector inherited;
        ContextVector contexts;

        using DimensionVector = std::vector< Interface::DimensionTrait* >;
        DimensionVector dimensions;

        using LinkVector = std::vector< Interface::LinkTrait* >;
        LinkVector links;

        void addContext( Interface::IContext* pContext )
        {
            const std::string& strIdentifier = Interface::getIdentifier( pContext );
            if( !identifiers.count( strIdentifier ) )
            {
                contexts.push_back( pContext );
                identifiers.insert( strIdentifier );
            }
        }
        void addDimension( Interface::DimensionTrait* pDimension )
        {
            const std::string& strIdentifier = Interface::getIdentifier( pDimension );
            if( !identifiers.count( strIdentifier ) )
            {
                dimensions.push_back( pDimension );
                identifiers.insert( strIdentifier );
            }
        }
        void addLink( Interface::LinkTrait* pLink )
        {
            const std::string& strIdentifier = Interface::getIdentifier( pLink );
            if( !identifiers.count( strIdentifier ) )
            {
                links.push_back( pLink );
                identifiers.insert( strIdentifier );
            }
        }
    };

    template < typename TContextType >
    void collectDimensions( Database& database, TContextType* pInterfaceContext, IdentifierMap& identifierMap )
    {
        VERIFY_RTE( pInterfaceContext );
        for( Interface::DimensionTrait* pDimension : pInterfaceContext->get_dimension_traits() )
        {
            identifierMap.addDimension( pDimension );
        }
    }

    template < typename TContextType >
    void collectLinks( Database& database, TContextType* pInterfaceContext, IdentifierMap& identifierMap )
    {
        VERIFY_RTE( pInterfaceContext );
        for( Interface::LinkTrait* pLink : pInterfaceContext->get_link_traits() )
        {
            identifierMap.addLink( pLink );
        }
    }

    template < typename TContextType >
    void collectContexts( TContextType* pInterfaceContext, IdentifierMap& identifierMap )
    {
        VERIFY_RTE( pInterfaceContext );
        for( Interface::IContext* pContext : pInterfaceContext->get_children() )
        {
            identifierMap.addContext( pContext );
        }
    }

    void recurseInheritance( Database& database, Concrete::Context* pConcreteRoot, Interface::IContext* pContext,
                             IdentifierMap& identifierMap )
    {
        using namespace Concrete;

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
            if( std::optional< Interface::InheritanceTrait* > inheritanceOpt = pAbstract->get_inheritance_trait_opt() )
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
            if( std::optional< Interface::InheritanceTrait* > inheritanceOpt = pState->get_inheritance_trait_opt() )
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
            if( std::optional< Interface::InheritanceTrait* > inheritanceOpt = pEvent->get_inheritance_trait_opt() )
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
        else if( auto pDecider = db_cast< Interface::Decider >( pContext ) )
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
            if( std::optional< Interface::InheritanceTrait* > inheritanceOpt = pObject->get_inheritance_trait_opt() )
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

    struct ContextElements
    {
        std::vector< Concrete::Context* >            childContexts;
        std::vector< Concrete::Dimensions::User* >   dimensions;
        std::vector< Concrete::Dimensions::Link* >   links;
        std::vector< Concrete::Dimensions::Bitset* > bitsets;
        std::vector< Concrete::Requirement* >        requirements;

        // object only
        Concrete::Dimensions::OwnershipLink* pOwnershipLink = nullptr;
        Concrete::Dimensions::Configuration* pConfiguration = nullptr;
        Concrete::Dimensions::Activation*    pActivation    = nullptr;
        Concrete::Dimensions::Enablement*    pEnablement    = nullptr;
        Concrete::Dimensions::History*       pHistory       = nullptr;
    };

    ContextElements constructElements( Database&                           database,
                                       Concrete::ContextGroup*             parentConcreteContextGroup,
                                       const IdentifierMap&                inheritedContexts,
                                       std::optional< Concrete::Object* >& concreteObjectOpt,
                                       Components::Component*              pComponent )
    {
        using namespace Concrete;

        VERIFY_RTE( pComponent );

        ContextElements elements;

        // contexts
        {
            for( Interface::IContext* pChildContext : inheritedContexts.contexts )
            {
                if( Context* pContext
                    = recurse( database, parentConcreteContextGroup, pChildContext, concreteObjectOpt, pComponent ) )
                {
                    elements.childContexts.push_back( pContext );

                    if( auto pRequirement = db_cast< Requirement >( pContext ) )
                    {
                        elements.requirements.push_back( pRequirement );
                    }
                }
            }
        }

        // user dimensions
        {
            for( Interface::DimensionTrait* pInterfaceDimension : inheritedContexts.dimensions )
            {
                if( auto pUserDimensionTrait = db_cast< Interface::UserDimensionTrait >( pInterfaceDimension ) )
                {
                    ASSERT( pUserDimensionTrait->get_parser_dimension()->get_id()->get_str()
                            != mega::EG_CONFIGURATION );

                    auto pParentConcreteContext          = db_cast< Concrete::Context >( parentConcreteContextGroup );
                    Dimensions::User* pConcreteDimension = database.construct< Dimensions::User >(
                        Dimensions::User::Args{ Concrete::Graph::Vertex::Args{ pComponent }, pParentConcreteContext,
                                                pInterfaceDimension } );
                    elements.dimensions.push_back( pConcreteDimension );
                }
                else if( auto pCompilerDimensionTrait
                         = db_cast< Interface::CompilerDimensionTrait >( pInterfaceDimension ) )
                {
                    // bitsets
                    if( auto pObject = db_cast< Object >( parentConcreteContextGroup ) )
                    {
                        if( pCompilerDimensionTrait->get_identifier() == mega::EG_CONFIGURATION )
                        {
                            elements.pConfiguration = database.construct< Dimensions::Configuration >(
                                Dimensions::Configuration ::Args{ Dimensions::Bitset::Args{
                                    Concrete::Graph::Vertex::Args{ pComponent }, pObject, pCompilerDimensionTrait } } );
                            elements.bitsets.push_back( elements.pConfiguration );
                        }
                        else if( pCompilerDimensionTrait->get_identifier() == mega::EG_STATE )
                        {
                            elements.pActivation = database.construct< Dimensions::Activation >(
                                Dimensions::Activation ::Args{ Dimensions::Bitset::Args{
                                    Concrete::Graph::Vertex::Args{ pComponent }, pObject, pCompilerDimensionTrait } } );
                            elements.bitsets.push_back( elements.pActivation );
                        }
                        else if( pCompilerDimensionTrait->get_identifier() == mega::EG_ENABLEMENT )
                        {
                            elements.pEnablement = database.construct< Dimensions::Enablement >(
                                Dimensions::Enablement ::Args{ Dimensions::Bitset::Args{
                                    Concrete::Graph::Vertex::Args{ pComponent }, pObject, pCompilerDimensionTrait } } );
                            elements.bitsets.push_back( elements.pEnablement );
                        }
                        else if( pCompilerDimensionTrait->get_identifier() == mega::EG_HISTORY )
                        {
                            elements.pHistory = database.construct< Dimensions::History >(
                                Dimensions::History ::Args{ Dimensions::Bitset::Args{
                                    Concrete::Graph::Vertex::Args{ pComponent }, pObject, pCompilerDimensionTrait } } );
                            elements.bitsets.push_back( elements.pHistory );
                        }
                        else
                        {
                            THROW_RTE( "Unknown compiler object dimension trait" );
                        }
                    }
                    else
                    {
                        THROW_RTE( "Unknown compiler dimension trait" );
                    }
                }
                else
                {
                    THROW_RTE( "Unknown dimension trait type" );
                }
            }
        }

        // links
        {
            for( Interface::LinkTrait* pInterfaceLink : inheritedContexts.links )
            {
                if( auto pUserLink = db_cast< Interface::UserLinkTrait >( pInterfaceLink ) )
                {
                    auto pParentConcreteContext = db_cast< Concrete::Context >( parentConcreteContextGroup );

                    Dimensions::UserLink* pConcreteLink = database.construct< Dimensions::UserLink >(
                        Dimensions::UserLink::Args{ Dimensions::Link::Args{ Concrete::Graph::Vertex::Args{ pComponent },
                                                                            pParentConcreteContext, pInterfaceLink },
                                                    pUserLink } );

                    auto pLinkType = database.construct< Dimensions::LinkType >(
                        Dimensions::LinkType::Args{ pParentConcreteContext, pConcreteLink } );
                    pConcreteLink->set_link_type( pLinkType );

                    elements.links.push_back( pConcreteLink );
                }
                else if( auto pOwnerLink = db_cast< Interface::OwnershipLinkTrait >( pInterfaceLink ) )
                {
                    auto pObject = db_cast< Object >( parentConcreteContextGroup );
                    VERIFY_RTE_MSG( pObject, "Owner link trait NOT in object" );
                    elements.pOwnershipLink
                        = database.construct< Dimensions::OwnershipLink >( Dimensions::OwnershipLink::Args{
                            Dimensions::Link::Args{
                                Concrete::Graph::Vertex::Args{ pComponent }, pObject, pInterfaceLink },
                            pOwnerLink } );

                    auto pLinkType = database.construct< Dimensions::LinkType >(
                        Dimensions::LinkType::Args{ pObject, elements.pOwnershipLink } );
                    elements.pOwnershipLink->set_link_type( pLinkType );

                    elements.links.push_back( elements.pOwnershipLink );
                }
                else
                {
                    THROW_RTE( "Unknown link trait type" );
                }
            }
        }

        return elements;
    }

    Concrete::Context* recurse( Database&                           database,
                                Concrete::ContextGroup*             pParentContextGroup,
                                Interface::IContext*                pContext,
                                std::optional< Concrete::Object* >& concreteObjectOpt,
                                Components::Component*              pComponent )
    {
        using namespace Concrete;

        VERIFY_RTE( pComponent );

        if( auto pNamespace = db_cast< Interface::Namespace >( pContext ) )
        {
            Namespace* pConcrete = database.construct< Namespace >( Namespace::Args{
                UserDimensionContext::Args{
                    Context::Args{ ContextGroup::Args{ Concrete::Graph::Vertex::Args{ pComponent }, {} },
                                   pParentContextGroup,
                                   pNamespace,
                                   {} },
                    {},
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

            ContextElements elements
                = constructElements( database, pConcrete, inheritedContexts, concreteObjectOpt, pComponent );

            VERIFY_RTE( elements.links.empty() );
            VERIFY_RTE_MSG( elements.links.empty(),
                            "Namespace cannot have links: " << Interface::printIContextFullType( pNamespace ) );
            VERIFY_RTE_MSG( elements.requirements.empty(),
                            "Namespace cannot have requirements: " << Interface::printIContextFullType( pNamespace ) );

            pConcrete->set_dimensions( elements.dimensions );
            pConcrete->set_children( elements.childContexts );

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
                    pConcrete = database.construct< Action >( Action::Args{
                        State::Args{
                            UserDimensionContext::Args{
                                Context::Args{ ContextGroup::Args{ Concrete::Graph::Vertex::Args{ pComponent }, {} },
                                               pParentContextGroup,
                                               pAction,
                                               {} },
                                {},
                                {},
                                {} },
                            pState,
                            {},
                            {} },
                        pAction } );
                }
                else if( auto pInterfaceComponent = db_cast< Interface::Component >( pState ) )
                {
                    pConcrete = database.construct< Component >( Component::Args{
                        State::Args{
                            UserDimensionContext::Args{
                                Context::Args{ ContextGroup::Args{ Concrete::Graph::Vertex::Args{ pComponent }, {} },
                                               pParentContextGroup,
                                               pInterfaceComponent,
                                               {} },
                                {},
                                {},
                                {} },
                            pState,
                            {},
                            {} },
                        pInterfaceComponent } );
                }
                else
                {
                    pConcrete = database.construct< State >( State::Args{
                        UserDimensionContext::Args{
                            Context::Args{ ContextGroup::Args{ Concrete::Graph::Vertex::Args{ pComponent }, {} },
                                           pParentContextGroup,
                                           pState,
                                           {} },
                            {},
                            {},
                            {} },
                        pState,
                        {},
                        {} } );
                }

                pParentContextGroup->push_back_children( pConcrete );
                pConcrete->set_concrete_object( concreteObjectOpt );

                IdentifierMap inheritedContexts;
                {
                    recurseInheritance( database, pConcrete, pState, inheritedContexts );
                    pConcrete->set_inheritance( inheritedContexts.inherited );
                }

                ContextElements elements
                    = constructElements( database, pConcrete, inheritedContexts, concreteObjectOpt, pComponent );

                pConcrete->set_dimensions( elements.dimensions );
                pConcrete->set_links( elements.links );
                pConcrete->set_children( elements.childContexts );
                pConcrete->set_requirements( elements.requirements );

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
                        Context::Args{ ContextGroup::Args{ Concrete::Graph::Vertex::Args{ pComponent }, {} },
                                       pParentContextGroup,
                                       pEvent,
                                       {} },
                        {},
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

                ContextElements elements
                    = constructElements( database, pConcrete, inheritedContexts, concreteObjectOpt, pComponent );

                VERIFY_RTE_MSG(
                    elements.links.empty(), "Event cannot have links: " << Interface::printIContextFullType( pEvent ) );
                VERIFY_RTE_MSG( elements.requirements.empty(),
                                "Event cannot have requirements: " << Interface::printIContextFullType( pEvent ) );

                pConcrete->set_dimensions( elements.dimensions );
                pConcrete->set_children( elements.childContexts );

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
                Interupt* pConcrete = [ & ]() -> Interupt*
                {
                    if( auto pInterfaceRequirement = db_cast< Interface::Requirement >( pInterupt ) )
                    {
                        return database.construct< Requirement >( Requirement::Args{ Interupt::Args{
                            Context::Args{ ContextGroup::Args{ Concrete::Graph::Vertex::Args{ pComponent }, {} },
                                           pParentContextGroup,
                                           pInterupt,
                                           {} },
                            pInterupt,
                            {} } } );
                    }
                    else
                    {
                        return database.construct< Interupt >( Interupt::Args{
                            Context::Args{ ContextGroup::Args{ Concrete::Graph::Vertex::Args{ pComponent }, {} },
                                           pParentContextGroup,
                                           pInterupt,
                                           {} },
                            pInterupt,
                            {} } );
                    }
                }();

                pParentContextGroup->push_back_children( pConcrete );
                pConcrete->set_concrete_object( concreteObjectOpt );

                IdentifierMap inheritedContexts;
                {
                    recurseInheritance( database, pConcrete, pInterupt, inheritedContexts );
                    pConcrete->set_inheritance( inheritedContexts.inherited );
                }

                ContextElements elements
                    = constructElements( database, pConcrete, inheritedContexts, concreteObjectOpt, pComponent );
                VERIFY_RTE_MSG( elements.dimensions.empty(),
                                "Interupt cannot have dimensions: " << Interface::printIContextFullType( pInterupt ) );
                VERIFY_RTE_MSG( elements.links.empty(),
                                "Interupt cannot have links: " << Interface::printIContextFullType( pInterupt ) );
                VERIFY_RTE_MSG(
                    elements.requirements.empty(),
                    "Interupt cannot have requirements: " << Interface::printIContextFullType( pInterupt ) );

                pConcrete->set_children( elements.childContexts );

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
                    Context::Args{ ContextGroup::Args{ Concrete::Graph::Vertex::Args{ pComponent }, {} },
                                   pParentContextGroup,
                                   pFunction,
                                   {} },
                    pFunction } );

                pParentContextGroup->push_back_children( pConcrete );
                pConcrete->set_concrete_object( concreteObjectOpt );

                IdentifierMap inheritedContexts;
                {
                    recurseInheritance( database, pConcrete, pFunction, inheritedContexts );
                    pConcrete->set_inheritance( inheritedContexts.inherited );
                }

                ContextElements elements
                    = constructElements( database, pConcrete, inheritedContexts, concreteObjectOpt, pComponent );
                VERIFY_RTE_MSG( elements.dimensions.empty(),
                                "Function cannot have dimensions: " << Interface::printIContextFullType( pFunction ) );
                VERIFY_RTE_MSG( elements.links.empty(),
                                "Function cannot have links: " << Interface::printIContextFullType( pFunction ) );
                VERIFY_RTE_MSG(
                    elements.requirements.empty(),
                    "Function cannot have requirements: " << Interface::printIContextFullType( pFunction ) );

                pConcrete->set_children( elements.childContexts );

                return pConcrete;
            }
            else
            {
                return nullptr;
            }
        }
        else if( auto pDecider = db_cast< Interface::Decider >( pContext ) )
        {
            if( concreteObjectOpt.has_value() )
            {
                Decider* pConcrete = database.construct< Decider >(
                    Decider::Args{ Context::Args{ ContextGroup::Args{ Concrete::Graph::Vertex::Args{ pComponent }, {} },
                                                  pParentContextGroup,
                                                  pDecider,
                                                  {} },
                                   pDecider } );

                pParentContextGroup->push_back_children( pConcrete );
                pConcrete->set_concrete_object( concreteObjectOpt );

                IdentifierMap inheritedContexts;
                {
                    recurseInheritance( database, pConcrete, pDecider, inheritedContexts );
                    pConcrete->set_inheritance( inheritedContexts.inherited );
                }

                ContextElements elements
                    = constructElements( database, pConcrete, inheritedContexts, concreteObjectOpt, pComponent );
                VERIFY_RTE_MSG( elements.dimensions.empty(),
                                "Decider cannot have dimensions: " << Interface::printIContextFullType( pDecider ) );
                VERIFY_RTE_MSG( elements.links.empty(),
                                "Decider cannot have links: " << Interface::printIContextFullType( pDecider ) );
                VERIFY_RTE_MSG( elements.requirements.empty(),
                                "Decider cannot have requirements: " << Interface::printIContextFullType( pDecider ) );

                pConcrete->set_children( elements.childContexts );

                return pConcrete;
            }
            else
            {
                return nullptr;
            }
        }
        else if( auto pObject = db_cast< Interface::Object >( pContext ) )
        {
            Object* pConcrete = database.construct< Object >(
                Object::Args{ UserDimensionContext::Args{
                                  Context::Args{ ContextGroup::Args{ Concrete::Graph::Vertex::Args{ pComponent }, {} },
                                                 pParentContextGroup,
                                                 pObject,
                                                 {} },
                                  {},
                                  {},
                                  {} },
                              pObject } );

            pParentContextGroup->push_back_children( pConcrete );

            concreteObjectOpt = pConcrete;
            pConcrete->set_concrete_object( concreteObjectOpt );

            IdentifierMap inheritedContexts;
            {
                recurseInheritance( database, pConcrete, pObject, inheritedContexts );
                pConcrete->set_inheritance( inheritedContexts.inherited );
            }

            ContextElements elements
                = constructElements( database, pConcrete, inheritedContexts, concreteObjectOpt, pComponent );
            VERIFY_RTE_MSG( elements.requirements.empty(),
                            "Object cannot have requirements: " << Interface::printIContextFullType( pObject ) );

            pConcrete->set_dimensions( elements.dimensions );
            pConcrete->set_children( elements.childContexts );
            pConcrete->set_links( elements.links );
            pConcrete->set_bitsets( elements.bitsets );
            pConcrete->set_ownership_link( elements.pOwnershipLink );
            pConcrete->set_configuration( elements.pConfiguration );
            pConcrete->set_activation( elements.pActivation );
            pConcrete->set_enablement( elements.pEnablement );
            pConcrete->set_history( elements.pHistory );

            return pConcrete;
        }
        else
        {
            THROW_RTE( "Unknown context type" );
        }
    }

    using CompletionsMap = std::multimap< Concrete::Context*, Concrete::Action* >;

    void recurseCompletions( Concrete::ContextGroup* pContext, std::optional< Concrete::Context* > completionHandler,
                             CompletionsMap& completions )
    {
        bool bFoundInteruptCompletionHandler = false;
        {
            for( Concrete::Context* pChildContext : pContext->get_children() )
            {
                if( auto pRequirement = db_cast< Concrete::Requirement >( pChildContext ) )
                {
                    THROW_TODO;
                }
                if( auto pInterupt = db_cast< Concrete::Interupt >( pChildContext ) )
                {
                    if( !pInterupt->get_interface_interupt()->get_events_trait_opt().has_value() )
                    {
                        const bool bHasTransition
                            = pInterupt->get_interface_interupt()->get_transition_trait_opt().has_value();
                        VERIFY_RTE_MSG(
                            bHasTransition,
                            "Invalid completion interupt does NOT specify a transition: "
                                << Interface::printIContextFullType( pInterupt->get_interface_interupt() ) );

                        VERIFY_RTE_MSG( !bFoundInteruptCompletionHandler,
                                        "Ambiguous completion interupt found: " << Interface::printIContextFullType(
                                            pInterupt->get_interface_interupt() ) );

                        completionHandler               = pInterupt;
                        bFoundInteruptCompletionHandler = true;
                    }
                }
            }
        }

        if( auto pState = db_cast< Concrete::State >( pContext ) )
        {
            const bool bHasTransition = pState->get_interface_state()->get_transition_trait_opt().has_value();
            if( bHasTransition )
            {
                VERIFY_RTE_MSG( !bFoundInteruptCompletionHandler,
                                "Ambiguous completion interupt found: "
                                    << Interface::printIContextFullType( pState->get_interface_state() ) );
                bFoundInteruptCompletionHandler = true;
                completionHandler               = pState;
            }
        }

        if( auto pAction = db_cast< Concrete::Action >( pContext ) )
        {
            pAction->set_completion_handler( completionHandler );
            if( completionHandler.has_value() )
            {
                completions.insert( { completionHandler.value(), pAction } );
            }
        }

        for( Concrete::Context* pChildContext : pContext->get_children() )
        {
            recurseCompletions( pChildContext, completionHandler, completions );
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

        using namespace Concrete;

        Database database( m_environment, m_sourceFilePath );

        Interface::Root*       pInterfaceRoot = database.one< Interface::Root >( m_sourceFilePath );
        Components::Component* pComponent     = pInterfaceRoot->get_root()->get_component();
        VERIFY_RTE( pComponent );

        Concrete::Root* pConcreteRoot = database.construct< Root >(
            Root::Args{ ContextGroup::Args{ Concrete::Graph::Vertex::Args{ pComponent }, {} }, pInterfaceRoot } );
        std::optional< Concrete::Object* > concreteObjectOpt;

        for( Interface::IContext* pChildContext : pInterfaceRoot->get_children() )
        {
            recurse( database, pConcreteRoot, pChildContext, concreteObjectOpt, pComponent );
        }

        // determine action completion handlers
        {
            CompletionsMap completions;
            recurseCompletions( pConcreteRoot, std::nullopt, completions );
            for( auto [ pContext, pAction ] : completions )
            {
                if( auto pState = db_cast< Concrete::State >( pContext ) )
                {
                    pState->push_back_completions( pAction );
                }
                else if( auto pInterupt = db_cast< Concrete::Interupt >( pContext ) )
                {
                    pInterupt->push_back_completions( pAction );
                }
                else
                {
                    THROW_RTE( "Unknown completion context type" );
                }
            }
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
