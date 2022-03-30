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
        using FileMap = std::map< Object::FileID, File::Ptr >;
        using FileMapCst = std::map< Object::FileID, File::PtrCst >;

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
        inline File::PtrCst getReadableFile() const;

        template < typename TFileType >
        inline File::Ptr getWritableFile() const;

        template < typename TStage >
        inline void load();

        template < typename TStage >
        inline void store() const;

    private:
        template < typename TFileType >
        inline Object::FileID getFileID() const;

        template < typename TStage >
        inline void getReadableFiles();

        template < typename TStage >
        inline void getWritableFiles();

    private:
        const Environment&                       m_environment;
        std::optional< boost::filesystem::path > m_object;
        const Manifest                           m_manifest;

        Object::FileID m_fileIDs[ FileInfo::TOTAL_FILE_TYPES ] = { Object::NO_FILE };

        FileSystem::FileMap m_readableFiles;
        FileSystem::FileMap m_writableFiles;
    };

    template < typename TFileType >
    inline File::PtrCst FileSystem::getReadableFile() const
    {
        File::PtrCst                        pFile;
        FileSystem::FileMap::const_iterator iFind = m_readableFiles.find( getFileID< TFileType >() );
        if ( iFind != m_readableFiles.end() )
            pFile = iFind->second;
        return pFile;
    }

    template < typename TFileType >
    inline File::Ptr FileSystem::getWritableFile() const
    {
        File::Ptr                           pFile;
        FileSystem::FileMap::const_iterator iFind = m_writableFiles.find( getFileID< TFileType >() );
        if ( iFind != m_writableFiles.end() )
            pFile = iFind->second;
        return pFile;
    }

    template < typename TStage >
    inline void FileSystem::load()
    {
        getReadableFiles< TStage >();
        getWritableFiles< TStage >();

        //load the readable files
        for ( FileSystem::FileMap::const_iterator i = m_readableFiles.begin(),
                                                  iEnd = m_readableFiles.end();
              i != iEnd;
              ++i )
        {
            i->second->preload( m_manifest );
        }
        for ( FileSystem::FileMap::const_iterator i = m_readableFiles.begin(),
                                                  iEnd = m_readableFiles.end();
              i != iEnd;
              ++i )
        {
            i->second->load( m_manifest );
        }
    }

    template < typename TStage >
    inline void FileSystem::store() const
    {
        for ( FileSystem::FileMap::const_iterator i = m_writableFiles.begin(),
                                                  iEnd = m_writableFiles.end();
              i != iEnd;
              ++i )
        {
            i->second->store( m_manifest );
        }
    }

    //stage::Component
    template <>
    inline void FileSystem::getReadableFiles< stage::Component >()
    {
    }

    template <>
    inline void FileSystem::getWritableFiles< stage::Component >()
    {
        VERIFY_RTE( !m_object.has_value() );

        for ( const FileInfo& fileInfo : m_manifest.getFileInfos() )
        {
            switch ( fileInfo.getFileType() )
            {
            case FileInfo::Component:
                m_writableFiles.insert( std::make_pair( fileInfo.getFileID(), std::make_shared< file::Component >( fileInfo ) ) );
                m_fileIDs[ FileInfo::Component ] = fileInfo.getFileID();
                break;
            default:
                break;
            }
        }
    }

    //stage::ObjectParse
    template <>
    inline void FileSystem::getReadableFiles< stage::ObjectParse >()
    {
        VERIFY_RTE( m_object.has_value() );
        for ( const FileInfo& fileInfo : m_manifest.getFileInfos() )
        {
            switch ( fileInfo.getFileType() )
            {
            case FileInfo::Component:
                m_readableFiles.insert( std::make_pair( fileInfo.getFileID(), std::make_shared< file::Component >( fileInfo ) ) );
                m_fileIDs[ FileInfo::Component ] = fileInfo.getFileID();
                break;
            default:
                break;
            }
        }
    }

    template <>
    inline void FileSystem::getWritableFiles< stage::ObjectParse >()
    {
        VERIFY_RTE( m_object.has_value() );

        for ( const FileInfo& fileInfo : m_manifest.getFileInfos() )
        {
            if ( fileInfo.getObjectSourceFilePath() == m_object )
            {
                switch ( fileInfo.getFileType() )
                {
                case FileInfo::Component:
                    break;
                case FileInfo::ObjectSourceFile:
                    break;
                case FileInfo::ObjectAST:
                    m_writableFiles.insert( std::make_pair( fileInfo.getFileID(), std::make_shared< file::ObjectAST >( fileInfo ) ) );
                    m_fileIDs[ FileInfo::ObjectAST ] = fileInfo.getFileID();
                    break;
                case FileInfo::ObjectBody:
                    m_writableFiles.insert( std::make_pair( fileInfo.getFileID(), std::make_shared< file::ObjectBody >( fileInfo ) ) );
                    m_fileIDs[ FileInfo::ObjectBody ] = fileInfo.getFileID();
                    break;
                default:
                    break;
                }
            }
        }
    }

    template <>
    inline Object::FileID FileSystem::getFileID< file::Component >() const
    {
        return m_fileIDs[ FileInfo::Component ];
    }

    template <>
    inline Object::FileID FileSystem::getFileID< file::ObjectAST >() const
    {
        return m_fileIDs[ FileInfo::ObjectAST ];
    }

    template <>
    inline Object::FileID FileSystem::getFileID< file::ObjectBody >() const
    {
        return m_fileIDs[ FileInfo::ObjectBody ];
    }

} // namespace io
} // namespace mega

#endif // IO_FILE_SYSTEM_26_MAR_2022
