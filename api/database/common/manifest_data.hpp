#ifndef MANIFEST_DATA_17_JUNE_2022
#define MANIFEST_DATA_17_JUNE_2022

#include "database/types/sources.hpp"

#include "database/model/file_info.hxx"

#include <vector>

namespace mega
{
namespace io
{
struct ManifestData
{
    using MegaSourceFileVector = std::vector< megaFilePath >;
    using CppSourceFileVector  = std::vector< cppFilePath >;
    using FileInfoVector       = std::vector< FileInfo >;

    const manifestFilePath&     getManifestFilePath() const { return m_manifestFilePath; }
    const MegaSourceFileVector& getMegaSourceFiles() const { return m_megaSourceFiles; }
    const CppSourceFileVector&  getCppSourceFiles() const { return m_cppSourceFiles; }
    const FileInfoVector&       getCompilationFileInfos() const { return m_compilationFileInfos; }

    ManifestData filterToObjects( const std::set< const mega::io::ObjectInfo* >& objectInfos ) const
    {
        ManifestData manifest{ m_manifestFilePath, {}, {} };
        {
            std::set< ObjectInfo::FileID > fileIDs;
            for ( const mega::io::ObjectInfo* pObjectInfo : objectInfos )
            {
                fileIDs.insert( pObjectInfo->getFileID() );
            }
            for ( const FileInfo& fileInfo : m_compilationFileInfos )
            {
                if ( fileIDs.count( fileInfo.getFileID() ) )
                {
                    manifest.m_compilationFileInfos.push_back( fileInfo );
                }
            }
        }
        return manifest;
    }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "manifestFilePath", m_manifestFilePath );
        archive& boost::serialization::make_nvp( "megaSourceFiles", m_megaSourceFiles );
        archive& boost::serialization::make_nvp( "cppSourceFiles", m_cppSourceFiles );
        archive& boost::serialization::make_nvp( "compilationFileInfos", m_compilationFileInfos );
    }

    manifestFilePath     m_manifestFilePath;
    MegaSourceFileVector m_megaSourceFiles;
    CppSourceFileVector  m_cppSourceFiles;
    FileInfoVector       m_compilationFileInfos;
};

} // namespace io
} // namespace mega

#endif // MANIFEST_DATA_17_JUNE_2022
