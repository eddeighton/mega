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

#include "map/map_format.h"

#include "schematic/cgalUtils.hpp"

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
        m_pAnalysisMarkup.reset( new MultiPathMarkup( *this, nullptr, eStage_Port ) );
    }
    if( !m_pPropertiesMarkup.get() )
    {
        m_pPropertiesMarkup.reset( new MultiPolygonMarkup( *this, nullptr, true, eStage_Properties ) );
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

bool Schematic::compile( CompilationStage stage, std::ostream& os )
{
    const int iStage = static_cast< int >( stage );

    Schematic::Ptr pThis = boost::dynamic_pointer_cast< Schematic >( getPtr() );

    try
    {
        if( iStage >= eStage_SiteContour )
        {
            for( Site::Ptr pSite : getSites() )
            {
                pSite->task_contour();
            }
        }

        if( iStage >= eStage_Extrusion )
        {
            for( Site::Ptr pSite : getSites() )
            {
                if( Space::Ptr pSpace = boost::dynamic_pointer_cast< Space >( pSite ) )
                {
                    pSpace->task_extrusions();
                }
            }
        }

        if( iStage >= eStage_Port )
        {
            m_pAnalysis.reset();
            m_pAnalysis.reset( new exact::Analysis( pThis ) );
            m_pAnalysis->contours();
            m_pAnalysis->ports();

            std::vector< MultiPathMarkup::SegmentMask > edges;
            m_pAnalysis->getAllEdges( edges );
            m_pAnalysisMarkup->set( edges );
        }

        if( iStage >= eStage_Partition )
        {
            m_pAnalysis->partition();

            std::vector< MultiPathMarkup::SegmentMask > edges;
            m_pAnalysis->getAllEdges( edges );
            m_pAnalysisMarkup->set( edges );
        }

        if( iStage >= eStage_Properties )
        {
            m_pAnalysis->properties();

            std::map< const exact::Analysis::Partition*, exact::Polygon_with_holes > floors;
            m_pAnalysis->getFloorPartitions( floors );

            std::map< const exact::Analysis::PartitionSegment*, exact::Polygon > boundaries;
            m_pAnalysis->getBoundaryPartitions( boundaries );

            MultiPolygonMarkup::PolygonWithHolesVector polygons;
            MultiPolygonMarkup::PolygonStyles          styles;
            for( const auto& [ pPartition, poly ] : floors )
            {
                polygons.push_back( Utils::convert< schematic::Kernel >( poly ) );

                std::string strStyle = "";
                for( auto pProperty : pPartition->properties )
                {
                    if( pProperty->getName() == "type" )
                    {
                        strStyle = pProperty->getValue();
                    }
                }
                styles.push_back( strStyle );
            }
            for( const auto& [ pPartitionSegment, poly ] : boundaries )
            {
                polygons.push_back( Polygon_with_holes{ Utils::convert< schematic::Kernel >( poly ) } );

                std::string strStyle = "";
                for( auto pProperty : pPartitionSegment->properties )
                {
                    if( pProperty->getName() == "type" )
                    {
                        strStyle = pProperty->getValue();
                    }
                }
                styles.push_back( strStyle );
            }

            m_pPropertiesMarkup->setPolygons( polygons );
            m_pPropertiesMarkup->setStyles( styles );
        }

        if( iStage >= eStage_Skeleton )
        {
            m_pAnalysis->skeleton();

            std::vector< MultiPathMarkup::SegmentMask > edges;
            m_pAnalysis->getAllEdges( edges );
            m_pAnalysisMarkup->set( edges );
        }
    }
    catch( std::exception& ex )
    {
        os << ex.what();

        m_pAnalysisMarkup->reset();
        m_pPropertiesMarkup->reset();
        return false;
    }
    catch( ... )
    {
        os << "Unknown exception compiling schematic";
        m_pAnalysisMarkup->reset();
        m_pPropertiesMarkup->reset();
        return false;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// Map compilation routines
namespace
{
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

struct Triangulation
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

    decltype( cdt.finite_face_handles() ) solve()
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

        return cdt.finite_face_handles();
    }
};

std::size_t buildVertex( fb::FlatBufferBuilder&                       builder,
                         std::vector< fb::Offset< Mega::Vertex3D > >& vertices,
                         const Mega::F2&                              uv,
                         const Mega::F3&                              normal,
                         const Mega::F4&                              tangent,
                         FBVertMap::FBVert                            fbVert,
                         Mega::Plane                                  plane )
{
    Mega::Vertex3DBuilder vertBuilder( builder );
    vertBuilder.add_vertex( fbVert );
    vertBuilder.add_normal( &normal );
    vertBuilder.add_plane( plane );
    vertBuilder.add_tangent( &tangent );
    vertBuilder.add_uv( &uv );
    auto fbVert3d = vertBuilder.Finish();

    std::size_t szIndex = vertices.size();
    vertices.push_back( fbVert3d );

    return szIndex;
}

fb::Offset< Mega::Mesh > buildHorizontalMesh( const FBVertMap&                                 fbVertMap,
                                              Mega::Plane                                      plane,
                                              const exact::Analysis::HalfEdgePolygonWithHoles& poly,
                                              fb::FlatBufferBuilder&                           builder )
{
    Triangulation triangulation;
    {
        auto insertConstraints = [ & ]( const exact::Analysis::HalfEdgeVector& contour )
        {
            std::vector< Triangulation::CDT::Vertex_handle > verts;
            // create the vertices - recording the original vertex handle from the arrangement
            for( auto& e : contour )
            {
                Triangulation::CDT::Vertex_handle v = triangulation.cdt.insert( e->source()->point() );
                v->info().vertex                    = e->source();
                verts.push_back( v );
            }
            for( auto i = verts.begin(), iNext = verts.begin(), iEnd = verts.end(); i != iEnd; ++i )
            {
                ++iNext;
                if( iNext == iEnd )
                {
                    iNext = verts.begin();
                }
                triangulation.cdt.insert_constraint( *i, *iNext );
            }
        };

        // NOTE: does not matter about order here
        insertConstraints( poly.outer );
        for( auto& h : poly.holes )
        {
            insertConstraints( h );
        }
    }

    using ArrVert = exact::Analysis::Arrangement::Vertex_const_handle;

    const Mega::F3 normal( 0.0f, 1.0f, 0.0f );
    const Mega::F4 tangent( 0.0f, 1.0f, 0.0f, 0.0f );

    std::vector< fb::Offset< Mega::Vertex3D > > vertices;
    std::vector< int >                          indices;
    for( Triangulation::CDT::Face_handle f : triangulation.solve() )
    {
        if( f->info().in_domain() )
        {
            for( int i = 0; i != 3; ++i )
            {
                auto        v       = f->vertex( i )->info().vertex;
                const auto& value   = fbVertMap.get( v );
                std::size_t szIndex = buildVertex(
                    builder, vertices, Mega::F2( value.x(), value.y() ), normal, tangent, fbVertMap( v ), plane );

                indices.push_back( szIndex );
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

// Generate a seperate quad for each edge
fb::Offset< Mega::Mesh > buildVerticalMesh( const FBVertMap&                    fbVertMap,
                                            Mega::Plane                         lower,
                                            Mega::Plane                         upper,
                                            const exact::Analysis::HalfEdgeSet& edges,
                                            fb::FlatBufferBuilder&              builder )
{
    std::vector< fb::Offset< Mega::Vertex3D > > vertices;
    std::vector< int >                          indices;

    for( auto e : edges )
    {
        const auto& sValue = fbVertMap.get( e->source() );
        const auto& tValue = fbVertMap.get( e->target() );
        const auto  sVert  = fbVertMap( e->source() );
        const auto  tVert  = fbVertMap( e->target() );

        const Mega::F2 sF2( sValue.x(), sValue.y() );
        const Mega::F2 tF2( tValue.x(), tValue.y() );

        const Mega::F2 tPerp( -tF2.y() - sF2.y(), tF2.x() - sF2.x() );

        const float mag = std::sqrt( tF2.x() * tF2.x() + sF2.y() * sF2.y() );

        const Mega::F3 normal( tPerp.x() / mag, tPerp.y() / mag, 0.0f );
        const Mega::F4 tangent( tPerp.x() / mag, tPerp.y() / mag, 0.0f, 0.0f );

        auto sLower = buildVertex( builder, vertices, Mega::F2( 0, 0 ), normal, tangent, sVert, lower );
        auto tLower = buildVertex( builder, vertices, Mega::F2( 0, 1 ), normal, tangent, tVert, lower );
        auto sUpper = buildVertex( builder, vertices, Mega::F2( 1, 0 ), normal, tangent, sVert, upper );
        auto tUpper = buildVertex( builder, vertices, Mega::F2( 1, 1 ), normal, tangent, tVert, upper );

        indices.push_back( sLower );
        indices.push_back( tLower );
        indices.push_back( sUpper );

        indices.push_back( sUpper );
        indices.push_back( tLower );
        indices.push_back( tUpper );
    }

    auto fbIndices = builder.CreateVector( indices );
    auto fbVerts   = builder.CreateVector( vertices );

    Mega::MeshBuilder meshBuilder( builder );

    meshBuilder.add_indices( fbIndices );
    meshBuilder.add_vertices( fbVerts );

    return meshBuilder.Finish();
}
fb::Offset< Mega::Mesh > buildVerticalMesh( const FBVertMap&                       fbVertMap,
                                            Mega::Plane                            lower,
                                            Mega::Plane                            upper,
                                            const exact::Analysis::HalfEdgeVector& edges,
                                            fb::FlatBufferBuilder&                 builder )
{
    std::vector< fb::Offset< Mega::Vertex3D > > vertices;
    std::vector< int >                          indices;

    float fUVDist = 0;
    for( auto e : edges )
    {
        const auto& sValue = fbVertMap.get( e->source() );
        const auto& tValue = fbVertMap.get( e->target() );
        const auto  sVert  = fbVertMap( e->source() );
        const auto  tVert  = fbVertMap( e->target() );

        const Mega::F2 sF2( sValue.x(), sValue.y() );
        const Mega::F2 tF2( tValue.x(), tValue.y() );

        const Mega::F2 tDist( tF2.x() - sF2.x(), tF2.y() - sF2.y() );
        const Mega::F2 tPerp( -tDist.y(), tDist.x() );

        const float mag = std::sqrt( tDist.x() * tDist.x() + tDist.y() * tDist.y() );

        const Mega::F3 normal( tPerp.x() / mag, tPerp.y() / mag, 0.0f );
        const Mega::F4 tangent( tPerp.x() / mag, tPerp.y() / mag, 0.0f, 0.0f );

        auto sLower = buildVertex( builder, vertices, Mega::F2( fUVDist, 0 ), normal, tangent, sVert, lower );
        auto sUpper = buildVertex( builder, vertices, Mega::F2( fUVDist, 1 ), normal, tangent, sVert, upper );

        fUVDist += mag;

        auto tLower = buildVertex( builder, vertices, Mega::F2( fUVDist, 0 ), normal, tangent, tVert, lower );
        auto tUpper = buildVertex( builder, vertices, Mega::F2( fUVDist, 1 ), normal, tangent, tVert, upper );

        indices.push_back( sLower );
        indices.push_back( tLower );
        indices.push_back( sUpper );

        indices.push_back( sUpper );
        indices.push_back( tLower );
        indices.push_back( tUpper );
    }

    auto fbIndices = builder.CreateVector( indices );
    auto fbVerts   = builder.CreateVector( vertices );

    Mega::MeshBuilder meshBuilder( builder );

    meshBuilder.add_indices( fbIndices );
    meshBuilder.add_vertices( fbVerts );

    return meshBuilder.Finish();
}

Mega::Plane convert( exact::Analysis::PartitionSegment::Plane plane )
{
    switch( plane )
    {
        case exact::Analysis::PartitionSegment::eHole:
            return Mega::Plane_eHole;
        case exact::Analysis::PartitionSegment::eGround:
            return Mega::Plane_eGround;
        case exact::Analysis::PartitionSegment::eMid:
            return Mega::Plane_eMid;
        case exact::Analysis::PartitionSegment::eCeiling:
            return Mega::Plane_eCeiling;
        default:
            THROW_RTE( "Unknone plane type" );
    }
}
} // namespace

void Schematic::compileMap( const boost::filesystem::path& filePath )
{
    using PolyWivOwls = exact::Analysis::HalfEdgePolygonWithHoles;

    std::ostringstream osError;
    if( !compile( schematic::TOTAL_COMPILAION_STAGES, osError ) )
    {
        throw std::runtime_error( osError.str() );
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
        fb::Offset< Mega::Mesh > fbMesh = buildHorizontalMesh( fbVertMap, Mega::Plane_eGround, floor.floor, builder );

        auto fbFloorPolygon = buildPolygon( fbVertMap, floor.floor.outer, builder );

        std::vector< fb::Offset< Mega::Mesh > > ex1Polys;
        for( auto& ex : floor.ex1 )
        {
            ex1Polys.push_back( buildHorizontalMesh( fbVertMap, Mega::Plane_eGround, ex, builder ) );
        }
        std::vector< fb::Offset< Mega::Mesh > > ex2Polys;
        for( auto& ex : floor.ex2 )
        {
            ex2Polys.push_back( buildHorizontalMesh( fbVertMap, Mega::Plane_eGround, ex, builder ) );
        }
        std::vector< fb::Offset< Mega::Mesh > > ex3Polys;
        for( auto& ex : floor.ex3 )
        {
            ex3Polys.push_back( buildHorizontalMesh( fbVertMap, Mega::Plane_eGround, ex, builder ) );
        }
        std::vector< fb::Offset< Mega::Mesh > > ex4Polys;
        for( auto& ex : floor.ex4 )
        {
            ex4Polys.push_back( buildHorizontalMesh( fbVertMap, Mega::Plane_eGround, ex, builder ) );
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

    // boundaries

    std::vector< fb::Offset< Mega::Boundary > > fbBoundaryVec;
    {
        for( const auto& boundary : m_pAnalysis->getBoundaries() )
        {
            std::vector< fb::Offset< Mega::Mesh > > fbhori_hole;
            for( const auto& hori_hole : boundary.hori_holes )
            {
                fbhori_hole.push_back(
                    buildHorizontalMesh( fbVertMap, Mega::Plane_eHole, PolyWivOwls{ hori_hole }, builder ) );
            }
            std::vector< fb::Offset< Mega::Mesh > > fbhori_floor;
            for( const auto& hori_floor : boundary.hori_floors )
            {
                fbhori_floor.push_back(
                    buildHorizontalMesh( fbVertMap, Mega::Plane_eGround, PolyWivOwls{ hori_floor }, builder ) );
            }
            std::vector< fb::Offset< Mega::Mesh > > fbhori_mid;
            for( const auto& hori_mid : boundary.hori_mids )
            {
                fbhori_mid.push_back(
                    buildHorizontalMesh( fbVertMap, Mega::Plane_eMid, PolyWivOwls{ hori_mid }, builder ) );
            }
            std::vector< fb::Offset< Mega::Mesh > > fbhori_ceiling;
            for( const auto& hori_ceiling : boundary.hori_ceilings )
            {
                fbhori_ceiling.push_back(
                    buildHorizontalMesh( fbVertMap, Mega::Plane_eCeiling, PolyWivOwls{ hori_ceiling }, builder ) );
            }

            auto fbhori_holeVec    = builder.CreateVector( fbhori_hole );
            auto fbhori_floorVec   = builder.CreateVector( fbhori_floor );
            auto fbhori_midVec     = builder.CreateVector( fbhori_mid );
            auto fbhori_ceilingVec = builder.CreateVector( fbhori_ceiling );

            std::vector< fb::Offset< Mega::Pane > > fbPaneVec;
            for( const auto& pane : boundary.panes )
            {
                auto pMesh
                    = buildVerticalMesh( fbVertMap, convert( pane.lower ), convert( pane.upper ), pane.edges, builder );
                Mega::PaneBuilder paneBuilder( builder );
                paneBuilder.add_quad( pMesh );
                fbPaneVec.push_back( paneBuilder.Finish() );
            }

            auto fbPanes = builder.CreateVector( fbPaneVec );

            std::vector< fb::Offset< Mega::WallSection > > fbWalls;
            for( const Analysis::Boundary::WallSection& wall : boundary.walls )
            {
                auto pMesh
                    = buildVerticalMesh( fbVertMap, convert( wall.lower ), convert( wall.upper ), wall.edges, builder );
                Mega::WallSection::Builder wallBuilder( builder );
                wallBuilder.add_mesh( pMesh );
                fbWalls.push_back( wallBuilder.Finish() );
            }

            auto fbWallVec = builder.CreateVector( fbWalls );

            Mega::Boundary::Builder boundaryBuilder( builder );

            boundaryBuilder.add_hori_holes( fbhori_holeVec );
            boundaryBuilder.add_hori_floors( fbhori_floorVec );
            boundaryBuilder.add_hori_mids( fbhori_midVec );
            boundaryBuilder.add_hori_ceilings( fbhori_ceilingVec );

            boundaryBuilder.add_vert_panes( fbPanes );

            boundaryBuilder.add_walls( fbWallVec );

            auto pBoundary = boundaryBuilder.Finish();
            fbBoundaryVec.push_back( pBoundary );
        }
    }

    auto fbBoundaries = builder.CreateVector( fbBoundaryVec );

    // map
    fb::Offset< Mega::Map > fbMap;
    {
        // auto pPartitionsVector = builder.CreateVector( mapPartitions );

        Mega::MapBuilder mapBuilder( builder );
        mapBuilder.add_contour( fbMapPolygon );
        mapBuilder.add_root_area( fbRootArea );
        mapBuilder.add_floors( fbFloors );
        mapBuilder.add_boundaries( fbBoundaries );
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
