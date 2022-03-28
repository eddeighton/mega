#ifndef IO_MANIFEST_26_MAR_2022
#define IO_MANIFEST_26_MAR_2022

#include "object.hpp"
#include "file_info.hpp"
#include "environment.hpp"

#include <boost/filesystem/path.hpp>

#include <memory>

namespace mega
{
namespace io
{
    class Component;
    class Manifest
    {
    public:
        using PtrCst = std::shared_ptr< const Manifest >;

        Manifest();

        // Load an existing manifest from the specified file.
        Manifest( const boost::filesystem::path& filepath );

        // Construct a manifest from the source and build directory by
        // recursively analysing existing source listing files.
        Manifest( const Environment& environment );

        std::shared_ptr< Component > getComponent() const { return m_pComponent; }
        void                         collectFileInfos( std::vector< FileInfo >& fileInfos ) const;

        void load( std::istream& is );
        void save( std::ostream& os ) const;
        void load( const boost::filesystem::path& filepath );
        void save( const boost::filesystem::path& filepath ) const;

        // remove all source listing and manifest files from the source and build directories
        static void removeManifestAndSourceListingFiles( const boost::filesystem::path& sourceDirectory,
                                                         const boost::filesystem::path& buildDirectory );

    private:
        void        addComponent( std::shared_ptr< Component > pComponent );
        void        constructRecurse( std::shared_ptr< Component >   pComponent,
                                      const Environment&             environment,
                                      const boost::filesystem::path& buildDirIter );
        static void removeManifestAndSourceListingFilesRecurse( const Environment&             environment,
                                                                const boost::filesystem::path& iteratorDir );

    private:
        std::shared_ptr< Component > m_pComponent;
    };

    inline std::istream& operator>>( std::istream& is, Manifest& manifest )
    {
        manifest.load( is );
        return is;
    }
    inline std::ostream& operator<<( std::ostream& os, const Manifest& manifest )
    {
        manifest.save( os );
        return os;
    }
} // namespace io
} // namespace mega

#endif // IO_MANIFEST_26_MAR_2022