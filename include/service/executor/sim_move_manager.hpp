
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

#ifndef GUARD_2023_September_05_sim_move_manager
#define GUARD_2023_September_05_sim_move_manager

#include "service/protocol/common/received_message.hpp"

#include <vector>
#include <set>
#include <unordered_map>

namespace mega::service
{

class SimMoveManager
{
    using Msg       = network::ReceivedMessage;
    using MsgVector = std::vector< Msg >;
    using AckVector = MsgVector;

    using MoveID = mega::U64;

    struct OwnMoveRequest
    {
        U64 id;
        using Map = std::unordered_map< U64, OwnMoveRequest >;
    };
    OwnMoveRequest::Map m_ownMoveRequest;
    bool                m_bSendingOwnMoveRequests = false;

    struct ForwardedMoveRequest
    {
        U64 id;
        using Map = std::unordered_map< U64, ForwardedMoveRequest >;
    };
    ForwardedMoveRequest::Map m_forwardedMoveRequest;

    static inline network::MessageID getMsgID( const Msg& msg ) { return msg.msg.getID(); }

public:
    using MoveComplete = network::sim::MSG_SimMoveComplete_Response;
    using MoveRequest  = network::sim::MSG_SimMove_Request;
    using MoveResponse = network::sim::MSG_SimMove_Response;

    inline bool sendMoveRequests()
    {
        m_bSendingOwnMoveRequests = false;
        return false;
    }

    enum MsgResult
    {
        eNothing,
        eMoveRequestsComplete,
        eProcessMoveComplete
    };

    inline void onMoveRequest( const MoveRequest& msg )
    {
        //
    }
    inline void onMoveResponse( const MoveResponse& msg )
    {
        //
    }

    inline MsgResult onMsg( const MsgVector& moveMsgs )
    {
        MsgResult result = eNothing;
        for( const auto& msg : moveMsgs )
        {
            switch( getMsgID( msg ) )
            {
                case MoveComplete::ID:
                {
                    VERIFY_RTE_MSG( !m_bSendingOwnMoveRequests,
                                    "Unreachable state of move complete when not finished sending move requests" );
                    result = eProcessMoveComplete;
                }
                break;
                case MoveRequest::ID:
                {
                    onMoveRequest( MoveRequest::get( msg.msg ) );
                }
                break;
                case MoveResponse::ID:
                {
                    onMoveResponse( MoveResponse::get( msg.msg ) );
                }
                break;
                default:
                {
                    THROW_RTE( "Unknown move request message type" );
                }
                break;
            }
        }

        if( m_bSendingOwnMoveRequests )
        {
            if( m_ownMoveRequest.empty() )
            {
                m_bSendingOwnMoveRequests = false;
                result                    = eMoveRequestsComplete;
            }
        }
        return result;
    }
};

} // namespace mega::service

#endif // GUARD_2023_September_05_sim_move_manager
