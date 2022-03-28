#ifndef IO_FILE_SYSTEM_26_MAR_2022
#define IO_FILE_SYSTEM_26_MAR_2022

#include "environment.hpp"
#include "file.hpp"
#include "manifest.hpp"
#include "stages.hpp"

#include <memory>
#include <type_traits>

namespace mega
{
namespace io
{
    class FileSystem
    {
    public:
        using FileMap = std::map< Object::FileID, File::Ptr >;
        using FileMapCst = std::map< Object::FileID, File::PtrCst >;

        FileSystem( const Environment& environment )
            : m_environment( environment )
            , m_manifest( m_environment.project_manifest() )
        {
        }

        const Manifest& getManifest() const
        {
            return m_manifest;
        }

        template < typename TFileType >
        inline Object::FileID getFileID() const;

        template < typename TStage >
        inline FileMapCst getReadableFiles() const;

        template < typename TStage >
        inline FileMap getWritableFiles() const;

    private:
        const Environment&            m_environment;
        const Manifest                m_manifest;

        Object::FileID m_testFileID;
    };

    template <>
    inline Object::FileID FileSystem::getFileID< ObjectAST >() const
    {
        return m_testFileID;
    }

    template <>
    inline FileSystem::FileMapCst FileSystem::getReadableFiles< stage::ObjectParse >() const
    {
        /*File::Ptr pManifestFile = std::make_shared< Manifest >( "manifest.txt", Object::MANIFEST_FILE );

        // preload all files
        pManifestFile->preload();

        // load all files
        pManifestFile->load();*/

        FileSystem::FileMapCst readableFiles;
        // readableFiles.insert( std::make_pair( pManifestFile->getFileID(), pManifestFile ) );

        return readableFiles;
    }

    template <>
    inline FileSystem::FileMap FileSystem::getWritableFiles< stage::ObjectParse >() const
    {
        FileSystem::FileMap writableFiles;

        // FileInfo fileInfo { FileInfo::ObjectAST, 0U,
        // File::Ptr pTestFile = std::make_shared< TestFile >( "foobar.txt", m_testFileID );
        // writableFiles.insert( std::make_pair( m_testFileID, pTestFile ) );

        return writableFiles;
    }

} // namespace io
} // namespace mega

#endif // IO_FILE_SYSTEM_26_MAR_2022
