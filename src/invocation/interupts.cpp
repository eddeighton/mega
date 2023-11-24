
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

#include "mega/values/compilation/hyper_graph.hpp"
#include "mega/values/compilation/operation_id.hpp"

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
#include "compiler/symbol_variant_printer.hpp"
#include "compiler/derivation_compiler.hpp"
namespace Derivation
{
#include "compiler/derivation_printer.hpp"
#include "compiler/disambiguate.hpp"
} // namespace Derivation
} // namespace OperationsStage

using namespace OperationsStage;

namespace mega
{
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
        return CommonAncestor::commonRootDerivation( pSource, pTarget, edges, true );
    }

    InvocationPolicy( Database& database )
        : InvocationPolicyBase( database )
    {
    }
};

std::vector< Derivation::Dispatch* > buildEventDispatches( Database& database, Derivation::Edge* pEdge )
{
    std::vector< Derivation::Dispatch* > dispatches;

    auto pCurrentStep = pEdge->get_next();

    auto edges = pCurrentStep->get_edges();
    if( edges.empty() )
    {
        auto pEventVertex = pCurrentStep->get_vertex();
        auto pEventState  = db_cast< Concrete::State >( pEventVertex );
        auto pEventEvent  = db_cast< Concrete::Event >( pEventVertex );
        VERIFY_RTE_MSG( pEventState || pEventEvent, "Interupt event does NOT specify an Event or State" );

        // create event dispatch
        auto pDispatch = database.construct< Derivation::Dispatch >( Derivation::Dispatch::Args{
            Derivation::Step::Args{ Derivation::Node::Args{ {} }, pCurrentStep->get_vertex() } } );
        dispatches.push_back( pDispatch );
    }
    else
    {
        for( auto p : edges )
        {
            if( !p->get_eliminated() )
            {
                auto result = buildEventDispatches( database, p );
                std::copy( result.begin(), result.end(), std::back_inserter( dispatches ) );
            }
        }
    }

    for( Derivation::Dispatch* pDispatch : dispatches )
    {
        Derivation::Step* pLastStep = pDispatch;
        while( !pLastStep->get_edges().empty() )
        {
            auto edges = pLastStep->get_edges();
            VERIFY_RTE( edges.size() == 1 );
            pLastStep = edges.front()->get_next();
            VERIFY_RTE( pLastStep );
        }

        // solve inverse step to the parent
        auto pSourceVertex = pLastStep->get_vertex();
        if( auto pTargetStep = db_cast< Derivation::Step >( pEdge->get_from() ) )
        {
            auto pTargetVertex = pTargetStep->get_vertex();
            if( auto pTargetOr = db_cast< Derivation::Or >( pTargetStep ) )
            {
                if( pSourceVertex != pTargetVertex )
                {
                    // common root derivation to or vertex
                    std::vector< Concrete::Graph::Edge* > edges;
                    VERIFY_RTE( CommonAncestor::commonRootDerivation( pSourceVertex, pTargetVertex, edges, true ) );

                    // initially only create the steps - which can be refined in second process
                    // to determine if Select or Or
                    auto pNextStep = database.construct< Derivation::Or >(
                        Derivation::Or::Args{ Derivation::Step::Args{ Derivation::Node::Args{ {} }, pTargetVertex } } );

                    auto pDispatchEdge = database.construct< Derivation::Edge >(
                        Derivation::Edge::Args{ pLastStep, pNextStep, false, false, 0, edges } );

                    pLastStep->push_back_edges( pDispatchEdge );
                }
            }
            else if( auto pTargetAnd = db_cast< Derivation::And >( pTargetStep ) )
            {
                Concrete::Graph::Vertex* pGraphInterObjectLinkVertex = nullptr;
                {
                    // previous step MUST be an Or
                    auto pLastOr = db_cast< Derivation::Or >( pLastStep );
                    VERIFY_RTE( pLastOr );

                    // the And MUST be on a link vertex
                    VERIFY_RTE( db_cast< Concrete::Dimensions::Link >( pTargetAnd->get_vertex() ) );

                    // and for interupt policy ALWAYS has two edges
                    auto graphEdges = pEdge->get_edges();
                    VERIFY_RTE( graphEdges.size() == 2 );

                    // the first of which is the inter-object hypergraph edge
                    auto pGraphInterObjectEdge = graphEdges[ 0 ];

                    // which MUST have a source of the target vertex
                    VERIFY_RTE( pGraphInterObjectEdge->get_source() == pTargetVertex );

                    // get the link on the other side
                    pGraphInterObjectLinkVertex = pGraphInterObjectEdge->get_target();
                    VERIFY_RTE( db_cast< Concrete::Dimensions::Link >( pGraphInterObjectLinkVertex ) );
                }

                // create the Selection to perform the inter-object derivation step
                auto pSelect = database.construct< Derivation::Select >(
                    Derivation::Select::Args{ Derivation::Or::Args{ Derivation::Step::Args{
                                                  Derivation::Node::Args{ {} }, pGraphInterObjectLinkVertex } },
                                              pTargetVertex } );

                // and construct the edge from the last step to the selection with the common root derivation edges
                {
                    // solve common root derivation to the link from the current vertex
                    std::vector< Concrete::Graph::Edge* > edges;
                    VERIFY_RTE( CommonAncestor::commonRootDerivation(
                        pSourceVertex, pGraphInterObjectLinkVertex, edges, true ) );

                    auto pEdgeToSelect = database.construct< Derivation::Edge >(
                        Derivation::Edge::Args{ pLastStep, pSelect, false, false, 0, edges } );
                    pLastStep->push_back_edges( pEdgeToSelect );
                }

                // determine the inverse inter-object edge of pGraphInterObjectEdge
                Concrete::Graph::Edge* pInverseInterObjectEdge = nullptr;
                {
                    for( auto pIter : pGraphInterObjectLinkVertex->get_out_edges() )
                    {
                        if( pIter->get_target() == pTargetVertex )
                        {
                            VERIFY_RTE_MSG( !pInverseInterObjectEdge, "Found duplicate hyper graph edges to target" );
                            pInverseInterObjectEdge = pIter;
                        }
                    }
                    VERIFY_RTE( pInverseInterObjectEdge );
                }

                // can now construct an Or vertex at the target which the Selection derives
                auto pFinalOrStep = database.construct< Derivation::Or >(
                    Derivation::Or::Args{ Derivation::Step::Args{ Derivation::Node::Args{ {} }, pTargetVertex } } );

                // finally create the edge for the select using the interobject hypergraph edge
                {
                    auto pSelectEdge = database.construct< Derivation::Edge >(
                        Derivation::Edge::Args{ pSelect, pFinalOrStep, false, false, 0, { pInverseInterObjectEdge } } );
                    pSelect->push_back_edges( pSelectEdge );
                }
            }
            else
            {
                THROW_RTE( "Unknown derivation step type" );
            }
        }
        else if( auto pTargetRoot = db_cast< Derivation::Root >( pEdge->get_from() ) )
        {
            // do nothing
        }
        else
        {
            THROW_RTE( "Unknown derivation node type" );
        }
    }

    return dispatches;
}

void buildEventDispatches( Database& database, InvocationPolicy::RootPtr pDerivationRoot,
                           std::vector< Derivation::Dispatch* >& eventDispatches )
{
    for( auto pEdge : pDerivationRoot->get_edges() )
    {
        if( !pEdge->get_eliminated() )
        {
            auto dispatches = buildEventDispatches( database, pEdge );
            std::copy( dispatches.begin(), dispatches.end(), std::back_inserter( eventDispatches ) );
        }
    }
}

} // namespace
} // namespace mega

namespace mega::invocation
{

void compileInterupts( Database& database, const mega::io::megaFilePath& sourceFile )
{
    for( auto pInterupt : database.many< Concrete::Interupt >( sourceFile ) )
    {
        std::vector< InvocationPolicy::RootPtr >  derivations;
        std::vector< Operations::EventDispatch* > eventDispatches;

        auto pEventTraitOpt = pInterupt->get_interface_interupt()->get_events_trait_opt();
        if( pEventTraitOpt.has_value() )
        {
            auto pEventTrait = pEventTraitOpt.value();
            for( auto pSymbolVariantSequence : pEventTrait->get_symbol_variant_sequences() )
            {
                InvocationPolicy::Spec derivationSpec( pInterupt, pSymbolVariantSequence, true );

                // solve the context free derivation
                InvocationPolicy              policy( database );
                InvocationPolicy::OrPtrVector finalFrontier;
                InvocationPolicy::RootPtr     pRoot
                    = DerivationSolver::solveContextFree( derivationSpec, policy, finalFrontier );

                VERIFY_RTE_MSG( !finalFrontier.empty(),
                                "Interupt derivation failed for: "
                                    << Concrete::printContextFullType( pInterupt ) << " "
                                    << Interface::printSymbolVariantSequence( pSymbolVariantSequence ) << "\n"
                                    << printDerivationStep( pRoot, true ) );

                Derivation::precedence( pRoot );
                derivations.push_back( pRoot );

                try
                {
                    // analysis inverse event dispatches ...
                    std::vector< Derivation::Dispatch* > dispatches;
                    buildEventDispatches( database, pRoot, dispatches );

                    for( auto pDispatch : dispatches )
                    {
                        auto pEvent = db_cast< Concrete::Context >( pDispatch->get_vertex() );
                        VERIFY_RTE( pEvent );
                        auto pEventDispatch
                            = database.construct< Operations::EventDispatch >( Operations::EventDispatch::Args{
                                pDispatch, pEvent, pInterupt } );
                        eventDispatches.push_back( pEventDispatch );
                    }
                }
                catch( std::exception& ex )
                {
                    std::ostringstream os;
                    os << "Exception while compiling interupt dispatch for: "
                       << Concrete::printContextFullType( pInterupt ) << "\n";
                    printDerivationStep( pRoot, true, os );
                    os << "\nError: " << ex.what();
                    THROW_RTE( os.str() );
                }
            }
        }

        pInterupt->set_events( derivations );
        pInterupt->set_event_dispatches( eventDispatches );
    }
}

} // namespace mega::invocation