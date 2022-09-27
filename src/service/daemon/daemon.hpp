#ifndef DAEMON_25_MAY_2022
#define DAEMON_25_MAY_2022

#include "shared_memory_manager.hpp"

#include "service/network/client.hpp"
#include "service/network/server.hpp"
#include "service/network/conversation_manager.hpp"

#include "mega/common.hpp"

#include "boost/asio/io_context.hpp"

namespace mega
{
namespace service
{
class Daemon : public network::ConversationManager
{
    friend class DaemonRequestConversation;
    friend class DaemonEnrole;

public:
    Daemon( boost::asio::io_context& ioContext, const std::string& strRootIP );
    ~Daemon();

    void shutdown();

    // network::ConversationManager
    virtual network::ConversationBase::Ptr joinConversation( const network::ConnectionID& originatingConnectionID,
                                                             const network::Header&       header,
                                                             const network::Message&      msg );

private:
    void onLeafDisconnect( const network::ConnectionID& connectionID, mega::MP mp );

    network::Client     m_rootClient;
    network::Server     m_server;
    SharedMemoryManager m_sharedMemoryManager;
    mega::MP            m_mp;
};

} // namespace service
} // namespace mega

#endif // DAEMON_25_MAY_2022
