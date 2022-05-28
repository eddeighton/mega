
#ifndef LOG_28_MAY_2022
#define LOG_28_MAY_2022

#include "service/protocol/model/messages.hxx"

#include "spdlog/spdlog.h"

#include "boost/filesystem/path.hpp"

#include <string>
#include <iterator>

namespace mega
{
namespace network
{
std::shared_ptr< spdlog::details::thread_pool > configureLog( const boost::filesystem::path& logFolder,
                                                              const std::string&             strLogName );
}
} // namespace mega

namespace fmt
{
template <>
struct formatter< mega::network::MessageVariant >
{
    constexpr auto parse( format_parse_context& ctx ) -> decltype( ctx.begin() ) { return ctx.begin(); }

    template < typename FormatContext >
    auto format( const mega::network::MessageVariant& msg, FormatContext& ctx ) -> decltype( ctx.out() )
    {
        std::ostringstream os;
        os << msg;
        return format_to( ctx.out(), "{}", os.str() );
    }
};

template <>
struct formatter< mega::network::ActivityID >
{
    constexpr auto parse( format_parse_context& ctx ) -> decltype( ctx.begin() ) { return ctx.begin(); }
    template < typename FormatContext >
    auto format( const mega::network::ActivityID& activityID, FormatContext& ctx ) -> decltype( ctx.out() )
    {
        std::ostringstream os;
        os << activityID;
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
        std::ostringstream os;
        os << header;
        return format_to( ctx.out(), "{}", os.str() );
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
