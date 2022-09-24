#ifndef STATUS_23_SEPT_2022
#define STATUS_23_SEPT_2022

#include "mega/reference.hpp"
#include "service/protocol/common/header.hpp"

#include <vector>
#include <ostream>
#include <optional>

namespace mega
{
namespace network
{

class Status
{
public:
    using StatusVector = std::vector< Status >;

    Status() {}
    Status( const StatusVector& children )
        : m_childStatus( children )
    {
    }

    const std::optional< mega::MPO >&             getMPO() const { return m_mpo; }
    const std::vector< network::ConversationID >& getConversations() const { return m_conversationIDs; }
    const std::string&                            getDescription() const { return m_description; }
    const StatusVector&                           getChildren() const { return m_childStatus; }

    void setMPO( mega::MPO mpo ) { m_mpo = mpo; }
    void setConversationID( const std::vector< network::ConversationID >& conversations )
    {
        m_conversationIDs = conversations;
    }
    void setDescription( const std::string& strDescription ) { m_description = strDescription; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_mpo;
        archive& m_conversationIDs;
        archive& m_description;
        archive& m_childStatus;
    }

private:
    std::optional< mega::MPO >             m_mpo;
    std::vector< network::ConversationID > m_conversationIDs;
    std::string                            m_description;
    StatusVector                           m_childStatus;
};

std::ostream& operator<<( std::ostream& os, const Status& conversationID );

} // namespace network
} // namespace mega

#endif // STATUS_23_SEPT_2022
