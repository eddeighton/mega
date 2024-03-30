
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

#ifndef GUARD_2024_March_30_request_player
#define GUARD_2024_March_30_request_player

#include "request_host.hpp"

namespace mega::service
{

class PlayerRequestLogicalThread : public HostRequestLogicalThread
{
public:
    PlayerRequestLogicalThread( Leaf& leaf, const network::LogicalThreadID& logicalthreadID )
        :   HostRequestLogicalThread( leaf, logicalthreadID )
    {

    }

    void SaveSnapshot( boost::asio::yield_context& )
    {
        SPDLOG_TRACE( "PlayerRequestLogicalThread::SaveSnapshot" );
        THROW_TODO;
    }

    void LoadSnapshot( boost::asio::yield_context& )
    {
        SPDLOG_TRACE( "PlayerRequestLogicalThread::LoadSnapshot" );
        THROW_TODO;
    }

    void LoadProgram( const mega::service::Program& program, boost::asio::yield_context& )
    {
        SPDLOG_TRACE( "PlayerRequestLogicalThread::LoadProgram {}", program );
        m_leaf.getRuntime().loadProgram( program );
    }

    void UnloadProgram( boost::asio::yield_context& )
    {
        SPDLOG_TRACE( "PlayerRequestLogicalThread::UnloadProgram" );
        m_leaf.getRuntime().unloadProgram();
    }

    mega::service::Program GetProgram( boost::asio::yield_context& )
    {
        SPDLOG_TRACE( "PlayerRequestLogicalThread::GetProgram" );
        return m_leaf.getRuntime().getProgram();
    }

};

}

#endif //GUARD_2024_March_30_request_player
