
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

namespace Derivation
{

using Vertex              = Concrete::Graph::Vertex;
using VertexVariant       = std::vector< Vertex* >;
using VertexSet           = std::unordered_set< Vertex* >;
using VertexVariantVector = std::vector< VertexVariant >;
using EdgeVector          = std::vector< Concrete::Graph::Edge* >;

struct Spec
{
    VertexVariant       context;
    VertexVariantVector path;
};

struct Step;

struct Edge
{
    using Ptr = std::shared_ptr< Edge >;

    std::shared_ptr< Step >               pNext;
    std::vector< Concrete::Graph::Edge* > edges;

    Edge( std::shared_ptr< Step > pNextStep )
        : pNext( pNextStep )
    {
    }
    Edge( std::shared_ptr< Step > pNextStep, std::vector< Concrete::Graph::Edge* > edges )
        : pNext( pNextStep )
        , edges( std::move( edges ) )
    {
    }
};

struct Step
{
    using Ptr       = std::shared_ptr< Step >;
    using PtrVector = std::vector< Ptr >;

    Concrete::Graph::Vertex* pVertex = nullptr;
    std::vector< Edge::Ptr > edges;

    Step() = default;
    Step( Concrete::Graph::Vertex* pVertex )
        : pVertex( pVertex )
    {
    }
    virtual ~Step() = default;
};

struct And : Step
{
    using Ptr       = std::shared_ptr< And >;
    using PtrVector = std::vector< Ptr >;
    And( Concrete::Graph::Vertex* pVertex = nullptr )
        : Step( pVertex )
    {
    }
};

struct Or : Step
{
    using Ptr       = std::shared_ptr< Or >;
    using PtrVector = std::vector< Ptr >;
    Or( Concrete::Graph::Vertex* pVertex )
        : Step( pVertex )
    {
    }
};

struct Solution
{
    Spec     spec;
    And::Ptr pStart;
};

enum Disambiguation
{
    eSuccess,
    eFailure,
    eAmbiguous
};

Disambiguation disambiguate( Step::Ptr pStep, const Or::PtrVector& finalFrontier )
{
    std::optional< Disambiguation > result;

    if( And::Ptr pAnd = std::dynamic_pointer_cast< And >( pStep ) )
    {
        for( auto pEdge : pStep->edges )
        {
            switch( disambiguate( pEdge->pNext, finalFrontier ) )
            {
                case eSuccess:
                {
                    if( result.has_value() )
                    {
                        switch( result.value() )
                        {
                            case eSuccess:
                                result = eSuccess;
                                break;
                            case eFailure:
                                result = eFailure;
                                break;
                            case eAmbiguous:
                                result = eAmbiguous;
                                break;
                        }
                    }
                    else
                    {
                        result = eSuccess;
                    }
                }
                break;
                case eFailure:
                {
                    result = eFailure;
                }
                break;
                case eAmbiguous:
                {
                    if( result.has_value() )
                    {
                        switch( result.value() )
                        {
                            case eSuccess:
                                result = eAmbiguous;
                                break;
                            case eFailure:
                                result = eFailure;
                                break;
                            case eAmbiguous:
                                result = eAmbiguous;
                                break;
                        }
                    }
                    else
                    {
                        result = eAmbiguous;
                    }
                }
                break;
            }
        }
    }
    else if( Or::Ptr pOr = std::dynamic_pointer_cast< Or >( pStep ) )
    {
        if( std::find( finalFrontier.begin(), finalFrontier.end(), pOr ) != finalFrontier.end() )
        {
            ASSERT( pStep->edges.empty() );
            return eSuccess;
        }

        for( auto pEdge : pStep->edges )
        {
            switch( disambiguate( pEdge->pNext, finalFrontier ) )
            {
                case eSuccess:
                {
                    if( result.has_value() )
                    {
                        switch( result.value() )
                        {
                            case eSuccess:
                                result = eAmbiguous;
                                break;
                            case eFailure:
                                result = eSuccess;
                                break;
                            case eAmbiguous:
                                result = eAmbiguous;
                                break;
                        }
                    }
                    else
                    {
                        result = eSuccess;
                    }
                }
                break;
                case eFailure:
                {
                    if( result.has_value() )
                    {
                        switch( result.value() )
                        {
                            case eSuccess:
                                result = eSuccess;
                                break;
                            case eFailure:
                                result = eFailure;
                                break;
                            case eAmbiguous:
                                result = eAmbiguous;
                                break;
                        }
                    }
                    else
                    {
                        result = eFailure;
                    }
                }
                break;
                case eAmbiguous:
                {
                    result = eAmbiguous;
                }
                break;
            }
        }
    }
    else
    {
        THROW_RTE( "Unknown step type" );
    }

    if( !result.has_value() )
    {
        return eFailure;
    }
    else
    {
        return result.value();
    }
}

// struct DerivationPolicy
// {
//     // enumerate actual polymorphic branches of a given link
//     std::vector< Concrete::Graph::Edge* > enumerateLink( Concrete::Graph::Vertex* pLink ) const;
//
//     // enumerate the dimension links within the parent vertex
//     using LinkDimension = std::pair< Concrete::Graph::Vertex*, Concrete::Graph::Edge* >;
//     std::vector< LinkDimension > enumerateLinks( Concrete::Graph::Vertex* pParentVertex ) const;
//
//     std::vector< Concrete::Graph::Vertex* > enumerateLinkContexts( Concrete::Graph::Vertex* pVertex,
//                                                                    bool                     bDerivingOnly ) const;
//
//     bool commonRootDerivation( Concrete::Graph::Vertex* pSource, Concrete::Graph::Vertex* pTarget,
//                                std::vector< Concrete::Graph::Edge* >& edges ) const;
// };

template < typename TDerivationPolicy >
Or::PtrVector solveStep( Or::Ptr pCurrentFrontierStep, const VertexVariant& typePathElement, bool bUseDerivingLinksOnly,
                         VertexSet& visited, const TDerivationPolicy& derivation )
{
    Or::PtrVector nextFrontier;

    auto pCurrentVertex = pCurrentFrontierStep->pVertex;
    if( visited.contains( pCurrentVertex ) )
    {
        return nextFrontier;
    }
    visited.insert( pCurrentVertex );

    // attempt in-object common root derivation
    for( auto pTypePathVertex : typePathElement )
    {
        std::vector< Concrete::Graph::Edge* > edges;
        if( derivation.commonRootDerivation( pCurrentVertex, pTypePathVertex, edges ) )
        {
            Or::Ptr   pNextStep = std::make_shared< Or >( pTypePathVertex );
            Edge::Ptr pEdge     = std::make_shared< Edge >( pNextStep, edges );

            pCurrentFrontierStep->edges.push_back( pEdge );
            nextFrontier.push_back( pNextStep );
        }
    }

    for( auto pLinkContextVertex : derivation.enumerateLinkContexts( pCurrentVertex, bUseDerivingLinksOnly ) )
    {
        // is there a common root derivation to this link vertex?
        std::vector< Concrete::Graph::Edge* > edges;
        if( derivation.commonRootDerivation( pCurrentVertex, pLinkContextVertex, edges ) )
        {
            Or::Ptr pInObjectToLink = std::make_shared< Or >( pLinkContextVertex );
            pCurrentFrontierStep->edges.push_back( std::make_shared< Edge >( pInObjectToLink, edges ) );

            // if so generate the AND step
            for( auto [ pLinkVertex, pEdge ] : derivation.enumerateLinks( pLinkContextVertex ) )
            {
                And::Ptr pBranch = std::make_shared< And >( pLinkVertex );
                pInObjectToLink->edges.push_back( std::make_shared< Edge >( pBranch, EdgeVector{ pEdge } ) );

                for( auto pLinkEdge : derivation.enumerateLink( pLinkVertex ) )
                {
                    Or::Ptr pLinkedObjectToTarget = std::make_shared< Or >( pLinkEdge->get_target() );
                    pBranch->edges.push_back(
                        std::make_shared< Edge >( pLinkedObjectToTarget, EdgeVector{ pLinkEdge } ) );

                    Or::PtrVector recursiveResult
                        = solveStep( pLinkedObjectToTarget, typePathElement, true, visited, derivation );
                    std::copy( recursiveResult.begin(), recursiveResult.end(), std::back_inserter( nextFrontier ) );
                }
            }
        }
    }

    return nextFrontier;
}

template < typename TDerivationPolicy >
std::pair< Disambiguation, And::Ptr > solve( const Spec& spec, const TDerivationPolicy& derivation )
{
    And::Ptr pSolutionRoot = std::make_shared< And >();

    Or::PtrVector frontier;

    for( auto pVert : spec.context )
    {
        Or::Ptr p = std::make_shared< Or >( pVert );
        frontier.push_back( p );
        pSolutionRoot->edges.push_back( std::make_shared< Edge >( p ) );
    }

    for( auto typePathElement : spec.path )
    {
        Or::PtrVector nextFrontier;
        for( Or::Ptr pCurrentFrontierStep : frontier )
        {
            VertexSet     visited;
            Or::PtrVector recursiveResult
                = solveStep( pCurrentFrontierStep, typePathElement, false, visited, derivation );
            std::copy( recursiveResult.begin(), recursiveResult.end(), std::back_inserter( nextFrontier ) );
        }
        nextFrontier.swap( frontier );
    }

    if( !frontier.empty() )
    {
        const auto disambiguationResult = disambiguate( pSolutionRoot, frontier );
        if( disambiguationResult == eSuccess )
        {
            return { eSuccess, pSolutionRoot };
        }
        else
        {
            return { disambiguationResult, {} };
        }
    }

    return { eFailure, {} };
}

} // namespace Derivation

#endif // GUARD_2023_September_21_derivation
