
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

#include <boost/beast/version.hpp>

namespace mega::service::report
{

MPOLogicalThread::MPOLogicalThread( Report&                         report,
                                    const network::LogicalThreadID& logicalthreadID,
                                    boost::asio::ip::tcp::socket&   socket )
    : ReportRequestLogicalThread( report, logicalthreadID )
    , mega::MPOContext( getID() )
    , m_report( report )
    , m_tcpStream( std::move( socket ) )
{
}

network::Message MPOLogicalThread::dispatchInBoundRequest( const network::Message&     msg,
                                                           boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if( result = network::report::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    return ReportRequestLogicalThread::dispatchInBoundRequest( msg, yield_ctx );
}

network::report_leaf::Request_Sender MPOLogicalThread::getReportRequest( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_report.getLeafSender(), yield_ctx };
}

network::mpo::Request_Sender MPOLogicalThread::getMPRequest( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_report.getLeafSender(), yield_ctx };
}

network::enrole::Request_Encoder MPOLogicalThread::getRootEnroleRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { [ leafRequest = getReportRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
             { return leafRequest.ReportRoot( msg ); },
             getID() };
}

network::stash::Request_Encoder MPOLogicalThread::getRootStashRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { [ leafRequest = getReportRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
             { return leafRequest.ReportRoot( msg ); },
             getID() };
}

network::memory::Request_Encoder MPOLogicalThread::getDaemonMemoryRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { [ leafRequest = getReportRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
             { return leafRequest.ReportDaemon( msg ); },
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
    return { *this, m_report.getLeafSender(), *m_pYieldContext };
}

network::jit::Request_Sender MPOLogicalThread::getLeafJITRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { *this, m_report.getLeafSender(), *m_pYieldContext };
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
            if( m_mpo.has_value() || ( msg.msg.getID() == network::leaf_report::MSG_RootSimRun_Request::ID ) )
            {
                acknowledgeInboundRequest( msg, yield_ctx );
            }
            else
            {
                // queue the messages while waiting for RootSim run to complete
                SPDLOG_TRACE( "MPOLogicalThread::dispatchInBoundRequestsUntilResponse queued: {}", msg.msg );
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
    SPDLOG_TRACE( "REPORT MPOLogicalThread: run" );
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

    setMPOContext( this );
    m_pYieldContext = &yield_ctx;

    SPDLOG_TRACE( "REPORT RootSimRun: Acquired mpo context: {}", mpo );
    {
        createRoot( project, mpo );

        for( const auto& msg : m_messageQueue )
        {
            SPDLOG_ERROR( "Unexpected pending message when starting up MPOLogicalThread: {}", msg.msg );
            using ::operator<<;
            THROW_RTE( "Unexpected pending message when starting up MPOLogicalThread: " << msg.msg );
        }

        m_bRunning = true;
        runHTMLSession( yield_ctx );
    }
    SPDLOG_TRACE( "REPORT RootSimRun: Releasing mpo context: {}", mpo );

    m_pYieldContext = nullptr;
    resetMPOContext();
}

// Return a response for the given request.
//
// The concrete type of the response message (which depends on the
// request), is type-erased in message_generator.

boost::beast::http::message_generator
MPOLogicalThread::handleRequest( boost::beast::http::request< boost::beast::http::string_body >& req,
                                 boost::asio::yield_context&                                     yield_ctx )
{
    namespace beast = boost::beast;         // from <boost/beast.hpp>
    namespace http  = beast::http;          // from <boost/beast/http.hpp>
    namespace net   = boost::asio;          // from <boost/asio.hpp>
    using tcp       = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

    // Returns a bad request response
    auto const bad_request = [ &req ]( beast::string_view why )
    {
        http::response< http::string_body > res{ http::status::bad_request, req.version() };
        res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
        res.set( http::field::content_type, "text/html" );
        res.keep_alive( req.keep_alive() );
        res.body() = std::string( why );
        res.prepare_payload();
        return res;
    };

    // Returns a not found response
    auto const not_found = [ &req ]( beast::string_view target )
    {
        http::response< http::string_body > res{ http::status::not_found, req.version() };
        res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
        res.set( http::field::content_type, "text/html" );
        res.keep_alive( req.keep_alive() );
        res.body() = "The resource '" + std::string( target ) + "' was not found.";
        res.prepare_payload();
        return res;
    };

    // Returns a server error response
    auto const server_error = [ &req ]( beast::string_view what )
    {
        http::response< http::string_body > res{ http::status::internal_server_error, req.version() };
        res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
        res.set( http::field::content_type, "text/html" );
        res.keep_alive( req.keep_alive() );
        res.body() = "An error occurred: '" + std::string( what ) + "'";
        res.prepare_payload();
        return res;
    };

    // Make sure we can handle the method
    if( req.method() != http::verb::get && req.method() != http::verb::head )
    {
        return bad_request( "Unknown HTTP-method" );
    }

    http::string_body::value_type body;
    {
        using ::           operator<<;
        std::ostringstream os;
        os << "<html><body><H1>";
        os << "Hello from MPOLogicalThread: " << getThisMPO() << "</h1></body></html>";
        body = os.str();
    }

    // Cache the size since we need it after the move
    auto const size = body.size();

    // Respond to HEAD request
    if( req.method() == http::verb::head )
    {
        http::response< http::empty_body > res{ http::status::ok, req.version() };
        res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
        res.set( http::field::content_type, "text/html" );
        res.content_length( size );
        res.keep_alive( req.keep_alive() );
        return res;
    }
    else
    {
        // Respond to GET request
        http::response< http::string_body > res{ std::piecewise_construct, std::make_tuple( std::move( body ) ),
                                                    std::make_tuple( http::status::ok, req.version() ) };
        res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
        res.set( http::field::content_type, "text/html" );
        res.content_length( size );
        res.keep_alive( req.keep_alive() );
        return res;
    }
}

void MPOLogicalThread::runHTMLSession( boost::asio::yield_context& yield_ctx )
{
    namespace beast = boost::beast;         // from <boost/beast.hpp>
    namespace http  = beast::http;          // from <boost/beast/http.hpp>
    namespace net   = boost::asio;          // from <boost/asio.hpp>
    using tcp       = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

    beast::error_code ec;

    // This buffer is required to persist across reads
    beast::flat_buffer buffer;

    // This lambda is used to send messages
    for( ;; )
    {
        // Set the timeout.
        m_tcpStream.expires_after( std::chrono::seconds( 1 ) );

        // Read a request
        http::request< http::string_body > req;
        {
            mega::_MPOContextStack _mpoStack;
            http::async_read( m_tcpStream, buffer, req, yield_ctx[ ec ] );
        }
        if( ec == http::error::end_of_stream )
        {
            break;
        }
        if( ec )
        {
            //THROW_RTE( "Error reading html request: " << ec );
            break;
        }

        // Handle the request
        http::message_generator msg = handleRequest( req, yield_ctx );

        // Determine if we should close the connection
        m_bRunning = m_bRunning && msg.keep_alive();

        // Send the response
        {
            mega::_MPOContextStack _mpoStack;
            beast::async_write( m_tcpStream, std::move( msg ), yield_ctx[ ec ] );
        }

        if( ec )
        {
            THROW_RTE( "Error writing html response: " << ec );
        }

        if( !m_bRunning )
        {
            // This means we should close the connection, usually because
            // the response indicated the "Connection: close" semantic.
            break;
        }

        //run_one( yield_ctx );
    }

    // Send a TCP shutdown
    m_tcpStream.socket().shutdown( tcp::socket::shutdown_send, ec );
}

std::string MPOLogicalThread::GetReport( const std::string& request, boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "MPOLogicalThread::GetReport" );
    return {};
}

} // namespace mega::service::report