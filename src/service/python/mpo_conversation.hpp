
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

#ifndef GUARD_2023_March_09_mpo_conversation
#define GUARD_2023_March_09_mpo_conversation

#include "request.hpp"

#include "service/mpo_context.hpp"

namespace mega::service::python
{

class MPOConversation : public PythonRequestConversation, public mega::MPOContext
{
    bool    m_bRunning = true;
    Python& m_python;

public:
    MPOConversation( Python& python, const network::ConversationID& conversationID )
        : PythonRequestConversation( python, conversationID )
        , mega::MPOContext( conversationID )
        , m_python( python )
    {
    }

    virtual network::Message dispatchRequest( const network::Message&     msg,
                                              boost::asio::yield_context& yield_ctx ) override
    {
        return PythonRequestConversation::dispatchRequest( msg, yield_ctx );
    }

    network::python_leaf::Request_Sender getPythonRequest( boost::asio::yield_context& yield_ctx )
    {
        return network::python_leaf::Request_Sender( *this, m_python.getLeafSender(), yield_ctx );
    }
    network::mpo::Request_Sender getMPRequest( boost::asio::yield_context& yield_ctx )
    {
        return network::mpo::Request_Sender( *this, m_python.getLeafSender(), yield_ctx );
    }
    virtual network::enrole::Request_Encoder getRootEnroleRequest() override
    {
        VERIFY_RTE( m_pYieldContext );
        return { [ leafRequest = getPythonRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
                 { return leafRequest.PythonRoot( msg ); },
                 getID() };
    }
    virtual network::stash::Request_Encoder getRootStashRequest() override
    {
        VERIFY_RTE( m_pYieldContext );
        return { [ leafRequest = getPythonRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
                 { return leafRequest.PythonRoot( msg ); },
                 getID() };
    }
    virtual network::memory::Request_Encoder getDaemonMemoryRequest() override
    {
        VERIFY_RTE( m_pYieldContext );
        return { [ leafRequest = getPythonRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
                 { return leafRequest.PythonDaemon( msg ); },
                 getID() };
    }
    virtual network::sim::Request_Encoder getMPOSimRequest( mega::MPO mpo ) override
    {
        VERIFY_RTE( m_pYieldContext );
        return { [ leafRequest = getMPRequest( *m_pYieldContext ), mpo ]( const network::Message& msg ) mutable
                 { return leafRequest.MPOUp( msg, mpo ); },
                 getID() };
    }
    virtual network::memory::Request_Sender getLeafMemoryRequest() override
    {
        VERIFY_RTE( m_pYieldContext );
        return { *this, m_python.getLeafSender(), *m_pYieldContext };
    }
    virtual network::jit::Request_Sender getLeafJITRequest() override
    {
        VERIFY_RTE( m_pYieldContext );
        return { *this, m_python.getLeafSender(), *m_pYieldContext };
    }

    virtual network::mpo::Request_Sender getMPRequest() override
    {
        VERIFY_RTE( m_pYieldContext );
        return getMPRequest( *m_pYieldContext );
    }

    void run( boost::asio::yield_context& yield_ctx ) override
    {
        SPDLOG_TRACE( "PYTHON MPO: run" );
        network::sim::Request_Encoder request(
            [ rootRequest = getMPRequest( yield_ctx ) ]( const network::Message& msg ) mutable
            { return rootRequest.MPRoot( msg, mega::MP{} ); },
            getID() );
        request.SimStart();

        dispatchRemaining( yield_ctx );
    }

    virtual void RootSimRun( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx ) override
    {
        m_python.setMPO( mpo );

        setMPOContext( this );
        m_pYieldContext = &yield_ctx;

        // note the runtime will query getThisMPO while creating the root
        SPDLOG_TRACE( "PYTHON RootSimRun: Acquired mpo context: {}", mpo );
        {
            createRoot( mpo );

            while( m_bRunning )
            {
                run_one( yield_ctx );
            }
        }
        SPDLOG_TRACE( "PYTHON RootSimRun: Releasing mpo context: {}", mpo );

        m_pYieldContext = nullptr;
        resetMPOContext();
    }

    virtual network::Status GetStatus( const std::vector< network::Status >& childNodeStatus,
                                       boost::asio::yield_context&           yield_ctx ) override
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
                os << "Python: " << m_log.getTimeStamp();
            }

            status.setLogIterator( m_log.getIterator() );

            status.setAllocationID( m_pMemoryManager->getAllocationID() );
            status.setAllocationCount( m_pMemoryManager->getAllocationCount() );
        }

        return status;
    }

    // mega::MPOContext
    // clock
    virtual TimeStamp cycle() override { return TimeStamp{}; }
    virtual F32       ct() override { return F32{}; }
    virtual F32       dt() override { return F32{}; }
};
} // namespace mega::service::python

#endif // GUARD_2023_March_09_mpo_conversation
