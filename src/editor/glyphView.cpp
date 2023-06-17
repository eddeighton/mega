
#include "glyphView.hpp"

#include "mainWindow.hpp"
#include "ui_mainWindow.h"

#include <QMessageBox>

#ifndef Q_MOC_RUN

#include "qtUtils.hpp"

#include "schematic/connection.hpp"
#include "schematic/cut.hpp"
#include "schematic/space.hpp"
#include "schematic/wall.hpp"
#include "schematic/object.hpp"

#endif

namespace editor
{

GlyphView::GlyphView( QWidget* pParent, MainWindow* pMainWindow )
    : GridView( pParent, pMainWindow )
    , m_selectionModel( &m_itemModel )
    , m_selectTool( *this )
    , m_lassoTool( *this )
    , m_penTool( *this )
    , m_editTool( *this )
    , m_pActiveTool( &m_selectTool )
    , m_pViewConfig( new ViewConfig{} )
    , m_compilationConfig( schematic::File::getDefaultCompilationConfig() )
{
    QObject::connect(
        &m_selectionModel, &SelectionModel::currentChanged, this, &GlyphView::OnCurrentSelectionItemChanged );
    QObject::connect( &m_selectionModel, &SelectionModel::selectionChanged, this, &GlyphView::OnSelectionChanged );
}

GlyphView::~GlyphView() = default;

void GlyphView::OnCurrentSelectionItemChanged( const QModelIndex&, const QModelIndex& )
{
}

Selectable* GlyphView::selectableFromNode( schematic::Node::PtrCst pNode ) const
{
    Selectable* pSelectable = nullptr;

    if( schematic::Site::PtrCst pSite = boost::dynamic_pointer_cast< const schematic::Site >( pNode ) )
    {
        if( auto pSpec = dynamic_cast< const schematic::GlyphSpec* >( pSite.get() ) )
        {
            auto iFind = m_specMap.find( pSpec );
            if( iFind != m_specMap.end() )
            {
                auto iFind2 = m_itemMap.find( iFind->second );
                if( iFind2 != m_itemMap.end() )
                {
                    pSelectable = Selection::glyphToSelectable( iFind2->second );
                }
            }
        }
    }
    return pSelectable;
}

void GlyphView::OnSelectionChanged( const QItemSelection& selected, const QItemSelection& deselected )
{
    const QModelIndexList selectedIndices = selected.indexes();
    for( QModelIndexList::const_iterator i = selectedIndices.begin(), iEnd = selectedIndices.end(); i != iEnd; ++i )
    {
        if( Selectable* pSelectable = selectableFromNode( m_itemModel.getIndexNode( *i ) ) )
            pSelectable->setSelected( true );
    }
    const QModelIndexList deselectedIndices = deselected.indexes();
    for( QModelIndexList::const_iterator i = deselectedIndices.begin(), iEnd = deselectedIndices.end(); i != iEnd; ++i )
    {
        if( Selectable* pSelectable = selectableFromNode( m_itemModel.getIndexNode( *i ) ) )
            pSelectable->setSelected( false );
    }
}

void GlyphView::postCreate( Document::Ptr pDocument )
{
    m_pDocument = pDocument;
    VERIFY_RTE( m_pDocument );

    GridView::postCreate( pDocument );
}

void GlyphView::onViewFocussed()
{
    GridView::onViewFocussed();

    CMD_CONNECT( actionTabOut, CmdTabOut );
    CMD_CONNECT( actionSelectAll, CmdSelectAll );
    CMD_CONNECT( actionCut, CmdCut );
    CMD_CONNECT( actionCopy, CmdCopy );
    CMD_CONNECT( actionPaste, CmdPaste );
    CMD_CONNECT( actionDelete, CmdDelete );
    CMD_CONNECT( actionRotate_Left, CmdRotateLeft );
    CMD_CONNECT( actionRotate_Right, CmdRotateRight );
    CMD_CONNECT( actionFlip_Horizontal, CmdFlipHorizontal );
    CMD_CONNECT( actionFlip_Vertical, CmdFlipVerical );
    CMD_CONNECT( actionShrink, CmdShrink );
    CMD_CONNECT( actionExtrude, CmdExtrude );
    CMD_CONNECT( actionUnion, CmdUnion );
    CMD_CONNECT( actionFilter, CmdFilter );
    CMD_CONNECT( actionAABB, CmdAABB );
    CMD_CONNECT( actionConvexHull, CmdConvexHull );
    CMD_CONNECT( actionReparent, CmdReparent );
    CMD_CONNECT( actionSelect, CmdSelectTool );
    CMD_CONNECT( actionLasso, CmdLassoTool );
    CMD_CONNECT( actionDraw, CmdDrawTool );
    CMD_CONNECT( actionEdit, CmdEditTool );

    m_pMainWindow->getUI()->treeView->setModel( &m_itemModel );
    m_pMainWindow->getUI()->treeView->setSelectionModel( &m_selectionModel );
    m_pMainWindow->getUI()->treeView->setActiveGlyphView( this );

    m_pMainWindow->getUI()->config->setViewConfig( m_pViewConfig );

    QObject::connect(
        m_pMainWindow->getUI()->config, &ViewConfigPanel::OnViewConfigModified, this, &GlyphView::OnViewConfigChanged );

    if( m_pMainWindow->getUI()->actionSelect->isChecked() )
    {
        m_pActiveTool = &m_selectTool;
    }
    else if( m_pMainWindow->getUI()->actionLasso->isChecked() )
    {
        m_pActiveTool = &m_lassoTool;
    }
    else if( m_pMainWindow->getUI()->actionDraw->isChecked() )
    {
        m_pActiveTool = &m_penTool;
    }
    else if( m_pMainWindow->getUI()->actionEdit->isChecked() )
    {
        m_pActiveTool = &m_editTool;
    }
    else
    {
        m_pMainWindow->getUI()->actionSelect->setChecked( true );
        m_pActiveTool = &m_selectTool;
    }
}

void GlyphView::onViewUnfocussed()
{
    GridView::onViewUnfocussed();

    QObject::disconnect(
        m_pMainWindow->getUI()->config, &ViewConfigPanel::OnViewConfigModified, this, &GlyphView::OnViewConfigChanged );

    CMD_DISCONNECT( actionTabOut, CmdTabOut );
    CMD_DISCONNECT( actionSelectAll, CmdSelectAll );
    CMD_DISCONNECT( actionCut, CmdCut );
    CMD_DISCONNECT( actionCopy, CmdCopy );
    CMD_DISCONNECT( actionPaste, CmdPaste );
    CMD_DISCONNECT( actionDelete, CmdDelete );
    CMD_DISCONNECT( actionRotate_Left, CmdRotateLeft );
    CMD_DISCONNECT( actionRotate_Right, CmdRotateRight );
    CMD_DISCONNECT( actionFlip_Horizontal, CmdFlipHorizontal );
    CMD_DISCONNECT( actionFlip_Vertical, CmdFlipVerical );
    CMD_DISCONNECT( actionShrink, CmdShrink );
    CMD_DISCONNECT( actionExtrude, CmdExtrude );
    CMD_DISCONNECT( actionUnion, CmdUnion );
    CMD_DISCONNECT( actionFilter, CmdFilter );
    CMD_DISCONNECT( actionAABB, CmdAABB );
    CMD_DISCONNECT( actionConvexHull, CmdConvexHull );
    CMD_DISCONNECT( actionReparent, CmdReparent );
    CMD_DISCONNECT( actionSelect, CmdSelectTool );
    CMD_DISCONNECT( actionLasso, CmdLassoTool );
    CMD_DISCONNECT( actionDraw, CmdDrawTool );
    CMD_DISCONNECT( actionEdit, CmdEditTool );

    m_pMainWindow->getUI()->treeView->setModel( nullptr );
    m_pMainWindow->getUI()->treeView->setActiveGlyphView( nullptr );

    m_pMainWindow->getUI()->config->setViewConfig( {} );
}

void GlyphView::onZoomed()
{
    const float currentZoomLevel = getZoomLevel();
    for( auto i = m_itemMap.begin(), iEnd = m_itemMap.end(); i != iEnd; ++i )
    {
        if( auto pZoomItem = dynamic_cast< ZoomDependent* >( i->second ) )
            pZoomItem->OnNewZoomLevel( currentZoomLevel );
    }
}

// glyph factory interface
schematic::IGlyph::Ptr GlyphView::createControlPoint( schematic::ControlPoint* pControlPoint,
                                                      schematic::IGlyph::Ptr   pParent )
{
    schematic::IGlyph::Ptr pNewGlyph( new GlyphControlPoint(
        pParent, m_pScene, GlyphMap( m_itemMap, m_specMap ), pControlPoint, getZoomLevel(), getToolbox() ) );
    CalculateOversizedSceneRect();
    return pNewGlyph;
}

schematic::IGlyph::Ptr GlyphView::createOrigin( schematic::Origin* pOrigin, schematic::IGlyph::Ptr pParent )
{
    schematic::IGlyph::Ptr pNewGlyph( new GlyphOrigin(
        pParent, m_pScene, GlyphMap( m_itemMap, m_specMap ), pOrigin, m_pActiveContext, getToolbox() ) );
    return pNewGlyph;
}

schematic::IGlyph::Ptr GlyphView::createMarkupPolygonGroup( schematic::MarkupPolygonGroup* pMarkupPolygonGroup,
                                                            schematic::IGlyph::Ptr         pParent )
{
    schematic::IGlyph::Ptr pNewGlyph( new GlyphPolygonGroup( pParent, m_pScene, GlyphMap( m_itemMap, m_specMap ),
                                                             pMarkupPolygonGroup, m_pViewConfig.get(), getZoomLevel(),
                                                             getToolbox() ) );
    return pNewGlyph;
}

schematic::IGlyph::Ptr GlyphView::createMarkupText( schematic::MarkupText* pMarkupText, schematic::IGlyph::Ptr pParent )
{
    schematic::IGlyph::Ptr pNewGlyph(
        new GlyphText( pParent, m_pScene, GlyphMap( m_itemMap, m_specMap ), pMarkupText, getToolbox() ) );
    return pNewGlyph;
}

void GlyphView::onEditted( bool bCommandCompleted )
{
    m_pDocument->onEditted( bCommandCompleted );
    m_itemModel.OnSchematicUpdate( m_pActiveContext );
}

void GlyphView::onDocumentUpdate()
{
    updateGlyphVisibility();
    m_pActiveTool->onUpdate();
}

void GlyphView::OnViewConfigChanged()
{
    onDocumentUpdate();

    // force update on all glyphs
    for( auto& [ pItem, pGlyph ] : m_itemMap )
    {
        if( auto* pConfigurable = dynamic_cast< Configurable* >( pGlyph ) )
        {
            pConfigurable->forceUpdate();
        }
        else
        {
            pGlyph->update();
        }
    }
}

void GlyphView::updateGlyphVisibility()
{
    const bool bShowText        = m_pViewConfig->isGlyphVisible( ViewConfig::eGlyphVis_Text );
    const bool bShowPoints      = m_pViewConfig->isGlyphVisible( ViewConfig::eGlyphVis_Points );
    const bool bShowSites       = m_pViewConfig->isGlyphVisible( ViewConfig::eGlyphVis_Sites );
    const bool bShowConnections = m_pViewConfig->isGlyphVisible( ViewConfig::eGlyphVis_Connections );

    VERIFY_RTE( m_pActiveContext );
    const schematic::Origin* pEditContextOrigin = m_pActiveContext->getOrigin();
    auto                     testGlyphOrigin    = [ bShowSites, bShowConnections, pEditContextOrigin ](
                               const GlyphOrigin* pOrigin, bool onlyLines = false ) -> bool
    {
        const schematic::Origin* pOriginSpec = pOrigin->getOrigin();

        // if the site is not within the active edit context then do not display it
        if( pEditContextOrigin && !onlyLines )
        {
            bool                        bIsChildOfActiveContext = false;
            const schematic::GlyphSpec* pIter                   = pOriginSpec;
            while( pIter )
            {
                if( pIter == pEditContextOrigin )
                {
                    bIsChildOfActiveContext = true;
                    break;
                }
                pIter = pIter->getParent();
            }
            if( !bIsChildOfActiveContext )
            {
                return false;
            }
        }

        if( dynamic_cast< const schematic::Connection* >( pOriginSpec ) )
        {
            if( !bShowConnections )
                return false;
        }
        else if( dynamic_cast< const schematic::Cut* >( pOriginSpec ) )
        {
            if( !bShowConnections )
                return false;
        }
        else if( dynamic_cast< const schematic::Space* >( pOriginSpec ) )
        {
            if( !bShowSites )
                return false;
        }
        else if( dynamic_cast< const schematic::Wall* >( pOriginSpec ) )
        {
            if( !bShowSites )
                return false;
        }
        else if( dynamic_cast< const schematic::Object* >( pOriginSpec ) )
        {
            if( !bShowSites )
                return false;
        }
        else
        {
            THROW_RTE( "Unknown origin type" );
        }
        return true;
    };

    for( auto i : m_itemMap )
    {
        schematic::IGlyph* pGlyph = i.second;

        if( auto pRenderable = dynamic_cast< Renderable* >( pGlyph ) )
        {
            bool bShowType = true;

            if( dynamic_cast< const GlyphText* >( pGlyph ) )
            {
                if( !bShowText )
                {
                    bShowType = false;
                }
                else
                {
                    if( auto pOrigin = dynamic_cast< const GlyphOrigin* >( pGlyph->getParent().get() ) )
                    {
                        if( !testGlyphOrigin( pOrigin ) )
                            bShowType = false;
                    }
                }
            }
            else if( dynamic_cast< const GlyphControlPoint* >( pGlyph ) )
            {
                if( !bShowPoints )
                {
                    bShowType = false;
                }
                else
                {
                    if( auto pOrigin = dynamic_cast< const GlyphOrigin* >( pGlyph->getParent().get() ) )
                    {
                        if( !testGlyphOrigin( pOrigin ) )
                            bShowType = false;
                    }
                }
            }
            else if( auto pOrigin = dynamic_cast< const GlyphOrigin* >( pGlyph ) )
            {
                if( !testGlyphOrigin( pOrigin ) )
                    bShowType = false;
            }
            else if( dynamic_cast< const GlyphPolygonGroup* >( pGlyph ) )
            {
                if( auto pOrigin = dynamic_cast< const GlyphOrigin* >( pGlyph->getParent().get() ) )
                {
                    if( !testGlyphOrigin( pOrigin, true ) )
                        bShowType = false;
                }
            }

            bool bIsCompiled = false;
            if( auto pSpec = pGlyph->getGlyphSpec() )
            {
                const auto compilationStage = pSpec->getCompilationStage();
                if( compilationStage >= 0 && compilationStage <= m_compilationConfig )
                {
                    bIsCompiled = true;
                }
            }

            pRenderable->setShouldRender( bIsCompiled && bShowType );
        }
    }
}

// context handling
void GlyphView::selectContext( schematic::IEditContext* pNewContext )
{
    setSelected( SelectionSet() );

    schematic::IEditContext* pOldContext = m_pActiveContext;
    m_pActiveContext                     = pNewContext;

    updateGlyphVisibility();

    // just update everything
    for( auto& [ pItem, pGlyph ] : m_itemMap )
    {
        pGlyph->update();
    }

    m_pActiveTool->onUpdate();

    m_itemModel.OnSchematicUpdate( m_pActiveContext );
}

// selection handling
schematic::Schematic::Ptr GlyphView::getCurrentClip() const
{
    return m_pMainWindow->getToolbox()->getCurrentItem();
}

Toolbox::Ptr GlyphView::getToolbox() const
{
    return m_pMainWindow->getToolbox();
}

SelectionSet GlyphView::getSelection() const
{
    SelectionSet selection;
    for( auto i = m_itemMap.begin(), iEnd = m_itemMap.end(); i != iEnd; ++i )
    {
        if( Selectable* pSelectable = Selection::glyphToSelectable( i->second ) )
        {
            if( pSelectable->isSelected() )
                selection.insert( i->second );
        }
    }
    return selection;
}

SelectionSet GlyphView::getSelectedByRect( const QRectF& rect ) const
{
    // ASSERT( m_pActiveTool );
    SelectionSet            selection;
    QList< QGraphicsItem* > stack = items( mapFromScene( rect ) );
    for( QList< QGraphicsItem* >::iterator i = stack.begin(), iEnd = stack.end(); i != iEnd; ++i )
    {
        if( schematic::IGlyph* pTest = findGlyph( *i ) )
        {
            if( m_pActiveContext->canEdit( pTest, m_pActiveTool->getToolType(), m_pActiveTool->getToolMode(),
                                           m_pViewConfig->isGlyphVisible( ViewConfig::eGlyphVis_Points ),
                                           m_pViewConfig->isGlyphVisible( ViewConfig::eGlyphVis_Sites ),
                                           m_pViewConfig->isGlyphVisible( ViewConfig::eGlyphVis_Connections )

                                               ) )
            {
                if( Selectable* pSelectable = Selection::glyphToSelectable( pTest ) )
                {
                    // const bool bIsCustomTool = m_pActiveTool->getToolType() != schematic::IEditContext::eSelect &&
                    //     m_pActiveTool->getToolType() != schematic::IEditContext::eDraw;
                    // if( ( !bIsCustomTool && pSelectable->isImage() ) ||
                    //     ( bIsCustomTool && !pSelectable->isImage() ) )
                    selection.insert( pTest );
                }
            }
        }
    }
    return selection;
}

SelectionSet GlyphView::getSelectedByPath( const QPainterPath& path ) const
{
    // ASSERT( m_pActiveTool );
    SelectionSet            selection;
    QList< QGraphicsItem* > stack = items( mapFromScene( path ) );
    for( QList< QGraphicsItem* >::iterator i = stack.begin(), iEnd = stack.end(); i != iEnd; ++i )
    {
        if( schematic::IGlyph* pTest = findGlyph( *i ) )
        {
            if( m_pActiveContext->canEdit( pTest, m_pActiveTool->getToolType(), m_pActiveTool->getToolMode(),
                                           m_pViewConfig->isGlyphVisible( ViewConfig::eGlyphVis_Points ),
                                           m_pViewConfig->isGlyphVisible( ViewConfig::eGlyphVis_Sites ),
                                           m_pViewConfig->isGlyphVisible( ViewConfig::eGlyphVis_Connections )

                                               ) )
            {
                if( Selectable* pSelectable = Selection::glyphToSelectable( pTest ) )
                {
                    // if( !pSelectable->isImage() )
                    selection.insert( pTest );
                }
            }
        }
    }
    return selection;
}

void GlyphView::setSelected( const SelectionSet& selection )
{
    for( auto i = m_itemMap.begin(), iEnd = m_itemMap.end(); i != iEnd; ++i )
    {
        if( Selectable* pSelectable = Selection::glyphToSelectable( i->second ) )
        {
            // if( !pSelectable->isImage() )
            {
                const bool bIsSelected = selection.find( i->second ) != selection.end();
                pSelectable->setSelected( bIsSelected );
            }
        }
    }

    QItemSelection itemSelection;

    for( auto i = selection.begin(), iEnd = selection.end(); i != iEnd; ++i )
    {
        schematic::IGlyph* pGlyph = *i;
        if( const auto* pSite = dynamic_cast< const schematic::Site* >( pGlyph->getGlyphSpec() ) )
        {
            itemSelection.push_back( QItemSelectionRange( m_itemModel.getNodeIndex( pSite->getPtr() ) ) );
        }
    }

    m_selectionModel.select( itemSelection, QItemSelectionModel::ClearAndSelect );
}

schematic::IGlyph* GlyphView::findGlyph( QGraphicsItem* pItem ) const
{
    schematic::IGlyph* pGlyph = 0u;
    auto               iFind  = m_itemMap.find( pItem );
    if( iFind != m_itemMap.end() )
        pGlyph = iFind->second;
    return pGlyph;
}

schematic::IGlyph* GlyphView::findSelectableTopmostGlyph( const QPointF& pos ) const
{
    schematic::IGlyph* pGlyph = nullptr;
    if( m_pActiveContext && m_pActiveTool )
    {
        QList< QGraphicsItem* > stack = items( pos.x(), pos.y() );
        for( QList< QGraphicsItem* >::iterator i = stack.begin(), iEnd = stack.end(); i != iEnd; ++i )
        {
            if( schematic::IGlyph* pTest = findGlyph( *i ) )
            {
                if( m_pActiveContext->canEdit( pTest, m_pActiveTool->getToolType(), m_pActiveTool->getToolMode(),
                                               m_pViewConfig->isGlyphVisible( ViewConfig::eGlyphVis_Points ),
                                               m_pViewConfig->isGlyphVisible( ViewConfig::eGlyphVis_Sites ),
                                               m_pViewConfig->isGlyphVisible( ViewConfig::eGlyphVis_Connections ) ) )
                {
                    if( Selection::glyphToSelectable( pTest ) )
                    {
                        pGlyph = pTest;
                        break;
                    }
                }
            }
        }
    }
    return pGlyph;
}

void GlyphView::mouseDoubleClickEvent( QMouseEvent* event )
{
    ASSERT( m_pActiveContext );
    if( event->button() == Qt::LeftButton )
    {
        QList< QGraphicsItem* >           stack = items( event->pos() );
        std::vector< schematic::IGlyph* > glyphStack;
        for( QList< QGraphicsItem* >::iterator i = stack.begin(), iEnd = stack.end(); i != iEnd; ++i )
        {
            if( schematic::IGlyph* pTest = findGlyph( *i ) )
                glyphStack.push_back( pTest );
        }
        if( schematic::IEditContext* pNewContext = m_pActiveContext->getNestedContext( glyphStack ) )
        {
            selectContext( pNewContext );
        }
        else if( m_pActiveContext->getParent() )
        {
            selectContext( m_pActiveContext->getParent() );
        }
    }
    else
    {
        GridView::mouseDoubleClickEvent( event );
    }
}

void GlyphView::mousePressEvent( QMouseEvent* pEvent )
{
    if( pEvent->button() == Qt::LeftButton )
    {
        m_pActiveTool->mousePressEvent( pEvent );

        if( schematic::Node::Ptr pInteractionNode = m_pActiveTool->GetInteractionNode() )
        {
            if( !m_pActiveContext->isNodeContext( pInteractionNode ) )
            {
                if( schematic::IEditContext* pNewContext = m_pActiveContext->getNodeContext( pInteractionNode ) )
                {
                    selectContext( pNewContext );
                }
            }
        }
    }

    GridView::mousePressEvent( pEvent );
}

void GlyphView::mouseMoveEvent( QMouseEvent* pEvent )
{
    m_pActiveTool->mouseMoveEvent( pEvent );

    GridView::mouseMoveEvent( pEvent );
}

void GlyphView::mouseReleaseEvent( QMouseEvent* pEvent )
{
    if( pEvent->button() == Qt::LeftButton )
    {
        m_pActiveTool->mouseReleaseEvent( pEvent );
    }

    GridView::mouseReleaseEvent( pEvent );
}
void GlyphView::CmdTabOut()
{
    if( m_pActiveContext && m_pActiveContext->getParent() )
    {
        selectContext( m_pActiveContext->getParent() );
    }
}

void GlyphView::CmdSelectAll()
{
    if( m_pActiveContext )
    {
        ASSERT( m_pActiveTool );
        SelectionSet selection;
        for( auto i = m_itemMap.begin(), iEnd = m_itemMap.end(); i != iEnd; ++i )
        {
            if( Selectable* pSelectable = Selection::glyphToSelectable( i->second ) )
            {
                if( pSelectable->isImage()
                    && m_pActiveContext->canEdit( i->second, m_pActiveTool->getToolType(), m_pActiveTool->getToolMode(),
                                                  m_pViewConfig->isGlyphVisible( ViewConfig::eGlyphVis_Points ),
                                                  m_pViewConfig->isGlyphVisible( ViewConfig::eGlyphVis_Sites ),
                                                  m_pViewConfig->isGlyphVisible( ViewConfig::eGlyphVis_Connections )

                                                      ) )
                {
                    selection.insert( i->second );
                }
            }
        }
        if( getSelection() != selection )
            setSelected( selection );
        else
            setSelected( SelectionSet() );
    }
}

void GlyphView::CmdCut()
{
    ASSERT( m_pActiveContext );
    ASSERT( m_pMainWindow );

    if( schematic::Schematic::Ptr pCut
        = boost::dynamic_pointer_cast< schematic::Schematic >( m_pActiveContext->cmd_cut( getSelection() ) ) )
    {
        m_pMainWindow->getToolbox()->add( "clipboard", pCut, true );
        OnClipboardAction();
    }
}

void GlyphView::CmdCopy()
{
    ASSERT( m_pActiveContext );
    ASSERT( m_pMainWindow );

    if( schematic::Schematic::Ptr pCopy
        = boost::dynamic_pointer_cast< schematic::Schematic >( m_pActiveContext->cmd_copy( getSelection() ) ) )
    {
        m_pMainWindow->getToolbox()->add( "clipboard", pCopy, true );
        OnClipboardAction();
    }
}

void GlyphView::CmdPaste()
{
    ASSERT( m_pActiveContext );
    ASSERT( m_pMainWindow );

    if( Toolbox::Palette::Ptr pPalette = m_pMainWindow->getToolbox()->getPalette( "clipboard" ) )
    {
        if( schematic::Schematic::Ptr pClip = pPalette->getSelection() )
        {
            const QPointF   pos( 0.0f, 0.0f );
            const QVector2D q = getQuantisationLevel();
            if( schematic::IInteraction::Ptr pInteraction
                = m_pActiveContext->cmd_paste( pClip, pos.x(), pos.y(), q.x(), q.y() ) )
            {
                SelectionSet currentSelection;
                pInteraction->GetSelection( currentSelection );
                setSelected( currentSelection );
            }
        }
    }
}

void GlyphView::CmdDelete()
{
    ASSERT( m_pActiveContext );
    ASSERT( m_pMainWindow );

    m_pActiveTool->reset();

    SelectionSet currentSelection = getSelection();
    setSelected( SelectionSet{} );

    try
    {
        m_pActiveContext->cmd_delete( currentSelection );
    }
    catch( std::exception& ex )
    {
        std::ostringstream os;
        os << "Error performing union command: " << ex.what();
        QMessageBox::warning( this, tr( "Schematic Edittor" ), QString::fromUtf8( os.str().c_str() ) );
    }
}

void GlyphView::CmdRotateLeft()
{
    m_pActiveContext->cmd_rotateLeft( getSelection() );
}

void GlyphView::CmdRotateRight()
{
    m_pActiveContext->cmd_rotateRight( getSelection() );
}

void GlyphView::CmdFlipHorizontal()
{
    m_pActiveContext->cmd_flipHorizontally( getSelection() );
}

void GlyphView::CmdFlipVerical()
{
    m_pActiveContext->cmd_flipVertically( getSelection() );
}

void GlyphView::CmdShrink()
{
    const QVector2D q = getQuantisationLevel();
    m_pActiveContext->cmd_shrink( getSelection(), q.x() );
}

void GlyphView::CmdExtrude()
{
    const QVector2D q = getQuantisationLevel();
    m_pActiveContext->cmd_extrude( getSelection(), q.x() );
}

void GlyphView::CmdUnion()
{
    SelectionSet currentSelection = getSelection();
    setSelected( SelectionSet{} );

    try
    {
        m_pActiveContext->cmd_union( currentSelection );
    }
    catch( std::exception& ex )
    {
        std::ostringstream os;
        os << "Error performing union command: " << ex.what();
        QMessageBox::warning( this, tr( "Schematic Edittor" ), QString::fromUtf8( os.str().c_str() ) );
    }
}

void GlyphView::CmdFilter()
{
    m_pActiveContext->cmd_filter( getSelection() );
}

void GlyphView::CmdAABB()
{
    m_pActiveContext->cmd_aabb( getSelection() );
}

void GlyphView::CmdConvexHull()
{
    m_pActiveContext->cmd_convexHull( getSelection() );
}

void GlyphView::CmdReparent()
{
    SelectionSet currentSelection = getSelection();
    setSelected( SelectionSet{} );
    try
    {
        m_pActiveContext->cmd_reparent( currentSelection );
    }
    catch( std::exception& ex )
    {
        std::ostringstream os;
        os << "Error performing union command: " << ex.what();
        QMessageBox::warning( this, tr( "Schematic Edittor" ), QString::fromUtf8( os.str().c_str() ) );
    }
}

void GlyphView::CmdSelectTool()
{
    m_pActiveTool->reset();
    m_pActiveTool = &m_selectTool;
}

void GlyphView::CmdLassoTool()
{
    m_pActiveTool->reset();
    m_pActiveTool = &m_lassoTool;
}

void GlyphView::CmdDrawTool()
{
    m_pActiveTool->reset();
    m_pActiveTool = &m_penTool;
}

void GlyphView::CmdEditTool()
{
    m_pActiveTool->reset();
    m_pActiveTool = &m_editTool;
}

} // namespace editor