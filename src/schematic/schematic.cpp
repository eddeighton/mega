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
    if( !m_pLaneAxisMarkup.get() )
    {
        m_pLaneAxisMarkup.reset( new MonoBitmapImage( *this, nullptr, m_laneBitmap, eStage_Lanes ) );
    }

    /*if( !( m_pLaneRadius = get< Property >( "laneRadius" ) ) )
    {
        m_pLaneRadius = Property::Ptr( new Property( getPtr(), "laneRadius" ) );
        m_pLaneRadius->init();
        m_pLaneRadius->setStatement( "0.75f" );
        add( m_pLaneRadius );
    }
    if( !( m_pLaneLining = get< Property >( "laneLining" ) ) )
    {
        m_pLaneLining = Property::Ptr( new Property( getPtr(), "laneLining" ) );
        m_pLaneLining->init();
        m_pLaneLining->setStatement( "0.15f" );
        add( m_pLaneLining );
    }
    if( !( m_pPavementRadius = get< Property >( "pavementRadius" ) ) )
    {
        m_pPavementRadius = Property::Ptr( new Property( getPtr(), "pavementRadius" ) );
        m_pPavementRadius->init();
        m_pPavementRadius->setStatement( "2.25f" );
        add( m_pPavementRadius );
    }
    if( !( m_pPavementLining = get< Property >( "pavementLining" ) ) )
    {
        m_pPavementLining = Property::Ptr( new Property( getPtr(), "pavementLining" ) );
        m_pPavementLining->init();
        m_pPavementLining->setStatement( "0.15f" );
        add( m_pPavementLining );
    }
    if( !( m_pClearance = get< Property >( "clearance" ) ) )
    {
        m_pClearance = Property::Ptr( new Property( getPtr(), "clearance" ) );
        m_pClearance->init();
        m_pClearance->setStatement( "3.5f" );
        add( m_pClearance );
    }*/
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

Schematic::LaneConfig Schematic::getLaneConfig()
{
    LaneConfig laneConfig;

    /*if( m_pLaneRadius )
    {
        laneConfig.laneRadius = m_pLaneRadius->getValue( laneConfig.laneRadius, m_laneRadiusOpt );
    }
    if( m_pLaneLining )
    {
        laneConfig.laneLining = m_pLaneLining->getValue( laneConfig.laneLining, m_laneLiningOpt );
    }
    if( m_pPavementRadius )
    {
        laneConfig.pavementRadius = m_pPavementRadius->getValue( laneConfig.pavementRadius, m_pavementRadiusOpt );
    }
    if( m_pPavementLining )
    {
        laneConfig.pavementLining = m_pPavementLining->getValue( laneConfig.pavementLining, m_pavementLiningOpt );
    }
    if( m_pClearance )
    {
        laneConfig.clearance = m_pClearance->getValue( laneConfig.clearance, m_clearanceOpt );
    }*/

    return laneConfig;
}

bool Schematic::compile( CompilationStage stage, std::ostream& os )
{
    const int iStage = static_cast< int >( stage );

    Schematic::Ptr pThis = boost::dynamic_pointer_cast< Schematic >( getPtr() );

    m_pAnalysis.reset();
    m_pAnalysisMarkup->reset();
    m_pPropertiesMarkup->reset();

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
            m_pAnalysis.reset( new exact::Analysis( pThis ) );
            m_pAnalysis->contours();
            m_pAnalysis->ports();
        }

        if( iStage >= eStage_Partition )
        {
            m_pAnalysis->partition();
        }

        if( iStage >= eStage_Properties )
        {
            m_pAnalysis->properties();
        }

        if( iStage >= eStage_Placement )
        {
            m_pAnalysis->placement();
        }

        if( iStage >= eStage_Lanes )
        {
            m_pAnalysis->lanes();
        }

        if( iStage >= eStage_Linings )
        {
            m_pAnalysis->linings();
        }

        if( iStage >= eStage_Values )
        {
            m_pAnalysis->values();
        }

        if( iStage >= eStage_Visibility )
        {
            m_pAnalysis->visibility();
        }
    }
    catch( std::exception& ex )
    {
        os << ex.what();

        return false;
    }
    catch( ... )
    {
        os << "Unknown exception compiling schematic";
        return false;
    }

    if( m_pAnalysis )
    {
        std::vector< MultiPathMarkup::SegmentMask > edges;
        m_pAnalysis->getAllEdges( edges );
        m_pAnalysisMarkup->set( edges );

        /*if( iStage >= eStage_Properties )
        {
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
        else*/
        {
            m_pPropertiesMarkup->reset();
        }

        if( iStage >= eStage_Lanes )
        {
            // m_pLaneAxisMarkup
            m_laneBitmap.setModified();
        }
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

class PositionConverter
{
    static exact::ExactToInexact m_convert;

public:
    template < typename ExactPointType >
    inline Mega::F2 operator()( const ExactPointType& pt )
    {
        const schematic::Point inexactPoint = m_convert( pt );
        return Mega::F2( inexactPoint.x(), inexactPoint.y() );
    }
};
exact::ExactToInexact PositionConverter::m_convert;

fb::Offset< Mega::Polygon > buildPolygon( const exact::Analysis::HalfEdgeCstVector& contour,
                                          fb::FlatBufferBuilder&                    builder )
{
    std::vector< Mega::F2 > vertices;
    {
        for( auto pEdge : contour )
        {
            vertices.push_back( PositionConverter()( pEdge->source()->point() ) );
        }
    }
    std::vector< const Mega::F2* > vertPtrs;
    for( const auto& f2 : vertices )
    {
        vertPtrs.push_back( &f2 );
    }
    auto pVertices = builder.CreateVector( vertPtrs );

    Mega::PolygonBuilder polyBuilder( builder );
    polyBuilder.add_vertices( pVertices );
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
        exact::Analysis::VertexCst vertex = exact::Analysis::VertexCst{};
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
                         const Mega::F2&                              position,
                         const Mega::F2&                              uv,
                         const Mega::F3&                              normal,
                         Mega::Plane                                  plane )
{
    Mega::Vertex3DBuilder vertBuilder( builder );
    vertBuilder.add_position( &position );
    vertBuilder.add_normal( &normal );
    vertBuilder.add_plane( plane );
    vertBuilder.add_uv( &uv );
    auto fbVert3d = vertBuilder.Finish();

    std::size_t szIndex = vertices.size();
    vertices.push_back( fbVert3d );

    return szIndex;
}

fb::Offset< Mega::Mesh > buildHorizontalMesh( Mega::Plane                                         plane,
                                              const exact::Analysis::HalfEdgeCstPolygonWithHoles& poly,
                                              fb::FlatBufferBuilder&                              builder,
                                              bool bAddGridVertices = true )
{
    std::optional< Rect > boundsOpt;
    Triangulation         triangulation;
    {
        auto insertConstraints = [ & ]( const exact::Analysis::VertexCstVector& contour )
        {
            std::vector< Triangulation::CDT::Vertex_handle > verts;
            // create the vertices - recording the original vertex handle from the arrangement
            for( auto vertex : contour )
            {
                Triangulation::CDT::Vertex_handle v = triangulation.cdt.insert( vertex->point() );
                v->info().vertex                    = vertex;
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
        {
            exact::Analysis::VertexCstVector outerVertices
                = exact::filterColinearEdges< exact::Analysis::VertexCst >( poly.outer );
            if( bAddGridVertices )
            {
                // calculate the bounding box
                std::vector< exact::Point > outerPoints;
                for( auto v : outerVertices )
                {
                    outerPoints.push_back( v->point() );
                }
                boundsOpt = CGAL::bbox_2( outerPoints.begin(), outerPoints.end() );
            }
            insertConstraints( outerVertices );
        }
        for( auto& h : poly.holes )
        {
            insertConstraints( exact::filterColinearEdges< exact::Analysis::VertexCst >( h ) );
        }
    }

   /* if( bAddGridVertices )
    {
        INVARIANT( boundsOpt.has_value(), "no bounding box" );
        // add grid vertices
        const int xMin = static_cast< int >( std::floor( CGAL::to_double( boundsOpt.value().xmin() ) ) );
        const int xMax = static_cast< int >( std::ceil( CGAL::to_double( boundsOpt.value().xmax() ) ) );
        const int yMin = static_cast< int >( std::floor( CGAL::to_double( boundsOpt.value().ymin() ) ) );
        const int yMax = static_cast< int >( std::ceil( CGAL::to_double( boundsOpt.value().ymax() ) ) );
        for( int x = xMin; x <= xMax; x++ )
        {
            for( int y = yMin; y <= yMax; y++ )
            {
                triangulation.cdt.insert( exact::Point( x, y ) );
            }
        }
    }*/

    using ArrVert = exact::Analysis::VertexCst;

    const Mega::F3 normal( 0.0f, 1.0f, 0.0f );

    std::vector< fb::Offset< Mega::Vertex3D > > vertices;
    std::vector< int >                          indices;
    for( Triangulation::CDT::Face_handle f : triangulation.solve() )
    {
        if( f->info().in_domain() )
        {
            for( int i = 0; i != 3; ++i )
            {
                const auto     vertex  = f->vertex( i );
                const Mega::F2 f2      = PositionConverter()( vertex->point() );
                std::size_t    szIndex = buildVertex( builder, vertices, f2, f2, normal, plane );
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

using VertexDistanceMap = std::map< exact::Analysis::VertexCst, float >;

void triangulateLiningRegion( Mega::Plane plane, const exact::Analysis::VertexCstVector& region,
                              const VertexDistanceMap& distances, std::vector< fb::Offset< Mega::Vertex3D > >& vertices,
                              std::vector< int >& indices, fb::FlatBufferBuilder& builder )
{
    std::map< exact::Analysis::VertexCst, std::size_t > indexMap;
    {
        const Mega::F3 normal( 0.0f, 1.0f, 0.0f );

        // First two vertices of the region are the boundary segment vertices in counter clockwise order
        // so can calculate coordinate axis along this line for uv
        INVARIANT( region.size() > 1, "Invalid region" );

        float fP1Distance;
        {
            auto iFind1 = distances.find( region[ 0 ] );
            INVARIANT( iFind1 != distances.end(), "Failed to locate vertex 0" );
            fP1Distance = iFind1->second;
        }

        const exact::Point                         regionStartPoint = region[ 0 ]->point();
        const exact::Point                         regionEndPoint   = region[ 1 ]->point();
        const exact::Segment                       segment( regionStartPoint, regionEndPoint );
        exact::Kernel::Construct_projected_point_2 project;

        // add the vertices around the region
        for( auto vertex : region )
        {
            const exact::Point vertexPoint        = vertex->point();
            const exact::Point nearestPointOnLine = project( segment.supporting_line(), vertexPoint );

            double uvXDist = fP1Distance;
            {
                const auto xDiffToStart     = nearestPointOnLine - regionStartPoint;
                const auto xqDistToStart    = xDiffToStart.squared_length();
                const auto xqDistToEnd      = ( nearestPointOnLine - regionEndPoint ).squared_length();
                const auto xqDistStartToEnd = ( regionEndPoint - regionStartPoint ).squared_length();

                const double xDistanceToStart = CGAL::approximate_sqrt( CGAL::to_double( xqDistToStart ) );

                if( xqDistStartToEnd < ( xqDistToStart + xqDistToEnd ) )
                {
                    // NOT within line segment
                    if( xqDistToStart < xqDistToEnd )
                    {
                        uvXDist -= xDistanceToStart;
                    }
                    else
                    {
                        uvXDist += xDistanceToStart;
                    }
                }
                else
                {
                    // must be within line segment
                    uvXDist += xDistanceToStart;
                }
            }

            const auto yDiff   = vertexPoint - nearestPointOnLine;
            const auto uvYDist = CGAL::approximate_sqrt( CGAL::to_double( yDiff.squared_length() ) );

            // NEED associated distance with each contour vertex with which to add dist
            std::size_t szIndex = buildVertex( builder, vertices, PositionConverter()( vertex->point() ),
                                               Mega::F2( uvXDist, uvYDist ), normal, plane );

            indexMap.insert( { vertex, szIndex } );
        }
    }

    Triangulation triangulation;
    {
        std::vector< Triangulation::CDT::Vertex_handle > verts;
        // create the vertices - recording the original vertex handle from the arrangement
        for( auto vertex : region )
        {
            Triangulation::CDT::Vertex_handle v = triangulation.cdt.insert( vertex->point() );
            v->info().vertex                    = vertex;
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
    }

    // add the faces
    for( Triangulation::CDT::Face_handle f : triangulation.solve() )
    {
        if( f->info().in_domain() )
        {
            for( int i = 0; i != 3; ++i )
            {
                const auto vertex = f->vertex( i )->info().vertex;
                auto       iFind  = indexMap.find( vertex );
                INVARIANT( iFind != indexMap.end(), "Invalid vertex" );
                indices.push_back( iFind->second );
            }
        }
    }
}

fb::Offset< Mega::Mesh > buildLiningHorizontalMesh( Mega::Plane                                         plane,
                                                    const exact::Analysis::HalfEdgeCstPolygonWithHoles& poly,
                                                    const exact::Analysis::SkeletonRegionQuery& skeletonEdgeQuery,
                                                    fb::FlatBufferBuilder&                      builder )
{
    VertexDistanceMap vertexDistances;
    auto              collectContourSegmentRegion
        = [ &skeletonEdgeQuery, &vertexDistances ]( const exact::Analysis::HalfEdgeCstVector& contour, bool ccw )
    {
        exact::Analysis::VertexCstVectorVector regions;
        auto filtered = exact::filterColinearEdges< exact::Analysis::VertexCst >( contour );
        {
            exact::Polygon outer;
            for( auto& e : filtered )
            {
                outer.push_back( e->point() );
            }
            // ensure it is a not a hole
            if( ccw != outer.is_counterclockwise_oriented() )
            {
                std::reverse( filtered.begin(), filtered.end() );
            }
        }
        float fDistance = 0.0f;
        for( auto i = filtered.begin(), iNext = filtered.begin(), iEnd = filtered.end(); i != iEnd; ++i )
        {
            ++iNext;
            if( iNext == iEnd )
            {
                iNext = filtered.begin();
            }
            exact::Analysis::HalfEdgeCst iNextEdge;
            for( auto e : contour )
            {
                if( e->source() == *iNext )
                {
                    iNextEdge = e;
                    break;
                }
            }
            INVARIANT( iNextEdge != exact::Analysis::HalfEdgeCst{}, "Failed to locate vertex edge" );
            regions.push_back( skeletonEdgeQuery.getRegion( *i, iNextEdge ) );

            vertexDistances.insert( { *i, fDistance } );
            const auto vDiff = ( *iNext )->point() - ( *i )->point();
            fDistance += CGAL::approximate_sqrt( CGAL::to_double( vDiff.squared_length() ) );
        }
        return regions;
    };

    std::vector< fb::Offset< Mega::Vertex3D > > vertices;
    std::vector< int >                          indices;

    std::vector< fb::Offset< Mega::Mesh > > resultMeshes;
    {
        exact::Analysis::VertexCstVectorVector outerRegions = collectContourSegmentRegion( poly.outer, true );
        for( const auto& region : outerRegions )
        {
            triangulateLiningRegion( plane, region, vertexDistances, vertices, indices, builder );
        }
    }
    for( const auto& hole : poly.holes )
    {
        exact::Analysis::VertexCstVectorVector holeRegions = collectContourSegmentRegion( hole, false );
        for( const auto& region : holeRegions )
        {
            triangulateLiningRegion( plane, region, vertexDistances, vertices, indices, builder );
        }
    }

    auto fbIndices = builder.CreateVector( indices );
    auto fbVerts   = builder.CreateVector( vertices );

    Mega::MeshBuilder meshBuilder( builder );

    meshBuilder.add_indices( fbIndices );
    meshBuilder.add_vertices( fbVerts );

    return meshBuilder.Finish();
}

float convertVerticalUV( Mega::Plane plane )
{
    switch( plane )
    {
        case Mega::Plane_eHole:
            return -1.0f;
        case Mega::Plane_eGround:
            return 0.0f;
        case Mega::Plane_eMid:
            return 1.0f;
        case Mega::Plane_eCeiling:
            return 2.0f;
        default:
            THROW_RTE( "Unknown plane" );
    }
}

// Generate a seperate quad for each edge
fb::Offset< Mega::Mesh > buildVerticalMesh( Mega::Plane                            lower,
                                            Mega::Plane                            upper,
                                            const exact::Analysis::HalfEdgeCstSet& edges,
                                            fb::FlatBufferBuilder&                 builder )
{
    std::vector< fb::Offset< Mega::Vertex3D > > vertices;
    std::vector< int >                          indices;

    const float fLowerUV = convertVerticalUV( lower );
    const float fUpperUV = convertVerticalUV( upper );

    for( auto e : edges )
    {
        const Mega::F2 sF2 = PositionConverter()( e->source()->point() );
        const Mega::F2 tF2 = PositionConverter()( e->target()->point() );

        const Mega::F2 tDist( tF2.x() - sF2.x(), tF2.y() - sF2.y() );
        const Mega::F2 tPerp( -tDist.y(), tDist.x() );
        const float    mag = std::sqrt( tDist.x() * tDist.x() + tDist.y() * tDist.y() );

        const Mega::F3 normal( tPerp.x() / mag, tPerp.y() / mag, 0.0f );

        auto sLower = buildVertex( builder, vertices, sF2, Mega::F2( 0, fLowerUV ), normal, lower );
        auto tLower = buildVertex( builder, vertices, tF2, Mega::F2( 0, fUpperUV ), normal, lower );
        auto sUpper = buildVertex( builder, vertices, sF2, Mega::F2( mag, fLowerUV ), normal, upper );
        auto tUpper = buildVertex( builder, vertices, tF2, Mega::F2( mag, fUpperUV ), normal, upper );

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

fb::Offset< Mega::Mesh > buildVerticalMesh( Mega::Plane                               lower,
                                            Mega::Plane                               upper,
                                            const exact::Analysis::HalfEdgeCstVector& edges,
                                            fb::FlatBufferBuilder&                    builder,
                                            bool                                      bReverse )
{
    std::vector< fb::Offset< Mega::Vertex3D > > vertices;
    std::vector< int >                          indices;

    const float fLowerUV = convertVerticalUV( lower );
    const float fUpperUV = convertVerticalUV( upper );

    const exact::Analysis::VertexCstVector edgeVerts
        = exact::filterColinearEdges< exact::Analysis::VertexCst >( bReverse ? reverse( edges ) : edges );
    const bool bIsLoop = edges.back()->target() == edges.front()->source();

    float fUVDist = 0;
    for( auto i = edgeVerts.begin(), iPrev = edgeVerts.end() - 1, iNext = edgeVerts.begin(), iEnd = edgeVerts.end();
         i != iEnd; ++i )
    {
        ++iNext;
        if( iNext == edgeVerts.end() )
        {
            iNext = edgeVerts.begin();
            if( !bIsLoop )
                break;
        }

        const bool bFirst = i == edgeVerts.begin();
        const bool bLast  = i == edgeVerts.end() - 1;

        /*if( !bIsLoop && bFirst )
        {
        }
        else if( !bIsLoop && bLast )
        {
        }
        else*/
        {
            auto v     = *i;
            auto vNext = *iNext;

            const exact::Direction halfEdgeDir( vNext->point() - v->point() );
            const exact::Direction halfEdgeOrthoDir = halfEdgeDir.perpendicular( CGAL::Orientation::COUNTERCLOCKWISE );
            const exact::Vector    vNorm            = halfEdgeOrthoDir.vector();

            const Mega::F2 sF2 = PositionConverter()( v->point() );
            const Mega::F2 tF2 = PositionConverter()( vNext->point() );

            const Mega::F2 tDist( tF2.x() - sF2.x(), tF2.y() - sF2.y() );
            const float    mag = std::sqrt( tDist.x() * tDist.x() + tDist.y() * tDist.y() );

            const Mega::F3 normal( CGAL::to_double( vNorm.x() ), CGAL::to_double( vNorm.y() ), 0.0 );

            auto sLower = buildVertex( builder, vertices, sF2, Mega::F2( fUVDist, fLowerUV ), normal, lower );
            auto sUpper = buildVertex( builder, vertices, sF2, Mega::F2( fUVDist, fUpperUV ), normal, upper );

            fUVDist -= mag;

            auto tLower = buildVertex( builder, vertices, tF2, Mega::F2( fUVDist, fLowerUV ), normal, lower );
            auto tUpper = buildVertex( builder, vertices, tF2, Mega::F2( fUVDist, fUpperUV ), normal, upper );

            indices.push_back( sLower );
            indices.push_back( tLower );
            indices.push_back( sUpper );

            indices.push_back( sUpper );
            indices.push_back( tLower );
            indices.push_back( tUpper );
        }

        ++iPrev;
        if( iPrev == edgeVerts.end() )
        {
            iPrev = edgeVerts.begin();
        }
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
    using PolyWivOwls = exact::Analysis::HalfEdgeCstPolygonWithHoles;

    std::ostringstream osError;
    if( !compile( schematic::TOTAL_COMPILAION_STAGES, osError ) )
    {
        throw std::runtime_error( osError.str() );
    }
    VERIFY_RTE_MSG( m_pAnalysis, "Schematic analysis failed" );

    fb::FlatBufferBuilder builder;

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

    const Analysis::SkeletonRegionQuery skeletonEdgeQuery( *m_pAnalysis );

    fb::Offset< Mega::Polygon > fbMapPolygon;
    {
        Analysis::HalfEdgeCstVector perimeter;
        m_pAnalysis->getPerimeterPolygon( perimeter );
        fbMapPolygon = buildPolygon( perimeter, builder );
    }

    std::vector< fb::Offset< Mega::Room > > fbRoomsVec;
    {
        Analysis::Room::Vector rooms = m_pAnalysis->getRooms();
        for( const auto& room : rooms )
        {
            std::vector< fb::Offset< Mega::Mesh > > roadPolys;
            for( const auto& polyWithHoles : room.roads )
            {
                fb::Offset< Mega::Mesh > fbMesh = buildHorizontalMesh( Mega::Plane_eGround, polyWithHoles, builder );
                roadPolys.push_back( fbMesh );
            }
            std::vector< fb::Offset< Mega::Mesh > > pavementPolys;
            for( const auto& polyWithHoles : room.pavements )
            {
                fb::Offset< Mega::Mesh > fbMesh = buildHorizontalMesh( Mega::Plane_eGround, polyWithHoles, builder );
                pavementPolys.push_back( fbMesh );
            }
            std::vector< fb::Offset< Mega::Mesh > > pavementLiningPolys;
            for( const auto& polyWithHoles : room.pavementLinings )
            {
                pavementLiningPolys.push_back(
                    buildLiningHorizontalMesh( Mega::Plane_eGround, polyWithHoles, skeletonEdgeQuery, builder ) );
            }
            std::vector< fb::Offset< Mega::Mesh > > laneLiningPolys;
            for( const auto& polyWithHoles : room.laneLinings )
            {
                laneLiningPolys.push_back(
                    buildLiningHorizontalMesh( Mega::Plane_eGround, polyWithHoles, skeletonEdgeQuery, builder ) );
            }

            std::vector< fb::Offset< Mega::Mesh > > laneFloorPolys;
            std::vector< fb::Offset< Mega::Mesh > > laneCoverPolys;
            std::vector< fb::Offset< Mega::Mesh > > laneWallsPolys;
            for( const auto& polyWithHoles : room.lanes )
            {
                laneFloorPolys.push_back(
                    buildLiningHorizontalMesh( Mega::Plane_eHole, polyWithHoles, skeletonEdgeQuery, builder ) );
                laneCoverPolys.push_back(
                    buildLiningHorizontalMesh( Mega::Plane_eGround, polyWithHoles, skeletonEdgeQuery, builder ) );

                fb::Offset< Mega::Mesh > fbWallOuter
                    = buildVerticalMesh( Mega::Plane_eHole, Mega::Plane_eGround, polyWithHoles.outer, builder, true );
                laneWallsPolys.push_back( fbWallOuter );

                for( const auto& hole : polyWithHoles.holes )
                {
                    fb::Offset< Mega::Mesh > fbWallHole
                        = buildVerticalMesh( Mega::Plane_eHole, Mega::Plane_eGround, hole, builder, true );
                    laneWallsPolys.push_back( fbWallHole );
                }
            }

            auto fbRoadPolys           = builder.CreateVector( roadPolys );
            auto fbPavementPolys       = builder.CreateVector( pavementPolys );
            auto fbPavementLiningPolys = builder.CreateVector( pavementLiningPolys );
            auto fbLaneLiningPolys     = builder.CreateVector( laneLiningPolys );

            auto fbLaneFloorPolys = builder.CreateVector( laneFloorPolys );
            auto fbLaneCoverPolys = builder.CreateVector( laneCoverPolys );
            auto fbLaneWallsPolys = builder.CreateVector( laneWallsPolys );

            Mega::RoomBuilder roomBuilder( builder );

            // floorBuilder.add_contour( fbRoomPolygon );

            roomBuilder.add_roads( fbRoadPolys );
            roomBuilder.add_pavements( fbPavementPolys );
            roomBuilder.add_pavement_linings( fbPavementLiningPolys );
            roomBuilder.add_lane_linings( fbLaneLiningPolys );

            roomBuilder.add_lane_floors( fbLaneFloorPolys );
            roomBuilder.add_lane_covers( fbLaneCoverPolys );
            roomBuilder.add_lane_walls( fbLaneWallsPolys );

            fbRoomsVec.push_back( roomBuilder.Finish() );
        }
    }

    auto fbRooms = builder.CreateVector( fbRoomsVec );

    // boundaries

    std::vector< fb::Offset< Mega::Boundary > > fbBoundaryVec;
    {
        for( const auto& boundary : m_pAnalysis->getBoundaries() )
        {
            std::vector< fb::Offset< Mega::Mesh > > fbhori_hole;
            for( const auto& hori_hole : boundary.hori_holes )
            {
                fbhori_hole.push_back( buildHorizontalMesh( Mega::Plane_eHole, PolyWivOwls{ hori_hole }, builder ) );
            }
            std::vector< fb::Offset< Mega::Mesh > > fbhori_floor;
            for( const auto& hori_floor : boundary.hori_floors )
            {
                fbhori_floor.push_back(
                    buildHorizontalMesh( Mega::Plane_eGround, PolyWivOwls{ hori_floor }, builder ) );
            }
            std::vector< fb::Offset< Mega::Mesh > > fbhori_mid;
            for( const auto& hori_mid : boundary.hori_mids )
            {
                fbhori_mid.push_back( buildHorizontalMesh( Mega::Plane_eMid, PolyWivOwls{ hori_mid }, builder ) );
            }
            std::vector< fb::Offset< Mega::Mesh > > fbhori_ceiling;
            for( const auto& hori_ceiling : boundary.hori_ceilings )
            {
                fbhori_ceiling.push_back(
                    buildHorizontalMesh( Mega::Plane_eCeiling, PolyWivOwls{ hori_ceiling }, builder ) );
            }

            auto fbhori_holeVec    = builder.CreateVector( fbhori_hole );
            auto fbhori_floorVec   = builder.CreateVector( fbhori_floor );
            auto fbhori_midVec     = builder.CreateVector( fbhori_mid );
            auto fbhori_ceilingVec = builder.CreateVector( fbhori_ceiling );

            std::vector< fb::Offset< Mega::Pane > > fbPaneVec;
            for( const auto& pane : boundary.panes )
            {
                auto pMesh = buildVerticalMesh( convert( pane.lower ), convert( pane.upper ), pane.edges, builder );
                Mega::PaneBuilder paneBuilder( builder );
                paneBuilder.add_quad( pMesh );
                fbPaneVec.push_back( paneBuilder.Finish() );
            }

            auto fbPanes = builder.CreateVector( fbPaneVec );

            std::vector< fb::Offset< Mega::WallSection > > fbWalls;
            for( const Analysis::Boundary::WallSection& wall : boundary.walls )
            {
                auto pMesh
                    = buildVerticalMesh( convert( wall.lower ), convert( wall.upper ), wall.edges, builder, false );
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
        mapBuilder.add_rooms( fbRooms );
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
