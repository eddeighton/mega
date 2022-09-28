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

namespace mega
{
namespace service
{

// network::project::Impl
network::Status DaemonRequestConversation::GetStatus( const std::vector< network::Status >& childNodeStatus,
                                                      boost::asio::yield_context&           yield_ctx )
{
    SPDLOG_TRACE( "DaemonRequestConversation::GetStatus" );

    network::Status status{ childNodeStatus };
    {
        std::vector< network::ConversationID > conversations;
        for ( const auto& [ id, pCon ] : m_daemon.m_conversations )
        {
            conversations.push_back( id );
        }
        status.setConversationID( conversations );
        status.setMP( m_daemon.m_mp );
        status.setDescription( m_daemon.m_strProcessName );
    }

    return status;
}

std::string DaemonRequestConversation::Ping( boost::asio::yield_context& yield_ctx )
{
    std::ostringstream os;
    os << "Ping from Daemon: " << m_daemon.getProcessName();
    return os.str();
}

} // namespace service
} // namespace mega
