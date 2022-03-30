#include "database/io/manifest.hpp"
#include "database/io/environment.hpp"
#include "database/io/file_info.hpp"
#include "database/io/component_info.hpp"

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
        Object::FileID fileIDCounter = 0;

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
                inputFileStream >> componentInfo;
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
        inputFileStream >> *this;
    }

    void Manifest::save( const boost::filesystem::path& filepath ) const
    {
        std::ofstream outputFileStream( filepath.native().c_str(), std::ios_base::trunc | std::ios_base::out );
        if ( !outputFileStream.good() )
        {
            THROW_RTE( "Failed to write to file: " << filepath.string() );
        }
        outputFileStream << *this;
    }

    std::istream& operator>>( std::istream& is, Manifest& manifest )
    {
        std::size_t szTotalFileInfos = 0U;
        is >> szTotalFileInfos;
        for ( std::size_t sz = 0U; sz != szTotalFileInfos; ++sz )
        {
            FileInfo fileInfo;
            is >> fileInfo;
            manifest.m_fileInfos.push_back( fileInfo );
        }
        return is;
    }

    std::ostream& operator<<( std::ostream& os, const Manifest& manifest )
    {
        os << manifest.m_fileInfos.size();
        for ( Manifest::FileInfoVector::const_iterator i = manifest.m_fileInfos.begin(),
                                                       iEnd = manifest.m_fileInfos.end();
              i != iEnd;
              ++i )
        {
            os << *i;
        }
        return os;
    }

} // namespace io
} // namespace mega
