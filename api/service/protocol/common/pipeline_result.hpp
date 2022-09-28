



#ifndef PIPELINE_RESULT_2_JUNE_2022
#define PIPELINE_RESULT_2_JUNE_2022

#include "common/stash.hpp"

#include "boost/serialization/nvp.hpp"
#include "boost/serialization/split_member.hpp"

#include <string>
#include <map>

namespace mega
{
namespace network
{

class BuildHashCode
{
public:
    BuildHashCode() {}

    BuildHashCode( const boost::filesystem::path& filePath, const task::FileHash& fileHashCode )
        : m_filePath( filePath )
        , m_fileHashCode( fileHashCode )
    {
    }

    template < typename Archive >
    void save( Archive& archive, const unsigned int v ) const
    {
        archive&          boost::serialization::make_nvp( "FilePath", m_filePath );
        const std::string strHex = m_fileHashCode.toHexString();
        archive&          boost::serialization::make_nvp( "FileHash", strHex );
    }

    template < typename Archive >
    void load( Archive& archive, const unsigned int v )
    {
        archive&    boost::serialization::make_nvp( "FilePath", m_filePath );
        std::string strHex;
        archive&    boost::serialization::make_nvp( "FileHash", strHex );
        m_fileHashCode.set( common::Hash::fromHexString( strHex ) );
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()

    boost::filesystem::path m_filePath;
    task::FileHash          m_fileHashCode;
};

class PipelineResult
{
public:
    using BuildHashCodeMap = std::map< boost::filesystem::path, task::FileHash >;

    PipelineResult() {}

    PipelineResult( bool bSuccess, const std::string& strMsg, const BuildHashCodeMap& buildHashCodes )
        : m_bSuccess( bSuccess )
        , m_strMsg( strMsg )
        , m_buildHashCodes( buildHashCodes )
    {
    }

    bool                    getSuccess() const { return m_bSuccess; }
    std::string             getMessage() const { return m_strMsg; }
    const BuildHashCodeMap& getBuildHashCodes() const { return m_buildHashCodes; }

    template < typename Archive >
    void save( Archive& archive, const unsigned int v ) const
    {
        archive& boost::serialization::make_nvp( "Success", m_bSuccess );
        archive& boost::serialization::make_nvp( "Message", m_strMsg );

        std::vector< BuildHashCode > buildHashCodes;
        for ( const auto& [ filePath, fileHash ] : m_buildHashCodes )
        {
            buildHashCodes.push_back( BuildHashCode{ filePath, fileHash } );
        }
        archive& boost::serialization::make_nvp( "BuildHashCodes", buildHashCodes );
    }

    template < typename Archive >
    void load( Archive& archive, const unsigned int v )
    {
        archive& boost::serialization::make_nvp( "Success", m_bSuccess );
        archive& boost::serialization::make_nvp( "Message", m_strMsg );

        std::vector< BuildHashCode > buildHashCodes;
        archive&                     boost::serialization::make_nvp( "BuildHashCodes", buildHashCodes );
        for ( const BuildHashCode& buildHashCode : buildHashCodes )
        {
            m_buildHashCodes.insert( { buildHashCode.m_filePath, buildHashCode.m_fileHashCode } );
        }
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()

    bool             m_bSuccess;
    std::string      m_strMsg;
    BuildHashCodeMap m_buildHashCodes;
};

} // namespace network
} // namespace mega

#endif // PIPELINE_RESULT_2_JUNE_2022