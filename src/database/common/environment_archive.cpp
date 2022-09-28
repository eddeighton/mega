//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.



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