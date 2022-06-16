#ifndef ROOT_26_MAY_2022
#define ROOT_26_MAY_2022

#include "service/network/server.hpp"
#include "service/network/conversation_manager.hpp"

#include "boost/asio/io_context.hpp"

#include "common/stash.hpp"

namespace mega
{
namespace service
{

class Root : public network::ConversationManager
{
    friend class RootRequestConversation;

public:
    Root( boost::asio::io_context& ioContext );
    void shutdown();

    // network::ConversationManager
    virtual network::ConversationBase::Ptr joinConversation( const network::ConnectionID&   originatingConnectionID,
                                                             const network::Header&         header,
                                                             const network::MessageVariant& msg );

private:
    network::Server m_server;
    task::Stash     m_stash;
};

} // namespace service
} // namespace mega

#endif // ROOT_26_MAY_2022
