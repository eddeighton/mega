
#ifndef LOG_28_MAY_2022
#define LOG_28_MAY_2022

#include "service/protocol/model/messages.hxx"

#include "spdlog/spdlog.h"

#include "boost/filesystem/path.hpp"

#include <string>
#include <iterator>
#include <chrono>

namespace mega
{
namespace network
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

std::shared_ptr< spdlog::details::thread_pool > configureLog( const boost::filesystem::path& logFolder,
                                                              const std::string&             strLogName,
                                                              LoggingLevel                   consoleLoggingLevel,
                                                              LoggingLevel                   fileLoggingLevel );

inline void logLinesInfo( const std::string strMsg )
{
    std::istringstream is( strMsg );
    while ( is )
    {
        std::string strLine;
        std::getline( is, strLine );
        if ( !strLine.empty() )
        {
            SPDLOG_INFO( "{}", strLine );
        }
    }
}
inline void logLinesWarn( const std::string& strTaskName, const std::string strMsg )
{
    std::istringstream is( strMsg );
    while ( is )
    {
        std::string strLine;
        std::getline( is, strLine );
        if ( !strLine.empty() )
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
    while ( is )
    {
        std::string strLine;
        std::getline( is, strLine );
        if ( !strLine.empty() )
        {
            if ( !bPrintedTime )
            {
                if ( bSuccess )
                    SPDLOG_INFO( "{} {}", strLine, firstLinePostFix );
                else
                    SPDLOG_WARN( "{} {}", strLine, firstLinePostFix );
                bPrintedTime = true;
            }
            else
            {
                if ( bSuccess )
                    SPDLOG_INFO( "{}", strLine );
                else
                    SPDLOG_WARN( "{}", strLine );
            }
        }
    }
}

} // namespace network
} // namespace mega

namespace fmt
{
template <>
struct formatter< mega::network::LogTime >
{
    constexpr auto parse( format_parse_context& ctx ) -> decltype( ctx.begin() ) { return ctx.begin(); }

    template < typename FormatContext >
    auto format( const mega::network::LogTime& logTime, FormatContext& ctx ) -> decltype( ctx.out() )
    {
        using DurationType = std::chrono::duration< std::int64_t, std::ratio< 1, 1'000'000'000 > >;
        auto c             = std::chrono::duration_cast< DurationType >( logTime ).count();
        return format_to(
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
struct formatter< mega::network::Message >
{
    constexpr auto parse( format_parse_context& ctx ) -> decltype( ctx.begin() ) { return ctx.begin(); }

    template < typename FormatContext >
    auto format( const mega::network::Message& msg, FormatContext& ctx ) -> decltype( ctx.out() )
    {
        std::ostringstream os;
        os << msg;
        return format_to( ctx.out(), "{}", os.str() );
    }
};

template <>
struct formatter< mega::network::ConversationID >
{
    constexpr auto parse( format_parse_context& ctx ) -> decltype( ctx.begin() ) { return ctx.begin(); }
    template < typename FormatContext >
    auto format( const mega::network::ConversationID& conversationID, FormatContext& ctx ) -> decltype( ctx.out() )
    {
        std::ostringstream os;
        os << conversationID;
        return format_to( ctx.out(), "{}", os.str() );
    }
};

template <>
struct formatter< mega::network::Header >
{
    constexpr auto parse( format_parse_context& ctx ) -> decltype( ctx.begin() ) { return ctx.begin(); }
    template < typename FormatContext >
    auto format( const mega::network::Header& header, FormatContext& ctx ) -> decltype( ctx.out() )
    {
        return format_to( ctx.out(), "{}::{}", header.getConversationID(), header.getMessageID() );
    }
};

template <>
struct formatter< mega::network::ConnectionID >
{
    constexpr auto parse( format_parse_context& ctx ) -> decltype( ctx.begin() ) { return ctx.begin(); }
    template < typename FormatContext >
    auto format( const mega::network::ConnectionID& connectionID, FormatContext& ctx ) -> decltype( ctx.out() )
    {
        std::ostringstream os;
        os << connectionID;
        return format_to( ctx.out(), "{}", os.str() );
    }
};

} // namespace fmt

#endif // LOG_28_MAY_2022
