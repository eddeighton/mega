
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

#include "mpo_conversation.hpp"

#include "service/protocol/model/jit.hxx"

namespace mega::service::python
{

MPOConversation::MPOConversation( Python& python, const network::ConversationID& conversationID )
    : PythonRequestConversation( python, conversationID )
    , mega::MPOContext( conversationID )
    , m_python( python )
{
}

network::Message MPOConversation::dispatchRequest( const network::Message& msg, boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if( result = network::python::Impl::dispatchRequest( msg, yield_ctx ); result )
        return result;
    return PythonRequestConversation::dispatchRequest( msg, yield_ctx );
}

network::python_leaf::Request_Sender MPOConversation::getPythonRequest( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_python.getLeafSender(), yield_ctx };
}

network::mpo::Request_Sender MPOConversation::getMPRequest( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_python.getLeafSender(), yield_ctx };
}

network::enrole::Request_Encoder MPOConversation::getRootEnroleRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { [ leafRequest = getPythonRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
             { return leafRequest.PythonRoot( msg ); },
             getID() };
}

network::stash::Request_Encoder MPOConversation::getRootStashRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { [ leafRequest = getPythonRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
             { return leafRequest.PythonRoot( msg ); },
             getID() };
}

network::memory::Request_Encoder MPOConversation::getDaemonMemoryRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { [ leafRequest = getPythonRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
             { return leafRequest.PythonDaemon( msg ); },
             getID() };
}

network::sim::Request_Encoder MPOConversation::getMPOSimRequest( mega::MPO mpo )
{
    VERIFY_RTE( m_pYieldContext );
    return { [ leafRequest = getMPRequest( *m_pYieldContext ), mpo ]( const network::Message& msg ) mutable
             { return leafRequest.MPOUp( msg, mpo ); },
             getID() };
}

network::memory::Request_Sender MPOConversation::getLeafMemoryRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { *this, m_python.getLeafSender(), *m_pYieldContext };
}

network::jit::Request_Sender MPOConversation::getLeafJITRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { *this, m_python.getLeafSender(), *m_pYieldContext };
}

network::mpo::Request_Sender MPOConversation::getMPRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return getMPRequest( *m_pYieldContext );
}

network::Message MPOConversation::dispatchRequestsUntilResponse( boost::asio::yield_context& yield_ctx )
{
    network::ReceivedMsg msg;
    while( true )
    {
        msg = receive( yield_ctx );

        // simulation is running so process as normal
        if( isRequest( msg.msg ) )
        {
            if( m_mpo.has_value() || ( msg.msg.getID() == network::leaf_python::MSG_RootSimRun_Request::ID ) )
            {
                dispatchRequestImpl( msg, yield_ctx );

                // check if connection has disconnected
                if( !m_disconnections.empty() )
                {
                    ASSERT( !m_stack.empty() );
                    if( m_disconnections.count( m_stack.back() ) )
                    {
                        SPDLOG_ERROR(
                            "Generating disconnect on conversation: {} for connection: {}", getID(), m_stack.back() );
                        const network::ReceivedMsg rMsg{
                            m_stack.back(), network::make_error_msg( msg.msg.getReceiverID(), "Disconnection" ) };
                        send( rMsg );
                    }
                }
            }
            else
            {
                // queue the messages while waiting for RootSim run to complete
                SPDLOG_TRACE( "SIM::dispatchRequestsUntilResponse queued: {}", msg.msg );
                m_messageQueue.push_back( msg );
            }
        }
        else
        {
            break;
        }
    }
    if( msg.msg.getID() == network::MSG_Error_Response::ID )
    {
        throw std::runtime_error( network::MSG_Error_Response::get( msg.msg ).what );
    }
    return msg.msg;
}

void MPOConversation::run( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "PYTHON MPOConversation: run" );
    network::sim::Request_Encoder request(
        [ rootRequest = getMPRequest( yield_ctx ) ]( const network::Message& msg ) mutable
        { return rootRequest.MPRoot( msg, mega::MP{} ); },
        getID() );
    request.SimStart();

    dispatchRemaining( yield_ctx );
}

void MPOConversation::RootSimRun( const mega::MPO& mpo, boost::asio::yield_context& yield_ctx )
{
    m_mpo = mpo;
    m_python.setMPO( mpo );

    setMPOContext( this );
    m_pYieldContext = &yield_ctx;

    // note the runtime will query getThisMPO while creating the root
    SPDLOG_TRACE( "PYTHON RootSimRun: Acquired mpo context: {}", mpo );
    {
        createRoot( mpo );

        for( const auto& msg : m_messageQueue )
        {
            if( msg.msg.getID() == network::python::MSG_PythonGetIdentities_Request::ID )
            {
                SPDLOG_TRACE( "PYTHON MPOConversation: run got network::python::MSG_PythonGetIdentities_Request::ID" );
                dispatchRequestImpl( msg, yield_ctx );
            }
            else
            {
                THROW_RTE( "Unexpected pending message when starting up MPOConversation" );
            }
        }

        while( m_bRunning )
        {
            run_one( yield_ctx );
        }
    }
    SPDLOG_TRACE( "PYTHON RootSimRun: Releasing mpo context: {}", mpo );

    m_pYieldContext = nullptr;
    resetMPOContext();
}

std::unordered_map< std::string, mega::TypeID > MPOConversation::PythonGetIdentities( boost::asio::yield_context& )
{
    SPDLOG_TRACE( "MPOConversation::PythonGetIdentities" );
    return getLeafJITRequest().GetIdentities();
}

void MPOConversation::PythonExecuteJIT( const mega::runtime::JITFunctor& func, boost::asio::yield_context& )
{
    SPDLOG_TRACE( "MPOConversation::PythonExecuteJIT" );
    getLeafJITRequest().ExecuteJIT( func );
}

} // namespace mega::service::python