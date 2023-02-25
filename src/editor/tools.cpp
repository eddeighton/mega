#include "tools.hpp"

#include "glyphView.hpp"

#ifndef Q_MOC_RUN

#include "schematic/wall.hpp"
#include "schematic/object.hpp"
#include "schematic/space.hpp"

#endif

namespace editor
{
    
void filterSelectionByDepth( SelectionSet& selection, unsigned int uiDepth )
{
    for( SelectionSet::iterator 
        i = selection.begin(),
        iEnd = selection.end(); i!=iEnd; )
    {
        //if( Selection::glyphToSelectable( *i )->getDepth() != uiDepth )
        if( Selection::glyphToSelectable( *i )->getDepth() < uiDepth )
            i = selection.erase( i );
        else
            ++i;
    }
}

unsigned int findLowestDepth( const SelectionSet& selection )
{
    unsigned int uiLowest = 100u;
    for( SelectionSet::iterator 
        i = selection.begin(),
        iEnd = selection.end(); i!=iEnd; ++i )
    {
        Selectable* pSelectable = Selection::glyphToSelectable( *i );
        ASSERT( pSelectable );
        if( pSelectable )
        {
            if( uiLowest > pSelectable->getDepth() )
                uiLowest = pSelectable->getDepth();
        }
    }
    return uiLowest;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//selection handling
const Selectable* Selection::glyphToSelectable( const schematic::IGlyph* pGlyph )
{
    return dynamic_cast< const Selectable* >( pGlyph );
}

Selectable* Selection::glyphToSelectable( schematic::IGlyph* pGlyph )
{
    return dynamic_cast< Selectable* >( pGlyph );
}

Selection::Mode Selection::getSelectionMode( QMouseEvent *event )
{
    Selection::Mode mode = Selection::eSet;
    if( event->modifiers() & Qt::ControlModifier )      mode = Selection::eXAND;
    else if( event->modifiers() & Qt::ShiftModifier )   mode = Selection::eOR;
    else if( event->modifiers() & Qt::AltModifier )     mode = Selection::eXOR;
    return mode;
}


Selection::Selection( GlyphView& view, Mode mode, const SelectionSet& interactiveSelection )
    :   m_view( view ),
        m_selectionMode( mode ),
        m_initialSelection( m_view.getSelection() )
{
    update( interactiveSelection );
}

void Selection::update( const SelectionSet& interactive )
{
    //calculate the new selection state
    SelectionSet selection;
    switch( m_selectionMode )
    {
        case Selection::eSet:
            selection = interactive;
            break;
        case Selection::eOR:
            selection.insert( interactive.begin(), interactive.end() );
            selection.insert( m_initialSelection.begin(), m_initialSelection.end() );
            break;
        case Selection::eXAND:
            selection = m_initialSelection;
            for( SelectionSet::iterator i = interactive.begin(),
                 iEnd = interactive.end(); i!=iEnd; ++i )
                selection.erase( *i );
            break;
        case Selection::eXOR:
            selection = m_initialSelection;
            for( SelectionSet::iterator i = interactive.begin(),
                 iEnd = interactive.end(); i!=iEnd; ++i )
            {
                if( selection.find( *i ) == selection.end() )
                    selection.insert( *i );
                else
                    selection.erase( *i );
            }
            break;
    }

    //apply the new result selection to the control points...
    m_view.setSelected( selection );
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
Selection_Rect::Selection_Rect( GlyphView& view, Selection::Mode mode, float fZoomLevel, const QPointF& downPos, const QColor& colour )
    :   m_view( view ),
        m_selectionRect( downPos, downPos ),
        m_selection( view, mode, view.getSelectedByRect( m_selectionRect ) ),
        m_downPos( downPos )
{
    m_selectionPen.setStyle(        Qt::SolidLine);
    m_selectionPen.setWidth(        2.0f / fZoomLevel );
    m_selectionPen.setBrush(        QColor( colour.red(), colour.green(), colour.blue() ) );
    m_selectionPen.setCapStyle(     Qt::RoundCap);
    m_selectionPen.setJoinStyle(    Qt::RoundJoin);
    m_pSelectionRectItem = m_view.scene()->addRect( m_selectionRect, m_selectionPen, QBrush( colour ) );
}

void Selection_Rect::update( const QPointF& pos )
{
    m_selectionRect = QRectF( m_downPos, pos ).normalized();
    m_pSelectionRectItem->setRect( m_selectionRect );
    m_selection.update( m_view.getSelectedByRect( m_selectionRect ) );
}

Selection_Rect::~Selection_Rect()
{
    m_view.scene()->removeItem( m_pSelectionRectItem );
    delete m_pSelectionRectItem;
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

Selection_Path::Selection_Path( GlyphView& view, Selection::Mode mode, float fZoomLevel, const QPointF& downPos, const QColor& colour )
    :   m_view( view ),
        m_path( downPos ),
        m_selection( view, mode, view.getSelectedByPath( m_path ) ),
        m_downPos( downPos ),
        m_uiDepth( findLowestDepth( m_selection.getInitial() ) )
{
    m_path.setFillRule( Qt::WindingFill );

    m_selectionPen.setStyle(        Qt::SolidLine );
    m_selectionPen.setWidth(        2.0f / fZoomLevel );
    m_selectionPen.setBrush(        QColor( colour.red(), colour.green(), colour.blue() ) );
    m_selectionPen.setCapStyle(     Qt::RoundCap);
    m_selectionPen.setJoinStyle(    Qt::RoundJoin);

    m_pSelectionPath = m_view.scene()->addPath( m_path, m_selectionPen, QBrush( colour ) );

    filterSelectionByDepth( m_selection.getInitial(), m_uiDepth );   
}

void Selection_Path::update( const QPointF& pos )
{
    m_path.lineTo( pos );
    QPainterPath p = m_path;
    p.closeSubpath();
    m_pSelectionPath->setPath( p );
    
    SelectionSet selection = m_view.getSelectedByPath( m_path );
    if( !selection.empty() )
    {
        SelectionSet actualSelection = m_view.getSelection();
        if( actualSelection.empty() )
        {
            //if the actual selection is empty then can the lowest depth
            m_uiDepth = findLowestDepth( selection );
            filterSelectionByDepth( selection, m_uiDepth );  
            m_selection.getInitial() = selection;
            m_selection.update( selection );
        }
        else
        {
            //otherwise stick to it
            filterSelectionByDepth( selection, m_uiDepth );   
            m_selection.update( selection );
        }
    }
}

Selection_Path::~Selection_Path()
{
    m_view.scene()->removeItem( m_pSelectionPath );
    delete m_pSelectionPath;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
Tool::Tool( GlyphView& view )
:   m_view( view )
{

}

Tool::~Tool()
{

}

schematic::Node::Ptr Tool::GetInteractionNode() const
{
    return m_pInteraction ? m_pInteraction->GetInteractionNode() : schematic::Node::Ptr();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
SelectTool::SelectTool( GlyphView& view )
    :   Tool( view ),
        m_toolMode( eSelectNodes )
{
}

void SelectTool::mousePressEvent(QMouseEvent *event)
{
    ASSERT( m_view.getActiveContext() );
    
    const QVector2D q = m_view.getQuantisationLevel();
    /*if( event->button() == Qt::RightButton )
    {
        m_view.setCursor( Qt::ArrowCursor );
        m_toolMode = eSelectNodes;
        m_pSelection.reset( new Selection_Path( 
            m_view,
            Selection::getSelectionMode( event ), 
            m_view.getZoomLevel().y(), 
            m_view.mapToScene( event->pos() ) ) );
    }
    else */
    if( event->button() == Qt::LeftButton )
    {
        if( schematic::IGlyph* pGlyph = m_view.findSelectableTopmostGlyph( event->pos() ) )
        {
            SelectionSet currentSelection = m_view.getSelection();
            if( currentSelection.find( pGlyph ) == currentSelection.end() )
            {
                currentSelection.clear();
                currentSelection.insert( pGlyph );
                m_view.setSelected( currentSelection );
            }
            const QPointF pos = m_view.mapToScene( event->pos() );

            if( event->modifiers() & Qt::ControlModifier )
            {
                if( m_pInteraction = m_view.getActiveContext()->cmd_paste( 
                        currentSelection, pos.x(), pos.y(), q.x(), q.y() ) )
                {
                    m_view.setCursor( Qt::DragCopyCursor );
                    m_toolMode = eDragCopyCmd;
                    
                    m_pInteraction->GetSelection( currentSelection );
                    m_view.setSelected( currentSelection );
                }
            }
            if( !m_pInteraction )
            {
                m_pInteraction = m_view.getActiveContext()->interaction_start( 
                    getToolMode(), pos.x(), pos.y(), q.x(), q.y(), pGlyph, currentSelection );
                m_toolMode = eCmd;
                m_view.setCursor( Qt::SizeAllCursor );
                
                m_pInteraction->GetSelection( currentSelection );
                m_view.setSelected( currentSelection );
            }
        }
        else
        {
            m_view.setCursor( Qt::ArrowCursor );
            m_toolMode = eSelectNodes;
            m_pSelection.reset( new Selection_Rect( 
                m_view, 
                Selection::getSelectionMode( event ), 
                m_view.getZoomLevel().y(),
                m_view.mapToScene( event->pos() ) ) );
        }
    }
    
}
void SelectTool::mouseMoveEvent(QMouseEvent *event)
{
    switch( m_toolMode )
    {
        case eSelectNodes:
            {
                if( m_pSelection.get() )
                {
                    m_pSelection->update( m_view.mapToScene( event->pos() ) );
                    m_view.setCursor( Qt::ArrowCursor );
                }
                else
                {
                    if( schematic::IGlyph* pGlyph = m_view.findSelectableTopmostGlyph( event->pos() ) )
                    {
                        m_view.setCursor( Qt::SizeAllCursor );
                    }
                    else
                    {
                        m_view.setCursor( Qt::ArrowCursor );
                    }
                }
            }
            break;
        case eCmd:
            m_view.setCursor( Qt::SizeAllCursor );
            if( m_pInteraction )
            {
                QPointF pos = m_view.mapToScene( event->pos() );
                m_pInteraction->OnMove( pos.x(), pos.y() );
            }
            break;
        case eDragCopyCmd:
            m_view.setCursor( Qt::DragCopyCursor );
            if( m_pInteraction )
            {
                QPointF pos = m_view.mapToScene( event->pos() );
                m_pInteraction->OnMove( pos.x(), pos.y() );
            }
            break;
    }
}
void SelectTool::mouseReleaseEvent( QMouseEvent* pEvent )
{
    reset();
    
    if( schematic::IGlyph* pGlyph = m_view.findSelectableTopmostGlyph( pEvent->pos() ) )
    {
        m_view.setCursor( Qt::SizeAllCursor );
    }
    else
    {
        m_view.setCursor( Qt::ArrowCursor );
    }
}

void SelectTool::keyPressEvent(QKeyEvent*)
{
}

void SelectTool::reset()
{
    m_pInteraction.reset();
    m_pSelection.reset();
    m_toolMode = eSelectNodes;
}

void SelectTool::onUpdate()
{
    
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
LassoTool::LassoTool( GlyphView& view )
    :   Tool( view ),
        m_toolMode( eSelectNodes )
{
}

void LassoTool::mousePressEvent(QMouseEvent *event)
{
    ASSERT( m_view.getActiveContext() );
    
    const QVector2D q = m_view.getQuantisationLevel();
    if( event->button() == Qt::LeftButton )
    {
        if( schematic::IGlyph* pGlyph = m_view.findSelectableTopmostGlyph( event->pos() ) )
        {
            SelectionSet currentSelection = m_view.getSelection();
            if( currentSelection.find( pGlyph ) == currentSelection.end() )
            {
                currentSelection.clear();
                currentSelection.insert( pGlyph );
                m_view.setSelected( currentSelection );
            }
            const QPointF pos = m_view.mapToScene( event->pos() );

            if( event->modifiers() & Qt::ControlModifier )
            {
                if( m_pInteraction = m_view.getActiveContext()->cmd_paste( 
                        currentSelection, pos.x(), pos.y(), q.x(), q.y() ) )
                {
                    m_view.setCursor( Qt::DragCopyCursor );
                    m_toolMode = eDragCopyCmd;
                    
                    m_pInteraction->GetSelection( currentSelection );
                    m_view.setSelected( currentSelection );
                }
            }
            if( !m_pInteraction )
            {
                m_pInteraction = m_view.getActiveContext()->interaction_start( 
                    getToolMode(), pos.x(), pos.y(), q.x(), q.y(), pGlyph, currentSelection );
                m_toolMode = eCmd;
                m_view.setCursor( Qt::SizeAllCursor );
                
                m_pInteraction->GetSelection( currentSelection );
                m_view.setSelected( currentSelection );
            }
        }
        else
        {
            m_view.setCursor( Qt::ArrowCursor );
            m_toolMode = eSelectNodes;
            m_pSelection.reset( new Selection_Path( 
                m_view,
                Selection::getSelectionMode( event ), 
                m_view.getZoomLevel().y(), 
                m_view.mapToScene( event->pos() ) ) );
        }
    }
}
void LassoTool::mouseMoveEvent(QMouseEvent *event)
{
    switch( m_toolMode )
    {
        case eSelectNodes:
            {
                if( m_pSelection.get() )
                {
                    m_pSelection->update( m_view.mapToScene( event->pos() ) );
                    m_view.setCursor( Qt::ArrowCursor );
                }
                else
                {
                    if( schematic::IGlyph* pGlyph = m_view.findSelectableTopmostGlyph( event->pos() ) )
                    {
                        m_view.setCursor( Qt::SizeAllCursor );
                    }
                    else
                    {
                        m_view.setCursor( Qt::ArrowCursor );
                    }
                }
            }
            break;
        case eCmd:
            m_view.setCursor( Qt::SizeAllCursor );
            if( m_pInteraction )
            {
                QPointF pos = m_view.mapToScene( event->pos() );
                m_pInteraction->OnMove( pos.x(), pos.y() );
            }
            break;
        case eDragCopyCmd:
            m_view.setCursor( Qt::DragCopyCursor );
            if( m_pInteraction )
            {
                QPointF pos = m_view.mapToScene( event->pos() );
                m_pInteraction->OnMove( pos.x(), pos.y() );
            }
            break;
    }
}
void LassoTool::mouseReleaseEvent( QMouseEvent* pEvent )
{
    reset();
    
    if( schematic::IGlyph* pGlyph = m_view.findSelectableTopmostGlyph( pEvent->pos() ) )
    {
        m_view.setCursor( Qt::SizeAllCursor );
    }
    else
    {
        m_view.setCursor( Qt::ArrowCursor );
    }
}

void LassoTool::keyPressEvent(QKeyEvent*)
{
}

void LassoTool::reset()
{
    m_pInteraction.reset();
    m_pSelection.reset();
    m_toolMode = eSelectNodes;
}

void LassoTool::onUpdate()
{
    
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
PenTool::PenTool( GlyphView& view )
    :   Tool( view ),
        m_toolMode( eDraw )
{
}

void PenTool::mousePressEvent( QMouseEvent* event )
{
    ASSERT( m_view.getActiveContext() );
    
    const QVector2D q = m_view.getQuantisationLevel();
    if( event->button() == Qt::LeftButton )
    {
        const QPointF pos = m_view.mapToScene( event->pos() );
        if( schematic::IGlyph* pGlyph = m_view.findSelectableTopmostGlyph( event->pos() ) )
        {
            SelectionSet selection;
            if( event->modifiers() & Qt::ControlModifier )
            {
                if( m_pInteraction = m_view.getActiveContext()->cmd_paste( pGlyph, pos.x(), pos.y(), q.x(), q.y() ) )
                {
                    m_toolMode = eDragCopy;
                    m_view.setCursor( Qt::DragCopyCursor );
                    
                    SelectionSet currentSelection;
                    m_pInteraction->GetSelection( currentSelection );
                    m_view.setSelected( currentSelection );
                }
            }
            if( !m_pInteraction )
            {
                m_pInteraction = m_view.getActiveContext()->interaction_start( 
                    getToolMode(), pos.x(), pos.y(), q.x(), q.y(), pGlyph, selection );
                m_toolMode = eCmd;
                m_view.setCursor( Qt::SizeAllCursor );
                
                SelectionSet currentSelection;
                m_pInteraction->GetSelection( currentSelection );
                m_view.setSelected( currentSelection );
            }
        }
        else if( schematic::Schematic::Ptr pClip = m_view.getCurrentClip() )
        {
            //draw new space
            m_toolMode = eDraw;
            m_view.setCursor( Qt::SizeAllCursor );
            m_pInteraction = m_view.getActiveContext()->interaction_draw_clip( 
                pos.x(), pos.y(), q.x(), q.y(), pClip );
                
            SelectionSet currentSelection;
            m_pInteraction->GetSelection( currentSelection );
            m_view.setSelected( currentSelection );
        }
    }
    /*else if( event->button() == Qt::RightButton )
    {
        m_view.setCursor( Qt::ArrowCursor );
        m_toolMode = eDelete;
        m_pSelection.reset( new Selection_Rect( m_view, Selection::getSelectionMode( event ), m_view.getZoomLevel().y(),
            m_view.mapToScene( event->pos() ), QColor( 200, 0, 0, 100) ) );
        m_bMoved = false;
    }*/
}

void PenTool::mouseMoveEvent( QMouseEvent* pEvent )
{
    switch( m_toolMode )
    {
        case eDraw:
            if( m_pInteraction )
            {
                QPointF pos = m_view.mapToScene( pEvent->pos() );
                m_pInteraction->OnMove( pos.x(), pos.y() );
                m_view.setCursor( Qt::SizeAllCursor );
            }
            else
            {
                if( schematic::IGlyph* pGlyph = m_view.findSelectableTopmostGlyph( pEvent->pos() ) )
                {
                    m_view.setCursor( Qt::SizeAllCursor );
                }
                else
                {
                    m_view.setCursor( Qt::CrossCursor );
                }
            }
            break;
        case eCmd:
            if( m_pInteraction )
            {
                m_view.setCursor( Qt::SizeAllCursor );
                QPointF pos = m_view.mapToScene( pEvent->pos() );
                m_pInteraction->OnMove( pos.x(), pos.y() );
            }
            else
            {
                m_view.setCursor( Qt::CrossCursor );
            }
            break;
        case eDragCopy:
            if( m_pInteraction )
            {
                m_view.setCursor( Qt::DragCopyCursor );
                QPointF pos = m_view.mapToScene( pEvent->pos() );
                m_pInteraction->OnMove( pos.x(), pos.y() );
            }
            else
            {
                m_view.setCursor( Qt::DragCopyCursor );
            }
            break;
        /*case eDelete:
            if( m_pSelection.get() )
            {
                m_pSelection->update( m_view.mapToScene( pEvent->pos() ) );
                m_view.setCursor( Qt::ArrowCursor );
                m_bMoved = true;
            }
            break;*/
    }
    
    /*
    if( m_view.findSelectableTopmostGlyph( pEvent->pos() ) )
        m_view.setCursor( Qt::SizeAllCursor );
    else
        m_view.setCursor( Qt::CrossCursor );*/
}

void PenTool::mouseReleaseEvent( QMouseEvent* pEvent )
{
    reset();
    
    if( schematic::IGlyph* pGlyph = m_view.findSelectableTopmostGlyph( pEvent->pos() ) )
    {
        m_view.setCursor( Qt::SizeAllCursor );
    }
    else
    {
        m_view.setCursor( Qt::CrossCursor );
    }
    /*switch( m_toolMode )
    {
        case eDelete:
            if( m_bMoved )
            {
                m_view.OnCmd_Delete();
            }
            break;
        default:
            break;
    }*/
}

void PenTool::keyPressEvent(QKeyEvent*)
{
}

void PenTool::reset()
{
    m_pInteraction.reset();
    m_pSelection.reset();
    m_toolMode = eDraw;
}

void PenTool::onUpdate()
{
    
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
EditTool::EditTool( GlyphView& view )
    :   Tool( view ),
        m_toolMode( eDraw )
{
    m_segmentPen.setStyle(        Qt::SolidLine);
    m_segmentPen.setWidth(        1.0f / m_view.getZoomLevel().y() );
    m_segmentPen.setBrush(        QColor( 125, 0, 0 ) );
    m_segmentPen.setCapStyle(     Qt::RoundCap );
    m_segmentPen.setJoinStyle(    Qt::RoundJoin );
    
    m_pClosestEdge = m_view.scene()->addPath( m_painter.getPath(), m_segmentPen );
    m_pClosestEdge->setVisible( false );
}

EditTool::~EditTool()
{
    if( m_pClosestEdge )
    {
        m_view.scene()->removeItem( m_pClosestEdge );
        delete m_pClosestEdge;
    }
}

void EditTool::mousePressEvent( QMouseEvent* pEvent )
{
    ASSERT( m_view.getActiveContext() );
    
    const QVector2D q = m_view.getQuantisationLevel();
    if( pEvent->button() == Qt::LeftButton )
    {
        const QPointF pos = m_view.mapToScene( pEvent->pos() );
        schematic::IGlyph* pGlyph = m_view.findSelectableTopmostGlyph( pEvent->pos() );
        const bool bCtrl = pEvent->modifiers() & Qt::ControlModifier;
        if( !bCtrl && pGlyph )
        {
            SelectionSet selection;
            m_pInteraction = m_view.getActiveContext()->interaction_start( 
                getToolMode(), pos.x(), pos.y(), q.x(), q.y(), pGlyph, selection );
            m_toolMode = eCmd;
            m_view.setCursor( Qt::SizeAllCursor );
            
            SelectionSet currentSelection;
            m_pInteraction->GetSelection( currentSelection );
            m_view.setSelected( currentSelection );
            m_pClosestEdge->setVisible( false );
        }
        else
        {
            //draw new control point
            m_toolMode = eDraw;
            m_view.setCursor( Qt::SizeAllCursor );
            
            schematic::IGlyph* pHit1 = nullptr;
            schematic::IGlyph* pHit2 = nullptr;
            if( !bCtrl )
            {
                m_view.getActiveContext()->interaction_hover( pos.x(), pos.y(), pHit1, pHit2 );
            }
            
            schematic::IEditContext::SiteType siteType = schematic::IEditContext::eSpace;
            if( schematic::Schematic::Ptr pSchematic = m_view.getCurrentClip() )
            {
                const schematic::Site::PtrVector& sites = pSchematic->getSites();
                for( schematic::Site::Ptr pSite : sites )
                {
                    if( schematic::Space::Ptr pSpace = boost::dynamic_pointer_cast< schematic::Space >( pSite ) )
                    {
                        siteType = schematic::IEditContext::eSpace;
                        break;
                    }
                    else if( schematic::Wall::Ptr pWall = boost::dynamic_pointer_cast< schematic::Wall >( pSite ) )
                    {
                        siteType = schematic::IEditContext::eWall;
                        break;
                    }
                    else if( schematic::Object::Ptr pObject = boost::dynamic_pointer_cast< schematic::Object >( pSite ) )
                    {
                        siteType = schematic::IEditContext::eObject;
                        break;
                    }
                }
            }
            
            m_pInteraction = m_view.getActiveContext()->interaction_draw_point( 
                pos.x(), pos.y(), q.x(), q.y(), pHit1, pHit2, siteType );
                
            SelectionSet currentSelection;
            m_pInteraction->GetSelection( currentSelection );
            m_view.setSelected( currentSelection );
            m_pClosestEdge->setVisible( false );
        }
    }
    /*else if( event->button() == Qt::RightButton )
    {
        m_view.setCursor( Qt::ArrowCursor );
        m_toolMode = eDelete;
        m_pSelection.reset( new Selection_Rect( m_view, Selection::getSelectionMode( event ), m_view.getZoomLevel().y(),
            m_view.mapToScene( event->pos() ), QColor( 200, 0, 0, 100) ) );
        m_bMoved = false;
    }*/
}

void EditTool::mouseMoveEvent( QMouseEvent* pEvent )
{
    switch( m_toolMode )
    {
        case eDraw:
            {
                QPointF pos = m_view.mapToScene( pEvent->pos() );
                if( m_pInteraction )
                {
                    m_pClosestEdge->setVisible( false );
                    m_pInteraction->OnMove( pos.x(), pos.y() );
                    m_view.setCursor( Qt::SizeAllCursor );
                }
                else
                {
                    schematic::IGlyph* pHit1 = nullptr;
                    schematic::IGlyph* pHit2 = nullptr;
                    if( m_view.getActiveContext()->interaction_hover( pos.x(), pos.y(), pHit1, pHit2 ) )
                    {
                        const schematic::ControlPoint* pCtrl1 = dynamic_cast< const schematic::ControlPoint* >( pHit1->getGlyphSpec() );
                        const schematic::ControlPoint* pCtrl2 = dynamic_cast< const schematic::ControlPoint* >( pHit2->getGlyphSpec() );
                        
                        schematic::Transform absTransform =
                            m_view.getActiveContext()->getOrigin()->getAbsoluteTransform() * 
                                m_view.getActiveContext()->getOrigin()->getTransform();
                        
                        VERIFY_RTE( pCtrl1 && pCtrl2 );
                        
                        const schematic::Point pt1 = absTransform( pCtrl1->getPoint() );
                        const schematic::Point pt2 = absTransform( pCtrl2->getPoint() );

                        m_painter.reset();
                        m_painter.moveTo( pt1 );
                        m_painter.lineTo( pt2 );
                        m_painter.updated();
                        
                        m_pClosestEdge->setPath( m_painter.getPath() );
                        m_pClosestEdge->setVisible( true );
                    }
                    else
                    {
                        m_pClosestEdge->setVisible( false );
                    }
                    
                    if( schematic::IGlyph* pGlyph = m_view.findSelectableTopmostGlyph( pEvent->pos() ) )
                    {
                        m_view.setCursor( Qt::SizeAllCursor );
                    }
                    else
                    {
                        m_view.setCursor( Qt::CrossCursor );
                    }
                }
            }
            break;
        case eCmd:
            if( m_pInteraction )
            {
                m_view.setCursor( Qt::SizeAllCursor );
                QPointF pos = m_view.mapToScene( pEvent->pos() );
                m_pInteraction->OnMove( pos.x(), pos.y() );
            }
            else
            {
                m_view.setCursor( Qt::CrossCursor );
            }
            break;
        case eDragCopy:
            if( m_pInteraction )
            {
                m_view.setCursor( Qt::DragCopyCursor );
                QPointF pos = m_view.mapToScene( pEvent->pos() );
                m_pInteraction->OnMove( pos.x(), pos.y() );
            }
            else
            {
                m_view.setCursor( Qt::DragCopyCursor );
            }
            break;
        /*case eDelete:
            if( m_pSelection.get() )
            {
                m_pSelection->update( m_view.mapToScene( pEvent->pos() ) );
                m_view.setCursor( Qt::ArrowCursor );
                m_bMoved = true;
            }
            break;*/
    }
    
}

void EditTool::mouseReleaseEvent( QMouseEvent* pEvent )
{
    /*switch( m_toolMode )
    {
        case eDelete:
            if( m_bMoved )
            {
                m_view.OnCmd_Delete();
            }
            break;
        default:
            break;
    }*/
    reset();
    
    if( schematic::IGlyph* pGlyph = m_view.findSelectableTopmostGlyph( pEvent->pos() ) )
    {
        m_view.setCursor( Qt::SizeAllCursor );
    }
    else
    {
        m_view.setCursor( Qt::CrossCursor );
    }
}

void EditTool::keyPressEvent(QKeyEvent*)
{
}

void EditTool::reset()
{
    m_pClosestEdge->setVisible( false );
    m_pInteraction.reset();
    m_pSelection.reset();
    m_toolMode = eDraw;
}

void EditTool::onUpdate()
{
}

}



















