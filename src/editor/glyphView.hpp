
#ifndef GLYPH_VIEW_03_FEB_2021
#define GLYPH_VIEW_03_FEB_2021

#include "gridView.hpp"
#include "selection_model.hpp"
#include "item_model.hpp"
#include "viewConfig.hpp"

#include <QGraphicsView>
#include <QWheelEvent>
#include <QGraphicsScene>
#include <QItemSelection>
#include <QPainterPath>
#include <QGraphicsSimpleTextItem>

#ifndef Q_MOC_RUN

#include "glyphs.hpp"
#include "tools.hpp"
#include "toolbox.hpp"
#include "document.hpp"

#include "schematic/editInteractions.hpp"

#include <memory>
#include <vector>

#endif

namespace editor
{

class GlyphView : public GridView, public schematic::GlyphFactory
{
    Q_OBJECT

public:
    // lifetime and focus
    explicit GlyphView( QWidget* parent, MainWindow* pMainWindow );
    ~GlyphView();

    virtual void postCreate( Document::Ptr pDocument );
    virtual void onViewFocussed();
    virtual void onViewUnfocussed();
    virtual void updateItemZoomLevels();

public:
    // glyph factory interface
    virtual schematic::IGlyph::Ptr createControlPoint( schematic::ControlPoint* pControlPoint,
                                                       schematic::IGlyph::Ptr   pParent );
    virtual schematic::IGlyph::Ptr createOrigin( schematic::Origin* pOrigin, schematic::IGlyph::Ptr pParent );
    virtual schematic::IGlyph::Ptr createMarkupPolygonGroup( schematic::MarkupPolygonGroup* pMarkupPolygonGroup,
                                                             schematic::IGlyph::Ptr         pParent );
    virtual schematic::IGlyph::Ptr createMarkupText( schematic::MarkupText* pMarkupText,
                                                     schematic::IGlyph::Ptr pParent );
    virtual schematic::IGlyph::Ptr createImage( schematic::ImageSpec* pImage, schematic::IGlyph::Ptr pParent );
    virtual void                   onEditted( bool bCommandCompleted );

public:
    virtual void onDocumentUpdate();
    void         updateGlyphVisibility();

public:
    // context handling
    void selectContext( schematic::IEditContext* pNewContext );

    // selection handling
    schematic::IEditContext*  getActiveContext() const { return m_pActiveContext; }
    schematic::Schematic::Ptr getCurrentClip() const;
    Toolbox::Ptr              getToolbox() const;

    SelectionSet getSelection() const;
    SelectionSet getSelectedByRect( const QRectF& rect ) const;
    SelectionSet getSelectedByPath( const QPainterPath& path ) const;
    void         setSelected( const SelectionSet& selection );

    schematic::IGlyph* findGlyph( QGraphicsItem* pItem ) const;
    schematic::IGlyph* findSelectableTopmostGlyph( const QPointF& pos ) const;

protected:
    // QT virtual functions
    virtual void mouseDoubleClickEvent( QMouseEvent* pEvent );
    virtual void mousePressEvent( QMouseEvent* pEvent );
    virtual void mouseMoveEvent( QMouseEvent* pEvent );
    virtual void mouseReleaseEvent( QMouseEvent* pEvent );

signals:
    void OnClipboardAction();

private slots:
    // commands
    void CmdTabOut();
    void CmdSelectAll();
    void CmdCut();
    void CmdCopy();
    void CmdPaste();
    void CmdDelete();

    void CmdRotateLeft();
    void CmdRotateRight();
    void CmdFlipHorizontal();
    void CmdFlipVerical();
    void CmdShrink();
    void CmdExtrude();
    void CmdUnion();
    void CmdFilter();
    void CmdAABB();
    void CmdConvexHull();
    void CmdReparent();

    void CmdSelectTool();
    void CmdLassoTool();
    void CmdDrawTool();
    void CmdEditTool();

    void OnCurrentSelectionItemChanged( const QModelIndex& current, const QModelIndex& previous );
    void OnSelectionChanged( const QItemSelection& selected, const QItemSelection& deselected );

protected slots:
    void OnViewConfigChanged();

private:
    Selectable* selectableFromNode( schematic::Node::PtrCst pNode ) const;

protected:
    Document::Ptr            m_pDocument;
    ItemModel                m_itemModel;
    SelectionModel           m_selectionModel;
    schematic::IEditContext* m_pActiveContext = nullptr;

    SelectTool                  m_selectTool;
    LassoTool                   m_lassoTool;
    PenTool                     m_penTool;
    EditTool                    m_editTool;
    Tool*                       m_pActiveTool;
    ViewConfig::Ptr             m_pViewConfig;
    schematic::CompilationStage m_compilationConfig;

private:
    ItemMap m_itemMap;
    SpecMap m_specMap;
};

} // namespace editor

#endif // GLYPH_VIEW_03_FEB_2021