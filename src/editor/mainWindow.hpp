
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "DockManager.h"

#include "messages.hpp"

#include <QMainWindow>
#include <QComboBox>
#include <QTimer>

#ifndef Q_MOC_RUN

#include "toolbox.hpp"
#include "document.hpp"

#include "boost/asio/spawn.hpp"

#include <map>
#include <thread>
#include <atomic>

#endif

namespace Ui
{
class MainWindow;
}

namespace editor
{
class MainWindow : public QMainWindow, public DocumentChangeObserver
{
    Q_OBJECT

    struct DocViewWidget
    {
        Document::Ptr     pDocument;
        QWidget*          pView       = nullptr;
        ads::CDockWidget* pDockWidget = nullptr;
        bool              bFocussed   = false;
    };
    using DocumentViewMap = std::map< ads::CDockWidget*, DocViewWidget >;

    using ActionMap = std::map< QAction*, int >;

public:
    explicit MainWindow( boost::asio::yield_context* pYieldCtx );
    virtual ~MainWindow();

    static MainWindow* getSingleton()
    {
        ASSERT( m_pThis );
        return m_pThis;
    }

    Ui::MainWindow* getUI() { return m_pMainWindowImpl; }
    Toolbox::Ptr    getToolbox() const { return m_pToolbox; }

    void addActionRef( QAction* pAction );
    void decActionRef( QAction* pAction );

protected:
    void closeEvent( QCloseEvent* pEvent );

    void OnNewSchematic( SchematicDocument::Ptr pNewSchematic );

    bool TestClosingDocument( DocViewWidget& docView );

protected:
    // DocumentChangeObserver
    virtual void OnDocumentChanged( Document* pDocument );
    virtual void OnDocumentError( Document* pDocument, const std::string& strErrorMsg );
    virtual void OnDocumentSuccess( Document* pDocument );

public:
    QComboBox* getCompilationModeComboBox() const { return m_pCompilationModeComboBox; }

    void queueIdleTimer() { QTimer::singleShot( 0, this, SLOT( OnIdle() ) ); }
public slots:
    void OnIdle();
    void OnDocumentSaved( const void* pDocument );

    void OnFloatingWidgetCreated( ads::CFloatingDockContainer* pFloatingWidget );
    void OnFocusedDockWidgetCloseRequested( ads::CFloatingDockContainer* pDockContainer );
    void OnDockWidgetCloseRequested( ads::CDockWidget* DockWidget );
    void OnDockWidgetClosed( ads::CDockWidget* DockWidget );
    /*
        void OnDockWidgetRemoved( ads::CDockWidget* DockWidget );
        void OnDockWidgetAboutToBeRemoved( ads::CDockWidget* DockWidget );
        */
    void OnFocusedDockWidgetChanged( ads::CDockWidget* pOld, ads::CDockWidget* pNew );

    void OnNewSchematic();
    void OnLoadSchematic( ClipboardMsg msg );
    void OnLoad();
    void OnSaveAll();

protected:
    static MainWindow*          m_pThis;
    boost::asio::yield_context* m_pYield_ctx;
    Ui::MainWindow*             m_pMainWindowImpl;
    ads::CDockManager*          m_pDockManager;
    DocumentViewMap             m_docViewMap;
    ActionMap                   m_actionRefCountMap;
    Toolbox::Ptr                m_pToolbox;
    QComboBox*                  m_pCompilationModeComboBox;
};

} // namespace editor

#endif // MAINWINDOW_H
