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

#ifndef INDEXED_FILE_25_MAR_2022
#define INDEXED_FILE_25_MAR_2022

#include "object_info.hpp"
#include "object_info.hpp"
#include "generics.hpp"
#include "object.hpp"
#include "object_loader.hpp"
#include "data_pointer.hpp"
#include "file_system.hpp"

#include "database/types/sources.hpp"

#include "database/model/file_info.hxx"

#include "common/hash.hpp"
#include "common/stash.hpp"

#include "nlohmann/json.hpp"

#include <cstddef>
#include <memory>
#include <optional>

namespace mega::io
{
class Manifest;
class File;
class Loader;

class File
{
public:
    using Ptr    = std::shared_ptr< File >;
    using PtrCst = std::shared_ptr< const File >;

private:
    const FileSystem&         m_fileSystem;
    FileInfo                  m_info;
    FileInfo::Stage           m_stage;
    data::ObjectPartLoader&   m_objectLoader;
    Object::Array             m_objects;
    std::shared_ptr< Loader > m_pLoader;

    void preload( Loader& loader );

public:
    File( const FileSystem& fileSystem, const FileInfo& info, FileInfo::Stage stage,
          data::ObjectPartLoader& objectLoader )
        : m_fileSystem( fileSystem )
        , m_info( info )
        , m_stage( stage )
        , m_objectLoader( objectLoader )
    {
    }

    mega::U64 getTotalObjects() const { return m_objects.size(); }
    Object*   getObject( ObjectInfo::Index objectIndex ) const
    {
        VERIFY_RTE( objectIndex >= 0 );
        VERIFY_RTE( objectIndex < m_objects.size() );
        return m_objects[ objectIndex ];
    }

    FileInfo::Type             getType() const { return m_info.getFileType(); }
    ObjectInfo::FileID         getFileID() const { return m_info.getFileID(); }
    const CompilationFilePath& getFilePath() const { return m_info.getFilePath(); }
    const SourceFilePath&      getObjectSourceFilePath() const { return m_info.getObjectSourceFilePath(); }

    void           load( const Manifest& manifest );
    void           load_post( const Manifest& manifest );
    task::FileHash save_temp( const Manifest& manifest ) const;
    void           to_json( const Manifest& manifest, nlohmann::json& data ) const;

    template < typename T, typename... Args >
    inline data::Ptr< T > construct( Args... args )
    {
        T* pNewObject = new T(
            m_objectLoader, io::ObjectInfo( T::Object_Part_Type_ID, m_info.getFileID(), m_objects.size() ), args... );
        m_objects.push_back( pNewObject );
        return data::Ptr< T >( m_objectLoader, pNewObject );
    }

    inline Range< Object::Array::const_iterator > range() const
    {
        return Range< Object::Array::const_iterator >( m_objects.cbegin(), m_objects.cend() );
    }

    inline Range< Object::Array::iterator > range()
    {
        return Range< Object::Array::iterator >( m_objects.begin(), m_objects.end() );
    }
};

} // namespace mega::io

#endif // INDEXED_FILE_25_MAR_2022
