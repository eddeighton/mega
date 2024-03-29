
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
    explicit SchematicView( QWidget* parent, MainWindow* pMainWindow );
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
    void OnCompilationModeChanged( int iIndex );
    void OnItemModelDataChanged( const QModelIndex& topLeft, const QModelIndex& bottomRight,
                                 const QList< int >& roles = QList< int >() );

    // commands
    bool CmdSave();
    bool CmdSaveAs();
    void CmdUndo();
    void CmdRedo();

    void CmdViewSiteContour();
    void CmdViewWalls();
    void CmdViewPort();
    void CmdViewPartition();
    void CmdViewProperties();
    void CmdViewLanes();
    void CmdViewLinings();
    void CmdViewPlacement();
    void CmdViewValues();
    void CmdViewVisibility();


private:
    void configureCompilationStage( schematic::CompilationStage stage );

private:
    SchematicDocument::Ptr        m_pSchematicDocument;
    schematic::EditSchematic::Ptr m_pEdit;
};

} // namespace editor

#endif // SCHEMATIC_VIEW_04_FEB_2021