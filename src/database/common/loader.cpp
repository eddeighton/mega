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

#include "database/common/loader.hpp"
#include "database/common/file.hpp"

#include "common/file.hpp"

#include "database/model/file_info.hxx"
#include "database/model/manifest.hxx"

namespace boost
{
    namespace archive
    {
        MegaIArchive::MegaIArchive( std::istream& os, ::data::ObjectPartLoader& loader )
            : binary_iarchive( os )
            , m_loader( loader )
        {
        }
    } // namespace archive
} // namespace boost

namespace mega
{
    namespace io
    {
        Loader::Loader( const Manifest& manifest, const boost::filesystem::path& filePath, ::data::ObjectPartLoader& loader )
            : m_runtimeManifest( manifest )
            , m_pFileStream( boost::filesystem::createBinaryInputFileStream( filePath ) )
            , m_archive( *m_pFileStream, loader )
        {
            Manifest loadedManifest;
            m_archive >> loadedManifest;

            // calculate mapping from the old fileIDs in the file to the new runtime ones in the
            // m_runtimeManifest
            std::size_t szHighest = 0U;
            for ( const FileInfo& fileInfo : loadedManifest.getCompilationFileInfos() )
            {
                if ( fileInfo.getFileID() + 1 > szHighest )
                {
                    szHighest = fileInfo.getFileID() + 1;
                }
            }
            m_archive.m_fileIDLoadedToRuntime.resize( szHighest, ObjectInfo::NO_FILE );

            for ( const FileInfo& fileInfo : loadedManifest.getCompilationFileInfos() )
            {
                for ( const FileInfo& runtimeFileInfo : m_runtimeManifest.getCompilationFileInfos() )
                {
                    if ( runtimeFileInfo.getFilePath() == fileInfo.getFilePath() )
                    {
                        VERIFY_RTE( fileInfo.getFileID() != ObjectInfo::NO_FILE );
                        VERIFY_RTE( runtimeFileInfo.getFileID() != ObjectInfo::NO_FILE );
                        m_archive.m_fileIDLoadedToRuntime[ fileInfo.getFileID() ] = runtimeFileInfo.getFileID();
                        break;
                    }
                }
            }
        }

    } // namespace io
} // namespace mega