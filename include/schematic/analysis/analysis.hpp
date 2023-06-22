
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
#include "invariant.hpp"

#include "schematic/cgalSettings.hpp"
#include "schematic/space.hpp"
#include "schematic/connection.hpp"

#include <CGAL/Arr_observer.h>

namespace schematic
{
class Schematic;
class Feature_Pin;
class Property;
} // namespace schematic

namespace exact
{
class Analysis
{
public:
    using Ptr = boost::shared_ptr< Analysis >;

    struct PartitionSegment
    {
        using Ptr       = std::unique_ptr< PartitionSegment >;
        using PtrVector = std::vector< Ptr >;
        enum Plane
        {
            eHole,
            eGround,
            eMid,
            eCeiling
        };
        Plane                                plane = eCeiling;
        schematic::Feature_Pin::PtrCstVector pins;
        schematic::Property::PtrCstVector    properties;
    };
    struct Partition
    {
        using Ptr       = std::unique_ptr< Partition >;
        using PtrVector = std::vector< Ptr >;

        schematic::Site::PtrCst              pSite;
        std::vector< PartitionSegment* >     segments;
        schematic::Feature_Pin::PtrCstVector pins;
        schematic::Property::PtrCstVector    properties;
    };

    struct VertexData
    {
    };
    struct HalfEdgeData
    {
        EdgeMask::Set                 flags;
        schematic::Site::PtrCstVector sites;
        Partition*                    pPartition        = nullptr;
        PartitionSegment*             pPartitionSegment = nullptr;

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
        Partition*        pPartition        = nullptr;
        PartitionSegment* pPartitionSegment = nullptr;
    };

    using Dcel           = CGAL::Arr_extended_dcel< Traits, VertexData, HalfEdgeData, FaceData >;
    using Arrangement    = CGAL::Arrangement_with_history_2< Traits, Dcel >;
    using Curve_handle   = Arrangement::Curve_handle;
    using Point_location = CGAL::Arr_simple_point_location< Arrangement >;
    using Formatter      = CGAL::Arr_extended_dcel_text_formatter< Arrangement >;

    Analysis( boost::shared_ptr< schematic::Schematic > pSchematic );
    void contours();
    void ports();
    void partition();
    void properties();
    void lanes();
    void placement();
    void values();
    void visibility();

    // queries used by map format
    using VertexVector         = std::vector< Arrangement::Vertex_const_handle >;
    using HalfEdge             = Arrangement::Halfedge_const_handle;
    using HalfEdgeSet          = std::set< HalfEdge >;
    using HalfEdgeVector       = std::vector< HalfEdge >;
    using HalfEdgeVectorVector = std::vector< HalfEdgeVector >;
    using Face                 = Arrangement::Face_const_handle;
    using FaceVector           = std::vector< Face >;
    using FaceSet              = std::set< Face >;

    // query used by editor for edge visualisation
    void getAllEdges( std::vector< std::pair< schematic::Segment, EdgeMask::Set > >& edges ) const;
    void getFloorPartitions( std::map< const Analysis::Partition*, exact::Polygon_with_holes >& floors );    // const;
    void getBoundaryPartitions( Analysis::HalfEdgeVectorVector& boundarySegments );                          // const;
    void getBoundaryPartitions( std::map< const Analysis::PartitionSegment*, exact::Polygon >& boundaries ); // const;

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

    // query functions used by map compilation
    void getVertices( VertexVector& vertices ) const;
    void getPerimeterPolygon( HalfEdgeVector& polygon ) const;

    struct HalfEdgePolygonWithHoles
    {
        HalfEdgeVector       outer;
        HalfEdgeVectorVector holes;
        using Vector = std::vector< HalfEdgePolygonWithHoles >;
    };

    static inline exact::Polygon fromHalfEdgePolygon( const HalfEdgeVector& poly )
    {
        exact::Polygon outer;
        for( auto& e : poly )
        {
            outer.push_back( e->source()->point() );
        }
        // ensure it is a not a hole
        if( !outer.is_counterclockwise_oriented() )
        {
            std::reverse( outer.begin(), outer.end() );
        }
        return outer;
    }
    static inline exact::Polygon_with_holes fromHalfEdgePolygonWithHoles( const HalfEdgePolygonWithHoles& poly )
    {
        exact::Polygon_with_holes polygonWithHoles( fromHalfEdgePolygon( poly.outer ) );
        for( auto& h : poly.holes )
        {
            Polygon hole;
            for( auto& e : h )
            {
                hole.push_back( e->source()->point() );
            }
            // ensure it is a hole
            if( !hole.is_clockwise_oriented() )
            {
                std::reverse( hole.begin(), hole.end() );
            }
            polygonWithHoles.add_hole( hole );
        }
        return polygonWithHoles;
    }

    struct Floor
    {
        Partition*                       pPartition;
        HalfEdgePolygonWithHoles         floor;
        HalfEdgePolygonWithHoles::Vector ex1;
        HalfEdgePolygonWithHoles::Vector ex2;
        HalfEdgePolygonWithHoles::Vector ex3;
        HalfEdgePolygonWithHoles::Vector ex4;
        using Vector = std::vector< Floor >;
    };
    Floor::Vector getFloors();

    struct Boundary
    {
        using Vector = std::vector< Boundary >;

        struct Pane
        {
            // type:Type;
            // properties:[Properties];
            PartitionSegment::Plane lower, upper;
            HalfEdgeSet             edges;
        };

        struct WallSection
        {
            // type:Type;
            // properties:[Properties];
            PartitionSegment::Plane lower, upper;
            HalfEdgeVector          edges;
        };

        // type:Type;
        // properties:[Properties];
        HalfEdgeVector contour;

        HalfEdgeVectorVector hori_holes;
        HalfEdgeVectorVector hori_floors;
        HalfEdgeVectorVector hori_mids;
        HalfEdgeVectorVector hori_ceilings;

        std::vector< Pane >        panes;
        std::vector< WallSection > walls;
    };
    Boundary::Vector getBoundaries();

private:
    // internal analysis routines
    void contoursRecurse( schematic::Site::Ptr pSpace );
    void renderSiteContours( schematic::Site::Ptr pSpace );
    void connect( schematic::Site::Ptr pSite );
    void constructConnectionEdges( schematic::Connection::Ptr   pConnection,
                                   Arrangement::Halfedge_handle firstBisectorEdge,
                                   Arrangement::Halfedge_handle secondBisectorEdge );
    void cut( schematic::Site::Ptr pSite );
    void propertiesRecurse( schematic::Site::Ptr pSpace, std::vector< schematic::Property::Ptr >& properties );

    boost::shared_ptr< schematic::Schematic > m_pSchematic;

    Partition::PtrVector        m_floors, m_boundaries;
    PartitionSegment::PtrVector m_boundarySegments;
    Arrangement                 m_arr;
    Observer                    m_observer;
    schematic::MonoBitmap&      m_laneBitmap;
};
} // namespace exact

#endif // GUARD_2023_June_05_analysis
