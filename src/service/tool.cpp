
#include "service/tool.hpp"

#include "service/network/conversation_manager.hpp"
#include "service/network/network.hpp"
#include "service/network/end_point.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/header.hpp"

#include "common/requireSemicolon.hpp"

#include "boost/system/detail/error_code.hpp"
#include "boost/bind/bind.hpp"

#include <thread>

namespace mega
{
namespace service
{

Tool::Tool()
    : network::ConversationManager( network::Node::toStr( network::Node::Tool ), m_io_context )
    , m_receiverChannel( m_io_context, *this )
    , m_work_guard( m_io_context.get_executor() )
    , m_leaf( m_receiverChannel.getSender(), network::Node::Tool )
{
    std::ostringstream os;
    os << network::Node::toStr( network::Node::Tool ) << "_" << std::this_thread::get_id();
    network::ConnectionID connectionID = os.str();
    m_receiverChannel.run( connectionID );
}

Tool::~Tool()
{
    m_receiverChannel.stop();
    m_work_guard.reset();
}

network::ConversationBase::Ptr Tool::joinConversation( const network::ConnectionID&   originatingConnectionID,
                                                       const network::Header&         header,
                                                       const network::MessageVariant& msg )
{
    // return network::ConversationBase::Ptr(
    //     new ToolRequestConversation( m_terminal, msgHeader.getConversationID(), originatingConnectionID ) );
    return network::ConversationBase::Ptr();
}

} // namespace service
} // namespace mega
