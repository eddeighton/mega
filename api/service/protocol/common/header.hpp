
#ifndef HEADER_25_MAY_2022
#define HEADER_25_MAY_2022

#include <cstddef>
#include <cstdint>
#include <string>
#include <ostream>

namespace mega
{
namespace network
{

using MessageSize  = std::uint32_t;
using ConnectionID = std::string;

class ActivityID
{
public:
    using ID = std::uint16_t;

    ActivityID()
        : m_id( 0U )
    {
    }

    ActivityID( ID id, const ConnectionID& connectionID )
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

    ID           getID() const { return m_id; }
    ConnectionID getConnectionID() const { return m_connectionID; }

private:
    ID           m_id;
    ConnectionID m_connectionID;
};

inline bool operator==( const ActivityID& left, const ActivityID& right )
{
    return ( left.getConnectionID() == right.getConnectionID() ) && ( left.getID() == right.getID() );
}

inline bool operator<( const ActivityID& left, const ActivityID& right )
{
    return ( left.getConnectionID() != right.getConnectionID() ) ? ( left.getConnectionID() < right.getConnectionID() )
                                                                 : left.getID() < right.getID();
}

using MessageID = std::uint32_t;

class Header
{
public:
    Header()
        : m_messageID( 0U )
    {
    }

    Header( MessageID messageID, const ActivityID& activityID )
        : m_messageID( messageID )
        , m_activityID( activityID )
    {
    }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_messageID;
        archive& m_activityID;
    }

    MessageID         getMessageID() const { return m_messageID; }
    const ActivityID& getActivityID() const { return m_activityID; }

private:
    MessageID  m_messageID;
    ActivityID m_activityID;
};

} // namespace network
} // namespace mega

#endif // HEADER_25_MAY_2022
