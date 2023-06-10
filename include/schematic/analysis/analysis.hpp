
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

#ifndef GUARD_2023_June_05_analysis
#define GUARD_2023_June_05_analysis

#include "edge_mask.hpp"

#include "schematic/cgalSettings.hpp"
#include "schematic/space.hpp"
#include "schematic/connection.hpp"

#include <CGAL/Arr_observer.h>

namespace schematic
{
class Schematic;
}

namespace exact
{
class Analysis
{
public:
    using Ptr = boost::shared_ptr< Analysis >;

    struct Partition
    {
        using Ptr       = std::unique_ptr< Partition >;
        using PtrVector = std::vector< Ptr >;

        schematic::Site::PtrCst              pSite;
        schematic::Feature_Pin::PtrCstVector pins;
    };

    struct VertexData
    {
    };
    struct HalfEdgeData
    {
        EdgeMask::Set                 flags;
        schematic::Site::PtrCstVector sites;
        Partition*                    pPartition = nullptr;

        void appendSiteUnique( schematic::Site::PtrCst pSite )
        {
            if( sites.empty() || sites.back() != pSite )
            {
                sites.push_back( pSite );
            }
        }
    };
    struct FaceData
    {
        Partition* pPartition = nullptr;
    };

    using Dcel           = CGAL::Arr_extended_dcel< Traits, VertexData, HalfEdgeData, FaceData >;
    using Arrangement    = CGAL::Arrangement_with_history_2< Traits, Dcel >;
    using Curve_handle   = Arrangement::Curve_handle;
    using Point_location = CGAL::Arr_simple_point_location< Arrangement >;
    using Formatter      = CGAL::Arr_extended_dcel_text_formatter< Arrangement >;

    class Observer : public CGAL::Arr_observer< Arrangement >
    {
        std::optional< HalfEdgeData > m_edgeData, m_edgeDataTwin;
        std::optional< FaceData >     m_faceData;

    public:
        Observer( Arrangement& arr )
            : CGAL::Arr_observer< Arrangement >( arr )
        {
            CGAL_precondition( arr.is_empty() );
        }

        // Capture the halfedge data before the edge is split - FOR BOTH sides
        virtual void before_split_edge( Halfedge_handle           e,
                                        Vertex_handle             v,
                                        const X_monotone_curve_2& c1,
                                        const X_monotone_curve_2& c2 )
        {
            VERIFY_RTE_MSG( !m_edgeData.has_value() && !m_edgeDataTwin.has_value(),
                            "before_split_edge when already have halfedge data" );
            m_edgeData     = e->data();
            m_edgeDataTwin = e->twin()->data();
        }

        // Set the captured data to both new edges - ON BOTH SIDES
        virtual void after_split_edge( Halfedge_handle e1, Halfedge_handle e2 )
        {
            VERIFY_RTE_MSG(
                m_edgeData.has_value() && m_edgeDataTwin.has_value(), "after_split_edge when no halfedge data" );

            e1->set_data( m_edgeData.value() );
            e2->set_data( m_edgeData.value() );
            e1->twin()->set_data( m_edgeDataTwin.value() );
            e2->twin()->set_data( m_edgeDataTwin.value() );
            m_edgeData.reset();
            m_edgeDataTwin.reset();
        }

        virtual void before_split_face( Face_handle f, Halfedge_handle e )
        {
            VERIFY_RTE_MSG( !m_faceData.has_value(), "before_split_face when already have face data" );
            m_faceData = f->data();
        }

        virtual void after_split_face( Face_handle f1, Face_handle f2, bool is_hole )
        {
            VERIFY_RTE_MSG( m_faceData.has_value(), "after_split_face when no face data" );
            f1->set_data( m_faceData.value() );
            f2->set_data( m_faceData.value() );
            m_faceData.reset();
        }
    };

    Analysis( boost::shared_ptr< schematic::Schematic > pSchematic );

    // query used by editor for edge visualisation
    void getEdges( std::vector< std::pair< schematic::Segment, EdgeMask::Set > >& edges );

    // queries used by map format
    using VertexVector         = std::vector< Arrangement::Vertex_const_handle >;
    using HalfEdgeVector       = std::vector< Arrangement::Halfedge_const_handle >;
    using HalfEdgeVectorVector = std::vector< HalfEdgeVector >;

    void getVertices( VertexVector& vertices ) const;
    void getPerimeterPolygon( HalfEdgeVector& polygon ) const;
    void getBoundaryPolygons( HalfEdgeVectorVector& polygons ) const;

private:
    template < typename TEdgeType >
    inline void classify( TEdgeType h, EdgeMask::Type mask )
    {
        h->data().flags.set( mask );
    }

    // internal analysis routines
    void renderContour( const exact::Transform& transform,
                        const exact::Polygon&   poly,
                        EdgeMask::Type          innerMask,
                        EdgeMask::Type          outerMask,
                        schematic::Site::PtrCst pInnerSite,
                        schematic::Site::PtrCst pOuterSite );
    void recurse( schematic::Site::Ptr pSpace );
    void recursePost( schematic::Site::Ptr pSpace );
    void connect( schematic::Site::Ptr pConnection );
    void constructConnectionEdges( schematic::Connection::Ptr   pConnection,
                                   Arrangement::Halfedge_handle firstBisectorEdge,
                                   Arrangement::Halfedge_handle secondBisectorEdge );
                                   
    template< typename TFunctor >
    inline bool anyFaceEdge( Arrangement::Face_const_handle hFace, TFunctor&& predicate ) const
    {
        if( !hFace->is_unbounded() )
        {
            Arrangement::Ccb_halfedge_const_circulator iter  = hFace->outer_ccb();
            Arrangement::Ccb_halfedge_const_circulator start = iter;
            do
            {
                if( predicate( iter->data() ) )
                {
                    return true;
                }
                ++iter;
            } while( iter != start );
        }

        // search through all holes
        for( Arrangement::Hole_const_iterator holeIter = hFace->holes_begin(), holeIterEnd = hFace->holes_end();
            holeIter != holeIterEnd; ++holeIter )
        {
            Arrangement::Ccb_halfedge_const_circulator iter  = *holeIter;
            Arrangement::Ccb_halfedge_const_circulator start = iter;
            do
            {
                if( predicate( iter->data() ) )
                {
                    return true;
                }
                ++iter;
            } while( iter != start );
        }
        return false;
    }          
    template< typename TFunctor >
    inline bool allFaceEdges( Arrangement::Face_const_handle hFace, TFunctor&& predicate ) const
    {
        if( !hFace->is_unbounded() )
        {
            Arrangement::Ccb_halfedge_const_circulator iter  = hFace->outer_ccb();
            Arrangement::Ccb_halfedge_const_circulator start = iter;
            do
            {
                if( !predicate( iter->data() ) )
                {
                    return false;
                }
                ++iter;
            } while( iter != start );
        }

        // search through all holes
        for( Arrangement::Hole_const_iterator holeIter = hFace->holes_begin(), holeIterEnd = hFace->holes_end();
            holeIter != holeIterEnd; ++holeIter )
        {
            Arrangement::Ccb_halfedge_const_circulator iter  = *holeIter;
            Arrangement::Ccb_halfedge_const_circulator start = iter;
            do
            {
                if( !predicate( iter->data() ) )
                {
                    return false;
                }
                ++iter;
            } while( iter != start );
        }
        return true;
    }

    bool areFacesAdjacent( Arrangement::Face_const_handle hFace1, Arrangement::Face_const_handle hFace2 ) const;
    void partition();

    // internal query implementation helper functions
    using HalfEdge    = Arrangement::Halfedge_const_handle;
    using HalfEdgeSet = std::set< HalfEdge >;

    template < typename Predicate >
    inline void getEdges( HalfEdgeSet& edges, Predicate&& predicate ) const
    {
        for( auto i = m_arr.halfedges_begin(); i != m_arr.halfedges_end(); ++i )
        {
            if( predicate( i ) )
            {
                edges.insert( i );
            }
        }
    }

    inline void getPolygons( const HalfEdgeSet& edges, HalfEdgeVectorVector& polygons ) const
    {
        HalfEdgeSet open = edges;

        while( !open.empty() )
        {
            HalfEdge currentEdge = *open.begin();

            HalfEdgeVector polygon;
            while( currentEdge != HalfEdge{} )
            {
                polygon.push_back( currentEdge );
                open.erase( currentEdge );

                HalfEdge nextEdge = {};
                {
                    using EdgeIter    = Arrangement::Halfedge_around_vertex_const_circulator;
                    EdgeIter edgeIter = currentEdge->target()->incident_halfedges(), edgeIterEnd = edgeIter;
                    do
                    {
                        HalfEdge outEdge = edgeIter->twin();
                        if( ( currentEdge != outEdge ) && open.contains( outEdge ) )
                        {
                            VERIFY_RTE_MSG( nextEdge == HalfEdge{}, "Duplicate next edge found" );
                            nextEdge = outEdge;
                        }
                        ++edgeIter;
                    } while( edgeIter != edgeIterEnd );
                }
                currentEdge = nextEdge;
            }
            VERIFY_RTE_MSG( polygon.size() >= 3, "Invalid polygon found with less than three edges" );
            polygons.push_back( polygon );
        }
    }

    boost::shared_ptr< schematic::Schematic > m_pSchematic;

    Partition::PtrVector m_partitions;
    Arrangement          m_arr;
    Observer             m_observer;
};
} // namespace exact

#endif // GUARD_2023_June_05_analysis
