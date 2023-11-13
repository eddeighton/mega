
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
struct InvocationPolicy
{
    using GraphVertex             = OperationsStage::Concrete::Graph::Vertex;
    using GraphEdge               = OperationsStage::Concrete::Graph::Edge;
    using GraphVertexVector       = std::vector< GraphVertex* >;
    using GraphVertexSet          = std::unordered_set< GraphVertex* >;
    using GraphVertexVectorVector = std::vector< GraphVertexVector >;
    using GraphEdgeVector         = std::vector< GraphEdge* >;

    struct Spec
    {
        GraphVertexVector       context;
        GraphVertexVectorVector path;
    };

    using StepPtr      = OperationsStage::Derivation::Step*;
    using EdgePtr      = OperationsStage::Derivation::Edge*;
    using OrPtr        = OperationsStage::Derivation::Or*;
    using OrPtrVector  = std::vector< OrPtr >;
    using AndPtr       = OperationsStage::Derivation::And*;
    using AndPtrVector = std::vector< AndPtr >;
    using RootPtr      = OperationsStage::Derivation::Root*;

    EdgePtr makeEdge( StepPtr pNext, const GraphEdgeVector& edges ) const
    {
        // initially no edges are eliminated
        return m_database.construct< Derivation::Edge >( Derivation::Edge::Args{ pNext, false, false, 0, edges } );
    }
    OrPtr makeOr( GraphVertex* pVertex ) const
    {
        return m_database.construct< Derivation::Or >( Derivation::Or::Args{ Derivation::Step::Args{ pVertex, {} } } );
    }
    /*AndPtr makeAnd( GraphVertex* pVertex ) const
    {
        return m_database.construct< Derivation::And >(
            Derivation::And::Args{ Derivation::Step::Args{ pVertex, {} } } );
    }*/
    RootPtr makeRoot( const GraphVertexVector& context ) const
    {
        return m_database.construct< Derivation::Root >( Derivation::Root::Args{ context, {} } );
    }
    EdgePtr makeRootEdge( OrPtr pNext ) const
    {
        return m_database.construct< Derivation::Edge >( Derivation::Edge::Args{ pNext, false, false, 0, {} } );
    }
    bool   isLinkDimension( GraphVertex* pVertex ) const { return db_cast< Concrete::Dimensions::Link >( pVertex ); }
    AndPtr isAndStep( StepPtr pStep ) const { return db_cast< OperationsStage::Derivation::And >( pStep ); }
    void   backtrack( EdgePtr pEdge ) const { pEdge->set_backtracked( true ); }

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
        : m_database( database )
    {
    }

private:
    Database& m_database;
};

} // namespace
} // namespace mega

namespace mega::invocation
{

void compileDeciders( OperationsStage::Database& database, const mega::io::megaFilePath& sourceFile )
{
    using namespace OperationsStage;

    for( auto pDecider : database.many< Concrete::Decider >( sourceFile ) )
    {
        std::vector< InvocationPolicy::RootPtr > derivations;

        auto pEventTrait = pDecider->get_interface_decider()->get_events_trait();

        for( auto pTypePathVariant : pEventTrait->get_tuple() )
        {
            InvocationPolicy::Spec derivationSpec{ { pDecider } };

            for( auto pSequence : pTypePathVariant->get_sequence() )
            {
                for( auto pSymbol : pSequence->get_types() )
                {
                    InvocationPolicy::GraphVertexVector pathElement;
                    for( auto pContext : pSymbol->get_contexts() )
                    {
                        for( auto pConcrete : pContext->get_concrete() )
                        {
                            pathElement.push_back( pConcrete );
                        }
                    }
                    VERIFY_RTE_MSG( !pathElement.empty(), "Event contains invalid symbols" );
                    if( !pathElement.empty() )
                    {
                        derivationSpec.path.push_back( pathElement );
                    }
                }

                // solve the context free derivation
                InvocationPolicy              policy( database );
                InvocationPolicy::OrPtrVector finalFrontier;
                InvocationPolicy::RootPtr     pRoot
                    = DerivationSolver::solveContextFree( derivationSpec, policy, finalFrontier );

                Derivation::precedence( pRoot );

                try
                {
                    const Derivation::Disambiguation result = Derivation::disambiguate( pRoot, finalFrontier );
                    if( result != Derivation::eSuccess )
                    {
                        std::ostringstream os;
                        using ::           operator<<;
                        if( result == Derivation::eAmbiguous )
                            os << "Derivation disambiguation was ambiguous for: "
                               << Concrete::printContextFullType( pDecider ) << "\n";
                        else if( result == Derivation::eFailure )
                            os << "Derivation disambiguation failed for: " << Concrete::printContextFullType( pDecider )
                               << "\n";
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
                    os << "Exception while compiling event for: " << Concrete::printContextFullType( pDecider ) << "\n";
                    printDerivationStep( pRoot, true, os );
                    os << "\nError: " << ex.what();
                    THROW_RTE( os.str() );
                }
            }
        }

        database.construct< Concrete::Decider >( Concrete::Decider::Args{ pDecider, derivations } );
    }
}

} // namespace mega::invocation
