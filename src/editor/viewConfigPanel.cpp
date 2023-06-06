
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

#include "viewConfigPanel.hpp"

namespace editor
{

ViewConfigPanel::ViewConfigPanel( QWidget* pParentWidget )
    : QGraphicsView( pParentWidget )
    , m_pMainWindow( MainWindow::getSingleton() )
    , m_pToolBox( m_pMainWindow->getToolbox() )
{
    VERIFY_RTE( pParentWidget );
    VERIFY_RTE( m_pToolBox );

    m_pScene = new QGraphicsScene;
    setScene( m_pScene );

    if( auto pToolBox = m_pMainWindow->getToolbox() )
    {
        pToolBox->getConfigValue( ".background.colour", m_bkgrnd );
        pToolBox->getConfigValue( ".background.step", m_iMainLineStep );
        pToolBox->getConfigValue( ".background.lines.width", m_lineWidth );
        pToolBox->getConfigValue( ".background.lines.main.colour", m_mainLineColour );
        pToolBox->getConfigValue( ".background.lines.other.colour", m_otherLineColour );
        pToolBox->getConfigValue( ".background.text.colour", m_textColor );
    }
}

void ViewConfigPanel::setViewConfig( editor::ViewConfig::Ptr pViewConfig )
{
    m_pConfig = pViewConfig;
}

void ViewConfigPanel::drawBackground( QPainter* painter, const QRectF& rect )
{
    painter->fillRect( rect, m_bkgrnd );
    QPen oldPen = painter->pen();
    {
        const QVector2D currentZoomLevel( 1.0f, 1.0f );

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

void ViewConfigPanel::resizeEvent( QResizeEvent* pEvent )
{
}

void ViewConfigPanel::mousePressEvent( QMouseEvent* pEvent )
{
}

void ViewConfigPanel::mouseMoveEvent( QMouseEvent* pEvent )
{
}

void ViewConfigPanel::mouseReleaseEvent( QMouseEvent* pEvent )
{
}

} // namespace editor