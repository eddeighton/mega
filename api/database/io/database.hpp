#ifndef IO_DATABASE_25_MARCH_2022
#define IO_DATABASE_25_MARCH_2022

#include "object.hpp"
#include "generics.hpp"
#include "environment.hpp"
#include "file_system.hpp"
#include "stages.hpp"

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

        Database( const Environment& environment, std::optional< boost::filesystem::path > object = std::optional< boost::filesystem::path >() )
            : m_fileSystem( environment, object )
        {
            m_fileSystem.load< Stage >();
        }

        void store() const
        {
            m_fileSystem.store< Stage >();
        }
        
        template < typename T >
        File::PtrCst getReadableFile() const
        {
            static_assert( std::is_class< typename T::FileType >::value, "Type missing FileType" );
            File::PtrCst   pFile = m_fileSystem.getReadableFile< typename T::FileType >();
            VERIFY_RTE_MSG( pFile,
                            "Failed to get compilation file for " << typeid( T ).name() << " in stage: " << typeid( Stage ).name() );
            return pFile;
        }

        template < typename T >
        File::Ptr getWritableFile() const
        {
            static_assert( std::is_class< typename T::FileType >::value, "Type missing FileType" );
            File::Ptr      pFile = m_fileSystem.getWritableFile< typename T::FileType >();
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

    };

} // namespace io
} // namespace mega

#endif // IO_DATABASE_25_MARCH_2022
