#ifndef GLYPHS_H
#define GLYPHS_H

#include <QDebug>
#include <QPixmap>
#include <QImage>
#include <QGraphicsPixmapItem>
#include <QGraphicsEffect>
#include <QGraphicsSimpleTextItem>
#include <QPainter>
#include <QGraphicsScene>

#ifndef Q_MOC_RUN

#include "toolbox.hpp"
#include "viewConfig.hpp"

#include "schematic/site.hpp"
#include "schematic/glyph.hpp"
#include "schematic/cgalUtils.hpp"
#include "schematic/editInteractions.hpp"

#include "common/assert_verify.hpp"
#include "common/tick.hpp"

#include <map>

#endif

namespace editor
{

using ItemMap = std::map< QGraphicsItem*, schematic::IGlyph* >;
using SpecMap = std::map< const schematic::GlyphSpec*, QGraphicsItem* >;

struct GlyphMap
{
    ItemMap& itemMap;
    SpecMap& specMap;
    GlyphMap( ItemMap& _itemMap, SpecMap& _specMap )
        : itemMap( _itemMap )
        , specMap( _specMap )
    {
    }

    QGraphicsItem* findItem( const schematic::GlyphSpec* pSpec ) const
    {
        QGraphicsItem* pItem = 0u;
        if( pSpec )
        {
            SpecMap::const_iterator iFind = specMap.find( pSpec );
            if( iFind != specMap.end() )
                pItem = iFind->second;
            // only the root can have no graphics item
            if( pSpec->getParent() )
            {
                ASSERT_MSG( pItem, "Failed to locate QGraphicsItem*" );
            }
        }
        return pItem;
    }

    void insert( QGraphicsItem* pGraphicsItem, const schematic::GlyphSpec* pSpec, schematic::IGlyph* pGlyph )
    {
        itemMap.insert( std::make_pair( pGraphicsItem, pGlyph ) );
        specMap.insert( std::make_pair( pSpec, pGraphicsItem ) );
    }
    void erase( QGraphicsItem* pGraphicsItem, const schematic::GlyphSpec* pSpec )
    {
        itemMap.erase( pGraphicsItem );
        specMap.erase( pSpec );
    }
};

class Selectable
{
public:
    Selectable( unsigned int uiDepth )
        : m_bSelected( false )
        , m_uiDepth( uiDepth )
    {
    }
    bool         isSelected() const { return m_bSelected; }
    unsigned int getDepth() const { return m_uiDepth; }
    virtual void setSelected( bool bSelected );
    virtual bool isOrigin() const = 0;

private:
    bool               m_bSelected;
    const unsigned int m_uiDepth;
};

class Renderable
{
public:
    virtual void setShouldRender( bool bShouldRender ) = 0;
};

class ZoomDependent
{
public:
    virtual void OnNewZoomLevel( float fZoom ) = 0;
};

class Configurable
{
public:
    virtual void forceUpdate() = 0;
};

static unsigned int calculateDepth( const schematic::GlyphSpec* pGlyphSpec )
{
    unsigned int uiDepth = 0u;
    while( const schematic::GlyphSpec* pParent = pGlyphSpec->getParent() )
    {
        pGlyphSpec = pParent;
        ++uiDepth;
    }
    return uiDepth;
}

class PainterImpl : public schematic::Painter
{
public:
    struct Path
    {
        QPainterPath path;
        std::string  style;
    };
    using PathPtr    = std::shared_ptr< Path >;
    using PathVector = std::vector< PathPtr >;

private:
    const ViewConfig*  m_pViewConfig = nullptr;
    PathVector         m_paths, m_oldPaths;
    Timing::UpdateTick m_updateTick;
    bool               m_bForceUpdate = true;
    float              m_fArrowSize   = 1.0f;

public:
    const PathVector& getPaths() const { return m_paths; }
    QPainterPath&     getPath()
    {
        if( m_paths.empty() )
        {
            m_paths.push_back( std::make_shared< Path >() );
        }
        return m_paths.back()->path;
    }

    PainterImpl() = default;

    PainterImpl( const ViewConfig* pViewConfig )
        : m_pViewConfig( pViewConfig )
    {
    }

    void setArrowHeadSize( float fSize ) { m_fArrowSize = fSize; }
    void forceUpdate() { m_bForceUpdate = true; }

    virtual bool needsUpdate( const Timing::UpdateTick& updateTick )
    {
        return m_bForceUpdate || ( updateTick > m_updateTick );
    }

    virtual void updated()
    {
        m_bForceUpdate = false;
        m_updateTick.update();
    }

    virtual void reset()
    {
        m_oldPaths = m_paths;
        m_paths.clear();
    }

    virtual void style( const std::string& strStyle )
    {
        if( m_paths.empty() || m_paths.back()->style != strStyle )
        {
            m_paths.push_back( std::make_shared< Path >() );
            m_paths.back()->style = strStyle;
        }
    }

    virtual void moveTo( const schematic::Point& pt ) { getPath().moveTo( pt.x(), pt.y() ); }
    virtual void lineTo( const schematic::Point& pt ) { getPath().lineTo( pt.x(), pt.y() ); }
    virtual void closePath() { getPath().closeSubpath(); }

    virtual void segment( const schematic::Segment& segment, exact::EdgeMask::Set mask )
    {
        auto showType = ViewConfig::eShowNormal;
        if( m_pViewConfig )
        {
            showType = m_pViewConfig->showSegment( mask );
        }
        switch( showType )
        {
            case ViewConfig::eShowNone:
            {
            }
            break;
            case ViewConfig::eShowNormal:
            {
                moveTo( segment[ 0 ] );
                lineTo( segment[ 1 ] );

                schematic::Segment arrow = schematic::Utils::makeArrowHead( segment, m_fArrowSize );
                moveTo( arrow[ 0 ] );
                lineTo( arrow[ 1 ] );
            }
            break;
            case ViewConfig::eShowHighlight:
            {
                moveTo( segment[ 0 ] );
                lineTo( segment[ 1 ] );
            }
            break;
            case ViewConfig::TOTAL_SHOW_TYPES:
            {
            }
            break;
        }
    }

    virtual void polygon( const schematic::Polygon& polygon )
    {
        std::size_t sz = 0, szTotal = polygon.size();
        for( const auto& pt : polygon )
        {
            if( sz == 0 )
            {
                moveTo( pt );
            }
            else if( sz == ( szTotal - 1U ) )
            {
                lineTo( pt );
                closePath();
            }
            else
            {
                lineTo( pt );
            }
            ++sz;
        }
    }

    virtual void polygonWithHoles( const schematic::Polygon_with_holes& polygon )
    {
        this->polygon( polygon.outer_boundary() );
        for( const auto& hole : polygon.holes() )
        {
            this->polygon( hole );
        }
    }
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
class GlyphControlPoint : public schematic::GlyphControlPoint,
                          public Selectable,
                          public ZoomDependent,
                          public Renderable
{
public:
    GlyphControlPoint( schematic::IGlyph::Ptr pParent, QGraphicsScene* pScene, GlyphMap map,
                       schematic::ControlPoint* pControlPoint, float fZoom, Toolbox::Ptr pToolboxPtr );
    ~GlyphControlPoint();

    // Selectable
    virtual void setSelected( bool bSelected );
    virtual bool isOrigin() const { return false; }

    // ZoomDependent
    virtual void OnNewZoomLevel( float fZoom );

    // Renderable
    virtual void setShouldRender( bool bShouldRender );

    // schematic::GlyphControlPoint
    virtual void update();

    void                  updateColours();
    QGraphicsEllipseItem* getItem() const;

private:
    float                 m_fSizeScaling;
    float                 m_fSize;
    QGraphicsScene*       m_pScene;
    GlyphMap              m_map;
    QGraphicsEllipseItem* m_pItem;
    Toolbox::Ptr          m_pToolBoxPtr;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
class GlyphPolygonGroup : public schematic::GlyphPolygonGroup,
                          public ZoomDependent,
                          public Renderable,
                          public Configurable
{
public:
    GlyphPolygonGroup( schematic::IGlyph::Ptr pParent, QGraphicsScene* pScene, GlyphMap map,
                       schematic::MarkupPolygonGroup* pPolygonGroup, const ViewConfig* pViewConfig, float fZoom,
                       Toolbox::Ptr pToolBoxPtr );
    ~GlyphPolygonGroup();

    // ZoomDependent
    virtual void OnNewZoomLevel( float fZoom );

    // Renderable
    virtual void setShouldRender( bool bShouldRender );

    // Configurable
    virtual void forceUpdate();

    // schematic::GlyphPath
    virtual void update();

private:
    float                             m_fSize;
    QGraphicsScene*                   m_pScene;
    const ViewConfig*                 m_pViewConfig;
    PainterImpl                       m_pathPainter;
    GlyphMap                          m_map;
    std::vector< QGraphicsPathItem* > m_items;
    QGraphicsPathItem*                m_pInvisibleItem = nullptr;
    Toolbox::Ptr                      m_pToolBoxPtr;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
class GlyphOrigin : public schematic::GlyphOrigin, public Selectable, public Renderable
{
public:
    GlyphOrigin( schematic::IGlyph::Ptr pParent, QGraphicsScene* pScene, GlyphMap map, schematic::Origin* pOrigin,
                 schematic::IEditContext*& pActiveContext, Toolbox::Ptr pToolBoxPtr );
    ~GlyphOrigin();

    // Selectable
    virtual void setSelected( bool bSelected );
    virtual bool isOrigin() const { return true; }

    // Renderable
    virtual void setShouldRender( bool bShouldRender );

    bool isActiveContext() const;

    // schematic::GlyphOrigin
    virtual void update();

    void updateColours();

private:
    void setOrCreateImageItem();

private:
    QGraphicsScene* m_pScene;
    GlyphMap        m_map;

    PainterImpl        m_pathPainter;
    QGraphicsLineItem* m_pItemX;
    QGraphicsLineItem* m_pItemY;
    QGraphicsPathItem* m_pPathItem;

    schematic::IEditContext*& m_pActiveContext;
    bool                      m_bActiveContext;
    Toolbox::Ptr              m_pToolBoxPtr;
    bool                      m_bShouldRender = true;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
class GlyphText : public schematic::GlyphText, public Renderable
{
public:
    GlyphText( schematic::IGlyph::Ptr pParent, QGraphicsScene* pScene, GlyphMap map, schematic::MarkupText* pText,
               Toolbox::Ptr pToolBoxPtr );
    ~GlyphText();

    // Renderable
    virtual void setShouldRender( bool bShouldRender );

    // schematic::GlyphText
    virtual void update();

private:
    QGraphicsScene*          m_pScene;
    GlyphMap                 m_map;
    QGraphicsSimpleTextItem* m_pItem;
    Toolbox::Ptr             m_pToolBoxPtr;
};
////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

class GlyphImage : public schematic::GlyphImage, public Renderable
{
public:
    GlyphImage( schematic::IGlyph::Ptr pParent, QGraphicsScene* pScene, GlyphMap map, schematic::ImageSpec* pImage,
                Toolbox::Ptr pToolBoxPtr );
    ~GlyphImage();

    // Renderable
    virtual void setShouldRender( bool bShouldRender );

    // schematic::GlyphImage
    virtual void update();

private:
    void setOrCreateImageItem();

private:
    static QVector< QRgb > m_coloursNormal;
    QPixmap                m_pixelMap;
    QGraphicsScene*        m_pScene;
    GlyphMap               m_map;
    QGraphicsPixmapItem*   m_pItem;
    Toolbox::Ptr           m_pToolBoxPtr;
    Timing::UpdateTick     m_lastUpdateTick;
};

} // namespace editor

#endif // GLYPHS_H
