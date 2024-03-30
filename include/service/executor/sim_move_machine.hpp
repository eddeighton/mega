
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

#include "service/executor/message_traits.hpp"

#include "service/mpo_visitor.hpp"
#include "service/protocol/common/received_message.hpp"
#include "service/protocol/common/transaction.hpp"

#include "mega/reference_tree.hpp"
#include "mega/tree_visitor.hpp"

#include <vector>
#include <set>
#include <unordered_map>

namespace mega::service
{
/*
struct MoveVisitor : public TreeVisitor< Pointer >
{
};
*/
struct MoveMsgTraits : public MsgTraits
{
    using MoveComplete = network::sim::MSG_SimMoveComplete_Response;
    using MoveRequest  = network::sim::MSG_SimMove_Request;
    using MoveResponse = network::sim::MSG_SimMove_Response;

    static inline bool isMsg( const Msg& msg )
    {
        switch( getMsgID( msg ) )
        {
            case MoveComplete::ID:
            case MoveRequest::ID:
            case MoveResponse::ID:
                return true;
            default:
                return false;
        }
    }
};

template < typename Simulation >
class SimMoveMachine : public MoveMsgTraits
{
    MsgVector                                   m_queue;
    Simulation&                                 m_sim;
    AckVector&                                  m_ackVector;
    network::TransactionProducer::MovedObjects& m_movedObjects;

    using MoveID = mega::U64;

    struct OwnMoveRequest
    {
        U64 id;
        using Map = std::unordered_map< U64, OwnMoveRequest >;
    };
    typename OwnMoveRequest::Map m_ownMoveRequest;
    bool                         m_bSendingOwnMoveRequests = false;

    struct ForwardedMoveRequest
    {
        U64 id;
        using Map = std::unordered_map< U64, ForwardedMoveRequest >;
    };
    typename ForwardedMoveRequest::Map m_forwardedMoveRequest;

    static inline network::MessageID getMsgID( const Msg& msg ) { return msg.msg.getID(); }

public:
    SimMoveMachine( Simulation& sim, AckVector& ackVector, network::TransactionProducer::MovedObjects& movedObjects )
        : m_sim( sim )
        , m_ackVector( ackVector )
        , m_movedObjects( movedObjects )
    {
    }

    // return true is should send MoveCompleteRequest
    inline bool sendMoveRequests()
    {
        m_bSendingOwnMoveRequests = false;

        // for( const auto& [ from, to ] : m_movedObjects )
        {
            // THROW_TODO;
            // MPORealVisitor                                        reader( from );
            // MoveVisitor                                           writer;
            // ReferenceTreeTraversal< MPORealVisitor, MoveVisitor > traversal( reader, writer );
            // traverse( traversal );
        }

        m_movedObjects.clear();

        return !m_bSendingOwnMoveRequests;
    }

    inline void onMoveRequest( const MoveRequest& )
    {
        //
    }
    inline void onMoveResponse( const MoveResponse& )
    {
        //
    }

    enum MsgResult
    {
        eNothing,
        eMoveRequestsComplete,
        eProcessMoveComplete
    };

    inline void queue( const Msg& msg ) { m_queue.push_back( msg ); }

    inline MsgResult onMessage( const Msg& msg )
    {
        MsgResult result = eNothing;

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
