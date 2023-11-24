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

#include "invocation/invocation.hpp"

#include "database/OperationsStage.hxx"

#include "compiler/derivation.hpp"

#include "mega/values/compilation/operation_id.hpp"
#include "mega/values/compilation/hyper_graph.hpp"

#include "mega/common_strings.hpp"
#include "mega/make_unique_without_reorder.hpp"

#include "common/unreachable.hpp"

#include <optional>
#include <unordered_set>

namespace OperationsStage
{
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
#include "compiler/common_ancestor.hpp"
#include "compiler/invocation_policy.hpp"
#include "compiler/derivation_compiler.hpp"
namespace Derivation
{
#include "compiler/derivation_printer.hpp"
#include "compiler/disambiguate.hpp"
} // namespace Derivation
} // namespace OperationsStage

using namespace OperationsStage;
using namespace OperationsStage::Operations;

namespace mega::invocation
{
SymbolTables::SymbolTables( Symbols::SymbolTable* pSymbolTable )
    : symbolIDMap( pSymbolTable->get_symbol_type_ids() )
    , interfaceIDMap( pSymbolTable->get_interface_type_ids() )
{
}

namespace
{
struct InvocationPolicy : public InvocationPolicyBase
{
    OrPtrVector expandLink( OrPtr pOr ) const
    {
        OrPtrVector result;

        if( auto pLink = db_cast< Concrete::Dimensions::Link >( pOr->get_vertex() ) )
        {
            GraphEdgeVector edges;
            for( auto pEdge : pLink->get_out_edges() )
            {
                switch( pEdge->get_type().get() )
                {
                    case EdgeType::eMonoSingularMandatory:
                    case EdgeType::ePolySingularMandatory:
                    case EdgeType::eMonoNonSingularMandatory:
                    case EdgeType::ePolyNonSingularMandatory:
                    case EdgeType::eMonoSingularOptional:
                    case EdgeType::ePolySingularOptional:
                    case EdgeType::eMonoNonSingularOptional:
                    case EdgeType::ePolyNonSingularOptional:
                    case EdgeType::ePolyParent:
                    {
                        edges.push_back( pEdge );
                    }
                    break;
                    case EdgeType::eParent:
                    case EdgeType::eChildSingular:
                    case EdgeType::eChildNonSingular:
                    case EdgeType::eDim:
                    case EdgeType::eLink:
                    case EdgeType::TOTAL_EDGE_TYPES:
                        break;
                }
            }
            VERIFY_RTE( !edges.empty() );

            auto pAnd = m_database.construct< Derivation::And >(
                Derivation::And::Args{ Derivation::Step::Args{ Derivation::Node::Args{ {} }, pLink } } );

            auto pOrToAndEdge
                = m_database.construct< Derivation::Edge >( Derivation::Edge::Args{ pOr, pAnd, false, false, 0, {} } );

            pOr->push_back_edges( pOrToAndEdge );

            for( auto pGraphEdge : edges )
            {
                // determine the parent context of the link target
                GraphVertex* pParentVertex = nullptr;
                GraphEdge*   pParentEdge   = nullptr;
                {
                    auto pLinkTarget = pGraphEdge->get_target();
                    for( auto pLinkGraphEdge : pLinkTarget->get_out_edges() )
                    {
                        if( pLinkGraphEdge->get_type().get() == EdgeType::eParent )
                        {
                            VERIFY_RTE( !pParentVertex );
                            pParentEdge   = pLinkGraphEdge;
                            pParentVertex = pLinkGraphEdge->get_target();
                        }
                    }
                }
                VERIFY_RTE( pParentEdge );
                VERIFY_RTE( pParentVertex );

                auto pLinkTargetOr = m_database.construct< Derivation::Or >(
                    Derivation::Or::Args{ Derivation::Step::Args{ Derivation::Node::Args{ {} }, pParentVertex } } );

                auto pDerivationEdge = m_database.construct< Derivation::Edge >(
                    Derivation::Edge::Args{ pAnd, pLinkTargetOr, false, false, 0, { pGraphEdge, pParentEdge } } );

                pAnd->push_back_edges( pDerivationEdge );

                result.push_back( pLinkTargetOr );
            }
        }
        else
        {
            result.push_back( pOr );
        }

        return result;
    }

    GraphVertex* commonRootDerivation( GraphVertex* pSource, GraphVertex* pTarget, GraphEdgeVector& edges ) const
    {
        return CommonAncestor::commonRootDerivation( pSource, pTarget, edges );
    }

    InvocationPolicy( Database& database )
        : InvocationPolicyBase( database )
    {
    }
};

void fromInvocationID( const SymbolTables& symbolTables, const mega::InvocationID& id,
                       std::vector< Concrete::Graph::Vertex* >& contextTypes, InvocationPolicy::Spec& spec )
{
    std::optional< mega::OperationID > operationIDOpt;

    for( auto& symbolID : id.m_context )
    {
        if( isOperationType( symbolID ) )
        {
            VERIFY_RTE_MSG( !operationIDOpt.has_value(), "Operation ID defined twice" );
            operationIDOpt = static_cast< mega::OperationID >( symbolID.getSymbolID() );
        }
        else if( symbolID.isSymbolID() )
        {
            auto iFind = symbolTables.symbolIDMap.find( symbolID );
            VERIFY_RTE( iFind != symbolTables.symbolIDMap.end() );
            auto pSymbol = iFind->second;
            for( auto pContext : pSymbol->get_contexts() )
            {
                for( auto pConcrete : pContext->get_concrete() )
                {
                    if( std::find( contextTypes.begin(), contextTypes.end(), pConcrete ) == contextTypes.end() )
                    {
                        contextTypes.push_back( pConcrete );
                        spec.context.push_back( pConcrete );
                    }
                }
            }
        }
        else
        {
            auto iFind = symbolTables.interfaceIDMap.find( symbolID );
            VERIFY_RTE( iFind != symbolTables.interfaceIDMap.end() );
            auto pSymbol = iFind->second;
            VERIFY_RTE( pSymbol->get_context().has_value() );
            if( auto pInterfaceContext = pSymbol->get_context().value() )
            {
                for( auto pConcrete : pInterfaceContext->get_concrete() )
                {
                    if( std::find( contextTypes.begin(), contextTypes.end(), pConcrete ) == contextTypes.end() )
                    {
                        contextTypes.push_back( pConcrete );
                        spec.context.push_back( pConcrete );
                    }
                }
            }
        }
    }

    for( auto& symbolID : id.m_type_path )
    {
        std::vector< Concrete::Graph::Vertex* > pathElement;

        std::optional< TypeID > actualSymbolID;
        {
            if( isOperationType( symbolID ) )
            {
                VERIFY_RTE_MSG( !operationIDOpt.has_value(), "Operation ID defined twice" );
                operationIDOpt = static_cast< mega::OperationID >( symbolID.getSymbolID() );
            }
            else if( symbolID.isContextID() )
            {
                // NOTE: Always convert any interface type id BACK to its symbol ID
                auto iFind = symbolTables.interfaceIDMap.find( symbolID );
                VERIFY_RTE( iFind != symbolTables.interfaceIDMap.end() );
                auto pInterfaceSymbol = iFind->second;

                const auto& typeIDSequence = pInterfaceSymbol->get_symbol_ids();
                VERIFY_RTE( !typeIDSequence.empty() );
                actualSymbolID = typeIDSequence.back();
            }
            else if( symbolID.isSymbolID() )
            {
                actualSymbolID = symbolID;
            }
        }

        if( actualSymbolID.has_value() )
        {
            auto iFind = symbolTables.symbolIDMap.find( actualSymbolID.value() );
            VERIFY_RTE( iFind != symbolTables.symbolIDMap.end() );
            auto pSymbol = iFind->second;
            for( auto pContext : pSymbol->get_contexts() )
            {
                for( auto pConcrete : pContext->get_concrete() )
                {
                    if( std::find( pathElement.begin(), pathElement.end(), pConcrete ) == pathElement.end() )
                    {
                        pathElement.push_back( pConcrete );
                    }
                }
            }
            for( auto pDimension : pSymbol->get_dimensions() )
            {
                for( auto pConcrete : pDimension->get_concrete() )
                {
                    if( std::find( pathElement.begin(), pathElement.end(), pConcrete ) == pathElement.end() )
                    {
                        pathElement.push_back( pConcrete );
                    }
                }
            }
            for( auto pLink : pSymbol->get_links() )
            {
                for( auto pConcrete : pLink->get_concrete() )
                {
                    if( std::find( pathElement.begin(), pathElement.end(), pConcrete ) == pathElement.end() )
                    {
                        pathElement.push_back( pConcrete );
                    }
                }
            }
            if( !pathElement.empty() )
            {
                spec.path.emplace_back( std::move( pathElement ) );
            }
        }
    }

    VERIFY_RTE_MSG( !operationIDOpt.has_value() || ( id.m_operation == operationIDOpt.value() ),
                    "Mismatching operation type in invocation: " << id );
}

class OperationBuilder
{
    Database&   m_database;
    Invocation* m_pInvocation;

    enum TargetType
    {
        eObjects,
        eComponents,
        eStates,
        eDeciders,
        eFunctions,
        eEvents,
        eUserDimensions,
        eLinkDimensions,
        eUNSET
    } m_targetType
        = eUNSET;

    // clang-format off
    std::vector< Concrete::Object*              > objects;
    std::vector< Concrete::Component*           > components;
    std::vector< Concrete::State*               > states;
    std::vector< Concrete::Decider*             > deciders;
    std::vector< Concrete::Function*            > functions;
    std::vector< Concrete::Namespace*           > namespaces;
    std::vector< Concrete::Event*               > events;
    std::vector< Concrete::Interupt*            > interupts;
    std::vector< Concrete::Dimensions::User*    > userDimensions;
    std::vector< Concrete::Dimensions::Link*    > linkDimensions;
    // clang-format on

    void findOperationVertices( Derivation::Node* pNode, std::vector< Concrete::Graph::Vertex* >& vertices )
    {
        auto edges = pNode->get_edges();
        if( edges.empty() )
        {
            auto pStep = db_cast< Derivation::Step >( pNode );
            VERIFY_RTE( pStep );
            vertices.push_back( pStep->get_vertex() );
        }
        else
        {
            for( auto pEdge : edges )
            {
                if( !pEdge->get_eliminated() )
                {
                    findOperationVertices( pEdge->get_next(), vertices );
                }
            }
        }

    }

    void classifyOperations()
    {
        std::vector< Concrete::Graph::Vertex* > operationContexts;
        findOperationVertices( m_pInvocation->get_derivation(), operationContexts );

        for( auto pVert : operationContexts )
        {
            if( auto p = db_cast< Concrete::Object >( pVert ) )
            {
                objects.push_back( p );
                VERIFY_RTE_MSG( ( m_targetType == eUNSET ) || ( m_targetType == eObjects ),
                                "Conflicting target types for invocation: " << m_pInvocation->get_id() );
                m_targetType = eObjects;
            }
            else if( auto p = db_cast< Concrete::Component >( pVert ) )
            {
                components.push_back( p );
                VERIFY_RTE_MSG( ( m_targetType == eUNSET ) || ( m_targetType == eComponents ),
                                "Conflicting target types for invocation: " << m_pInvocation->get_id() );
                m_targetType = eComponents;
            }
            else if( auto p = db_cast< Concrete::State >( pVert ) )
            {
                states.push_back( p );
                VERIFY_RTE_MSG( ( m_targetType == eUNSET ) || ( m_targetType == eStates ),
                                "Conflicting target types for invocation: " << m_pInvocation->get_id() );
                m_targetType = eStates;
            }
            else if( auto p = db_cast< Concrete::Decider >( pVert ) )
            {
                deciders.push_back( p );
                VERIFY_RTE_MSG( ( m_targetType == eUNSET ) || ( m_targetType == eDeciders ),
                                "Conflicting target types for invocation: " << m_pInvocation->get_id() );
                m_targetType = eDeciders;
            }
            else if( auto p = db_cast< Concrete::Function >( pVert ) )
            {
                functions.push_back( p );
                VERIFY_RTE_MSG( ( m_targetType == eUNSET ) || ( m_targetType == eFunctions ),
                                "Conflicting target types for invocation: " << m_pInvocation->get_id() );
                m_targetType = eFunctions;
            }
            else if( auto p = db_cast< Concrete::Namespace >( pVert ) )
            {
                namespaces.push_back( p );
            }
            else if( auto p = db_cast< Concrete::Event >( pVert ) )
            {
                events.push_back( p );
                VERIFY_RTE_MSG( ( m_targetType == eUNSET ) || ( m_targetType == eEvents ),
                                "Conflicting target types for invocation: " << m_pInvocation->get_id() );
                m_targetType = eEvents;
            }
            else if( auto p = db_cast< Concrete::Interupt >( pVert ) )
            {
                interupts.push_back( p );
            }
            else if( auto p = db_cast< Concrete::Dimensions::User >( pVert ) )
            {
                userDimensions.push_back( p );
                VERIFY_RTE_MSG( ( m_targetType == eUNSET ) || ( m_targetType == eUserDimensions ),
                                "Conflicting target types for invocation: " << m_pInvocation->get_id() );
                m_targetType = eUserDimensions;
            }
            else if( auto p = db_cast< Concrete::Dimensions::Link >( pVert ) )
            {
                linkDimensions.push_back( p );
                VERIFY_RTE_MSG( ( m_targetType == eUNSET ) || ( m_targetType == eLinkDimensions ),
                                "Conflicting target types for invocation: " << m_pInvocation->get_id() );
                m_targetType = eLinkDimensions;
            }
        }

        VERIFY_RTE_MSG( namespaces.empty(), "Invalid invocation on namespace: " << m_pInvocation->get_id() );
        VERIFY_RTE_MSG( interupts.empty(), "Invalid invocation on interupt: " << m_pInvocation->get_id() );
        VERIFY_RTE_MSG( m_targetType != eUNSET, "No targe type for invocation: " << m_pInvocation->get_id() );

        std::set< std::string > dimensionTypes;
        for( auto pDim : userDimensions )
        {
            dimensionTypes.insert( pDim->get_interface_dimension()->get_canonical_type() );
        }
    }

    void buildNoParams()
    {
        switch( m_targetType )
        {
            case eObjects:
                THROW_RTE( "Implicit invocation cannot be on object context" );
                break;
            case eComponents:
                THROW_RTE( "Implicit invocation cannot be on component context" );
                break;
            case eStates:
            {
                // return type is the started state
                {
                    std::vector< Interface::IContext* > contexts;
                    for( auto pConcrete : states )
                    {
                        contexts.push_back( pConcrete->get_interface() );
                    }
                    contexts = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                }

                m_database.construct< Start >( Start::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Start );
            }
            break;
            case eDeciders:
            {
                // return type is the function return type
                {
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Bool >(
                        ReturnTypes::Bool::Args{ ReturnTypes::ReturnType::Args{} } ) );
                }

                m_database.construct< Call >( Call::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Call );
            }
            break;
            case eFunctions:
            {
                // return type is the function return type
                {
                    std::vector< Interface::Function* > contexts;
                    std::set< std::string >             types;
                    for( auto pConcrete : functions )
                    {
                        contexts.push_back( pConcrete->get_interface_function() );
                        types.insert(
                            pConcrete->get_interface_function()->get_return_type_trait()->get_canonical_type() );
                    }
                    const bool bHomogenous = ( types.size() == 1 ) ? true : false;
                    contexts               = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Function >(
                        ReturnTypes::Function::Args{ ReturnTypes::ReturnType::Args{}, contexts, bHomogenous } ) );
                }

                m_database.construct< Call >( Call::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Call );
            }
            break;
            case eEvents:
            {
                // return type is the signaled event
                {
                    std::vector< Interface::IContext* > contexts;
                    for( auto pConcrete : events )
                    {
                        contexts.push_back( pConcrete->get_interface() );
                    }
                    contexts = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                }

                m_database.construct< Signal >( Signal::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Signal );
            }
            break;
            case eUserDimensions:
            {
                // return type is the read dimensions
                {
                    std::vector< Interface::DimensionTrait* > contexts;
                    std::set< std::string >                   types;
                    for( auto pConcrete : userDimensions )
                    {
                        auto pDimensionTrait = pConcrete->get_interface_dimension();
                        contexts.push_back( pDimensionTrait );
                        types.insert( pDimensionTrait->get_canonical_type() );
                    }
                    const bool bHomogenous = ( types.size() == 1 ) ? true : false;
                    contexts               = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Dimension >(
                        ReturnTypes::Dimension::Args{ ReturnTypes::ReturnType::Args{}, contexts, bHomogenous } ) );
                }

                m_database.construct< Read >( Read::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Read );
            }
            break;
            case eLinkDimensions:
            {
                // return type is the target of the link
                {
                    std::vector< Interface::IContext* > targets;
                    bool                                bSingular = true;
                    for( auto pConcrete : linkDimensions )
                    {
                        bool bFound = false;
                        for( auto pGraphEdge : pConcrete->get_out_edges() )
                        {
                            switch( pGraphEdge->get_type().get() )
                            {
                                case EdgeType::eMonoSingularMandatory:
                                case EdgeType::ePolySingularMandatory:
                                case EdgeType::eMonoSingularOptional:
                                case EdgeType::ePolySingularOptional:
                                case EdgeType::ePolyParent:
                                {
                                    // VERIFY_RTE( !bFound );
                                    auto pTargetContext
                                        = db_cast< Concrete::Dimensions::Link >( pGraphEdge->get_target() );
                                    VERIFY_RTE( pTargetContext );
                                    auto pParentContext = pTargetContext->get_parent_context();
                                    targets.push_back( pParentContext->get_interface() );
                                    bFound = true;
                                }
                                break;

                                case EdgeType::eMonoNonSingularMandatory:
                                case EdgeType::ePolyNonSingularMandatory:
                                case EdgeType::eMonoNonSingularOptional:
                                case EdgeType::ePolyNonSingularOptional:
                                {
                                    bSingular = false;
                                    // VERIFY_RTE( !bFound );
                                    auto pTargetContext
                                        = db_cast< Concrete::Dimensions::Link >( pGraphEdge->get_target() );
                                    VERIFY_RTE( pTargetContext );
                                    auto pParentContext = pTargetContext->get_parent_context();
                                    targets.push_back( pParentContext->get_interface() );
                                    bFound = true;
                                }
                                break;
                            }
                        }
                        VERIFY_RTE( bFound );
                    }

                    targets = make_unique_without_reorder( targets );
                    if( bSingular )
                    {
                        m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                            ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, targets } ) );
                    }
                    else
                    {
                        m_pInvocation->set_return_type(
                            m_database.construct< ReturnTypes::Range >( ReturnTypes::Range::Args{
                                ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, targets } } ) );
                    }
                }

                m_database.construct< LinkRead >( LinkRead::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Link_Read );
            }
            break;
            case eUNSET:
            default:
                UNREACHABLE;
                break;
        }
    }

    void buildParams()
    {
        switch( m_targetType )
        {
            case eObjects:
                THROW_RTE( "Implicit invocation cannot be on object context" );
                break;
            case eComponents:
                THROW_RTE( "Implicit invocation cannot be on component context" );
                break;
            case eStates:
                THROW_RTE( "Start operation cannot have parameters" );
                break;
            case eDeciders:
                THROW_RTE( "Decider cannot be invoked" );
                break;
            case eFunctions:
            {
                // return type is the function return type
                {
                    std::vector< Interface::Function* > contexts;
                    std::set< std::string >             types;
                    for( auto pConcrete : functions )
                    {
                        contexts.push_back( pConcrete->get_interface_function() );
                        types.insert(
                            pConcrete->get_interface_function()->get_return_type_trait()->get_canonical_type() );
                    }
                    const bool bHomogenous = ( types.size() == 1 ) ? true : false;
                    contexts               = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Function >(
                        ReturnTypes::Function::Args{ ReturnTypes::ReturnType::Args{}, contexts, bHomogenous } ) );
                }

                m_database.construct< Call >( Call::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Call );
            }
            break;
            case eEvents:
            {
                THROW_RTE( "Event operation cannot have parameters" );
            }
            break;
            case eUserDimensions:
            {
                // return the context of the write
                {
                    std::vector< Interface::IContext* > contexts;
                    for( auto pConcrete : userDimensions )
                    {
                        contexts.push_back( pConcrete->get_parent_context()->get_interface() );
                    }
                    contexts = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                }
                // parameter type
                ReturnTypes::Dimension* pParameterType = nullptr;
                {
                    std::vector< Interface::DimensionTrait* > contexts;
                    std::set< std::string >                   types;
                    for( auto pConcrete : userDimensions )
                    {
                        auto pDimensionTrait = pConcrete->get_interface_dimension();
                        contexts.push_back( pDimensionTrait );
                        types.insert( pDimensionTrait->get_canonical_type() );
                    }
                    const bool bHomogenous = ( types.size() == 1 ) ? true : false;
                    contexts               = make_unique_without_reorder( contexts );
                    pParameterType         = m_database.construct< ReturnTypes::Dimension >(
                        ReturnTypes::Dimension::Args{ ReturnTypes::ReturnType::Args{}, contexts, bHomogenous } );
                }

                m_database.construct< Write >( Write::Args{ m_pInvocation, pParameterType } );
                m_pInvocation->set_explicit_operation( id_exp_Write );
            }
            break;
            case eLinkDimensions:
            {
                // return the context of the write
                {
                    std::vector< Interface::IContext* > contexts;
                    for( auto pConcrete : linkDimensions )
                    {
                        contexts.push_back( pConcrete->get_parent_context()->get_interface() );
                    }
                    contexts = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                }

                m_database.construct< LinkAdd >( LinkAdd::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Link_Add );
            }
            break;
            case eUNSET:
            default:
                UNREACHABLE;
                break;
        }
    }

    void buildGet()
    {
        switch( m_targetType )
        {
            case eObjects:
            {
                // reference context of object
                {
                    std::vector< Interface::IContext* > contexts;
                    for( auto pConcrete : objects )
                    {
                        contexts.push_back( pConcrete->get_interface() );
                    }
                    contexts = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                }

                m_database.construct< GetContext >( GetContext::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_GetContext );
            }
            break;
            case eComponents:
            {
                // reference context of component
                {
                    std::vector< Interface::IContext* > contexts;
                    for( auto pConcrete : components )
                    {
                        contexts.push_back( pConcrete->get_interface() );
                    }
                    contexts = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                }

                m_database.construct< GetContext >( GetContext::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_GetContext );
            }
            break;
            case eStates:
            {
                // reference context of state
                {
                    std::vector< Interface::IContext* > contexts;
                    for( auto pConcrete : states )
                    {
                        contexts.push_back( pConcrete->get_interface() );
                    }
                    contexts = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                }

                m_database.construct< GetContext >( GetContext::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_GetContext );
            }
            break;
            case eDeciders:
            {
                THROW_RTE( "Cannot get a decider" );
            }
            break;
            case eFunctions:
            {
                // reference context of function
                {
                    std::vector< Interface::IContext* > contexts;
                    for( auto pConcrete : functions )
                    {
                        contexts.push_back( pConcrete->get_interface() );
                    }
                    contexts = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                }

                m_database.construct< GetContext >( GetContext::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_GetContext );
            }
            break;
            case eEvents:
            {
                // reference context of events
                {
                    std::vector< Interface::IContext* > contexts;
                    for( auto pConcrete : events )
                    {
                        contexts.push_back( pConcrete->get_interface() );
                    }
                    contexts = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                }

                m_database.construct< GetContext >( GetContext::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_GetContext );
            }
            break;
            case eUserDimensions:
                THROW_RTE( "Cannot get a dimension" );
                break;
            case eLinkDimensions:
                THROW_RTE( "Cannot get a link" );
                break;
            case eUNSET:
            default:
                UNREACHABLE;
                break;
        }
    }

    void buildRemove()
    {
        switch( m_targetType )
        {
            case eObjects:
                THROW_RTE( "Cannot remove an object" );
                break;
            case eComponents:
                THROW_RTE( "Cannot remove a component" );
                break;
            case eStates:
                THROW_RTE( "Cannot remove a state" );
                break;
            case eDeciders:
                THROW_RTE( "Cannot remove a decider" );
                break;
            case eFunctions:
                THROW_RTE( "Cannot remove a function" );
                break;
            case eEvents:
                THROW_RTE( "Cannot remove an event" );
                break;
            case eUserDimensions:
                THROW_RTE( "Cannot remove a dimension" );
                break;
            case eLinkDimensions:
            {
                // parameter type
                /*ReturnTypes::Context* pParameterType = nullptr;
                {
                    std::vector< Interface::DimensionTrait* > contexts;
                    std::set< std::string >                   types;
                    for( auto pConcrete : linkDimensions )
                    {
                        auto pDimensionTrait = pConcrete->get_interface_dimension();
                        contexts.push_back( pDimensionTrait );
                        types.insert( pDimensionTrait->get_canonical_type() );
                    }
                    contexts               = make_unique_without_reorder( contexts );
                    pParameterType         = m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } );
                }*/
                // return the context of the write
                {
                    std::vector< Interface::IContext* > contexts;
                    for( auto pConcrete : linkDimensions )
                    {
                        contexts.push_back( pConcrete->get_parent_context()->get_interface() );
                    }
                    contexts = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                }

                m_database.construct< LinkRemove >( LinkRemove::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Link_Remove );
            }
            break;
            case eUNSET:
            default:
                UNREACHABLE;
                break;
        }
    }

    void buildClear()
    {
        switch( m_targetType )
        {
            case eObjects:
                THROW_RTE( "Cannot clear an object" );
                break;
            case eComponents:
                THROW_RTE( "Cannot clear a component" );
                break;
            case eStates:
                THROW_RTE( "Cannot clear a state" );
                break;
            case eDeciders:
                THROW_RTE( "Cannot clear a decider" );
                break;
            case eFunctions:
                THROW_RTE( "Cannot clear a function" );
                break;
            case eEvents:
                THROW_RTE( "Cannot clear an event" );
                break;
            case eUserDimensions:
                THROW_RTE( "Cannot clear a dimension" );
                break;
            case eLinkDimensions:
            {
                // return the context of the write
                {
                    std::vector< Interface::IContext* > contexts;
                    for( auto pConcrete : linkDimensions )
                    {
                        contexts.push_back( pConcrete->get_parent_context()->get_interface() );
                    }
                    contexts = make_unique_without_reorder( contexts );
                    m_pInvocation->set_return_type( m_database.construct< ReturnTypes::Context >(
                        ReturnTypes::Context::Args{ ReturnTypes::ReturnType::Args{}, contexts } ) );
                }

                m_database.construct< LinkClear >( LinkClear::Args{ m_pInvocation } );
                m_pInvocation->set_explicit_operation( id_exp_Link_Clear );
            }
            break;
            case eUNSET:
            default:
                UNREACHABLE;
                break;
        }
    }

    void buildMove()
    {
        THROW_TODO;
        switch( m_targetType )
        {
            case eObjects:
            case eComponents:
            case eStates:
            case eDeciders:
            case eFunctions:
            case eEvents:
            case eUserDimensions:
            case eLinkDimensions:
            case eUNSET:
            default:
                UNREACHABLE;
                break;
        }
        // m_database.construct< Move >( Move::Args{ m_pInvocation } );
        // m_pInvocation->set_explicit_operation( id_exp_Move );
    }

    void buildRange()
    {
        THROW_TODO;
        switch( m_targetType )
        {
            case eObjects:
            case eComponents:
            case eStates:
            case eDeciders:
            case eFunctions:
            case eEvents:
            case eUserDimensions:
            case eLinkDimensions:
            case eUNSET:
            default:
                UNREACHABLE;
                break;
        }
        // m_database.construct< Range >( Range::Args{ m_pInvocation } );
        // m_pInvocation->set_explicit_operation( id_exp_Range );
    }

public:
    OperationBuilder( Database& database, Operations::Invocation* pInvocation )
        : m_database( database )
        , m_pInvocation( pInvocation )
    {
    }

    void build()
    {
        classifyOperations();

        m_pInvocation->set_explicit_operation( HIGHEST_EXPLICIT_OPERATION_TYPE );

        switch( m_pInvocation->get_id().m_operation )
        {
            case mega::id_Imp_NoParams:
            {
                buildNoParams();
            }
            break;
            case mega::id_Imp_Params:
            {
                buildParams();
            }
            break;
            case mega::id_Move:
            {
                buildMove();
            }
            break;
            case mega::id_Get:
            {
                buildGet();
            }
            break;
            case mega::id_Range:
            {
                buildRange();
            }
            break;
            case mega::id_Remove:
            {
                buildRemove();
            }
            break;
            case mega::id_Clear:
            {
                buildClear();
            }
            break;
            default:
            case mega::HIGHEST_OPERATION_TYPE:
            {
                THROW_RTE( "Unknown implicit operation type" );
            }
            break;
        }

        VERIFY_RTE_MSG( m_pInvocation->get_explicit_operation() != HIGHEST_EXPLICIT_OPERATION_TYPE,
                        "Failed to determine invocation explicit operation type: " << m_pInvocation->get_id() );
    }
};

} // namespace

Operations::Invocation* compileInvocation( Database& database, const SymbolTables& symbolTables,
                                           const mega::InvocationID& id )
{
    // determine the derivation of the invocationID
    std::vector< Concrete::Graph::Vertex* > contextTypes;
    InvocationPolicy::Spec                  derivationSpec;
    fromInvocationID( symbolTables, id, contextTypes, derivationSpec );

    // solve the context free derivation
    InvocationPolicy              policy( database );
    InvocationPolicy::OrPtrVector finalFrontier;
    InvocationPolicy::RootPtr     pRoot = DerivationSolver::solveContextFree( derivationSpec, policy, finalFrontier );

    Derivation::precedence( pRoot );

    Operations::Invocation* pInvocation = nullptr;
    try
    {
        const Derivation::Disambiguation result = Derivation::disambiguate( pRoot, finalFrontier );
        if( result != Derivation::eSuccess )
        {
            std::ostringstream os;
            if( result == Derivation::eAmbiguous )
                os << "Derivation disambiguation was ambiguous for: " << id << "\n";
            else if( result == Derivation::eFailure )
                os << "Derivation disambiguation failed for: " << id << "\n";
            else
                THROW_RTE( "Unknown derivation failure type" );
            THROW_RTE( os.str() );
        }

        // is the invocation a link invocation?
        /*InvocationPolicy::AndPtrVector linkFrontier;
        if( ( id.m_operation == id_Imp_NoParams ) || ( id.m_operation == id_Imp_Params ) )
        {
            bool bTargetsObjectOrComponent = false;
            bool bTargetsOtherContextType  = false;
            for( auto pOr : finalFrontier )
            {
                if( db_cast< Concrete::Object >( pOr->get_vertex() )
                    || db_cast< Concrete::Component >( pOr->get_vertex() ) )
                {
                    bTargetsObjectOrComponent = true;
                }
                else
                {
                    bTargetsOtherContextType = true;
                }
            }
            VERIFY_RTE_MSG( !bTargetsObjectOrComponent || !bTargetsOtherContextType,
                            "Conficting target types for invocation: " << id );
            if( bTargetsObjectOrComponent )
            {
                // invocation IS a link read or write operation
                DerivationSolver::backtrackToLinkDimensions( pRoot, policy, linkFrontier );
            }
        }*/

        {
            pInvocation = database.construct< Operations::Invocation >( Operations::Invocation::Args{ pRoot, id } );
        }

        {
            OperationBuilder builder( database, pInvocation );
            builder.build();
        }
    }
    catch( std::exception& ex )
    {
        std::ostringstream os;
        os << "Exception while compiling invocation: " << id << "\n";
        printDerivationStep( pRoot, true, os );
        os << "\nError: " << ex.what();
        THROW_RTE( os.str() );
    }

    return pInvocation;
}

} // namespace mega::invocation
