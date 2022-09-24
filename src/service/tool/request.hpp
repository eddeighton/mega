#ifndef REQUEST_TOOL_23_SEPT_2022
#define REQUEST_TOOL_23_SEPT_2022

#include "service/tool.hpp"

#include "service/protocol/model/leaf_tool.hxx"
#include "service/protocol/model/tool_leaf.hxx"

namespace mega
{
namespace service
{

class ToolRequestConversation : public network::InThreadConversation, public network::leaf_tool::Impl
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
};

} // namespace service
} // namespace mega

#endif // REQUEST_TOOL_23_SEPT_2022
