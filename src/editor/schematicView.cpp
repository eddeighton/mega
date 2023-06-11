
#include "schematicView.hpp"

#include "mainWindow.hpp"
#include "ui_mainWindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QProcessEnvironment>

#ifndef Q_MOC_RUN

#include "qtUtils.hpp"

#endif

namespace editor
{

SchematicView::SchematicView( QWidget* pParent, MainWindow* pMainWindow )
    : GlyphView( pParent, pMainWindow )
{
}

SchematicView::~SchematicView() = default;

void SchematicView::postCreate( SchematicDocument::Ptr pDocument )
{
    GlyphView::postCreate( pDocument );

    m_pSchematicDocument = pDocument;
    VERIFY_RTE( pDocument );

    m_itemModel.OnSchematicFocussed( m_pSchematicDocument->getSchematic() );

    GlyphView* pGlyphView = this;

    m_pEdit.reset( new schematic::EditSchematic( *pGlyphView, m_pSchematicDocument->getSchematic() ) );

    m_pActiveContext = m_pEdit.get();

    m_pSchematicDocument->setCompilationConfig( m_compilationConfig );

    onDocumentUpdate();
}

void SchematicView::onViewFocussed()
{
    GlyphView::onViewFocussed();

    // clang-format off
    CMD_CONNECT( actionSave,    CmdSave );
    CMD_CONNECT( actionSave_As, CmdSaveAs );
    CMD_CONNECT( actionUndo,    CmdUndo );
    CMD_CONNECT( actionRedo,    CmdRedo );
    
    CMD_CONNECT( actionView_SiteContour  , CmdViewSiteContour );
    CMD_CONNECT( actionView_Walls        , CmdViewWalls       );
    CMD_CONNECT( actionView_Analysis     , CmdViewAnalysis    );
    CMD_CONNECT( actionView_Skeleton     , CmdViewSkeleton  );
    
    m_pMainWindow->getUI()->actionView_SiteContour  ->setChecked( m_compilationConfig[ schematic::Schematic::eStage_SiteContour ] );
    m_pMainWindow->getUI()->actionView_Walls        ->setChecked( m_compilationConfig[ schematic::Schematic::eStage_Extrusion ] );
    m_pMainWindow->getUI()->actionView_Analysis     ->setChecked( m_compilationConfig[ schematic::Schematic::eStage_Compilation ] );
    m_pMainWindow->getUI()->actionView_Skeleton     ->setChecked( m_compilationConfig[ schematic::Schematic::eStage_Skeleton ] );
    // clang-format on

    if( m_pSchematicDocument )
    {
        m_itemModel.OnSchematicFocussed( m_pSchematicDocument->getSchematic() );
    }
}

void SchematicView::onViewUnfocussed()
{
    GlyphView::onViewUnfocussed();

    CMD_DISCONNECT( actionSave, CmdSave );
    CMD_DISCONNECT( actionSave_As, CmdSaveAs );
    CMD_DISCONNECT( actionUndo, CmdUndo );
    CMD_DISCONNECT( actionRedo, CmdRedo );

    CMD_DISCONNECT( actionView_SiteContour, CmdViewSiteContour );
    CMD_DISCONNECT( actionView_Walls, CmdViewWalls );
    CMD_DISCONNECT( actionView_Analysis, CmdViewAnalysis );
    CMD_DISCONNECT( actionView_Skeleton, CmdViewSkeleton );
}

void SchematicView::onDocumentUpdate()
{
    m_pEdit->update();
    GlyphView::onDocumentUpdate();
}

void SchematicView::mousePressEvent( QMouseEvent* pEvent )
{
    GlyphView::mousePressEvent( pEvent );
}

void SchematicView::mouseMoveEvent( QMouseEvent* pEvent )
{
    GlyphView::mouseMoveEvent( pEvent );
}

void SchematicView::mouseReleaseEvent( QMouseEvent* pEvent )
{
    GlyphView::mouseReleaseEvent( pEvent );

    if( pEvent->button() == Qt::RightButton )
    {
        // show context menu
        QMenu menu( this );

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

bool SchematicView::CmdSave()
{
    if( !m_pSchematicDocument->getFilePath().has_value() )
    {
        return CmdSaveAs();
    }
    else
    {
        const std::string strFilePath = m_pSchematicDocument->getFilePath().value().string();
        try
        {
            m_pSchematicDocument->save();
            emit OnDocumentSaved( m_pSchematicDocument.get() );
            return true;
        }
        catch( std::exception& ex )
        {
            std::ostringstream os;
            os << "Error saving file: " << strFilePath << " : " << ex.what();
            QMessageBox::warning( this, tr( "Schematic Edittor" ), QString::fromUtf8( os.str().c_str() ) );
        }
        return false;
    }
}

bool SchematicView::CmdSaveAs()
{
    QProcessEnvironment environment    = QProcessEnvironment::systemEnvironment();
    QString             strDefaultPath = environment.value( "BLUEPRINT_TOOLBOX_PATH" );
    if( m_pSchematicDocument->getFilePath().has_value() )
    {
        const std::string strFilePath = m_pSchematicDocument->getFilePath().value().string();
        strDefaultPath                = QString::fromUtf8( strFilePath.c_str() );
    }

    QString strFilePath = QFileDialog::getSaveFileName(
        this, tr( "SaveAs Schematic" ), strDefaultPath, tr( "Schematic Files (*.sch)" ) );
    if( !strFilePath.isEmpty() )
    {
        try
        {
            m_pSchematicDocument->saveAs( strFilePath.toStdString() );
            emit OnDocumentSaved( m_pSchematicDocument.get() );
            return true;
        }
        catch( std::exception& ex )
        {
            std::ostringstream os;
            os << "Error saving file: " << strFilePath.toStdString() << " : " << ex.what();
            QMessageBox::warning( this, tr( "Schematic Edittor" ), QString::fromUtf8( os.str().c_str() ) );
        }
    }
    return false;
}

void SchematicView::CmdUndo()
{
    m_pEdit.reset();
    m_pActiveContext = nullptr;

    m_pDocument->undo();

    GlyphView* pGlyphView = this;

    m_pEdit.reset( new schematic::EditSchematic( *pGlyphView, m_pSchematicDocument->getSchematic() ) );

    selectContext( m_pEdit.get() );

    m_pEdit->update();
}

void SchematicView::CmdRedo()
{
    m_pEdit.reset();
    m_pActiveContext = nullptr;

    m_pDocument->redo();

    GlyphView* pGlyphView = this;

    m_pEdit.reset( new schematic::EditSchematic( *pGlyphView, m_pSchematicDocument->getSchematic() ) );

    selectContext( m_pEdit.get() );

    m_pEdit->update();
}

void SchematicView::configureCompilationStage( schematic::Schematic::CompilationStage stage, bool bEnable )
{
    if( bEnable )
    {
        m_compilationConfig.set( stage );
    }
    else
    {
        m_compilationConfig.reset( stage );
    }

    m_pSchematicDocument->setCompilationConfig( m_compilationConfig );

    onDocumentUpdate();
}

void SchematicView::CmdViewSiteContour()
{
    configureCompilationStage(
        schematic::Schematic::eStage_SiteContour, m_pMainWindow->getUI()->actionView_SiteContour->isChecked() );
}

void SchematicView::CmdViewWalls()
{
    configureCompilationStage(
        schematic::Schematic::eStage_Extrusion, m_pMainWindow->getUI()->actionView_Walls->isChecked() );
}

void SchematicView::CmdViewAnalysis()
{
    configureCompilationStage(
        schematic::Schematic::eStage_Compilation, m_pMainWindow->getUI()->actionView_Analysis->isChecked() );
}

void SchematicView::CmdViewSkeleton()
{
    configureCompilationStage(
        schematic::Schematic::eStage_Skeleton, m_pMainWindow->getUI()->actionView_Skeleton->isChecked() );
}

} // namespace editor