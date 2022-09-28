

#include "database/common/environment_archive.hpp"

#include "common/string.hpp"

namespace mega
{
namespace io
{

ArchiveEnvironment::ArchiveEnvironment( const boost::filesystem::path& archiveFilePath )
    : m_fileArchive( archiveFilePath )
{
    m_fileArchive.verify();
}

// FileSystem
std::unique_ptr< std::istream > ArchiveEnvironment::read( const BuildFilePath& filePath ) const
{
    return m_fileArchive.read( filePath );
}
std::unique_ptr< std::ostream > ArchiveEnvironment::write_temp( const BuildFilePath&     filePath,
                                                                boost::filesystem::path& tempFilePath ) const
{
    THROW_RTE( "Invalid use of retail environment" );
}
void ArchiveEnvironment::temp_to_real( const BuildFilePath& filePath ) const
{
    THROW_RTE( "Invalid use of retail environment" );
}

std::unique_ptr< std::istream > ArchiveEnvironment::read( const SourceFilePath& filePath ) const
{
    return m_fileArchive.read( filePath );
}
std::unique_ptr< std::ostream > ArchiveEnvironment::write_temp( const SourceFilePath&    filePath,
                                                                boost::filesystem::path& tempFilePath ) const
{
    THROW_RTE( "Invalid use of retail environment" );
}
void ArchiveEnvironment::temp_to_real( const SourceFilePath& filePath ) const
{
    THROW_RTE( "Invalid use of retail environment" );
}

} // namespace io
} // namespace mega