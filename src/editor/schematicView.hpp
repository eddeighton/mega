
#ifndef SCHEMATIC_VIEW_04_FEB_2021
#define SCHEMATIC_VIEW_04_FEB_2021

#include "glyphView.hpp"

#ifndef Q_MOC_RUN

#include "document.hpp"

#include "schematic/editSchematic.hpp"

#endif

namespace editor
{
class MainWindow;

class SchematicView : public GlyphView
{
    Q_OBJECT

public:
    explicit SchematicView( QWidget *parent, MainWindow* pMainWindow );
    ~SchematicView();
    
    virtual void postCreate( SchematicDocument::Ptr pDocument );
    virtual void onViewFocussed();
    virtual void onViewUnfocussed();
    virtual void onDocumentUpdate();

    virtual void mousePressEvent( QMouseEvent* pEvent );
    virtual void mouseMoveEvent( QMouseEvent* pEvent );
    virtual void mouseReleaseEvent( QMouseEvent* pEvent );
    
signals:
    void OnDocumentSaved( const void* );

public slots:
    //commands
    bool CmdSave();
    bool CmdSaveAs();
    void CmdUndo();
    void CmdRedo();
    
    void CmdViewText        ();
    void CmdViewPoints      ();
    void CmdViewConnections ();
    void CmdViewSite        ();
    void CmdViewSiteContour ();
    void CmdViewWalls       ();
    void CmdViewCompilation ();
    void CmdViewFloor       ();
    void CmdViewSections    ();
    void CmdViewVisibility  ();
    
private:
    void configureCompilationStage( schematic::Schematic::CompilationStage stage, bool bEnable );

private:
    SchematicDocument::Ptr m_pSchematicDocument;

    schematic::EditSchematic::Ptr m_pEdit;
    
    schematic::File::CompilationConfig m_compilationConfig;
};

}


#endif //SCHEMATIC_VIEW_04_FEB_2021