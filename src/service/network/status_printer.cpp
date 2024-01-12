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

#include "service/network/status_printer.hpp"

#include "mega/values/service/logical_thread_id.hpp"

namespace mega::utilities
{

StatusPrinter::StatusPrinter( Config config )
    : m_config( config )
{
}

std::ostream& StatusPrinter::print( std::ostream& os, int iIndent, char c ) const
{
    int iBarCount = m_stack.size() - 1;

    for( int iIndex = 0; iIndex != m_stack.size(); ++iIndex )
    {
        const int  iBar      = m_stack[ iIndex ];
        const bool bLastLast = ( ( iIndex + 1 ) < m_stackLast.size() ) ? m_stackLast[ iIndex + 1 ] : false;

        for( int i = 1; i < iBar; ++i )
        {
            if( i == 1 )
            {
                if( bLastLast )
                {
                    os << " ";
                }
                else
                {
                    os << "|";
                }
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

void StatusPrinter::printNodeInfo( const network::Status& status, std::ostream& os )
{
    int indent = 4;

    if( m_config.m_bLog )
    {
        if( status.getLogFolder().has_value() )
        {
            line( os, indent ) << "LOG: " << status.getLogFolder().value() << "\n";
        }
        if( status.getLogIterator().has_value() )
        {
            const event::IndexRecord& iter = status.getLogIterator().value();
            for( auto i = 0; i != event::toInt( event::TrackID::TOTAL ); ++i )
            {
                if( auto amt = iter.get( event::TrackID( i ) ).get(); amt > 0 )
                {
                    line( os, indent ) << "  " << event::toName( event::TrackID( i ) ) << ": "
                                       << iter.get( event::TrackID( i ) ).get() << "\n";
                }
            }
        }
    }

    if( m_config.m_bMemory )
    {
        if( status.getMemory().has_value() )
        {
            const network::MemoryStatus& memory = status.getMemory().value();

            line( os, indent ) << "Heap Memory: " << memory.m_heap << "\n";
            line( os, indent ) << "Objects:     " << memory.m_object << "\n";
            /*line( os, indent ) << "Fixed Memory\n";

            for( const auto& alloc : memory.m_allocators )
            {
                if( alloc.typeID != mega::TypeID{} )
                {
                    line( os, indent * 2 ) << "Type: " << alloc.typeID << "\n";
                    line( os, indent * 2 ) << "Total:" << alloc.status.total << " ( " << alloc.status.blockSize << " * "
                                           << alloc.status.allocations << " )\n";
                    line( os, indent * 2 ) << "Free: " << alloc.status.free << "\n";
                }
            }*/
        }
    }

    if( m_config.m_bLocks )
    {
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
    }

    if( m_config.m_bLogicalThreads )
    {
        for( const auto& threadID : status.getLogicalThreads() )
        {
            line( os, indent ) << "ThreadID: " << threadID << "\n";
        }
    }

    int iCount = status.getChildren().size();
    for( const network::Status& child : status.getChildren() )
    {
        --iCount;
        printNode( child, os, iCount == 0 );
    }
}

void StatusPrinter::printNode( const network::Status& status, std::ostream& os, bool bLast )
{
    int iIndent = 4;

    // empty line
    line( os, iIndent ) << "\n";

    if( !status.getDescription().empty() )
    {
        std::string strID;
        {
            std::ostringstream osID;
            // generate padding
            if( status.getMPO().has_value() )
            {
                osID << status.getMPO().value() << " ";
            }
            else if( status.getMP().has_value() )
            {
                osID << status.getMP().value() << " ";
            }
            else if( status.getMachineID().has_value() )
            {
                osID << status.getMachineID().value() << " ";
            }
            strID = osID.str();
        }

        dash( os ) << strID << status.getDescription() << "\n";
    }

    m_stack.push_back( iIndent );
    m_stackLast.push_back( bLast );
    printNodeInfo( status, os );
    m_stack.pop_back();
    m_stackLast.pop_back();
}

void StatusPrinter::print( const network::Status& status, std::ostream& os )
{
    printNode( status, os, true );
}

} // namespace mega::utilities
