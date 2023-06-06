
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

#ifndef Q_MOC_RUN
#include "toolbox.hpp"
#include "viewConfig.hpp"
#endif

namespace editor
{
class ViewConfigPanel : public QGraphicsView
{
    Q_OBJECT
private:
public:
    explicit ViewConfigPanel( QWidget* parent = nullptr );

    void setToolbox( Toolbox::Ptr pToolbox );
    void setViewConfig( editor::ViewConfig::Ptr pViewConfig );

protected:
    void drawBackground( QPainter* painter, const QRectF& rect );
    void resizeEvent( QResizeEvent* pEvent );

    void mousePressEvent( QMouseEvent* pEvent );
    void mouseMoveEvent( QMouseEvent* pEvent );
    void mouseReleaseEvent( QMouseEvent* pEvent );

private:
    editor::MainWindow*     m_pMainWindow = nullptr;
    Toolbox::Ptr            m_pToolBox;
    QGraphicsScene*         m_pScene;
    editor::ViewConfig::Ptr m_pConfig;
    
    QColor m_mainLineColour  = QColor( 100, 100, 100, 125 );
    QColor m_otherLineColour = QColor( 200, 200, 200, 125 );
    QColor m_bkgrnd          = QColor( 255, 255, 255 );
    QColor m_textColor       = QColor( 0, 0, 0, 255 );
    int    m_iMainLineStep   = 4;
    float  m_lineWidth       = 0.5f;
};

} // namespace editor

#endif // GUARD_2023_June_06_viewConfigPanel
