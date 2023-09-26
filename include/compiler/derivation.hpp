
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

#ifndef GUARD_2023_September_21_derivation
#define GUARD_2023_September_21_derivation

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
    GraphVertexVector   enumerateLinkContexts( GraphVertex* pVertex, bool bDerivingOnly ) const;
    bool commonRootDerivation( GraphVertex* pSource, GraphVertex* pTarget, GraphEdgeVector& edges ) const;
};*/

template < typename TPolicy >
typename TPolicy::OrPtrVector
solveStep( typename TPolicy::OrPtr pCurrentFrontierStep, const typename TPolicy::GraphVertexVector& typePathElement,
           bool bUseDerivingLinksOnly, typename TPolicy::GraphVertexSet& visited, const TPolicy& policy )
{
    typename TPolicy::OrPtrVector nextFrontier;

    auto pCurrentVertex = pCurrentFrontierStep->get_vertex();
    if( visited.contains( pCurrentVertex ) )
    {
        return nextFrontier;
    }
    visited.insert( pCurrentVertex );

    // attempt in-object common root derivation
    for( auto pTypePathVertex : typePathElement )
    {
        typename TPolicy::GraphEdgeVector edges;
        if( policy.commonRootDerivation( pCurrentVertex, pTypePathVertex, edges ) )
        {
            typename TPolicy::OrPtr   pNextStep = policy.makeOr( pTypePathVertex );
            typename TPolicy::EdgePtr pEdge     = policy.makeEdge( pNextStep, edges );
            pCurrentFrontierStep->push_back_edges( pEdge );
            nextFrontier.push_back( pNextStep );
        }
    }

    for( auto pLinkContextVertex : policy.enumerateLinkContexts( pCurrentVertex, bUseDerivingLinksOnly ) )
    {
        // is there a common root derivation to this link vertex?
        typename TPolicy::GraphEdgeVector edges;
        if( policy.commonRootDerivation( pCurrentVertex, pLinkContextVertex, edges ) )
        {
            typename TPolicy::OrPtr pInObjectToLink = policy.makeOr( pLinkContextVertex );
            pCurrentFrontierStep->push_back_edges( policy.makeEdge( pInObjectToLink, edges ) );

            // if so generate the AND step
            for( auto pContextToLinkVertex : policy.enumerateLinks( pLinkContextVertex ) )
            {
                auto pLinkTarget = pContextToLinkVertex->get_target();
                typename TPolicy::AndPtr pBranch = policy.makeAnd( pLinkTarget );
                pInObjectToLink->push_back_edges(
                    policy.makeEdge( pBranch, typename TPolicy::GraphEdgeVector{ pContextToLinkVertex } ) );

                for( auto pLinkEdge : policy.enumerateLink( pLinkTarget ) )
                {
                    typename TPolicy::OrPtr pLinkedObjectToTarget = policy.makeOr( pLinkEdge->get_target() );
                    pBranch->push_back_edges(
                        policy.makeEdge( pLinkedObjectToTarget, typename TPolicy::GraphEdgeVector{ pLinkEdge } ) );

                    typename TPolicy::OrPtrVector recursiveResult
                        = solveStep( pLinkedObjectToTarget, typePathElement, true, visited, policy );
                    std::copy( recursiveResult.begin(), recursiveResult.end(), std::back_inserter( nextFrontier ) );
                }
            }
        }
    }

    return nextFrontier;
}

template < typename TPolicy >
typename TPolicy::RootPtr solveContextFree( 
        const typename TPolicy::Spec& spec, const TPolicy& policy, typename TPolicy::OrPtrVector& frontier )
{
    typename TPolicy::RootPtr pSolutionRoot = policy.makeRoot( spec.context );

    for( auto pVert : spec.context )
    {
        typename TPolicy::OrPtr p = policy.makeOr( pVert );
        frontier.push_back( p );
        pSolutionRoot->push_back_edges( policy.makeRootEdge( p ) );
    }

    for( auto typePathElement : spec.path )
    {
        typename TPolicy::OrPtrVector nextFrontier;
        for( typename TPolicy::OrPtr pCurrentFrontierStep : frontier )
        {
            typename TPolicy::GraphVertexSet visited;
            typename TPolicy::OrPtrVector    recursiveResult
                = solveStep( pCurrentFrontierStep, typePathElement, false, visited, policy );
            std::copy( recursiveResult.begin(), recursiveResult.end(), std::back_inserter( nextFrontier ) );
        }
        nextFrontier.swap( frontier );
    }

    return pSolutionRoot;
}



} // namespace DerivationSolver

#endif // GUARD_2023_September_21_derivation
