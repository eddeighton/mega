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
        
        template < typename T, typename... Args >
        inline T* construct( Args... args )
        {
            static_assert( std::is_class< typename T::FileType >::value, "Type missing FileType" );
            File::Ptr      pFile = m_fileSystem.getWritableFile< typename T::FileType >();

            VERIFY_RTE_MSG( pFile,
                            "Failed to get compilation file for " << typeid( T ).name() << " in stage: " << typeid( Stage ).name() );
            return pFile->template construct< T, Args... >( args... );
        }

        template < typename T >
        inline std::vector< const T* > many_cst() const
        {
            return io::many< const T >( m_fileSystem.range_cst() );
        }

        template < typename T >
        inline std::vector< T* > many() const
        {
            return io::many< T >( m_fileSystem.range() );
        }

        template < typename T >
        inline const T* one_cst() const
        {
            return io::one< const T >( m_fileSystem.range_cst() );
        }

        template < typename T >
        inline T* one() const
        {
            return io::one< T >( m_fileSystem.range() );
        }

        template < typename T >
        inline const T* one_opt_cst() const
        {
            return io::oneOpt< const T >( m_fileSystem.range_cst() );
        }

        template < typename T >
        inline T* one_opt() const
        {
            return io::oneOpt< T >( m_fileSystem.range() );
        }

    private:
        FileSystem m_fileSystem;

    };

} // namespace io
} // namespace mega

#endif // IO_DATABASE_25_MARCH_2022
