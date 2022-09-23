#include "service/protocol/common/status.hpp"

namespace mega
{
namespace network
{
namespace
{
struct StatusPrinter
{
    std::ostream& os;
    int           iDepth = 0;
    StatusPrinter( std::ostream& os )
        : os( os )
    {
    }
    StatusPrinter( std::ostream& os, int iDepth )
        : os( os )
        , iDepth( iDepth )
    {
    }

    void pad( int i ) const
    {
        for ( int i = 0; i < iDepth; ++i )
            os << " ";
    }

    std::ostream& operator()( const Status& status ) const
    {
        // generate padding
        pad( iDepth );
        os << status.getDescription() << "\n";

        for ( const Status& child : status.getChildren() )
        {
            ( StatusPrinter( os, iDepth + 4 ) )( child );
        }
        return os;
    }
};
} // namespace

std::ostream& operator<<( std::ostream& os, const Status& status ) { return ( StatusPrinter( os ) )( status ); }

} // namespace network
} // namespace mega