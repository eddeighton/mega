
#ifndef HEADER_25_MAY_2022
#define HEADER_25_MAY_2022

#include "common/hash.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <ostream>
#include <istream>

namespace mega
{
namespace network
{

using MessageSize  = std::uint32_t;
using ConnectionID = std::string;

/// ConversationID
/// This is a value type representing an OPAQUE value to identify a converation network-wide.
/// The internal values MUST NOT be interpreted to mean anything i.e. the connectionID will NOT
/// reliably mean anything to a given process.
class ConversationID
{
    friend bool operator==( const ConversationID& left, const ConversationID& right );
    friend bool operator<( const ConversationID& left, const ConversationID& right );
    friend std::ostream& operator<<( std::ostream& os, const ConversationID& conversationID );
public:
    using ID = std::uint16_t;

    ConversationID()
        : m_id( 0U )
    {
    }

    ConversationID( ID id, const ConnectionID& connectionID )
        : m_id( id )
        , m_connectionID( connectionID )
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
        inline std::size_t operator()( const ConversationID& id ) const noexcept 
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

inline bool operator<( const ConversationID& left, const ConversationID& right )
{
    return ( left.getConnectionID() != right.getConnectionID() ) ? ( left.getConnectionID() < right.getConnectionID() )
                                                                 : left.getID() < right.getID();
}

std::ostream& operator<<( std::ostream& os, const ConversationID& conversationID );
std::istream& operator>>( std::istream& is, ConversationID& conversationID );

using MessageID = std::uint32_t;

class Header
{
public:
    Header()
        : m_messageID( 0U )
    {
    }

    Header( MessageID messageID, const ConversationID& conversationID )
        : m_messageID( messageID )
        , m_conversationID( conversationID )
    {
    }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_messageID;
        archive& m_conversationID;
    }

    MessageID             getMessageID() const { return m_messageID; }
    const ConversationID& getConversationID() const { return m_conversationID; }

private:
    MessageID      m_messageID;
    ConversationID m_conversationID;
};

} // namespace network
} // namespace mega

#endif // HEADER_25_MAY_2022
