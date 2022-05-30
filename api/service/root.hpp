#ifndef ROOT_26_MAY_2022
#define ROOT_26_MAY_2022

#include "service/network/server.hpp"
#include "service/network/activity_manager.hpp"

#include "boost/asio/io_context.hpp"

namespace mega
{
namespace service
{

class Root
{
    class RootActivityFactory : public network::ActivityFactory
    {
    public:
        RootActivityFactory( Root& root ) : m_root( root ) {}
        virtual network::Activity::Ptr
        createRequestActivity( const network::Header&       msgHeader,
                               const network::ConnectionID& originatingConnectionID ) const;

    private:
        Root& m_root;
    };
    friend class RootRequestActivity;
    friend class RootPipelineActivity;

public:
    Root( boost::asio::io_context& ioContext );

private:
    boost::asio::io_context& m_io_context;
    RootActivityFactory      m_activityFactory;
    network::ActivityManager m_activityManager;
    network::Server          m_server;
};

} // namespace service
} // namespace mega

#endif // ROOT_26_MAY_2022
