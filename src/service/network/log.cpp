
#include "service/network/log.hpp"

#include "common/processID.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/async.h"

#include <chrono>
#include <spdlog/common.h>
#include <sstream>

namespace mega
{
    namespace network
    {
        std::shared_ptr< spdlog::details::thread_pool > configureLog( const boost::filesystem::path& logFolderPath, const std::string& strLogName )
        {
            spdlog::drop( strLogName );
            
            auto console_sink = std::make_shared< spdlog::sinks::stdout_color_sink_mt >();
            {
                console_sink->set_level( spdlog::level::warn );
                console_sink->set_pattern( "%l [%^%l%$] %v");
            }
            
            std::ostringstream osLogFileName;
            osLogFileName << strLogName << "_" << Common::getProcessID() << ".log";
            
            const boost::filesystem::path logFilePath = logFolderPath / osLogFileName.str();
            
            auto file_sink = std::make_shared< spdlog::sinks::daily_file_sink_st >( logFilePath.string(), 23, 59 );
            {
                file_sink->set_level( spdlog::level::trace );
            }
            auto threadPool = std::make_shared< spdlog::details::thread_pool >( 8192, 1 );
            auto logger = std::shared_ptr< spdlog::async_logger >( 
                new spdlog::async_logger( strLogName, {console_sink, file_sink}, 
                    threadPool, spdlog::async_overflow_policy::block ) );
            {
                logger->set_level( spdlog::level::trace );
            }
            
            spdlog::flush_every( std::chrono::seconds( 1 ) );
            spdlog::set_default_logger( logger );
            
            return threadPool;
        }

    }
}
