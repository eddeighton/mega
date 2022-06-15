
#include "service/network/log.hpp"

#include "common/processID.hpp"
#include "common/assert_verify.hpp"

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
        static std::vector< std::string > logLevels = 
        {
            std::string{ "debug" },
            std::string{ "trace" },
            std::string{ "info" },
            std::string{ "warn" },
            std::string{ "error" },
            std::string{ "off" }
        };
        LoggingLevel fromStr( const std::string& str )
        {
            auto iter =
                std::find( logLevels.begin(), logLevels.end(), str );
            VERIFY_RTE_MSG( iter != logLevels.end(), "Invalid log level" );
            return static_cast< LoggingLevel >( std::distance( logLevels.begin(), iter ) );
        }

        std::shared_ptr< spdlog::details::thread_pool > configureLog( const boost::filesystem::path& logFolderPath, const std::string& strLogName,
                                                            LoggingLevel consoleLoggingLevel, LoggingLevel fileLoggingLevel  )
        {
            auto consoleLevel = spdlog::level::warn;
            switch( consoleLoggingLevel )
            {
                case eDebug:   consoleLevel = spdlog::level::debug; break;
                case eTrace:   consoleLevel = spdlog::level::trace; break;
                case eInfo:    consoleLevel = spdlog::level::info; break;
                case eWarn:    consoleLevel = spdlog::level::warn; break;
                case eError:   consoleLevel = spdlog::level::err; break;
                case eOff:     consoleLevel = spdlog::level::off; break;
            }
            auto fileLevel = spdlog::level::warn;
            switch( fileLoggingLevel )
            {
                case eDebug:   fileLevel = spdlog::level::debug; break;
                case eTrace:   fileLevel = spdlog::level::trace; break;
                case eInfo:    fileLevel = spdlog::level::info; break;
                case eWarn:    fileLevel = spdlog::level::warn; break;
                case eError:   fileLevel = spdlog::level::err; break;
                case eOff:     fileLevel = spdlog::level::off; break;
            }
            const spdlog::level::level_enum sinkLevel = std::min( consoleLevel, fileLevel );
            
            spdlog::drop( strLogName );
            
            auto console_sink = std::make_shared< spdlog::sinks::stdout_color_sink_mt >();
            {
                console_sink->set_level( consoleLevel );
                console_sink->set_pattern( "%v");
            }
            
            std::ostringstream osLogFileName;
            osLogFileName << strLogName << "_" << Common::getProcessID() << ".log";
            
            const boost::filesystem::path logFilePath = logFolderPath / osLogFileName.str();
            
            auto file_sink = std::make_shared< spdlog::sinks::daily_file_sink_st >( logFilePath.string(), 23, 59 );
            {
                file_sink->set_level( fileLevel );
                file_sink->set_pattern( "%v");
            }
            auto threadPool = std::make_shared< spdlog::details::thread_pool >( 8192, 1 );
            auto logger = std::shared_ptr< spdlog::async_logger >( 
                new spdlog::async_logger( strLogName, {console_sink, file_sink}, 
                    threadPool, spdlog::async_overflow_policy::block ) );
            {
                logger->set_level( sinkLevel );
            }
            
            spdlog::flush_every( std::chrono::seconds( 1 ) );
            spdlog::set_default_logger( logger );
            
            return threadPool;
        }

    }
}
