
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

template < typename HalfEdgeType, typename FaceType, typename NodeType >
class PolygonNodeT : std::enable_shared_from_this< PolygonNodeT< HalfEdgeType, FaceType, NodeType > >
{
public:
    using HalfEdge       = HalfEdgeType;
    using HalfEdgeVector = std::vector< HalfEdge >;
    using Face           = FaceType;
    using FaceVector     = std::vector< Face >;
    using Ptr            = std::shared_ptr< PolygonNodeT< HalfEdgeType, FaceType, NodeType > >;
    using PtrVector      = std::vector< Ptr >;

    static inline bool includes( const PolygonNodeT& includer, const PolygonNodeT& included )
    {
        return std::includes(
            includer.m_faces.begin(), includer.m_faces.end(), included.m_faces.begin(), included.m_faces.end() );
    }

    static inline bool intersects( const PolygonNodeT& left, const PolygonNodeT& right )
    {
        FaceVector intersection;
        std::set_intersection( left.m_faces.begin(), left.m_faces.end(), right.m_faces.begin(), right.m_faces.end(),
                               std::back_inserter( intersection ) );
        return !intersection.empty();
    }

    static inline bool equal( const PolygonNodeT& left, const PolygonNodeT& right )
    {
        return left.m_faces == right.m_faces;
    }

    using Type = NodeType;

    PolygonNodeT( const HalfEdgeVector& polygon )
        : m_type( Type{} )
        , m_polygon( polygon )
    {
        // NOTE: m_faces is sorted based on ptr
        m_bPolygonIsFace = getSortedFacesInsidePolygon( m_polygon, m_faces );
        INVARIANT( !m_faces.empty(), "No faces for polygon" );
    }
    PolygonNodeT( Type type, const HalfEdgeVector& polygon )
        : m_type( type )
        , m_polygon( polygon )
    {
        // NOTE: m_faces is sorted based on ptr
        m_bPolygonIsFace = getSortedFacesInsidePolygon( m_polygon, m_faces );
        INVARIANT( !m_faces.empty(), "No faces for polygon" );
    }

    int                   size() const { return m_faces.size(); }
    NodeType              type() const { return m_type; }
    bool                  face() const { return m_bPolygonIsFace; }
    const HalfEdgeVector& polygon() const { return m_polygon; }
    const FaceVector&     faces() const { return m_faces; }
    const PtrVector&      contained() const { return m_contained; }
    int                   validate() const
    {
        int count = 1;
        for( auto c : m_contained )
        {
            INVARIANT( includes( *this, *c ), "Invalid child poly node" );
            count += c->validate();
        }
        return count;
    }

    // NOTE: insert ONLY functions correctly for use with sorted range
    bool insert( Ptr pNode )
    {
        if( equal( *this, *pNode ) )
        {
            INVARIANT( type() != pNode->type(), "Attempted to insert same type into equal" );
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
    const Type     m_type;
    bool           m_bPolygonIsFace;
    HalfEdgeVector m_polygon;
    FaceVector     m_faces;
    PtrVector      m_contained;
};

using PolygonNode    = PolygonNodeT< Analysis::HalfEdge, Analysis::Face, char >;
using PolygonNodeCst = PolygonNodeT< Analysis::HalfEdgeCst, Analysis::FaceCst, char >;

// NOTE: outer and inner DOES NOT imply whether polygon is hole or face
template < typename HalfEdgeType, typename FaceType, typename NodeType >
inline std::vector< typename PolygonNodeT< HalfEdgeType, FaceType, NodeType >::Ptr >
sortPolygonNodes( std::vector< typename PolygonNodeT< HalfEdgeType, FaceType, NodeType >::Ptr >& nodes )
{
    // construct a node for all polygons collecting their faces
    using PolygonNode = PolygonNodeT< HalfEdgeType, FaceType, NodeType >;
    using NodePtr     = typename PolygonNode::Ptr;
    using NodeVector  = std::vector< NodePtr >;

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
                    if( pLeft->face() != pRight->face() )
                    {
                        return !pLeft->face();
                    }
                    else
                    {
                        // NOTE: ONLY inner can contain outer - so order Inner > Outer
                        INVARIANT( pLeft->type() != pRight->type(), "Equal polygons are same type" );
                        return pLeft->type() > pRight->type();
                    }
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
        iCount += r->validate();
    }
    INVARIANT( iCount == nodes.size(), "Poly tree error" );

    return rootNodes;
}

} // namespace exact

#endif // GUARD_2023_June_14_polygon_with_holes
