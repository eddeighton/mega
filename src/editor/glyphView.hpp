
#ifndef GLYPH_VIEW_03_FEB_2021
#define GLYPH_VIEW_03_FEB_2021

#include "gridView.hpp"

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

#include "map/editInteractions.hpp"
#include "map/clip.hpp"

#include <memory>
#include <vector>

#endif

namespace editor
{

class GlyphView : public GridView, public map::GlyphFactory
{
    Q_OBJECT

public:
    //lifetime and focus
    explicit GlyphView( QWidget *parent, MainWindow* pMainWindow );
    ~GlyphView();

    virtual void postCreate( Document::Ptr pDocument );
    virtual void onViewFocussed();
    virtual void onViewUnfocussed();
    virtual void onZoomed();
    
public:
    //glyph factory interface
    virtual map::IGlyph::Ptr createControlPoint( map::ControlPoint* pControlPoint, map::IGlyph::Ptr pParent );
    virtual map::IGlyph::Ptr createOrigin( map::Origin* pOrigin, map::IGlyph::Ptr pParent );
    virtual map::IGlyph::Ptr createMarkupPolygonGroup( map::MarkupPolygonGroup* pMarkupPolygonGroup, map::IGlyph::Ptr pParent );
    virtual map::IGlyph::Ptr createMarkupText( map::MarkupText* pMarkupText, map::IGlyph::Ptr pParent );
    virtual void onEditted( bool bCommandCompleted );
    
public:
    virtual void onDocumentUpdate();
    
    //visibility handling
    using GlyphVisibilityConfig = std::bitset< 16 >;
    enum GlyphVisibility
    {
        eGlyphVis_Text,
        eGlyphVis_Points,
        eGlyphVis_Sites,
        eGlyphVis_Connections
    };
    
    const GlyphVisibilityConfig& getGlyphVisibilityConfig() const { return m_visibilityConfig; }
    
    void updateVisibility( 
        const GlyphVisibilityConfig& glyphVisibilityConfig,
        const map::File::CompilationConfig& config );
    
public:
    //context handling
    void selectContext( map::IEditContext* pNewContext );

    //selection handling
    map::IEditContext* getActiveContext() const { return m_pActiveContext; }
    map::Schematic::Ptr getCurrentClip() const;
    Toolbox::Ptr getToolbox() const;
    
    SelectionSet getSelection() const;
    SelectionSet getSelectedByRect( const QRectF& rect ) const;
    SelectionSet getSelectedByPath( const QPainterPath& path ) const;
    void setSelected( const SelectionSet& selection );

    map::IGlyph* findGlyph( QGraphicsItem* pItem ) const;
    map::IGlyph* findSelectableTopmostGlyph( const QPointF& pos ) const;

protected:
    //QT virtual functions
    virtual void mouseDoubleClickEvent( QMouseEvent* pEvent );
    virtual void mousePressEvent( QMouseEvent* pEvent );
    virtual void mouseMoveEvent( QMouseEvent* pEvent );
    virtual void mouseReleaseEvent( QMouseEvent* pEvent );
        
signals:
    void OnClipboardAction();
        
private slots:
    //commands
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

protected:
    Document::Ptr m_pDocument;
    map::IEditContext* m_pActiveContext = nullptr;
    
    SelectTool  m_selectTool;
    LassoTool   m_lassoTool;
    PenTool     m_penTool;
    EditTool    m_editTool;
    Tool*       m_pActiveTool;
    GlyphVisibilityConfig m_visibilityConfig;
    
private:
    ItemMap m_itemMap;
    SpecMap m_specMap;
    
};

}


#endif //GLYPH_VIEW_03_FEB_2021