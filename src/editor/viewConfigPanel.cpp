
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

    if( auto pToolBox = m_pMainWindow->getToolbox() )
    {
        pToolBox->getConfigValue( ".background.colour", m_bkgrnd );

        pToolBox->getConfigValue( ".config.selection.colour", m_selectionColour );
        pToolBox->getConfigValue( ".config.scene.size", m_fSize );
        pToolBox->getConfigValue( ".config.text.colour", m_textColour );
        pToolBox->getConfigValue( ".config.text.font", m_strFont );
        pToolBox->getConfigValue( ".config.text.fontsize", m_fontSize );
        pToolBox->getConfigValue( ".config.grid.colour", m_gridColour );
        pToolBox->getConfigValue( ".config.grid.width", m_gridWidth );
    }

    m_font = QFont( m_strFont.c_str(), m_fontSize, QFont::Bold, false );

    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    m_pScene = new QGraphicsScene;
    setScene( m_pScene );

    m_pScene->setSceneRect( QRectF( 0.0f, 0.0f, m_fSize, m_fSize ) );
    fitInView( m_pScene->sceneRect(), Qt::IgnoreAspectRatio );
}

static QString glyphStrings[]
    = { QString{ "Text" }, QString{ "Points" }, QString{ "Sites" }, QString{ "Connections" } };

void ViewConfigPanel::setViewConfig( editor::ViewConfig::Ptr pViewConfig )
{
    auto resetRows = [ this ]( RowVector& rows )
    {
        for( auto& row : rows )
        {
            m_pScene->removeItem( row.pLabel );
            m_pScene->removeItem( row.pSetting );
        }
        rows.clear();
    };
    resetRows( m_glyphRows );
    resetRows( m_maskRows );

    m_pConfig = pViewConfig;

    const QRectF rect = m_pScene->sceneRect();

    static const std::size_t szTotalItems = ViewConfig::TOTAL_GLYPH_SETTINGS + exact::EdgeMask::TOTAL_MASK_TYPES;
    float                    fRectHeight  = rect.height() / szTotalItems;
    float                    fRectWidth   = rect.width() / 4;

    if( m_pConfig )
    {
        for( auto i = 0U; i != ViewConfig::TOTAL_GLYPH_SETTINGS; ++i )
        {
            const bool bVisible = m_pConfig->isGlyphVisible( static_cast< ViewConfig::GlyphVisibility >( i ) );

            int column = bVisible;

            auto pLabel = new QGraphicsSimpleTextItem( glyphStrings[ i ] );
            pLabel->setFont( m_font );
            pLabel->setBrush( QBrush( m_textColour ) );
            pLabel->setFlag( QGraphicsItem::ItemIgnoresTransformations );
            pLabel->setPos( rect.left(), rect.top() + fRectHeight * i );

            auto pRect = new QGraphicsRectItem( QRectF(
                rect.left() + column * fRectWidth * 2, rect.top() + fRectHeight * i, fRectWidth * 2, fRectHeight ) );
            pRect->setPen( Qt::NoPen );
            pRect->setBrush( QBrush( m_selectionColour ) );

            m_glyphRows.emplace_back( Row{ pLabel, pRect } );
            m_pScene->addItem( m_glyphRows.back().pLabel );
            m_pScene->addItem( m_glyphRows.back().pSetting );
        }

        for( auto i = 0U; i != exact::EdgeMask::TOTAL_MASK_TYPES; ++i )
        {
            auto edgeMask = static_cast< exact::EdgeMask::Type >( i );

            auto pLabel = new QGraphicsSimpleTextItem( QString( exact::EdgeMask::toStr( edgeMask ) ) );
            pLabel->setFont( m_font );
            pLabel->setBrush( QBrush( m_textColour ) );
            pLabel->setFlag( QGraphicsItem::ItemIgnoresTransformations );
            pLabel->setPos( rect.left(), rect.top() + fRectHeight * ( i + ViewConfig::TOTAL_GLYPH_SETTINGS ) );

            int column = m_pConfig->getMaskSetting( edgeMask );

            auto pRect = new QGraphicsRectItem( QRectF(
                rect.left() + column * fRectWidth, rect.top() + fRectHeight * ( i + ViewConfig::TOTAL_GLYPH_SETTINGS ),
                fRectWidth, fRectHeight ) );
            pRect->setPen( Qt::NoPen );
            pRect->setBrush( QBrush( m_selectionColour ) );

            m_maskRows.emplace_back( Row{ pLabel, pRect } );
            m_pScene->addItem( m_maskRows.back().pLabel );
            m_pScene->addItem( m_maskRows.back().pSetting );
        }
    }
}

void ViewConfigPanel::drawBackground( QPainter* painter, const QRectF& rect )
{
    painter->fillRect( rect, m_bkgrnd );
    QPen oldPen = painter->pen();
    {
        const QRectF rect = m_pScene->sceneRect();

        static const std::size_t szTotalItems = ViewConfig::TOTAL_GLYPH_SETTINGS + exact::EdgeMask::TOTAL_MASK_TYPES;
        float                    fRectHeight  = rect.height() / szTotalItems;
        float                    fRectWidth   = rect.width() / 4;

        QPen gridPen;
        gridPen.setStyle( Qt::DotLine );
        gridPen.setWidth( m_gridWidth );
        gridPen.setColor( m_gridColour );
        painter->setPen( gridPen );

        for( float x = rect.left(); x < rect.right(); x += fRectWidth )
        {
            painter->drawLine( QPoint( x, rect.top() ), QPoint( x, rect.bottom() ) );
        }

        for( float y = rect.top(); y < rect.bottom(); y += fRectHeight )
        {
            painter->drawLine( QPoint( rect.left(), y ), QPoint( rect.right(), y ) );
        }
    }
    painter->setPen( oldPen );
}

void ViewConfigPanel::resizeEvent( QResizeEvent* pEvent )
{
    fitInView( m_pScene->sceneRect(), Qt::IgnoreAspectRatio );
    QGraphicsView::resizeEvent( pEvent );
}

bool ViewConfigPanel::setConfig( int iRow, int iCol )
{
    const QRectF             rect         = m_pScene->sceneRect();
    static const std::size_t szTotalItems = ViewConfig::TOTAL_GLYPH_SETTINGS + exact::EdgeMask::TOTAL_MASK_TYPES;
    float                    fRectHeight  = rect.height() / szTotalItems;
    float                    fRectWidth   = rect.width() / 4;

    if( iCol >= 0 && iCol < 4 )
    {
        if( iRow < ViewConfig::TOTAL_GLYPH_SETTINGS )
        {
            const auto setting   = static_cast< ViewConfig::GlyphVisibility >( iRow );
            bool       bNewValue = iCol >= 2;
            if( m_glyphRows.size() > setting )
            {
                if( m_pConfig->isGlyphVisible( setting ) != bNewValue )
                {
                    if( bNewValue )
                    {
                        m_pConfig->setGlyphVisible( setting );
                    }
                    else
                    {
                        m_pConfig->resetGlyphVisible( setting );
                    }

                    m_glyphRows[ setting ].pSetting->setRect(
                        QRectF( rect.left() + ( bNewValue ? ( fRectWidth * 2.0f ) : 0.0f ),
                                rect.top() + fRectHeight * iRow, fRectWidth * 2, fRectHeight ) );
                    return true;
                }
            }
        }
        else if( ( iRow - ViewConfig::TOTAL_GLYPH_SETTINGS ) < exact::EdgeMask::TOTAL_MASK_TYPES )
        {
            const auto maskType = static_cast< exact::EdgeMask::Type >( iRow - ViewConfig::TOTAL_GLYPH_SETTINGS );
            if( m_maskRows.size() > maskType )
            {
                if( m_pConfig->getMaskSetting( maskType ) != iCol )
                {
                    m_pConfig->setMaskSetting( maskType, static_cast< ViewConfig::MaskSetting >( iCol ) );

                    m_maskRows[ maskType ].pSetting->setRect( QRectF(
                        rect.left() + iCol * fRectWidth, rect.top() + fRectHeight * iRow, fRectWidth, fRectHeight ) );
                    return true;
                }
            }
        }
    }
    return false;
}

void ViewConfigPanel::mousePressEvent( QMouseEvent* pEvent )
{
    QGraphicsView::mousePressEvent( pEvent );

    m_mouseDownColumn.reset();

    if( pEvent->button() == Qt::LeftButton )
    {
        if( m_pConfig )
        {
            const QRectF  rect = m_pScene->sceneRect();
            const QPointF pos  = mapToScene( pEvent->pos() );

            static const std::size_t szTotalItems
                = ViewConfig::TOTAL_GLYPH_SETTINGS + exact::EdgeMask::TOTAL_MASK_TYPES;
            float fRectHeight = rect.height() / szTotalItems;
            float fRectWidth  = rect.width() / 4;

            const int iRow = ( pos.y() - rect.top() ) / fRectHeight;
            const int iCol = ( pos.x() - rect.left() ) / fRectWidth;

            if( setConfig( iRow, iCol ) )
            {
                m_mouseDownColumn = iCol;
                OnViewConfigModified();
            }
        }
    }
}

void ViewConfigPanel::mouseMoveEvent( QMouseEvent* pEvent )
{
    QGraphicsView::mouseMoveEvent( pEvent );

    if( m_mouseDownColumn.has_value() )
    {
        const QRectF  rect = m_pScene->sceneRect();
        const QPointF pos  = mapToScene( pEvent->pos() );

        static const std::size_t szTotalItems = ViewConfig::TOTAL_GLYPH_SETTINGS + exact::EdgeMask::TOTAL_MASK_TYPES;
        float                    fRectHeight  = rect.height() / szTotalItems;
        const int                iRow         = ( pos.y() - rect.top() ) / fRectHeight;

        if( setConfig( iRow, m_mouseDownColumn.value() ) )
        {
            OnViewConfigModified();
        }
    }
}

void ViewConfigPanel::mouseReleaseEvent( QMouseEvent* pEvent )
{
    QGraphicsView::mouseReleaseEvent( pEvent );
    m_mouseDownColumn.reset();
}

} // namespace editor