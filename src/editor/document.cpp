
#include "document.hpp"
#include "mainWindow.hpp"

#include <QMessageBox>

#ifndef Q_MOC_RUN

#include "schematic/base.hpp"
#include "schematic/ship.hpp"
#include "schematic/factory.hpp"

#include "common/file.hpp"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <sstream>
#include <algorithm>
#include <cctype>

#endif

namespace
{
int eds_isalnum( int ch )
{
    return std::isalnum( ch );
}

std::string style_replace_non_alpha_numeric( const std::string& str, char r )
{
    std::string strResult;
    std::replace_copy_if(
        str.begin(), str.end(), std::back_inserter( strResult ), []( char c ) { return !eds_isalnum( c ); }, r );
    return strResult;
}

std::string generateUUID()
{
    using namespace boost::uuids;

    static random_generator gen;
    uuid                    id = gen();
    std::ostringstream      os;
    os << id;
    return os.str();
}

boost::filesystem::path generateUndoFilePath( const std::string& szUniqueID, std::size_t szIndex )
{
    std::ostringstream os;
    os << "undo_" << szUniqueID << '_'
       << boost::posix_time::to_simple_string( boost::posix_time::second_clock::universal_time() ) << '_' << szIndex;

    boost::filesystem::path newPath = style_replace_non_alpha_numeric( os.str(), '_' );

    return newPath.replace_extension( ".sch" );
}

} // namespace

namespace editor
{

//////////////////////////////////////////////////
//////////////////////////////////////////////////
Document::UndoHistory::UndoHistory( Document& document )
    : m_document( document )
    , m_tempFolder( boost::filesystem::current_path() / "history" )
{
}

Document::UndoHistory::~UndoHistory() = default;

boost::filesystem::path Document::UndoHistory::getUndoFileName() const
{
    std::size_t             szUniqueID = 0;
    boost::filesystem::path newPath;

    // ensure unique
    bool bIsUnique = false;
    while( !bIsUnique )
    {
        newPath = generateUndoFilePath( m_document.getUniqueObjectName(), szUniqueID );
        ++szUniqueID;
        bIsUnique = true;

        if( m_current.has_value() )
        {
            if( m_current.value() == newPath )
            {
                bIsUnique = false;
            }
        }
        if( std::find( m_history.begin(), m_history.end(), newPath ) != m_history.end() )
        {
            bIsUnique = false;
        }
        if( std::find( m_future.begin(), m_future.end(), newPath ) != m_future.end() )
        {
            bIsUnique = false;
        }
    }

    return newPath;
}

void Document::UndoHistory::onNewVersion()
{
    schematic::File::Ptr pFile = m_document.getFile();
    VERIFY_RTE( pFile );

    if( m_current.has_value() )
    {
        m_history.push_back( m_current.value() );
    }
    m_future.clear();

    m_current = getUndoFileName();

    boost::filesystem::ensureFoldersExist( m_tempFolder / m_current.value() );

    schematic::save( pFile, m_tempFolder / m_current.value() );

    m_lastRecord.update();
}

schematic::File::Ptr Document::UndoHistory::onUndo()
{
    schematic::File::Ptr pOldFile = m_document.getFile();
    VERIFY_RTE( pOldFile );

    if( !m_history.empty() )
    {
        if( m_current.has_value() )
        {
            m_future.push_front( m_current.value() );
        }

        m_current = m_history.back();
        m_history.pop_back();

        m_lastRecord.update();

        return schematic::load( m_tempFolder / m_current.value() );
    }
    else
    {
        return pOldFile;
    }
}

schematic::File::Ptr Document::UndoHistory::onRedo()
{
    schematic::File::Ptr pOldFile = m_document.getFile();
    VERIFY_RTE( pOldFile );

    if( !m_future.empty() )
    {
        if( m_current.has_value() )
        {
            m_history.push_back( m_current.value() );
        }

        m_current = m_future.front();
        m_future.pop_front();

        m_lastRecord.update();

        return schematic::load( m_tempFolder / m_current.value() );
    }
    else
    {
        return pOldFile;
    }
}

//////////////////////////////////////////////////
//////////////////////////////////////////////////
Document::Document( DocumentChangeObserver& observer, const schematic::File::CompilationConfig& config )
    : m_documentChangeObserver( observer )
    , m_uniqueObjectName( generateUUID() )
    , m_undoHistory( *this )
    , m_compilationConfig( config )
{
}

Document::Document( DocumentChangeObserver& observer, const schematic::File::CompilationConfig& config,
                    const boost::filesystem::path& path )
    : m_documentChangeObserver( observer )
    , m_optPath( path )
    , m_uniqueObjectName( generateUUID() )
    , m_undoHistory( *this )
    , m_compilationConfig( config )
{
}

bool Document::isModified() const
{
    if( schematic::File::Ptr pFile = getFile() )
    {
        if( pFile->getLastModifiedTickForTree() > m_lastModifiedTick )
        {
            return true;
        }
    }
    return false;
}

void Document::setCompilationConfig( const schematic::File::CompilationConfig& config )
{
    if( config != m_compilationConfig )
    {
        m_compilationConfig = config;
        m_documentChangeObserver.OnDocumentChanged( this );
    }
}

void Document::onEditted( bool bCommandCompleted )
{
    if( schematic::File::Ptr pFile = getFile() )
    {
        if( bCommandCompleted )
        {
            if( pFile->getLastModifiedTickForTree() > m_undoHistory.getLastRecordTick() )
            {
                // generate backup
                m_undoHistory.onNewVersion();
            }
        }
    }
    m_documentChangeObserver.OnDocumentChanged( this );
}

void Document::saved( const boost::filesystem::path& filePath )
{
    m_optPath = filePath;
    m_lastModifiedTick.update();
}

//////////////////////////////////////////////////
//////////////////////////////////////////////////
SchematicDocument::SchematicDocument( DocumentChangeObserver& observer, schematic::Schematic::Ptr pSchematic,
                                      const schematic::File::CompilationConfig& config )
    : Document( observer, config )
    , m_pSchematic( pSchematic )
{
    calculateDerived();

    m_undoHistory.onNewVersion();
}

SchematicDocument::SchematicDocument( DocumentChangeObserver& observer, schematic::Schematic::Ptr pSchematic,
                                      const schematic::File::CompilationConfig& config,
                                      const boost::filesystem::path&            path )
    : Document( observer, config, path )
    , m_pSchematic( pSchematic )
{
    calculateDerived();

    m_undoHistory.onNewVersion();
}

void SchematicDocument::calculateDerived( const schematic::File::CompilationConfig& config )
{
    if( m_pSchematic )
    {
        if( config[ schematic::Schematic::eStage_SiteContour ] )
        {
            m_pSchematic->task_contours();
        }
        if( config[ schematic::Schematic::eStage_Extrusion ] )
        {
            m_pSchematic->task_extrusions();
        }
        if( config[ schematic::Schematic::eStage_Compilation ] )
        {
            m_pSchematic->task_compilation();
        }
    }
}
void SchematicDocument::calculateDerived()
{
    calculateDerived( m_compilationConfig );
}

void SchematicDocument::setCompilationConfig( const schematic::File::CompilationConfig& config )
{
    if( config != m_compilationConfig )
    {
        calculateDerived( config );
    }
    Document::setCompilationConfig( config );
}

void SchematicDocument::onEditted( bool bCommandCompleted )
{
    calculateDerived();
    Document::onEditted( bCommandCompleted );
}

void SchematicDocument::save()
{
    saveAs( m_optPath.value().string() );
}

void SchematicDocument::saveAs( const std::string& strFilePath )
{
    boost::filesystem::path filePath = strFilePath;

    schematic::save( m_pSchematic, filePath );

    saved( filePath );
}

void SchematicDocument::undo()
{
    schematic::File::Ptr      pUndoFile      = m_undoHistory.onUndo();
    schematic::Schematic::Ptr pUndoSchematic = boost::dynamic_pointer_cast< schematic::Schematic >( pUndoFile );
    VERIFY_RTE( pUndoSchematic );

    if( pUndoSchematic != m_pSchematic )
    {
        m_pSchematic = pUndoSchematic;
        calculateDerived();
    }
    m_documentChangeObserver.OnDocumentChanged( this );
}

void SchematicDocument::redo()
{
    schematic::File::Ptr      pUndoFile      = m_undoHistory.onRedo();
    schematic::Schematic::Ptr pUndoSchematic = boost::dynamic_pointer_cast< schematic::Schematic >( pUndoFile );
    VERIFY_RTE( pUndoSchematic );

    if( pUndoSchematic != m_pSchematic )
    {
        m_pSchematic = pUndoSchematic;
        calculateDerived();
    }
    m_documentChangeObserver.OnDocumentChanged( this );
}

} // namespace editor