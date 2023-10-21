
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

#include "reports/renderer.hpp"

#include "mega/iterator.hpp"
#include "mega/logical_tree.hpp"
#include "mega/printer.hpp"

#include "service/mpo_visitor.hpp"
#include "service/protocol/model/jit.hxx"
#include "service/reporters.hpp"

#include "service/protocol/common/type_erase.hpp"
#include "service/protocol/common/sender_ref.hpp"

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

    boost::beast::error_code ec;

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

        startTCPStream();

        while( m_bRunning )
        {
            unqueue();
            const network::ReceivedMessage msg = receiveDeferred( yield_ctx );

            switch( msg.msg.getID() )
            {
                case HTTPRequestMsg::ID:
                {
                    const auto& httpRequest = HTTPRequestMsg::get( msg.msg ).httpRequestData;
                    switch( httpRequest.verb )
                    {
                        case eClose:
                        case eError:
                        {
                            m_bRunning = false;
                            SPDLOG_TRACE( "MPOLogicalThread::RootSimRun shutting down" );
                        }
                        break;
                        case eGet:
                        case eHead:
                        case ePost:
                        {
                            // generate response
                            if( httpRequest.version != 0 )
                            {
                                SPDLOG_TRACE( "MPOLogicalThread::RootSimRun got html request" );
                                // Handle the request
                                boost::beast::http::message_generator httpMsg
                                    = handleHTTPRequest( httpRequest, yield_ctx );

                                // Determine if we should close the connection
                                m_bRunning = httpMsg.keep_alive();

                                // Send the response
                                {
                                    mega::_MPOContextStack _mpoStack;
                                    boost::beast::async_write( m_tcpStream, std::move( httpMsg ), yield_ctx[ ec ] );
                                }

                                if( ec )
                                {
                                    THROW_RTE( "Error writing html response: " << ec );
                                }
                            }
                        }
                        break;
                        case TOTAL_HTTP_VERBS:
                        default:
                        {
                            THROW_RTE( "Unknown verb type" );
                        }
                    }
                }
                break;
                default:
                {
                    acknowledgeInboundRequest( msg, yield_ctx );
                }
                break;
            }
        }
    }
    SPDLOG_TRACE( "REPORT RootSimRun: Releasing mpo context: {}", mpo );

    m_tcpStream.socket().shutdown( boost::asio::ip::tcp::socket::shutdown_send, ec );

    m_pYieldContext = nullptr;
    resetMPOContext();
}

void MPOLogicalThread::startTCPStream()
{
    // start tcp session reader coroutine
    boost::asio::spawn(

        m_report.getIOContext(),

        [ this, timeout = m_report.getTimeoutSeconds() ]( boost::asio::yield_context yield_ctx )
        {
            boost::beast::error_code ec;

            // This buffer is required to persist across reads
            boost::beast::flat_buffer buffer;

            // This lambda is used to send messages
            for( ; m_bRunning; )
            {
                // Set the timeout.
                m_tcpStream.expires_after( std::chrono::seconds( timeout ) );

                // Read a request
                boost::beast::http::request< boost::beast::http::string_body > req;
                {
                    boost::beast::http::async_read( m_tcpStream, buffer, req, yield_ctx[ ec ] );
                }

                if( ec == boost::beast::http::error::end_of_stream )
                {
                    SPDLOG_TRACE( "MPOLogicalThread::startTCPStream End of stream" );
                    break;
                }
                else if( ec )
                {
                    SPDLOG_ERROR( "MPOLogicalThread::startTCPStream Error: {}", ec.message() );
                    break;
                }

                HTTPVerbType verbType = TOTAL_HTTP_VERBS;
                {
                    switch( req.method() )
                    {
                        case boost::beast::http::verb::get:
                            verbType = eGet;
                            break;
                        case boost::beast::http::verb::head:
                            verbType = eHead;
                            break;
                        case boost::beast::http::verb::post:
                            verbType = ePost;
                            break;
                        default:
                        {
                            THROW_RTE( "Unknown http verb" );
                        }
                    }
                }

                send( HTTPRequestMsg::make( getID(),
                                         HTTPRequestMsg{ mega::network::HTTPRequestData{
                                             verbType, req.version(), req.target(), req.keep_alive() } } ) );
            }

            // Send a TCP shutdown
            SPDLOG_TRACE( "MPOLogicalThread::startTCPStream shutdown" );
            send( HTTPRequestMsg::make( getID(), HTTPRequestMsg{ mega::network::HTTPRequestData{ eClose } } ) );
        }
    // segmented stacks do NOT work on windows
#ifndef BOOST_USE_SEGMENTED_STACKS
        ,
        boost::coroutines::attributes( network::NON_SEGMENTED_STACK_SIZE )
#endif
    );
}

boost::beast::http::message_generator MPOLogicalThread::handleHTTPRequest( const network::HTTPRequestData& httpRequest,
                                                                           boost::asio::yield_context&     yield_ctx )
{
    namespace beast = boost::beast;         // from <boost/beast.hpp>
    namespace http  = beast::http;          // from <boost/beast/http.hpp>
    namespace net   = boost::asio;          // from <boost/asio.hpp>
    using tcp       = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

    // Returns a bad request response
    auto const bad_request = [ &httpRequest ]( beast::string_view why )
    {
        http::response< http::string_body > res{ http::status::bad_request, httpRequest.version };
        res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
        res.set( http::field::content_type, "text/html" );
        res.keep_alive( httpRequest.keep_alive );
        res.body() = std::string( why );
        res.prepare_payload();
        return res;
    };

    // Returns a not found response
    auto const not_found = [ &httpRequest ]( beast::string_view target )
    {
        http::response< http::string_body > res{ http::status::not_found, httpRequest.version };
        res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
        res.set( http::field::content_type, "text/html" );
        res.keep_alive( httpRequest.keep_alive );
        res.body() = "The resource '" + std::string( target ) + "' was not found.";
        res.prepare_payload();
        return res;
    };

    // Returns a server error response
    auto const server_error = [ &httpRequest ]( beast::string_view what )
    {
        http::response< http::string_body > res{ http::status::internal_server_error, httpRequest.version };
        res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
        res.set( http::field::content_type, "text/html" );
        res.keep_alive( httpRequest.keep_alive );
        res.body() = "An error occurred: '" + std::string( what ) + "'";
        res.prepare_payload();
        return res;
    };

    // Make sure we can handle the method
    if( httpRequest.verb != eGet && httpRequest.verb != eHead )
    {
        return bad_request( "Unknown HTTP-method" );
    }

    http::string_body::value_type body;
    {
        using namespace mega::reports;
        const auto& megaInstall = m_report.getMegastructureInstallation();
        mega::reports::Renderer renderer( megaInstall.getRuntimeTemplateDir() );
        renderer.registerReporter( std::make_unique< MemoryReporter >( *m_pMemoryManager, *m_pDatabase ) );

        using ::           operator<<;
        std::ostringstream os;

        mega::reports::URL url;
        {
            std::ostringstream osURL;
            osURL << "http://0.0.0.0:8080/";
            url.reportID           = "memory";
            url.reporterLinkTarget = "memory";
            url.url                = osURL.str();
        }

        renderer.generate( url, os );

        body = os.str();
    }


        /*os << "<html><body><H1>";
        os << "Hello from MPOLogicalThread: " << getThisMPO() << "</h1><P>";
        {
            MPORealToLogicalVisitor mpoRealInstantiation( getThisRoot() );
            LogicalTreePrinter      printer( os );
            LogicalTreeTraversal    objectTraversal( mpoRealInstantiation, printer );
            traverse( objectTraversal );
        }
        os << "</P></body></html>";*/
        
    // Cache the size since we need it after the move
    auto const size = body.size();

    // Respond to HEAD request
    if( httpRequest.verb == eHead )
    {
        http::response< http::empty_body > res{ http::status::ok, httpRequest.version };
        res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
        res.set( http::field::content_type, "text/html" );
        res.content_length( size );
        res.keep_alive( httpRequest.keep_alive );
        return res;
    }
    else
    {
        // Respond to GET request
        http::response< http::string_body > res{ std::piecewise_construct, std::make_tuple( std::move( body ) ),
                                                 std::make_tuple( http::status::ok, httpRequest.version ) };
        res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
        res.set( http::field::content_type, "text/html" );
        res.content_length( size );
        res.keep_alive( httpRequest.keep_alive );
        return res;
    }
}

mega::network::HTTPRequestData MPOLogicalThread::HTTPRequest( boost::asio::yield_context& )
{
    SPDLOG_TRACE( "MPOLogicalThread::HTTPRequest" );
    THROW_RTE( "MPOLogicalThread::HTTPRequest" );
    return mega::network::HTTPRequestData{};
}

} // namespace mega::service::report