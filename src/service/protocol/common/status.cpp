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
StatusPrinter::StatusPrinter()
{
}

std::ostream& StatusPrinter::print( std::ostream& os, int iIndent, char c ) const
{
    int iBarCount = m_stack.size() - 1;
    for( auto iBar : m_stack )
    {
        for( int i = 1; i < iBar; ++i )
        {
            if( i == 1 )
            {
                os << "|";
            }
            else if( iBarCount == 0 )
            {
                os << c;
            }
            else
            {
                os << ' ';
            }
        }
        --iBarCount;
    }
    for( int i = 1; i < iIndent; ++i )
    {
        os << c;
    }
    return os;
}

std::ostream& StatusPrinter::line( std::ostream& os, int iIndent ) const
{
    return print( os, iIndent, ' ' );
}

std::ostream& StatusPrinter::dash( std::ostream& os, int iIndent ) const
{
    return print( os, iIndent, '-' );
}

void StatusPrinter::printNodeInfo( const Status& status, std::ostream& os )
{
    using ::operator<<; //( std::ostream&, const mega::MP& );

    int indent = 4;

    // generate padding
    if( status.getMPO().has_value() )
    {
        line( os, 4 ) << "MPO: " << status.getMPO().value() << "\n";
    }
    else if( status.getMP().has_value() )
    {
        line( os, indent ) << "MP: " << status.getMP().value() << "\n";
    }
    else if( status.getMachineID().has_value() )
    {
        line( os, indent ) << "M: " << status.getMachineID().value() << "\n";
    }

    if( status.getLogIterator().has_value() )
    {
        const log::IndexRecord& iter = status.getLogIterator().value();
        for( auto i = 0; i != log::toInt( log::TrackType::TOTAL ); ++i )
        {
            if( auto amt = iter.get( log::TrackType( i ) ).get(); amt > 0 )
            {
                line( os, indent ) << "LOG: " << log::toName( log::TrackType( i ) ) << ": "
                                   << iter.get( log::TrackType( i ) ).get() << "\n";
            }
        }
    }

    if( status.getMemory().has_value() )
    {
        const network::MemoryStatus& memory = status.getMemory().value();

        line( os, indent ) << "Heap Memory\n";
        line( os, indent * 2 ) << "Bytes:  " << memory.m_heap << "\n";
        line( os, indent * 2 ) << "Objects:" << memory.m_object << "\n";
        line( os, indent ) << "Fixed Memory\n";

        for( const auto& alloc : memory.m_allocators )
        {
            if( alloc.typeID != mega::TypeID{} )
            {
                line( os, indent * 2 ) << "Type: " << alloc.typeID << "\n";
                line( os, indent * 2 ) << "Total:" << alloc.status.total << " ( " << alloc.status.blockSize << " * "
                                       << alloc.status.allocations << " )\n";
                line( os, indent * 2 ) << "Free: " << alloc.status.free << "\n";
            }
        }
    }

    if( status.getReads().has_value() )
    {
        for( const auto& [ mpo, lockCycle ] : status.getReads().value() )
        {
            line( os, indent ) << "Read: " << mpo << " cycle: " << lockCycle << "\n";
        }
    }

    if( status.getWrites().has_value() )
    {
        for( const auto& [ mpo, lockCycle ] : status.getWrites().value() )
        {
            line( os, indent ) << "Write: " << mpo << " cycle: " << lockCycle << "\n";
        }
    }

    if( status.getReaders().has_value() )
    {
        for( const auto& mpo : status.getReaders().value() )
        {
            line( os, indent ) << "Reader: " << mpo << "\n";
        }
    }

    if( status.getWriter().has_value() )
    {
        line( os, indent ) << "Writer: " << status.getWriter().value() << "\n";
    }

    // for( const auto& conID : status.getConversations() )
    // {
    //     line( os, indent ) << "ConID: " << conID << "\n";
    // }

    for( const Status& child : status.getChildren() )
    {
        printNode( child, os );
    }
}

void StatusPrinter::printNode( const Status& status, std::ostream& os )
{
    // empty line
    line( os, 4 ) << "\n";

    if( !status.getDescription().empty() )
    {
        dash( os ) << "NODE[ " << status.getDescription() << " ]\n";
    }

    m_stack.push_back( 4 );
    printNodeInfo( status, os );
    m_stack.pop_back();
}

void StatusPrinter::print( const Status& status, std::ostream& os )
{
    printNode( status, os );
}

std::ostream& operator<<( std::ostream& os, const Status& status )
{
    StatusPrinter printer;
    printer.print( status, os );
    return os;
}
#

} // namespace mega::network
