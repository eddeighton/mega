
#ifndef GRID_VIEW_01_FEB_2021
#define GRID_VIEW_01_FEB_2021

#include <QGraphicsView>
#include <QWheelEvent>
#include <QGraphicsScene>
#include <QItemSelection>
#include <QPainterPath>
#include <QGraphicsSimpleTextItem>

#ifndef Q_MOC_RUN

#include "document.hpp"

#include <memory>
#include <vector>

#endif

namespace editor
{
class MainWindow;

class GridView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit GridView( QWidget* parent, MainWindow* pMainWindow );
    ~GridView();

    virtual void postCreate( Document::Ptr pDocument );
    virtual void onViewFocussed();
    virtual void onViewUnfocussed();

    virtual void updateItemZoomLevels() = 0;

    bool isPanning() const { return m_pScrollData.get() ? true : false; }

    float     getZoomLevel() const { return fabs( getZoomVector().x() ); }
    QVector2D getZoomVector() const;
    QVector2D getQuantisationLevel() const;
    void      SetZoom( QVector2D v2NewZoomLevel );
    void      DoZoom( float fAmt );

protected slots:
    void CmdZoomToAll();

protected:
    void CalculateOversizedSceneRect();
    void CalculateRulerItems();

protected:
    void drawBackground( QPainter* painter, const QRectF& rect );
    void resizeEvent( QResizeEvent* pEvent );

    void mousePressEvent( QMouseEvent* pEvent );
    void mouseMoveEvent( QMouseEvent* pEvent );
    void mouseReleaseEvent( QMouseEvent* pEvent );
    void scrollContentsBy( int dx, int dy );
    void wheelEvent( QWheelEvent* pEvent );

protected:
    MainWindow*     m_pMainWindow;
    QGraphicsScene* m_pScene;

private:
    struct ScrollData
    {
        QPointF m_downPos, m_downScrollPos;
        ScrollData( const QPointF& downPos, const QPointF& downScrollPos )
            : m_downPos( downPos )
            , m_downScrollPos( downScrollPos )
        {
        }
    };
    std::unique_ptr< ScrollData > m_pScrollData;

    using TextItemVector = std::vector< QGraphicsSimpleTextItem* >;
    TextItemVector m_rulerVertItems, m_rulerHoriItems;

    float m_fDefaultZoom  = 256.0f;
    bool  m_bInitialising = true;

    QColor m_mainLineColour  = QColor( 100, 100, 100, 125 );
    QColor m_otherLineColour = QColor( 200, 200, 200, 125 );
    QColor m_bkgrnd          = QColor( 255, 255, 255 );
    QColor m_textColor       = QColor( 0, 0, 0, 255 );
    int    m_iMainLineStep   = 4;
    float  m_lineWidth       = 0.5f;
    float  m_fZoomRate       = 0.025f;
};

} // namespace editor

#endif // GRID_VIEW_01_FEB_2021