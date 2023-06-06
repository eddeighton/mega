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
void Schematic::task_compilation()
{
    std::vector< MultiPathMarkup::SegmentMask > edges;
    try
    {
        Schematic::Ptr pThis = boost::dynamic_pointer_cast< Schematic >( getPtr() );

        m_pAnalysis.reset();
        m_pAnalysis.reset( new exact::Analysis( pThis ) );
        m_pAnalysis->getEdges( edges );
    }
    catch( std::exception& )
    {
        m_pAnalysis.reset();
        edges.clear();
    }

    m_pAnalysisMarkup->set( edges );
}

void recurseSites( flatbuffers::FlatBufferBuilder& builder, Site::Ptr pSite )
{
    // Mega::AreaBuilder::
    {
        Mega::AreaBuilder areaBuilder( builder );

        Mega::Type type( 1 );

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
    flatbuffers::FlatBufferBuilder builder;

    const auto sites = getSites();
    VERIFY_RTE_MSG( sites.size() < 2, "More than one root site in schematic" );
    if( !sites.empty() )
    {
        recurseSites( builder, sites.front() );
    }

    builder.Finished();

    const auto size = builder.GetSize();

    const auto* pBuffer = builder.GetBufferPointer();
}

} // namespace schematic