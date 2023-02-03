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

#include "service/terminal.hpp"

#include "log/log.hpp"

#include "service/network/log.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <vector>
#include <string>
#include <iostream>

namespace driver
{
namespace log
{

void command( bool bHelp, const std::vector< std::string >& args )
{
    boost::filesystem::path logFolderPath;

    bool bShowLogRecords        = true;
    bool bShowMemoryRecords     = false;
    bool bShowSchedulingRecords = false;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Simulation Commands" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "folder",     po::value( &logFolderPath ),                                "Log folder path. ( Default argumnet )" )
            
            ( "log",        po::bool_switch( &bShowLogRecords )->default_value(true),   "Show log records." )
            ( "memory",     po::bool_switch( &bShowMemoryRecords ),                     "Show memory records." )
            ( "schedule",   po::bool_switch( &bShowSchedulingRecords ),                 "Show scheduling records." )
            ;
        // clang-format on
    }

    po::positional_options_description p;
    p.add( "folder", -1 );

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).positional( p ).run(), vm );
    po::notify( vm );

    {
        if( bShowMemoryRecords )
            bShowLogRecords = false;
        if( bShowSchedulingRecords )
            bShowLogRecords = false;
    }

    if( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        if( !logFolderPath.empty() )
        {
            VERIFY_RTE_MSG(
                boost::filesystem::exists( logFolderPath ), "Failed to locate folder: " << logFolderPath.string() );
            mega::log::Storage log( logFolderPath, true );
            SPDLOG_INFO( "Loaded log folder: {}", logFolderPath.string() );
/*
            if( bShowLogRecords )
            {
                for( auto i = log.logBegin(), iEnd = log.logEnd(); i != iEnd; ++i )
                {
                    const auto& logMsg = *i;
                    switch( logMsg.getType() )
                    {
                        case mega::log::LogMsg::eTrace:
                            SPDLOG_LOGGER_CALL( spdlog::default_logger_raw(), spdlog::level::trace, logMsg.getMsg() );
                            break;
                        case mega::log::LogMsg::eDebug:
                            SPDLOG_LOGGER_CALL( spdlog::default_logger_raw(), spdlog::level::debug, logMsg.getMsg() );
                            break;
                        case mega::log::LogMsg::eInfo:
                            SPDLOG_LOGGER_CALL( spdlog::default_logger_raw(), spdlog::level::info, logMsg.getMsg() );
                            break;
                        case mega::log::LogMsg::eWarn:
                            SPDLOG_LOGGER_CALL( spdlog::default_logger_raw(), spdlog::level::warn, logMsg.getMsg() );
                            break;
                        case mega::log::LogMsg::eError:
                            SPDLOG_LOGGER_CALL( spdlog::default_logger_raw(), spdlog::level::err, logMsg.getMsg() );
                            break;
                        case mega::log::LogMsg::eFatal:
                            SPDLOG_LOGGER_CALL( spdlog::default_logger_raw(), spdlog::level::err, logMsg.getMsg() );
                            break;
                    }
                }
            }
            if( bShowMemoryRecords )
            {
                for( auto iTrack = 0; iTrack != mega::log::toInt( mega::log::TrackType::TOTAL ); ++iTrack )
                {
                    if( iTrack == mega::log::toInt( mega::log::TrackType::Log )
                        || iTrack == mega::log::toInt( mega::log::TrackType::Scheduler ) )
                        continue;

                    const auto trackType = static_cast< mega::log::MemoryTrackType >( iTrack );
                    for( auto i = log.memoryBegin( trackType ), iEnd = log.memoryEnd( trackType ); i != iEnd; ++i )
                    {
                        const mega::log::MemoryRecordRead& memoryRecord = *i;
                        SPDLOG_LOGGER_CALL(
                            spdlog::default_logger_raw(), spdlog::level::info, memoryRecord.getReference() );
                    }
                }
            }
            if( bShowSchedulingRecords )
            {
                for( auto i = log.schedBegin(), iEnd = log.schedEnd(); i != iEnd; ++i )
                {
                    const mega::log::SchedulerRecordRead& schedulerRecord = *i;
                    switch( schedulerRecord.getType() )
                    {
                        case mega::log::SchedulerRecord::Start:
                        {
                            std::ostringstream os;
                            os << "START: " << schedulerRecord.getReference();
                            SPDLOG_LOGGER_CALL( spdlog::default_logger_raw(), spdlog::level::info, os.str() );
                        }
                        break;
                        case mega::log::SchedulerRecord::Stop:
                        {
                            std::ostringstream os;
                            os << "STOP:  " << schedulerRecord.getReference();
                            SPDLOG_LOGGER_CALL( spdlog::default_logger_raw(), spdlog::level::info, os.str() );
                        }
                        break;
                        default:
                            THROW_RTE( "Unknown scheduling record type" );
                            break;
                    }
                }
            }*/
        }
        else
        {
            std::cout << "Unrecognised project command" << std::endl;
        }
    }
}
} // namespace log
} // namespace driver
