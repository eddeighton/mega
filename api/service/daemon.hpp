#ifndef DAEMON_25_MAY_2022
#define DAEMON_25_MAY_2022

#include "service/network/client.hpp"
#include "service/network/server.hpp"
#include "service/network/conversation_manager.hpp"

#include "boost/asio/io_context.hpp"

namespace mega
{
namespace service
{
class Daemon : public network::ConversationManager
{
    friend class DaemonRequestConversation;
public:
    Daemon( boost::asio::io_context& ioContext, const std::string& strRootIP );
    ~Daemon();

    void shutdown();

    // network::ConversationManager
    virtual network::ConversationBase::Ptr joinConversation( const network::ConnectionID&   originatingConnectionID,
                                                             const network::Header&         header,
                                                             const network::MessageVariant& msg );

private:
    network::Client m_rootClient;
    network::Server m_leafServer;
};
} // namespace service
} // namespace mega

#endif // DAEMON_25_MAY_2022
