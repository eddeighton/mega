
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
#include "service/plugin/plugin.hpp"

#include "common/requireSemicolon.hpp"

#include <spdlog/async.h>

#include <thread>
#include <cstdlib>
#include <sstream>

namespace mega::service
{
namespace
{

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
                const char* pszCFG_TYPE = std::getenv( network::ENV_CFG_TYPE );
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
            m_pLogger = mega::network::configureLog( logFolder, "plugin", mega::network::fromStr( strConsoleLogLevel ),
                                                     mega::network::fromStr( strLogFileLevel ) );
            m_pThreadPool = spdlog::thread_pool();
        }

        m_pPlugin = std::make_shared< mega::service::Plugin >( m_ioContext, uiNumThreads );

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
    Plugin::Ptr                                      m_pPlugin;
};

static PluginWrapper::Ptr g_pPluginWrapper;

} // namespace

} // namespace mega::service

MEGA_PLUGIN_EXPORT void mp_initialise( const char* pszConsoleLogLevel, const char* pszFileLogLevel )
{
    try
    {
        mega::service::g_pPluginWrapper.reset();
        mega::service::g_pPluginWrapper
            = std::make_unique< mega::service::PluginWrapper >( pszConsoleLogLevel, pszFileLogLevel );
    }
    catch( std::exception& ex )
    {
        SPDLOG_ERROR( "Exception in mp_initialise msg: {}", ex.what() );
        mega::service::g_pPluginWrapper.reset();
    }
    catch( ... )
    {
        SPDLOG_ERROR( "Unknown Exception in mp_initialise" );
        mega::service::g_pPluginWrapper.reset();
    }
}

#define TRAP_EXCEPTIONS( code )                                                           \
    DO_STUFF_AND_REQUIRE_SEMI_COLON(                                                      \
        try {                                                                             \
            if( mega::service::g_pPluginWrapper.get() )                                   \
            {                                                                             \
                code;                                                                     \
            }                                                                             \
            else                                                                          \
            {                                                                             \
                SPDLOG_ERROR( "Plugin NOT INITIALISED in {}", BOOST_CURRENT_FUNCTION );   \
            }                                                                             \
        } catch( std::exception & ex ) {                                                  \
            SPDLOG_ERROR( "Exception in {} msg: {}", BOOST_CURRENT_FUNCTION, ex.what() ); \
        } catch( ... ) { SPDLOG_ERROR( "Unknown Exception in {}", BOOST_CURRENT_FUNCTION ); } )

MEGA_PLUGIN_EXPORT const void* mp_downstream()
{
    TRAP_EXCEPTIONS( return mega::service::g_pPluginWrapper->m_pPlugin->downstream() );
    return nullptr;
}

MEGA_PLUGIN_EXPORT mega::U64 mp_database_hashcode()
{
    TRAP_EXCEPTIONS( return mega::service::g_pPluginWrapper->m_pPlugin->database_hashcode() );
    return 0U;
}

MEGA_PLUGIN_EXPORT const char* mp_database()
{
    TRAP_EXCEPTIONS( return mega::service::g_pPluginWrapper->m_pPlugin->database() );
    return nullptr;
}

MEGA_PLUGIN_EXPORT void mp_upstream( float delta, void* pRange )
{
    TRAP_EXCEPTIONS( mega::service::g_pPluginWrapper->m_pPlugin->upstream( delta, pRange ) );
}

MEGA_PLUGIN_EXPORT void mp_shutdown()
{
    TRAP_EXCEPTIONS( mega::service::g_pPluginWrapper.reset() );
}

MEGA_PLUGIN_EXPORT mega::U64 mp_network_count()
{
    TRAP_EXCEPTIONS( return mega::service::g_pPluginWrapper->m_pPlugin->network_count() );
    return 0U;
}

MEGA_PLUGIN_EXPORT const char* mp_network_name( mega::U64 networkID )
{
    TRAP_EXCEPTIONS( return mega::service::g_pPluginWrapper->m_pPlugin->network_name( networkID ) );
    return nullptr;
}

MEGA_PLUGIN_EXPORT void mp_network_connect( mega::U64 networkID )
{
    TRAP_EXCEPTIONS( mega::service::g_pPluginWrapper->m_pPlugin->network_connect( networkID ) );
}

MEGA_PLUGIN_EXPORT void mp_network_disconnect()
{
    TRAP_EXCEPTIONS( mega::service::g_pPluginWrapper->m_pPlugin->network_disconnect() );
}

MEGA_PLUGIN_EXPORT mega::U64 mp_network_current()
{
    TRAP_EXCEPTIONS( return mega::service::g_pPluginWrapper->m_pPlugin->network_current() );
    return 0U;
}

MEGA_PLUGIN_EXPORT void mp_planet_create()
{
    TRAP_EXCEPTIONS( mega::service::g_pPluginWrapper->m_pPlugin->planet_create() );
}

MEGA_PLUGIN_EXPORT void mp_planet_destroy()
{
    TRAP_EXCEPTIONS( mega::service::g_pPluginWrapper->m_pPlugin->planet_destroy() );
}

MEGA_PLUGIN_EXPORT bool mp_planet_current()
{
    TRAP_EXCEPTIONS( return mega::service::g_pPluginWrapper->m_pPlugin->planet_current() );
    return false;
}
