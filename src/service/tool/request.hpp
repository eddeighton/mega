//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#ifndef REQUEST_TOOL_23_SEPT_2022
#define REQUEST_TOOL_23_SEPT_2022

#include "service/tool.hpp"

#include "service/protocol/model/leaf_tool.hxx"
#include "service/protocol/model/tool_leaf.hxx"
#include "service/protocol/model/mpo.hxx"
#include "service/protocol/model/status.hxx"
#include "service/protocol/model/project.hxx"

namespace mega::service
{

class ToolRequestConversation : public network::InThreadConversation,
                                public network::leaf_tool::Impl,
                                public network::mpo::Impl,
                                public network::status::Impl,
                                public network::project::Impl
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

    virtual void error( const network::ReceivedMsg& msg, const std::string& strErrorMsg,
                        boost::asio::yield_context& yield_ctx ) override;

    network::tool_leaf::Request_Sender getToolRequest( boost::asio::yield_context& yield_ctx );

    template < typename RequestEncoderType >
    RequestEncoderType getRootRequest( boost::asio::yield_context& yield_ctx )
    {
        return RequestEncoderType( [ rootRequest = getToolRequest( yield_ctx ) ]( const network::Message& msg ) mutable
                                   { return rootRequest.ToolRoot( msg ); },
                                   getID() );
    }

    template < typename RequestEncoderType >
    RequestEncoderType getDaemonRequest( boost::asio::yield_context& yield_ctx )
    {
        return RequestEncoderType( [ rootRequest = getToolRequest( yield_ctx ) ]( const network::Message& msg ) mutable
                                   { return rootRequest.ToolDaemon( msg ); },
                                   getID() );
    }

    // network::leaf_tool::Impl
    virtual network::Message RootAllBroadcast( const network::Message&     request,
                                               boost::asio::yield_context& yield_ctx ) override;

    // network::mpo::Impl
    virtual network::Message MPDown( const network::Message& request, const mega::MP& mp,
                                     boost::asio::yield_context& yield_ctx ) override;
    virtual network::Message MPODown( const network::Message& request, const mega::MPO& mpo,
                                      boost::asio::yield_context& yield_ctx ) override;

    // network::status::Impl
    virtual network::Status GetStatus( const std::vector< network::Status >& status,
                                       boost::asio::yield_context&           yield_ctx ) override;
    virtual std::string     Ping( const std::string& strMsg, boost::asio::yield_context& yield_ctx ) override;

    // network::project::Impl
    virtual void SetProject( const mega::network::Project& project, boost::asio::yield_context& yield_ctx ) override;
};

} // namespace mega::service

#endif // REQUEST_TOOL_23_SEPT_2022
