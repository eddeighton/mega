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
    explicit ClipScene(QWidget *parent = 0);

    void setClip( schematic::Schematic::Ptr pSchematic, Toolbox::Ptr pToolBox );
    void calculateSceneRect();

    //glyph factory interface
    virtual schematic::IGlyph::Ptr createControlPoint( schematic::ControlPoint* pControlPoint, schematic::IGlyph::Ptr pParent );
    virtual schematic::IGlyph::Ptr createOrigin( schematic::Origin* pOrigin, schematic::IGlyph::Ptr pParent );
    //virtual schematic::IGlyph::Ptr createMarkupPath( schematic::MarkupPath* pMarkupPath, schematic::IGlyph::Ptr pParent );
    virtual schematic::IGlyph::Ptr createMarkupPolygonGroup( schematic::MarkupPolygonGroup* pMarkupPolygonGroup, schematic::IGlyph::Ptr pParent );
    virtual schematic::IGlyph::Ptr createMarkupText( schematic::MarkupText* pMarkupText, schematic::IGlyph::Ptr pParent );
    virtual void onEditted( bool bCommandCompleted );

signals:

public slots:

protected:
    virtual void drawBackground(QPainter *painter, const QRectF &rect);

private:
    ItemMap m_itemMap;
    SpecMap m_specMap;
    schematic::EditSchematic::Ptr m_pEdit;
    schematic::Schematic::Ptr m_pSchematic;
    schematic::IEditContext* m_pNullContext;
    Toolbox::Ptr m_pToolBox;
    
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
        typedef boost::shared_ptr< FlowItem > Ptr;
        typedef std::vector< Ptr > PtrVector;
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
        QRectF getRect() const { return m_pImageItem->sceneBoundingRect(); }
        bool onHit( QGraphicsItem* pItem );
        void updatePosition( int x, int y, float fWidth, float fHeight, float fSpacing );
    private:
        FlowView& m_view;
        schematic::Schematic::Ptr m_pSchematic;
        QGraphicsItem* m_pImageItem;
        Toolbox::Palette::Ptr m_pPalette;
    };
    typedef std::map< schematic::Schematic::Ptr, FlowItem::Ptr > ItemMap;

public:
    explicit FlowView( MissionToolbox& toolbox, Toolbox::Palette::Ptr pPalette );
    ~FlowView();

    Toolbox::Ptr getToolbox() const;
    Toolbox::Palette::Ptr getPalette() const { return m_pPalette; }

    void updateClips();
    void calculateLayout();
    void clear();
    
signals:
    void OnClipboardAction();
    void OnMenu( ClipboardMsg msg );
    void OnQuickLoad( ClipboardMsg msg );

protected:
    void mouseDoubleClickEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent * event);
private:
    int findIndex( schematic::Schematic::Ptr pSchematic );
    schematic::Schematic::Ptr getClickedSchematic( QMouseEvent* event );
private:
    MissionToolbox& m_toolBox;
    Toolbox::Palette::Ptr m_pPalette;
    ItemMap m_items;
    QGraphicsRectItem* m_pSelectionRectItem;
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
        typedef boost::shared_ptr< ToolboxPanel > Ptr;

        ToolboxPanel( MissionToolbox& toolbox, 
            Toolbox::Palette::Ptr pPalette, unsigned int uiIndex );
        ~ToolboxPanel();
        void updateClips();
        const Timing::UpdateTick& getUpdateTick() const { return m_updateTick; }
    private:
        std::string m_strName;
        MissionToolbox& m_toolbox;
        Toolbox::Palette::Ptr m_pPalette;
        FlowView* m_pView;
        Timing::UpdateTick m_updateTick;
    };
    typedef std::map< std::string, ToolboxPanel::Ptr > PanelMap;
    
public:
    explicit MissionToolbox( QWidget *parent = 0 );

    void setToolbox( Toolbox::Ptr pToolbox );
    void setMainWindow( MainWindow* pMainWindow ) { m_pMainWindow = pMainWindow; }
    
    Toolbox::Ptr getToolbox() const { return m_pToolBox; }
    
public slots:
    void updateToolbox();
    void onCurrentPaletteChanged( int index );

    void OnQuickLoad( ClipboardMsg msg );
    void OnPaletteMenu( ClipboardMsg msg );
    void On_DeleteClip( ClipboardMsg msg );
    void On_RescanToolBoxDir();

private:
    Toolbox::Ptr m_pToolBox;
    MainWindow* m_pMainWindow = nullptr;
    PanelMap m_panels;
};

}//namespace editor

#endif // TOOLBOX_PANEL_05_FEB_2021_H
