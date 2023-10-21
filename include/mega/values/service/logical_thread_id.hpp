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

#ifndef LOGICALTHREAD_ID_25_MAY_2022
#define LOGICALTHREAD_ID_25_MAY_2022

#include "common/hash.hpp"

#include "mega/values/native_types.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/serialization/array_wrapper.hpp>

#include <string>
#include <ostream>
#include <istream>

namespace mega::network
{

using MessageSize  = mega::U32;

/// LogicalThreadID
/// This is a value type representing an OPAQUE value to identify a converation network-wide.
class LogicalThreadID
{
    LogicalThreadID( boost::uuids::uuid u )
        : m_uuid( std::move( u ) )
    {
    }

public:
    LogicalThreadID()                                   = default;
    LogicalThreadID( LogicalThreadID& )                  = default;
    LogicalThreadID( const LogicalThreadID& )            = default;
    LogicalThreadID( LogicalThreadID&& )                 = default;
    LogicalThreadID& operator=( LogicalThreadID& )       = default;
    LogicalThreadID& operator=( const LogicalThreadID& ) = default;

    static inline LogicalThreadID fromUUID( const std::string& strUUID )
    {
        boost::uuids::uuid u;
        {
            std::istringstream is( strUUID );
            is >> u;
        }
        return LogicalThreadID{ u };
    }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive & boost::serialization::make_array( m_uuid.data, m_uuid.size() );
    }

    struct Hash
    {
        inline mega::U64 operator()( const LogicalThreadID& id ) const noexcept
        {
            common::Hash hash;
            for( const auto& v : id.m_uuid )
            {
                hash ^= v;
            }
            return hash.get();
        }
    };

    inline std::string toStr() const { return boost::uuids::to_string( m_uuid ); }

    inline bool operator==( const LogicalThreadID& right ) const { return m_uuid == right.m_uuid; }
    inline bool operator!=( const LogicalThreadID& right ) const { return m_uuid != right.m_uuid; }
    inline bool operator<( const LogicalThreadID& right ) const { return m_uuid < right.m_uuid; }

private:
    boost::uuids::uuid m_uuid = boost::uuids::random_generator()();
};

inline std::ostream& operator<<( std::ostream& os, const LogicalThreadID& logicalthreadID )
{
    return os << logicalthreadID.toStr();
}

inline std::istream& operator>>( std::istream& is, LogicalThreadID& logicalthreadID )
{
    std::string str;
    is >> str;
    logicalthreadID = LogicalThreadID::fromUUID( str );
    return is;
}

using MessageID = mega::U32;

} // namespace mega::network

#endif // LOGICALTHREAD_ID_25_MAY_2022
