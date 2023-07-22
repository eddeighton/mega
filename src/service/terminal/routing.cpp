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

namespace mega::service
{

TerminalRequestLogicalThread::TerminalRequestLogicalThread( Terminal&                      terminal,
                                                          const network::LogicalThreadID& logicalthreadID,
                                                          const network::ConnectionID&   originatingConnectionID )
    : InThreadLogicalThread( terminal, logicalthreadID, originatingConnectionID )
    , m_terminal( terminal )
{
}

network::Message TerminalRequestLogicalThread::dispatchRequest( const network::Message&     msg,
                                                               boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if ( result = network::leaf_term::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::status::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    if ( result = network::project::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    THROW_RTE( "TerminalRequestLogicalThread::dispatchRequest failed" );
}
void TerminalRequestLogicalThread::dispatchResponse( const network::ConnectionID& connectionID,
                                                    const network::Message& msg, boost::asio::yield_context& yield_ctx )
{
    if ( m_terminal.getLeafSender().getConnectionID() == connectionID )
    {
        m_terminal.getLeafSender().send( msg, yield_ctx );
    }
    else if ( m_terminal.m_receiverChannel.getSender()->getConnectionID() == connectionID )
    {
        m_terminal.getLeafSender().send( msg, yield_ctx );
    }
    else
    {
        // This can happen when initiating request has received exception - in which case
        SPDLOG_ERROR( "Terminal cannot resolve connection: {} on error: {}", connectionID, msg );
    }
}

void TerminalRequestLogicalThread::error( const network::ReceivedMsg& msg, const std::string& strErrorMsg,
                                         boost::asio::yield_context& yield_ctx )
{
    if ( m_terminal.getLeafSender().getConnectionID() == msg.connectionID )
    {
        m_terminal.getLeafSender().sendErrorResponse( msg, strErrorMsg, yield_ctx );
    }
    else if ( m_terminal.m_receiverChannel.getSender()->getConnectionID() == msg.connectionID )
    {
        m_terminal.m_receiverChannel.getSender()->sendErrorResponse( msg, strErrorMsg, yield_ctx );
    }
    else
    {
        // This can happen when initiating request has received exception - in which case
        SPDLOG_ERROR( "Terminal cannot resolve connection: {} on error: {}", msg.connectionID, strErrorMsg );
    }
}

network::Message TerminalRequestLogicalThread::RootAllBroadcast( const network::Message&     request,
                                                                boost::asio::yield_context& yield_ctx )
{
    return dispatchRequest( request, yield_ctx );
}

} // namespace mega::service
