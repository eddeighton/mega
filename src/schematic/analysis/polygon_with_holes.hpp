
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

#ifndef GUARD_2023_June_14_polygon_with_holes
#define GUARD_2023_June_14_polygon_with_holes

#include "algorithms.hpp"

namespace exact
{

class PolygonNode : std::enable_shared_from_this< PolygonNode >
{
public:
    using Ptr       = std::shared_ptr< PolygonNode >;
    using PtrVector = std::vector< Ptr >;

    static inline bool includes( const PolygonNode& includer, const PolygonNode& included )
    {
        return std::includes(
            includer.m_faces.begin(), includer.m_faces.end(), included.m_faces.begin(), included.m_faces.end() );
    }

    static inline bool intersects( const PolygonNode& left, const PolygonNode& right )
    {
        Analysis::FaceCstVector intersection;
        std::set_intersection( left.m_faces.begin(), left.m_faces.end(), right.m_faces.begin(), right.m_faces.end(),
                               std::back_inserter( intersection ) );
        return !intersection.empty();
    }

    static inline bool equal( const PolygonNode& left, const PolygonNode& right )
    {
        return left.m_faces == right.m_faces;
    }

    enum Type
    {
        eOuter,
        eInner,
        TOTAL_POLYNODE_TYPES
    };
    static inline Type opposite( Type type )
    {
        switch( type )
        {
            case eOuter:
                return eInner;
            case eInner:
                return eOuter;
            case TOTAL_POLYNODE_TYPES:
                return TOTAL_POLYNODE_TYPES;
            default:
                INVARIANT( false, "Unexpected polynode type" );
                break;
        }
    }

    PolygonNode( const Analysis::HalfEdgeCstVector& polygon )
        : m_type( TOTAL_POLYNODE_TYPES )
        , m_polygon( polygon )
    {
        // NOTE: m_faces is sorted based on ptr
        m_bPolygonIsFace = getSortedFacesInsidePolygon( m_polygon, m_faces );
        INVARIANT( !m_faces.empty(), "No faces for polygon" );
    }
    PolygonNode( Type type, const Analysis::HalfEdgeCstVector& polygon )
        : m_type( type )
        , m_polygon( polygon )
    {
        // NOTE: m_faces is sorted based on ptr
        m_bPolygonIsFace = getSortedFacesInsidePolygon( m_polygon, m_faces );
        INVARIANT( !m_faces.empty(), "No faces for polygon" );
    }

    int  size() const { return m_faces.size(); }
    bool outer() const { return ( m_type == eOuter ) || ( m_type == TOTAL_POLYNODE_TYPES ); }
    bool inner() const { return ( m_type == eInner ) || ( m_type == TOTAL_POLYNODE_TYPES ); }
    bool face() const { return m_bPolygonIsFace; }
    const Analysis::HalfEdgeCstVector& polygon() const { return m_polygon; }
    const Analysis::FaceCstVector&        faces() const { return m_faces; }
    const PtrVector&                   contained() const { return m_contained; }
    int                                validate( Type type = eOuter ) const
    {
        int count = 1;
        INVARIANT( ( m_type == TOTAL_POLYNODE_TYPES ) || ( type == m_type ), "Wrong polynode type" );
        for( auto c : m_contained )
        {
            INVARIANT( includes( *this, *c ), "Invalid child poly node" );
            count += c->validate( opposite( type ) );
        }
        return count;
    }

    // NOTE: insert ONLY functions correctly for use with sorted range
    bool insert( PolygonNode::Ptr pNode )
    {
        if( equal( *this, *pNode ) )
        {
            INVARIANT( pNode->outer(), "Attempted to insert inner into equal" );
            INVARIANT( inner(), "Attempted to insert equal into outer" );
            m_contained.push_back( pNode );
            return true;
        }
        else if( includes( *this, *pNode ) )
        {
            bool bContained = false;
            for( auto pChild : m_contained )
            {
                if( pChild->insert( pNode ) )
                {
                    bContained = true;
                    break;
                }
            }
            if( !bContained )
            {
                m_contained.push_back( pNode );
            }
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    const Type                  m_type;
    bool                        m_bPolygonIsFace;
    Analysis::HalfEdgeCstVector m_polygon;
    Analysis::FaceCstVector        m_faces;
    PtrVector                   m_contained;
};

// NOTE: outer and inner DOES NOT imply whether polygon is hole or face
inline std::vector< PolygonNode::Ptr > getPolygonNodes( std::vector< PolygonNode::Ptr >& nodes )
{
    // construct a node for all polygons collecting their faces
    using NodePtr    = PolygonNode::Ptr;
    using NodeVector = std::vector< NodePtr >;

    // sort in order of greatest to smallest
    std::sort(
        //
        nodes.begin(), nodes.end(),
        []( NodePtr pLeft, NodePtr pRight )
        {
            if( pLeft->size() == pRight->size() )
            {
                // if equal then hole is greater than face
                if( PolygonNode::equal( *pLeft, *pRight ) )
                {
                    // NOTE: ONLY inner can contain outer - so order Inner > Outer
                    INVARIANT( pLeft->inner() != pRight->inner(), "Equal polygons are same type" );
                    return pLeft->inner();
                }
                // ensure equal face sets are adjacent
                else
                {
                    return pLeft->faces() > pRight->faces();
                }
            }
            // otherwise if intersect then one MUST contain the other
            else if( PolygonNode::intersects( *pLeft, *pRight ) )
            {
                if( PolygonNode::includes( *pLeft, *pRight ) )
                {
                    return true;
                }
                else if( PolygonNode::includes( *pRight, *pLeft ) )
                {
                    return false;
                }
                else
                {
                    INVARIANT( false, "Polygons overlap but neither contains the other" );
                }
            }
            // if disjoint then order by size
            else
            {
                return pLeft->size() > pRight->size();
            }
        } );

    NodeVector rootNodes;

    for( NodePtr pNode : nodes )
    {
        bool bFoundParent = false;
        for( auto pRoot : rootNodes )
        {
            if( pRoot->insert( pNode ) )
            {
                bFoundParent = true;
                break;
            }
        }
        if( !bFoundParent )
            rootNodes.push_back( pNode );
    }

    int iCount = 0;
    for( NodePtr r : rootNodes )
    {
        INVARIANT( r->outer(), "Root node is not outer node" );
        iCount += r->validate();
    }
    INVARIANT( iCount == nodes.size(), "Poly tree error" );

    return rootNodes;
}

// NOTE: outer and inner DOES NOT imply whether polygon is hole or face
template < typename OuterEdgePredicate, typename InnerEdgePredicate >
inline std::vector< PolygonNode::Ptr > getPolygonNodes( Analysis::Arrangement& arr,
                                                        OuterEdgePredicate&&   outerEdgePredicate,
                                                        InnerEdgePredicate&&   innerEdgePredicate )
{
    // construct a node for all polygons collecting their faces
    using NodePtr    = typename PolygonNode::Ptr;
    using NodeVector = std::vector< NodePtr >;
    NodeVector nodes;
    {
        Analysis::HalfEdgeCstVectorVector outerPolygons;
        {
            Analysis::HalfEdgeCstSet outerEdges;
            getEdges( arr, outerEdges, outerEdgePredicate );
            getPolygons( outerEdges, outerPolygons );
        }

        Analysis::HalfEdgeCstVectorVector innerPolygons;
        {
            Analysis::HalfEdgeCstSet innerEdges;
            getEdges( arr, innerEdges, innerEdgePredicate );
            getPolygons( innerEdges, innerPolygons );
        }

        for( auto& poly : outerPolygons )
        {
            nodes.push_back( std::make_shared< PolygonNode >( PolygonNode::eOuter, poly ) );
        }
        for( auto& poly : innerPolygons )
        {
            nodes.push_back( std::make_shared< PolygonNode >( PolygonNode::eInner, poly ) );
        }
    }

    return getPolygonNodes( nodes );
}

template < typename OuterEdgePredicate, typename InnerEdgePredicate >
inline void getPolyonsWithHoles( Analysis::Arrangement& arr, OuterEdgePredicate&& outerEdgePredicate,
                                 InnerEdgePredicate&&                        innerEdgePredicate,
                                 Analysis::HalfEdgeCstPolygonWithHoles::Vector& polygonsWithHoles )
{
    struct Visitor
    {
        Analysis::HalfEdgeCstPolygonWithHoles::Vector& polygonsWithHoles;

        Visitor( Analysis::HalfEdgeCstPolygonWithHoles::Vector& polygonsWithHoles )
            : polygonsWithHoles( polygonsWithHoles )
        {
        }

        void outer( PolygonNode& node ) const
        {
            INVARIANT( node.outer(), "Found hole when expecting contour" );
            Analysis::HalfEdgeCstPolygonWithHoles p;
            p.outer = node.polygon();
            for( auto pHole : node.contained() )
            {
                inner( *pHole, p );
            }
            polygonsWithHoles.push_back( p );
        }

        void inner( PolygonNode& node, Analysis::HalfEdgeCstPolygonWithHoles& p ) const
        {
            INVARIANT( node.inner(), "Found contour when expecting hole" );
            p.holes.push_back( node.polygon() );
            for( auto pContour : node.contained() )
            {
                outer( *pContour );
            }
        }
    };

    auto rootNodes = getPolygonNodes( arr, outerEdgePredicate, innerEdgePredicate );

    Visitor visitor( polygonsWithHoles );
    for( auto pNode : rootNodes )
    {
        visitor.outer( *pNode );
    }
}

} // namespace exact

#endif // GUARD_2023_June_14_polygon_with_holes
