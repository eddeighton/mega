
#include "document.hpp"
#include "mainWindow.hpp"

#include <QMessageBox>

#ifndef Q_MOC_RUN

#include "map/base.hpp"
#include "map/ship.hpp"
#include "map/factory.hpp"

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
    map::File::Ptr pFile = m_document.getFile();
    VERIFY_RTE( pFile );

    if( m_current.has_value() )
    {
        m_history.push_back( m_current.value() );
    }
    m_future.clear();

    m_current = getUndoFileName();

    boost::filesystem::ensureFoldersExist( m_tempFolder / m_current.value() );

    map::save( pFile, m_tempFolder / m_current.value() );

    m_lastRecord.update();
}

map::File::Ptr Document::UndoHistory::onUndo()
{
    map::File::Ptr pOldFile = m_document.getFile();
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

        return map::load( m_tempFolder / m_current.value() );
    }
    else
    {
        return pOldFile;
    }
}

map::File::Ptr Document::UndoHistory::onRedo()
{
    map::File::Ptr pOldFile = m_document.getFile();
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

        return map::load( m_tempFolder / m_current.value() );
    }
    else
    {
        return pOldFile;
    }
}

//////////////////////////////////////////////////
//////////////////////////////////////////////////
Document::Document( DocumentChangeObserver& observer )
    : m_documentChangeObserver( observer )
    , m_uniqueObjectName( generateUUID() )
    , m_undoHistory( *this )
{
}

Document::Document( DocumentChangeObserver& observer, const boost::filesystem::path& path )
    : m_documentChangeObserver( observer )
    , m_optPath( path )
    , m_uniqueObjectName( generateUUID() )
    , m_undoHistory( *this )
{
}

bool Document::isModified() const
{
    if( map::File::Ptr pFile = getFile() )
    {
        if( pFile->getLastModifiedTickForTree() > m_lastModifiedTick )
        {
            return true;
        }
    }
    return false;
}

void Document::setCompilationConfig( const map::File::CompilationConfig& config )
{
    if( config != m_compilationConfig )
    {
        m_compilationConfig = config;
        m_documentChangeObserver.OnDocumentChanged( this );
    }
}

void Document::onEditted( bool bCommandCompleted )
{
    if( map::File::Ptr pFile = getFile() )
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
SchematicDocument::SchematicDocument( DocumentChangeObserver& observer, map::Schematic::Ptr pSchematic )
    : Document( observer )
    , m_pSchematic( pSchematic )
{
    m_undoHistory.onNewVersion();
}

SchematicDocument::SchematicDocument( DocumentChangeObserver& observer, map::Schematic::Ptr pSchematic,
                                      const boost::filesystem::path& path )
    : Document( observer, path )
    , m_pSchematic( pSchematic )
{
    m_undoHistory.onNewVersion();
}

void SchematicDocument::calculateDerived()
{
    if( m_pSchematic )
    {
        if( m_compilationConfig[ map::Schematic::eStage_SiteContour ] )
        {
            m_pSchematic->task_contours();
        }
        if( m_compilationConfig[ map::Schematic::eStage_Extrusion ] )
        {
            m_pSchematic->task_extrusions();
        }
    }
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

    map::save( m_pSchematic, filePath );

    saved( filePath );
}

void SchematicDocument::undo()
{
    map::File::Ptr      pUndoFile      = m_undoHistory.onUndo();
    map::Schematic::Ptr pUndoSchematic = boost::dynamic_pointer_cast< map::Schematic >( pUndoFile );
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
    map::File::Ptr      pUndoFile      = m_undoHistory.onRedo();
    map::Schematic::Ptr pUndoSchematic = boost::dynamic_pointer_cast< map::Schematic >( pUndoFile );
    VERIFY_RTE( pUndoSchematic );

    if( pUndoSchematic != m_pSchematic )
    {
        m_pSchematic = pUndoSchematic;
        calculateDerived();
    }
    m_documentChangeObserver.OnDocumentChanged( this );
}

} // namespace editor