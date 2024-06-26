
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

#include "http_logical_thread.hpp"

#include "environment/environment.hpp"

#include "database_reporters/factory.hpp"

#include "mega/iterator.hpp"
#include "mega/logical_tree.hpp"
#include "mega/printer.hpp"

#include "service/mpo_visitor.hpp"
#include "service/reporters.hpp"

#include "service/protocol/model/jit.hxx"
#include "service/protocol/model/host.hxx"

#include "service/protocol/common/type_erase.hpp"
#include "service/protocol/common/sender_ref.hpp"

#include "spdlog/stopwatch.h"

#include <boost/beast/version.hpp>

namespace mega::service::report
{
HTTPLogicalThread::HTTPLogicalThread( ReportServer&                   reportServer,
                                      const network::LogicalThreadID& logicalthreadID,
                                      boost::asio::ip::tcp::socket&   socket )
    : ReportRequestLogicalThread( reportServer, logicalthreadID )
    , runtime::MPOContext( getID() )
    , m_reportServer( reportServer )
    , m_tcpStream( std::move( socket ) )
{
    m_bEnableQueueing = true;
}

network::Message HTTPLogicalThread::dispatchInBoundRequest( const network::Message&     msg,
                                                            boost::asio::yield_context& yield_ctx )
{
    network::Message result;
    if( result = network::report::Impl::dispatchInBoundRequest( msg, yield_ctx ); result )
        return result;
    return ReportRequestLogicalThread::dispatchInBoundRequest( msg, yield_ctx );
}

network::report_leaf::Request_Sender HTTPLogicalThread::getReportRequest( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_reportServer.getLeafSender(), yield_ctx };
}

network::mpo::Request_Sender HTTPLogicalThread::getMPRequest( boost::asio::yield_context& yield_ctx )
{
    return { *this, m_reportServer.getLeafSender(), yield_ctx };
}

network::enrole::Request_Encoder HTTPLogicalThread::getRootEnroleRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { [ leafRequest = getReportRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
             { return leafRequest.ReportRoot( msg ); },
             getID() };
}

network::stash::Request_Encoder HTTPLogicalThread::getRootStashRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { [ leafRequest = getReportRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
             { return leafRequest.ReportRoot( msg ); },
             getID() };
}

network::memory::Request_Encoder HTTPLogicalThread::getDaemonMemoryRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { [ leafRequest = getReportRequest( *m_pYieldContext ) ]( const network::Message& msg ) mutable
             { return leafRequest.ReportDaemon( msg ); },
             getID() };
}

network::sim::Request_Encoder HTTPLogicalThread::getMPOSimRequest( runtime::MPO mpo )
{
    VERIFY_RTE( m_pYieldContext );
    return { [ leafRequest = getMPRequest( *m_pYieldContext ), mpo ]( const network::Message& msg ) mutable
             { return leafRequest.MPOUp( msg, mpo ); },
             getID() };
}

network::memory::Request_Sender HTTPLogicalThread::getLeafMemoryRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { *this, m_reportServer.getLeafSender(), *m_pYieldContext };
}

network::jit::Request_Sender HTTPLogicalThread::getLeafJITRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return { *this, m_reportServer.getLeafSender(), *m_pYieldContext };
}

network::mpo::Request_Sender HTTPLogicalThread::getMPRequest()
{
    VERIFY_RTE( m_pYieldContext );
    return getMPRequest( *m_pYieldContext );
}

void HTTPLogicalThread::run( boost::asio::yield_context& yield_ctx )
{
    SPDLOG_TRACE( "REPORT HTTPLogicalThread: run" );
    network::sim::Request_Encoder request(
        [ rootRequest = getMPRequest( yield_ctx ) ]( const network::Message& msg ) mutable
        { return rootRequest.MPRoot( msg, mega::runtime::MP{} ); },
        getID() );
    request.SimStart();

    dispatchRemaining( yield_ctx );

    m_bRunComplete = true;
}

bool HTTPLogicalThread::queue( const network::ReceivedMessage& msg )
{
    // if processing a report request then postpone further http requests
    if( msg.msg.getID() == HTTPRequestMsg::ID )
    {
        const network::HTTPRequestData& httpRequest = HTTPRequestMsg::get( msg.msg ).httpRequestData;
        if( m_queueStack != 0 )
        {
            SPDLOG_TRACE( "HTTPLogicalThread::queue queuing HTTPRequestMsg verb: {} request: {}",
                          verbToString( static_cast< HTTPVerbType >( httpRequest.verb ) ), httpRequest.request );
            m_messageQueue.push_back( msg );
            return true;
        }
        else
        {
            SPDLOG_TRACE( "HTTPLogicalThread::queue NOT queuing HTTPRequestMsg verb: {} request: {}",
                          verbToString( static_cast< HTTPVerbType >( httpRequest.verb ) ), httpRequest.request );
        }
    }
    return ReportRequestLogicalThread::queue( msg );
}

void HTTPLogicalThread::unqueue()
{
    // at startup will queue the SimCreate request which should be
    // acknowledged once simulation has completed startup
    if( !m_messageQueue.empty() )
    {
        SPDLOG_TRACE( "HTTPLogicalThread::unqueue" );
        VERIFY_RTE( m_unqueuedMessages.empty() );
        m_unqueuedMessages.swap( m_messageQueue );
        std::reverse( m_unqueuedMessages.begin(), m_unqueuedMessages.end() );
    }
    else
    {
        // allow ordinary request processing
        ReportRequestLogicalThread::unqueue();
    }
}

void HTTPLogicalThread::spawnTCPStream()
{
    // start tcp session reader coroutine
    boost::asio::spawn(

        m_reportServer.getIOContext(),

        [ this, timeout = m_reportServer.getTimeoutSeconds() ]( boost::asio::yield_context yield_ctx )
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
                    SPDLOG_TRACE( "HTTPLogicalThread::spawnTCPStream End of stream" );
                    break;
                }
                else if( ec )
                {
                    SPDLOG_ERROR( "HTTPLogicalThread::spawnTCPStream complete: {}", ec.message() );
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

                SPDLOG_TRACE( "HTTPLogicalThread::spawnTCPStream sending {}", verbToString( verbType ) );
                send( HTTPRequestMsg::make( getID(),
                                            HTTPRequestMsg{ mega::network::HTTPRequestData{
                                                verbType, req.version(), req.target(), req.keep_alive() } } ) );
            }

            // Send a TCP shutdown
            SPDLOG_TRACE( "HTTPLogicalThread::spawnTCPStream sending eClose" );
            send( HTTPRequestMsg::make( getID(), HTTPRequestMsg{ mega::network::HTTPRequestData{ eClose } } ) );
        }
    // segmented stacks do NOT work on windows
#ifndef BOOST_USE_SEGMENTED_STACKS
        ,
        boost::coroutines::attributes( network::NON_SEGMENTED_STACK_SIZE )
#endif
    );
}

void HTTPLogicalThread::RootSimRun( const mega::runtime::MPO& mpo, boost::asio::yield_context& yield_ctx )
{
    m_mpo = mpo;

    runtime::setMPOContext( this );
    m_pYieldContext = &yield_ctx;

    boost::beast::error_code ec;

    SPDLOG_TRACE( "REPORT RootSimRun: Acquired mpo context: {}", mpo );
    {
        createRoot( mpo );

        for( const auto& msg : m_messageQueue )
        {
            SPDLOG_ERROR( "Unexpected pending message when starting up HTTPLogicalThread: {}", msg.msg );
            THROW_RTE( "Unexpected pending message when starting up HTTPLogicalThread: " << msg.msg );
        }

        m_bRunning = true;

        spawnTCPStream();

        while( m_bRunning )
        {
            ASSERT( m_queueStack == 0 );
            unqueue();
            const network::ReceivedMessage msg = receiveDeferred( yield_ctx );

            switch( msg.msg.getID() )
            {
                case HTTPRequestMsg::ID:
                {
                    const network::HTTPRequestData& httpRequest = HTTPRequestMsg::get( msg.msg ).httpRequestData;
                    switch( httpRequest.verb )
                    {
                        case eClose:
                        case eError:
                        {
                            m_bRunning = false;
                            SPDLOG_TRACE( "HTTPLogicalThread::RootSimRun shutting down" );
                        }
                        break;
                        case eGet:
                        case eHead:
                        case ePost:
                        {
                            // generate response
                            if( httpRequest.version != 0 )
                            {
                                SPDLOG_TRACE( "HTTPLogicalThread::RootSimRun HTTP Request {}", httpRequest.request );

                                // Handle the request
                                boost::beast::http::message_generator httpMsg
                                    = handleHTTPRequest( httpRequest, yield_ctx );

                                // Determine if we should close the connection
                                m_bRunning = httpMsg.keep_alive();

                                // Send the response
                                {
                                    mega::runtime::_MPOContextStack _mpoStack;
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
                    QueueStackDepth queueMsgs( m_queueStack );
                    acknowledgeInboundRequest( msg, yield_ctx );
                }
                break;
            }
        }
    }
    SPDLOG_TRACE( "REPORT RootSimRun: Releasing mpo context: {}", mpo );

    m_tcpStream.socket().shutdown( boost::asio::ip::tcp::socket::shutdown_send, ec );

    m_pYieldContext = nullptr;
    runtime::resetMPOContext();
}

boost::beast::http::message_generator HTTPLogicalThread::handleHTTPRequest( const network::HTTPRequestData& httpRequest,
                                                                            boost::asio::yield_context&     yield_ctx )
{
    namespace beast = boost::beast;
    namespace http  = beast::http;
    namespace net   = boost::asio;

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
    /*auto const not_found = [ &httpRequest ]( beast::string_view target )
    {
        http::response< http::string_body > res{ http::status::not_found, httpRequest.version };
        res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
        res.set( http::field::content_type, "text/html" );
        res.keep_alive( httpRequest.keep_alive );
        res.body() = "The resource '" + std::string( target ) + "' was not found.";
        res.prepare_payload();
        return res;
    };*/

    // Returns a server error response
    /*auto const server_error = [ &httpRequest ]( beast::string_view what )
    {
        http::response< http::string_body > res{ http::status::internal_server_error, httpRequest.version };
        res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
        res.set( http::field::content_type, "text/html" );
        res.keep_alive( httpRequest.keep_alive );
        res.body() = "An error occurred: '" + std::string( what ) + "'";
        res.prepare_payload();
        return res;
    };*/

    // Make sure we can handle the method
    if( httpRequest.verb != eGet && httpRequest.verb != eHead )
    {
        return bad_request( "Unknown HTTP-method" );
    }

    URL url;
    {
        const auto httpEndpoint = m_reportServer.getHTTPEndPoint();

        url = ::report::fromString( httpRequest.request );
        url.set_encoded_host_address( httpEndpoint.address().to_string() );
        url.set_port_number( httpEndpoint.port() );
    }

    spdlog::stopwatch             sw;
    http::string_body::value_type body = generateHTTPResponse( url, yield_ctx );
    SPDLOG_INFO( "HTTP Request: {} took time: {}", url.c_str(),
                 std::chrono::duration_cast< mega::network::LogTime >( sw.elapsed() ) );

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

boost::beast::http::string_body::value_type
HTTPLogicalThread::generateHTTPResponse( const URL& url, boost::asio::yield_context& yield_ctx )
{
    std::optional< Report > reportContainerOpt;

    // either service request or database
    if( mega::reporters::isCompilationReportType( url ) )
    {
        QueueStackDepth queueMsgs( m_queueStack );
        {
            network::host::Request_Sender leafHostRequest{ *this, m_reportServer.getLeafSender(), yield_ctx };
            const auto                    program     = leafHostRequest.GetProgram();
            const auto                    programPath = Environment::prog( program );

            if( boost::filesystem::exists( programPath ) )
            {
                const ProgramManifest programManifest = Environment::load( program );
                const auto            databaseArchive = programManifest.getDatabase();

                VERIFY_RTE_MSG( boost::filesystem::exists( databaseArchive ),
                                "Failed to locate program database at: " << databaseArchive.string() );

                mega::io::ArchiveEnvironment environment( databaseArchive );
                mega::io::Manifest           manifest( environment, environment.project_manifest() );

                reportContainerOpt = mega::reporters::generateCompilationReport(
                    url, mega::reporters::CompilationReportArgs{ manifest, environment } );
            }
            else
            {
                SPDLOG_ERROR( "Cannot generated report: {} when no active program", url.c_str() );
            }
        }
    }
    else
    {
        // is the file set ?
        if( auto fileOpt = ::report::getFile( url ) )
        {
            reportContainerOpt = mega::reports::getFileReport( url );
        }
        else
        {
            QueueStackDepth queueMsgs( m_queueStack );
            {
                auto reportRequest = getRootRequest< network::report::Request_Encoder >( yield_ctx );
                reportContainerOpt = reportRequest.GetNetworkReport( url );
            }
        }
    }

    std::ostringstream os;
    if( reportContainerOpt.has_value() )
    {
        /*struct Linker : public mega::reports::Linker
        {
            const URL& m_url;
            Linker( const URL& url )
                : m_url( url )
            {
            }
            std::optional< URL > link( const mega::reports::Value& value ) const override
            {
                if( auto pTypeID = std::get_if< mega::interface::TypeID >( &value ) )
                {
                    URL url = m_url;
                    url.set_fragment( mega::reports::toString( value ) );
                    return url;
                }
                else if( auto pTypeID = std::get_if< mega::concrete::TypeID >( &value ) )
                {
                    URL url = m_url;
                    url.set_fragment( mega::reports::toString( value ) );
                    return url;
                }
                else if( auto pMPO = std::get_if< mega::runtime::MPO >( &value ) )
                {
                    URL url = m_url;
                    url.set_fragment( mega::reports::toString( value ) );
                    return url;
                }
                return {};
            }
        } linker( url );*/

        if( !m_pHTMLTemplateEngine )
        {
            m_pHTMLTemplateEngine = std::make_unique< ::report::HTMLTemplateEngine >( true );
        }

        ::report::renderHTML( reportContainerOpt.value(), os, *m_pHTMLTemplateEngine );
    }
    return os.str();
}
/*
reports::HTMLRenderer::JavascriptShortcuts HTTPLogicalThread::getJavascriptShortcuts() const
{
    reports::HTMLRenderer::JavascriptShortcuts shortcuts;
    {
        std::vector< report::ReporterID > reporterIDs;
        mega::reporters::getDatabaseReporterIDs( reporterIDs );
        mega::reports::getServiceReporters( reporterIDs );

        std::set< char > used;
        for( const auto& reportID : reporterIDs )
        {
            char key = '<';
            for( char c : reportID )
            {
                if( std::isalpha( c ) )
                {
                    if( !used.contains( c ) )
                    {
                        key = c;
                        break;
                    }
                    else if( !used.contains( std::toupper( c ) ) )
                    {
                        key = std::toupper( c );
                        break;
                    }
                    else if( !used.contains( std::tolower( c ) ) )
                    {
                        key = std::tolower( c );
                        break;
                    }
                }
            }

            VERIFY_RTE_MSG( key != '<', "Could not find shortcut key for reportID: " << reportID );
            shortcuts.add( { reportID, key } );
            used.insert( key );
        }
    }
    return shortcuts;
}
*/
mega::network::HTTPRequestData HTTPLogicalThread::HTTPRequest( boost::asio::yield_context& )
{
    SPDLOG_TRACE( "HTTPLogicalThread::HTTPRequest" );
    THROW_RTE( "HTTPLogicalThread::HTTPRequest" );
    return mega::network::HTTPRequestData{};
}

} // namespace mega::service::report