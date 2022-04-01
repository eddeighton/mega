#ifndef IO_FILE_SYSTEM_26_MAR_2022
#define IO_FILE_SYSTEM_26_MAR_2022

#include "file_info.hpp"
#include "generics.hpp"
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
    class FileSystem : public FileAccess
    {
        using FileMap = std::map< ObjectInfo::FileID, File::Ptr >;
        using FileMapCst = std::map< ObjectInfo::FileID, File::PtrCst >;

    public:
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
        inline File::Ptr getWritableFile() const
        {
            File::Ptr               pFile;
            FileMap::const_iterator iFind = m_writableFiles.find( getFileID< TFileType >() );
            if ( iFind != m_writableFiles.end() )
                pFile = iFind->second;
            return pFile;
        }

        template < typename TStage >
        inline void load();

        template < typename TStage >
        inline void store() const;

        virtual File::PtrCst getFile( ObjectInfo::FileID fileId ) const
        {
            File::PtrCst            pFile;
            FileMap::const_iterator iFind = m_readableFiles.find( fileId );
            if ( iFind != m_readableFiles.end() )
                pFile = iFind->second;
            return pFile;
        }

        inline Range< MultiRangeIter< Object::Array::const_iterator > > range_cst() const
        {
            std::vector< Range< Object::Array::const_iterator > > ranges;
            for ( FileMap::const_iterator i = m_readableFiles.begin(),
                                          iEnd = m_readableFiles.end();
                  i != iEnd;
                  ++i )
            {
                ranges.push_back( static_cast< const File* >( i->second.get() )->range() );
            }
            return Range< MultiRangeIter< Object::Array::const_iterator > >(
                MultiRangeIter< Object::Array::const_iterator >( ranges ),
                MultiRangeIter< Object::Array::const_iterator >( ranges, true ) );
        }

        inline Range< MultiRangeIter< Object::Array::iterator > > range() const
        {
            std::vector< Range< Object::Array::iterator > > ranges;
            for ( FileMap::const_iterator i = m_writableFiles.begin(),
                                          iEnd = m_writableFiles.end();
                  i != iEnd;
                  ++i )
            {
                ranges.push_back( i->second->range() );
            }
            return Range< MultiRangeIter< Object::Array::iterator > >(
                MultiRangeIter< Object::Array::iterator >( ranges ),
                MultiRangeIter< Object::Array::iterator >( ranges, true ) );
        }

    private:
        template < typename TFileType >
        inline ObjectInfo::FileID getFileID() const
        {
            return m_writableFileIDMap[ TFileType::Type ];
        }

        template < typename TStage >
        inline void getReadableFiles();

        template < typename TStage >
        inline void getWritableFiles();

    private:
        const Environment&                       m_environment;
        std::optional< boost::filesystem::path > m_object;
        const Manifest                           m_manifest;

        ObjectInfo::FileID m_writableFileIDMap[ FileInfo::TOTAL_FILE_TYPES ] = { ObjectInfo::NO_FILE };

        FileMap m_readableFiles;
        FileMap m_writableFiles;
    };

    template < typename TStage >
    inline void FileSystem::load()
    {
        getReadableFiles< TStage >();
        getWritableFiles< TStage >();

        // load the readable files
        for ( FileMap::const_iterator i = m_readableFiles.begin(),
                                      iEnd = m_readableFiles.end();
              i != iEnd;
              ++i )
        {
            i->second->preload( *this, m_manifest );
        }
        for ( FileMap::const_iterator i = m_readableFiles.begin(),
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
        for ( FileMap::const_iterator i = m_writableFiles.begin(),
                                      iEnd = m_writableFiles.end();
              i != iEnd;
              ++i )
        {
            i->second->store( m_manifest );
        }
    }

    // stage::Component
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
                m_writableFileIDMap[ FileInfo::Component ] = fileInfo.getFileID();
                break;
            default:
                break;
            }
        }
    }

    // stage::ObjectParse
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
                case FileInfo::ObjectAST:
                    m_writableFiles.insert( std::make_pair( fileInfo.getFileID(), std::make_shared< file::ObjectAST >( fileInfo ) ) );
                    m_writableFileIDMap[ FileInfo::ObjectAST ] = fileInfo.getFileID();
                    break;
                case FileInfo::ObjectBody:
                    m_writableFiles.insert( std::make_pair( fileInfo.getFileID(), std::make_shared< file::ObjectBody >( fileInfo ) ) );
                    m_writableFileIDMap[ FileInfo::ObjectBody ] = fileInfo.getFileID();
                    break;
                default:
                    break;
                }
            }
        }
    }

    // stage::DependencyAnalysis
    template <>
    inline void FileSystem::getReadableFiles< stage::DependencyAnalysis >()
    {
        VERIFY_RTE( !m_object.has_value() );
        for ( const FileInfo& fileInfo : m_manifest.getFileInfos() )
        {
            switch ( fileInfo.getFileType() )
            {
            case FileInfo::Component:
                m_readableFiles.insert( std::make_pair( fileInfo.getFileID(), std::make_shared< file::Component >( fileInfo ) ) );
                break;
            case FileInfo::ObjectAST:
                m_readableFiles.insert( std::make_pair( fileInfo.getFileID(), std::make_shared< file::ObjectAST >( fileInfo ) ) );
                break;
            case FileInfo::ObjectBody:
                m_readableFiles.insert( std::make_pair( fileInfo.getFileID(), std::make_shared< file::ObjectBody >( fileInfo ) ) );
                break;
            default:
                break;
            }
        }
    }

    template <>
    inline void FileSystem::getWritableFiles< stage::DependencyAnalysis >()
    {
        VERIFY_RTE( !m_object.has_value() );

        for ( const FileInfo& fileInfo : m_manifest.getFileInfos() )
        {
            switch ( fileInfo.getFileType() )
            {
            case FileInfo::DependencyAnalysis:
                m_writableFiles.insert( std::make_pair( fileInfo.getFileID(), std::make_shared< file::DependencyAnalysis >( fileInfo ) ) );
                m_writableFileIDMap[ FileInfo::DependencyAnalysis ] = fileInfo.getFileID();
                break;
            default:
                break;
            }
        }
    }

} // namespace io
} // namespace mega

#endif // IO_FILE_SYSTEM_26_MAR_2022
