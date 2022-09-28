

#include "database/common/loader.hpp"
#include "database/common/file.hpp"
#include "database/common/file_header.hpp"
#include "database/common/exception.hpp"

#include "common/file.hpp"

#include "database/types/sources.hpp"
#include "database/model/file_info.hxx"
#include "database/model/manifest.hxx"
#include "database/model/environment.hxx"

#include <utility>

namespace boost
{
namespace archive
{
MegaIArchive::MegaIArchive( std::istream& is, std::set< mega::io::ObjectInfo* >& objectInfos,
                            ::data::ObjectPartLoader& loader )
    : binary_iarchive( is )
    , m_objectInfos( objectInfos )
    , m_loader( loader )
{
}

void MegaIArchive::objectInfo( mega::io::ObjectInfo* pObjectInfo ) { m_objectInfos.insert( pObjectInfo ); }
} // namespace archive
} // namespace boost

namespace mega
{
namespace io
{
Loader::Loader( const FileSystem& fileSystem, const CompilationFilePath& filePath,
                ::data::ObjectPartLoader& loader )
    : m_pFileStream( fileSystem.read( filePath ) )
    , m_archive( *m_pFileStream, m_objectInfos, loader )
{
    {
        FileHeader fileHeader;
        m_archive& fileHeader;
        if ( fileHeader.getVersion() != Environment::getVersion() )
        {
            std::ostringstream os;
            os << "Compilation file: " << filePath.path().string() << 
                "has version: " << fileHeader.getVersion() << " when current version is: " << Environment::getVersion();
            throw mega::io::DatabaseVersionException( os.str() );
        }
    }
}

void Loader::postLoad( const Manifest& runtimeManifest )
{
    // load the manifest from the end of the file
    // m_archive.

    Manifest   loadedManifest;
    m_archive& loadedManifest;

    std::vector< mega::io::ObjectInfo::FileID > fileIDLoadedToRuntime;

    // calculate mapping from the old fileIDs in the file to the new runtime ones in the
    // runtimeManifest
    ObjectInfo::FileID szHighest = 0;
    for ( const FileInfo& fileInfo : loadedManifest.getCompilationFileInfos() )
    {
        szHighest = std::max( szHighest, fileInfo.getFileID() );
    }
    fileIDLoadedToRuntime.resize( szHighest + 1, ObjectInfo::NO_FILE );

    for ( const FileInfo& fileInfo : loadedManifest.getCompilationFileInfos() )
    {
        bool bFound = false;
        for ( const FileInfo& runtimeFileInfo : runtimeManifest.getCompilationFileInfos() )
        {
            if ( runtimeFileInfo.getFilePath() == fileInfo.getFilePath() )
            {
                VERIFY_RTE( fileInfo.getFileID() != ObjectInfo::NO_FILE );
                VERIFY_RTE( runtimeFileInfo.getFileID() != ObjectInfo::NO_FILE );
                fileIDLoadedToRuntime[ fileInfo.getFileID() ] = runtimeFileInfo.getFileID();
                bFound                                        = true;
                break;
            }
        }
        VERIFY_RTE_MSG(
            bFound, "Failed to locate: " << fileInfo.getFilePath().path().string() << " in runtime manifest" );
    }

    for ( mega::io::ObjectInfo* pObjectInfo : m_objectInfos )
    {
        const ObjectInfo::FileID storedFileID = pObjectInfo->getFileID();
        VERIFY_RTE_MSG( storedFileID != ObjectInfo::NO_FILE, "File ID of NO_FILE" );
        VERIFY_RTE_MSG( storedFileID < fileIDLoadedToRuntime.size(), "Invalid File ID: " << storedFileID );
        const ObjectInfo::FileID mappedFileID = fileIDLoadedToRuntime[ storedFileID ];
        VERIFY_RTE_MSG( mappedFileID != ObjectInfo::NO_FILE, "File ID failed to map" );
        pObjectInfo->setFileID( mappedFileID );
    }
}

} // namespace io
} // namespace mega
