#include "toolbox.hpp"

#ifndef Q_MOC_RUN

#include "map/factory.hpp"

#include "map/clip.hpp"
#include "map/space.hpp"
#include "map/wall.hpp"
#include "map/connection.hpp"
#include "map/object.hpp"

#include "common/file.hpp"
#include "common/assert_verify.hpp"

#include <boost/filesystem.hpp>

#endif

namespace editor
{
    
Toolbox::Palette::Palette( const std::string& strName, int iMaximumSize )
    :   m_strName( strName ),
        m_iMaximumSize( iMaximumSize ),
        m_iterSelection( m_clips.end() )
{
}

void Toolbox::Palette::add( NodeType::Ptr pSchematic, bool bSelect )
{
    m_clips.push_front( pSchematic );
    if( bSelect || m_iterSelection == m_clips.end() )
        m_iterSelection = m_clips.begin();

    if( m_iMaximumSize > 0 )
    {
        while( static_cast< int >( m_clips.size() ) > m_iMaximumSize )
        {
            if( !bSelect && *m_iterSelection == m_clips.back() )
            {
                NodeType::Ptr pSelectedSite = *m_iterSelection;
                m_clips.pop_back();
                if( m_clips.size() > 1u )
                {
                    m_clips.pop_back();
                    m_clips.push_back( pSelectedSite );
                    m_iterSelection = m_clips.end();
                    --m_iterSelection;
                }
                else
                    m_iterSelection = m_clips.end();
            }
            else
                m_clips.pop_back();
        }
    }

    m_updateTick.update();
}

void Toolbox::Palette::remove( NodeType::Ptr pSchematic )
{
    NodeType::PtrList::iterator iFind = std::find( m_clips.begin(), m_clips.end(), pSchematic );
    if( iFind != m_clips.end() )
    {
        const bool bWasSelection = ( m_iterSelection == m_clips.end() ) ? false : ( pSchematic == *m_iterSelection );
        NodeType::PtrList::iterator iNext = m_clips.erase( iFind );
        if( bWasSelection )
        {
            if( !m_clips.empty() )
            {
                if( iNext != m_clips.end() )
                    m_iterSelection = iNext;
                else
                    m_iterSelection = --(m_clips.end());
            }
            else
                m_iterSelection = m_clips.end();
        }
    }
    m_updateTick.update();
}

void Toolbox::Palette::clear()
{
    m_iterSelection = m_clips.end();
    m_clips.clear();
    m_updateTick.update();
}

map::Schematic::Ptr Toolbox::Palette::getSelection() const
{
    NodeType::Ptr pSelected;
    if( m_iterSelection != m_clips.end() )
        pSelected = *m_iterSelection;
    return pSelected;
}

void Toolbox::Palette::select( NodeType::Ptr pSite )
{
    NodeType::PtrList::iterator iFind = std::find( m_clips.begin(), m_clips.end(), pSite );
    ASSERT( iFind != m_clips.end() );
    if( iFind != m_clips.end() )
        m_iterSelection = iFind;
    m_updateTick.update();
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

Toolbox::Toolbox( const std::string& strDirectoryPath )
{
    //recursively load all missions under the root directory
    using namespace boost::filesystem;
    VERIFY_RTE_MSG( exists( strDirectoryPath ), "Could not locate toolbox data path at: " << strDirectoryPath );
    m_rootPath = canonical( absolute( strDirectoryPath ) );
    VERIFY_RTE_MSG( exists( m_rootPath ), "Path did not canonicalise properly: " << m_rootPath.string() );
    
    //attempt to load the config file if it exists
    const path configFile = m_rootPath / "config.ed";
    if( exists( configFile ) )
    {
        //THROW_RTE( "TODO" );
        
        //Ed::BasicFileSystem filesystem;
        //Ed::File edConfigFile( filesystem, configFile.string() );
        //edConfigFile.expandShorthand();
        //edConfigFile.removeTypes();
        
        //edConfigFile.toNode( m_config );
    }
    else
    {
        THROW_RTE( "Failed to locate file: " << configFile.string() );
    }
    
    reload();
}
    
map::Schematic::Ptr Toolbox::getCurrentItem() const
{
    map::Schematic::Ptr pItem;
    if( m_pCurrentPalette )
        pItem = m_pCurrentPalette->getSelection();
    return pItem;
}
    
Toolbox::Palette::Ptr Toolbox::getPalette( const std::string& strName ) const
{
    Palette::Ptr pResult;
    Palette::PtrMap::const_iterator iFind = m_palettes.find( strName );
    if( iFind != m_palettes.end() )
        pResult = iFind->second;
    return pResult;
}

void Toolbox::selectPalette( Palette::Ptr pPalette )
{
    m_pCurrentPalette = pPalette;
}

std::string pathToName( const boost::filesystem::path& p )
{
    return p.string();
    //return p.leaf().replace_extension().string();
}

void Toolbox::reload()
{
    //recursively load all missions under the root directory
    using namespace boost::filesystem;
    
    m_pCurrentPalette.reset();
    m_palettes.clear();
    
    //generate defaults...
    {
        map::Clip::Ptr pDefaultClip( new map::Clip( "wall" ) );
        map::Wall::Ptr pDefaultSpace( new map::Wall( pDefaultClip, "wall" ) );
        pDefaultClip->add( pDefaultSpace );
        add( "basic", pDefaultClip, false );
    }
    {
        map::Clip::Ptr pDefaultClip( new map::Clip( "object" ) );
        map::Object::Ptr pDefaultSpace( new map::Object( pDefaultClip, "object" ) );
        pDefaultClip->add( pDefaultSpace );
        add( "basic", pDefaultClip, false );
    }
    {
        map::Clip::Ptr pDefaultClip( new map::Clip( "connection" ) );
        map::Connection::Ptr pDefaultSpace( new map::Connection( pDefaultClip, "connection" ) );
        pDefaultClip->add( pDefaultSpace );
        add( "basic", pDefaultClip, false );
    }
    {
        map::Clip::Ptr pDefaultClip( new map::Clip( "connection" ) );
        map::Connection::Ptr pDefaultSpace( new map::Connection( pDefaultClip, "connection" ) );
        pDefaultSpace->init();
        for( auto i = 0; i != map::QuarterTurn; ++i )
            pDefaultSpace->cmd_rotateLeft( pDefaultSpace->getAABB() );
        pDefaultClip->add( pDefaultSpace );
        add( "basic", pDefaultClip, false );
    }
    {
        map::Clip::Ptr pDefaultClip( new map::Clip( "space" ) );
        map::Space::Ptr pDefaultSpace( new map::Space( pDefaultClip, "space" ) );
        pDefaultClip->add( pDefaultSpace );
        add( "basic", pDefaultClip, true ); //ensure space is the default one
    }
    
    //THROW_RTE( "TODO" );
    
    std::vector< std::string > ignoredFolders;
    //getConfigValueRange( ".toolbox.folders.ignor", ignoredFolders );
    //
    std::vector< std::string > location;
    location.push_back( "data" );
    
    recursiveLoad( m_rootPath, location, ignoredFolders );
}

void Toolbox::recursiveLoad( const boost::filesystem::path& pathIter, 
        const std::vector< std::string >& currentLocation, 
        const std::vector< std::string >& ignorFolders )
{
    std::ostringstream os;
    for( const std::string& str : currentLocation )
    {
        os << str << '/';
    }
    
    using namespace boost::filesystem;
    for( directory_iterator iter( pathIter ); iter != directory_iterator(); ++iter )
    {
        boost::filesystem::path pth = *iter;
        if( is_regular_file( *iter ) && pth.extension().string() == ".sch" )
        {
            try
            {
                if( map::File::Ptr pFile = map::load( pth ) )
                {
					if( map::Schematic::Ptr pSchematic = 
						boost::dynamic_pointer_cast< map::Schematic >( pFile ) )
                    {
                    	add( os.str(), pSchematic, false );
                    }
                }
            }
            catch( std::exception& ex )
            {
                //...
            }
        }
        else if( is_directory( *iter ) )
        {
            std::vector< std::string > nestedDirectory = currentLocation;
            {
                const boost::filesystem::path folderPath = *iter;
                nestedDirectory.push_back( folderPath.filename().string() );
            }
            //if( std::find( ignorFolders.begin(), ignorFolders.end(), nestedDirectory )
            //        == ignorFolders.end() )
            {
                recursiveLoad( *iter, nestedDirectory, ignorFolders );
            }
        }
    }
}

void Toolbox::add( const std::string& strName, map::Schematic::Ptr pNode, bool bSelect )
{
    Palette::Ptr pPalette = getPalette( strName );
    if( !pPalette )
    {
        pPalette = Toolbox::Palette::Ptr( new Toolbox::Palette( strName ) );
        m_palettes.insert( std::make_pair( strName, pPalette ) );
    }
    pPalette->add( pNode, bSelect );
    if( !m_pCurrentPalette ) m_pCurrentPalette = pPalette;
}

void Toolbox::remove( Palette::Ptr pPalette )
{
    Palette::PtrMap::const_iterator iFind = m_palettes.find( pPalette->getName() );
    if( iFind != m_palettes.end() )
    {
        iFind = m_palettes.erase( iFind );
        if( m_pCurrentPalette == pPalette )
        {
            if( iFind != m_palettes.end() )
                m_pCurrentPalette = iFind->second;
            else if( m_palettes.empty() )
                m_pCurrentPalette.reset();
            else
                m_pCurrentPalette = (--(m_palettes.end()))->second;
        }
    }
}

}
