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

#include "mega/common_strings.hpp"

#include "base_task.hpp"

#include "database/InterfaceStage.hxx"

#include "mega/values/compilation/anon_symbols.hpp"

namespace InterfaceStage
{
#include "compiler/interface.hpp"
#include "compiler/interface_printer.hpp"
} // namespace InterfaceStage

namespace mega::compiler
{

using namespace InterfaceStage;

class Task_AST : public BaseTask
{
    using ReservedSymbolMap = std::map< std::string, Parser::ReservedSymbol* >;

public:
    Task_AST( const TaskArguments& taskArguments )
        : BaseTask( taskArguments )
    {
    }

    using SymbolVector = std::vector< Parser::Symbol* >;

    Interface::NodeGroup* findOrCreate( Database& database, Components::Component* pComponent,
                                        Interface::NodeGroup* pIter, SymbolVector::const_iterator i,
                                        SymbolVector::const_iterator         iEnd,
                                        std::vector< Interface::IContext* >& icontexts )
    {
        for( ; i != iEnd; ++i )
        {
            Parser::Symbol* pSymbol = *i;

            bool bFound = false;
            for( auto pChild : pIter->get_children() )
            {
                if( pChild->get_symbol()->get_token() == pSymbol->get_token() )
                {
                    pIter  = pChild;
                    bFound = true;
                    break;
                }
            }

            if( !bFound )
            {
                // create new IContext
                auto pNewIContext = database.construct< Interface::IContext >(
                    // clang-format off
                    Interface::IContext::Args
                    { 
                        Interface::Node::Args
                        {
                            Interface::NodeGroup::Args
                            { 
                                {} 
                            },
                            pSymbol, 
                            pIter, 
                            pComponent
                        }, 
                        {}  // containers
                    } );
                // clang-format on
                pIter->push_back_children( pNewIContext );

                icontexts.push_back( pNewIContext );
                pIter = pNewIContext;
            }
        }
        return pIter;
    }

    Parser::Symbol* findUnusedSymbol( const ReservedSymbolMap& reservedSymbols, Interface::NodeGroup* pNodeGroup )
    {
        // find anon symbol for context
        std::optional< std::string > strUnusedAnonOpt;
        {
            for( const auto& strAnon : getAnonSymbols() )
            {
                bool bFound = false;
                for( auto pExisting : pNodeGroup->get_children() )
                {
                    if( pExisting->get_symbol()->get_token() == strAnon )
                    {
                        bFound = true;
                        break;
                    }
                }
                if( !bFound )
                {
                    strUnusedAnonOpt = strAnon;
                    break;
                }
            }
            VERIFY_RTE_MSG( strUnusedAnonOpt.has_value(), "No remaining anon symbols to use" );
        }

        // use the anon symbol - and allow duplicate use outside of context
        auto iFind = reservedSymbols.find( strUnusedAnonOpt.value() );
        VERIFY_RTE( iFind != reservedSymbols.end() );
        return iFind->second;
    }

    void buildInterfaceTree( Database& database, Components::Component* pComponent, Interface::NodeGroup* pNodeGroup,
                             Parser::Container* pContainer, std::vector< Interface::IContext* >& icontexts,
                             const ReservedSymbolMap& reservedSymbols )
    {
        VERIFY_RTE( pNodeGroup );

        auto symbols = pContainer->get_symbols();
        if( symbols.empty() )
        {
            symbols = { findUnusedSymbol( reservedSymbols, pNodeGroup ) };
        }

        pNodeGroup = findOrCreate( database, pComponent, pNodeGroup, symbols.begin(), symbols.end(), icontexts );
        VERIFY_PARSER(
            db_cast< Interface::IContext >( pNodeGroup ), "Conflicting context type detected: ", pContainer );

        if( auto pIContext = db_cast< Interface::IContext >( pNodeGroup ) )
        {
            pIContext->push_back_containers( pContainer );

            for( auto pAggregate : pContainer->get_aggregates() )
            {
                auto aggregateSymbols = pAggregate->get_symbols();

                auto pAggregateParent = pIContext;
                if( aggregateSymbols.size() > 1 )
                {
                    pAggregateParent = db_cast< Interface::IContext >(
                        findOrCreate( database, pComponent, pIContext, aggregateSymbols.begin(),
                                      aggregateSymbols.end() - 1, icontexts ) );
                    VERIFY_RTE( pAggregateParent );
                }

                Parser::Symbol* pSymbol = nullptr;
                {
                    if( !aggregateSymbols.empty() )
                    {
                        pSymbol = aggregateSymbols.back();
                        // check for duplicate symbols
                        for( auto pExisting : pAggregateParent->get_children() )
                        {
                            VERIFY_PARSER( pExisting->get_symbol()->get_token() != pSymbol->get_token(),
                                           "Duplicate symbols: " << pSymbol->get_token(), pContainer );
                        }
                    }
                    else
                    {
                        pSymbol = findUnusedSymbol( reservedSymbols, pAggregateParent );
                    }
                }

                // clang-format off
                Interface::Aggregate* pInterfaceAggregate =
                    database.construct< Interface::Aggregate >(
                        Interface::Aggregate::Args
                        {
                            Interface::Node::Args
                            {
                                Interface::NodeGroup::Args
                                {
                                    {}
                                },
                                pSymbol, 
                                pAggregateParent, 
                                pComponent
                            },
                            pAggregate
                        }
                    );
                
                // clang-format on
                pAggregateParent->push_back_children( pInterfaceAggregate );

                // refine the aggregate as needed
                if( auto pParserDimension = db_cast< Parser::Dimension >( pAggregate ) )
                {
                    database.construct< Interface::UserDimension >(
                        Interface::UserDimension::Args{ pInterfaceAggregate, pParserDimension } );
                }
                else if( auto pParserLink = db_cast< Parser::Link >( pAggregate ) )
                {
                    database.construct< Interface::UserLink >(
                        Interface::UserLink::Args{ pInterfaceAggregate, pParserLink } );
                }
                else if( auto pParserAlias = db_cast< Parser::Alias >( pAggregate ) )
                {
                    database.construct< Interface::UserAlias >(
                        Interface::UserAlias::Args{ pInterfaceAggregate, pParserAlias } );
                }
                else if( auto pParserUsing = db_cast< Parser::Using >( pAggregate ) )
                {
                    database.construct< Interface::UserUsing >(
                        Interface::UserUsing::Args{ pInterfaceAggregate, pParserUsing } );
                }
                else
                {
                    THROW_RTE( "Unknown parsed aggregate type" );
                }
            }
        }
        else
        {
            VERIFY_RTE( pContainer->get_aggregates().empty() );
        }

        for( auto pChild : pContainer->get_children() )
        {
            buildInterfaceTree( database, pComponent, pNodeGroup, pChild, icontexts, reservedSymbols );
        }

        for( auto pInclude : pContainer->get_includes() )
        {
            if( auto pMegaInclude = db_cast< Parser::MegaInclude >( pInclude ) )
            {
                if( auto pMegaIncludeInline = db_cast< Parser::MegaIncludeInline >( pMegaInclude ) )
                {
                    buildInterfaceTree( database, pComponent, pNodeGroup, pMegaIncludeInline->get_root()->get_ast(),
                                        icontexts, reservedSymbols );
                }
                else if( auto pMegaIncludeNested = db_cast< Parser::MegaIncludeNested >( pMegaInclude ) )
                {
                    auto pIdentifier = pMegaIncludeNested->get_identifier();
                    auto symbols     = pIdentifier->get_symbols();

                    auto pIncludeGroup
                        = findOrCreate( database, pComponent, pNodeGroup, symbols.begin(), symbols.end(), icontexts );
                    buildInterfaceTree( database, pComponent, pIncludeGroup, pMegaIncludeNested->get_root()->get_ast(),
                                        icontexts, reservedSymbols );
                }
                else
                {
                    THROW_RTE( "Unknown include type" );
                }
            }
        }
    }

    template < typename InterfaceType >
    InterfaceType* getOrCreate( Database& database, Interface::IContext* pIContext, Parser::Container* pContainer,
                                bool& bRefined )
    {
        InterfaceType* p = nullptr;
        if( bRefined )
        {
            p = db_cast< InterfaceType >( pIContext );
            VERIFY_PARSER( p,
                           "Conflicting container types defined of: " << Interface::getKind( pIContext ) << " and: "
                                                                      << Interface::getKind< InterfaceType >(),
                           pContainer );
        }
        else
        {
            p        = database.construct< InterfaceType >( typename InterfaceType::Args{ pIContext } );
            bRefined = true;
        }
        return p;
    }

    template < typename InterfaceType >
    InterfaceType* getOrCreateInvocationContext( Database& database, Interface::IContext* pIContext,
                                                 Parser::Container* pContainer, bool& bRefined )
    {
        InterfaceType* p = nullptr;
        if( bRefined )
        {
            p = db_cast< InterfaceType >( pIContext );
            VERIFY_PARSER( p,
                           "Conflicting container types defined of: " << Interface::getKind( pIContext ) << " and: "
                                                                      << Interface::getKind< InterfaceType >(),
                           pContainer );
        }
        else
        {
            p = database.construct< InterfaceType >(
                typename InterfaceType::Args{ Interface::InvocationContext::Args{ pIContext } } );
            bRefined = true;
        }
        return p;
    }

    template < typename ParserContainerType, typename IContextType >
    void refineInheritance( ParserContainerType* pContainer, IContextType* pIContext )
    {
        if( auto pInheritanceOpt = pContainer->get_inheritance_type_opt() )
        {
            VERIFY_PARSER(
                !pIContext->is_inheritance_opt(), "Type has duplicate inheritance specifications", pContainer );
            pIContext->set_inheritance_opt( pInheritanceOpt.value() );
        }
    }

    template < typename ParserContainerType, typename IContextType >
    void refineSize( ParserContainerType* pContainer, IContextType* pIContext )
    {
        auto sizeOpt = pContainer->get_size_opt();
        if( sizeOpt.has_value() )
        {
            Parser::Size* pSize      = sizeOpt.value();
            bool          bWriteSize = false;

            if( pIContext->is_size_opt() )
            {
                if( !pSize->get_singular() )
                {
                    VERIFY_PARSER( pIContext->get_singular(), "Duplicate size specification", pContainer );
                    bWriteSize = true;
                }
            }
            else
            {
                bWriteSize = true;
            }

            if( bWriteSize )
            {
                pIContext->set_size_opt( pSize );
                pIContext->set_size( pSize->get_amount() );
                pIContext->set_singular( pSize->get_singular() );
            }
        }
    }

    void refineIContext( Database& database, Interface::IContext* pIContext )
    {
        using namespace InterfaceStage::Parser;

        bool bRefined = false;

        for( auto pContainer : pIContext->get_containers() )
        {
            if( auto pAbstract = db_cast< Abstract >( pContainer ) )
            {
                auto pIAbstract = getOrCreate< Interface::Abstract >( database, pIContext, pContainer, bRefined );

                refineInheritance( pAbstract, pIAbstract );
                refineSize( pAbstract, pIAbstract );
            }
            else if( auto pNamespace = db_cast< Namespace >( pContainer ) )
            {
                auto pINamespace = getOrCreate< Interface::Namespace >( database, pIContext, pContainer, bRefined );

                // allow namespace to be included into other type
                bool bFoundParentObject = false;
                for( Interface::IContext* p = pIContext; p != nullptr;
                    p                      = db_cast< Interface::IContext >( p->get_parent() ) )
                {
                    if( db_cast< Interface::Object >( p ) )
                    {
                        bFoundParentObject = true;
                        break;
                    }
                }
                pINamespace->set_is_global( bFoundParentObject );
            }
            else if( auto pEvent = db_cast< Event >( pContainer ) )
            {
                auto pIEvent = getOrCreate< Interface::Event >( database, pIContext, pContainer, bRefined );
                refineInheritance( pEvent, pIEvent );
                refineSize( pEvent, pIEvent );
            }
            else if( auto pInterupt = db_cast< Interupt >( pContainer ) )
            {
                auto* p
                    = getOrCreateInvocationContext< Interface::Interupt >( database, pIContext, pContainer, bRefined );

                if( auto eventsOpt = pInterupt->get_events_type_opt() )
                {
                    VERIFY_PARSER( !p->is_events_opt(), "Interupt has duplicate event specifications", pContainer );
                    p->set_events_opt( eventsOpt );
                }
                if( auto transitionOpt = pInterupt->get_transition_type_opt() )
                {
                    VERIFY_PARSER(
                        !p->is_transition_opt(), "Interupt has duplicate transition specifications", pContainer );
                    p->set_transition_opt( transitionOpt );
                }
                if( auto bodyOpt = pInterupt->get_body_type_opt() )
                {
                    VERIFY_PARSER( !p->is_body_opt(), "Interupt has duplicate body specifications", pContainer );
                    p->set_body_opt( bodyOpt );
                }
            }
            else if( auto pRequirement = db_cast< Requirement >( pContainer ) )
            {
                auto* p = getOrCreateInvocationContext< Interface::Requirement >(
                    database, pIContext, pContainer, bRefined );

                if( auto eventsOpt = pRequirement->get_events_type_opt() )
                {
                    VERIFY_PARSER( !p->is_events(), "Duplicate events specified for requirement", pContainer );
                    p->set_events( eventsOpt.value() );
                }
                if( auto transitionOpt = pRequirement->get_transition_type_opt() )
                {
                    VERIFY_PARSER( !p->is_transition(), "Duplicate events specified for requirement", pContainer );
                    p->set_transition( transitionOpt.value() );
                }
                if( auto bodyOpt = pRequirement->get_body_type_opt() )
                {
                    VERIFY_PARSER( !p->is_body_opt(), "Requirement has duplicate body specifications", pContainer );
                    p->set_body_opt( bodyOpt );
                }
            }
            else if( auto pFunction = db_cast< Function >( pContainer ) )
            {
                auto* p
                    = getOrCreateInvocationContext< Interface::Function >( database, pIContext, pContainer, bRefined );

                if( p->is_return_type() )
                {
                    //     // check return types match ...
                    THROW_TODO;
                }
                else
                {
                    p->set_return_type( pFunction->get_return_type() );
                }

                if( p->is_arguments() )
                {
                    // check return types match ...
                    THROW_TODO;
                }
                else
                {
                    p->set_arguments( pFunction->get_arguments_type() );
                }

                if( auto bodyOpt = pFunction->get_body_type_opt() )
                {
                    VERIFY_PARSER( !p->is_body_opt(), "Existing function body defined", pContainer );
                    p->set_body_opt( bodyOpt.value() );
                }
            }
            else if( auto pDecider = db_cast< Decider >( pContainer ) )
            {
                auto* p
                    = getOrCreateInvocationContext< Interface::Decider >( database, pIContext, pContainer, bRefined );

                p->set_events( pDecider->get_events_type() );

                if( auto bodyOpt = pDecider->get_body_type_opt() )
                {
                    VERIFY_PARSER( !p->is_body_opt(), "Existing decider body defined", pContainer );
                    p->set_body_opt( bodyOpt.value() );
                }
            }
            else if( auto pObject = db_cast< Object >( pContainer ) )
            {
                auto* p = getOrCreate< Interface::Object >( database, pIContext, pContainer, bRefined );
                refineInheritance( pObject, p );
                refineSize( pObject, p );
            }
            else if( auto pAction = db_cast< Action >( pContainer ) )
            {
                Interface::Action* p = nullptr;
                {
                    if( bRefined )
                    {
                        p = db_cast< Interface::Action >( pIContext );
                        VERIFY_PARSER( p,
                                       "Conflicting container types defined of: " << Interface::getKind(
                                           pIContext ) << " and: " << Interface::getKind< Interface::Action >(),
                                       pContainer );
                    }
                    else
                    {
                        p        = database.construct< Interface::Action >( typename Interface::Action::Args{
                            Interface::State::Args{ Interface::InvocationContext::Args{ pIContext } } } );
                        bRefined = true;
                    }
                }
                refineInheritance( pAction, p );
                refineSize( pAction, p );

                if( auto transitionOpt = pAction->get_transition_type_opt() )
                {
                    VERIFY_PARSER(
                        !p->is_transition_opt(), "Action has duplicate transition specifications", pContainer );
                    p->set_transition_opt( transitionOpt.value() );
                }
                for( auto pPart : pAction->get_parts() )
                {
                    p->push_back_parts( pPart );
                }
                if( auto bodyOpt = pAction->get_body_type_opt() )
                {
                    VERIFY_PARSER( !p->is_body_opt(), "Existing action body defined", pContainer );
                    p->set_body_opt( bodyOpt );
                }
            }
            else if( auto pComponent = db_cast< Component >( pContainer ) )
            {
                Interface::Component* p = nullptr;
                {
                    if( bRefined )
                    {
                        p = db_cast< Interface::Component >( pIContext );
                        VERIFY_PARSER( p,
                                       "Conflicting container types defined of: " << Interface::getKind(
                                           pIContext ) << " and: " << Interface::getKind< Interface::Component >(),
                                       pContainer );
                    }
                    else
                    {
                        p        = database.construct< Interface::Component >( typename Interface::Component::Args{
                            Interface::State::Args{ Interface::InvocationContext::Args{ pIContext } } } );
                        bRefined = true;
                    }
                }
                refineInheritance( pComponent, p );
                refineSize( pComponent, p );

                if( auto transitionOpt = pComponent->get_transition_type_opt() )
                {
                    VERIFY_PARSER(
                        !p->is_transition_opt(), "Component has duplicate transition specifications", pContainer );
                    p->set_transition_opt( transitionOpt.value() );
                }
                for( auto pPart : pComponent->get_parts() )
                {
                    p->push_back_parts( pPart );
                }
            }
            else if( auto pState = db_cast< State >( pContainer ) )
            {
                auto* p = getOrCreateInvocationContext< Interface::State >( database, pIContext, pContainer, bRefined );
                refineInheritance( pState, p );
                refineSize( pState, p );

                if( auto transitionOpt = pState->get_transition_type_opt() )
                {
                    VERIFY_PARSER(
                        !p->is_transition_opt(), "State has duplicate transition specifications", pContainer );
                    p->set_transition_opt( transitionOpt.value() );
                }
                for( auto pPart : pState->get_parts() )
                {
                    p->push_back_parts( pPart );
                }
            }
            else
            {
                // leave unspecified and detect after all containers
            }
        }

        // if not refined then turn into namespace
        if( !bRefined )
        {
            // is this a global namespace
            database.construct< Interface::Unspecified >( Interface::Unspecified::Args{ pIContext } );
            bRefined = true;
        }
    }

    void checkAndConstructIContext( Database& database, Interface::IContext* pIContext,
                                    const ReservedSymbolMap& reservedSymbols )
    {
        using namespace InterfaceStage::Interface;

        // get first container associated wtih IContext
        Parser::Container* pContainer = nullptr;
        {
            for( auto pIter = pIContext; pIter; pIter = db_cast< Interface::IContext >( pIter->get_parent() ) )
            {
                auto containers = pIter->get_containers();
                if( !containers.empty() )
                {
                    pContainer = containers.front();
                    break;
                }
            }
        }

        if( !pIContext->is_size_opt() )
        {
            pIContext->set_size_opt( std::nullopt );
            pIContext->set_size( 1 );
            pIContext->set_singular( true );
        }

        std::vector< IContext* >  contexts;
        std::vector< Aggregate* > aggregates;
        Interface::getNodes( pIContext, contexts, aggregates );

        // check that no aggregate has symbol matching any IContext in same node group
        {
            std::set< std::string > uniqueSymbols;
            for( auto pAggregate : aggregates )
            {
                const auto& token = pAggregate->get_symbol()->get_token();
                VERIFY_PARSER(
                    uniqueSymbols.insert( token ).second, "Duplicate symbol: " << token << " found", pContainer );
            }
            for( auto pContext : contexts )
            {
                const auto& token = pContext->get_symbol()->get_token();
                VERIFY_PARSER(
                    uniqueSymbols.insert( token ).second, "Duplicate symbol: " << token << " found", pContainer );
            }
        }

        {
            if( auto pNamespace = db_cast< Namespace >( pIContext ) )
            {
                if( pNamespace->get_is_global() )
                {
                    // Allow namespaces to be inherited like an interface so they should be able to
                    // contain anything
                }
            }
            else if( auto pAbstract = db_cast< Abstract >( pIContext ) )
            {
            }
            else if( auto pUnspecified = db_cast< Unspecified >( pIContext ) )
            {
            }
            else if( auto pEvent = db_cast< Event >( pIContext ) )
            {
            }
            else if( auto pInterupt = db_cast< Interupt >( pIContext ) )
            {
                VERIFY_PARSER( pInterupt->get_singular(), "Interupt cannot have non-singular size", pContainer );
            }
            else if( auto pRequirement = db_cast< Requirement >( pIContext ) )
            {
                VERIFY_PARSER( pRequirement->get_singular(), "Requirement cannot have non-singular size", pContainer );
                VERIFY_PARSER( pRequirement->is_events(), "Requirement requires events", pContainer );
                VERIFY_PARSER( pRequirement->is_transition(), "Requirement requires transitions", pContainer );
            }
            else if( auto pFunction = db_cast< Function >( pIContext ) )
            {
                VERIFY_PARSER( pFunction->get_singular(), "Function cannot have non-singular size", pContainer );
                VERIFY_PARSER( pFunction->is_return_type(), "Function requires return type", pContainer );
                VERIFY_PARSER( pFunction->is_arguments(), "Function requires return type", pContainer );
            }
            else if( auto pDecider = db_cast< Decider >( pIContext ) )
            {
                VERIFY_PARSER( pDecider->get_singular(), "Decider cannot have non-singular size", pContainer );
                VERIFY_PARSER( pDecider->is_events(), "Decider MUST have events", pContainer );
            }
            else if( auto pObject = db_cast< Object >( pIContext ) )
            {
            }
            else if( auto pAction = db_cast< Action >( pIContext ) )
            {
            }
            else if( auto pComponent = db_cast< Component >( pIContext ) )
            {
            }
            else if( auto pState = db_cast< State >( pIContext ) )
            {
                VERIFY_PARSER(
                    !pState->is_body_opt(), "State cannot have function body", pState->get_containers().front() );
            }
            else
            {
                THROW_RTE( "Unknown IContext type" );
            }
        }

        // ensure optional elements are set
        {
            if( !pIContext->is_inheritance_opt() )
            {
                pIContext->set_inheritance_opt( std::nullopt );
            }

            if( auto pInvocable = db_cast< InvocationContext >( pIContext ) )
            {
                if( !pInvocable->is_body_opt() )
                {
                    pInvocable->set_body_opt( std::nullopt );
                }
            }

            if( auto pInterupt = db_cast< Interupt >( pIContext ) )
            {
                if( !pInterupt->is_events_opt() )
                {
                    pInterupt->set_events_opt( std::nullopt );
                }
                if( !pInterupt->is_transition_opt() )
                {
                    pInterupt->set_transition_opt( std::nullopt );
                }
            }

            if( auto pState = db_cast< State >( pIContext ) )
            {
                if( !pState->is_parts() )
                {
                    pState->set_parts( {} );
                }
                if( !pState->is_transition_opt() )
                {
                    pState->set_transition_opt( std::nullopt );
                }
            }
        }
    }

    void check( Database& database, Interface::Node* pNode, const ReservedSymbolMap& reservedSymbols )
    {
        pNode->set_kind( Interface::getKind( pNode ) );

        // set the kind
        if( auto pIContext = db_cast< Interface::IContext >( pNode ) )
        {
            checkAndConstructIContext( database, pIContext, reservedSymbols );
        }

        for( auto pChild : pNode->get_children() )
        {
            check( database, pChild, reservedSymbols );
        }
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const auto                          projectManifestPath = m_environment.project_manifest();
        const mega::io::CompilationFilePath treePath
            = m_environment.InterfaceStage_Tree( m_environment.project_manifest() );
        start( taskProgress, "Task_AST", projectManifestPath.path(), treePath.path() );

        task::DeterminantHash determinant{ m_toolChain.toolChainHash };
        for( const mega::io::megaFilePath& sourceFilePath : getSortedSourceFiles() )
        {
            determinant ^= m_environment.getBuildHashCode( m_environment.ParserStage_AST( sourceFilePath ) );
        }

        if( m_environment.restore( treePath, determinant ) )
        {
            m_environment.setBuildHashCode( treePath );
            cached( taskProgress );
            return;
        }

        Database database( m_environment, projectManifestPath );

        ReservedSymbolMap reservedSymbols;
        for( const mega::io::megaFilePath& sourceFilePath : getSortedSourceFiles() )
        {
            for( auto pReservedSymbol : database.many< Parser::ReservedSymbol >( sourceFilePath ) )
            {
                reservedSymbols.insert( { pReservedSymbol->get_token(), pReservedSymbol } );
            }
        }

        Interface::Root* pInterfaceRoot
            = database.construct< Interface::Root >( Interface::Root::Args{ Interface::NodeGroup::Args{ {} } } );

        std::vector< Interface::IContext* > icontexts;
        {
            // first build the icontext tree with no refinement
            for( const mega::io::megaFilePath& sourceFilePath : getSortedSourceFiles() )
            {
                auto* pComponent = getComponent< Components::Component >( database, sourceFilePath );
                VERIFY_RTE( pComponent );
                auto* pParserRoot = database.one< Parser::ObjectSourceRoot >( sourceFilePath );
                VERIFY_RTE( pParserRoot );
                auto* pParserAST = pParserRoot->get_ast();
                VERIFY_RTE( pParserAST );
                for( auto pParserNode : pParserAST->get_children() )
                {
                    buildInterfaceTree( database, pComponent, pInterfaceRoot, pParserNode, icontexts, reservedSymbols );
                }
            }
        }

        // now go through ALL icontext and refine their types
        for( auto pIContext : icontexts )
        {
            refineIContext( database, pIContext );
        }

        // add generated meta inheritance types
        for( const auto& strFlags : IContextFlags::strings() )
        {
            auto iFind = reservedSymbols.find( strFlags );
            VERIFY_RTE( iFind != reservedSymbols.end() );
            auto pContext = database.construct< Interface::Abstract >( Interface::Abstract::Args{
                Interface::IContext::Args{ Interface::Node::Args{ Interface::NodeGroup::Args{ {} }, iFind->second,
                                                                  pInterfaceRoot, std::nullopt },
                                           {} } } );
            pInterfaceRoot->push_back_children( pContext );
        }

        for( auto pNode : pInterfaceRoot->get_children() )
        {
            check( database, pNode, reservedSymbols );
        }

        const task::FileHash fileHashCode = database.save_Tree_to_temp();
        m_environment.setBuildHashCode( treePath, fileHashCode );
        m_environment.temp_to_real( treePath );
        m_environment.stash( treePath, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_AST( const TaskArguments& taskArguments )
{
    return std::make_unique< Task_AST >( taskArguments );
}

} // namespace mega::compiler
