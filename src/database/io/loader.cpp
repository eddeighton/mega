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

#include "database/io/loader.hpp"
#include "database/io/file.hpp"

#include "common/file.hpp"
#include "database/io/file_info.hpp"

namespace mega
{
namespace io
{
    Loader::Loader( const Manifest& manifest, const FileAccess& fileAccess, const boost::filesystem::path& filePath )
        : m_runtimeManifest( manifest )
        , m_fileAccess( fileAccess )
        , m_pFileStream( boost::filesystem::createBinaryInputFileStream( filePath ) )
        , m_archive( *m_pFileStream )
    {
        Manifest loadedManifest;
        m_archive >> loadedManifest;

        // calculate mapping from the old fileIDs in the file to the new runtime ones in the m_runtimeManifest
        std::size_t szHighest = 0U;
        for ( const FileInfo& fileInfo : loadedManifest.getFileInfos() )
        {
            if ( fileInfo.getFileID() + 1 > szHighest )
            {
                szHighest = fileInfo.getFileID() + 1;
            }
        }
        m_fileIDLoadedToRuntime.resize( szHighest, ObjectInfo::NO_FILE );
        m_fileIDRuntimeToLoaded.resize( szHighest, ObjectInfo::NO_FILE );

        for ( const FileInfo& fileInfo : loadedManifest.getFileInfos() )
        {
            for ( const FileInfo& runtimeFileInfo : m_runtimeManifest.getFileInfos() )
            {
                if ( runtimeFileInfo.getFilePath() == fileInfo.getFilePath() )
                {
                    m_fileIDLoadedToRuntime[ fileInfo.getFileID() ] = runtimeFileInfo.getFileID();
                    m_fileIDRuntimeToLoaded[ runtimeFileInfo.getFileID() ] = fileInfo.getFileID();
                    break;
                }
            }
        }
    }

    Object* Loader::loadObjectRef()
    {
        Object*            pObject = nullptr;
        ObjectInfo::FileID fileID = ObjectInfo::NO_FILE;
        load( fileID );
        if ( fileID != ObjectInfo::NO_FILE )
        {
            // map the loaded fileID to the runtime fileID
            fileID = m_fileIDLoadedToRuntime[ fileID ];
            VERIFY_RTE_MSG( fileID != ObjectInfo::NO_FILE, "Failed to map fileID to valid file" );

            File::PtrCst pFile = m_fileAccess.getFile( fileID );
            VERIFY_RTE_MSG( pFile, "Failed to find valid file for fileID" );

            ObjectInfo::Index szIndex = ObjectInfo::NO_INDEX;
            load( szIndex );
            VERIFY_RTE( szIndex < pFile->getTotalObjects() );

            pObject = pFile->getObject( szIndex );
        }
        return pObject;
    }
} // namespace io
} // namespace mega
