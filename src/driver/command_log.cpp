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

#include "log/file_log.hpp"

#include "service/network/log.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <vector>
#include <string>
#include <iostream>

namespace driver::log
{

void command( bool bHelp, const std::vector< std::string >& args )
{
    boost::filesystem::path logFolderPath;

    bool bShowLogRecords        = false;
    bool bShowStructureRecords  = false;
    bool bShowSchedulingRecords = false;
    bool bShowMemoryRecords     = false;
    bool bShowAll               = false;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Simulation Commands" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "folder",     po::value( &logFolderPath ),                "Log folder path. ( Default argumnet )" )
            
            ( "msgs",       po::bool_switch( &bShowLogRecords )->default_value( true ),     "Show log message records." )
            ( "structure",  po::bool_switch( &bShowStructureRecords ),                      "Show structure records." )
            ( "scheduling", po::bool_switch( &bShowSchedulingRecords ),                     "Show scheduling records." )
            ( "memory",     po::bool_switch( &bShowMemoryRecords ),                         "Show memory records." )
            ( "all",        po::bool_switch( &bShowAll ),                                   "Show all records." )
            ;
        // clang-format on
    }

    po::positional_options_description p;
    p.add( "folder", -1 );

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).positional( p ).run(), vm );
    po::notify( vm );

    if( bShowAll )
    {
        bShowLogRecords        = true;
        bShowStructureRecords  = true;
        bShowSchedulingRecords = true;
        bShowMemoryRecords     = true;
    }
    // if nothing then show log messages
    else if( !bShowStructureRecords && !bShowSchedulingRecords && !bShowMemoryRecords )
    {
        bShowLogRecords = true;
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
            mega::log::FileStorage log( logFolderPath, true );
            SPDLOG_INFO( "Loading event log: {}", logFolderPath.string() );

            if( bShowLogRecords )
            {
                using namespace mega::log::Log;
                for( auto i = log.begin< Read >(), iEnd = log.end< Read >(); i != iEnd; ++i )
                {
                    const Read&        logMsg = *i;
                    std::ostringstream os;
                    os << std::setw( 15 ) << std::setfill( ' ' ) << toString( logMsg.getType() ) << ": "
                       << logMsg.getMessage();
                    switch( logMsg.getType() )
                    {
                        case eTrace:
                            SPDLOG_TRACE( os.str() );
                            break;
                        case eDebug:
                            SPDLOG_DEBUG( os.str() );
                            break;
                        case eInfo:
                            SPDLOG_INFO( os.str() );
                            break;
                        case eWarn:
                            SPDLOG_WARN( os.str() );
                            break;
                        case eError:
                            SPDLOG_ERROR( os.str() );
                            break;
                        case eFatal:
                            SPDLOG_CRITICAL( os.str() );
                            break;
                    }
                }
            }
            if( bShowStructureRecords )
            {
                using namespace mega::log::Structure;
                for( auto i = log.begin< Read >(), iEnd = log.end< Read >(); i != iEnd; ++i )
                {
                    const Read&        record = *i;
                    std::ostringstream os;
                    os << std::setw( 15 ) << std::setfill( ' ' ) << toString( record.getType() ) << ": "
                       << record.getSource() << ": " << record.getTarget() << ": " << record.getRelation();
                    SPDLOG_INFO( os.str() );
                }
            }
            if( bShowSchedulingRecords )
            {
                using namespace mega::log::Scheduling;
                for( auto i = log.begin< Read >(), iEnd = log.end< Read >(); i != iEnd; ++i )
                {
                    const Read&        schedulingRecord = *i;
                    std::ostringstream os;
                    os << std::setw( 15 ) << std::setfill( ' ' ) << toString( schedulingRecord.getType() ) << ": "
                       << schedulingRecord.getRef();
                    SPDLOG_INFO( os.str() );
                }
            }
            if( bShowMemoryRecords )
            {
                using namespace mega::log::Memory;
                for( auto i = log.begin< Read >(), iEnd = log.end< Read >(); i != iEnd; ++i )
                {
                    const Read& memoryRecord = *i;
                    SPDLOG_INFO( memoryRecord.getRef() );
                }
            }
        }
        else
        {
            std::cout << "Unrecognised project command" << std::endl;
        }
    }
}

} // namespace driver::log
