#ifndef IO_FILE_SYSTEM_26_MAR_2022
#define IO_FILE_SYSTEM_26_MAR_2022

#include "database/io/file_info.hpp"
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
        using FileMap       = std::map< Object::FileID, File::Ptr >;
        using FileMapCst    = std::map< Object::FileID, File::PtrCst >;

        FileSystem( const Environment& environment, std::optional< boost::filesystem::path > object )
            : m_environment( environment )
            , m_object( object )
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
        inline FileMapCst getReadableFiles();

        template < typename TStage >
        inline FileMap getWritableFiles();

    private:
        const Environment&                       m_environment;
        std::optional< boost::filesystem::path > m_object;
        const Manifest                           m_manifest;

        Object::FileID m_fileIDs[ FileInfo::TOTAL_FILE_TYPES ] = { Object::NO_FILE };
    };

    template <>
    inline FileSystem::FileMapCst FileSystem::getReadableFiles< stage::ObjectParse >()
    {
        VERIFY_RTE( m_object.has_value() );
        FileSystem::FileMapCst readableFiles;
        return readableFiles;
    }

    template <>
    inline FileSystem::FileMap FileSystem::getWritableFiles< stage::ObjectParse >()
    {
        VERIFY_RTE( m_object.has_value() );
        
        FileSystem::FileMap writableFiles;

        std::vector< FileInfo > fileInfos;
        m_manifest.collectFileInfos(fileInfos);
        for( const FileInfo& fileInfo : fileInfos )
        {
            if( fileInfo.m_objectSourceFilePath == m_object )
            {
                switch( fileInfo.m_fileType )
                {
                    case FileInfo::ObjectSourceFile:
                        break;
                    case FileInfo::ObjectAST:
                        writableFiles.insert( std::make_pair( fileInfo.m_fileID, std::make_shared< ObjectAST >( fileInfo ) ) );
                        m_fileIDs[ FileInfo::ObjectAST ] = fileInfo.m_fileID;
                        break;
                    case FileInfo::ObjectBody:
                        writableFiles.insert( std::make_pair( fileInfo.m_fileID, std::make_shared< ObjectBody >( fileInfo ) ) );
                        m_fileIDs[ FileInfo::ObjectBody ] = fileInfo.m_fileID;
                        break;
                    default:
                        break;
                }
            }
        }

        return writableFiles;
    }

    template <>
    inline Object::FileID FileSystem::getFileID< ObjectAST >() const
    {
        return m_fileIDs[ FileInfo::ObjectAST ];
    }

    template <>
    inline Object::FileID FileSystem::getFileID< ObjectBody >() const
    {
        return m_fileIDs[ FileInfo::ObjectBody ];
    }

} // namespace io
} // namespace mega

#endif // IO_FILE_SYSTEM_26_MAR_2022
