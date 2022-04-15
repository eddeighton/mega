
#ifndef SOURCES_15_APRIL_2022
#define SOURCES_15_APRIL_2022

#include "boost/filesystem/path.hpp"

namespace mega
{
    namespace io
    {
        class manifest
        {
            static boost::filesystem::path EXTENSION;
        public:
            manifest( const boost::filesystem::path& filePath )
                : m_filePath( filePath )
            {
            }

            const boost::filesystem::path& path() const { return m_filePath; }

            static const boost::filesystem::path& extension() { return EXTENSION; }

        private:
            boost::filesystem::path m_filePath;
        };

        class mega
        {
            static boost::filesystem::path EXTENSION;
        public:
            mega( const boost::filesystem::path& filePath )
                : m_filePath( filePath )
            {
            }

            const boost::filesystem::path& path() const { return m_filePath; }
            static const boost::filesystem::path& extension() { return EXTENSION; }

        private:
            boost::filesystem::path m_filePath;
        };

    } // namespace io
} // namespace mega

#endif //SOURCES_15_APRIL_2022
