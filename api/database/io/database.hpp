#ifndef IO_DATABASE_25_MARCH_2022
#define IO_DATABASE_25_MARCH_2022

#include "object.hpp"
#include "generics.hpp"
#include "environment.hpp"
#include "file_system.hpp"

#include <memory>
#include <type_traits>

namespace mega
{
namespace io
{
    template < typename TStage >
    class Database
    {
    public:
        using Stage = TStage;

        Database( const Environment& environment )
            : m_fileSystem( environment )
        {
            m_readableFiles = m_fileSystem.getReadableFiles< Stage >();
            m_writableFiles = m_fileSystem.getWritableFiles< Stage >();
        }

        void store() const
        {
            for ( FileSystem::FileMap::const_iterator i = m_writableFiles.begin(),
                                                      iEnd = m_writableFiles.end();
                  i != iEnd;
                  ++i )
            {
                i->second->store( m_fileSystem.getManifest() );
            }
        }

        File::PtrCst getReadableFile( Object::FileID fileID ) const
        {
            File::PtrCst                     pFile;
            FileSystem::FileMapCst::const_iterator iFind = m_readableFiles.find( fileID );
            if ( iFind != m_readableFiles.end() )
                pFile = iFind->second;
            return pFile;
        }

        File::Ptr getWritableFile( Object::FileID fileID ) const
        {
            File::Ptr                     pFile;
            FileSystem::FileMap::const_iterator iFind = m_writableFiles.find( fileID );
            if ( iFind != m_writableFiles.end() )
                pFile = iFind->second;
            return pFile;
        }

        template < typename T >
        File::PtrCst getReadableFile() const
        {
            static_assert( std::is_class< typename T::FileType >::value, "Type missing FileType" );
            const Object::FileID fileID = m_fileSystem.getFileID< typename T::FileType >();
            File::PtrCst   pFile = getReadableFile( fileID );
            VERIFY_RTE_MSG( pFile,
                            "Failed to get compilation file for " << typeid( T ).name() << " in stage: " << typeid( Stage ).name() );
            return pFile;
        }

        template < typename T >
        File::Ptr getWritableFile() const
        {
            static_assert( std::is_class< typename T::FileType >::value, "Type missing FileType" );
            const Object::FileID fileID = m_fileSystem.getFileID< typename T::FileType >();
            File::Ptr      pFile = getWritableFile( fileID );
            VERIFY_RTE_MSG( pFile,
                            "Failed to get compilation file for " << typeid( T ).name() << " in stage: " << typeid( Stage ).name() );
            return pFile;
        }

        template < typename T, typename... Args >
        inline T* construct( Args... args )
        {
            return getWritableFile< T >()->template construct< T, Args... >( args... );
        }

        template < typename T >
        inline std::vector< T* > many() const
        {
            return getWritableFile< T >()->template collect< T >( []( const Object::Array& objects )
                                                                  { return io::many< T >( objects ); } );
        }

        template < typename T >
        inline std::vector< const T* > many_cst() const
        {
            return getReadableFile< T >()->template collect< T >( []( const Object::Array& objects )
                                                                  { return io::many_cst< T >( objects ); } );
        }

        template < typename T >
        inline T* one() const
        {
            return getWritableFile< T >()->template collect< T >( []( const Object::Array& objects )
                                                                  { return io::one< T >( objects ); } );
        }

        template < typename T >
        inline const T* one_cst() const
        {
            return getReadableFile< T >()->template collect< T >( []( const Object::Array& objects )
                                                                  { return io::one_cst< T >( objects ); } );
        }

        template < typename T >
        inline T* oneOpt() const
        {
            return getWritableFile< T >()->template collect< T >( []( const Object::Array& objects )
                                                                  { return io::oneOpt< T >( objects ); } );
        }

        template < typename T >
        inline const T* oneOpt_cst() const
        {
            return getReadableFile< T >()->template collect< T >( []( const Object::Array& objects )
                                                                  { return io::oneOpt_cst< T >( objects ); } );
        }

        template < typename T >
        inline T* root() const
        {
            return getWritableFile< T >()->template collect< T >( []( const Object::Array& objects )
                                                                  { return io::root< T >( objects ); } );
        }

        template < typename T >
        inline const T* root_cst() const
        {
            return getReadableFile< T >()->template collect< T >( []( const Object::Array& objects )
                                                                  { return io::root_cst< T >( objects ); } );
        }

    private:
        FileSystem m_fileSystem;

        FileSystem::FileMapCst m_readableFiles;
        FileSystem::FileMap    m_writableFiles;
    };

} // namespace io
} // namespace mega

#endif // IO_DATABASE_25_MARCH_2022
