
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

#ifndef GUARD_2023_June_16_treeView
#define GUARD_2023_June_16_treeView

#include "mainWindow.hpp"
#include "gridView.hpp"
#include "glyphView.hpp"
#include "selection_model.hpp"
#include "item_model.hpp"
#include "viewConfig.hpp"

#include <QTreeView>
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

class TreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit TreeView( QWidget* parent );

    void setActiveGlyphView( editor::GlyphView* pGlyphView ) { m_pGlyphView = pGlyphView; }

public slots:
    void CmdEdit();

protected:
    virtual void mouseReleaseEvent( QMouseEvent* event ) override;

private:
    editor::MainWindow* m_pMainWindow = nullptr;
    Toolbox::Ptr        m_pToolBox;
    editor::GlyphView*  m_pGlyphView = nullptr;
};

} // namespace editor

#endif // GUARD_2023_June_16_treeView
