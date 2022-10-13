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

    namespace po = boost::program_options;
    po::options_description commandOptions( " Simulation Commands" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "folder",     po::value( &logFolderPath ),    "Log folder path" )
            
            ;
        // clang-format on
    }

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).run(), vm );
    po::notify( vm );

    if ( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        if ( !logFolderPath.empty() )
        {
            VERIFY_RTE_MSG(
                boost::filesystem::exists( logFolderPath ), "Failed to locate folder: " << logFolderPath.string() );
            mega::log::Storage log( logFolderPath, true );
            SPDLOG_INFO( "Loaded log folder: {}", logFolderPath.string() );

            for ( auto i = log.logBegin(), iEnd = log.logEnd(); i != iEnd; ++i )
            {
                const auto& logMsg = *i;
                switch ( logMsg.getType() )
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
        else
        {
            std::cout << "Unrecognised project command" << std::endl;
        }
    }
}
} // namespace log
} // namespace driver
