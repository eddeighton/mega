
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

#include "service/plugin/api.hpp"

#include "service/executor/executor.hpp"

#include "mega/native_types.hpp"

#include "service/network/log.hpp"

#include <spdlog/async.h>

#include <boost/asio/io_context.hpp>
#include <boost/filesystem.hpp>

#include <thread>
#include <cstdlib>
#include <sstream>
#include <memory>

namespace mega::service
{
namespace
{

class FakePlugin
{
    mega::service::Executor m_executor;

public:
    using Ptr = std::shared_ptr< FakePlugin >;

    FakePlugin( boost::asio::io_context& ioContext, U64 uiNumThreads )
        : m_executor( ioContext, uiNumThreads, mega::network::MegaDaemonPort() )
    {
    }

    FakePlugin( const FakePlugin& )            = delete;
    FakePlugin( FakePlugin&& )                 = delete;
    FakePlugin& operator=( const FakePlugin& ) = delete;
    FakePlugin& operator=( FakePlugin&& )      = delete;

    mega::U64   database_hashcode() { return 0; }
    const char* database() { return nullptr; }

    mega::U64   memory_state() { return 0; }
    mega::U64   memory_size() { return 0; }
    const void* memory_data() { return nullptr; }

    const log::Range* downstream() { return nullptr; }

    void upstream( float delta, void* pRange )
    {
        //
        boost::filesystem::path f( "/build/linux_gcc_shared_debug/game/install/bin/unityDatabase.json" );
        auto                    h = task::FileHash( f );
        SPDLOG_TRACE( "Got unityDatabase.json hashcode of: {}", h.get() );
    }

    U64         network_count() { return 0; }
    const char* network_name( U64 networkID ) { return nullptr; }
    void        network_connect( U64 networkID ) {}
    void        network_disconnect() {}
    U64         network_current() { return 0; }

    void planet_create() {}
    void planet_destroy() {}
    bool planet_current() { return false; }
};

class PluginWrapper
{
public:
    using Ptr = std::unique_ptr< PluginWrapper >;

    PluginWrapper( const char* pszConsoleLogLevel, const char* pszFileLogLevel,
                   U64 uiNumThreads = std::thread::hardware_concurrency() )
        : m_work_guard( m_ioContext.get_executor() )
    {
        {
            boost::filesystem::path logFolder;
            {
                const char* pszCFG_TYPE = std::getenv( "CFG_TYPE" );
                if( pszCFG_TYPE != nullptr )
                {
                    std::ostringstream os;
                    os << "/home/foobar/test_" << pszCFG_TYPE << "/log";
                    logFolder = os.str();
                }
                else
                {
                    logFolder = boost::filesystem::current_path() / "log";
                }
            }

            std::string strConsoleLogLevel = "warn";
            std::string strLogFileLevel    = "warn";
            if( pszConsoleLogLevel )
                strConsoleLogLevel = pszConsoleLogLevel;
            if( pszFileLogLevel )
                strLogFileLevel = pszFileLogLevel;
            m_pLogger
                = mega::network::configureLog( logFolder, "plugin_fake", mega::network::fromStr( strConsoleLogLevel ),
                                               mega::network::fromStr( strLogFileLevel ) );
            m_pThreadPool = spdlog::thread_pool();
        }

        m_pPlugin = std::make_shared< FakePlugin >( m_ioContext, uiNumThreads );

        for( int i = 0; i < uiNumThreads; ++i )
        {
            m_threads.emplace_back( std::move( std::thread( [ &ioContext = m_ioContext ]() { ioContext.run(); } ) ) );
        }
    }
    ~PluginWrapper()
    {
        m_pPlugin.reset();
        m_work_guard.reset();
        m_ioContext.stop();
        for( std::thread& thread : m_threads )
        {
            thread.join();
        }
    }

    std::shared_ptr< spdlog::logger >               m_pLogger;
    std::shared_ptr< spdlog::details::thread_pool > m_pThreadPool;
    boost::asio::io_context                         m_ioContext;
    using ExecutorType = decltype( m_ioContext.get_executor() );
    boost::asio::executor_work_guard< ExecutorType > m_work_guard;
    std::vector< std::thread >                       m_threads;
    FakePlugin::Ptr                                  m_pPlugin;
};

static PluginWrapper::Ptr g_pPluginWrapper;

} // namespace

} // namespace mega::service

void mp_initialise( const char* pszConsoleLogLevel, const char* pszFileLogLevel )
{
    mega::service::g_pPluginWrapper.reset();
    mega::service::g_pPluginWrapper
        = std::make_unique< mega::service::PluginWrapper >( pszConsoleLogLevel, pszFileLogLevel );
}

const void* mp_downstream()
{
    return mega::service::g_pPluginWrapper->m_pPlugin->downstream();
}

mega::U64 mp_database_hashcode()
{
    return mega::service::g_pPluginWrapper->m_pPlugin->database_hashcode();
}

const char* mp_database()
{
    return mega::service::g_pPluginWrapper->m_pPlugin->database();
}

mega::U64 mp_memory_state()
{
    return mega::service::g_pPluginWrapper->m_pPlugin->memory_state();
}

mega::U64 mp_memory_size()
{
    return mega::service::g_pPluginWrapper->m_pPlugin->memory_size();
}

const void* mp_memory_data()
{
    return mega::service::g_pPluginWrapper->m_pPlugin->memory_data();
}

void mp_upstream( float delta, void* pRange )
{
    mega::service::g_pPluginWrapper->m_pPlugin->upstream( delta, pRange );
}

void mp_shutdown()
{
    mega::service::g_pPluginWrapper.reset();
}

mega::U64 mp_network_count()
{
    return mega::service::g_pPluginWrapper->m_pPlugin->network_count();
}

const char* mp_network_name( mega::U64 networkID )
{
    return mega::service::g_pPluginWrapper->m_pPlugin->network_name( networkID );
}

void mp_network_connect( mega::U64 networkID )
{
    mega::service::g_pPluginWrapper->m_pPlugin->network_connect( networkID );
}

void mp_network_disconnect()
{
    mega::service::g_pPluginWrapper->m_pPlugin->network_disconnect();
}

mega::U64 mp_network_current()
{
    return mega::service::g_pPluginWrapper->m_pPlugin->network_current();
}

void mp_planet_create()
{
    mega::service::g_pPluginWrapper->m_pPlugin->planet_create();
}

void mp_planet_destroy()
{
    mega::service::g_pPluginWrapper->m_pPlugin->planet_destroy();
}

bool mp_planet_current()
{
    return mega::service::g_pPluginWrapper->m_pPlugin->planet_current();
}