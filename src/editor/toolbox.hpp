#ifndef TOOLBOX_23_09_2013
#define TOOLBOX_23_09_2013

#ifndef Q_MOC_RUN

#include "schematic/schematic.hpp"

#include "common/tick.hpp"

#include "boost/shared_ptr.hpp"
#include "boost/filesystem/path.hpp"

#include <string>

#endif

namespace editor
{

class Toolbox
{
public:
    class Palette
    {
    public:
        typedef boost::shared_ptr< Palette > Ptr;
        typedef std::map< std::string, Ptr > PtrMap;

        using NodeType = schematic::Schematic;

        Palette( const std::string& strName, int iMaximumSize = -1 );

        bool operator<( const Palette& cmp ) const { return m_strName < cmp.m_strName; }

        const std::string&        getName() const { return m_strName; }
        const NodeType::PtrList&  get() const { return m_clips; }
        NodeType::Ptr             getSelection() const;
        const Timing::UpdateTick& getLastModifiedTick() const { return m_updateTick; }

        void add( NodeType::Ptr pSchematic, bool bSelect = true );

        void remove( NodeType::Ptr pSchematic );
        void clear();

        void select( NodeType::Ptr pSite );

    private:
        const std::string           m_strName;
        int                         m_iMaximumSize;
        NodeType::PtrList           m_clips;
        NodeType::PtrList::iterator m_iterSelection;
        Timing::UpdateTick          m_updateTick;
    };
    typedef boost::shared_ptr< Toolbox > Ptr;

    Toolbox( const std::string& strDirectoryPath );

    void reload();

    schematic::Schematic::Ptr    getCurrentItem() const;
    const Palette::PtrMap& get() const { return m_palettes; }
    Palette::Ptr           getPalette( const std::string& strName ) const;
    void                   selectPalette( Palette::Ptr pPalette );

    void add( const std::string& strName, schematic::Schematic::Ptr pNode, bool bSelect = false );

    void remove( Palette::Ptr pPalette );

    template < typename TValue >
    void getConfigValue( const std::string& strKey, TValue& value ) const
    {
        static std::map< std::string, TValue > cache;

        auto iFind = cache.find( strKey );
        if( iFind != cache.end() )
        {
            value = iFind->second;
            return;
        }
        // THROW_RTE( "TODO" );
        // boost::optional< const Ed::Node& > findResult ;//=
        //     //Ed::find( m_config, strKey );
        // Ed::IShorthandStream is( findResult.get().statement.shorthand.get() );
        // is >> value;

        cache.insert( std::make_pair( strKey, value ) );
    }

    template < typename TValue >
    void getConfigValueRange( const std::string& strKey, TValue& values ) const
    {
        static std::map< std::string, TValue > cache;

        auto iFind = cache.find( strKey );
        if( iFind != cache.end() )
        {
            values = iFind->second;
            return;
        }

        // THROW_RTE( "TODO" );
        // boost::optional< const Ed::Node& > findResult;// =
        //    // Ed::find( m_config, strKey );
        // Ed::IShorthandStream is( findResult.get().statement.shorthand.get() );
        // Ed::serialiseIn( is, values );

        cache.insert( std::make_pair( strKey, values ) );
    }

private:
    void recursiveLoad( const boost::filesystem::path&    pathIter,
                        const std::vector< std::string >& currentLocation,
                        const std::vector< std::string >& ignorFolders );

private:
    boost::filesystem::path m_rootPath;
    Palette::PtrMap         m_palettes;
    Palette::Ptr            m_pCurrentPalette;

    // Ed::Node m_config;
};

} // namespace editor

#endif // TOOLBOX_23_09_2013
