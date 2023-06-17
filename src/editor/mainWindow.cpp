
#include "mainWindow.hpp"

#include "DockAreaWidget.h"
#include "schematicView.hpp"

#include "ui_mainWindow.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QProcessEnvironment>
#include <QFileDialog>
#include <QScrollBar>

#ifndef Q_MOC_RUN

#include "schematic/space.hpp"
#include "schematic/schematic.hpp"
#include "schematic/factory.hpp"

#include "common/assert_verify.hpp"

#include "boost/filesystem.hpp"

#include <sstream>
#include <chrono>
#include <functional>
#include <fstream>

#endif

namespace editor
{
MainWindow* MainWindow::m_pThis = nullptr;

MainWindow::MainWindow( QWidget* pParent )
    : QMainWindow( pParent )
    , m_pMainWindowImpl( new Ui::MainWindow )
    , m_pToolbox( new Toolbox( boost::filesystem::current_path().string() ) )
{
    VERIFY_RTE( m_pThis == nullptr );
    m_pThis = this;

    m_pMainWindowImpl->setupUi( this );

    {
        // toolbox
        QObject::connect( m_pMainWindowImpl->toolBox, SIGNAL( currentChanged( int ) ), m_pMainWindowImpl->toolBox,
                          SLOT( onCurrentPaletteChanged( int ) ) );

        QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();

        // std::ostringstream osMsg;
        // for( const QString& str : environment.toStringList() )
        //{
        //     osMsg << str.toLocal8Bit().constData() << "\n";
        // }

        /*const QString strToolBoxPath = environment.value( "BLUEPRINT_TOOLBOX_PATH" );
        if( strToolBoxPath.isEmpty() )
        {
            THROW_RTE( "Missing BLUEPRINT_TOOLBOX_PATH environment variable" );//: " << osMsg.str() );
        }
        m_pToolbox.reset( new Toolbox( strToolBoxPath.toLocal8Bit().constData() ) );*/
    }

    {
        // delete undo history from prior execution
        const boost::filesystem::path tempFolder( boost::filesystem::current_path() / "history" );
        if( boost::filesystem::exists( tempFolder ) )
        {
            boost::filesystem::remove_all( tempFolder );
        }
    }

    // compilatin toolbar
    {
        m_pCompilationModeComboBox = new QComboBox( m_pMainWindowImpl->toolbarCompilation );
        m_pMainWindowImpl->toolbarCompilation->addWidget( m_pCompilationModeComboBox );

        m_pCompilationModeComboBox->addItem( "Site" );
        m_pCompilationModeComboBox->addItem( m_pMainWindowImpl->actionView_SiteContour->text() );
        m_pCompilationModeComboBox->addItem( m_pMainWindowImpl->actionView_Walls->text() );
        m_pCompilationModeComboBox->addItem( m_pMainWindowImpl->actionView_Analysis->text() );
        m_pCompilationModeComboBox->addItem( m_pMainWindowImpl->actionView_Partition->text() );
        m_pCompilationModeComboBox->addItem( m_pMainWindowImpl->actionView_Properties->text() );
        m_pCompilationModeComboBox->addItem( m_pMainWindowImpl->actionView_Skeleton->text() );
    }

    // dockStructure->toggleViewAction()
    {
        m_pMainWindowImpl->toolbarView->addAction( m_pMainWindowImpl->dockStructure->toggleViewAction() );
        m_pMainWindowImpl->dockStructure->toggleViewAction()->setIcon( QIcon( ":/art/dialog.png" ) );
        m_pMainWindowImpl->menuView->addAction( m_pMainWindowImpl->dockStructure->toggleViewAction() );
        m_pMainWindowImpl->dockStructure->toggleViewAction()->setShortcut( Qt::Key_F1 );
    }

    // dockViewConfig
    {
        m_pMainWindowImpl->toolbarView->addAction( m_pMainWindowImpl->dockViewConfig->toggleViewAction() );
        m_pMainWindowImpl->dockViewConfig->toggleViewAction()->setIcon( QIcon( ":/art/notepad.png" ) );
        m_pMainWindowImpl->menuView->addAction( m_pMainWindowImpl->dockViewConfig->toggleViewAction() );
        m_pMainWindowImpl->dockViewConfig->toggleViewAction()->setShortcut( Qt::Key_F3 );
    }

    // dockToolbox->toggleViewAction()
    {
        m_pMainWindowImpl->toolbarView->addAction( m_pMainWindowImpl->dockToolbox->toggleViewAction() );
        m_pMainWindowImpl->dockToolbox->toggleViewAction()->setIcon( QIcon( ":/art/clipboard.png" ) );
        m_pMainWindowImpl->menuView->addAction( m_pMainWindowImpl->dockToolbox->toggleViewAction() );
        m_pMainWindowImpl->dockToolbox->toggleViewAction()->setShortcut( Qt::Key_F2 );
    }

    // dockLog
    {
        m_pMainWindowImpl->toolbarView->addAction( m_pMainWindowImpl->dockLog->toggleViewAction() );
        m_pMainWindowImpl->dockLog->toggleViewAction()->setIcon( QIcon( ":/art/zoomdocument.png" ) );
        m_pMainWindowImpl->menuView->addAction( m_pMainWindowImpl->dockLog->toggleViewAction() );
        m_pMainWindowImpl->dockLog->toggleViewAction()->setShortcut( Qt::Key_F4 );
    }

    // actionLoad
    {
        QObject::connect( m_pMainWindowImpl->actionLoad, SIGNAL( triggered() ), this, SLOT( OnLoad() ) );
    }

    // actionNewSchematic
    {
        QObject::connect(
            m_pMainWindowImpl->actionNewSchematic, SIGNAL( triggered() ), this, SLOT( OnNewSchematic() ) );
    }

    ads::CDockManager::setConfigFlag( ads::CDockManager::FocusHighlighting, true );
    ads::CDockManager::setConfigFlag( ads::CDockManager::AlwaysShowTabs, true );

    QVBoxLayout* Layout = new QVBoxLayout( m_pMainWindowImpl->dockContainer );
    Layout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    m_pDockManager = new ads::CDockManager( m_pMainWindowImpl->dockContainer );

    Layout->addWidget( m_pDockManager );

    QObject::connect( m_pMainWindowImpl->actionExit, SIGNAL( triggered() ), this, SLOT( close() ) );

    QObject::connect( m_pMainWindowImpl->actionSave_All, SIGNAL( triggered() ), this, SLOT( OnSaveAll() ) );

    /*
    QObject::connect(
        m_pDockManager, &ads::CDockManager::dockWidgetRemoved,
        this,           &MainWindow::OnDockWidgetRemoved );

    QObject::connect(
        m_pDockManager, &ads::CDockManager::dockWidgetAboutToBeRemoved,
        this,           &MainWindow::OnDockWidgetAboutToBeRemoved );
    */

    QObject::connect(
        m_pDockManager, &ads::CDockManager::floatingWidgetCreated, this, &MainWindow::OnFloatingWidgetCreated );

    QObject::connect(
        m_pDockManager, &ads::CDockManager::focusedDockWidgetChanged, this, &MainWindow::OnFocusedDockWidgetChanged );

    // customize layout a bit because dont know how to do it in mainWindow.ui
    {
        // m_pDockManager->addDockWidgetTab( ads::LeftDockWidgetArea, m_pMainWindowImpl->dockStructure );
        m_pMainWindowImpl->dockLog->hide();
        m_pMainWindowImpl->dockStructure->hide();
    }
}

MainWindow::~MainWindow()
{
    delete m_pMainWindowImpl;
}

void MainWindow::OnDocumentChanged( Document* pDocument )
{
    for( DocumentViewMap::iterator i = m_docViewMap.begin(), iEnd = m_docViewMap.end(); i != iEnd; ++i )
    {
        DocViewWidget& docView = i->second;

        if( docView.pDocument.get() == pDocument )
        {
            if( SchematicView* pSchematicView = dynamic_cast< SchematicView* >( docView.pView ) )
            {
                pSchematicView->onDocumentUpdate();
            }
        }
    }
}

void MainWindow::OnDocumentError( Document* pDocument, const std::string& strErrorMsg )
{
    // if( pDocument->getFilePath().has_value() )
    // {
    // }

    m_pMainWindowImpl->logView->appendPlainText( QString::fromUtf8( strErrorMsg ) );
    m_pMainWindowImpl->logView->verticalScrollBar()->setValue(
        m_pMainWindowImpl->logView->verticalScrollBar()->maximum() );
}

void MainWindow::addActionRef( QAction* pAction )
{
    ActionMap::iterator iFind = m_actionRefCountMap.find( pAction );
    if( iFind != m_actionRefCountMap.end() )
    {
        VERIFY_RTE( iFind->second >= 0 );
        if( iFind->second == 0 )
        {
            iFind->first->setVisible( true );
        }
        ++iFind->second;
    }
    else
    {
        m_actionRefCountMap[ pAction ] = 1;
        pAction->setVisible( true );
    }
}

void MainWindow::decActionRef( QAction* pAction )
{
    ActionMap::iterator iFind = m_actionRefCountMap.find( pAction );
    if( iFind != m_actionRefCountMap.end() )
    {
        VERIFY_RTE( iFind->second > 0 );
        --iFind->second;

        if( iFind->second == 0 )
        {
            iFind->first->setVisible( false );
        }
    }
    else
    {
        THROW_RTE( "Error in action ref count" );
    }
}

bool MainWindow::TestClosingDocument( DocViewWidget& docView )
{
    bool bAllowClose = true;

    if( docView.pDocument->isModified() )
    {
        std::ostringstream os;
        if( docView.pDocument->getFilePath().has_value() )
        {
            os << "Document: " << docView.pDocument->getFilePath().value().string() << " has unsaved changes.";
        }
        else
        {
            os << "Unsaved document detected.";
        }

        os << "\nDo you want to save before closing?";
        const int Result = QMessageBox::question( this, tr( "Schematic Editor" ), QString::fromUtf8( os.str().c_str() ),
                                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel );
        if( QMessageBox::Yes == Result )
        {
            if( SchematicView* pSchematicView = dynamic_cast< SchematicView* >( docView.pView ) )
            {
                bAllowClose = pSchematicView->CmdSave();
            }
            else
            {
                THROW_RTE( "Invalid view type" );
            }
        }
        else if( QMessageBox::No == Result )
        {
            // do nothing
        }
        else if( QMessageBox::Cancel == Result )
        {
            bAllowClose = false;
        }
        else
        {
            THROW_RTE( "Unexpected response" );
        }
    }

    return bAllowClose;
}

void MainWindow::closeEvent( QCloseEvent* pEvent )
{
    bool bContinueClosing = true;

    for( DocumentViewMap::iterator i = m_docViewMap.begin(), iEnd = m_docViewMap.end(); i != iEnd; ++i )
    {
        DocViewWidget& docView = i->second;

        if( !TestClosingDocument( docView ) )
        {
            bContinueClosing = false;
            break;
        }
    }

    if( bContinueClosing )
    {
        pEvent->accept();
    }
    else
    {
        pEvent->ignore();
    }
}

void MainWindow::OnDocumentSaved( const void* pDocument )
{
    for( DocumentViewMap::iterator i = m_docViewMap.begin(), iEnd = m_docViewMap.end(); i != iEnd; ++i )
    {
        DocViewWidget& docView = i->second;

        if( docView.pDocument.get() == pDocument )
        {
            if( docView.pDocument->getFilePath().has_value() )
            {
                const std::string strFilePath = docView.pDocument->getFilePath().value().string();
                docView.pDockWidget->setWindowTitle( QString::fromUtf8( strFilePath.c_str() ) );
            }
        }
    }
}

void MainWindow::OnFocusedDockWidgetCloseRequested( ads::CFloatingDockContainer* pDockContainer )
{
    bool bAllowClosing = true;

    std::vector< ads::CDockWidget* > toClose;
    for( ads::CDockWidget* pDockWidget : pDockContainer->dockWidgets() )
    {
        DocumentViewMap::iterator iFind = m_docViewMap.find( pDockWidget );
        if( iFind != m_docViewMap.end() )
        {
            DocViewWidget& docView = iFind->second;
            if( !TestClosingDocument( docView ) )
            {
                bAllowClosing = false;
                toClose.clear();
                break;
            }
            else
            {
                toClose.push_back( pDockWidget );
            }
        }
    }

    if( bAllowClosing )
    {
        for( ads::CDockWidget* pDockWidgetToClose : toClose )
        {
            pDockWidgetToClose->closeDockWidget();
        }
        pDockContainer->close();
    }
}

void MainWindow::OnDockWidgetCloseRequested( ads::CDockWidget* pDockWidget )
{
    DocumentViewMap::iterator iFind = m_docViewMap.find( pDockWidget );
    if( iFind != m_docViewMap.end() )
    {
        DocViewWidget& docView = iFind->second;
        if( TestClosingDocument( docView ) )
        {
            pDockWidget->closeDockWidget();
        }
    }
}

void MainWindow::OnDockWidgetClosed( ads::CDockWidget* pDockWidget )
{
    DocumentViewMap::iterator iFind = m_docViewMap.find( pDockWidget );
    if( iFind != m_docViewMap.end() )
    {
        const DocViewWidget& docView = iFind->second;
        if( docView.bFocussed )
        {
            if( SchematicView* pSchematicView = dynamic_cast< SchematicView* >( docView.pView ) )
            {
                pSchematicView->onViewUnfocussed();
            }
            else
            {
                THROW_RTE( "Invalid view type" );
            }
        }
    }

    m_docViewMap.erase( iFind );
}

void MainWindow::OnFloatingWidgetCreated( ads::CFloatingDockContainer* pFloatingWidget )
{
    MainWindow* pThis = this;

    // closeRequested
    QObject::connect( pFloatingWidget, &ads::CFloatingDockContainer::closeRequested,
                      [ pThis, pFloatingWidget ]() { pThis->OnFocusedDockWidgetCloseRequested( pFloatingWidget ); } );
}
/*
void MainWindow::OnDockWidgetRemoved( ads::CDockWidget* DockWidget )
{
    qDebug() << "MainWindow::OnDockWidgetRemoved";
}

void MainWindow::OnDockWidgetAboutToBeRemoved( ads::CDockWidget* DockWidget )
{
    qDebug() << "MainWindow::OnDockWidgetAboutToBeRemoved";
}*/

void MainWindow::OnFocusedDockWidgetChanged( ads::CDockWidget* pOld, ads::CDockWidget* pNew )
{
    {
        DocumentViewMap::iterator iFind = m_docViewMap.find( pOld );
        if( iFind != m_docViewMap.end() )
        {
            DocViewWidget& docView = iFind->second;
            if( docView.bFocussed )
            {
                if( SchematicView* pSchematicView = dynamic_cast< SchematicView* >( docView.pView ) )
                {
                    pSchematicView->onViewUnfocussed();
                }
                else
                {
                    THROW_RTE( "Invalid view type" );
                }
                docView.bFocussed = false;
            }
        }
    }

    {
        DocumentViewMap::iterator iFind = m_docViewMap.find( pNew );
        if( iFind != m_docViewMap.end() )
        {
            DocViewWidget& docView = iFind->second;
            if( !docView.bFocussed )
            {
                if( SchematicView* pSchematicView = dynamic_cast< SchematicView* >( docView.pView ) )
                {
                    pSchematicView->onViewFocussed();
                }
                else
                {
                    THROW_RTE( "Invalid view type" );
                }
                docView.bFocussed = true;
            }
        }
    }
}

void MainWindow::OnNewSchematic( SchematicDocument::Ptr pNewSchematic )
{
    SchematicView* pNewView = nullptr;

    const std::string strTitle = pNewSchematic->getSchematic()->getName();

    // Create a dock widget with the title Label 1 and set the created label
    // as the dock widget content
    ads::CDockWidget* pDockWidget = new ads::CDockWidget( QString::fromUtf8( strTitle.c_str() ) );
    {
        pDockWidget->setObjectName( QString::fromUtf8( pNewSchematic->getUniqueObjectName().c_str() ) );
        pDockWidget->setFeature( ads::CDockWidget::CustomCloseHandling, true );
        pDockWidget->setFeature( ads::CDockWidget::DockWidgetDeleteOnClose, true );
        pDockWidget->setFeature( ads::CDockWidget::DockWidgetForceCloseWithArea, true );

        pNewView = new SchematicView( pDockWidget, this );

        pDockWidget->setWidget( pNewView );
    }

    DocViewWidget docView = { pNewSchematic, pNewView, pDockWidget };
    m_docViewMap.insert( std::make_pair( pDockWidget, docView ) );

    MainWindow* pThis = this;

    QObject::connect( pDockWidget, &ads::CDockWidget::closeRequested,
                      [ pThis, pDockWidget ]() { pThis->OnDockWidgetCloseRequested( pDockWidget ); } );

    QObject::connect( pDockWidget, &ads::CDockWidget::closed,
                      [ pThis, pDockWidget ]() { pThis->OnDockWidgetClosed( pDockWidget ); } );

    QObject::connect( pNewView, &SchematicView::OnDocumentSaved, this, &MainWindow::OnDocumentSaved );

    QObject::connect(
        pNewView, &SchematicView::OnClipboardAction, m_pMainWindowImpl->toolBox, &MissionToolbox::updateToolbox );

    // Add the toggleViewAction of the dock widget to the menu to give
    // the user the possibility to show the dock widget if it has been closed
    // ui->menuView->addAction( pDockWidget->toggleViewAction() );

    if( ads::CDockWidget* pFocussedDockWidget = m_pDockManager->focusedDockWidget() )
    {
        if( ads::CDockAreaWidget* pDockAreaWidget = pFocussedDockWidget->dockAreaWidget() )
        {
            m_pDockManager->addDockWidgetTabToArea( pDockWidget, pDockAreaWidget );
        }
        else
        {
            m_pDockManager->addDockWidget( ads::TopDockWidgetArea, pDockWidget );
        }
    }
    else
    {
        // auto* pCentralDockArea =
        m_pDockManager->addDockWidget( ads::TopDockWidgetArea, pDockWidget );
    }

    // Add the dock widget to the top dock widget area
    // pCentralDockArea->setAllowedAreas( ads::DockWidgetArea::OuterDockAreas );

    pNewView->postCreate( pNewSchematic );
}

void MainWindow::OnNewSchematic()
{
    schematic::Schematic::Ptr pNewBase( new schematic::Schematic( "Untitled Base" ) );
    {
        pNewBase->init();
    }

    SchematicDocument::Ptr pNewSchematic(
        new SchematicDocument( *this, pNewBase, schematic::File::getDefaultCompilationConfig() ) );
    OnNewSchematic( pNewSchematic );
}

void MainWindow::OnLoadSchematic( ClipboardMsg msg )
{
    if( msg.optFilePath.has_value() )
    {
        SchematicDocument::Ptr pNewSchematic( new SchematicDocument(
            *this, msg.pSchematic, schematic::File::getDefaultCompilationConfig(), msg.optFilePath.value() ) );
        OnNewSchematic( pNewSchematic );
    }
    else
    {
        SchematicDocument::Ptr pNewSchematic(
            new SchematicDocument( *this, msg.pSchematic, schematic::File::getDefaultCompilationConfig() ) );
        OnNewSchematic( pNewSchematic );
    }
}

void MainWindow::OnLoad()
{
    QProcessEnvironment environment    = QProcessEnvironment::systemEnvironment();
    QString             strDefaultPath = environment.value( "BLUEPRINT_TOOLBOX_PATH" );

    // TODO
    // get currently focussed dockwidget to determine directory to use

    std::string strFilePath;
    {
        QString qFilePath
            = QFileDialog::getOpenFileName( this, tr( "Open File" ), strDefaultPath, tr( "Schematic Files (*.sch)" ) );
        strFilePath = qFilePath.toStdString();
    }

    if( !strFilePath.empty() )
    {
        try
        {
            const boost::filesystem::path filePath = strFilePath;

            schematic::File::Ptr pFile = schematic::load( filePath );

            if( schematic::Schematic::Ptr pSchematic = boost::dynamic_pointer_cast< schematic::Schematic >( pFile ) )
            {
                SchematicDocument::Ptr pNewSchematic( new SchematicDocument(
                    *this, pSchematic, schematic::File::getDefaultCompilationConfig(), filePath ) );
                OnNewSchematic( pNewSchematic );
            }
            else
            {
                THROW_RTE( "Unsupported file type: " << strFilePath );
            }
        }
        catch( std::exception& ex )
        {
            std::ostringstream os;
            os << "Error loading file: " << strFilePath << " : " << ex.what();
            QMessageBox::warning( this, tr( "Edittor" ), QString::fromUtf8( os.str().c_str() ) );
        }
    }
}

void MainWindow::OnSaveAll()
{
    std::string strFilePath;
    try
    {
        for( DocumentViewMap::iterator i = m_docViewMap.begin(), iEnd = m_docViewMap.end(); i != iEnd; ++i )
        {
            DocViewWidget& docView = i->second;

            if( docView.pDocument->isModified() )
            {
                if( SchematicView* pSchematicView = dynamic_cast< SchematicView* >( docView.pView ) )
                {
                    if( docView.pDocument->getFilePath().has_value() )
                        strFilePath = docView.pDocument->getFilePath().value().string();
                    else
                        strFilePath = "New File";
                    pSchematicView->CmdSave();
                }
                else
                {
                    THROW_RTE( "Invalid view type" );
                }
            }
        }
    }
    catch( std::exception& ex )
    {
        std::ostringstream os;
        os << "Error saving file: " << strFilePath << " : " << ex.what();
        QMessageBox::warning( this, tr( "Schematic Edittor" ), QString::fromUtf8( os.str().c_str() ) );
    }
}

} // namespace editor