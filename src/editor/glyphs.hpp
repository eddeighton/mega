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

#include "map/site.hpp"
#include "map/glyph.hpp"
#include "map/editInteractions.hpp"

#include "common/assert_verify.hpp"
#include "common/tick.hpp"

#include <map>

#endif

namespace editor
{

typedef std::map< QGraphicsItem*, map::IGlyph* > ItemMap;
typedef std::map< const map::GlyphSpec*, QGraphicsItem* > SpecMap;

struct GlyphMap
{
    ItemMap& itemMap;
    SpecMap& specMap;
    GlyphMap( ItemMap& _itemMap, SpecMap& _specMap )
        : itemMap( _itemMap ),
          specMap( _specMap )
    {

    }

    QGraphicsItem* findItem( const map::GlyphSpec* pSpec ) const
    {
        QGraphicsItem* pItem = 0u;
        if( pSpec )
        {
            SpecMap::const_iterator iFind = specMap.find( pSpec );
            if( iFind != specMap.end() )
                pItem = iFind->second;
            //only the root can have no graphics item
            if( pSpec->getParent() )
            {
                ASSERT_MSG( pItem, "Failed to locate QGraphicsItem*" );
            }
        }
        return pItem;
    }

    void insert( QGraphicsItem* pGraphicsItem, const map::GlyphSpec* pSpec, map::IGlyph* pGlyph)
    {
        itemMap.insert( std::make_pair( pGraphicsItem, pGlyph ) );
        specMap.insert( std::make_pair( pSpec, pGraphicsItem ) );
    }
    void erase( QGraphicsItem* pGraphicsItem, const map::GlyphSpec* pSpec )
    {
        itemMap.erase( pGraphicsItem );
        specMap.erase( pSpec );
    }
};

class Selectable
{
public:
    Selectable( unsigned int uiDepth )
        :   m_bSelected( false ),
            m_uiDepth( uiDepth )
    {
    }
    bool isSelected() const { return m_bSelected; }
    unsigned int getDepth() const { return m_uiDepth; }
    virtual void setSelected( bool bSelected );
    virtual bool isImage() const = 0;
private:
    bool m_bSelected;
    const unsigned int m_uiDepth;
};

class Renderable
{
public:
    virtual void setShouldRender( bool bShouldRender )=0;
};

class ZoomDependent
{
public:
    virtual void OnNewZoomLevel( float fZoom ) = 0;
};

static unsigned int calculateDepth( const map::GlyphSpec* pGlyphSpec )
{
    unsigned int uiDepth = 0u;
    while( const map::GlyphSpec* pParent = pGlyphSpec->getParent() )
    {
        pGlyphSpec = pParent;
        ++uiDepth;
    }
    return uiDepth;
}


class PainterImpl : public map::Painter
{
    std::shared_ptr< QPainterPath > m_pPath, m_pOldPath;
    Timing::UpdateTick m_updateTick;
    bool m_bInitialising = true;
public:
    const QPainterPath& getPath() const { return *m_pPath; }

    PainterImpl()
        :   m_pPath( std::make_shared< QPainterPath >() )
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
    virtual void moveTo( const map::Point& pt )
    {
        m_pPath->moveTo( pt.x(), pt.y() );
    }
    virtual void lineTo( const map::Point& pt )
    {
        m_pPath->lineTo( pt.x(), pt.y() );
    }
    virtual void closePath()
    {
        m_pPath->closeSubpath();
    }
    virtual void polygon( const map::Polygon& polygon )
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
class GlyphControlPoint : public map::GlyphControlPoint, public Selectable, public ZoomDependent, public Renderable
{
public:
    GlyphControlPoint( map::IGlyph::Ptr pParent, QGraphicsScene* pScene,
                       GlyphMap map, map::ControlPoint* pControlPoint, float fZoom,
                       Toolbox::Ptr pToolboxPtr );
    ~GlyphControlPoint();

    //Selectable
    virtual void setSelected( bool bSelected );
    virtual bool isImage() const { return false; }

    //ZoomDependent
    virtual void OnNewZoomLevel( float fZoom );

    //Renderable
    virtual void setShouldRender( bool bShouldRender );

    //map::GlyphControlPoint
    virtual void update();

    void updateColours();
    QGraphicsEllipseItem* getItem() const;

private:
    float m_fSizeScaling;
    float m_fSize;
    QGraphicsScene* m_pScene;
    GlyphMap m_map;
    QGraphicsEllipseItem* m_pItem;
    Toolbox::Ptr m_pToolBoxPtr;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
class GlyphPolygonGroup : public map::GlyphPolygonGroup, public ZoomDependent, public Renderable
{
public:
    GlyphPolygonGroup( map::IGlyph::Ptr pParent, QGraphicsScene* pScene,
               GlyphMap map, map::MarkupPolygonGroup* pPolygonGroup, float fZoom,
               Toolbox::Ptr pToolBoxPtr );
    ~GlyphPolygonGroup();

    //ZoomDependent
    virtual void OnNewZoomLevel( float fZoom );

    //Renderable
    virtual void setShouldRender( bool bShouldRender );

    //map::GlyphPath
    virtual void update();

private:
    float m_fSize;
    PainterImpl m_pathPainter;
    QGraphicsScene* m_pScene;
    GlyphMap m_map;
    QGraphicsPathItem* m_pItem;
    Toolbox::Ptr m_pToolBoxPtr;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
class GlyphOrigin : public map::GlyphOrigin, public Selectable, public Renderable
{
public:
    GlyphOrigin( map::IGlyph::Ptr pParent, QGraphicsScene* pScene,
                GlyphMap map, map::Origin* pOrigin, 
                map::IEditContext*& pActiveContext,
                Toolbox::Ptr pToolBoxPtr );
    ~GlyphOrigin();

    //Selectable
    virtual void setSelected( bool bSelected );
    virtual bool isImage() const { return true; }

    //Renderable
    virtual void setShouldRender( bool bShouldRender );

    bool isActiveContext() const;

    //map::GlyphOrigin
    virtual void update();

    void updateColours();
private:
    void setOrCreateImageItem();
private:
    QGraphicsScene* m_pScene;
    GlyphMap m_map;
    
    PainterImpl m_pathPainter;
    QGraphicsPathItem* m_pPathItem;
    QGraphicsLineItem* m_pItemX;
    QGraphicsLineItem* m_pItemY;
    
    map::IEditContext*& m_pActiveContext;
    bool m_bActiveContext;
    Toolbox::Ptr m_pToolBoxPtr;
    bool m_bShouldRender = true;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
/*
class GlyphImage : public map::GlyphImage, public Selectable, public Renderable
{
public:
    GlyphImage( map::IGlyph::Ptr pParent, QGraphicsScene* pScene,
                GlyphMap map, map::ImageSpec* pImage, 
                map::IEditContext*& pActiveContext,
                Toolbox::Ptr pToolBoxPtr );
    ~GlyphImage();

    //Selectable
    virtual void setSelected( bool bSelected );
    virtual bool isImage() const { return true; }

    //Renderable
    virtual void setShouldRender( bool bShouldRender );

    bool isActiveContext() const;

    //map::GlyphImage
    virtual void update();

private:
    void setOrCreateImageItem();
private:
    static QVector< QRgb > m_coloursNormal, m_coloursSelected, m_coloursActiveContext;
    QPixmap m_pixelMap;
    QGraphicsScene* m_pScene;
    GlyphMap m_map;
    QGraphicsPixmapItem* m_pItem;
    map::IEditContext*& m_pActiveContext;
    Timing::UpdateTick m_lastUpdateTick;
    bool m_bActiveContext;
    Toolbox::Ptr m_pToolBoxPtr;
};
*/

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
class GlyphText : public map::GlyphText, public Renderable
{
public:
    GlyphText( map::IGlyph::Ptr pParent, QGraphicsScene* pScene,
               GlyphMap map, map::MarkupText* pText,
               Toolbox::Ptr pToolBoxPtr );
    ~GlyphText();

    //Renderable
    virtual void setShouldRender( bool bShouldRender );

    //map::GlyphText
    virtual void update();

private:
    QGraphicsScene* m_pScene;
    GlyphMap m_map;
    QGraphicsSimpleTextItem* m_pItem;
    Toolbox::Ptr m_pToolBoxPtr;
};

}

#endif // GLYPHS_H
