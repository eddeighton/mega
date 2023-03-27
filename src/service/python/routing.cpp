
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

#include "request.hpp"

#include "service/network/log.hpp"

namespace mega::service::python
{

PythonRequestConversation::PythonRequestConversation( Python& python, const network::ConversationID& conversationID,
                                                      std::optional< network::ConnectionID > originatingConnectionID )
    : InThreadConversation( python, conversationID, originatingConnectionID )
    , m_python( python )
{
}

network::Message PythonRequestConversation::dispatchRequest( const network::Message&     msg,
                                                             boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if( result = network::leaf_python::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::python_leaf::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::mpo::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::project::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if( result = network::status::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    THROW_RTE( "PythonRequestConversation::dispatchRequest failed: " << msg );
}

void PythonRequestConversation::dispatchResponse( const network::ConnectionID& connectionID,
                                                  const network::Message& msg, boost::asio::yield_context& yield_ctx )
{
    if( m_python.getLeafSender().getConnectionID() == connectionID )
    {
        m_python.getLeafSender().send( msg, yield_ctx );
    }
    else if( m_python.m_receiverChannel.getSender()->getConnectionID() == connectionID )
    {
        m_python.getLeafSender().send( msg, yield_ctx );
    }
    else if( m_python.getExternalConversation()->getID() == msg.getSenderID() )
    {
        m_python.getExternalConversation()->send( network::ReceivedMsg{ connectionID, msg } );
    }
    else
    {
        SPDLOG_ERROR( "PythonRequestConversation::dispatchResponse cannot resolve connection: {} on error: {}",
                      connectionID, msg );
    }
}

void PythonRequestConversation::error( const network::ReceivedMsg& msg, const std::string& strErrorMsg,
                                       boost::asio::yield_context& yield_ctx )
{
    if( m_python.getLeafSender().getConnectionID() == msg.connectionID )
    {
        m_python.getLeafSender().sendErrorResponse( msg, strErrorMsg, yield_ctx );
    }
    else if( m_python.m_receiverChannel.getSender()->getConnectionID() == msg.connectionID )
    {
        m_python.m_receiverChannel.getSender()->sendErrorResponse( msg, strErrorMsg, yield_ctx );
    }
    else if( m_python.getExternalConversation()->getID() == msg.msg.getSenderID() )
    {
        m_python.getExternalConversation()->sendErrorResponse( msg, strErrorMsg );
    }
    else if( m_python.getExternalConversation()->getID() == msg.msg.getReceiverID() )
    {
        m_python.getExternalConversation()->sendErrorResponse( msg, strErrorMsg );
    }
    else
    {
        // This can happen when initiating request has received exception - in which case
        SPDLOG_ERROR( "PythonRequestConversation::error cannot resolve connection: {} on error: {}", msg.connectionID,
                      strErrorMsg );
    }
}

network::python_leaf::Request_Sender
PythonRequestConversation::getPythonRequest( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_python.getLeafSender(), yield_ctx };
}

network::Message PythonRequestConversation::RootAllBroadcast( const network::Message&     request,
                                                              boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );
}

network::Message PythonRequestConversation::PythonRoot( const network::Message&     request,
                                                        boost::asio::yield_context& yield_ctx )
{
    return getPythonRequest( yield_ctx ).PythonRoot( request );
}
network::Message PythonRequestConversation::PythonDaemon( const network::Message&     request,
                                                          boost::asio::yield_context& yield_ctx )
{
    return getPythonRequest( yield_ctx ).PythonDaemon( request );
}

network::Message PythonRequestConversation::MPRoot( const network::Message& request, const mega::MP& mp,
                                                    boost::asio::yield_context& yield_ctx )
{
    network::mpo::Request_Sender rq{ *this, m_python.getLeafSender(), yield_ctx };
    return rq.MPRoot( request, mp );
}

network::Message PythonRequestConversation::MPUp( const network::Message& request, const mega::MP& mp,
                                                    boost::asio::yield_context& yield_ctx )
{
    network::mpo::Request_Sender rq{ *this, m_python.getLeafSender(), yield_ctx };
    return rq.MPUp( request, mp );
}

network::Message PythonRequestConversation::MPDown( const network::Message& request, const mega::MP& mp,
                                                    boost::asio::yield_context& yield_ctx )
{
    VERIFY_RTE( mega::MP( m_python.getMPO() ) == mp );
    return dispatchRequest( request, yield_ctx );
}

network::Message PythonRequestConversation::MPODown( const network::Message& request, const mega::MPO& mpo,
                                                     boost::asio::yield_context& yield_ctx )
{
    VERIFY_RTE( mega::MPO( m_python.getMPO() ) == mpo );
    return dispatchRequest( request, yield_ctx );
}

} // namespace mega::service::python
