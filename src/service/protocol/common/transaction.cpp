
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

#include "service/protocol/common/transaction.hpp"

namespace mega::network
{

TransactionProducer::TransactionProducer( mega::log::Storage& log )
    : m_log( log )
    , m_iteratorEnd( m_log.getIterator() )
    , m_iterator( m_log.getIterator() )
{
}
void TransactionProducer::generateStructure( MPOTransactions& transactions, UnparentedSet& unparented )
{
    using RecordType                    = log::Structure::Read;
    log::Iterator< RecordType > iter    = m_log.begin< RecordType >( m_iterator );
    log::Iterator< RecordType > iterEnd = m_log.begin< RecordType >( m_iteratorEnd );
    for( ; iter != iterEnd; ++iter )
    {
        const RecordType r = *iter;
        switch( r.getType() )
        {
            case log::Structure::eConstruct:
            {
                // do nothing
            }
            break;
            case log::Structure::eDestruct:
            {
                // do nothing
            }
            break;
            case log::Structure::eMake:
            case log::Structure::eMakeSource:
            case log::Structure::eMakeTarget:
            {
                transactions[ r.getSource().getMPO() ].push_back( r );
            }
            break;
            case log::Structure::eBreak:
            {
                ASSERT( r.getSource().getMPO() == r.getTarget().getMPO() );
                transactions[ r.getSource().getMPO() ].push_back( r );
            }
            break;
            case log::Structure::eBreakSource:
            {
                transactions[ r.getSource().getMPO() ].push_back( r );
                ASSERT( r.getSource().isHeapAddress() );
                if( r.getSource().getRefCount() == 0U )
                {
                    unparented.insert( r.getSource().getObjectAddress() );
                }
            }
            break;
            case log::Structure::eBreakTarget:
            {
                transactions[ r.getTarget().getMPO() ].push_back( r );
                ASSERT( r.getTarget().isHeapAddress() );
                if( r.getTarget().getRefCount() == 0U )
                {
                    unparented.insert( r.getTarget().getObjectAddress() );
                }
            }
            break;
            case log::Structure::eMove:
            {
            }
            break;
            default:
            {
                THROW_RTE( "Unknown structure record type" );
            }
            break;
        }
    }
}
void TransactionProducer::generateScheduling( MPOTransactions& transactions )
{
    using RecordType                    = log::Scheduling::Read;
    log::Iterator< RecordType > iter    = m_log.begin< RecordType >( m_iterator );
    log::Iterator< RecordType > iterEnd = m_log.begin< RecordType >( m_iteratorEnd );
    for( ; iter != iterEnd; ++iter )
    {
        const RecordType r = *iter;
        transactions[ r.getRef().getMPO() ].push_back( r );
    }
}
void TransactionProducer::generateMemory( MPOTransactions& transactions )
{
    using RecordType                    = log::Memory::Read;
    log::Iterator< RecordType > iter    = m_log.begin< RecordType >( m_iterator );
    log::Iterator< RecordType > iterEnd = m_log.begin< RecordType >( m_iteratorEnd );
    for( ; iter != iterEnd; ++iter )
    {
        const RecordType r = *iter;
        transactions[ r.getRef().getMPO() ].push_back( r );
    }
}

void TransactionProducer::generate( MPOTransactions& transactions, UnparentedSet& unparented )
{
    generateStructure( transactions, unparented );
    generateScheduling( transactions );
    generateMemory( transactions );
    m_iterator = m_iteratorEnd;
}

} // namespace mega::network