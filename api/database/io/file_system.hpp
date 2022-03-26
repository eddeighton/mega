#ifndef IO_FILE_SYSTEM_26_MAR_2022
#define IO_FILE_SYSTEM_26_MAR_2022

#include "file.hpp"
#include "manifest.hpp"

#include <memory>
#include <type_traits>

namespace mega
{
namespace io
{
    class FileSystem
    {
    public:
        using FileMap = std::map< Object::FileID, file::File::Ptr >;
        using FileMapCst = std::map< Object::FileID, file::File::PtrCst >;

        FileSystem( const boost::filesystem::path& sourceDir, const boost::filesystem::path& buildDir )
            : m_sourceDir( sourceDir )
            , m_buildDir( buildDir )
        {
            // attempt to load the manifest file
            const boost::filesystem::path manifestPath = Manifest::filepath( buildDir );
            m_pManifest = Manifest::load( manifestPath );
        }

        FileSystem( const boost::filesystem::path& sourceDir, const boost::filesystem::path& buildDir, Manifest::PtrCst pManifest )
            : m_sourceDir( sourceDir )
            , m_buildDir( buildDir )
            , m_pManifest( pManifest )
        {
            const boost::filesystem::path manifestPath = Manifest::filepath( buildDir );
            m_pManifest->save( manifestPath );
        }

        const Manifest& getManifest() const { VERIFY_RTE(m_pManifest); return *m_pManifest; }

        template < typename TFileType >
        inline Object::FileID getFileID() const;

        template < typename TStage >
        inline FileMapCst getReadableFiles() const;

        template < typename TStage >
        inline FileMap getWritableFiles() const;

    private:
        const boost::filesystem::path m_sourceDir;
        const boost::filesystem::path m_buildDir;

        Manifest::PtrCst m_pManifest;

        Object::FileID m_testFileID;
    };

    template <>
    inline Object::FileID FileSystem::getFileID< file::TestFile >() const
    {
        return m_testFileID;
    }

    template <>
    inline FileSystem::FileMapCst FileSystem::getReadableFiles< stage::Test >() const
    {
        /*file::File::Ptr pManifestFile = std::make_shared< file::Manifest >( "manifest.txt", Object::MANIFEST_FILE );

        // preload all files
        pManifestFile->preload();

        // load all files
        pManifestFile->load();*/

        FileSystem::FileMapCst readableFiles;
        // readableFiles.insert( std::make_pair( pManifestFile->getFileID(), pManifestFile ) );

        return readableFiles;
    }

    template <>
    inline FileSystem::FileMap FileSystem::getWritableFiles< stage::Test >() const
    {
        FileSystem::FileMap writableFiles;

        file::File::Ptr pTestFile = std::make_shared< file::TestFile >( "foobar.txt", m_testFileID );

        writableFiles.insert( std::make_pair( m_testFileID, pTestFile ) );

        return writableFiles;
    }

} // namespace io
} // namespace mega

#endif // IO_FILE_SYSTEM_26_MAR_2022
