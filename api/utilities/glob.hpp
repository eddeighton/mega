



#ifndef GLOB_29_APRIL_2022
#define GLOB_29_APRIL_2022

#include "boost/filesystem/path.hpp"

#include <string>
#include <vector>

namespace mega
{
    namespace utilities
    {
        struct Glob
        {
            boost::filesystem::path source_file;
            std::string             glob;
            void*                   pDiagnostic = nullptr;
        };

        class GlobException : public std::runtime_error
        {
        public:
            GlobException( const std::string& str ) : std::runtime_error( str ) {}
        };

        using FilePathVector = std::vector< boost::filesystem::path >;

        void resolveGlob( const Glob& glob, const boost::filesystem::path& srcDir, FilePathVector& results );

    } // namespace io

} // namespace mega

#endif // GLOB_29_APRIL_2022
