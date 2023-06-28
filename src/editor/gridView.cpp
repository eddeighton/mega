
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
    : QGraphicsView( pParentWidget )
    , m_pMainWindow( pMainWindow )
{
    this->setTransform( QTransform::fromScale( 1.0f, -1.0f ) );

    m_pScene = new QGraphicsScene;
    setScene( m_pScene );
    CalculateOversizedSceneRect();
    setTransformationAnchor( AnchorUnderMouse );

    if( auto pToolBox = pMainWindow->getToolbox() )
    {
        pToolBox->getConfigValue( ".background.colour", m_bkgrnd );
        pToolBox->getConfigValue( ".background.step", m_iMainLineStep );
        pToolBox->getConfigValue( ".background.lines.width", m_lineWidth );
        pToolBox->getConfigValue( ".background.lines.main.colour", m_mainLineColour );
        pToolBox->getConfigValue( ".background.lines.other.colour", m_otherLineColour );
        pToolBox->getConfigValue( ".background.text.colour", m_textColor );
    }
}

GridView::~GridView() = default;

void GridView::postCreate( Document::Ptr pDocument )
{
    m_bInitialising = false;
    CalculateOversizedSceneRect();
}

void GridView::onViewFocussed()
{
    CMD_CONNECT( actionZoomToAll, CmdZoomToAll );
}

void GridView::onViewUnfocussed()
{
    CMD_DISCONNECT( actionZoomToAll, CmdZoomToAll );
}

QVector2D GridView::getZoomVector() const
{
    return QVector2D( this->transform().m11(), this->transform().m22() );
}

QVector2D GridView::getQuantisationLevel() const
{
    const QVector2D zoomLevel = getZoomVector();

    float fXStep = 1.0f;
    while( fXStep < fabs( m_iQuantisation / zoomLevel.x() ) )
        fXStep *= 2.0f;

    float fYStep = 1.0f;
    while( fYStep < fabs( m_iQuantisation / zoomLevel.y() ) )
        fYStep *= 2.0f;

    return QVector2D( fXStep, fYStep );
}

void GridView::SetZoom( QVector2D v2NewZoomLevel )
{
    setTransform( QTransform::fromScale( v2NewZoomLevel.x(), v2NewZoomLevel.y() ) );
    updateItemZoomLevels();
    CalculateRulerItems();
}

void GridView::DoZoom( float fAmt )
{
    SetZoom( getZoomVector() * fAmt );
}

void GridView::CmdZoomToAll()
{
    QRectF rect( 0, 0, 64, 64 );
    for( QGraphicsItem* pItem : items() )
    {
        // avoid including the ruler text
        if( !( dynamic_cast< QGraphicsSimpleTextItem* >( pItem ) ) )
        {
            rect = rect.united( pItem->sceneBoundingRect() );
        }
    }

    // this->sceneRect()
    //  TODO possibly constrain ZoomToAll to only the active context items...
    //  for( ItemMap::const_iterator i = m_itemMap.begin(),
    //       iEnd = m_itemMap.end(); i!=iEnd; ++i )
    //{
    //      if( !m_pActiveContext || m_pActiveContext->canEdit( i->second, m_pActiveTool->getToolType(), m_toolMode ) )
    //          rect = rect.united( i->first->sceneBoundingRect() );
    //  }

    auto oldAnchor = transformationAnchor();
    setTransformationAnchor( NoAnchor );
    fitInView( rect, Qt::KeepAspectRatio );
    setTransformationAnchor( oldAnchor );

    updateItemZoomLevels();
    CalculateRulerItems();
}

void GridView::CalculateOversizedSceneRect()
{
    QRectF sceneRect( 0.0f, 0.0f, 0.0f, 0.0f );
    for( QGraphicsItem* pItem : items() )
    {
        if( !( dynamic_cast< QGraphicsSimpleTextItem* >( pItem ) ) )
        {
            sceneRect = sceneRect.united( pItem->sceneBoundingRect() );
        }
    }
    const float f = 1024.0f;
    m_pScene->setSceneRect( QRectF(
        sceneRect.left() - f, sceneRect.top() - f, sceneRect.width() + f * 2.0f, sceneRect.height() + f * 2.0f ) );
}

void GridView::resizeEvent( QResizeEvent* pEvent )
{
    if( !m_bInitialising )
    {
        CalculateRulerItems();
    }

    QGraphicsView::resizeEvent( pEvent );
}

void GridView::drawBackground( QPainter* painter, const QRectF& rect )
{
    painter->fillRect( rect, m_bkgrnd );

    // m_bkgrndContext

    // grid lines
    QPen oldPen = painter->pen();
    {
        const QVector2D currentZoomLevel = getZoomVector();

        float fXStep = 1.0f;
        while( fXStep / 16.0f < fabs( 1 / currentZoomLevel.x() ) )
            fXStep *= 2.0f;
        float fYStep = 1.0f;
        while( fYStep / 16.0f < fabs( 1 / currentZoomLevel.y() ) )
            fYStep *= 2.0f;

        QPen gridPen;
        gridPen.setStyle( Qt::DotLine );
        gridPen.setWidth( m_lineWidth );
        painter->setPen( gridPen );

        const float fQuantLeft   = Math::quantize< float >( rect.left(), fXStep * m_iMainLineStep );
        const float fQuantRight  = Math::quantize< float >( rect.right(), fXStep ) + fXStep * m_iMainLineStep;
        const float fQuantTop    = Math::quantize< float >( rect.top(), fYStep * m_iMainLineStep );
        const float fQuantBottom = Math::quantize< float >( rect.bottom(), fYStep ) + fYStep * m_iMainLineStep;

        unsigned int uiX = 0u;
        for( float x = fQuantLeft; x <= fQuantRight; x += fXStep, ++uiX )
        {
            if( x == 0 )
            {
                gridPen.setStyle( Qt::SolidLine );
                gridPen.setColor( m_mainLineColour );
            }
            else
            {
                gridPen.setStyle( Qt::DotLine );
                if( uiX % m_iMainLineStep )
                {
                    gridPen.setColor( m_otherLineColour );
                }
                else
                {
                    gridPen.setColor( m_mainLineColour );
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
                gridPen.setColor( m_mainLineColour );
            }
            else
            {
                gridPen.setStyle( Qt::DotLine );
                if( uiY % m_iMainLineStep )
                    gridPen.setColor( m_otherLineColour );
                else
                    gridPen.setColor( m_mainLineColour );
            }
            painter->setPen( gridPen );
            painter->drawLine( QPoint( fQuantLeft, y ), QPoint( fQuantRight, y ) );
        }
    }
    painter->setPen( oldPen );
}

void GridView::CalculateRulerItems()
{
    const QVector2D currentZoomLevel = getZoomVector();
    const QRect     viewportRect     = viewport()->rect();
    const QRectF    rect( mapToScene( viewportRect.topLeft() ), mapToScene( viewportRect.bottomRight() ) );

    // if( m_pToolBox )
    //{
    //     m_pToolBox->getConfigValue( ".background.step", m_iMainLineStep );
    // }

    float fXStep = 1.0f;
    while( fXStep / 16.0f < fabs( m_iMainLineStep / currentZoomLevel.x() ) )
        fXStep *= 2.0f;
    float fYStep = 1.0f;
    while( fYStep / 16.0f < fabs( m_iMainLineStep / currentZoomLevel.y() ) )
        fYStep *= 2.0f;

    const float fOffsetX = 8.0f / currentZoomLevel.x();
    const float fOffsetY = 8.0f / currentZoomLevel.y();

    const float fQuantLeft   = Math::quantize< float >( rect.left(), fXStep );
    const float fQuantRight  = Math::quantize< float >( rect.right(), fXStep ) + fXStep;
    const float fQuantTop    = Math::quantize< float >( rect.top(), fYStep );
    const float fQuantBottom = Math::quantize< float >( rect.bottom(), fYStep ) + fYStep;

    // labels across the bottom
    {
        TextItemVector::iterator xIter = m_rulerVertItems.begin();
        for( float x = fQuantLeft; x <= fQuantRight; x += fXStep )
        {
            QGraphicsSimpleTextItem* pItem = 0u;
            std::ostringstream       os;
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
            pItem->setBrush( QBrush( m_textColor ) );
            pItem->setPos( x, rect.bottom() - fOffsetY * 2.0f );
        }
        for( TextItemVector::iterator x = xIter; x != m_rulerVertItems.end(); ++x )
        {
            m_pScene->removeItem( *x );
            delete *x;
        }
        m_rulerVertItems.erase( xIter, m_rulerVertItems.end() );
    }

    // labels down the left hand side
    {
        TextItemVector::iterator yIter = m_rulerHoriItems.begin();
        for( float y = fQuantBottom; y <= fQuantTop; y += fYStep )
        {
            QGraphicsSimpleTextItem* pItem = 0u;
            std::ostringstream       os;
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
            pItem->setBrush( QBrush( m_textColor ) );
            pItem->setPos( rect.left() + fOffsetX, y - fOffsetY );
        }
        for( TextItemVector::iterator y = yIter; y != m_rulerHoriItems.end(); ++y )
        {
            m_pScene->removeItem( *y );
            delete *y;
        }
        m_rulerHoriItems.erase( yIter, m_rulerHoriItems.end() );
    }
}

void GridView::mousePressEvent( QMouseEvent* pEvent )
{
    if( pEvent->button() == Qt::MiddleButton || pEvent->button() == Qt::ForwardButton
        || pEvent->button() == Qt::ExtraButton1 || pEvent->button() == Qt::BackButton )
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
            horizontalScrollBar()->setValue( m_pScrollData->m_downScrollPos.x() + delta.x() );
            verticalScrollBar()->setValue( m_pScrollData->m_downScrollPos.y() + delta.y() );
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

    if( isPanning() )
    {
        // ignor wheel while panning
        return;
    }

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

} // namespace editor