#ifndef INDEXED_FILE_25_MAR_2022
#define INDEXED_FILE_25_MAR_2022

#include "sources.hpp"
#include "object_info.hpp"
#include "object_info.hpp"
#include "generics.hpp"
#include "object.hpp"
#include "object_loader.hpp"
#include "data_pointer.hpp"
#include "file_system.hpp"

#include "database/model/file_info.hxx"

#include "common/hash.hpp"

#include "nlohmann/json.hpp"

#include <memory>
#include <optional>

namespace mega
{
    namespace io
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
            data::ObjectPartLoader&   m_objectLoader;
            Object::Array             m_objects;
            std::shared_ptr< Loader > m_pLoader;

            void preload( Loader& loader );

        public:
            File( const FileSystem& fileSystem, const FileInfo& info, data::ObjectPartLoader& objectLoader )
                : m_fileSystem( fileSystem )
                , m_info( info )
                , m_objectLoader( objectLoader )
            {
            }

            std::size_t getTotalObjects() const { return m_objects.size(); }
            Object*     getObject( ObjectInfo::Index objectIndex ) const
            {
                VERIFY_RTE( objectIndex >= 0 );
                VERIFY_RTE( objectIndex < m_objects.size() );
                return m_objects[ objectIndex ];
            }

            FileInfo::Type             getType() const { return m_info.getFileType(); }
            ObjectInfo::FileID         getFileID() const { return m_info.getFileID(); }
            const CompilationFilePath& getFilePath() const { return m_info.getFilePath(); }
            const SourceFilePath&      getObjectSourceFilePath() const { return m_info.getObjectSourceFilePath(); }

            void load( const Manifest& manifest );
            void load_post( const Manifest& manifest );
            common::HashCode save_temp( const Manifest& manifest ) const;
            void to_json( const Manifest& manifest, nlohmann::json& data ) const;

            template < typename T, typename... Args >
            inline data::Ptr< T > construct( Args... args )
            {
                T* pNewObject
                    = new T( m_objectLoader, io::ObjectInfo( T::Object_Part_Type_ID, m_info.getFileID(), m_objects.size() ), args... );
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

    } // namespace io
} // namespace mega

#endif // INDEXED_FILE_25_MAR_2022
