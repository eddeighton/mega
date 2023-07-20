
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
#include "mpo_conversation.hpp"

#include "service/network/log.hpp"

namespace mega::service::python
{

// network::project::Impl

network::Status MPOConversation::GetStatus( const std::vector< network::Status >& childNodeStatus,
                                            boost::asio::yield_context&           yield_ctx )
{
    SPDLOG_TRACE( "PythonRequestConversation::GetStatus" );

    network::Status status{ childNodeStatus };
    {
        std::vector< network::ConversationID > conversations;
        {
            for( const auto& id : m_python.reportConversations() )
            {
                if( id != getID() )
                {
                    conversations.push_back( id );
                }
            }
        }
        status.setConversationID( conversations );
        status.setMPO( m_python.getMPO() );
        status.setDescription( m_python.getProcessName() );

        using MPOTimeStampVec = std::vector< std::pair< mega::MPO, TimeStamp > >;
        using MPOVec          = std::vector< mega::MPO >;
        if( const auto& reads = m_lockTracker.getReads(); !reads.empty() )
            status.setReads( MPOTimeStampVec{ reads.begin(), reads.end() } );
        if( const auto& writes = m_lockTracker.getWrites(); !writes.empty() )
            status.setWrites( MPOTimeStampVec{ writes.begin(), writes.end() } );

        {
            std::ostringstream os;
            os << "Python: " << getLog().getTimeStamp();
        }

        status.setLogIterator( getLog().getIterator() );
        status.setLogFolder( getLog().getFolder().string() );

        status.setMemory( m_pMemoryManager->getStatus() );
    }

    return status;
}

network::Status PythonRequestConversation::GetStatus( const std::vector< network::Status >& childNodeStatus,
                                                      boost::asio::yield_context&           yield_ctx )
{
    SPDLOG_TRACE( "PythonRequestConversation::GetStatus" );

    network::Status status{ childNodeStatus };
    {
        std::vector< network::ConversationID > conversations;
        {
            for( const auto& id : m_python.reportConversations() )
            {
                if( id != getID() )
                {
                    conversations.push_back( id );
                }
            }
        }
        status.setConversationID( conversations );
        status.setMPO( m_python.getMPO() );
        status.setDescription( m_python.getProcessName() );
    }

    return status;
}

std::string PythonRequestConversation::Ping( const std::string& strMsg, boost::asio::yield_context& yield_ctx )
{
    using ::           operator<<;
    std::ostringstream os;
    os << "Ping reached: " << common::ProcessID::get() << " got: " << strMsg.size() << " bytes";
    return os.str();
}

} // namespace mega::service::python
