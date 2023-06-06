#ifndef TOOLBOX_23_09_2013
#define TOOLBOX_23_09_2013

#include <QColor>

#ifndef Q_MOC_RUN

#include "schematic/schematic.hpp"

#include "ed/node.hpp"
#include "ed/shorthandio.hpp"

#include "common/tick.hpp"

#include "boost/shared_ptr.hpp"
#include "boost/filesystem/path.hpp"

#include <string>

#endif

namespace editor
{

inline Ed::IShorthandStream& operator>>( Ed::IShorthandStream& is, QColor& colour )
{
    int r, g, b, a;
    is >> r >> g >> b >> a;
    colour = QColor( r, g, b, a );
    return is;
}

class Toolbox
{
public:
    class Palette
    {
    public:
        using Ptr = boost::shared_ptr<Palette>;
        using PtrMap = std::map<std::string, Ptr>;

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
    using Ptr = boost::shared_ptr<Toolbox>;

    Toolbox( const std::string& strDirectoryPath );

    void reload();

    schematic::Schematic::Ptr getCurrentItem() const;
    const Palette::PtrMap&    get() const { return m_palettes; }
    Palette::Ptr              getPalette( const std::string& strName ) const;
    void                      selectPalette( Palette::Ptr pPalette );

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

        try
        {
            boost::optional< const Ed::Node& > findResult = Ed::find( m_config, strKey );
            Ed::IShorthandStream               is( findResult.get().statement.shorthand.get() );
            is >> value;
            cache.insert( std::make_pair( strKey, value ) );
        }
        catch( std::exception& ex )
        {
            THROW_RTE( "Failed to load config value: " << strKey << " with error:" << ex.what() );
        }
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
        try
        {
            boost::optional< const Ed::Node& > findResult = Ed::find( m_config, strKey );
            Ed::IShorthandStream               is( findResult.get().statement.shorthand.get() );
            Ed::serialiseIn( is, values );
            cache.insert( std::make_pair( strKey, values ) );
        }
        catch( std::exception& ex )
        {
            THROW_RTE( "Failed to load config value: " << strKey << " with error:" << ex.what() );
        }
    }

private:
    void recursiveLoad( const boost::filesystem::path&    pathIter,
                        const std::vector< std::string >& currentLocation,
                        const std::vector< std::string >& ignorFolders );

private:
    boost::filesystem::path m_rootPath;
    Palette::PtrMap         m_palettes;
    Palette::Ptr            m_pCurrentPalette;
    Ed::Node                m_config;
};

} // namespace editor

#endif // TOOLBOX_23_09_2013
