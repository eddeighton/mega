#ifndef ROOT_26_MAY_2022
#define ROOT_26_MAY_2022

#include "service/network/server.hpp"
#include "service/network/conversation_manager.hpp"

#include "boost/asio/io_context.hpp"

#include "common/stash.hpp"
#include "service/protocol/common/root_config.hpp"

namespace mega
{
namespace service
{

class Root : public network::ConversationManager
{
    friend class RootPipelineConversation;
    friend class RootRequestConversation;

public:
    Root( boost::asio::io_context& ioContext );
    void shutdown();

    // network::ConversationManager
    virtual network::ConversationBase::Ptr joinConversation( const network::ConnectionID& originatingConnectionID,
                                                             const network::Header&       header,
                                                             const network::Message&      msg );

    const network::Project& getProject() const { return m_config.getProject(); }
    void                    setProject( const network::Project& project ) { m_config.setProject( project ); }

private:
    void loadConfig();
    void saveConfig();

private:
    network::Server           m_server;
    task::Stash               m_stash;
    mega::network::RootConfig m_config;
};

} // namespace service
} // namespace mega

#endif // ROOT_26_MAY_2022
