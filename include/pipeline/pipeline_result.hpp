
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

#ifndef GUARD_2023_January_21_pipeline_result
#define GUARD_2023_January_21_pipeline_result

#include "build_hash_code.hpp"
#include "common/stash.hpp"

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_member.hpp>

#include <string>
#include <map>

namespace mega::pipeline
{

class PipelineResult
{
public:
    using BuildHashCodeMap = std::map< boost::filesystem::path, task::FileHash >;

    PipelineResult() = default;

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
    void save( Archive& archive, const unsigned int ) const
    {
        archive& boost::serialization::make_nvp( "Success", m_bSuccess );
        archive& boost::serialization::make_nvp( "Message", m_strMsg );

        std::vector< BuildHashCode > buildHashCodes;
        for( const auto& [ filePath, fileHash ] : m_buildHashCodes )
        {
            buildHashCodes.push_back( BuildHashCode{ filePath, fileHash } );
        }
        archive& boost::serialization::make_nvp( "BuildHashCodes", buildHashCodes );
    }

    template < typename Archive >
    void load( Archive& archive, const unsigned int )
    {
        archive& boost::serialization::make_nvp( "Success", m_bSuccess );
        archive& boost::serialization::make_nvp( "Message", m_strMsg );

        std::vector< BuildHashCode > buildHashCodes;
        archive&                     boost::serialization::make_nvp( "BuildHashCodes", buildHashCodes );
        for( const BuildHashCode& buildHashCode : buildHashCodes )
        {
            m_buildHashCodes.insert( { buildHashCode.m_filePath, buildHashCode.m_fileHashCode } );
        }
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()

    bool             m_bSuccess;
    std::string      m_strMsg;
    BuildHashCodeMap m_buildHashCodes;
};
} // namespace mega::pipeline

#endif // GUARD_2023_January_21_pipeline_result
