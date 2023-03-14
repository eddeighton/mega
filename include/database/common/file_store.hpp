
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

#ifndef GUARD_2023_March_14_file_store
#define GUARD_2023_March_14_file_store

#include "object_info.hpp"
#include "file.hpp"
#include "object_loader.hpp"

#include "database/model/manifest.hxx"

#include <map>

namespace mega::io
{

class FileStore : public data::ObjectPartLoader
{
public:
    using Ptr        = std::shared_ptr< FileStore >;
    using FileMap    = std::map< ObjectInfo::FileID, File::Ptr >;
    using FileMapCst = std::map< ObjectInfo::FileID, File::PtrCst >;

    FileStore( const Environment& environment, const Manifest& manifest, FileInfo::Stage stage )
        : m_environment( environment )
        , m_manifest( manifest )
        , m_stage( stage )
    {
    }

    // ObjectPartLoader
    virtual Object* load( const ObjectInfo& objectInfo )
    {
        File::PtrCst pFile;
        {
            auto iFind = m_readwriteFiles.find( objectInfo.getFileID() );
            if( iFind != m_readwriteFiles.end() )
            {
                pFile = iFind->second;
            }
            else
            {
                auto iFind2 = m_readOnlyFiles.find( objectInfo.getFileID() );
                if( iFind2 != m_readOnlyFiles.end() )
                {
                    pFile = iFind2->second;
                }
                else
                {
                    pFile = m_manifest.loadFile( m_environment, *this, m_stage, objectInfo.getFileID() );
                }
            }
        }
        return pFile->getObject( objectInfo.getIndex() );
    }

    File::PtrCst getLoadedReadOnly( ObjectInfo::FileID fileID ) const
    {
        {
            auto iFind = m_readOnlyFiles.find( fileID );
            if( iFind != m_readOnlyFiles.end() )
            {
                return iFind->second;
            }
        }
        {
            auto iFind = m_readwriteFiles.find( fileID );
            if( iFind != m_readwriteFiles.end() )
            {
                return iFind->second;
            }
        }
        return nullptr;
    }

    void loadedReadOnly( ObjectInfo::FileID fileID, File::PtrCst pFile )
    {
        VERIFY_RTE( m_readOnlyFiles.insert( { fileID, pFile } ).second );
    }
    void createdReadWrite( ObjectInfo::FileID fileID, File::Ptr pFile )
    {
        VERIFY_RTE( m_readwriteFiles.insert( { fileID, pFile } ).second );
    }

    void to_json( nlohmann::json& data ) const
    {
        {
            for( auto i = m_readOnlyFiles.begin(), iEnd = m_readOnlyFiles.end(); i != iEnd; ++i )
            {
                i->second->to_json( m_manifest, data );
            }
        }
        {
            for( auto i = m_readwriteFiles.begin(), iEnd = m_readwriteFiles.end(); i != iEnd; ++i )
            {
                i->second->to_json( m_manifest, data );
            }
        }
    }

private:
    const Environment& m_environment;
    const Manifest&    m_manifest;
    FileInfo::Stage    m_stage;
    FileMapCst         m_readOnlyFiles;
    FileMap            m_readwriteFiles;
};
} // namespace mega::io

#endif // GUARD_2023_March_14_file_store
