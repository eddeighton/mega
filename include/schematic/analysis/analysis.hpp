
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

    struct VertexData
    {
    };
    struct HalfEdgeData
    {
        HalfEdgeData() = default;
        HalfEdgeData( EdgeMask::Type flag ) { flags.set( flag ); }
        EdgeMask::Set        flags;
        schematic::Site::Ptr pSite = schematic::Site::Ptr{};
    };
    struct FaceData
    {
    };

    using Dcel           = CGAL::Arr_extended_dcel< Traits, VertexData, HalfEdgeData, FaceData >;
    using Arrangement    = CGAL::Arrangement_with_history_2< Traits, Dcel >;
    using Curve_handle   = Arrangement::Curve_handle;
    using Point_location = CGAL::Arr_simple_point_location< Arrangement >;
    using Formatter      = CGAL::Arr_extended_dcel_text_formatter< Arrangement >;

    class Observer : public CGAL::Arr_observer< Arrangement >
    {
        std::optional< HalfEdgeData > m_data, m_dataTwin;

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
            VERIFY_RTE_MSG(
                !m_data.has_value() && !m_dataTwin.has_value(), "before_split_edge when already have halfedge data" );
            m_data     = e->data();
            m_dataTwin = e->twin()->data();
        }

        // Set the captured data to both new edges - ON BOTH SIDES
        virtual void after_split_edge( Halfedge_handle e1, Halfedge_handle e2 )
        {
            VERIFY_RTE_MSG( m_data.has_value() && m_dataTwin.has_value(), "after_split_edge when no halfedge data" );

            e1->set_data( m_data.value() );
            e2->set_data( m_data.value() );
            e1->twin()->set_data( m_dataTwin.value() );
            e2->twin()->set_data( m_dataTwin.value() );
            m_data.reset();
            m_dataTwin.reset();
        }
    };

    Analysis( boost::shared_ptr< schematic::Schematic > pSchematic );

    using FaceHandle    = Arrangement::Face_const_handle;
    using FaceHandleSet = std::set< FaceHandle >;

    void getEdges( std::vector< std::pair< schematic::Segment, EdgeMask::Set > >& edges );

private:
    void classify( Arrangement::Halfedge_handle h, EdgeMask::Type mask );
    void renderContour( const exact::Transform& transform,
                        const exact::Polygon&   poly,
                        EdgeMask::Type          innerMask,
                        EdgeMask::Type          outerMask );
    void recurse( schematic::Site::Ptr pSpace );
    void recursePost( schematic::Site::Ptr pSpace );
    void connect( schematic::Site::Ptr pConnection );
    void findSpaceFaces( schematic::Space::Ptr pSpace, FaceHandleSet& faces, FaceHandleSet& spaceFaces );
    void constructConnectionEdges( schematic::Connection::Ptr   pConnection,
                                   Arrangement::Halfedge_handle firstBisectorEdge,
                                   Arrangement::Halfedge_handle secondBisectorEdge );

    boost::shared_ptr< schematic::Schematic > m_pSchematic;

    Arrangement m_arr;
    Observer    m_observer;
};
} // namespace exact

#endif // GUARD_2023_June_05_analysis
