
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

namespace DerivationSolver
{
/*
struct ExamplePolicy
{
    using GraphVertex             = Concrete::Graph::Vertex;
    using GraphEdge               = Concrete::Graph::Edge;
    using GraphVertexVector       = std::vector< GraphVertex* >;
    using GraphVertexSet          = std::unordered_set< GraphVertex* >;
    using GraphVertexVectorVector = std::vector< GraphVertexVector >;
    using GraphEdgeVector         = std::vector< GraphEdge* >;

    struct Spec
    {
        VertexVector       context;
        VertexVectorVector path;
    };

    using StepPtr     = int;
    using EdgePtr     = int;
    using OrPtr       = int;
    using OrPtrVector = int;
    using AndPtr      = int;

    EdgePtr makeEdge( StepPtr pNext, const GraphEdgeVector& edges );
    OrPtr   makeOr( Vertex* pVertex );
    AndPtr  makeAnd( Vertex* pVertex );
    AndPtr  makeRoot();
    EdgePtr makeRootEdge( AndPtr pNext );

    // enumerate actual polymorphic branches of a given link
    GraphEdgeVector enumerateLink( Vertex* pLink ) const;
    // enumerate the dimension links within the parent vertex
    using LinkDimension = std::pair< GraphVertex*, Concrete::Graph::Edge* >;
    GraphEdgeVector     enumerateLinks( GraphVertex* pParentVertex ) const;
    GraphVertexVector   enumerateLinkContexts( GraphVertex* pVertex ) const;
    GraphVertex* commonRootDerivation( GraphVertex* pSource, GraphVertex* pTarget, GraphEdgeVector& edges ) const;
};*/

template < typename TPolicy >
static typename TPolicy::OrPtrVector solveStep( typename TPolicy::OrPtr                    pCurrentFrontierStep,
                                                const typename TPolicy::GraphVertexVector& typePathElement,
                                                const TPolicy&                             policy )
{
    typename TPolicy::OrPtrVector nextFrontier;

    auto pCurrentVertex = pCurrentFrontierStep->get_vertex();

    // attempt in-object common root derivation
    for( auto pTypePathVertex : typePathElement )
    {
        typename TPolicy::GraphEdgeVector edges;
        if( pCurrentVertex == pTypePathVertex )
        {
            nextFrontier.push_back( pCurrentFrontierStep );
        }
        else if( policy.commonRootDerivation( pCurrentVertex, pTypePathVertex, edges ) )
        {
            typename TPolicy::OrPtr   pNextStep = policy.makeOr( pTypePathVertex );
            typename TPolicy::EdgePtr pEdge     = policy.makeEdge( pNextStep, edges );
            pCurrentFrontierStep->push_back_edges( pEdge );
            nextFrontier.push_back( pNextStep );
        }
    }

    /*if( bAllowInterObjectStep )
    {
        for( auto pLinkContextVertex : policy.enumerateLinkContexts( pCurrentVertex ) )
        {
            // create common root if required ( which may fail )
            auto pStep = pCurrentFrontierStep;
            if( pCurrentVertex != pLinkContextVertex )
            {
                typename TPolicy::GraphEdgeVector edges;
                // if( policy.commonRootDerivation( pCurrentVertex, pLinkContextVertex, edges ) )
                // {
                //     pStep = policy.makeOr( pLinkContextVertex );
                //     pCurrentFrontierStep->push_back_edges( policy.makeEdge( pStep, edges ) );
                // }
                // else
                {
                    pStep = nullptr;
                }
            }
            if( pStep )
            {
                // if so generate the AND step
                for( auto pContextToLinkVertex : policy.enumerateLinks( pLinkContextVertex ) )
                {
                    auto pLinkTarget = pContextToLinkVertex->get_target();
                    auto linkEdges   = policy.enumerateLink( pLinkTarget );
                    if( !linkEdges.empty() )
                    {
                        typename TPolicy::AndPtr pBranch = policy.makeAnd( pLinkTarget );
                        pStep->push_back_edges(
                            policy.makeEdge( pBranch, typename TPolicy::GraphEdgeVector{ pContextToLinkVertex } ) );

                        for( auto pLinkEdge : linkEdges )
                        {
                            typename TPolicy::OrPtr pLinkedObjectToTarget = policy.makeOr( pLinkEdge->get_target() );
                            pBranch->push_back_edges( policy.makeEdge(
                                pLinkedObjectToTarget, typename TPolicy::GraphEdgeVector{ pLinkEdge } ) );

                            typename TPolicy::OrPtrVector recursiveResult
                                = solveStep( pLinkedObjectToTarget, typePathElement, false, policy );
                            std::copy(
                                recursiveResult.begin(), recursiveResult.end(), std::back_inserter( nextFrontier ) );
                        }
                    }
                }
            }
        }
    }*/

    return nextFrontier;
}

template < typename TPolicy >
static typename TPolicy::RootPtr solveContextFree( const typename TPolicy::Spec& spec, const TPolicy& policy,
                                                   typename TPolicy::OrPtrVector& frontier )
{
    typename TPolicy::RootPtr pSolutionRoot = policy.makeRoot( spec.context );

    for( auto pVert : spec.context )
    {
        typename TPolicy::OrPtr p = policy.makeOr( pVert );
        frontier.push_back( p );
        pSolutionRoot->push_back_edges( policy.makeRootEdge( p ) );
    }

    for( auto i = spec.path.begin(), iNext = spec.path.begin(), iEnd = spec.path.end(); i != iEnd; ++i )
    {
        ++iNext;

        const bool  bLast           = iNext == iEnd;
        const auto& typePathElement = *i;

        typename TPolicy::OrPtrVector nextFrontier;
        for( typename TPolicy::OrPtr pCurrentFrontierStep : frontier )
        {
            typename TPolicy::OrPtrVector recursiveResult = solveStep( pCurrentFrontierStep, typePathElement, policy );

            if( !bLast )
            {
                for( auto pOr : recursiveResult )
                {
                    auto linkFrontier = policy.expandLink( pOr );
                    std::copy( linkFrontier.begin(), linkFrontier.end(), std::back_inserter( nextFrontier ) );
                }
            }
            else
            {
                std::copy( recursiveResult.begin(), recursiveResult.end(), std::back_inserter( nextFrontier ) );
            }
        }
        nextFrontier.swap( frontier );
    }

    return pSolutionRoot;
}

// bottom up recursion
// return if the edge should be back tracked
/*
template < typename TPolicy >
static bool backtrackToLinkDimensionsRecurse( typename TPolicy::EdgePtr pEdge, const TPolicy& policy,
                                              typename TPolicy::AndPtrVector& frontier )
{
    auto pStep = pEdge->get_next();

    auto       pAnd                 = policy.isAndStep( pStep );
    const bool bIsLinkDimAndAndStep = pAnd && policy.isLinkDimension( pStep->get_vertex() );

    std::vector< typename TPolicy::EdgePtr > nonElimatedEdges;
    for( auto pEdge : pStep->get_edges() )
    {
        if( !pEdge->get_eliminated() )
        {
            nonElimatedEdges.push_back( pEdge );
        }
    }

    if( nonElimatedEdges.empty() )
    {
        return !bIsLinkDimAndAndStep;
    }

    int iBackTrackedEdges = 0;
    for( auto pNextEdge : nonElimatedEdges )
    {
        if( backtrackToLinkDimensionsRecurse( pNextEdge, policy, frontier ) )
        {
            policy.backtrack( pNextEdge );
            ++iBackTrackedEdges;
        }
    }
    VERIFY_RTE_MSG( ( iBackTrackedEdges == 0 ) || ( iBackTrackedEdges == nonElimatedEdges.size() ),
                    "Backtracking unbalenced in invocation" );

    if( bIsLinkDimAndAndStep && ( iBackTrackedEdges != 0 ) )
    {
        frontier.push_back( pAnd );
    }

    return ( !bIsLinkDimAndAndStep ) && ( iBackTrackedEdges != 0 );
}

template < typename TPolicy >
static void backtrackToLinkDimensions( typename TPolicy::RootPtr pRoot, const TPolicy& policy,
                                       typename TPolicy::AndPtrVector& frontier )
{
    auto originalFrontier = frontier;
    frontier.clear();

    for( auto pEdge : pRoot->get_edges() )
    {
        if( !pEdge->get_eliminated() )
        {
            backtrackToLinkDimensionsRecurse( pEdge, policy, frontier );
        }
    }
}*/

} // namespace DerivationSolver
