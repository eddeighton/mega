
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
                Derivation::And::Args{ Derivation::Step::Args{ pLink, {} } } );

            auto pOrToAndEdge
                = m_database.construct< Derivation::Edge >( Derivation::Edge::Args{ pAnd, false, false, 0, {} } );

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
                    Derivation::Or::Args{ Derivation::Step::Args{ pParentVertex, {} } } );

                auto pDerivationEdge = m_database.construct< Derivation::Edge >(
                    Derivation::Edge::Args{ pLinkTargetOr, false, false, 0, { pGraphEdge, pParentEdge } } );

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

} // namespace
} // namespace mega

namespace mega::invocation
{

void compileInterupts( Database& database, const mega::io::megaFilePath& sourceFile )
{
    using namespace OperationsStage;

    for( auto pInterupt : database.many< Concrete::Interupt >( sourceFile ) )
    {
        std::vector< InvocationPolicy::RootPtr > derivations;

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
                    "Interupt derivation failed for: " << Concrete::printContextFullType( pInterupt ) <<
                    " " << Interface::printSymbolVariantSequence( pSymbolVariantSequence ) <<
                    "\n" << printDerivationStep( pRoot, true ) );

                Derivation::precedence( pRoot );

                try
                {
                    const Derivation::Disambiguation result
                        = Derivation::eSuccess; // Derivation::disambiguate( pRoot, finalFrontier );
                    if( result != Derivation::eSuccess )
                    {
                        std::ostringstream os;
                        using ::           operator<<;
                        if( result == Derivation::eAmbiguous )
                            os << "Derivation disambiguation was ambiguous for: "
                                << Concrete::printContextFullType( pInterupt ) << "\n";
                        else if( result == Derivation::eFailure )
                            os << "Derivation disambiguation failed for: "
                                << Concrete::printContextFullType( pInterupt ) << "\n";
                        else
                            THROW_RTE( "Unknown derivation failure type" );
                        THROW_RTE( os.str() );
                    }
                    else
                    {
                        derivations.push_back( pRoot );
                    }
                }
                catch( std::exception& ex )
                {
                    std::ostringstream os;
                    os << "Exception while compiling event for: " << Concrete::printContextFullType( pInterupt )
                        << "\n";
                    printDerivationStep( pRoot, true, os );
                    os << "\nError: " << ex.what();
                    THROW_RTE( os.str() );
                }
            }
        }

        pInterupt->set_events( derivations );
    }
}

} // namespace mega::invocation
