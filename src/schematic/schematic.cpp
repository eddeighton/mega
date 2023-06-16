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

#include "schematic/schematic.hpp"
#include "schematic/file.hpp"
#include "schematic/space.hpp"
#include "schematic/wall.hpp"
#include "schematic/object.hpp"
#include "schematic/connection.hpp"
#include "schematic/factory.hpp"

#include "map/map_format.h"

#include "CGAL/Constrained_Delaunay_triangulation_2.h"
#include "CGAL/Triangulation_face_base_with_info_2.h"
#include "CGAL/Triangulation_vertex_base_with_info_2.h"

#include "common/file.hpp"

namespace schematic
{

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
Schematic::Schematic( const std::string& strName )
    : BaseType( strName )
{
}

Schematic::Schematic( Schematic::PtrCst pOriginal, Node::Ptr pNewParent, const std::string& strName )
    : BaseType( pOriginal, pNewParent, strName )
{
}

void Schematic::init()
{
    File::init();

    if( !m_pAnalysisMarkup.get() )
    {
        m_pAnalysisMarkup.reset( new MultiPathMarkup( *this, nullptr, Schematic::eStage_Compilation ) );
    }
}

Node::Ptr Schematic::copy( Node::Ptr pParent, const std::string& strName ) const
{
    VERIFY_RTE( !pParent );
    return Node::copy< Schematic >(
        boost::dynamic_pointer_cast< const Schematic >( shared_from_this() ), pParent, strName );
}

void Schematic::load( const format::Node& node )
{
    File::load( node );
    VERIFY_RTE( node.has_file() && node.file().has_schematic() );
}

void Schematic::save( format::Node& node ) const
{
    format::Node::File::Schematic& schematic = *node.mutable_file()->mutable_schematic();
    File::save( node );
}

void Schematic::task_contours()
{
    for( Site::Ptr pSite : getSites() )
    {
        pSite->task_contour();
    }
}

void Schematic::task_extrusions()
{
    for( Site::Ptr pSite : getSites() )
    {
        if( Space::Ptr pSpace = boost::dynamic_pointer_cast< Space >( pSite ) )
        {
            pSpace->task_extrusions();
        }
    }
}

bool Schematic::task_compilation( std::ostream& os )
{
    std::vector< MultiPathMarkup::SegmentMask > edges;
    try
    {
        Schematic::Ptr pThis = boost::dynamic_pointer_cast< Schematic >( getPtr() );

        m_pAnalysis.reset();
        m_pAnalysis.reset( new exact::Analysis( pThis ) );
        m_pAnalysis->getAllEdges( edges );
        m_pAnalysisMarkup->set( edges );
    }
    catch( std::exception& ex )
    {
        os << ex.what();
        edges.clear();
        m_pAnalysisMarkup->set( edges );
        return false;
    }

    return true;
}

bool Schematic::task_partition( std::ostream& os )
{
    std::vector< MultiPathMarkup::SegmentMask > edges;
    try
    {
        if( m_pAnalysis )
        {
            m_pAnalysis->partition();
            m_pAnalysis->getAllEdges( edges );
            m_pAnalysisMarkup->set( edges );
        }
    }
    catch( std::exception& ex )
    {
        os << ex.what();
        m_pAnalysis.reset();
        edges.clear();
        m_pAnalysisMarkup->set( edges );
        return false;
    }

    return true;
}

bool Schematic::task_skeleton( std::ostream& os )
{
    std::vector< MultiPathMarkup::SegmentMask > edges;
    try
    {
        if( m_pAnalysis )
        {
            m_pAnalysis->skeleton();
            m_pAnalysis->getAllEdges( edges );
            m_pAnalysisMarkup->set( edges );
        }
    }
    catch( std::exception& ex )
    {
        os << ex.what();
        m_pAnalysis.reset();
        edges.clear();
        m_pAnalysisMarkup->set( edges );
        return false;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// Map compilation routines
namespace fb = flatbuffers;

using AreaMap = std::map< Site::Ptr, fb::Offset< Mega::Area > >;

fb::Offset< Mega::Area > recurseSites( fb::FlatBufferBuilder& builder, Site::Ptr pSite, AreaMap& areas )
{
    fb::Offset< Mega::Area > pAreaPtr;

    // recurse and collected nested areas
    std::vector< fb::Offset< Mega::Area > > fbAreas;
    for( Site::Ptr pChildSite : pSite->getSites() )
    {
        auto fbArea = recurseSites( builder, pChildSite, areas );
        fbAreas.push_back( fbArea );
    }

    // collect area properties
    std::vector< fb::Offset< Mega::Properties > > properties;

    // Mega::AreaBuilder::
    {
        auto propertiesVec = builder.CreateVector( properties );
        auto areasVec      = builder.CreateVector( fbAreas );

        Mega::AreaBuilder areaBuilder( builder );
        Mega::Type        type( 1 );
        areaBuilder.add_type( &type );
        areaBuilder.add_properties( propertiesVec );
        areaBuilder.add_areas( areasVec );

        pAreaPtr = areaBuilder.Finish();
    }

    areas.insert( { pSite, pAreaPtr } );

    return pAreaPtr;
}

struct FBVertMap
{
    using AnalysisVert = exact::Analysis::Arrangement::Vertex_const_handle;
    using FBVert       = fb::Offset< Mega::Vertex >;
    using VertexMap    = std::map< AnalysisVert, FBVert >;
    using ValueMap     = std::map< AnalysisVert, Mega::F2 >;

    FBVertMap( exact::Analysis& analysis, fb::FlatBufferBuilder& builder )
    {
        exact::ExactToInexact convert;

        // record all vertices
        exact::Analysis::VertexVector vertices;
        analysis.getVertices( vertices );

        for( auto pVert : vertices )
        {
            const schematic::Point pt = convert( pVert->point() );
            const Mega::F2         f2( pt.x(), pt.y() );

            Mega::VertexBuilder vertBuilder( builder );
            vertBuilder.add_position( &f2 );
            fb::Offset< Mega::Vertex > pVertex = vertBuilder.Finish();
            vertexMap.insert( { pVert, pVertex } );
            valueMap.insert( { pVert, f2 } );
        }
    }

    const FBVert& operator()( AnalysisVert pVert ) const
    {
        auto iFind = vertexMap.find( pVert );
        VERIFY_RTE_MSG( iFind != vertexMap.end(), "Failed to find vertex" );
        return iFind->second;
    }

    const Mega::F2& get( AnalysisVert pVert ) const
    {
        auto iFind = valueMap.find( pVert );
        VERIFY_RTE_MSG( iFind != valueMap.end(), "Failed to find vertex" );
        return iFind->second;
    }

private:
    VertexMap vertexMap;
    ValueMap  valueMap;
};

fb::Offset< Mega::Polygon > buildPolygon( const FBVertMap& fbVertMap, const exact::Analysis::HalfEdgeVector& contour,
                                          fb::FlatBufferBuilder& builder )
{
    std::vector< FBVertMap::FBVert > vertices;
    {
        for( auto pEdge : contour )
        {
            vertices.push_back( fbVertMap( pEdge->source() ) );
        }
    }
    auto pVerts = builder.CreateVector( vertices );

    Mega::PolygonBuilder polyBuilder( builder );
    polyBuilder.add_vertices( pVerts );
    return polyBuilder.Finish();
}

fb::Offset< Mega::Mesh > buildHorizontalMesh( const FBVertMap&                                 fbVertMap,
                                              const exact::Analysis::HalfEdgePolygonWithHoles& poly,
                                              fb::FlatBufferBuilder&                           builder )
{
    struct FaceData
    {
        FaceData()         = default;
        int  nesting_level = -1;
        bool in_domain()
        {
            // VERIFY_RTE_MSG( nesting_level < 2, "Nested polygon in polygon with holes" );
            return nesting_level % 2 == 1;
        }
    };

    struct VertexData
    {
        exact::Analysis::Arrangement::Vertex_const_handle vertex;
    };

    using K    = exact::Kernel;
    using Vb   = CGAL::Triangulation_vertex_base_with_info_2< VertexData, K >;
    using Fbb  = CGAL::Triangulation_face_base_with_info_2< FaceData, K >;
    using Fb   = CGAL::Constrained_triangulation_face_base_2< K, Fbb >;
    using TDS  = CGAL::Triangulation_data_structure_2< Vb, Fb >;
    using Itag = CGAL::Exact_predicates_tag;
    using CDT  = CGAL::Constrained_Delaunay_triangulation_2< K, TDS, Itag >;
    // NOTE: CDT::Edge = std::pair< Face_handle, int >

    CDT cdt;

    {
        auto insertConstraints = [ & ]( const exact::Analysis::HalfEdgeVector& contour )
        {
            std::vector< CDT::Vertex_handle > verts;
            // create the vertices - recording the original vertex handle from the arrangement
            for( auto& e : contour )
            {
                CDT::Vertex_handle v = cdt.insert( e->source()->point() );
                v->info().vertex     = e->source();
                verts.push_back( v );
            }
            for( auto i = verts.begin(), iNext = verts.begin(), iEnd = verts.end(); i != iEnd; ++i )
            {
                ++iNext;
                if( iNext == iEnd )
                {
                    iNext = verts.begin();
                }
                cdt.insert_constraint( *i, *iNext );
            }
        };

        // NOTE: does not matter about order here
        insertConstraints( poly.outer );
        for( auto& h : poly.holes )
        {
            insertConstraints( h );
        }
    }

    {
        using CDTEdgeList = std::list< CDT::Edge >;
        using CDTFaceList = std::list< CDT::Face_handle >;

        // https://doc.cgal.org/latest/Triangulation_2/index.html#Section_2D_Triangulations_Constrained_Delaunay

        auto mark_domains = [ & ]( CDT::Face_handle startFace, int index, CDTEdgeList& border )
        {
            CDTFaceList faceQueue;
            faceQueue.push_back( startFace );
            while( !faceQueue.empty() )
            {
                CDT::Face_handle fh = faceQueue.front();
                faceQueue.pop_front();

                if( fh->info().nesting_level == -1 )
                {
                    fh->info().nesting_level = index;
                    for( int i = 0; i < 3; i++ )
                    {
                        CDT::Face_handle n = fh->neighbor( i );
                        if( n->info().nesting_level == -1 )
                        {
                            const CDT::Edge e( fh, i );
                            if( cdt.is_constrained( e ) )
                                border.push_back( e );
                            else
                                faceQueue.push_back( n );
                        }
                    }
                }
            }
        };

        // use the cdt.infinite_face() as starting point
        // implement breadth first search marking nesting level
        CDTEdgeList border;
        mark_domains( cdt.infinite_face(), 0, border );
        while( !border.empty() )
        {
            CDT::Edge e = border.front();
            border.pop_front();
            // cross the constrained edge from one face to other face
            CDT::Face_handle n = e.first->neighbor( e.second );
            if( n->info().nesting_level == -1 )
            {
                mark_domains( n, e.first->info().nesting_level + 1, border );
            }
        }
    }

    using ArrVert = exact::Analysis::Arrangement::Vertex_const_handle;

    std::vector< fb::Offset< Mega::Vertex3D > > vertices;
    std::vector< int >                          indices;
    for( CDT::Face_handle f : cdt.finite_face_handles() )
    {
        if( f->info().in_domain() )
        {
            for( int i = 0; i != 3; ++i )
            {
                CDT::Vertex_handle triVert = f->vertex( i );
                ArrVert            v       = triVert->info().vertex;
                const auto&        value   = fbVertMap.get( v );
                FBVertMap::FBVert  fbVert  = fbVertMap( v );

                const Mega::F3 normal( 0.0f, 1.0f, 0.0f );
                const Mega::F4 tangent( 0.0f, 1.0f, 0.0f, 0.0f );
                const Mega::F2 uv( value.x(), value.y() );

                Mega::Vertex3DBuilder vertBuilder( builder );
                vertBuilder.add_vertex( fbVert );
                vertBuilder.add_normal( &normal );
                vertBuilder.add_plane( Mega::Plane::Plane_eLower );
                vertBuilder.add_tangent( &tangent );
                vertBuilder.add_uv( &uv );
                auto fbVert3d = vertBuilder.Finish();

                indices.push_back( vertices.size() );
                vertices.push_back( fbVert3d );
            }
        }
    }

    auto fbIndices = builder.CreateVector( indices );
    auto fbVerts   = builder.CreateVector( vertices );

    Mega::MeshBuilder meshBuilder( builder );

    meshBuilder.add_indices( fbIndices );
    meshBuilder.add_vertices( fbVerts );

    return meshBuilder.Finish();
}

void Schematic::compileMap( const boost::filesystem::path& filePath )
{
    if( !m_pAnalysis )
    {
        std::ostringstream osError;

        task_contours();
        task_extrusions();
        if( !task_compilation( osError ) )
        {
            throw std::runtime_error( osError.str() );
        }
        if( !task_partition( osError ) )
        {
            throw std::runtime_error( osError.str() );
        }
        if( !task_skeleton( osError ) )
        {
            throw std::runtime_error( osError.str() );
        }
    }
    VERIFY_RTE_MSG( m_pAnalysis, "Schematic analysis failed" );

    fb::FlatBufferBuilder builder;

    FBVertMap fbVertMap( *m_pAnalysis, builder );

    // write the site tree
    fb::Offset< Mega::Area > fbRootArea;
    AreaMap                  areaMap;
    {
        const auto sites = getSites();
        VERIFY_RTE_MSG( sites.size() < 2, "More than one root site in schematic" );
        if( !sites.empty() )
        {
            fbRootArea = recurseSites( builder, sites.front(), areaMap );
        }
    }

    using namespace exact;

    fb::Offset< Mega::Polygon > fbMapPolygon;
    {
        Analysis::HalfEdgeVector perimeter;
        m_pAnalysis->getPerimeterPolygon( perimeter );
        fbMapPolygon = buildPolygon( fbVertMap, perimeter, builder );
    }

    Analysis::Floor::Vector floors = m_pAnalysis->getFloors();

    std::vector< fb::Offset< Mega::Floor > > fbFloorsVec;

    for( const auto& floor : floors )
    {
        fb::Offset< Mega::Mesh > fbMesh = buildHorizontalMesh( fbVertMap, floor.floor, builder );

        auto fbFloorPolygon = buildPolygon( fbVertMap, floor.floor.outer, builder );

        std::vector< fb::Offset< Mega::Mesh > > ex1Polys;
        for( auto& ex : floor.ex1 )
        {
            ex1Polys.push_back( buildHorizontalMesh( fbVertMap, ex, builder ) );
        }
        std::vector< fb::Offset< Mega::Mesh > > ex2Polys;
        for( auto& ex : floor.ex2 )
        {
            ex2Polys.push_back( buildHorizontalMesh( fbVertMap, ex, builder ) );
        }
        std::vector< fb::Offset< Mega::Mesh > > ex3Polys;
        for( auto& ex : floor.ex3 )
        {
            ex3Polys.push_back( buildHorizontalMesh( fbVertMap, ex, builder ) );
        }
        std::vector< fb::Offset< Mega::Mesh > > ex4Polys;
        for( auto& ex : floor.ex4 )
        {
            ex4Polys.push_back( buildHorizontalMesh( fbVertMap, ex, builder ) );
        }

        auto fbEx1 = builder.CreateVector( ex1Polys );
        auto fbEx2 = builder.CreateVector( ex2Polys );
        auto fbEx3 = builder.CreateVector( ex3Polys );
        auto fbEx4 = builder.CreateVector( ex4Polys );

        Mega::FloorBuilder floorBuilder( builder );

        floorBuilder.add_contour( fbFloorPolygon );
        floorBuilder.add_floor( fbMesh );
        floorBuilder.add_ground_one( fbEx1 );
        floorBuilder.add_ground_two( fbEx2 );
        floorBuilder.add_ground_three( fbEx3 );
        floorBuilder.add_ground_four( fbEx4 );

        fbFloorsVec.push_back( floorBuilder.Finish() );
    }

    auto fbFloors = builder.CreateVector( fbFloorsVec );

    // map
    fb::Offset< Mega::Map > fbMap;
    {
        // auto pPartitionsVector = builder.CreateVector( mapPartitions );

        Mega::MapBuilder mapBuilder( builder );
        mapBuilder.add_contour( fbMapPolygon );
        mapBuilder.add_root_area( fbRootArea );
        mapBuilder.add_floors( fbFloors );
        fbMap = mapBuilder.Finish();
    }

    builder.Finish( fbMap );

    // write the file
    {
        const auto                                     size    = builder.GetSize();
        const uint8_t*                                 pBuffer = builder.GetBufferPointer();
        std::unique_ptr< boost::filesystem::ofstream > pFileStream
            = boost::filesystem::createBinaryOutputFileStream( filePath );
        pFileStream->write( reinterpret_cast< const char* >( pBuffer ), size );
    }
}

} // namespace schematic
