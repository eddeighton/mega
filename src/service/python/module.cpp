
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

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include "python.hpp"
#include "mpo_conversation.hpp"

#include "service/network/network.hpp"
#include "service/network/log.hpp"

#include "service/protocol/common/conversation_base.hpp"

#include "common/assert_verify.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/config.hpp>

#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

namespace mega::service
{
namespace
{

class PythonModule : public network::ConversationBase
{
    using MessageChannel
        = boost::asio::experimental::concurrent_channel< void( boost::system::error_code, network::ReceivedMsg ) >;

    struct LogConfig
    {
        LogConfig( const char* pszConsoleLogLevel, const char* pszFileLogLevel )
        {
            boost::filesystem::path logFolder          = boost::filesystem::current_path() / "log";
            std::string             strConsoleLogLevel = "warn";
            std::string             strLogFileLevel    = "warn";
            if( pszConsoleLogLevel )
                strConsoleLogLevel = pszConsoleLogLevel;
            if( pszFileLogLevel )
                strLogFileLevel = pszFileLogLevel;
            mega::network::configureLog( logFolder, "python", mega::network::fromStr( strConsoleLogLevel ),
                                         mega::network::fromStr( strLogFileLevel ) );
        }
    };

public:
    using Ptr = std::shared_ptr< PythonModule >;

    // Sender
    virtual network::ConnectionID     getConnectionID() const { THROW_TODO; }
    virtual boost::system::error_code send( const network::Message& msg, boost::asio::yield_context& yield_ctx )
    {
        THROW_TODO;
    }
    virtual void sendErrorResponse( const network::ReceivedMsg& msg, const std::string& strErrorMsg,
                                    boost::asio::yield_context& yield_ctx )
    {
        THROW_TODO;
    }

    // network::ConversationBase
    virtual const ID& getID() const
    {
        static network::ConversationID pluginID{ 0, "PLUGIN" };
        return pluginID;
    }
    virtual void send( const network::ReceivedMsg& msg )
    {
        m_channel.async_send(
            boost::system::error_code(), msg,
            [ &msg ]( boost::system::error_code ec )
            {
                if( ec )
                {
                    SPDLOG_ERROR( "Failed to send request: {} with error: {}", msg.msg, ec.what() );
                    THROW_RTE( "Failed to send request on channel: " << msg.msg << " : " << ec.what() );
                }
            } );
    }
    virtual network::Message   dispatchRequestsUntilResponse( boost::asio::yield_context& yield_ctx ) { THROW_TODO; }
    virtual void               run( boost::asio::yield_context& yield_ctx ) { THROW_TODO; }
    virtual const std::string& getProcessName() const { THROW_TODO; }
    virtual U64                getStackSize() const { THROW_TODO; }
    virtual void               onDisconnect( const network::ConnectionID& connectionID ) { THROW_TODO; }
    virtual void               requestStarted( const network::ConnectionID& connectionID ) { ; }
    virtual void               requestCompleted() { ; }

    static Ptr makePlugin( short daemonPort, const char* pszConsoleLogLevel, const char* pszFileLogLevel )
    {
        auto pPlugin
            = std::make_shared< mega::service::PythonModule >( daemonPort, pszConsoleLogLevel, pszFileLogLevel );
        // work around shared_from_this in constructor issues
        pPlugin->m_python.externalConversationInitiated( pPlugin );
        return pPlugin;
    }

    static void releasePlugin( Ptr& pPlugin )
    {
        // work around shared_from_this in constructor issues
        pPlugin->shutdown();
        pPlugin->m_python.conversationCompleted( pPlugin );
        pPlugin.reset();
    }

    // PythonModule
    PythonModule( short daemonPort, const char* pszConsoleLogLevel, const char* pszFileLogLevel )
        : m_logConfig( pszConsoleLogLevel, pszFileLogLevel )
        , m_ioContext()
        , m_python( m_ioContext, daemonPort )
        , m_channel( m_ioContext )
    {
        {
            m_mpoConversation = std::make_shared< MPOConversation >(
                m_python, m_python.createConversationID( m_python.getLeafSender().getConnectionID() ) );
            m_python.conversationInitiated( m_mpoConversation, m_python.getLeafSender() );
        }

        SPDLOG_TRACE( "PythonModule::ctor" );
    }

    ~PythonModule() { SPDLOG_TRACE( "PythonModule::dtor" ); }

    PythonModule( const PythonModule& )            = delete;
    PythonModule( PythonModule&& )                 = delete;
    PythonModule& operator=( const PythonModule& ) = delete;
    PythonModule& operator=( PythonModule&& )      = delete;

    void shutdown() 
    { 
        SPDLOG_TRACE( "PythonModule::shutdown" ); 

        // send shutdown to MPOConversation
    }

    void run_one()
    {
        while( m_ioContext.poll_one() )
            ;
    }

private:
    LogConfig                      m_logConfig;
    boost::asio::io_context        m_ioContext;
    Python                         m_python;
    MessageChannel                 m_channel;
    network::ConversationBase::Ptr m_mpoConversation;
};

PythonModule::Ptr getModule()
{
    static PythonModule::Ptr g_pPythonModule;
    if( !g_pPythonModule )
    {
        g_pPythonModule = mega::service::PythonModule::makePlugin( mega::network::MegaDaemonPort(), "trace", "trace" );
    }
    return g_pPythonModule;
}

} // namespace

bool testFunction()
{
    return true;
}

void run_one()
{
    getModule()->run_one();
}

} // namespace mega::service

PYBIND11_MODULE( megastructure, pythonModule )
{
    pythonModule.doc() = "Python Module for Megastructure";

    pythonModule.def( "testFunction", &mega::service::testFunction, "Test Function" );
    pythonModule.def( "run_one", &mega::service::run_one, "Run the Megastructure Message Queue for one message" );
}
