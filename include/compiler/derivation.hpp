
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

struct DerivationPolicyBase
{
    using GraphVertexVector       = std::vector< Concrete::Node* >;
    using GraphVertexSet          = std::unordered_set< Concrete::Node* >;
    using GraphVertexVectorVector = std::vector< GraphVertexVector >;

    struct Spec
    {
        GraphVertexVector       context;
        GraphVertexVectorVector path;

        Spec() = default;

        Spec( GraphVertexVector context, Parser::Type::Deriving* pDerivation, bool bIncludeLinks = false )
            : context( std::move( context ) )
        {
            for( auto pSymbolVariant : pDerivation->get_variants() )
            {
                DerivationPolicyBase::GraphVertexVector pathElement;
                for( auto pSymbol : pSymbolVariant->get_symbols() )
                {
                    auto pSymbolID = pSymbol->get_id();

                    for( auto pInterfaceID : pSymbolID->get_interfaceIDs() )
                    {
                        Interface::Node* pNode = pInterfaceID->get_node();
                        if( !bIncludeLinks )
                        {
                            if( db_cast< Interface::UserLink >( pNode ) )
                            {
                                continue;
                            }
                        }
                        for( auto pConcrete : pNode->get_inheritors() )
                        {
                            pathElement.push_back( pConcrete );
                        }
                    }
                }
                pathElement = mega::make_unique_without_reorder( pathElement );
                VERIFY_RTE_MSG( !pathElement.empty(), "Event contains invalid symbols" );
                path.push_back( pathElement );
            }
            path = mega::make_unique_without_reorder( path );
        }
    };

    Derivation::Edge* makeEdge( Derivation::Node* pFrom, Derivation::Step* pNext,
                                const std::vector< Concrete::Edge* >& edges ) const
    {
        // initially no edges are eliminated
        return m_database.construct< Derivation::Edge >(
            Derivation::Edge::Args{ pFrom, pNext, false, false, 0, edges } );
    }
    Derivation::Or* makeOr( Concrete::Node* pVertex ) const
    {
        return m_database.construct< Derivation::Or >(
            Derivation::Or::Args{ Derivation::Step::Args{ Derivation::Node::Args{ {} }, pVertex } } );
    }
    Derivation::Root* makeRoot( const GraphVertexVector& context ) const
    {
        return m_database.construct< Derivation::Root >(
            Derivation::Root::Args{ Derivation::Node::Args{ {} }, context } );
    }
    Derivation::Edge* makeRootEdge( Derivation::Root* pFrom, Derivation::Or* pNext ) const
    {
        return m_database.construct< Derivation::Edge >( Derivation::Edge::Args{ pFrom, pNext, false, false, 0, {} } );
    }

    DerivationPolicyBase( Database& database )
        : m_database( database )
    {
    }

protected:
    Database& m_database;
};

struct NonInterObjectDerivationPolicy : public DerivationPolicyBase
{
    std::vector< Derivation::Or* > expandLink( Derivation::Or* pOr ) const
    {
        std::vector< Derivation::Or* > result;
        result.push_back( pOr );
        return result;
    }

    Concrete::Node* commonRootDerivation( Concrete::Node* pSource, Concrete::Node* pTarget,
                                          std::vector< Concrete::Edge* >& edges ) const
    {
        return CommonAncestor::commonRootDerivation( pSource, pTarget, edges, true );
    }

    NonInterObjectDerivationPolicy( Database& database )
        : DerivationPolicyBase( database )
    {
    }
};

struct InterObjectDerivationPolicy : public DerivationPolicyBase
{
    std::vector< Derivation::Or* > expandLink( Derivation::Or* pOr ) const
    {
        std::vector< Derivation::Or* > result;

        std::vector< Concrete::Edge* > edges;
        {
            for( auto pEdge : pOr->get_vertex()->get_out_edges() )
            {
                switch( pEdge->get_type().get() )
                {
                    case mega::EdgeType::eMonoSingularMandatory:
                    case mega::EdgeType::ePolySingularMandatory:
                    case mega::EdgeType::eMonoNonSingularMandatory:
                    case mega::EdgeType::ePolyNonSingularMandatory:
                    case mega::EdgeType::eMonoSingularOptional:
                    case mega::EdgeType::ePolySingularOptional:
                    case mega::EdgeType::eMonoNonSingularOptional:
                    case mega::EdgeType::ePolyNonSingularOptional:
                    case mega::EdgeType::ePolyParent:
                    {
                        edges.push_back( pEdge );
                    }
                    break;
                    case mega::EdgeType::eParent:
                    case mega::EdgeType::eChildSingular:
                    case mega::EdgeType::eChildNonSingular:
                    case mega::EdgeType::eDim:
                    case mega::EdgeType::eLink:
                    case mega::EdgeType::TOTAL_EDGE_TYPES:
                        break;
                }
            }
        }

        if( !edges.empty() )
        {
            auto pAnd = m_database.construct< Derivation::And >(
                Derivation::And::Args{ Derivation::Step::Args{ Derivation::Node::Args{ {} }, pOr->get_vertex() } } );

            auto pOrToAndEdge
                = m_database.construct< Derivation::Edge >( Derivation::Edge::Args{ pOr, pAnd, false, false, 0, {} } );

            pOr->push_back_edges( pOrToAndEdge );

            for( auto pGraphEdge : edges )
            {
                // determine the parent context of the link target
                Concrete::Node* pParentVertex = nullptr;
                Concrete::Edge* pParentEdge   = nullptr;
                {
                    auto pLinkTarget = pGraphEdge->get_target();
                    for( auto pLinkGraphEdge : pLinkTarget->get_out_edges() )
                    {
                        if( pLinkGraphEdge->get_type().get() == mega::EdgeType::eParent )
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

    Concrete::Node* commonRootDerivation( Concrete::Node* pSource, Concrete::Node* pTarget,
                                          std::vector< Concrete::Edge* >& edges ) const
    {
        return CommonAncestor::commonRootDerivation( pSource, pTarget, edges );
    }

    InterObjectDerivationPolicy( Database& database )
        : DerivationPolicyBase( database )
    {
    }
};

template < typename TPolicy >
static std::vector< Derivation::Or* > solveStep( Derivation::Or*                       pCurrentFrontierStep,
                                                 const std::vector< Concrete::Node* >& typePathElement,
                                                 const TPolicy&                        policy )
{
    std::vector< Derivation::Or* > nextFrontier;

    auto pCurrentVertex = pCurrentFrontierStep->get_vertex();

    // attempt in-object common root derivation
    for( auto pTypePathVertex : typePathElement )
    {
        std::vector< Concrete::Edge* > edges;
        if( pCurrentVertex == pTypePathVertex )
        {
            nextFrontier.push_back( pCurrentFrontierStep );
        }
        else if( policy.commonRootDerivation( pCurrentVertex, pTypePathVertex, edges ) )
        {
            Derivation::Or*   pNextStep = policy.makeOr( pTypePathVertex );
            Derivation::Edge* pEdge     = policy.makeEdge( pCurrentFrontierStep, pNextStep, edges );
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
                std::vector< Concrete::Edge* > edges;
                // if( policy.commonRootDerivation( pCurrentVertex, pLinkContextVertex, edges ) )
                // {
                //     pStep = policy.makeOr( pLinkContextVertex );
                //     pCurrentFrontierStep->push_back_edges( policy.makeEdge( pCurrentFrontierStep, pStep, edges ) );
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
                            policy.makeEdge( pBranch, std::vector< Concrete::Edge* >{ pContextToLinkVertex } ) );

                        for( auto pLinkEdge : linkEdges )
                        {
                            Derivation::Or* pLinkedObjectToTarget = policy.makeOr( pLinkEdge->get_target() );
                            pBranch->push_back_edges( policy.makeEdge( pCurrentFrontierStep,
                                pLinkedObjectToTarget, std::vector< Concrete::Edge* >{ pLinkEdge } ) );

                            std::vector< Derivation::Or* > recursiveResult
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
static Derivation::Root* solveContextFree( const typename TPolicy::Spec& spec, const TPolicy& policy,
                                           std::vector< Derivation::Or* >& frontier )
{
    Derivation::Root* pSolutionRoot = policy.makeRoot( spec.context );

    for( auto pVert : spec.context )
    {
        Derivation::Or* p = policy.makeOr( pVert );
        frontier.push_back( p );
        pSolutionRoot->push_back_edges( policy.makeRootEdge( pSolutionRoot, p ) );
    }

    for( auto i = spec.path.begin(), iNext = spec.path.begin(), iEnd = spec.path.end(); i != iEnd; ++i )
    {
        ++iNext;

        const bool  bLast           = iNext == iEnd;
        const auto& typePathElement = *i;

        std::vector< Derivation::Or* > nextFrontier;
        for( Derivation::Or* pCurrentFrontierStep : frontier )
        {
            std::vector< Derivation::Or* > recursiveResult = solveStep( pCurrentFrontierStep, typePathElement, policy );
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
static bool backtrackToLinkDimensionsRecurse( Concrete::Edge* pEdge, const TPolicy& policy,
                                              typename TPolicy::AndPtrVector& frontier )
{
    auto pStep = pEdge->get_next();

    auto       pAnd                 = policy.isAndStep( pStep );
    const bool bIsLinkDimAndAndStep = pAnd && policy.isLinkDimension( pStep->get_vertex() );

    std::vector< Concrete::Edge* > nonElimatedEdges;
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
static void backtrackToLinkDimensions( Derivation::Root* pRoot, const TPolicy& policy,
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
