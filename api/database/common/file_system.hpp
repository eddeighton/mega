

#ifndef FILE_SYSTEM_21_APRIL_2022
#define FILE_SYSTEM_21_APRIL_2022

#include "sources.hpp"

#include "common/hash.hpp"

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>

#include <memory>

namespace mega
{
    namespace io
    {
        class FileSystem
        {
        public:
            virtual ~FileSystem() {}

            // stash
            virtual common::HashCode getBuildHashCode( const ComponentListingFilePath& key ) const                            = 0;
            virtual void             setBuildHashCode( const ComponentListingFilePath& key, common::HashCode hashCode ) const = 0;
            virtual void             setBuildHashCode( const ComponentListingFilePath& key ) const                            = 0;
            virtual void             stash( const ComponentListingFilePath& file, const common::HashCode& code ) const        = 0;
            virtual bool             restore( const ComponentListingFilePath& file, const common::HashCode& code ) const      = 0;

            virtual common::HashCode getBuildHashCode( const CompilationFilePath& key ) const                            = 0;
            virtual void             setBuildHashCode( const CompilationFilePath& key, common::HashCode hashCode ) const = 0;
            virtual void             setBuildHashCode( const CompilationFilePath& key ) const                            = 0;
            virtual void             stash( const CompilationFilePath& file, const common::HashCode& code ) const        = 0;
            virtual bool             restore( const CompilationFilePath& file, const common::HashCode& code ) const      = 0;

            virtual common::HashCode getBuildHashCode( const SourceFilePath& key ) const                            = 0;
            virtual void             setBuildHashCode( const SourceFilePath& key, common::HashCode hashCode ) const = 0;
            virtual void             setBuildHashCode( const SourceFilePath& key ) const                            = 0;
            virtual void             stash( const SourceFilePath& file, const common::HashCode& code ) const        = 0;
            virtual bool             restore( const SourceFilePath& file, const common::HashCode& code ) const      = 0;

            // file io
            virtual std::unique_ptr< boost::filesystem::ifstream > read( const ComponentListingFilePath& filePath ) const         = 0;
            virtual std::unique_ptr< boost::filesystem::ofstream > write_temp( const ComponentListingFilePath& filePath,
                                                                               boost::filesystem::path&        tempFilePath ) const      = 0;
            virtual void                                           temp_to_real( const ComponentListingFilePath& filePath ) const = 0;

            virtual std::unique_ptr< boost::filesystem::ifstream > read( const CompilationFilePath& filePath ) const         = 0;
            virtual std::unique_ptr< boost::filesystem::ofstream > write_temp( const CompilationFilePath& filePath,
                                                                               boost::filesystem::path&   tempFilePath ) const = 0;
            virtual void                                           temp_to_real( const CompilationFilePath& filePath ) const = 0;

            virtual std::unique_ptr< boost::filesystem::ifstream > read( const SourceFilePath& filePath ) const            = 0;
            virtual std::unique_ptr< boost::filesystem::ofstream > write_temp( const SourceFilePath&  filePath,
                                                                               boost::filesystem::path& tempFilePath ) const = 0;
            virtual void                                           temp_to_real( const SourceFilePath& filePath ) const    = 0;
        };
    } // namespace io
} // namespace mega

#endif // FILE_SYSTEM_21_APRIL_2022
