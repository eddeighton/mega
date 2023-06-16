
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

#include "treeView.hpp"
#include "item_model.hpp"

#include "mainWindow.hpp"
#include "ui_mainWindow.h"

#include <QMenu>

#ifndef Q_MOC_RUN

#include "qtUtils.hpp"
#include "schematic/schematic.hpp"
#include "schematic/property.hpp"

#endif

namespace editor
{

TreeView::TreeView( QWidget* parent )
    : QTreeView( parent )
    , m_pMainWindow( MainWindow::getSingleton() )
    , m_pToolBox( m_pMainWindow->getToolbox() )
{
    CMD_CONNECT( actionEditContext, CmdEdit );
}

void TreeView::CmdEdit()
{
    if( m_pGlyphView )
    {
        // see if any properties are selected.
        bool bEditedProperty = false;
        if( const ItemModel* pItemModel = dynamic_cast< const ItemModel* >( this->model() ) )
        {
            const QModelIndexList indexes = this->selectionModel()->selection().indexes();

            if( !indexes.empty() )
            {
                std::vector< schematic::Property::PtrCst > properties;
                for( const auto& index : indexes )
                {
                    if( schematic::Property::PtrCst pProperty
                        = boost::dynamic_pointer_cast< const schematic::Property >( pItemModel->getIndexNode( index ) ) )
                    {
                        properties.push_back( pProperty );
                    }
                }
                if( !properties.empty() )
                {
                    bEditedProperty = true;
                    std::ostringstream os;
                    for( auto p : properties )
                    {
                        os << p->getName() << " ";
                    }
                    
                }
            }
        }
        if( !bEditedProperty )
        {
            // if not attempt to set the edit context
            SelectionSet selection = m_pGlyphView->getSelection();
            m_pGlyphView->setSelected( selection );
        }
    }
}

void TreeView::mouseReleaseEvent( QMouseEvent* pEvent )
{
    QTreeView::mouseReleaseEvent( pEvent );

    if( pEvent->button() == Qt::RightButton )
    {
        // show context menu
        QMenu menu( this );

        menu.addAction( m_pMainWindow->getUI()->actionEditContext );
        menu.addAction( m_pMainWindow->getUI()->actionTabOut );
        menu.addAction( m_pMainWindow->getUI()->actionSelectAll );
        menu.addSeparator();
        menu.addAction( m_pMainWindow->getUI()->actionUndo );
        menu.addAction( m_pMainWindow->getUI()->actionRedo );
        menu.addSeparator();
        menu.addAction( m_pMainWindow->getUI()->actionCut );
        menu.addAction( m_pMainWindow->getUI()->actionCopy );
        menu.addAction( m_pMainWindow->getUI()->actionPaste );
        menu.addAction( m_pMainWindow->getUI()->actionDelete );
        menu.addSeparator();
        menu.addAction( m_pMainWindow->getUI()->actionRotate_Left );
        menu.addAction( m_pMainWindow->getUI()->actionRotate_Right );
        menu.addAction( m_pMainWindow->getUI()->actionFlip_Horizontal );
        menu.addAction( m_pMainWindow->getUI()->actionFlip_Vertical );
        menu.addSeparator();
        menu.addAction( m_pMainWindow->getUI()->actionShrink );
        menu.addAction( m_pMainWindow->getUI()->actionExtrude );
        menu.addAction( m_pMainWindow->getUI()->actionUnion );
        menu.addAction( m_pMainWindow->getUI()->actionFilter );
        menu.addAction( m_pMainWindow->getUI()->actionAABB );
        menu.addAction( m_pMainWindow->getUI()->actionConvexHull );
        menu.addAction( m_pMainWindow->getUI()->actionReparent );

        menu.exec( QCursor::pos() );
    }
}

} // namespace editor