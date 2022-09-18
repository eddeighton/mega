//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
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