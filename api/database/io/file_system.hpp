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
        FileSystem( const Environment&                       environment,
                    std::optional< boost::filesystem::path > object )
            : m_environment( environment )
            , m_object( object )
            , m_manifest( m_environment.project_manifest() )
        {
        }

        const Manifest& getManifest() const { return m_manifest; }

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
            for ( FileMap::const_iterator i = m_readableFiles.begin(), iEnd = m_readableFiles.end();
                  i != iEnd; ++i )
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
            for ( FileMap::const_iterator i = m_writableFiles.begin(), iEnd = m_writableFiles.end();
                  i != iEnd; ++i )
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

        ObjectInfo::FileID m_writableFileIDMap[ FileInfo::TOTAL_FILE_TYPES ]
            = { ObjectInfo::NO_FILE };

        FileMap m_readableFiles;
        FileMap m_writableFiles;
    };

    template < typename TStage >
    inline void FileSystem::load()
    {
        getReadableFiles< TStage >();
        getWritableFiles< TStage >();

        VERIFY_RTE( !m_writableFiles.empty() );

        // load the readable files
        for ( FileMap::const_iterator i = m_readableFiles.begin(), iEnd = m_readableFiles.end();
              i != iEnd; ++i )
        {
            i->second->preload( *this, m_manifest );
        }
        for ( FileMap::const_iterator i = m_readableFiles.begin(), iEnd = m_readableFiles.end();
              i != iEnd; ++i )
        {
            i->second->load( m_manifest );
        }
    }

    template < typename TStage >
    inline void FileSystem::store() const
    {
        for ( FileMap::const_iterator i = m_writableFiles.begin(), iEnd = m_writableFiles.end();
              i != iEnd; ++i )
        {
            i->second->store( m_manifest );
        }
    }

    template < typename StageType >
    inline void FileSystem::getReadableFiles()
    {
        for ( const FileInfo& fileInfo : m_manifest.getCompilationFileInfos() )
        {
            //if ( !m_object.has_value() || !fileInfo.getObjectSourceFilePath().has_value()
            //     || ( fileInfo.getObjectSourceFilePath() == m_object ) )
            {
                switch ( fileInfo.getFileType() )
                {
                    // clang-format off
                    // clang-format on
#define FILE_TYPE( file_type, stage_type )                                      \
    case FileInfo::file_type:                                                              \
    {                                                                                      \
        if ( static_cast< size_t >( FileInfo::stage_type )                                 \
             < static_cast< size_t >( StageType::Stage ) )                                 \
        {                                                                                  \
            m_readableFiles.insert( std::make_pair(                                        \
                fileInfo.getFileID(), std::make_shared< file::file_type >( fileInfo ) ) ); \
        }                                                                                  \
    }                                                                                      \
    break;
#include "database/model/file_types_global.hxx"
#include "database/model/file_types_object.hxx"
#undef FILE_TYPE
                    default:
                        break;
                }
            }
        }
    }

    template < typename StageType >
    inline void FileSystem::getWritableFiles()
    {
        for ( const FileInfo& fileInfo : m_manifest.getCompilationFileInfos() )
        {
            if ( !m_object.has_value() || !fileInfo.getObjectSourceFilePath().has_value()
                 || ( fileInfo.getObjectSourceFilePath() == m_object ) )
            {
                switch ( fileInfo.getFileType() )
                {
                    // clang-format off
                    // clang-format on
#define FILE_TYPE( file_type, stage_type )                                      \
    case FileInfo::file_type:                                                              \
    {                                                                                      \
        if ( static_cast< size_t >( FileInfo::stage_type )                                 \
             == static_cast< size_t >( StageType::Stage ) )                                \
        {                                                                                  \
            m_writableFiles.insert( std::make_pair(                                        \
                fileInfo.getFileID(), std::make_shared< file::file_type >( fileInfo ) ) ); \
            m_writableFileIDMap[ FileInfo::file_type ] = fileInfo.getFileID();             \
        }                                                                                  \
    }                                                                                      \
    break;
#include "database/model/file_types_global.hxx"
#include "database/model/file_types_object.hxx"
#undef FILE_TYPE
                    default:
                        break;
                }
            }
        }
    }

} // namespace io
} // namespace mega

#endif // IO_FILE_SYSTEM_26_MAR_2022
