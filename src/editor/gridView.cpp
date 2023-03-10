
#include "gridView.hpp"

#include "mainWindow.hpp"
#include "ui_mainWindow.h"

#include <QGraphicsSceneWheelEvent>
#include <QScrollBar>
#include <QDebug>

#ifndef Q_MOC_RUN
#include "qtUtils.hpp"
#include "common/math.hpp"
#include "common/rounding.hpp"
#endif

namespace editor
{

GridView::GridView( QWidget* pParentWidget, MainWindow* pMainWindow )
:   QGraphicsView( pParentWidget ),
    m_pMainWindow( pMainWindow )
{
    m_pScene = new QGraphicsScene;
    setScene( m_pScene );
    CalculateOversizedSceneRect();
}

GridView::~GridView()
{

}

void GridView::postCreate( Document::Ptr pDocument )
{
    m_bInitialising = false;
    
    CalculateOversizedSceneRect();
    CmdZoomToAll();
    update();
    invalidateScene();
}

void GridView::onViewFocussed()
{
    CMD_CONNECT( actionZoomToAll, CmdZoomToAll );
}

void GridView::onViewUnfocussed()
{
    CMD_DISCONNECT( actionZoomToAll, CmdZoomToAll );
}

QVector2D GridView::getZoomLevel() const 
{ 
    return QVector2D( this->transform().m11(), this->transform().m22() ); 
}

QVector2D GridView::getQuantisationLevel() const
{
    const QVector2D zoomLevel = getZoomLevel();
        
    float fXStep = 1.0f;
    while( fXStep < ( m_iQuantisation / zoomLevel.x() ) )
        fXStep *= 2.0f;
        
    float fYStep = 1.0f;
    while( fYStep < ( m_iQuantisation / zoomLevel.y() ) )
        fYStep *= 2.0f;

    return QVector2D( fXStep, fYStep );
}

void GridView::SetZoom( QVector2D v2NewZoomLevel )
{
    setTransform( QTransform::fromScale( v2NewZoomLevel.x(), v2NewZoomLevel.y() ) );
    
    onZoomed();

    CalculateRulerItems();
}

void GridView::DoZoom( float fAmt )
{
    ViewportAnchor oldAnchor = transformationAnchor();
    setTransformationAnchor( AnchorUnderMouse );

    const QVector2D currentZoomLevel = getZoomLevel();
    SetZoom( QVector2D( currentZoomLevel.y() * fAmt, currentZoomLevel.y() * fAmt  ) );

    setTransformationAnchor( oldAnchor );
}

void GridView::CmdZoomToAll()
{
    QRectF rect( -m_fDefaultZoom / 2.0f, -m_fDefaultZoom / 2.0f, m_fDefaultZoom, m_fDefaultZoom );
    
    QList< QGraphicsItem* > allItems = items();
    for( QList< QGraphicsItem* >::iterator i = allItems.begin(),
         iEnd = allItems.end(); i!=iEnd; ++i )
        rect = rect.united( ( *i )->boundingRect() );

    //TODO possibly constrain ZoomToAll to only the active context items...
    //for( ItemMap::const_iterator i = m_itemMap.begin(),
    //     iEnd = m_itemMap.end(); i!=iEnd; ++i )
    //{
    //    if( !m_pActiveContext || m_pActiveContext->canEdit( i->second, m_pActiveTool->getToolType(), m_toolMode ) )
    //        rect = rect.united( i->first->sceneBoundingRect() );
    //}

    fitInView( rect, Qt::KeepAspectRatio );
    DoZoom( 1.0f );
    
    CalculateRulerItems();
}

void GridView::CalculateOversizedSceneRect()
{
    QRectF sceneRect( 0.0f,0.0f,0.0f,0.0f );
    QList< QGraphicsItem* > allItems = items();
    for( QList< QGraphicsItem* >::iterator i = allItems.begin(),
         iEnd = allItems.end(); i!=iEnd; ++i )
        sceneRect = sceneRect.united( ( *i )->boundingRect() );
    const float f = 1024.0f;
    m_pScene->setSceneRect(
                QRectF( sceneRect.left() - f, sceneRect.top() - f,
                        sceneRect.width() + f*2.0f, sceneRect.height() + f*2.0f ) );
}

void GridView::resizeEvent( QResizeEvent* pEvent )
{
    if( !m_bInitialising )
    {
        //qDebug() << "GridView::resizeEvent init=false";
        CalculateRulerItems();
    }
    else
    {
        //qDebug() << "GridView::resizeEvent init=true";
    }
    
    QGraphicsView::resizeEvent( pEvent );
}

void GridView::drawBackground( QPainter *painter, const QRectF &rect )
{
    const QVector2D currentZoomLevel = getZoomLevel();

    float fXStep = 1.0f;
    while( fXStep / 16.0f < ( 1 / currentZoomLevel.x() ) )
        fXStep *= 2.0f;
    float fYStep = 1.0f;
    while( fYStep / 16.0f < ( 1 / currentZoomLevel.y() ) )
        fYStep *= 2.0f;

    QColor mainLineColour( 100, 100, 100, 125 );
    QColor otherLineColour( 200, 200, 200, 125 );
    QColor bkgrnd( 255, 255, 255 );
    
    int iMainLineStep = 4;
    float lineWidth = 0.5f;
    /*if( m_pToolBox )
    {
        m_pToolBox->getConfigValue( ".background.colour", bkgrnd );
        m_pToolBox->getConfigValue( ".background.step", iMainLineStep );
        m_pToolBox->getConfigValue( ".background.lines.width", lineWidth );
        m_pToolBox->getConfigValue( ".background.lines.main.colour", mainLineColour );
        m_pToolBox->getConfigValue( ".background.lines.other.colour", otherLineColour );
    }*/
    
    painter->fillRect( rect, bkgrnd );

    //grid lines
    QPen oldPen = painter->pen();
    {
        QPen gridPen;
        gridPen.setStyle( Qt::DotLine );
        gridPen.setWidth( lineWidth );
        painter->setPen( gridPen );

        const float fQuantLeft    =   Math::quantize< float >( rect.left(),      fXStep * iMainLineStep );
        const float fQuantRight   =   Math::quantize< float >( rect.right(),     fXStep ) + fXStep * iMainLineStep;
        const float fQuantTop     =   Math::quantize< float >( rect.top(),       fYStep * iMainLineStep );
        const float fQuantBottom  =   Math::quantize< float >( rect.bottom(),    fYStep ) + fYStep * iMainLineStep;

        unsigned int uiX = 0u;
        for( float x = fQuantLeft; x <= fQuantRight; x += fXStep, ++uiX )
        {
            if( x == 0 )
            {
                gridPen.setStyle( Qt::SolidLine );
                gridPen.setColor( mainLineColour );
            }
            else 
            {
                gridPen.setStyle( Qt::DotLine );
                if( uiX % iMainLineStep )
                {
                    gridPen.setColor( otherLineColour );
                }
                else
                {
                    gridPen.setColor( mainLineColour );
                }
            }
            painter->setPen( gridPen );
            painter->drawLine( QPoint( x, fQuantTop ), QPoint( x, fQuantBottom ) );
        }
        unsigned int uiY = 0u;
        for( float y = fQuantTop; y <= fQuantBottom; y += fYStep, ++uiY )
        {
            if( y == 0 )
            {
                gridPen.setStyle( Qt::SolidLine );
                gridPen.setColor( mainLineColour );
            }
            else
            {
                gridPen.setStyle( Qt::DotLine );
                if( uiY % iMainLineStep )
                    gridPen.setColor( otherLineColour );
                else
                    gridPen.setColor( mainLineColour );
            }
            painter->setPen( gridPen );
            painter->drawLine( QPoint( fQuantLeft, y ), QPoint( fQuantRight, y ) );
        }
    }
    painter->setPen( oldPen );
}

void GridView::CalculateRulerItems()
{
    const QVector2D currentZoomLevel = getZoomLevel();
    const QRect viewportRect = viewport()->rect();
    const QRectF rect( mapToScene( viewportRect.topLeft() ), mapToScene( viewportRect.bottomRight() ) );

    int iMainLineStep = 4;
    /*if( m_pToolBox )
    {
        m_pToolBox->getConfigValue( ".background.step", iMainLineStep );
    }*/
        
    float fXStep = 1.0f;
    while( fXStep / 16.0f < ( iMainLineStep / currentZoomLevel.x() ) )
        fXStep *= 2.0f;
    float fYStep = 1.0f;
    while( fYStep / 16.0f < ( iMainLineStep / currentZoomLevel.y() ) )
        fYStep *= 2.0f;

    const float fOffsetX = 8.0f / currentZoomLevel.x();
    const float fOffsetY = 8.0f / currentZoomLevel.y();

    QColor textColor( 0,0,0,255 );
    
    /*if( m_pToolBox )
    {
        m_pToolBox->getConfigValue( ".background.text.colour", textColor );
    }*/

    const float fQuantLeft    =   Math::quantize< float >( rect.left(),      fXStep );
    const float fQuantRight   =   Math::quantize< float >( rect.right(),     fXStep ) + fXStep;
    const float fQuantTop     =   Math::quantize< float >( rect.top(),       fYStep );
    const float fQuantBottom  =   Math::quantize< float >( rect.bottom(),    fYStep ) + fYStep;

    //labels across the bottom
    {
        TextItemVector::iterator xIter = m_rulerVertItems.begin();
        for( float x = fQuantLeft; x <= fQuantRight; x += fXStep )
        {
            QGraphicsSimpleTextItem* pItem = 0u;
            std::ostringstream os;
            os << x;
            if( xIter != m_rulerVertItems.end() )
            {
                pItem = *xIter;
                ++xIter;
            }
            else
            {
                pItem = m_pScene->addSimpleText( os.str().c_str() );
                pItem->setFlag( QGraphicsItem::ItemIgnoresTransformations );
                pItem->setZValue( 0.0f );
                m_rulerVertItems.push_back( pItem );
                xIter = m_rulerVertItems.end();
            }
            pItem->setText( os.str().c_str() );
            pItem->setBrush( QBrush( textColor ) );
            pItem->setPos( x, rect.bottom() - fOffsetY * 2.0f );
        }
        for( TextItemVector::iterator x = xIter ;x != m_rulerVertItems.end(); ++x )
        {
            m_pScene->removeItem( *x );
            delete *x;
        }
        m_rulerVertItems.erase( xIter, m_rulerVertItems.end() );
    }

    //labels down the left hand side
    {
        TextItemVector::iterator yIter = m_rulerHoriItems.begin();
        for( float y = fQuantTop; y <= fQuantBottom; y += fYStep )
        {
            QGraphicsSimpleTextItem* pItem = 0u;
            std::ostringstream os;
            os << y;
            if( yIter != m_rulerHoriItems.end() )
            {
                pItem = *yIter;
                ++yIter;
            }
            else
            {
                pItem = m_pScene->addSimpleText( os.str().c_str() );
                pItem->setFlag( QGraphicsItem::ItemIgnoresTransformations );
                pItem->setZValue( 0.0f );
                m_rulerHoriItems.push_back( pItem );
                yIter = m_rulerHoriItems.end();
            }
            pItem->setText( os.str().c_str() );
            pItem->setBrush( QBrush( textColor ) );
            pItem->setPos( rect.left() + fOffsetX, y - fOffsetY );
        }
        for( TextItemVector::iterator y = yIter ;y != m_rulerHoriItems.end(); ++y )
        {
            m_pScene->removeItem( *y );
            delete *y;
        }
        m_rulerHoriItems.erase( yIter, m_rulerHoriItems.end() );
    }
}

void GridView::mousePressEvent( QMouseEvent* pEvent )
{
    if( pEvent->button() == Qt::MiddleButton )
    {
        setCursor( Qt::ClosedHandCursor );
        QPointF scrollPositions( horizontalScrollBar()->value(), verticalScrollBar()->value() );
        m_pScrollData.reset( new ScrollData( pEvent->pos(), scrollPositions ) );
    }
    else
    {
        m_pScrollData.reset();
    }
    
    QGraphicsView::mousePressEvent( pEvent );
    
}
void GridView::mouseMoveEvent( QMouseEvent* pEvent )
{
    if( isPanning() )
    {
        setCursor( Qt::ClosedHandCursor );
        if( m_pScrollData.get() )
        {
            const QPointF delta = m_pScrollData->m_downPos - pEvent->pos();
            horizontalScrollBar()->setValue(    m_pScrollData->m_downScrollPos.x() + delta.x() );
            verticalScrollBar()->setValue(      m_pScrollData->m_downScrollPos.y() + delta.y() );
        }
    }
    
    QGraphicsView::mouseMoveEvent( pEvent );
}

void GridView::mouseReleaseEvent( QMouseEvent* pEvent )
{
    if( isPanning() )
    {
        m_pScrollData.reset();
        setCursor( Qt::ArrowCursor );
    }

    QGraphicsView::mouseReleaseEvent( pEvent );
}

void GridView::scrollContentsBy( int dx, int dy )
{
    QGraphicsView::scrollContentsBy( dx, dy );
    
    if( !m_bInitialising )
    {
        CalculateRulerItems();
    }
}

void GridView::wheelEvent( QWheelEvent* pEvent )
{
    static const float m_fZoomRate = 0.1f;
    
    if( pEvent->angleDelta().y() > 0 )
    {
        DoZoom( 1.0f + m_fZoomRate );
    }
    else if( pEvent->angleDelta().y() < 0 )
    {
        DoZoom( 1.0f - m_fZoomRate );
    }
    else
    {
        QGraphicsView::wheelEvent( pEvent );
    }
}


}