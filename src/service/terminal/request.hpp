#ifndef TERMINAL_REQUEST_23_SEPT_2022
#define TERMINAL_REQUEST_23_SEPT_2022

#include "service/terminal.hpp"

#include "service/protocol/model/leaf_term.hxx"

namespace mega
{
namespace service
{

class TerminalRequestConversation : public network::InThreadConversation, public network::leaf_term::Impl
{
protected:
    Terminal& m_terminal;

public:
    TerminalRequestConversation( Terminal& terminal, const network::ConversationID& conversationID,
                                 const network::ConnectionID& originatingConnectionID );

    virtual network::Message dispatchRequest( const network::Message&     msg,
                                              boost::asio::yield_context& yield_ctx ) override;
    virtual void             dispatchResponse( const network::ConnectionID& connectionID, const network::Message& msg,
                                               boost::asio::yield_context& yield_ctx ) override;

    virtual void error( const network::ConnectionID& connection, const std::string& strErrorMsg,
                        boost::asio::yield_context& yield_ctx ) override;
};

} // namespace service
} // namespace mega

#endif //TERMINAL_REQUEST_23_SEPT_2022
