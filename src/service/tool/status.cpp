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

namespace mega
{
namespace service
{

// network::project::Impl
network::Status ToolRequestConversation::GetStatus( const std::vector< network::Status >& childNodeStatus,
                                                    boost::asio::yield_context&           yield_ctx )
{
    SPDLOG_TRACE( "ToolRequestConversation::GetStatus" );

    network::Status status{ childNodeStatus };
    {
        // mega::runtime::getRuntimeStatus();
        status.setConversationID( m_tool.reportConversations() );
        status.setMPO( m_tool.getMPO() );
        status.setDescription( m_tool.m_strProcessName );
    }

    return status;
}

std::string ToolRequestConversation::Ping( boost::asio::yield_context& yield_ctx )
{
    std::ostringstream os;
    os << "Ping from Tool: " << m_tool.m_strProcessName << " " << m_tool.getMPO();
    return os.str();
}

} // namespace service
} // namespace mega
