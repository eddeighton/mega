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

#include "service/protocol/model/memory.hxx"
#include "service/protocol/model/address.hxx"

namespace mega::service
{

// network::memory::Impl
void DaemonRequestConversation::MPODestroyed( const MPO& mpo, const bool& bDeleteShared,
                                              boost::asio::yield_context& yield_ctx )
{
    bool bFirst = true;
    for ( auto& [ id, pCon ] : m_daemon.m_server.getConnections() )
    {
        network::memory::Request_Sender sender( *this, *pCon, yield_ctx );
        sender.MPODestroyed( mpo, bFirst );
        bFirst = false;
    }

    m_daemon.getMemoryManager().release( mpo );
}

reference DaemonRequestConversation::Allocate( const MPO& mpo, const TypeID& objectTypeID,
                                               boost::asio::yield_context& yield_ctx )
{
    const NetworkAddress networkAddress
        = getRootRequest< network::address::Request_Encoder >( yield_ctx ).AllocateNetworkAddress( mpo, objectTypeID );
    return m_daemon.getMemoryManager().allocate( mpo, objectTypeID, networkAddress );
}

std::string DaemonRequestConversation::Acquire( const MPO& mpo, boost::asio::yield_context& yield_ctx )
{
    return m_daemon.getMemoryManager().acquire( mpo );
}

reference DaemonRequestConversation::NetworkToMachine( const reference& ref, boost::asio::yield_context& yield_ctx )
{
    return m_daemon.getMemoryManager().networkToMachine( ref );
}

} // namespace mega::service
