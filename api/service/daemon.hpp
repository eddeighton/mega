#ifndef DAEMON_25_MAY_2022
#define DAEMON_25_MAY_2022

#include "mega/common.hpp"

#include "service/shared_memory_manager.hpp"

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
    friend class DaemonEnrole;

public:
    Daemon( boost::asio::io_context& ioContext, const std::string& strRootIP );
    ~Daemon();

    void shutdown();

    // network::ConversationManager
    virtual network::ConversationBase::Ptr joinConversation( const network::ConnectionID& originatingConnectionID,
                                                             const network::Header&       header,
                                                             const network::Message&      msg );
    virtual void conversationNew( const network::Header& header, const network::ReceivedMsg& msg );
    virtual void conversationEnd( const network::Header& header, const network::ReceivedMsg& msg );

private:
    void onLeafDisconnect( mega::MPE mpe );

    network::Client     m_rootClient;
    network::Server     m_leafServer;
    SharedMemoryManager m_sharedMemoryManager;
    mega::MPE           m_mpe;
};

} // namespace service
} // namespace mega

#endif // DAEMON_25_MAY_2022
