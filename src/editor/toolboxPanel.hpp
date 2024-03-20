#ifndef TOOLBOX_PANEL_05_FEB_2021_H
#define TOOLBOX_PANEL_05_FEB_2021_H

#include "messages.hpp"

#include "glyphView.hpp"

#include <QToolBox>
#include <QAbstractListModel>
#include <QListView>
#include <qgraphicsscene.h>

#ifndef Q_MOC_RUN

#include "toolbox.hpp"

#include "schematic/node.hpp"
#include "schematic/schematic.hpp"
#include "schematic/editSchematic.hpp"

#include <boost/shared_ptr.hpp>

#include <string>
#include <map>

#endif

namespace editor
{

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
class ClipScene : public QGraphicsScene, public schematic::GlyphFactory
{
    Q_OBJECT

public:
    explicit ClipScene( QWidget* parent = nullptr );

    void setClip( schematic::Schematic::Ptr pSchematic, Toolbox::Ptr pToolBox );
    void calculateSceneRect();

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

signals:

public slots:

protected:
    virtual void drawBackground( QPainter* painter, const QRectF& rect );

private:
    ItemMap                       m_itemMap;
    SpecMap                       m_specMap;
    schematic::EditSchematic::Ptr m_pEdit;
    schematic::Schematic::Ptr     m_pSchematic;
    schematic::IEditContext*      m_pNullContext;
    Toolbox::Ptr                  m_pToolBox;

    float m_fDeviceWidth;
};

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
class MissionToolbox;
class FlowView : public QGraphicsView
{
    Q_OBJECT

    class FlowItem
    {
    public:
        using Ptr       = boost::shared_ptr< FlowItem >;
        using PtrVector = std::vector< Ptr >;
        FlowItem( FlowView& view, schematic::Schematic::Ptr pSchematic, Toolbox::Palette::Ptr pPalette );
        ~FlowItem()
        {
            if( m_view.scene() )
            {
                m_view.scene()->removeItem( m_pImageItem );
                delete m_pImageItem;
            }
        }
        schematic::Schematic::Ptr getClip() const { return m_pSchematic; }
        QRectF                    getRect() const { return m_pImageItem->sceneBoundingRect(); }
        bool                      onHit( QGraphicsItem* pItem );
        void                      updatePosition( int x, int y, float fWidth, float fHeight, float fSpacing );

    private:
        FlowView&                 m_view;
        schematic::Schematic::Ptr m_pSchematic;
        QGraphicsItem*            m_pImageItem;
        Toolbox::Palette::Ptr     m_pPalette;
    };
    using ItemMap = std::map< schematic::Schematic::Ptr, FlowItem::Ptr >;

public:
    explicit FlowView( MissionToolbox& toolbox, Toolbox::Palette::Ptr pPalette );
    ~FlowView();

    Toolbox::Ptr          getToolbox() const;
    Toolbox::Palette::Ptr getPalette() const { return m_pPalette; }

    void updateClips();
    void calculateLayout();
    void clear();

signals:
    void OnClipboardAction();
    void OnMenu( ClipboardMsg msg );
    void OnQuickLoad( ClipboardMsg msg );

protected:
    void         mouseDoubleClickEvent( QMouseEvent* event );
    void         mousePressEvent( QMouseEvent* event );
    void         mouseMoveEvent( QMouseEvent* event );
    void         mouseReleaseEvent( QMouseEvent* event );
    virtual void resizeEvent( QResizeEvent* event );

private:
    int                       findIndex( schematic::Schematic::Ptr pSchematic );
    schematic::Schematic::Ptr getClickedSchematic( QMouseEvent* event );

private:
    MissionToolbox&       m_toolBox;
    Toolbox::Palette::Ptr m_pPalette;
    ItemMap               m_items;
    QGraphicsRectItem*    m_pSelectionRectItem;
};

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
class MissionToolbox : public QToolBox
{
    Q_OBJECT
private:
    friend struct CompareUpdateTimes;
    class ToolboxPanel
    {
    public:
        using Ptr = boost::shared_ptr< ToolboxPanel >;

        ToolboxPanel( MissionToolbox& toolbox, Toolbox::Palette::Ptr pPalette, unsigned int uiIndex );
        ~ToolboxPanel();
        void                      updateClips();
        const Timing::UpdateTick& getUpdateTick() const { return m_updateTick; }

    private:
        MissionToolbox&       m_toolbox;
        Toolbox::Palette::Ptr m_pPalette;
        std::string           m_strName;
        FlowView*             m_pView;
        Timing::UpdateTick    m_updateTick;
    };
    using PanelMap = std::map< std::string, ToolboxPanel::Ptr >;

public:
    explicit MissionToolbox( QWidget* parent = nullptr );

    Toolbox::Ptr getToolbox() const { return m_pToolBox; }

public slots:
    void updateToolbox();
    void onCurrentPaletteChanged( int index );

    void OnQuickLoad( ClipboardMsg msg );
    void OnPaletteMenu( ClipboardMsg msg );
    void On_DeleteClip( ClipboardMsg msg );
    void On_RescanToolBoxDir();

private:
    MainWindow*  m_pMainWindow = nullptr;
    Toolbox::Ptr m_pToolBox;
    PanelMap     m_panels;
};

} // namespace editor

#endif // TOOLBOX_PANEL_05_FEB_2021_H
