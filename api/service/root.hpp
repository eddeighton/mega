#ifndef ROOT_26_MAY_2022
#define ROOT_26_MAY_2022

#include "execution_context_manager.hpp"
#include "logical_address_space.hpp"

#include "service/network/server.hpp"
#include "service/network/conversation_manager.hpp"

#include "service/protocol/common/megastructure_installation.hpp"
#include "service/protocol/common/root_config.hpp"

#include "common/stash.hpp"

#include "boost/asio/io_context.hpp"

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
    virtual void conversationNew( const network::Header& header, const network::ReceivedMsg& msg );
    virtual void conversationEnd( const network::Header& header, const network::ReceivedMsg& msg );

    network::MegastructureInstallation getMegastructureInstallation();
    const network::Project&            getProject() const { return m_config.getProject(); }
    void                               setProject( const network::Project& project ) { m_config.setProject( project ); }

private:
    void loadConfig();
    void saveConfig();

    void onDaemonDisconnect( mega::MPE mpe );

private:
    network::Server                                     m_server;
    task::BuildHashCodes                                m_buildHashCodes;
    task::Stash                                         m_stash;
    mega::network::RootConfig                           m_config;
    std::optional< network::MegastructureInstallation > m_megastructureInstallationOpt;
    ExecutionContextManager                             m_executionContextManager;
    LogicalAddressSpace                                 m_logicalAddressSpace;
};

} // namespace service
} // namespace mega

#endif // ROOT_26_MAY_2022
