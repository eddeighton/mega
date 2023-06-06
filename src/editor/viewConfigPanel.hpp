
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

#ifndef GUARD_2023_June_06_viewConfigPanel
#define GUARD_2023_June_06_viewConfigPanel

#include "mainWindow.hpp"

#include <QGraphicsView>
#include <QWheelEvent>
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsRectItem>
#include <QFont>

#ifndef Q_MOC_RUN
#include "toolbox.hpp"
#include "viewConfig.hpp"

#include <optional>
#endif

namespace editor
{
class ViewConfigPanel : public QGraphicsView
{
    Q_OBJECT
private:
    struct Row
    {
        QGraphicsSimpleTextItem* pLabel;
        QGraphicsRectItem*       pSetting;
    };
    using RowVector = std::vector< Row >;

public:
    explicit ViewConfigPanel( QWidget* parent = nullptr );

    void setToolbox( Toolbox::Ptr pToolbox );
    void setViewConfig( editor::ViewConfig::Ptr pViewConfig );

signals:
    void OnViewConfigModified();

protected:
    void drawBackground( QPainter* painter, const QRectF& rect );
    void resizeEvent( QResizeEvent* pEvent );

    void mousePressEvent( QMouseEvent* pEvent );
    void mouseMoveEvent( QMouseEvent* pEvent );
    void mouseReleaseEvent( QMouseEvent* pEvent );

private:
    bool setConfig( int iRow, int iCol );

private:
    editor::MainWindow*     m_pMainWindow = nullptr;
    Toolbox::Ptr            m_pToolBox;
    QGraphicsScene*         m_pScene;
    editor::ViewConfig::Ptr m_pConfig;
    RowVector               m_glyphRows;
    RowVector               m_maskRows;

    QColor      m_selectionColour = QColor( 125, 0, 0, 55 );
    QColor      m_bkgrnd          = QColor( 255, 255, 255, 255 );
    QColor      m_textColour      = QColor( 0, 0, 0 );
    std::string m_strFont         = "Times";
    int         m_fontSize        = 8;
    float       m_fSize           = 100.0f;
    QColor      m_gridColour      = QColor( 125, 125, 125, 255 );
    int         m_gridWidth       = 1;

    QFont m_font;

    std::optional< int > m_mouseDownColumn;
};

} // namespace editor

#endif // GUARD_2023_June_06_viewConfigPanel
