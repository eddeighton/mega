
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

#include <thread>

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
    {
        {
            boost::filesystem::path logFolder          = boost::filesystem::current_path() / "log";
            std::string             strConsoleLogLevel = "warn";
            std::string             strLogFileLevel    = "warn";
            if( pszConsoleLogLevel )
                strConsoleLogLevel = pszConsoleLogLevel;
            if( pszFileLogLevel )
                strLogFileLevel = pszFileLogLevel;
            mega::network::configureLog( logFolder, "plugin", mega::network::fromStr( strConsoleLogLevel ),
                                         mega::network::fromStr( strLogFileLevel ) );
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

        m_ioContext.stop();

        for( std::thread& thread : m_threads )
        {
            thread.join();
        }
    }

    boost::asio::io_context    m_ioContext;
    std::vector< std::thread > m_threads;
    Plugin::Ptr                m_pPlugin;
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

mega::I64 mp_database_hashcode()
{
    return mega::service::g_pPluginWrapper->m_pPlugin->database_hashcode();
}

const char* mp_database()
{
    return mega::service::g_pPluginWrapper->m_pPlugin->database();
}

MEGA_64 mp_memory_state()
{
    return mega::service::g_pPluginWrapper->m_pPlugin->memory_state();
}

MEGA_64 mp_memory_size()
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

mega::I64 mp_network_count()
{
    return mega::service::g_pPluginWrapper->m_pPlugin->network_count();
}

const char* mp_network_name( mega::I64 networkID )
{
    return mega::service::g_pPluginWrapper->m_pPlugin->network_name( networkID );
}

void mp_network_connect( mega::I64 networkID )
{
    mega::service::g_pPluginWrapper->m_pPlugin->network_connect( networkID );
}

void mp_network_disconnect()
{
    mega::service::g_pPluginWrapper->m_pPlugin->network_disconnect();
}

mega::I64 mp_network_current()
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