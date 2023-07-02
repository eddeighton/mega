
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
    QObject::connect( &m_itemModel, &ItemModel::dataChanged, this, &SchematicView::OnItemModelDataChanged );
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

    selectContext( m_pEdit.get() );

    m_pSchematicDocument->setCompilationConfig( m_compilationConfig );

    onDocumentUpdate();
    
    CmdZoomToAll();
}

void SchematicView::OnItemModelDataChanged( const QModelIndex& topLeft, const QModelIndex& bottomRight,
                                            const QList< int >& roles )
{
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
    CMD_CONNECT( actionView_Walls        , CmdViewWalls );
    CMD_CONNECT( actionView_Port         , CmdViewPort );
    CMD_CONNECT( actionView_Partition    , CmdViewPartition );
    CMD_CONNECT( actionView_Properties   , CmdViewProperties );
    CMD_CONNECT( actionView_Lanes        , CmdViewLanes );
    CMD_CONNECT( actionView_Linings      , CmdViewLinings );
    CMD_CONNECT( actionView_Placement    , CmdViewPlacement );
    CMD_CONNECT( actionView_Values       , CmdViewValues );
    CMD_CONNECT( actionView_Visibility   , CmdViewVisibility );

    m_pMainWindow->getCompilationModeComboBox()->setCurrentIndex( m_compilationConfig );
    QObject::connect( m_pMainWindow->getCompilationModeComboBox(), 
        SIGNAL( currentIndexChanged( int ) ), this, SLOT( OnCompilationModeChanged( int ) ) );

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
    CMD_DISCONNECT( actionView_Port, CmdViewPort );
    CMD_DISCONNECT( actionView_Partition, CmdViewPartition );
    CMD_DISCONNECT( actionView_Properties, CmdViewProperties );
    CMD_DISCONNECT( actionView_Lanes, CmdViewLanes );
    CMD_DISCONNECT( actionView_Linings, CmdViewLinings );
    CMD_DISCONNECT( actionView_Placement, CmdViewPlacement );
    CMD_DISCONNECT( actionView_Values, CmdViewValues );
    CMD_DISCONNECT( actionView_Visibility, CmdViewVisibility );
    
    QObject::disconnect( m_pMainWindow->getCompilationModeComboBox(), 
        SIGNAL( currentIndexChanged( int ) ), this, SLOT( OnCompilationModeChanged( int ) ) );
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

        menu.addAction( m_pMainWindow->getUI()->actionZoomToAll );
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

void SchematicView::configureCompilationStage( schematic::CompilationStage stage )
{
    m_compilationConfig = stage;
    m_pSchematicDocument->setCompilationConfig( m_compilationConfig );
    onDocumentUpdate();
}

void SchematicView::OnCompilationModeChanged( int iIndex )
{
    configureCompilationStage( static_cast< schematic::CompilationStage >( iIndex ) );
}

void SchematicView::CmdViewSiteContour()
{
    configureCompilationStage( schematic::eStage_SiteContour );
}

void SchematicView::CmdViewWalls()
{
    configureCompilationStage( schematic::eStage_Extrusion );
}

void SchematicView::CmdViewPort()
{
    configureCompilationStage( schematic::eStage_Port );
}

void SchematicView::CmdViewPartition()
{
    configureCompilationStage( schematic::eStage_Partition );
}

void SchematicView::CmdViewProperties()
{
    configureCompilationStage( schematic::eStage_Properties );
}

void SchematicView::CmdViewLanes()
{
    configureCompilationStage( schematic::eStage_Lanes );
}

void SchematicView::CmdViewLinings()
{
    configureCompilationStage( schematic::eStage_Linings );
}

void SchematicView::CmdViewPlacement()
{
    configureCompilationStage( schematic::eStage_Placement );
}

void SchematicView::CmdViewValues()
{
    configureCompilationStage( schematic::eStage_Values );
}

void SchematicView::CmdViewVisibility()
{
    configureCompilationStage( schematic::eStage_Visibility );
}

} // namespace editor