#ifndef TOOLS_05_FEB_2021
#define TOOLS_05_FEB_2021

#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QGraphicsRectItem>
#include <QWheelEvent>

#ifndef Q_MOC_RUN

#include "glyphs.hpp"
// #include "voronoi.hpp"

#include "schematic/glyph.hpp"
#include "schematic/editRoot.hpp"

#include <set>
#include <memory>

#endif

namespace editor
{

class GlyphView;

typedef std::set< schematic::IGlyph* > SelectionSet;

class Selection
{
public:
    enum Mode
    {
        eSet,
        eOR,
        eXAND,
        eXOR
    };

    static const Selectable* glyphToSelectable( const schematic::IGlyph* pGlyph );
    static Selectable*       glyphToSelectable( schematic::IGlyph* pGlyph );
    static Mode              getSelectionMode( QMouseEvent* event );

public:
    Selection( GlyphView& view, Mode mode, const SelectionSet& interactiveSelection );
    void                update( const SelectionSet& interactiveSelection );
    SelectionSet&       getInitial() { return m_initialSelection; }
    const SelectionSet& getInitial() const { return m_initialSelection; }

private:
    GlyphView&   m_view;
    const Mode   m_selectionMode;
    SelectionSet m_initialSelection;
};

class Selection_Interaction
{
public:
    virtual ~Selection_Interaction() {}
    virtual void update( const QPointF& pos ) = 0;
};

class Selection_Rect : public Selection_Interaction
{
public:
    Selection_Rect( GlyphView& view, Selection::Mode mode, float fZoomLevel, const QPointF& downPos,
                    const QColor& colour = QColor( 0, 0, 200, 50 ) );
    ~Selection_Rect();
    void update( const QPointF& pos );

private:
    GlyphView&         m_view;
    QRectF             m_selectionRect;
    Selection          m_selection;
    QPointF            m_downPos;
    QPen               m_selectionPen;
    QGraphicsRectItem* m_pSelectionRectItem;
};

class Selection_Path : public Selection_Interaction
{
public:
    Selection_Path( GlyphView& view, Selection::Mode mode, float fZoomLevel, const QPointF& downPos,
                    const QColor& colour = QColor( 0, 0, 200, 50 ) );
    ~Selection_Path();
    void         update( const QPointF& pos );
    unsigned int getDepth() const { return m_uiDepth; }

private:
    GlyphView&         m_view;
    QPainterPath       m_path;
    Selection          m_selection;
    QPointF            m_downPos;
    QPen               m_selectionPen;
    QGraphicsPathItem* m_pSelectionPath;
    unsigned int       m_uiDepth;
};

class Tool
{
public:
    Tool( GlyphView& view );
    virtual ~Tool();

    virtual void                              mousePressEvent( QMouseEvent* event )   = 0;
    virtual void                              mouseMoveEvent( QMouseEvent* event )    = 0;
    virtual void                              mouseReleaseEvent( QMouseEvent* event ) = 0;
    virtual void                              keyPressEvent( QKeyEvent* event )       = 0;
    virtual void                              reset()                                 = 0;
    virtual void                              onUpdate()                              = 0;
    virtual schematic::IEditContext::ToolType getToolType() const                     = 0;
    virtual schematic::IEditContext::ToolMode getToolMode() const                     = 0;

    virtual schematic::Node::Ptr GetInteractionNode() const;

protected:
    GlyphView&                               m_view;
    std::shared_ptr< Selection_Interaction > m_pSelection;
    schematic::IInteraction::Ptr             m_pInteraction;
};

class SelectTool : public Tool
{
    enum ToolMode
    {
        eSelectNodes,
        eCmd,
        eDragCopyCmd
    };

public:
    SelectTool( GlyphView& view );

    virtual void mousePressEvent( QMouseEvent* event );
    virtual void mouseMoveEvent( QMouseEvent* event );
    virtual void mouseReleaseEvent( QMouseEvent* event );
    virtual void keyPressEvent( QKeyEvent* event );
    virtual void reset();
    virtual void onUpdate();

    virtual schematic::IEditContext::ToolType getToolType() const { return schematic::IEditContext::eSelect; }
    virtual schematic::IEditContext::ToolMode getToolMode() const { return schematic::IEditContext::eArea; }

private:
    ToolMode m_toolMode;
};

class LassoTool : public Tool
{
    enum ToolMode
    {
        eSelectNodes,
        eCmd,
        eDragCopyCmd
    };

public:
    LassoTool( GlyphView& view );

    virtual void mousePressEvent( QMouseEvent* event );
    virtual void mouseMoveEvent( QMouseEvent* event );
    virtual void mouseReleaseEvent( QMouseEvent* event );
    virtual void keyPressEvent( QKeyEvent* event );
    virtual void reset();
    virtual void onUpdate();

    virtual schematic::IEditContext::ToolType getToolType() const { return schematic::IEditContext::eSelect; }
    virtual schematic::IEditContext::ToolMode getToolMode() const { return schematic::IEditContext::eFeature; }

private:
    ToolMode m_toolMode;
};

class PenTool : public Tool
{
protected:
    enum ToolMode
    {
        eDraw,
        eCmd,
        eDragCopy /*,
         eDelete*/
    };

public:
    PenTool( GlyphView& view );

    virtual void mousePressEvent( QMouseEvent* event );
    virtual void mouseMoveEvent( QMouseEvent* event );
    virtual void mouseReleaseEvent( QMouseEvent* event );
    virtual void keyPressEvent( QKeyEvent* event );
    virtual void reset();
    virtual void onUpdate();

    virtual schematic::IEditContext::ToolType getToolType() const { return schematic::IEditContext::eDraw; }
    virtual schematic::IEditContext::ToolMode getToolMode() const { return schematic::IEditContext::eArea; }

protected:
    ToolMode m_toolMode;
    bool     m_bMoved;
};

class Painter
{
public:
    virtual ~Painter(){};
};

class EditTool : public Tool
{
protected:
    enum ToolMode
    {
        eDraw,
        eCmd,
        eDragCopy
    };

public:
    EditTool( GlyphView& view );
    ~EditTool();

    virtual void mousePressEvent( QMouseEvent* event );
    virtual void mouseMoveEvent( QMouseEvent* event );
    virtual void mouseReleaseEvent( QMouseEvent* event );
    virtual void keyPressEvent( QKeyEvent* event );
    virtual void reset();
    virtual void onUpdate();

    virtual schematic::IEditContext::ToolType getToolType() const { return schematic::IEditContext::eDraw; }
    virtual schematic::IEditContext::ToolMode getToolMode() const { return schematic::IEditContext::eFeature; }

protected:
    ToolMode m_toolMode;
    bool     m_bMoved;

    QPen               m_segmentPen;
    PainterImpl        m_painter;
    QGraphicsPathItem* m_pClosestEdge = nullptr;
};
} // namespace editor

#endif // TOOLS_05_FEB_2021
