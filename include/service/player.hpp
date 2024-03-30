
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

#ifndef GUARD_2023_November_28_player
#define GUARD_2023_November_28_player

#include "service/host.hpp"
#include "service/clock.hpp"

#include "service/network/logical_thread.hpp"

namespace mega::service
{

class Player : public Host
{
public:
    struct RuntimeLock
    {
        Player&                     m_player;
        network::LogicalThread&     m_logicalThread;
        boost::asio::yield_context& m_yield_context;
        RuntimeLock( Player& player, network::LogicalThread& logicalThread, boost::asio::yield_context& yield_context );
        ~RuntimeLock();
    };

    Player( network::Log log, network::Sender::Ptr pSender, network::Node nodeType, short daemonPortNumber,
            ProcessClock& processClock );

    inline RuntimeLock acquireRuntimeLock( network::LogicalThread&     logicalThread,
                                           boost::asio::yield_context& yield_context );

    // network::LogicalThreadManager
    virtual network::LogicalThreadBase::Ptr joinLogicalThread( const network::Message& msg );

private:
    void releaseRuntimeLock( network::LogicalThread& logicalThread, boost::asio::yield_context& yield_context );

    // std::optional< task::FileHash > m_unityDatabaseHashCode;
    ProcessClock& m_processClock;
};

} // namespace mega::service

#endif // GUARD_2023_November_28_player
