
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

#include "mpo_logical_thread.hpp"

#include "service/protocol/model/jit.hxx"

#include "service/protocol/common/type_erase.hpp"

namespace mega::service::python
{

MPOLogicalThread::MPOLogicalThread( Python& python, const network::LogicalThreadID& logicalthreadID )
    : PythonRequestLogicalThread( python, logicalthreadID )
    , mega::MPOContext( logicalthreadID )
    , m_python( python )
{
}

network::Message MPOLogicalThread::dispatchInBoundRequest( const network::Message&     msg,
                                                           boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if( result = network::python::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    return PythonRequestLogicalThread::dispatchInBoundRequest( msg, yield_ctx );
}

network::python_leaf::Request_Sender MPOLogicalThread::getPythonRequest( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_python.getLeafSender(), yield_ctx };
}

network::mpo::Request_Sender MPOLogicalThread::getMPRequest( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_python.getLeafSender(), yield_ctx };
}

network::enrole::Request_Encoder MPOLogicalThread::getRootEnroleRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { [ leafRequest = getPythonRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
             { return leafRequest.PythonRoot( msg ); },
             getID() };
}

network::stash::Request_Encoder MPOLogicalThread::getRootStashRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { [ leafRequest = getPythonRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
             { return leafRequest.PythonRoot( msg ); },
             getID() };
}

network::memory::Request_Encoder MPOLogicalThread::getDaemonMemoryRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { [ leafRequest = getPythonRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
             { return leafRequest.PythonDaemon( msg ); },
             getID() };
}

network::sim::Request_Encoder MPOLogicalThread::getMPOSimRequest( mega::MPO mpo )
{
    VERIFY_RTE( m_pYieldContext );
    return { [ leafRequest = getMPRequest( *m_pYieldContext ), mpo ]( const network::Message& msg ) mutable
             { return leafRequest.MPOUp( msg, mpo ); },
             getID() };
}

network::memory::Request_Sender MPOLogicalThread::getLeafMemoryRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { *this, m_python.getLeafSender(), *m_pYieldContext };
}

network::jit::Request_Sender MPOLogicalThread::getLeafJITRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { *this, m_python.getLeafSender(), *m_pYieldContext };
}

network::mpo::Request_Sender MPOLogicalThread::getMPRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return getMPRequest( *m_pYieldContext );
}

network::Message MPOLogicalThread::dispatchInBoundRequestsUntilResponse( boost::asio::yield_context& yield_ctx )
{
    network::ReceivedMessage msg;
    while( true )
    {
        msg = receive( yield_ctx );

        // simulation is running so process as normal
        if( isRequest( msg.msg ) )
        {
            if( m_mpo.has_value() || ( msg.msg.getID() == network::leaf_python::MSG_RootSimRun_Request::ID ) )
            {
                acknowledgeInboundRequest( msg, yield_ctx );
            }
            else
            {
                // queue the messages while waiting for RootSim run to complete
                SPDLOG_TRACE( "SIM::dispatchInBoundRequestsUntilResponse queued: {}", msg.msg );
                m_messageQueue.push_back( msg );
            }
        }
        else
        {
            break;
        }
    }
    if( msg.msg.getID() == network::MSG_Error_Disconnect::ID )
    {
        const std::string& strError = network::MSG_Error_Disconnect::get( msg.msg ).what;
        SPDLOG_ERROR( "Got error disconnect: {}", strError );
        throw std::runtime_error( strError );
    }
    else if( msg.msg.getID() == network::MSG_Error_Response::ID )
    {
        const std::string& strError = network::MSG_Error_Response::get( msg.msg ).what;
        SPDLOG_ERROR( "Got error response: {}", strError );
        throw std::runtime_error( strError );
    }
    return msg.msg;
}

void MPOLogicalThread::run( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "PYTHON MPOLogicalThread: run" );
    network::sim::Request_Encoder request(
        [ rootRequest = getMPRequest( yield_ctx ) ]( const network::Message& msg ) mutable
        { return rootRequest.MPRoot( msg, mega::MP{} ); },
        getID() );
    request.SimStart();

    dispatchRemaining( yield_ctx );

    m_bRunComplete = true;
}

void MPOLogicalThread::RootSimRun( const Project& project, const mega::MPO& mpo, boost::asio::yield_context& yield_ctx )
{
    m_mpo = mpo;
    m_python.setMPO( mpo );

    setMPOContext( this );
    m_pYieldContext = &yield_ctx;

    // note the runtime will query getThisMPO while creating the root
    SPDLOG_TRACE( "PYTHON RootSimRun: Acquired mpo context: {}", mpo );
    {
        createRoot( project, mpo );

        for( const auto& msg : m_messageQueue )
        {
            if( msg.msg.getID() == network::python::MSG_PythonGetIdentities_Request::ID )
            {
                SPDLOG_TRACE( "PYTHON RootSimRun: run got network::python::MSG_PythonGetIdentities_Request::ID" );
                acknowledgeInboundRequest( msg, yield_ctx );
            }
            else
            {
                THROW_RTE( "Unexpected pending message when starting up MPOLogicalThread" );
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

TypeID MPOLogicalThread::PythonGetInterfaceTypeID( const TypeID& concreteTypeID, boost::asio::yield_context& )
{
    SPDLOG_TRACE( "MPOLogicalThread::PythonGetInterfaceTypeID" );
    return getLeafJITRequest().GetInterfaceTypeID( concreteTypeID );
}

std::unordered_map< std::string, mega::TypeID > MPOLogicalThread::PythonGetIdentities( boost::asio::yield_context& )
{
    SPDLOG_TRACE( "MPOLogicalThread::PythonGetIdentities" );
    return getLeafJITRequest().GetIdentities();
}

void MPOLogicalThread::PythonExecuteJIT( const mega::runtime::JITFunctor& func, boost::asio::yield_context& )
{
    SPDLOG_TRACE( "MPOLogicalThread::PythonExecuteJIT" );
    getLeafJITRequest().ExecuteJIT( func );
}

TimeStamp MPOLogicalThread::PythonCycle( boost::asio::yield_context& )
{
    SPDLOG_TRACE( "MPOLogicalThread::PythonCycle" );
    cycleComplete();
    return getLog().getTimeStamp();
}

void MPOLogicalThread::PythonFunctor( const mega::runtime::Functor& functor, boost::asio::yield_context& )
{
    SPDLOG_TRACE( "MPOLogicalThread::PythonFunctor" );
    functor();
}

void MPOLogicalThread::PythonShutdown( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "MPOLogicalThread::PythonShutdown" );
    m_bRunning = false;
}

} // namespace mega::service::python