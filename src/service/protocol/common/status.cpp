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
    int           iCurrentDepth = 0;
    StatusPrinter( std::ostream& os )
        : os( os )
    {
    }
    StatusPrinter( std::ostream& os, int iCurrentDepth )
        : os( os )
        , iCurrentDepth( iCurrentDepth )
    {
    }

    std::ostream& line( int iPadding ) const
    {
        for ( int i = 1; i < iPadding; ++i )
        {
            os << " ";
        }
        return os;
    }

    std::ostream& operator()( const Status& status ) const
    {
        line( iCurrentDepth ) << status.getDescription() << "\n";

        // generate padding
        if ( status.getMPO().has_value() )
        {
            const mega::MPO& mpo = status.getMPO().value();
            line( iCurrentDepth + 2 ) << "MPO: " << ( int )mpo.getMachineID() << "." << ( int )mpo.getProcessID() << "."
                                      << ( int )mpo.getOwnerID() << "\n";
        }

        for ( const auto& conID : status.getConversations() )
        {
            line( iCurrentDepth + 2 ) << "ConID: " << conID << "\n";
        }

        for ( const Status& child : status.getChildren() )
        {
            ( StatusPrinter( os, iCurrentDepth + 4 ) )( child );
        }
        return os;
    }
};
} // namespace

std::ostream& operator<<( std::ostream& os, const Status& status ) { return ( StatusPrinter( os ) )( status ); }

} // namespace network
} // namespace mega