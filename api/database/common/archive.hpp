#ifndef ARCHIVE_21_APRIL_2022
#define ARCHIVE_21_APRIL_2022

#include "database/common/sources.hpp"
#include "database/model/manifest.hxx"

#include "boost/filesystem/path.hpp"

#include <iostream>
#include <memory>

namespace mega
{
    namespace io
    {

        class ReadArchive
        {
        public:
            ReadArchive( const boost::filesystem::path& filePath );

            std::unique_ptr< std::istream > read( const SourceFilePath& filePath ) const;
            std::unique_ptr< std::istream > read( const BuildFilePath& filePath ) const;

            static void compile_archive( const boost::filesystem::path& filePath, const Manifest& manifest,
                                         const boost::filesystem::path& srcDir, const boost::filesystem::path& buildDir );

        private:
            struct Pimpl;
            std::shared_ptr< Pimpl > m_pImpl;
        };

    } // namespace io
} // namespace mega

#endif // ARCHIVE_21_APRIL_2022
