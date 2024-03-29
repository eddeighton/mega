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

#include "log/log.hpp"

#include "common/processID.hpp"
#include "common/assert_verify.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/async.h"

#include <chrono>
#include <spdlog/common.h>
#include <sstream>

namespace mega::network
{
namespace
{
static std::vector< std::string > logLevels = { std::string{ "debug" }, std::string{ "trace" }, std::string{ "info" },
                                                std::string{ "warn" },  std::string{ "error" }, std::string{ "off" } };

struct StaticInit
{
    StaticInit()
    {
        spdlog::init_thread_pool( 8192, 1 ); // 1 backing thread.
    }
};
} // namespace

LoggingLevel fromStr( const std::string& str )
{
    auto iter = std::find( logLevels.begin(), logLevels.end(), str );
    VERIFY_RTE_MSG( iter != logLevels.end(), "Invalid log level" );
    return static_cast< LoggingLevel >( std::distance( logLevels.begin(), iter ) );
}

Log configureLog( Log::Config config )
{
    Log result{ config };

    // hacks!!
    static StaticInit init;

    auto consoleLevel = spdlog::level::warn;
    switch( result.config.consoleLoggingLevel )
    {
        case eDebug:
            consoleLevel = spdlog::level::debug;
            break;
        case eTrace:
            consoleLevel = spdlog::level::trace;
            break;
        case eInfo:
            consoleLevel = spdlog::level::info;
            break;
        case eWarn:
            consoleLevel = spdlog::level::warn;
            break;
        case eError:
            consoleLevel = spdlog::level::err;
            break;
        default:
        case eOff:
            consoleLevel = spdlog::level::off;
            break;
    }
    auto fileLevel = spdlog::level::warn;
    switch( result.config.fileLoggingLevel )
    {
        case eDebug:
            fileLevel = spdlog::level::debug;
            break;
        case eTrace:
            fileLevel = spdlog::level::trace;
            break;
        case eInfo:
            fileLevel = spdlog::level::info;
            break;
        case eWarn:
            fileLevel = spdlog::level::warn;
            break;
        case eError:
            fileLevel = spdlog::level::err;
            break;
        default:
        case eOff:
            fileLevel = spdlog::level::off;
            break;
    }
    const spdlog::level::level_enum sinkLevel = std::min( consoleLevel, fileLevel );

    spdlog::drop( result.config.strLogName );

    auto console_sink = std::make_shared< spdlog::sinks::stdout_color_sink_mt >();
    {
        console_sink->set_level( consoleLevel );
        console_sink->set_pattern( "%v" );
    }

    if( fileLevel == spdlog::level::off )
    {
        auto logger = std::shared_ptr< spdlog::async_logger >( new spdlog::async_logger(
            result.config.strLogName, { console_sink }, spdlog::thread_pool(), spdlog::async_overflow_policy::block ) );
        {
            logger->set_level( sinkLevel );
        }

        spdlog::flush_every( std::chrono::seconds( 1 ) );

        result.pLogger = logger;
    }
    else
    {
        boost::filesystem::path desiredLogFilePath;
        {
            std::ostringstream osLogFileName;
            osLogFileName << result.config.strLogName << "_" << common::ProcessID::get().getPID() << ".log";
            desiredLogFilePath = result.config.logFolder / osLogFileName.str();
        }

        auto file_sink = std::make_shared< spdlog::sinks::daily_file_sink_st >( desiredLogFilePath.string(), 23, 59 );
        {
            file_sink->set_level( fileLevel );
            file_sink->set_pattern( "%v" );
        }
        result.logFile = file_sink->filename();

        auto logger = std::shared_ptr< spdlog::async_logger >(
            new spdlog::async_logger( result.config.strLogName, { console_sink, file_sink }, spdlog::thread_pool(),
                                      spdlog::async_overflow_policy::block ) );
        {
            logger->set_level( sinkLevel );
        }

        spdlog::flush_every( std::chrono::seconds( 1 ) );

        result.pLogger = logger;
    }

    spdlog::set_default_logger( result.pLogger );

    return result;
}

} // namespace mega::network
