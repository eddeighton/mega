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

#include "map/factory.hpp"
#include "map/property.hpp"

#include "map/mission.hpp"
#include "map/ship.hpp"
#include "map/base.hpp"
#include "map/clip.hpp"
#include "map/space.hpp"
#include "map/wall.hpp"
#include "map/connection.hpp"
#include "map/object.hpp"
#include "map/basicFeature.hpp"

#include "common/assert_verify.hpp"
#include "common/variant_utils.hpp"
#include "common/file.hpp"

#include <boost/variant.hpp>
#include <boost/filesystem.hpp>

#include <fstream>
#include <memory>

namespace map
{

namespace
{
static constexpr auto boostXMLArchiveFlags = boost::archive::no_header | boost::archive::no_codecvt
                                              | boost::archive::no_tracking; // | boost::archive::no_xml_tag_checking

format::File loadFile( const std::string& strFilePath )
{
    format::File file;
    try
    {
        auto                         inFile = boost::filesystem::loadFileStream( strFilePath );
        boost::archive::xml_iarchive ia( *inFile, boostXMLArchiveFlags );
        ia&                          boost::serialization::make_nvp( "file", file );
    }
    catch( std::exception& ex )
    {
        THROW_RTE( "Failed to load file: " << strFilePath << " exception: " << ex.what() );
    }
    return file;
}

void saveFile( const std::string& strFilePath, const format::File& file )
{
    try
    {
        auto                         outFile = boost::filesystem::createOrLoadNewFileStream( strFilePath );
        boost::archive::xml_oarchive oa( *outFile, boostXMLArchiveFlags );
        oa&                          boost::serialization::make_nvp( "file", file );
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

    const format::File file = loadFile( strFilePath );

    File::Ptr pFile;
    {
        // dispatch to the concrete type and instantiate it
        if( file.has_schematic() )
        {
            const format::File::Schematic& schematic = file.schematic();

            if( schematic.has_clip() )
            {
                pFile.reset( new Clip( strFilePath ) );
            }
            else if( schematic.has_ship() )
            {
                pFile.reset( new Ship( strFilePath ) );
            }
            else if( schematic.has_base() )
            {
                pFile.reset( new Base( strFilePath ) );
            }
            else
            {
                THROW_RTE( "Unrecognised schematic file type: " << strFilePath );
            }
        }
        else if( file.has_script() )
        {
            THROW_RTE( "TODO" );
        }
        else
        {
            THROW_RTE( "Unrecognised format use in file: " << strFilePath );
        }
    }

    VERIFY_RTE_MSG( pFile, "Failed to load: " << strFilePath );

    pFile->init();

    pFile->load( file );

    return pFile;
}

void load( File::Ptr pFile, const boost::filesystem::path& filePath )
{
    VERIFY_RTE_MSG( boost::filesystem::exists( filePath ), "Could not find file: " << filePath.string() );

    const std::string  strFilePath = filePath.string();
    const format::File file        = loadFile( strFilePath );

    {
        // check correct type
        if( file.has_schematic() )
        {
            const format::File::Schematic& schematic = file.schematic();
            if( schematic.has_clip() )
            {
                Clip::Ptr pClip = boost::dynamic_pointer_cast< Clip >( pFile );
                VERIFY_RTE( pClip );
            }
            else if( schematic.has_ship() )
            {
                Ship::Ptr pShip = boost::dynamic_pointer_cast< Ship >( pFile );
                VERIFY_RTE( pShip );
            }
            else if( schematic.has_base() )
            {
                Base::Ptr pBase = boost::dynamic_pointer_cast< Base >( pFile );
                VERIFY_RTE( pBase );
            }
            else
            {
                THROW_RTE( "Unrecognised schematic file type: " << strFilePath );
            }
        }
        else if( file.has_script() )
        {
            THROW_RTE( "TODO" );
        }
        else
        {
            THROW_RTE( "Unrecognised format use in file: " << strFilePath );
        }
    }

    pFile->init();

    pFile->load( file );
}

void save( File::PtrCst pFile, const boost::filesystem::path& filePath )
{
    format::File file;
    pFile->save( file );
    saveFile( filePath.string(), file );
}

Site::Ptr construct( Node::Ptr pParent, const format::Site& site )
{
    Site::Ptr pNewSite;

    // dispatch to deriving site type
    if( site.has_space() )
    {
        pNewSite = Space::Ptr( new Space( pParent, site.name ) );
    }
    else if( site.has_connection() )
    {
        pNewSite = Connection::Ptr( new Connection( pParent, site.name ) );
    }
    else if( site.has_wall() )
    {
        pNewSite = Wall::Ptr( new Wall( pParent, site.name ) );
    }
    else if( site.has_object() )
    {
        pNewSite = Object::Ptr( new Object( pParent, site.name ) );
    }
    else
    {
        THROW_RTE( "Unrecognised site type" );
    }

    VERIFY_RTE( pNewSite );

    return pNewSite;
}

Polygon formatPolygonFromPath( const format::Path& path )
{
    Polygon polygon;
    polygon.resize( path.points.size() );
    for( const auto& element : path.points )
    {
        const int         uiIndex = element.first;
        const format::F2& point   = element.second;
        polygon[ uiIndex ]        = Point( point.x, point.y );
    }
    return polygon;
}

void formatPolygonToPath( const Polygon& polygon, format::Path& path )
{
    int szIndex = 0;
    for( const Point& pt : polygon )
    {
        path.points.insert( { szIndex, format::F2{ pt.x(), pt.y() } } );
        ++szIndex;
    }
}

} // namespace map
