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
    virtual bool isImage() const = 0;

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
    const ViewConfig*               m_pViewConfig = nullptr;
    std::shared_ptr< QPainterPath > m_pPath, m_pOldPath;
    Timing::UpdateTick              m_updateTick;
    bool                            m_bInitialising = true;

public:
    const QPainterPath& getPath() const { return *m_pPath; }

    PainterImpl()
        : m_pPath( std::make_shared< QPainterPath >() )
    {
    }

    PainterImpl( const ViewConfig* pViewConfig )
        : m_pViewConfig( pViewConfig )
        , m_pPath( std::make_shared< QPainterPath >() )
    {
    }

    virtual bool needsUpdate( const Timing::UpdateTick& updateTick )
    {
        return m_bInitialising || ( updateTick > m_updateTick );
    }

    virtual void updated()
    {
        m_bInitialising = false;
        m_updateTick.update();
    }

    virtual void reset()
    {
        m_pOldPath = m_pPath;
        m_pPath.reset( new QPainterPath );
    }
    virtual void moveTo( const schematic::Point& pt ) { m_pPath->moveTo( pt.x(), pt.y() ); }
    virtual void lineTo( const schematic::Point& pt ) { m_pPath->lineTo( pt.x(), pt.y() ); }
    virtual void closePath() { m_pPath->closeSubpath(); }

    virtual void segment( const schematic::Segment& segment, exact::EdgeMask::Set mask )
    {
        auto showType = ViewConfig::eNormal;
        if( m_pViewConfig )
        {
            showType = m_pViewConfig->showSegment( mask );
        }
        switch( showType )
        {
            case ViewConfig::eNone:
            {
            }
            break;
            case ViewConfig::eNormal:
            {
                moveTo( segment[0] );
                lineTo( segment[1] );
            }
            break;
            case ViewConfig::eHighlight:
            {
                moveTo( segment[0] );
                lineTo( segment[1] );
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
    virtual bool isImage() const { return false; }

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
class GlyphPolygonGroup : public schematic::GlyphPolygonGroup, public ZoomDependent, public Renderable
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

    // schematic::GlyphPath
    virtual void update();

private:
    float              m_fSize;
    QGraphicsScene*    m_pScene;
    const ViewConfig*  m_pViewConfig;
    PainterImpl        m_pathPainter;
    GlyphMap           m_map;
    QGraphicsPathItem* m_pItem;
    Toolbox::Ptr       m_pToolBoxPtr;
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
    virtual bool isImage() const { return true; }

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
    QGraphicsPathItem* m_pPathItem;
    QGraphicsLineItem* m_pItemX;
    QGraphicsLineItem* m_pItemY;

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

} // namespace editor

#endif // GLYPHS_H
