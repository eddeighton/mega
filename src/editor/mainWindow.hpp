
#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "DockManager.h"

#include "messages.hpp"

#include <QMainWindow>

#ifndef Q_MOC_RUN

#include "toolbox.hpp"
#include "document.hpp"

#include "common/scheduler.hpp"

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
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

        struct DocViewWidget
        {
            Document::Ptr pDocument;
            QWidget* pView = nullptr;
            ads::CDockWidget* pDockWidget = nullptr;
            bool bFocussed = false;
        };
        using DocumentViewMap = std::map< ads::CDockWidget*, DocViewWidget >;

        using ActionMap = std::map< QAction*, int >;
        
        void fifoMonitor();
    public:
        explicit MainWindow(QWidget *parent = 0);
        virtual ~MainWindow();

        Ui::MainWindow* getUI() { return m_pMainWindowImpl; }
        Toolbox::Ptr getToolbox() const { return m_pToolbox; }

        void addActionRef( QAction* pAction );
        void decActionRef( QAction* pAction );

    protected:
        void closeEvent( QCloseEvent* pEvent );
        
        void OnNewSchematic( SchematicDocument::Ptr pNewSchematic );

        bool TestClosingDocument( DocViewWidget& docView );
    public:
    
    signals:
        void OnDocumentTaskCompleted( const void* );
        
    public slots:
        void OnDocumentTaskCompletedSlot( const void* pDocument );
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

        void OnNewBase();
        void OnNewShip();
        void OnLoadSchematic( ClipboardMsg msg );
        void OnLoad();
        void OnSaveAll();
        

    protected:
        Ui::MainWindow* m_pMainWindowImpl;
        ads::CDockManager* m_pDockManager;
        DocumentViewMap m_docViewMap;
        ActionMap m_actionRefCountMap;
        Toolbox::Ptr m_pToolbox;
        
        task::StatusFIFO m_fifo;
        task::Scheduler m_scheduler;
        std::thread m_fifoMonitorThread;
        std::atomic< bool > m_bQuit;
    };

}

#endif // MAINWINDOW_H
