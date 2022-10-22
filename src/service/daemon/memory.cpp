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

#include "service/protocol/model/runtime.hxx"
#include "service/protocol/model/address.hxx"

namespace mega::service
{

// network::memory::Impl
std::string DaemonRequestConversation::AcquireSharedMemory( const MPO& mpo, boost::asio::yield_context& yield_ctx )
{
    return m_daemon.getMemoryManager().acquire( mpo );
}
void DaemonRequestConversation::ReleaseSharedMemory( const MPO& mpo, boost::asio::yield_context& yield_ctx )
{
    m_daemon.getMemoryManager().release( mpo );

    bool bFirst = true;
    for ( auto& [ id, pCon ] : m_daemon.m_server.getConnections() )
    {
        network::runtime::Request_Sender sender( *this, *pCon, yield_ctx );
        sender.MPODestroyed( mpo, bFirst );
        bFirst = false;
    }
}

mega::network::MemoryConfig DaemonRequestConversation::GetSharedMemoryConfig( boost::asio::yield_context& yield_ctx )
{
    return m_daemon.getMemoryManager().getConfig();
}

void DaemonRequestConversation::NewMachineAddress( const mega::reference&      machineAddress,
                                                   boost::asio::yield_context& yield_ctx )
{
    network::address::Request_Sender rq( *this, m_daemon.m_rootClient, yield_ctx );
    auto                             netAddress = rq.AllocateNetworkAddress( machineAddress, machineAddress.type );
    m_daemon.getMemoryManager().allocated( reference{ machineAddress, machineAddress, netAddress }, machineAddress );
}

void DaemonRequestConversation::NewRootMachineAddress( const mega::reference&      machineAddress,
                                                       boost::asio::yield_context& yield_ctx )
{
    // same for now
    NewMachineAddress( machineAddress, yield_ctx );
}

} // namespace mega::service
