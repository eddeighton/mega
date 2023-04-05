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
        for( int i = 1; i < iPadding; ++i )
        {
            os << " ";
        }
        return os;
    }

    std::ostream& operator()( const Status& status ) const
    {
        using ::operator<<; //( std::ostream&, const mega::MP& );

        if( !status.getDescription().empty() )
        {
            line( iCurrentDepth ) << "NODE[ " << status.getDescription() << " ]\n";
        }

        // generate padding
        if( status.getMachineID().has_value() )
        {
            const mega::MachineID& machineID = status.getMachineID().value();
            line( iCurrentDepth + 2 ) << "M: " << machineID << "\n";
        }

        if( status.getMP().has_value() )
        {
            const mega::MP& mp = status.getMP().value();
            line( iCurrentDepth + 2 ) << "MP: " << mp << "\n";
        }

        if( status.getMPO().has_value() )
        {
            const mega::MPO& mpo = status.getMPO().value();
            line( iCurrentDepth + 2 ) << "MPO: " << mpo << "\n";
        }

        if( status.getLogIterator().has_value() )
        {
            const log::IndexRecord& iter = status.getLogIterator().value();
            for( auto i = 0; i != log::toInt( log::TrackType::TOTAL ); ++i )
            {
                if( auto amt = iter.get( log::TrackType( i ) ).get(); amt > 0 )
                {
                    line( iCurrentDepth + 2 ) << "LOG: " << log::toName( log::TrackType( i ) ) << ": "
                                              << iter.get( log::TrackType( i ) ).get() << "\n";
                }
            }
        }

        if( status.getMemory().has_value() )
        {
            const network::MemoryStatus& memory = status.getMemory().value();

            line( iCurrentDepth + 2 ) << "Heap Memory\n";
            line( iCurrentDepth + 4 ) << "Bytes:   " << memory.m_heap << "\n";
            line( iCurrentDepth + 4 ) << "Objects:" << memory.m_object << "\n";
            line( iCurrentDepth + 2 ) << "Fixed Memory\n";

            for( const auto& alloc : memory.m_allocators )
            {
                if( alloc.typeID != mega::TypeID{} )
                {
                    line( iCurrentDepth + 4 ) << "Type: " << alloc.typeID << "\n";
                    line( iCurrentDepth + 4 ) << "Total:" << alloc.status.total << "\n";
                    line( iCurrentDepth + 4 ) << "Block:" << alloc.status.blockSize << "\n";
                    line( iCurrentDepth + 4 ) << "Alloc:" << alloc.status.allocations << "\n";
                    line( iCurrentDepth + 4 ) << "Free: " << alloc.status.free << "\n";
                }
            }
        }

        if( status.getReads().has_value() )
        {
            for( const auto& [ mpo, lockCycle ] : status.getReads().value() )
            {
                line( iCurrentDepth + 2 ) << "Read: " << mpo << " cycle: " << lockCycle << "\n";
            }
        }

        if( status.getWrites().has_value() )
        {
            for( const auto& [ mpo, lockCycle ] : status.getWrites().value() )
            {
                line( iCurrentDepth + 2 ) << "Write: " << mpo << " cycle: " << lockCycle << "\n";
            }
        }

        if( status.getReaders().has_value() )
        {
            for( const auto& mpo : status.getReaders().value() )
            {
                line( iCurrentDepth + 2 ) << "Reader: " << mpo << "\n";
            }
        }

        if( status.getWriter().has_value() )
        {
            line( iCurrentDepth + 2 ) << "Writer: " << status.getWriter().value() << "\n";
        }

        for( const auto& conID : status.getConversations() )
        {
            line( iCurrentDepth + 2 ) << "ConID: " << conID << "\n";
        }

        for( const Status& child : status.getChildren() )
        {
            ( StatusPrinter( os, iCurrentDepth + 4 ) )( child );
        }
        return os;
    }
};
} // namespace

std::ostream& operator<<( std::ostream& os, const Status& status )
{
    return ( StatusPrinter( os ) )( status );
}

} // namespace mega::network
