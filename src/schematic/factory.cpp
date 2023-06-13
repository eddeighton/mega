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

#include "schematic/factory.hpp"
#include "schematic/property.hpp"

#include "schematic/space.hpp"
#include "schematic/wall.hpp"
#include "schematic/connection.hpp"
#include "schematic/cut.hpp"
#include "schematic/object.hpp"
#include "schematic/feature.hpp"
#include "schematic/schematic.hpp"

#include "ed/file.hpp"

#include "common/assert_verify.hpp"
#include "common/variant_utils.hpp"
#include "common/file.hpp"

#include <boost/variant.hpp>
#include <boost/filesystem.hpp>

#include <fstream>
#include <memory>

namespace schematic
{

namespace
{

format::Node loadFile( const std::string& strFilePath )
{
    format::Node file;
    try
    {
        Ed::Node rootNode;
        {
            Ed::BasicFileSystem fileSystem;
            Ed::File            file( fileSystem, strFilePath );

            //file.expandShorthand();
            //file.removeTypes();

            file.toNode( rootNode );
        }

        VERIFY_RTE_MSG( rootNode.children.size() == 1,
            "File has more than one root node: " << strFilePath );

        for( const auto& n : rootNode )
        {
            file.load( n );
            break;
        }
    }
    catch( std::exception& ex )
    {
        THROW_RTE( "Failed to load file: " << strFilePath << " exception: " << ex.what() );
    }
    return file;
}

void saveFile( const std::string& strFilePath, const format::Node& file )
{
    try
    {
        Ed::Node rootNode;
        file.save( rootNode );

        Ed::Node parentNode;
        parentNode.children.emplace_back( rootNode );
        Ed::saveNodeToFile( strFilePath, parentNode );
    }
    catch( std::exception& ex )
    {
        THROW_RTE( "Failed to save file: " << strFilePath << " exception: " << ex.what() );
    }
}
} // namespace

File::Ptr load( const boost::filesystem::path& filePath )
{
    VERIFY_RTE_MSG( boost::filesystem::exists( filePath ), "Could not find file: " << filePath.string() );

    const std::string strFilePath = filePath.string();

    const format::Node rootNode = loadFile( strFilePath );

    File::Ptr pFile;

    if( rootNode.has_file() )
    {
        const format::Node::File& file = rootNode.file();

        // dispatch to the concrete type and instantiate it
        if( file.has_schematic() )
        {
            const format::Node::File::Schematic& schematic = file.schematic();
            pFile.reset( new schematic::Schematic( strFilePath ) );
        }
        else
        {
            THROW_RTE( "Unrecognised format use in file: " << strFilePath );
        }
    }
    else
    {
        THROW_RTE( "Invalid root node type" );
    }

    VERIFY_RTE_MSG( pFile, "Failed to load: " << strFilePath );

    pFile->load( rootNode );

    pFile->init();

    return pFile;
}

void save( File::PtrCst pFile, const boost::filesystem::path& filePath )
{
    format::Node file;
    pFile->save( file );
    saveFile( filePath.string(), file );
}

Node::Ptr construct( Node::Ptr pParent, const format::Node& node )
{
    Node::Ptr pNewNode;

    // dispatch to deriving type

    if( node.has_site() )
    {
        const format::Node::Site& site = node.site();

        if( site.has_space() )
        {
            pNewNode = Space::Ptr( new Space( pParent, node.name ) );
        }
        else if( site.has_connection() )
        {
            pNewNode = Connection::Ptr( new Connection( pParent, node.name ) );
        }
        else if( site.has_cut() )
        {
            pNewNode = Cut::Ptr( new Cut( pParent, node.name ) );
        }
        else if( site.has_wall() )
        {
            pNewNode = Wall::Ptr( new Wall( pParent, node.name ) );
        }
        else if( site.has_object() )
        {
            pNewNode = Object::Ptr( new Object( pParent, node.name ) );
        }
        else
        {
            THROW_RTE( "Unrecognised site type" );
        }
    }
    else if( node.has_feature() )
    {
        const format::Node::Feature& feature = node.feature();
        if( feature.has_point() )
        {
            pNewNode = Feature_Point::Ptr( new Feature_Point( pParent, node.name ) );
        }
        else if( feature.has_contour() )
        {
            pNewNode = Feature_Contour::Ptr( new Feature_Contour( pParent, node.name ) );
        }
        else if( feature.has_lineSegment() )
        {
            pNewNode = Feature_LineSegment::Ptr( new Feature_LineSegment( pParent, node.name ) );
        }
        else if( feature.has_pin() )
        {
            pNewNode = Feature_Pin::Ptr( new Feature_Pin( pParent, node.name ) );
        }
        else
        {
            THROW_RTE( "Unrecognised site type" );
        }
    }
    else if( node.has_property() )
    {
        pNewNode = Property::Ptr( new Property( pParent, node.name ) );
    }
    else
    {
        THROW_RTE( "Unrecognised node type" );
    }

    VERIFY_RTE( pNewNode );

    return pNewNode;
}

void formatPolygonToPath( const Polygon& polygon, format::Path& path )
{
    for( const Point& pt : polygon )
    {
        path.points.push_back( format::F2{ pt.x(), pt.y() } );
    }
}

} // namespace schematic
