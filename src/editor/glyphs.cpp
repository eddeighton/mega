#include "glyphs.hpp"

#include <QGraphicsOpacityEffect>
#include <QBitmap>


namespace editor
{

void Selectable::setSelected( bool bSelected )
{
    m_bSelected = bSelected;
}

void cleanUpItem( QGraphicsItem* pItem, GlyphMap& map, const map::GlyphSpec* pSpec, QGraphicsScene*  )
{
    if( pItem )
    {
        ASSERT( pItem->childItems().isEmpty() );
        map.erase( pItem, pSpec );
        delete pItem;
    }
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
GlyphControlPoint::GlyphControlPoint( map::IGlyph::Ptr pParent, QGraphicsScene* pScene,
                                      GlyphMap map, map::ControlPoint* pControlPoint, 
                                      float fZoom,
                                      Toolbox::Ptr pToolboxPtr )
    :   map::GlyphControlPoint( pControlPoint, pParent ),
        Selectable( calculateDepth( pControlPoint ) ),
        
        m_fSizeScaling( 12.0f ),
        m_fSize( m_fSizeScaling / fZoom ),
        
        m_pScene( pScene ),
        m_map( map ),
        m_pItem( nullptr ),
        m_pToolBoxPtr( pToolboxPtr )
{    
    QColor pointColour( 255, 255, 255 );
    if( m_pToolBoxPtr )
    {
        m_pToolBoxPtr->getConfigValue( ".glyphs.points.colour", pointColour );
        m_pToolBoxPtr->getConfigValue( ".glyphs.points.width", m_fSizeScaling );
    }
    
    QGraphicsItem* pParentItem = m_map.findItem( getControlPoint()->getParent() );
    m_pItem = new QGraphicsEllipseItem( -( m_fSize / 2.0f ), -( m_fSize / 2.0f ), m_fSize, m_fSize, pParentItem );
    if( !pParentItem ) m_pScene->addItem( m_pItem );
    m_pItem->setPos( getControlPoint()->getPoint().x(), getControlPoint()->getPoint().y() );
    m_pItem->setPen( QPen( Qt::NoPen ) );
    m_pItem->setBrush( QBrush( pointColour ) );
    m_pItem->setZValue( 2.0f );
    updateColours();
    m_map.insert( m_pItem, getControlPoint(), this );
    
    OnNewZoomLevel( fZoom );
}

GlyphControlPoint::~GlyphControlPoint()
{
    cleanUpItem( m_pItem, m_map, m_pGlyphSpec /*getControlPoint()*/, m_pScene );
}

void GlyphControlPoint::updateColours()
{
    m_pItem->setBrush( isSelected() ?    QBrush( Qt::green ) :
                                        QBrush( Qt::black ) );
}

void GlyphControlPoint::setSelected( bool bSelected )
{
    const bool bUpdate = isSelected() != bSelected;
    Selectable::setSelected( bSelected );
    if( bUpdate )
        updateColours();
}

void GlyphControlPoint::update()
{
    const map::ControlPoint* pControlPoint = getControlPoint();
    VERIFY_RTE( pControlPoint );
    if( pControlPoint )
    {
        m_pItem->setPos( pControlPoint->getPoint().x(), pControlPoint->getPoint().y() );
        m_pItem->setRect( -( m_fSize / 2.0f ), -( m_fSize / 2.0f ), m_fSize, m_fSize );
    }
}

void GlyphControlPoint::OnNewZoomLevel( float fZoom )
{
    m_fSize = m_fSizeScaling / fZoom;
    update();
    updateColours();
}

QGraphicsEllipseItem* GlyphControlPoint::getItem() const
{
    return m_pItem;
}

void GlyphControlPoint::setShouldRender( bool bShouldRender )
{
    m_pItem->setVisible( bShouldRender );
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
static QColor g_pathColor( 155,155,155,125 );
static float g_pathWidth( 4.0f );
static QColor g_polygonColor( 155,155,155,200 );
static float g_polygonWidth( 4.0f );

GlyphPolygonGroup::GlyphPolygonGroup( map::IGlyph::Ptr pParent, QGraphicsScene* pScene,
                      GlyphMap map, map::MarkupPolygonGroup* pPolygonGroup, 
                      float fZoom, 
                      Toolbox::Ptr pToolBoxPtr )
    :   map::GlyphPolygonGroup( pPolygonGroup, pParent ),
        m_pScene( pScene ),
        m_map( map ),
        m_pItem( 0u ),
        m_pToolBoxPtr( pToolBoxPtr )
{
    if( m_pToolBoxPtr )
    {
        m_pToolBoxPtr->getConfigValue( ".glyphs.polygons.colour", g_polygonColor );
        m_pToolBoxPtr->getConfigValue( ".glyphs.polygons.width", g_polygonWidth );
    }
    
    m_fSize = g_polygonWidth / fZoom;
    
    //convert to painter path
    getMarkupPolygonGroup()->paint( m_pathPainter );
    
    QGraphicsItem* pParentItem = m_map.findItem( getMarkupPolygonGroup()->getParent() );
    m_pItem = new QGraphicsPathItem( m_pathPainter.getPath(), pParentItem );
    if( !pParentItem ) m_pScene->addItem( m_pItem );
    m_pItem->setPen( QPen( QBrush( g_pathColor ), m_fSize, Qt::SolidLine ) );
    
    if( getMarkupPolygonGroup()->isPolygonsFilled() )
    {
        m_pItem->setBrush( QBrush( g_polygonColor ) );
    }
    else
    {
        m_pItem->setBrush( QBrush( Qt::NoBrush ) );
    }
    m_pItem->setZValue( 1.5f );
    m_map.insert( m_pItem, getMarkupPolygonGroup(), this );
}

GlyphPolygonGroup::~GlyphPolygonGroup()
{
    cleanUpItem( m_pItem, m_map, getMarkupPolygonGroup(), m_pScene );
}

void GlyphPolygonGroup::OnNewZoomLevel( float fZoom )
{
    m_fSize = g_polygonWidth / fZoom;
    m_pItem->setPen( QPen( QBrush( g_pathColor ), m_fSize, Qt::SolidLine ) );
}

void GlyphPolygonGroup::update()
{
    if( getMarkupPolygonGroup()->paint( m_pathPainter ) )
    {
        m_pItem->setPath( m_pathPainter.getPath() );
        m_pItem->setPen( QPen( QBrush( g_pathColor ), m_fSize, Qt::SolidLine ) );
        if( getMarkupPolygonGroup()->isPolygonsFilled() )
        {
            m_pItem->setBrush( QBrush( g_polygonColor ) );
        }
        else
        {
            m_pItem->setBrush( QBrush( Qt::NoBrush ) );
        }
    }
}

void GlyphPolygonGroup::setShouldRender( bool bShouldRender )
{
    m_pItem->setVisible( bShouldRender );
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
static QColor g_axisColorX( 255,0,0,255 );
static QColor g_axisColorY( 0,255,0,255 );
static QColor g_originActive( 100, 0, 0, 25 );
static QColor g_originSelected( 0, 255, 0, 25 );
static QColor g_originIdle( 0, 0, 255 , 25 );
static QColor g_originInvisible( 0, 0, 0, 0 );

GlyphOrigin::GlyphOrigin( map::IGlyph::Ptr pParent, QGraphicsScene* pScene,
                        GlyphMap map, map::Origin* pOrigin, 
                        map::IEditContext*& pActiveContext,
                        Toolbox::Ptr pToolBoxPtr )
    :   map::GlyphOrigin( pOrigin, pParent ),
        Selectable( calculateDepth( pOrigin ) ),
        m_pScene( pScene ),
        m_map( map ),
        m_pItemX( nullptr ),
        m_pItemY( nullptr ),
        m_pPathItem( nullptr ),
        m_pActiveContext( pActiveContext ),
        m_bActiveContext( false ),
        m_pToolBoxPtr( pToolBoxPtr )
{
    float fLength = 4.0f;
    float fWidth = 1.0f;
    if( m_pToolBoxPtr )
    {
        m_pToolBoxPtr->getConfigValue( ".glyphs.origin.colourx",   g_axisColorX );
        m_pToolBoxPtr->getConfigValue( ".glyphs.origin.coloury",   g_axisColorY );
        m_pToolBoxPtr->getConfigValue( ".glyphs.origin.length",    fLength );
        m_pToolBoxPtr->getConfigValue( ".glyphs.origin.width",     fWidth );
        m_pToolBoxPtr->getConfigValue( ".glyphs.origin.active",    g_originActive );
        m_pToolBoxPtr->getConfigValue( ".glyphs.origin.selected",  g_originSelected );
        m_pToolBoxPtr->getConfigValue( ".glyphs.origin.idle",      g_originIdle );
    }
    
    QGraphicsItem* pParentItem = m_map.findItem( getOrigin()->getParent() );
    {
        if( const map::MarkupPolygonGroup* pMarkupContour = 
            getOrigin()->getMarkupContour() )
        {
            pMarkupContour->paint( m_pathPainter );
            m_pPathItem = new QGraphicsPathItem( m_pathPainter.getPath(), pParentItem );
            if( !pParentItem ) m_pScene->addItem( m_pPathItem );
            m_pPathItem->setPen( QPen( Qt::NoPen ) );
            m_pPathItem->setZValue( 1.5f );
            m_map.insert( m_pPathItem, getOrigin(), this );
        }
    }
    
    {
        m_pItemX = new QGraphicsLineItem( 0.0f, 0.0f, fLength, 0.0f, pParentItem );
        if( !pParentItem ) m_pScene->addItem( m_pItemX );
        m_pItemX->setPen( QPen( QBrush( g_axisColorX ), fWidth, Qt::SolidLine ) );
        m_pItemX->setZValue( 3.0f );
    }
    {
        m_pItemY = new QGraphicsLineItem( 0.0f, 0.0f, 0.0f, fLength, pParentItem );
        if( !pParentItem ) m_pScene->addItem( m_pItemY );
        m_pItemY->setPen( QPen( QBrush( g_axisColorY ), fWidth, Qt::SolidLine ) );
        m_pItemY->setZValue( 3.0f );
    }
        
    updateColours();
}

GlyphOrigin::~GlyphOrigin()
{
    delete m_pItemX;
    delete m_pItemY;
    cleanUpItem( m_pPathItem, m_map, getOrigin(), m_pScene );
}

bool GlyphOrigin::isActiveContext() const
{
    return ( m_pActiveContext && m_pActiveContext->getOrigin() == getOrigin() ) ? true : false;
}

void GlyphOrigin::setSelected( bool bSelected )
{
    const bool bUpdate = 
        ( isSelected() != bSelected ) || 
        ( m_bActiveContext != isActiveContext() );
    Selectable::setSelected( bSelected );
    if( bUpdate )
    {
        m_bActiveContext = isActiveContext();
        updateColours();
    }
}

void GlyphOrigin::updateColours()
{                               
    if( isActiveContext() )
    {
        if( m_pPathItem )
        {
            if( !m_bShouldRender )
            {
                m_pPathItem->setBrush( QBrush( g_originInvisible ) );
            }
            else
            {
                m_pPathItem->setBrush( QBrush( g_originActive ) );
            }
        }
        m_pItemX->setVisible( true );
        m_pItemY->setVisible( true );
    }
    else if( isSelected() )
    {
        if( m_pPathItem )
        {
            if( !m_bShouldRender )
            {
                m_pPathItem->setBrush( QBrush( g_originInvisible ) );
            }
            else
            {
                m_pPathItem->setBrush( QBrush( g_originSelected ) );
            }
        }
        m_pItemX->setVisible( true );
        m_pItemY->setVisible( true );
    }
    else
    {
        if( m_pPathItem )
        {
            if( !m_bShouldRender )
            {
                m_pPathItem->setBrush( QBrush( g_originInvisible ) );
            }
            else
            {
                m_pPathItem->setBrush( QBrush( g_originIdle ) );
            }
        }
        m_pItemX->setVisible( false );
        m_pItemY->setVisible( false );
    }
    
    const map::Transform& originMatrix = getOrigin()->getTransform();
    const QTransform transform( 
        CGAL::to_double( originMatrix.m( 0, 0 ) ), 
        CGAL::to_double( originMatrix.m( 1, 0 ) ),
        CGAL::to_double( originMatrix.m( 0, 1 ) ),
        CGAL::to_double( originMatrix.m( 1, 1 ) ),
        CGAL::to_double( originMatrix.m( 0, 2 ) ),
        CGAL::to_double( originMatrix.m( 1, 2 ) ) );
    
    if( m_pPathItem )
    {
        m_pPathItem->setTransform( transform );
    }
    m_pItemX->setTransform( transform );
    m_pItemY->setTransform( transform );
}

void GlyphOrigin::update()
{
    if( m_pPathItem )
    {
        if( getOrigin()->getMarkupContour()->paint( m_pathPainter ) )
        {
            m_pPathItem->setPath( m_pathPainter.getPath() );
        }
    }
    
    updateColours();
}

void GlyphOrigin::setShouldRender( bool bShouldRender )
{
    m_bShouldRender = bShouldRender;
    updateColours();
    
    if( m_pPathItem )
    {
        //m_pPathItem->setVisible( bShouldRender );
    }
    //m_pItemX->setVisible( bShouldRender );
    //m_pItemY->setVisible( bShouldRender );
}

/*
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
QVector< QRgb > GlyphImage::m_coloursNormal;
QVector< QRgb > GlyphImage::m_coloursSelected;
QVector< QRgb > GlyphImage::m_coloursActiveContext;

GlyphImage::GlyphImage( map::IGlyph::Ptr pParent, QGraphicsScene* pScene,
                        GlyphMap map, map::ImageSpec* pImage, 
                        map::IEditContext*& pActiveContext, 
                        Toolbox::Ptr pToolBoxPtr )
    :   map::GlyphImage( pImage, pParent ),
        Selectable( calculateDepth( pImage ) ),
        m_pScene( pScene ),
        m_map( map ),
        m_pItem( 0u ),
        m_pActiveContext( pActiveContext ),
        m_bActiveContext( false ),
        m_pToolboxPtr( pToolBoxPtr )
{
    static bool bColourTablesInit = true;
    if( bColourTablesInit )
    {
        bColourTablesInit = false;
        m_coloursNormal.push_back( 0u );
        m_coloursNormal.push_back( 0xA03030A0 );
        m_coloursNormal.push_back( 0xA06F6FA0 );

        m_coloursSelected.push_back( 0u );
        m_coloursSelected.push_back( 0xA03030FF );
        m_coloursSelected.push_back( 0xA06F6FFF );

        m_coloursActiveContext.push_back( 0u );
        m_coloursActiveContext.push_back( 0x30A03000 );
        m_coloursActiveContext.push_back( 0x30FF6F00 );
    }

    if( map::NavBitmap::Ptr pBuffer = getImageSpec()->getBuffer() )
    {
        QImage m_image( pBuffer->get(),pBuffer->getWidth(),pBuffer->getHeight(),pBuffer->getStride(),QImage::Format_Indexed8 );
        m_image.setColorTable( m_coloursNormal );
        m_pixelMap = QPixmap::fromImage( m_image, Qt::ColorOnly );
        setOrCreateImageItem();
    }
}

GlyphImage::~GlyphImage()
{
    cleanUpItem( m_pItem, m_map, getImageSpec(), m_pScene );
}

bool GlyphImage::isActiveContext() const
{
    return ( m_pActiveContext && m_pActiveContext->getImageSpec() == getImageSpec() ) ? true : false;
}

void GlyphImage::setSelected( bool bSelected )
{
    const bool bUpdate = 
        ( isSelected() != bSelected ) || 
        ( m_bActiveContext != isActiveContext() );
    Selectable::setSelected( bSelected );
    if( bUpdate )
    {
        m_bActiveContext = isActiveContext();
        if( m_bActiveContext )
        {
            if( map::NavBitmap::Ptr pBuffer = getImageSpec()->getBuffer() )
            {
                QImage m_image( pBuffer->get(),pBuffer->getWidth(),pBuffer->getHeight(),pBuffer->getStride(),QImage::Format_Indexed8 );
                m_image.setColorTable( m_coloursActiveContext );
                m_pixelMap = QPixmap::fromImage( m_image, Qt::ColorOnly );
                setOrCreateImageItem();
            }
        }
        else if( isSelected() )
        {
            if( map::NavBitmap::Ptr pBuffer = getImageSpec()->getBuffer() )
            {
                QImage m_image( pBuffer->get(),pBuffer->getWidth(),pBuffer->getHeight(),pBuffer->getStride(),QImage::Format_Indexed8 );
                m_image.setColorTable( m_coloursSelected );
                m_pixelMap = QPixmap::fromImage( m_image, Qt::ColorOnly );
                setOrCreateImageItem();
            }
        }
        else
        {
            if( map::NavBitmap::Ptr pBuffer = getImageSpec()->getBuffer() )
            {
                QImage m_image( pBuffer->get(),pBuffer->getWidth(),pBuffer->getHeight(),pBuffer->getStride(),QImage::Format_Indexed8 );
                m_image.setColorTable( m_coloursNormal );
                m_pixelMap = QPixmap::fromImage( m_image, Qt::ColorOnly );
                setOrCreateImageItem();
            }
        }
    }
}

void GlyphImage::setOrCreateImageItem()
{
    if( !m_pItem )
    {
        QGraphicsItem* pParentItem = m_map.findItem( getImageSpec()->getParent() );
        m_pItem = new QGraphicsPixmapItem( m_pixelMap, pParentItem );
        if( !pParentItem ) m_pScene->addItem( m_pItem );
        m_map.insert( m_pItem, getImageSpec(), this );
        m_pItem->setZValue( 0.0f );
        m_pItem->setPos( getImageSpec()->getX(), getImageSpec()->getY() );
        m_pItem->setOffset( getImageSpec()->getOffsetX(), getImageSpec()->getOffsetY() );
    }
    else
        m_pItem->setPixmap( m_pixelMap );
}

void GlyphImage::update()
{
    if( map::NavBitmap::Ptr pBuffer = getImageSpec()->getBuffer() )
    {
        if( ( m_lastUpdateTick < pBuffer->getLastUpdateTick() ) || !m_pItem || 
            ( isActiveContext() != m_bActiveContext ) )
        {
            QImage m_image( pBuffer->get(), pBuffer->getWidth(),
                              pBuffer->getHeight(), pBuffer->getStride(), QImage::Format_Indexed8 );
            m_bActiveContext = isActiveContext();
            if( m_bActiveContext )
                m_image.setColorTable( m_coloursActiveContext );
            else if( isSelected() )
                m_image.setColorTable( m_coloursSelected );
            else
                m_image.setColorTable( m_coloursNormal );

            m_pixelMap.convertFromImage( m_image, Qt::ColorOnly );
            setOrCreateImageItem();
            m_lastUpdateTick.update();
        }
        m_pItem->setPos( getImageSpec()->getX(), getImageSpec()->getY() );
        m_pItem->setOffset( getImageSpec()->getOffsetX(), getImageSpec()->getOffsetY() );
    }
    //else if( m_pItem )
    //{
    //    cleanUpItem( m_pItem, m_map, getImageSpec(), m_pScene );
    //    m_pItem = 0u;
    //}
}

void GlyphImage::setShouldRender( bool bShouldRender )
{
    m_pItem->setVisible( bShouldRender );
}
*/
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
static QColor g_textColour( 0, 0, 0 );

GlyphText::GlyphText( map::IGlyph::Ptr pParent, QGraphicsScene* pScene, 
                     GlyphMap map, map::MarkupText* pText,
                     Toolbox::Ptr pToolBoxPtr )
    :   map::GlyphText( pText, pParent ),
        m_pScene( pScene ),
        m_map( map ),
        m_pItem( nullptr ),
        m_pToolBoxPtr( pToolBoxPtr )
{
    if( m_pToolBoxPtr )
    {
        m_pToolBoxPtr->getConfigValue( ".glyphs.text.colour", g_textColour );
    }
    
    //convert to painter path
    QGraphicsItem* pParentItem = m_map.findItem( getMarkupText()->getParent() );
    m_pItem = new QGraphicsSimpleTextItem( getMarkupText()->getText().c_str(), pParentItem );
    if( !pParentItem ) m_pScene->addItem( m_pItem );
    m_pItem->setZValue( 0.9f );
    m_pItem->setFlag( QGraphicsItem::ItemIgnoresTransformations );
    m_pItem->setPos( getMarkupText()->getPoint().x(), getMarkupText()->getPoint().y() );
    m_pItem->setBrush( QBrush( QColor( g_textColour ) ) );
    m_map.insert( m_pItem, pText, this );
}

GlyphText::~GlyphText()
{
    cleanUpItem( m_pItem, m_map, getMarkupText(), m_pScene );
}

void GlyphText::update()
{
    m_pItem->setText( getMarkupText()->getText().c_str() );
    m_pItem->setPos( getMarkupText()->getPoint().x(), getMarkupText()->getPoint().y() );
}

void GlyphText::setShouldRender( bool bShouldRender )
{
    m_pItem->setVisible( bShouldRender );
}

}


