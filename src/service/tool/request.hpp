#ifndef REQUEST_TOOL_23_SEPT_2022
#define REQUEST_TOOL_23_SEPT_2022

#include "service/tool.hpp"

#include "service/protocol/model/leaf_tool.hxx"
#include "service/protocol/model/tool_leaf.hxx"
#include "service/protocol/model/mpo.hxx"
#include "service/protocol/model/status.hxx"

namespace mega
{
namespace service
{

class ToolRequestConversation : public network::InThreadConversation,
                                public network::leaf_tool::Impl,
                                public network::mpo::Impl,
                                public network::status::Impl
{
protected:
    Tool& m_tool;

public:
    ToolRequestConversation( Tool& tool, const network::ConversationID& conversationID,
                             const network::ConnectionID& originatingConnectionID );

    virtual network::Message dispatchRequest( const network::Message&     msg,
                                              boost::asio::yield_context& yield_ctx ) override;
    virtual void             dispatchResponse( const network::ConnectionID& connectionID, const network::Message& msg,
                                               boost::asio::yield_context& yield_ctx ) override;

    virtual void error( const network::ConnectionID& connection, const std::string& strErrorMsg,
                        boost::asio::yield_context& yield_ctx ) override;

    network::tool_leaf::Request_Sender getToolRequest( boost::asio::yield_context& yield_ctx );

    // network::mpo::Impl
    virtual network::Message MPODown( const network::Message& request, const mega::MPO& mpo,
                                      boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPDown( const network::Message& request, const mega::MP& mp,
                                     boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPUp( const network::Message& request, const mega::MP& mp,
                                   boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPOUp( const network::Message& request, const mega::MPO& mpo,
                                    boost::asio::yield_context& yield_ctx ) override;

    // network::status::Impl
    virtual network::Status GetStatus( const std::vector< network::Status >& status,
                                       boost::asio::yield_context&           yield_ctx ) override;
    virtual std::string     Ping( boost::asio::yield_context& yield_ctx ) override;
};

} // namespace service
} // namespace mega

#endif // REQUEST_TOOL_23_SEPT_2022
