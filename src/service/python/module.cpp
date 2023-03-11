
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

#include "service/protocol/model/status.hxx"
#include "service/protocol/model/enrole.hxx"
#include "service/protocol/model/python_leaf.hxx"

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

class PythonModule
{
    struct LogConfig
    {
        LogConfig( const char* pszConsoleLogLevel, const char* pszFileLogLevel )
        {
            boost::filesystem::path logFolder          = boost::filesystem::current_path() / "log";
            std::string             strConsoleLogLevel = "info";
            std::string             strLogFileLevel    = "info";
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

    static Ptr makePlugin( short daemonPort, const char* pszConsoleLogLevel, const char* pszFileLogLevel )
    {
        auto pPlugin
            = std::make_shared< mega::service::PythonModule >( daemonPort, pszConsoleLogLevel, pszFileLogLevel );
        // work around shared_from_this in constructor issues
        return pPlugin;
    }

    static void releasePlugin( Ptr& pPlugin )
    {
        // work around shared_from_this in constructor issues
        pPlugin->shutdown();
        pPlugin.reset();
    }

    // PythonModule
    PythonModule( short daemonPort, const char* pszConsoleLogLevel, const char* pszFileLogLevel )
        : m_logConfig( pszConsoleLogLevel, pszFileLogLevel )
        , m_python( m_ioContext, daemonPort )
    {
        {
            m_pExternalConversation = std::make_shared< network::ExternalConversation >(
                m_python, m_python.createConversationID( m_python.getLeafSender().getConnectionID() ), m_ioContext );
            m_python.externalConversationInitiated( m_pExternalConversation );
        }
        {
            m_mpoConversation = std::make_shared< MPOConversation >(
                m_python, m_python.createConversationID( m_python.getLeafSender().getConnectionID() ) );
            m_python.conversationInitiated( m_mpoConversation, m_python.getLeafSender() );
        }

        SPDLOG_TRACE( "PythonModule::ctor" );
    }

    ~PythonModule()
    {
        SPDLOG_TRACE( "PythonModule::dtor" );
        m_python.conversationCompleted( m_pExternalConversation );
    }

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

    template < typename Request >
    Request request()
    {
        return Request(
            [ mpoCon = m_mpoConversation,
              extCon = m_pExternalConversation ]( const network::Message& msg ) mutable -> network::Message
            {
                network::python_leaf::External_Request_Sender sender( *mpoCon, *extCon );
                return sender.PythonRoot( msg );
            },
            m_pExternalConversation->getID() );
    }

    void getStatus()
    {
        auto               result = request< network::status::Request_Encoder >().GetNetworkStatus();
        std::ostringstream os;
        os << result;
        SPDLOG_INFO( os.str() );
    }

    void getDaemons()
    {
        auto result = request< network::enrole::Request_Encoder >().EnroleGetDaemons();
        for( mega::MachineID machineID : result )
        {
            SPDLOG_INFO( "daemon: {}", machineID );
        }
    }

    void getMPs()
    {
        auto daemons = request< network::enrole::Request_Encoder >().EnroleGetDaemons();
        for( mega::MachineID machineID : daemons )
        {
            auto result = request< network::enrole::Request_Encoder >().EnroleGetProcesses( machineID );
            for( mega::MP machineProcess : result )
            {
                SPDLOG_INFO( "mp: {}", machineProcess );
            }
        }
    }

    void getMPOs()
    {
        auto daemons = request< network::enrole::Request_Encoder >().EnroleGetDaemons();
        for( mega::MachineID machineID : daemons )
        {
            auto mps = request< network::enrole::Request_Encoder >().EnroleGetProcesses( machineID );
            for( mega::MP machineProcess : mps )
            {
                auto result = request< network::enrole::Request_Encoder >().EnroleGetMPO( machineProcess );
                for( mega::MPO mpo : result )
                {
                    SPDLOG_INFO( "mpo: {}", mpo );
                }
            }
        }
    }

private:
    LogConfig                          m_logConfig;
    boost::asio::io_context            m_ioContext;
    Python                             m_python;
    network::ExternalConversation::Ptr m_pExternalConversation;
    network::ConversationBase::Ptr     m_mpoConversation;
};

PythonModule::Ptr getModule()
{
    static PythonModule::Ptr g_pPythonModule;
    if( !g_pPythonModule )
    {
        g_pPythonModule = mega::service::PythonModule::makePlugin( mega::network::MegaDaemonPort(), "info", "trace" );
    }
    return g_pPythonModule;
}

} // namespace

} // namespace mega::service

PYBIND11_MODULE( megastructure, pythonModule )
{
    pythonModule.doc() = "Python Module for Megastructure";

    pythonModule.def(
        "getStatus", [] { mega::service::getModule()->getStatus(); }, "Get Megastructure Status" );
    pythonModule.def(
        "getDaemons", [] { mega::service::getModule()->getDaemons(); }, "List Daemons" );
    pythonModule.def(
        "getMPs", [] { mega::service::getModule()->getMPs(); }, "List Machine Processes" );
    pythonModule.def(
        "getMPOs", [] { mega::service::getModule()->getMPOs(); }, "List Machine Process Owners" );
    pythonModule.def(
        "run_one", [] { mega::service::getModule()->run_one(); },
        "Run the Megastructure Message Queue for one message" );
}
