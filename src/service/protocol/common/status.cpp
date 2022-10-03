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

#include "service/protocol/common/status.hpp"

#include "mega/reference.hpp"
#include "mega/reference_io.hpp"

namespace mega::network
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
        using ::operator<<; //( std::ostream&, const mega::MP& );

        if ( !status.getDescription().empty() )
        {
            line( iCurrentDepth ) << status.getDescription() << "\n";
        }

        // generate padding
        if ( status.getMP().has_value() )
        {
            const mega::MP& mp = status.getMP().value();
            line( iCurrentDepth + 2 ) << "MP: " << mp << "\n";
        }

        if ( status.getMPO().has_value() )
        {
            const mega::MPO& mpo = status.getMPO().value();
            line( iCurrentDepth + 2 ) << "MPO: " << mpo << "\n";
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

} // namespace mega::network
