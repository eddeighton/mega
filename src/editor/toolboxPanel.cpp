
#include "toolboxPanel.hpp"

#include <QLabel>
#include <QMenu>

#include "mainWindow.hpp"

#ifndef Q_MOC_RUN

#include "common/assert_verify.hpp"
#include "common/stl.hpp"
#include "common/compose.hpp"
#include "common/math.hpp"
#include "common/rounding.hpp"

#include "boost/filesystem.hpp"

#include <functional>

#endif

namespace editor
{

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
ClipScene::ClipScene( QWidget* parent )
    : QGraphicsScene( parent )
    , m_pNullContext( 0u )
{
}

void ClipScene::setClip( schematic::Schematic::Ptr pSchematic, Toolbox::Ptr pToolBox )
{
    m_pToolBox = pToolBox;

    float fScaling = 1.0f;
    if( m_pToolBox )
    {
        m_pToolBox->getConfigValue( ".toolbox.item.width", m_fDeviceWidth );
        m_pToolBox->getConfigValue( ".toolbox.item.scaling", fScaling );
    }

    m_fDeviceWidth *= fScaling; // artificially increase so that control points are smaller

    if( pSchematic )
    {
        m_pEdit.reset();
        VERIFY_RTE( m_itemMap.empty() );
        VERIFY_RTE( m_specMap.empty() );

        m_pSchematic = pSchematic;
        m_pSchematic->init();
        m_pEdit.reset( new schematic::EditSchematic( *this, m_pSchematic ) );
        m_pEdit->updateGlyphs();
    }
}

void ClipScene::calculateSceneRect()
{
    QRectF rect;
    for( ItemMap::const_iterator i = m_itemMap.begin(), iEnd = m_itemMap.end(); i != iEnd; ++i )
    {
        rect = rect.united( i->first->sceneBoundingRect() );
    }
    rect = rect.marginsAdded( QMargins( 2, 2, 2, 2 ) );
    setSceneRect( rect );
}

// glyph factory interface
schematic::IGlyph::Ptr ClipScene::createControlPoint( schematic::ControlPoint* pControlPoint,
                                                      schematic::IGlyph::Ptr   pParent )
{
    schematic::IGlyph::Ptr pNewGlyph( new GlyphControlPoint( pParent, this, GlyphMap( m_itemMap, m_specMap ),
                                                             pControlPoint, m_fDeviceWidth / this->sceneRect().height(),
                                                             m_pToolBox ) );
    if( Renderable* pRenderable = dynamic_cast< Renderable* >( pNewGlyph.get() ) )
        pRenderable->setShouldRender( false );
    return pNewGlyph;
}

schematic::IGlyph::Ptr ClipScene::createOrigin( schematic::Origin* pOrigin, schematic::IGlyph::Ptr pParent )
{
    schematic::IGlyph::Ptr pNewGlyph(
        new GlyphOrigin( pParent, this, GlyphMap( m_itemMap, m_specMap ), pOrigin, m_pNullContext, m_pToolBox ) );
    return pNewGlyph;
}

schematic::IGlyph::Ptr ClipScene::createMarkupPolygonGroup( schematic::MarkupPolygonGroup* pMarkupPolygonGroup,
                                                            schematic::IGlyph::Ptr         pParent )
{
    schematic::IGlyph::Ptr pNewGlyph(
        new GlyphPolygonGroup( pParent, this, GlyphMap( m_itemMap, m_specMap ), pMarkupPolygonGroup, nullptr,
                               m_fDeviceWidth / this->sceneRect().height(), m_pToolBox ) );
    return pNewGlyph;
}

schematic::IGlyph::Ptr ClipScene::createMarkupText( schematic::MarkupText* pMarkupText, schematic::IGlyph::Ptr pParent )
{
    schematic::IGlyph::Ptr pNewGlyph(
        new GlyphText( pParent, this, GlyphMap( m_itemMap, m_specMap ), pMarkupText, m_pToolBox ) );
    if( Renderable* pRenderable = dynamic_cast< Renderable* >( pNewGlyph.get() ) )
        pRenderable->setShouldRender( false );
    return pNewGlyph;
}

schematic::IGlyph::Ptr ClipScene::createImage( schematic::ImageSpec* pImage, schematic::IGlyph::Ptr pParent )
{
    schematic::IGlyph::Ptr pNewGlyph(
        new GlyphImage( pParent, this, GlyphMap( m_itemMap, m_specMap ), pImage, m_pToolBox ) );
    if( Renderable* pRenderable = dynamic_cast< Renderable* >( pNewGlyph.get() ) )
        pRenderable->setShouldRender( false );
    return pNewGlyph;
}

void ClipScene::onEditted( bool bCommandCompleted )
{
}

void ClipScene::drawBackground( QPainter*, const QRectF& )
{
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
FlowView::FlowItem::FlowItem( FlowView& view, schematic::Schematic::Ptr pSchematic, Toolbox::Palette::Ptr pPalette )
    : m_view( view )
    , m_pSchematic( pSchematic )
    , m_pPalette( pPalette )
{
    QColor      bkgrnd( 255, 255, 255 );
    int         itemWidth  = 256;
    int         itemHeight = 256;
    QColor      textColour( 0, 0, 0 );
    std::string strFont  = "Times";
    int         fontSize = 8;

    Toolbox::Ptr pToolBox = view.getToolbox();
    if( pToolBox )
    {
        pToolBox->getConfigValue( ".toolbox.item.background.colour", bkgrnd );
        pToolBox->getConfigValue( ".toolbox.item.width", itemWidth );
        pToolBox->getConfigValue( ".toolbox.item.height", itemHeight );
        pToolBox->getConfigValue( ".toolbox.item.text.colour", textColour );
        pToolBox->getConfigValue( ".toolbox.item.text.font", strFont );
        pToolBox->getConfigValue( ".toolbox.item.text.fontsize", fontSize );
    }

    QPixmap buffer( itemWidth, itemHeight );
    {
        ClipScene tempScene;
        tempScene.setClip( pSchematic, m_view.getToolbox() );

        QPainter painter;
        // painter.setRenderHint( QPainter::HighQualityAntialiasing, true );

        painter.begin( &buffer );
        {
            // fill the square
            {
                painter.fillRect( 0, 0, itemWidth, itemHeight, bkgrnd );
            }

            // paint the clip scene
            {
                tempScene.calculateSceneRect();
                tempScene.render( &painter );
            }

            // render useful text over the top
            {
                painter.setPen( textColour );

                static QFont newFont( strFont.c_str(), fontSize, QFont::Bold, false );
                painter.setFont( newFont );

                std::string strName = pSchematic->schematic::Node::getName();

                // skip everything up to last '/' or '\'
                {
                    auto i = strName.find_last_of( '/' );
                    if( i != std::string::npos )
                        strName = strName.substr( i + 1 );
                    i = strName.find_last_of( '\\' );
                    if( i != std::string::npos )
                        strName = strName.substr( i + 1 );

                    // remove extension
                    strName = strName.substr( 0, strName.find_last_of( '.' ) );
                }
                for( auto& c : strName )
                {
                    if( c == '_' )
                        c = ' ';
                }

                QRectF bounds( 0, 0, itemWidth, itemHeight );
                painter.drawText( bounds, Qt::AlignCenter | Qt::TextWordWrap, strName.c_str() ); //
            }
        }

        painter.end();
    }

    m_pImageItem = new QGraphicsPixmapItem( buffer );
    m_pImageItem->setZValue( 0.0f );
    m_view.scene()->addItem( m_pImageItem );
}
bool FlowView::FlowItem::onHit( QGraphicsItem* pItem )
{
    bool bHit = false;
    if( m_pImageItem == pItem )
    {
        m_pPalette->select( m_pSchematic );
        m_view.OnClipboardAction();
        bHit = true;
    }
    return bHit;
}
void FlowView::FlowItem::updatePosition( int x, int y, float fWidth, float fHeight, float fSpacing )
{
    QRectF bounds = m_pImageItem->boundingRect();
    if( bounds.isNull() || bounds.isEmpty() )
        bounds = QRectF( 0, 0, 1, 1 );

    m_pImageItem->setPos( x * ( fWidth + fSpacing ), y * ( fHeight + fSpacing ) );
    m_pImageItem->setTransform( QTransform::fromScale( fWidth / bounds.width(), fHeight / bounds.height() ) );
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
static float  g_fItemWidth      = 256.0f;
static float  g_fItemHeight     = 256.0f;
static float  g_fItemSpacing    = 8.0f;
static float  g_fSelectionWidth = 2.0f;
static QColor g_selectionColour( 0, 0, 255 );
static QColor g_selectionBackground( 0, 0, 150, 100 );
FlowView::FlowView( MissionToolbox& toolbox, Toolbox::Palette::Ptr pPalette )
    : QGraphicsView( &toolbox )
    , m_toolBox( toolbox )
    , m_pPalette( pPalette )
{
    setScene( new QGraphicsScene( this ) );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setTransformationAnchor( QGraphicsView::AnchorViewCenter );

    Toolbox::Ptr pToolBox = toolbox.getToolbox();
    if( pToolBox )
    {
        pToolBox->getConfigValue( ".toolbox.item.width", g_fItemWidth );
        pToolBox->getConfigValue( ".toolbox.item.height", g_fItemHeight );
        pToolBox->getConfigValue( ".toolbox.item.spacing", g_fItemSpacing );
        pToolBox->getConfigValue( ".toolbox.selection.width", g_fSelectionWidth );
        pToolBox->getConfigValue( ".toolbox.selection.colour", g_selectionColour );
        pToolBox->getConfigValue( ".toolbox.selection.background", g_selectionBackground );
    }

    QPen selectionPen( Qt::SolidLine );
    selectionPen.setWidth( g_fSelectionWidth );
    selectionPen.setBrush( g_selectionColour );
    m_pSelectionRectItem = scene()->addRect( QRect( 0, 0, 0, 0 ), selectionPen, QBrush( g_selectionBackground ) );
    m_pSelectionRectItem->setZValue( 1.0f );
    m_pSelectionRectItem->setVisible( false );

    updateClips();
}

FlowView::~FlowView()
{
}

Toolbox::Ptr FlowView::getToolbox() const
{
    return m_toolBox.getToolbox();
}

void FlowView::updateClips()
{
    schematic::Schematic::PtrSet clips( m_pPalette->get().begin(), m_pPalette->get().end() );

    ItemMap                      removals;
    schematic::Schematic::PtrSet additions;
    generics::match(
        m_items.begin(), m_items.end(), clips.begin(), clips.end(),
        generics::lessthan( generics::first< ItemMap::const_iterator >(),
                            generics::deref< schematic::Schematic::PtrSet::const_iterator >() ),
        generics::collect( removals, generics::deref< ItemMap::const_iterator >() ),
        generics::collect( additions, generics::deref< schematic::Schematic::PtrSet::const_iterator >() ) );

    for( ItemMap::iterator i = removals.begin(), iEnd = removals.end(); i != iEnd; ++i )
        m_items.erase( i->first );

    removals.clear();

    for( schematic::Schematic::PtrSet::iterator i = additions.begin(), iEnd = additions.end(); i != iEnd; ++i )
    {
        schematic::Schematic::Ptr pSchematic = *i;
        m_items.insert( std::make_pair( pSchematic, FlowItem::Ptr( new FlowItem( *this, pSchematic, m_pPalette ) ) ) );
    }

    calculateLayout();
}

void FlowView::clear()
{
    m_items.clear();
}

int FlowView::findIndex( schematic::Schematic::Ptr pSite )
{
    schematic::Schematic::PtrList::const_iterator iFind
        = std::find( m_pPalette->get().begin(), m_pPalette->get().end(), pSite );
    if( iFind != m_pPalette->get().end() )
        return std::distance( m_pPalette->get().begin(), iFind );
    else
        return -1;
}

void FlowView::calculateLayout()
{
    // determine the indices for each element
    FlowItem::Ptr       pSelectedItem;
    FlowItem::PtrVector items( m_items.size() );
    for( ItemMap::iterator i = m_items.begin(), iEnd = m_items.end(); i != iEnd; ++i )
    {
        const int iIndex = findIndex( i->first );
        if( iIndex >= 0 )
        {
            items[ iIndex ] = i->second;
            if( i->second->getClip() == m_pPalette->getSelection() )
                pSelectedItem = i->second;
        }
    }

    // decide the stride
    int iStride = std::max( 1.0f, viewport()->rect().width() / g_fItemWidth );

    // now calculate the positions and sizes
    FlowItem::PtrVector::iterator i = items.begin();
    int                           y = 0;
    for( ; i != items.end(); ++y )
    {
        for( int x = 0; x < iStride && i != items.end(); ++x, ++i )
            ( *i )->updatePosition( x, y, g_fItemWidth, g_fItemHeight, g_fItemSpacing );
    }

    setSceneRect( 0.0f, 0.0f, iStride * ( g_fItemWidth + g_fItemSpacing ), y * ( g_fItemHeight + g_fItemSpacing ) );

    if( pSelectedItem )
    {
        m_pSelectionRectItem->setRect( pSelectedItem->getRect() );
        m_pSelectionRectItem->setVisible( true );
    }
    else
    {
        m_pSelectionRectItem->setVisible( false );
    }
}

void FlowView::resizeEvent( QResizeEvent* pEvent )
{
    QGraphicsView::resizeEvent( pEvent );
    calculateLayout();
}

schematic::Schematic::Ptr FlowView::getClickedSchematic( QMouseEvent* pEvent )
{
    schematic::Schematic::Ptr pSchematic;
    {
        QGraphicsPixmapItem* pImageItem = nullptr;

        QList< QGraphicsItem* > stack = items( pEvent->pos() );
        for( QList< QGraphicsItem* >::iterator i = stack.begin(), iEnd = stack.end(); i != iEnd; ++i )
        {
            if( pImageItem = dynamic_cast< QGraphicsPixmapItem* >( *i ) )
                break;
        }

        if( pImageItem )
        {
            for( ItemMap::iterator i = m_items.begin(), iEnd = m_items.end(); i != iEnd; ++i )
            {
                if( i->second->onHit( pImageItem ) )
                {
                    pSchematic = i->second->getClip();
                    break;
                }
            }
        }
    }
    return pSchematic;
}

void FlowView::mouseDoubleClickEvent( QMouseEvent* pEvent )
{
    QGraphicsView::mouseDoubleClickEvent( pEvent );

    if( schematic::Schematic::Ptr pSchematic = getClickedSchematic( pEvent ) )
    {
        const boost::filesystem::path filePath = pSchematic->getName();
        if( boost::filesystem::exists( filePath ) )
        {
            OnQuickLoad( ClipboardMsg( pSchematic, m_pPalette, filePath ) );
        }
        else
        {
            OnQuickLoad( ClipboardMsg( pSchematic, m_pPalette ) );
        }
    }
}

void FlowView::mousePressEvent( QMouseEvent* pEvent )
{
    QGraphicsView::mousePressEvent( pEvent );

    if( schematic::Schematic::Ptr pSchematic = getClickedSchematic( pEvent ) )
    {
        if( pEvent->button() == Qt::RightButton )
        {
            // see if the name is the file path?
            const boost::filesystem::path filePath = pSchematic->getName();
            if( boost::filesystem::exists( filePath ) )
            {
                OnMenu( ClipboardMsg( pSchematic, m_pPalette, filePath ) );
            }
            else
            {
                OnMenu( ClipboardMsg( pSchematic, m_pPalette ) );
            }
        }
    }
}

void FlowView::mouseMoveEvent( QMouseEvent* pEvent )
{
    QGraphicsView::mouseMoveEvent( pEvent );
}

void FlowView::mouseReleaseEvent( QMouseEvent* pEvent )
{
    QGraphicsView::mouseReleaseEvent( pEvent );
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
MissionToolbox::MissionToolbox( QWidget* pParentWidget )
    : QToolBox( pParentWidget )
    , m_pMainWindow( MainWindow::getSingleton() )
    , m_pToolBox( m_pMainWindow->getToolbox() )
{
    VERIFY_RTE( m_pMainWindow );
    VERIFY_RTE( m_pToolBox );
    updateToolbox();
    onCurrentPaletteChanged( 0 );
}

MissionToolbox::ToolboxPanel::ToolboxPanel( MissionToolbox& toolbox, Toolbox::Palette::Ptr pPalette,
                                            unsigned int uiIndex )
    : m_toolbox( toolbox )
    , m_pPalette( pPalette )
    , m_strName( pPalette->getName() )
{
    m_pView                    = new FlowView( m_toolbox, pPalette );
    unsigned int uiActualIndex = m_toolbox.insertItem( uiIndex, m_pView, m_strName.c_str() );
    m_toolbox.setItemEnabled( uiActualIndex, true );
    updateClips();

    QObject::connect( m_pView, &FlowView::OnClipboardAction, &m_toolbox, &MissionToolbox::updateToolbox );
    qRegisterMetaType< ClipboardMsg >();
    QObject::connect( m_pView, &FlowView::OnMenu, &m_toolbox, &MissionToolbox::OnPaletteMenu, Qt::QueuedConnection );
    QObject::connect( m_pView, &FlowView::OnQuickLoad, &m_toolbox, &MissionToolbox::OnQuickLoad, Qt::QueuedConnection );
}

MissionToolbox::ToolboxPanel::~ToolboxPanel()
{
    m_pView->clear();
    const int iIndex = m_toolbox.indexOf( m_pView );
    m_toolbox.removeItem( iIndex );
    delete m_pView;
}

void MissionToolbox::ToolboxPanel::updateClips()
{
    m_pView->updateClips();
    m_updateTick.update();
}

struct CompareUpdateTimes
{
    inline bool operator()( MissionToolbox::PanelMap::const_iterator i1,
                            Toolbox::Palette::PtrMap::const_iterator i2 ) const
    {
        return i1->second->getUpdateTick() < i2->second->getLastModifiedTick();
    }
};

void MissionToolbox::updateToolbox()
{
    ASSERT( m_pToolBox );

    if( m_pToolBox )
    {
        typedef Toolbox::Palette::PtrMap ClipMap;
        const ClipMap&                   tools = m_pToolBox->get();

        PanelMap removals, updates;
        ClipMap  additions;
        generics::matchGetUpdates( m_panels.begin(), m_panels.end(), tools.begin(), tools.end(),
                                   generics::lessthan( generics::first< PanelMap::const_iterator >(),
                                                       generics::first< ClipMap::const_iterator >() ),
                                   CompareUpdateTimes(),
                                   generics::collect( removals, generics::deref< PanelMap::const_iterator >() ),
                                   generics::collect( additions, generics::deref< ClipMap::const_iterator >() ),
                                   generics::collect( updates, generics::deref< PanelMap::const_iterator >() ) );

        for( PanelMap::iterator i = removals.begin(), iEnd = removals.end(); i != iEnd; ++i )
        {
            // remove the panel
            m_panels.erase( i->first );
        }
        removals.clear();
        for( ClipMap::iterator i = additions.begin(), iEnd = additions.end(); i != iEnd; ++i )
        {
            // add new panel
            unsigned int uiIndex = 0u;
            for( ClipMap::const_iterator j = tools.begin(), jEnd = tools.end(); j != jEnd; ++j, ++uiIndex )
            {
                if( j->first == i->first )
                    break;
            }

            ToolboxPanel::Ptr pNewPanel( new ToolboxPanel( *this, i->second, uiIndex ) );
            m_panels.insert( std::make_pair( i->first, pNewPanel ) );
        }

        // typedef std::map< std::string, ToolboxPanel::Ptr > PanelMap;
        for( auto& i : updates )
        {
            i.second->updateClips();
        }
    }
}

void MissionToolbox::onCurrentPaletteChanged( int index )
{
    if( FlowView* pView = dynamic_cast< FlowView* >( widget( index ) ) )
    {
        if( m_pToolBox )
            m_pToolBox->selectPalette( pView->getPalette() );
    }
}

void MissionToolbox::OnQuickLoad( ClipboardMsg msg )
{
    m_pMainWindow->OnLoadSchematic( msg );
}

void MissionToolbox::OnPaletteMenu( ClipboardMsg msg )
{
    // qDebug() << "MissionToolbox::OnPaletteMenu()";

    QMenu menu( this );

    QObject::connect( menu.addAction( "Load" ), &QAction::triggered,
                      std::function< void() >( std::bind( &MainWindow::OnLoadSchematic, m_pMainWindow, msg ) ) );

    QObject::connect( menu.addAction( "Delete" ), &QAction::triggered,
                      std::function< void() >( std::bind( &MissionToolbox::On_DeleteClip, this, msg ) ) );

    QObject::connect( menu.addAction( "Rescan Toolbox" ), SIGNAL( triggered() ), this, SLOT( On_RescanToolBoxDir() ) );

    menu.exec( QCursor::pos() );
}

void MissionToolbox::On_DeleteClip( ClipboardMsg msg )
{
    msg.pPalette->remove( msg.pSchematic );
    updateToolbox();
}

void MissionToolbox::On_RescanToolBoxDir()
{
    m_panels.clear();
    m_pToolBox->reload();
    updateToolbox();
}

} // namespace editor