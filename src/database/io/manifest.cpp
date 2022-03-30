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

    Manifest::Manifest()
    {
    }

    Manifest::Manifest( const boost::filesystem::path& filepath )
    {
        load( filepath );
    }
    Manifest::Manifest( const Environment& environment, const std::vector< boost::filesystem::path >& componentInfos )
    {
        ObjectInfo::FileID fileIDCounter = 0;

        // generate the component compilation file
        {
            const FileInfo fileInfo(
                FileInfo::Component,
                fileIDCounter++,
                environment.component() );
            m_fileInfos.push_back( fileInfo );
        }

        for ( const boost::filesystem::path& componentInfoPath : componentInfos )
        {
            ComponentInfo componentInfo;
            {
                VERIFY_RTE_MSG( boost::filesystem::exists( componentInfoPath ),
                                "Failed to locate file: " << componentInfoPath.string() );
                std::ifstream inputFileStream( componentInfoPath.native().c_str(), std::ios::in );
                if ( !inputFileStream.good() )
                {
                    THROW_RTE( "Failed to open file: " << componentInfoPath.string() );
                }
                InputArchiveType ia( inputFileStream );
                ia >> boost::serialization::make_nvp( "componentInfo", componentInfo );
            }

            for ( const boost::filesystem::path& sourceFilePath : componentInfo.getSourceFiles() )
            {
                const FileInfo fileInfo(
                    FileInfo::ObjectSourceFile,
                    fileIDCounter++,
                    sourceFilePath,
                    sourceFilePath );
                m_fileInfos.push_back( fileInfo );

                {
                    FileInfo compilationFile(
                        FileInfo::ObjectAST,
                        fileIDCounter++,
                        environment.objectAST( fileInfo.getFilePath() ),
                        fileInfo.getFilePath() );
                    m_fileInfos.push_back( compilationFile );
                }

                {
                    FileInfo compilationFile(
                        FileInfo::ObjectBody,
                        fileIDCounter++,
                        environment.objectBody( fileInfo.getFilePath() ),
                        fileInfo.getFilePath() );
                    m_fileInfos.push_back( compilationFile );
                }
            }
        }
    }

    void Manifest::load( const boost::filesystem::path& filepath )
    {
        VERIFY_RTE_MSG( boost::filesystem::exists( filepath ),
                        "Failed to locate file: " << filepath.string() );
        std::ifstream inputFileStream( filepath.native().c_str(), std::ios::in );
        if ( !inputFileStream.good() )
        {
            THROW_RTE( "Failed to open file: " << filepath.string() );
        }
        InputArchiveType ia( inputFileStream );
        ia >> boost::serialization::make_nvp( "manifest", *this );
    }

    void Manifest::save( const boost::filesystem::path& filepath ) const
    {
        std::ofstream outputFileStream( filepath.native().c_str(), std::ios_base::trunc | std::ios_base::out );
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
