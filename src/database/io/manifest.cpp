#include "database/io/manifest.hpp"
#include "database/io/environment.hpp"
#include "database/io/file_info.hpp"
#include "database/io/component_info.hpp"
#include "database/io/archive.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>

#include <fstream>

namespace mega
{
namespace io
{

    Manifest::Manifest() {}

    Manifest::Manifest( const boost::filesystem::path& filepath ) { load( filepath ); }
    Manifest::Manifest( const Environment&                            environment,
                        const std::vector< boost::filesystem::path >& componentInfos )
    {
        ObjectInfo::FileID sourceFileID = 0;
        ObjectInfo::FileID compilationFileID = 0;

        for ( const boost::filesystem::path& componentInfoPath : componentInfos )
        {
            ComponentInfo componentInfo;
            {
                VERIFY_RTE_MSG( boost::filesystem::exists( componentInfoPath ),
                                "Failed to locate file: " << componentInfoPath.string() );
                std::ifstream inputFileStream( componentInfoPath.native().c_str(), std::ios::in | std::ios_base::binary );
                if ( !inputFileStream.good() )
                {
                    THROW_RTE( "Failed to open file: " << componentInfoPath.string() );
                }
                InputArchiveType ia( inputFileStream );
                ia >> boost::serialization::make_nvp( "componentInfo", componentInfo );
            }

            for ( const boost::filesystem::path& sourceFilePath : componentInfo.getSourceFiles() )
            {
                {
                    m_sourceFiles.push_back( sourceFilePath );
                }

// clang-format off
// clang-format on
#define FILE_TYPE( filetype, stagetype )                                  \
    {                                                                     \
        FileInfo compilationFile( FileInfo::filetype,                     \
                                  compilationFileID++,                    \
                                  environment.filetype( sourceFilePath ), \
                                  sourceFilePath );                       \
        m_compilationFileInfos.push_back( compilationFile );              \
    }
#include "database/model/file_types_object.hxx"
#undef FILE_TYPE
            }
        }

// clang-format off
// clang-format on
#define FILE_TYPE( filetype, stagetype )                                       \
    {                                                                          \
        FileInfo compilationFile(                                              \
            FileInfo::filetype, compilationFileID++, environment.filetype() ); \
        m_compilationFileInfos.push_back( compilationFile );                   \
    }
#include "database/model/file_types_global.hxx"
#undef FILE_TYPE

    } // namespace io

    const FileInfo& Manifest::getCompilationFilePath( ObjectInfo::FileID fileID ) const
    {
        VERIFY_RTE_MSG( fileID >= 0 && fileID < m_compilationFileInfos.size(),
            "Invalid fileID of: " << fileID );
        const FileInfo& fileInfo = m_compilationFileInfos[ fileID ];
        VERIFY_RTE( fileInfo.getFileID() == fileID );
        return fileInfo;
    }

    void Manifest::getCompilationFileInfos( FileInfo::Type fileType, FileInfoVector& fileInfos ) const
    {
        for( const FileInfo& fileInfo : m_compilationFileInfos )
        {
            if( fileInfo.getFileType() == fileType )
            {
                fileInfos.push_back( fileInfo );
            }
        }
    }

    void Manifest::getCompilationFileInfos( FileInfo::Type fileType, const boost::filesystem::path& objectFile, FileInfoVector& fileInfos ) const
    {
        for( const FileInfo& fileInfo : m_compilationFileInfos )
        {
            if( fileInfo.getFileType() == fileType && fileInfo.getObjectSourceFilePath() == objectFile )
            {
                fileInfos.push_back( fileInfo );
            }
        }
    }
    
    void Manifest::load( const boost::filesystem::path& filepath )
    {
        VERIFY_RTE_MSG(
            boost::filesystem::exists( filepath ), "Failed to locate file: " << filepath.string() );
        std::ifstream inputFileStream( filepath.native().c_str(), std::ios::in | std::ios_base::binary );
        if ( !inputFileStream.good() )
        {
            THROW_RTE( "Failed to open file: " << filepath.string() );
        }
        InputArchiveType ia( inputFileStream );
        ia >> boost::serialization::make_nvp( "manifest", *this );
    }

    void Manifest::save( const boost::filesystem::path& filepath ) const
    {
        std::ofstream outputFileStream(
            filepath.native().c_str(), std::ios_base::trunc | std::ios_base::out | std::ios_base::binary );
        if ( !outputFileStream.good() )
        {
            THROW_RTE( "Failed to write to file: " << filepath.string() );
        }
        OutputArchiveType oa( outputFileStream );
        oa << boost::serialization::make_nvp( "manifest", *this );
    }

} // namespace io
} // namespace mega

BOOST_CLASS_IMPLEMENTATION( mega::io::Manifest, object_serializable )
