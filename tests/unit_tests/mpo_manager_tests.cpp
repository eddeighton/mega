
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

// GUARD_2022_November_04_mpo_manager_tests

#include <gtest/gtest.h>

#include "service/root/mpo_manager.hpp"

using namespace mega;
using namespace mega::service;

static const network::ConversationID testID( 123, "test" );

TEST( MPO, basic )
{
    {
        MP m1( 0, 0 );
        ASSERT_EQ( m1.getMachineID(), MachineID( 0U ) );
        ASSERT_EQ( m1.getProcessID(), 0U );
    }
    {
        MP m1( 1, 1 );
        ASSERT_EQ( m1.getMachineID(), MachineID( 1U ) );
        ASSERT_EQ( m1.getProcessID(), 1U );
    }
    {
        MP m1( MAX_MACHINES - 1, MAX_PROCESS_PER_MACHINE - 1 );
        ASSERT_EQ( m1.getMachineID(), MachineID( MAX_MACHINES - 1 ) );
        ASSERT_EQ( m1.getProcessID(), MAX_PROCESS_PER_MACHINE - 1 );
    }
}

TEST( MPOManager, basic )
{
    MPOManager mpoMgr;

    ASSERT_TRUE( mpoMgr.getMachines().empty() );

    {
        MachineID d1 = mpoMgr.newDaemon();
        ASSERT_EQ( d1, MachineID{ 0U } );
        ASSERT_EQ( mpoMgr.getMachines(), std::vector< MachineID >{ 0U } );
    }

    {
        MachineID d2 = mpoMgr.newDaemon();
        ASSERT_EQ( d2, MachineID{ 1U } );
        std::vector< MachineID > expected{ 0U, 1U };
        ASSERT_EQ( mpoMgr.getMachines(), expected );
    }

    ASSERT_EQ( mpoMgr.newLeaf( 0 ), MP( 0, 0 ) );
    ASSERT_EQ( mpoMgr.newLeaf( 0 ), MP( 0, 1 ) );
    ASSERT_EQ( mpoMgr.newLeaf( 0 ), MP( 0, 2 ) );

    ASSERT_EQ( mpoMgr.newLeaf( 1 ), MP( 1, 0 ) );
    ASSERT_EQ( mpoMgr.newLeaf( 1 ), MP( 1, 1 ) );
    ASSERT_EQ( mpoMgr.newLeaf( 1 ), MP( 1, 2 ) );

    ASSERT_EQ( mpoMgr.newOwner( MP( 0, 0 ), testID ), MPO( 0, 0, 0 ) );
    ASSERT_EQ( mpoMgr.newOwner( MP( 0, 0 ), testID ), MPO( 0, 0, 1 ) );

    ASSERT_EQ( mpoMgr.newOwner( MP( 1, 0 ), testID ), MPO( 1, 0, 0 ) );
    ASSERT_EQ( mpoMgr.newOwner( MP( 1, 0 ), testID ), MPO( 1, 0, 1 ) );

    ASSERT_EQ( mpoMgr.newOwner( MP( 0, 1 ), testID ), MPO( 0, 1, 0 ) );
    ASSERT_EQ( mpoMgr.newOwner( MP( 0, 2 ), testID ), MPO( 0, 2, 0 ) );

    ASSERT_EQ( mpoMgr.newOwner( MP( 1, 1 ), testID ), MPO( 1, 1, 0 ) );
    ASSERT_EQ( mpoMgr.newOwner( MP( 1, 2 ), testID ), MPO( 1, 2, 0 ) );
    ASSERT_EQ( mpoMgr.newOwner( MP( 1, 1 ), testID ), MPO( 1, 1, 1 ) );

    {
        std::vector< MachineID > expected{ 0U, 1U };
        ASSERT_EQ( mpoMgr.getMachines(), expected );
    }
    {
        std::vector< MP > expected{ MP( 0, 0 ), MP( 0, 1 ), MP( 0, 2 ) };
        ASSERT_EQ( mpoMgr.getMachineProcesses( 0 ), expected );
    }
    {
        std::vector< MP > expected{ MP( 1, 0 ), MP( 1, 1 ), MP( 1, 2 ) };
        ASSERT_EQ( mpoMgr.getMachineProcesses( 1 ), expected );
    }
    {
        std::vector< MPO > expected{ MPO( 0, 0, 0 ), MPO( 0, 0, 1 ) };
        ASSERT_EQ( mpoMgr.getMPO( MP( 0, 0 ) ), expected );
    }
    {
        std::vector< MPO > expected{ MPO( 1, 1, 0 ), MPO( 1, 1, 1 ) };
        ASSERT_EQ( mpoMgr.getMPO( MP( 1, 1 ) ), expected );
    }

    mpoMgr.release( MPO( 1, 1, 1 ) );
    {
        std::vector< MPO > expected{ MPO( 1, 1, 0 ) };
        ASSERT_EQ( mpoMgr.getMPO( MP( 1, 1 ) ), expected );
    }

    mpoMgr.release( MPO( 0, 1, 0 ) );
    {
        std::vector< MPO > expected{};
        ASSERT_EQ( mpoMgr.getMPO( MP( 0, 1 ) ), expected );
    }

    mpoMgr.leafDisconnected( MP( 1, 0 ) );
    {
        std::vector< MP > expected{ MP( 1, 1 ), MP( 1, 2 ) };
        ASSERT_EQ( mpoMgr.getMachineProcesses( 1 ), expected );
    }
    ASSERT_EQ( mpoMgr.newLeaf( 1 ), MP( 1, 0 ) );

    mpoMgr.daemonDisconnect( 0 );
    {
        std::vector< MachineID > expected{ 1U };
        ASSERT_EQ( mpoMgr.getMachines(), expected );
    }

    {
        MachineID d2 = mpoMgr.newDaemon();
        ASSERT_EQ( d2, MachineID{ 0U } );
        std::vector< MachineID > expected{ 0U, 1U };
        ASSERT_EQ( mpoMgr.getMachines(), expected );
    }
}
