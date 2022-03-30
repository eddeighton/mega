#ifndef IO_MANIFEST_26_MAR_2022
#define IO_MANIFEST_26_MAR_2022

#include "object.hpp"
#include "file_info.hpp"
#include "environment.hpp"

#include "database/io/archive.hpp"

#include <boost/filesystem/path.hpp>

#include <memory>

namespace mega
{
namespace io
{
    class Component;
    class Manifest
    {
        using FileInfoVector = std::vector< FileInfo >;
    public:
        using PtrCst = std::shared_ptr< const Manifest >;

        Manifest();

        // Load an existing manifest from the specified file.
        Manifest( const boost::filesystem::path& filepath );

        // Construct a manifest from the source and build directory by
        // recursively analysing existing source listing files.
        Manifest( const Environment& environment, const std::vector< boost::filesystem::path >& sourceListings );

        const FileInfoVector& getFileInfos() const { return m_fileInfos; }

        void load( const boost::filesystem::path& filepath );
        void save( const boost::filesystem::path& filepath ) const;

        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            archive & m_fileInfos;
        }

    private:
        FileInfoVector m_fileInfos;
    };

} // namespace io
} // namespace mega

#endif // IO_MANIFEST_26_MAR_2022