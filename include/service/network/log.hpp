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

#ifndef LOG_28_MAY_2022
#define LOG_28_MAY_2022

#include "mega/values/compilation/invocation_id.hpp"
#include "mega/values/compilation/type_id.hpp"
#include "mega/values/compilation/sub_type_instance.hpp"
#include "mega/values/compilation/type_instance.hpp"

#include "mega/values/runtime/reference.hpp"
#include "mega/values/runtime/reference_io.hpp"

#include "mega/values/service/logical_thread_id.hpp"

#include "common/time.hpp"

#include <spdlog/spdlog.h>

#include <boost/filesystem/path.hpp>

#include <string>
#include <iterator>
#include <chrono>

namespace mega::network
{

using LogTime = std::chrono::steady_clock::duration;

enum LoggingLevel
{
    eDebug,
    eTrace,
    eInfo,
    eWarn,
    eError,
    eOff,
    TOTAL_LOG_LEVELS
};
LoggingLevel fromStr( const std::string& str );

struct Log
{
    struct Config
    {
        // configuration
        boost::filesystem::path logFolder;
        std::string             strLogName;
        LoggingLevel            consoleLoggingLevel;
        LoggingLevel            fileLoggingLevel;
    } config;

    // result
    boost::filesystem::path           logFile;
    std::shared_ptr< spdlog::logger > pLogger;
};

Log configureLog( Log::Config config );

inline void logLinesInfo( const std::string strMsg )
{
    std::istringstream is( strMsg );
    while( is )
    {
        std::string strLine;
        std::getline( is, strLine );
        if( !strLine.empty() )
        {
            SPDLOG_INFO( "{}", strLine );
        }
    }
}
inline void logLinesWarn( const std::string& strTaskName, const std::string strMsg )
{
    std::istringstream is( strMsg );
    while( is )
    {
        std::string strLine;
        std::getline( is, strLine );
        if( !strLine.empty() )
        {
            SPDLOG_WARN( "{} {}", strTaskName, strLine );
        }
    }
}

template < typename T >
inline void logLinesSuccessFail( const std::string strMsg, bool bSuccess, const T& firstLinePostFix )
{
    std::istringstream is( strMsg );
    bool               bPrintedTime = false;
    while( is )
    {
        std::string strLine;
        std::getline( is, strLine );
        if( !strLine.empty() )
        {
            if( !bPrintedTime )
            {
                if( bSuccess )
                    SPDLOG_INFO( "{} {}", strLine, firstLinePostFix );
                else
                    SPDLOG_WARN( "{} {}", strLine, firstLinePostFix );
                bPrintedTime = true;
            }
            else
            {
                if( bSuccess )
                    SPDLOG_INFO( "{}", strLine );
                else
                    SPDLOG_WARN( "{}", strLine );
            }
        }
    }
}

} // namespace mega::network

namespace fmt
{
template <>
struct formatter< mega::network::LogTime >
{
    constexpr auto parse( format_parse_context& ctx ) -> decltype( ctx.begin() ) { return ctx.begin(); }

    template < typename FormatContext >
    inline auto format( const mega::network::LogTime& logTime, FormatContext& ctx ) -> decltype( ctx.out() )
    {
        using DurationType = std::chrono::duration< mega::I64, std::ratio< 1, 1'000'000'000 > >;
        auto c             = std::chrono::duration_cast< DurationType >( logTime ).count();
        return fmt::format_to(
            ctx.out(), "{}s.{:03}ms.{:03}us",
            // clang-format off
            ( c % 1'000'000'000'000   ) / 1'000'000'000,
            ( c % 1'000'000'000       ) / 1'000'000,
            ( c % 1'000'000           ) / 1'000
            //( c % 1'000           )
            // clang-format on
        );
    }
};

template <>
struct formatter< mega::network::LogicalThreadID >
{
    constexpr auto parse( format_parse_context& ctx ) -> decltype( ctx.begin() ) { return ctx.begin(); }
    template < typename FormatContext >
    inline auto format( const mega::network::LogicalThreadID& logicalthreadID, FormatContext& ctx )
        -> decltype( ctx.out() )
    {
        std::ostringstream os;
        os << logicalthreadID;
        return fmt::format_to( ctx.out(), "{}", os.str() );
    }
};

template <>
struct formatter< mega::TypeID >
{
    constexpr auto parse( format_parse_context& ctx ) -> decltype( ctx.begin() ) { return ctx.begin(); }
    template < typename FormatContext >
    inline auto format( const mega::TypeID& typeID, FormatContext& ctx ) -> decltype( ctx.out() )
    {
        std::ostringstream os;
        os << typeID;
        return fmt::format_to( ctx.out(), "{}", os.str() );
    }
};

template <>
struct formatter< mega::TypeInstance >
{
    constexpr auto parse( format_parse_context& ctx ) -> decltype( ctx.begin() ) { return ctx.begin(); }
    template < typename FormatContext >
    inline auto format( const mega::TypeInstance& typeInstance, FormatContext& ctx ) -> decltype( ctx.out() )
    {
        std::ostringstream os;
        os << typeInstance;
        return fmt::format_to( ctx.out(), "{}", os.str() );
    }
};

template <>
struct formatter< mega::MP >
{
    constexpr auto parse( format_parse_context& ctx ) -> decltype( ctx.begin() ) { return ctx.begin(); }
    template < typename FormatContext >
    inline auto format( const mega::MP& mp, FormatContext& ctx ) -> decltype( ctx.out() )
    {
        std::ostringstream os;
        os << mp;
        return fmt::format_to( ctx.out(), "{}", os.str() );
    }
};

template <>
struct formatter< mega::MPO >
{
    constexpr auto parse( format_parse_context& ctx ) -> decltype( ctx.begin() ) { return ctx.begin(); }
    template < typename FormatContext >
    inline auto format( const mega::MPO& mpo, FormatContext& ctx ) -> decltype( ctx.out() )
    {
        std::ostringstream os;
        os << mpo;
        return fmt::format_to( ctx.out(), "{}", os.str() );
    }
};

template <>
struct formatter< mega::reference >
{
    constexpr auto parse( format_parse_context& ctx ) -> decltype( ctx.begin() ) { return ctx.begin(); }
    template < typename FormatContext >
    inline auto format( const mega::reference& ref, FormatContext& ctx ) -> decltype( ctx.out() )
    {
        std::ostringstream os;
        os << ref;
        return fmt::format_to( ctx.out(), "{}", os.str() );
    }
};

template <>
struct formatter< mega::InvocationID >
{
    constexpr auto parse( format_parse_context& ctx ) -> decltype( ctx.begin() ) { return ctx.begin(); }
    template < typename FormatContext >
    inline auto format( const mega::InvocationID& invocationID, FormatContext& ctx ) -> decltype( ctx.out() )
    {
        std::ostringstream os;
        os << invocationID;
        return fmt::format_to( ctx.out(), "{}", os.str() );
    }
};

} // namespace fmt

#endif // LOG_28_MAY_2022
