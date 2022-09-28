

#include "database/common/storer.hpp"
#include "database/common/file_header.hpp"

#include "database/model/environment.hxx"

#include "common/file.hpp"

namespace boost
{
namespace archive
{
MegaOArchive::MegaOArchive( std::ostream& os, std::set< const mega::io::ObjectInfo* >& objectInfos )
    : binary_oarchive( os )
    , m_objectInfos( objectInfos )
{
}

void MegaOArchive::objectInfo( const mega::io::ObjectInfo* pObjectInf ) { m_objectInfos.insert( pObjectInf ); }

} // namespace archive
} // namespace boost

namespace mega
{
namespace io
{
Storer::Storer( const FileSystem& fileSystem, const CompilationFilePath& filePath, boost::filesystem::path& tempFile )
    : m_pFileStream( fileSystem.write_temp( filePath, tempFile ) )
    , m_archive( *m_pFileStream, m_objectInfos )
{
    {
        mega::U64 manifestHash = 0U;
        FileHeader  fileHeader( Environment::getVersion(), manifestHash );
        m_archive&  fileHeader;
    }
}

} // namespace io
} // namespace mega