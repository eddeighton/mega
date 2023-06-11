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
        m_pAnalysis.reset();
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

void recurseSites( flatbuffers::FlatBufferBuilder& builder, Site::Ptr pSite )
{
    // Mega::AreaBuilder::
    {
        Mega::AreaBuilder areaBuilder( builder );
        Mega::Type        type( 1 );
        areaBuilder.add_type( &type );
        flatbuffers::Offset< Mega::Area > pAreaPtr = areaBuilder.Finish();
    }

    for( Site::Ptr pChildSite : pSite->getSites() )
    {
        recurseSites( builder, pChildSite );
    }
}

void Schematic::compileMap( const boost::filesystem::path& filePath )
{
    exact::ExactToInexact convert;

    if( !m_pAnalysis )
    {
        task_contours();
        task_extrusions();
        std::ostringstream osError;
        if( !task_compilation( osError ) )
        {
            throw std::runtime_error( osError.str() );
        }
    }
    VERIFY_RTE_MSG( m_pAnalysis, "Schematic analysis failed" );

    flatbuffers::FlatBufferBuilder builder;

    // write the site tree
    /*{
        const auto sites = getSites();
        VERIFY_RTE_MSG( sites.size() < 2, "More than one root site in schematic" );
        if( !sites.empty() )
        {
            recurseSites( builder, sites.front() );
        }
    }*/

    using AnalysisVert = exact::Analysis::Arrangement::Vertex_const_handle;
    using MapVert      = flatbuffers::Offset< Mega::Vertex >;
    using VertexMap    = std::map< AnalysisVert, MapVert >;
    VertexMap vertexMap;

    struct Partition
    {
        flatbuffers::Offset< Mega::Polygon > pPolygon;
    };
    std::vector< Partition > partitions;

    auto buildPolygon = [ & ]( const exact::Analysis::HalfEdgeVector& contour ) -> flatbuffers::Offset< Mega::Polygon >
    {
        std::vector< MapVert > vertices;
        {
            for( auto pEdge : contour )
            {
                auto iFind = vertexMap.find( pEdge->source() );
                VERIFY_RTE_MSG( iFind != vertexMap.end(), "Failed to find vertex" );
                vertices.push_back( iFind->second );
            }
        }
        auto pVerts = builder.CreateVector( vertices );

        Mega::PolygonBuilder polyBuilder( builder );
        polyBuilder.add_vertices( pVerts );
        return polyBuilder.Finish();
    };

    using namespace exact;

    // record all vertices
    {
        exact::Analysis::VertexVector vertices;
        m_pAnalysis->getVertices( vertices );

        for( auto pVert : vertices )
        {
            const schematic::Point pt = convert( pVert->point() );
            const Mega::F2         f2( pt.x(), pt.y() );

            Mega::VertexBuilder vertBuilder( builder );
            vertBuilder.add_position( &f2 );
            flatbuffers::Offset< Mega::Vertex > pVertex = vertBuilder.Finish();
            vertexMap.insert( { pVert, pVertex } );
        }
    }

    flatbuffers::Offset< Mega::Polygon > mapPolygon;
    // polygons
    {
        exact::Analysis::HalfEdgeVector perimeter;
        m_pAnalysis->getPerimeterPolygon( perimeter );
        mapPolygon = buildPolygon( perimeter );
    }
    {
        exact::Analysis::HalfEdgeVectorVector boundaries;
        m_pAnalysis->getBoundaryPolygons( boundaries );

        for( const auto& boundary : boundaries )
        {
            flatbuffers::Offset< Mega::Polygon > pPolygon  = buildPolygon( boundary );
            Partition                            partition = { pPolygon };
            partitions.push_back( partition );
        }
    }

    // partitions
    std::vector< flatbuffers::Offset< Mega::Partition > > mapPartitions;
    {
        for( const auto& partition : partitions )
        {
            Mega::PartitionBuilder partitionBuilder( builder );
            partitionBuilder.add_contour( partition.pPolygon );
            auto pPartition = partitionBuilder.Finish();
            mapPartitions.push_back( pPartition );
        }
    }

    // map
    flatbuffers::Offset< Mega::Map > pMap;
    {
        auto pPartitionsVector = builder.CreateVector( mapPartitions );

        Mega::MapBuilder mapBuilder( builder );
        mapBuilder.add_contour( mapPolygon );
        mapBuilder.add_partitions( pPartitionsVector );
        pMap = mapBuilder.Finish();
    }

    builder.Finish( pMap );

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
