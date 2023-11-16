
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

struct InvocationPolicyBase
{
    using GraphVertex             = Concrete::Graph::Vertex;
    using GraphEdge               = Concrete::Graph::Edge;
    using GraphVertexVector       = std::vector< GraphVertex* >;
    using GraphVertexSet          = std::unordered_set< GraphVertex* >;
    using GraphVertexVectorVector = std::vector< GraphVertexVector >;
    using GraphEdgeVector         = std::vector< GraphEdge* >;

    struct Spec
    {
        GraphVertexVector       context;
        GraphVertexVectorVector path;

        Spec() = default;

        Spec( GraphVertex* pContext, Interface::SymbolVariantSequence* pSymbolVariantSequence,
              bool bIncludeLinks = false )
            : context{ pContext }
        {
            for( auto pSymbolVariant : pSymbolVariantSequence->get_variants() )
            {
                InvocationPolicyBase::GraphVertexVector pathElement;
                for( auto pSymbol : pSymbolVariant->get_symbols() )
                {
                    for( auto pContext : pSymbol->get_contexts() )
                    {
                        for( auto pConcrete : pContext->get_concrete() )
                        {
                            pathElement.push_back( pConcrete );
                        }
                    }
                    if( bIncludeLinks )
                    {
                        for( auto pLink : pSymbol->get_links() )
                        {
                            for( auto pConcrete : pLink->get_concrete() )
                            {
                                pathElement.push_back( pConcrete );
                            }
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

    using NodePtr      = Derivation::Node*;
    using StepPtr      = Derivation::Step*;
    using EdgePtr      = Derivation::Edge*;
    using OrPtr        = Derivation::Or*;
    using OrPtrVector  = std::vector< OrPtr >;
    using AndPtr       = Derivation::And*;
    using AndPtrVector = std::vector< AndPtr >;
    using RootPtr      = Derivation::Root*;

    EdgePtr makeEdge( NodePtr pFrom, StepPtr pNext, const GraphEdgeVector& edges ) const
    {
        // initially no edges are eliminated
        return m_database.construct< Derivation::Edge >(
            Derivation::Edge::Args{ pFrom, pNext, false, false, 0, edges } );
    }
    OrPtr makeOr( GraphVertex* pVertex ) const
    {
        return m_database.construct< Derivation::Or >(
            Derivation::Or::Args{ Derivation::Step::Args{ Derivation::Node::Args{ {} }, pVertex } } );
    }
    RootPtr makeRoot( const GraphVertexVector& context ) const
    {
        return m_database.construct< Derivation::Root >( Derivation::Root::Args{ Derivation::Node::Args{ {} }, context } );
    }
    EdgePtr makeRootEdge( RootPtr pFrom, OrPtr pNext ) const
    {
        return m_database.construct< Derivation::Edge >( Derivation::Edge::Args{ pFrom, pNext, false, false, 0, {} } );
    }

    InvocationPolicyBase( Database& database )
        : m_database( database )
    {
    }

protected:
    Database& m_database;
};