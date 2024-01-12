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

#include "event/file_log.hpp"

#include "log/log.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>

#include <vector>
#include <string>
#include <iostream>

namespace driver::log
{

namespace
{
struct Options
{
    bool bShowLogRecords       = false;
    bool bShowStructureRecords = false;
    bool bShowEvents           = false;
    bool bShowTransitions      = false;
    bool bShowMemoryRecords    = false;

    void configure( bool bShowAll )
    {
        if( bShowAll )
        {
            bShowLogRecords       = true;
            bShowStructureRecords = true;
            bShowEvents           = true;
            bShowTransitions      = true;
            bShowMemoryRecords    = true;
        }
        // if nothing then show log messages
        else if( !bShowStructureRecords && !bShowEvents && !bShowTransitions && !bShowMemoryRecords )
        {
            bShowLogRecords = true;
        }
    }
};

void printLog( mega::event::FileStorage& log, const Options& options, mega::runtime::TimeStamp timestamp )
{
    if( options.bShowLogRecords )
    {
        using namespace mega::event::Log;
        for( auto i = log.begin< Read >( timestamp ), iEnd = log.end< Read >(); i != iEnd; ++i )
        {
            const Read&        logMsg = *i;
            std::ostringstream os;
            os << std::setw( 15 ) << std::setfill( ' ' ) << toString( logMsg.getType() ) << ": " << logMsg.getMessage();
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
    if( options.bShowStructureRecords )
    {
        using namespace mega::event::Structure;
        for( auto i = log.begin< Read >( timestamp ), iEnd = log.end< Read >(); i != iEnd; ++i )
        {
            const Read&        record = *i;
            std::ostringstream os;
            os << std::setw( 15 ) << std::setfill( ' ' ) << toString( record.getType() ) << ": " << record.getSource()
               << ": " << record.getTarget() << ": " << record.getRelation();
            SPDLOG_INFO( os.str() );
        }
    }
    if( options.bShowEvents )
    {
        using namespace mega::event::Event;
        for( auto i = log.begin< Read >( timestamp ), iEnd = log.end< Read >(); i != iEnd; ++i )
        {
            const Read&        eventRecord = *i;
            std::ostringstream os;
            os << std::setw( 15 ) << std::setfill( ' ' ) << toString( eventRecord.getType() ) << ": "
               << eventRecord.getRef();
            SPDLOG_INFO( os.str() );
        }
    }
    if( options.bShowTransitions )
    {
        using namespace mega::event::Transition;
        for( auto i = log.begin< Read >( timestamp ), iEnd = log.end< Read >(); i != iEnd; ++i )
        {
            const Read&        transitionRecord = *i;
            std::ostringstream os;
            os << std::setw( 17 ) << std::setfill( ' ' ) << "Transition: " << transitionRecord.getRef();
            SPDLOG_INFO( os.str() );
        }
    }
    if( options.bShowMemoryRecords )
    {
        using namespace mega::event::Memory;
        for( auto i = log.begin< Read >( timestamp ), iEnd = log.end< Read >(); i != iEnd; ++i )
        {
            const Read& memoryRecord = *i;
            const auto& data         = memoryRecord.getData();

            std::ostringstream osMem;
            osMem << memoryRecord.getRef();

            int x = 0;
            for( auto i = data.begin(), iEnd = data.end(); i != iEnd; ++i, ++x )
            {
                if( x % 4 == 0 )
                    osMem << ' ';
                osMem << std::hex << std::setw( 2 ) << std::setfill( '0' ) << static_cast< unsigned >( *i );
            }

            SPDLOG_INFO( osMem.str() );
        }
    }
}
} // namespace

void command( mega::network::Log& log, bool bHelp, const std::vector< std::string >& args )
{
    boost::filesystem::path logFolderPath;

    Options options;
    bool    bShowAll = false;

    float fSeconds = 0.0f;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Simulation Commands" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "folder",     po::value( &logFolderPath ),                "Log folder path. ( Default argumnet )" )
            
            ( "msgs",       po::bool_switch( &options.bShowLogRecords ),        "Show log message records." )
            ( "structure",  po::bool_switch( &options.bShowStructureRecords ),  "Show structure records." )
            ( "event",      po::bool_switch( &options.bShowEvents ),            "Show event records." )
            ( "transition", po::bool_switch( &options.bShowTransitions ),       "Show transition records." )
            ( "memory",     po::bool_switch( &options.bShowMemoryRecords ),     "Show memory records." )
            ( "all",        po::bool_switch( &bShowAll ),               "Show all records." )

            ( "sec",        po::value( &fSeconds ),                     "Stream to output at rate in seconds" )
            ;
        // clang-format on
    }

    po::positional_options_description p;
    p.add( "folder", -1 );

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).positional( p ).run(), vm );
    po::notify( vm );

    options.configure( bShowAll );

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
            SPDLOG_TRACE( "Loading event log: {}", logFolderPath.string() );

            if( fSeconds > 0.0f )
            {
                const auto rate    = std::chrono::milliseconds( static_cast< int >( fSeconds * 1000.0f ) );
                using Milliseconds = decltype( rate );

                mega::runtime::TimeStamp timestamp;

                boost::asio::io_context   io;
                boost::asio::steady_timer timer( io );
                timer.expires_from_now( rate );

                struct TimerCallback
                {
                    boost::asio::io_context&   io;
                    boost::asio::steady_timer& timer;
                    Milliseconds               rate;
                    mega::runtime::TimeStamp&  timestamp;
                    boost::filesystem::path&   logFolderPath;
                    Options&                   options;

                    void operator()() const
                    {
                        mega::event::FileStorage log( logFolderPath, true );
                        printLog( log, options, timestamp );
                        timestamp = log.getTimeStamp();

                        timer.expires_from_now( rate );
                        timer.async_wait( [ next = *this ]( boost::system::error_code ec )
                                          { boost::asio::post( next.io, next ); } );
                    }
                } callback{ io, timer, rate, timestamp, logFolderPath, options };

                boost::asio::post( io, callback );
                io.run();
            }
            else
            {
                mega::event::FileStorage log( logFolderPath, true );
                printLog( log, options, 0 );
            }
        }
        else
        {
            std::cout << "Unrecognised project command" << std::endl;
        }
    }
}

} // namespace driver::log
