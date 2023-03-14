//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#ifndef MANIFEST_DATA_17_JUNE_2022
#define MANIFEST_DATA_17_JUNE_2022

#include "database/common/api.hpp"
#include "database/types/sources.hpp"

#include "database/model/file_info.hxx"

#include <vector>

namespace mega::io
{
struct EGDB_EXPORT ManifestData
{
    using MegaSourceFileVector      = std::vector< megaFilePath >;
    using CppSourceFileVector       = std::vector< cppFilePath >;
    using PySourceFileVector        = std::vector< pyFilePath >;
    using SchematicSourceFileVector = std::vector< schFilePath >;
    using FileInfoVector            = std::vector< FileInfo >;

    const manifestFilePath&          getManifestFilePath() const { return m_manifestFilePath; }
    const MegaSourceFileVector&      getMegaSourceFiles() const { return m_megaSourceFiles; }
    const CppSourceFileVector&       getCppSourceFiles() const { return m_cppSourceFiles; }
    const PySourceFileVector&        getPySourceFiles() const { return m_pySourceFiles; }
    const SchematicSourceFileVector& getSchematicSourceFiles() const { return m_schematicSourceFiles; }
    const FileInfoVector&            getCompilationFileInfos() const { return m_compilationFileInfos; }

    ManifestData filterToObjects( const std::set< const mega::io::ObjectInfo* >& objectInfos ) const
    {
        ManifestData manifest{ m_manifestFilePath, {}, {} };
        {
            std::set< ObjectInfo::FileID > fileIDs;
            for( const mega::io::ObjectInfo* pObjectInfo : objectInfos )
            {
                fileIDs.insert( pObjectInfo->getFileID() );
            }
            for( const FileInfo& fileInfo : m_compilationFileInfos )
            {
                if( fileIDs.count( fileInfo.getFileID() ) )
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
        archive& boost::serialization::make_nvp( "pySourceFiles", m_pySourceFiles );
        archive& boost::serialization::make_nvp( "schematicSourceFiles", m_schematicSourceFiles );
        archive& boost::serialization::make_nvp( "compilationFileInfos", m_compilationFileInfos );
    }

    manifestFilePath          m_manifestFilePath;
    MegaSourceFileVector      m_megaSourceFiles;
    CppSourceFileVector       m_cppSourceFiles;
    PySourceFileVector        m_pySourceFiles;
    SchematicSourceFileVector m_schematicSourceFiles;
    FileInfoVector            m_compilationFileInfos;
};

} // namespace mega::io

#endif // MANIFEST_DATA_17_JUNE_2022
