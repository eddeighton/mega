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

#ifndef CONVERSATION_ID_25_MAY_2022
#define CONVERSATION_ID_25_MAY_2022

#include "common/hash.hpp"

#include "mega/native_types.hpp"

#include <string>
#include <ostream>
#include <istream>
#include <utility>

namespace mega::network
{

using MessageSize  = mega::U32;
using ConnectionID = std::string;

/// ConversationID
/// This is a value type representing an OPAQUE value to identify a converation network-wide.
/// The internal values MUST NOT be interpreted to mean anything i.e. the connectionID will NOT
/// reliably mean anything to a given process.
class ConversationID
{
    friend bool          operator==( const ConversationID& left, const ConversationID& right );
    friend bool          operator<( const ConversationID& left, const ConversationID& right );
    friend std::ostream& operator<<( std::ostream& os, const ConversationID& conversationID );

public:
    using ID = mega::U16;

    ConversationID()
        : m_id( 0U )
    {
    }

    ConversationID( ID id, ConnectionID connectionID )
        : m_id( id )
        , m_connectionID(std::move( connectionID ))
    {
    }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_id;
        archive& m_connectionID;
    }

    struct Hash
    {
        inline mega::U64 operator()( const ConversationID& id ) const noexcept
        {
            const common::Hash hash{ id.m_id, id.m_connectionID };
            return hash.get();
        }
    };

private:
    ID           getID() const { return m_id; }
    ConnectionID getConnectionID() const { return m_connectionID; }

    ID           m_id;
    ConnectionID m_connectionID;
};

inline bool operator==( const ConversationID& left, const ConversationID& right )
{
    return ( left.getConnectionID() == right.getConnectionID() ) && ( left.getID() == right.getID() );
}

inline bool operator!=( const ConversationID& left, const ConversationID& right ) { return !( left == right ); }

inline bool operator<( const ConversationID& left, const ConversationID& right )
{
    return ( left.getConnectionID() != right.getConnectionID() ) ? ( left.getConnectionID() < right.getConnectionID() )
                                                                 : left.getID() < right.getID();
}

inline std::ostream& operator<<( std::ostream& os, const ConversationID& conversationID )
{
    return os << conversationID.getID() << "_" << conversationID.getConnectionID();
}

inline std::istream& operator>>( std::istream& is, ConversationID& conversationID )
{
    std::string str;
    is >> str;

    auto iter = std::find( str.begin(), str.end(), '_' );

    VERIFY_RTE( iter != str.end() );

    const std::string strCon( iter + 1, str.end() );

    ConversationID::ID id;
    {
        std::string        strInt( str.begin(), iter );
        std::istringstream isInt( strInt );
        isInt >> id;
    }

    conversationID = ConversationID{ id, strCon };

    return is;
}

using MessageID = mega::U32;

} // namespace mega::network

#endif // CONVERSATION_ID_25_MAY_2022
