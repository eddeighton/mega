
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

TransactionProducer::TransactionProducer( mega::log::FileStorage& log )
    : m_log( log )
    , m_iteratorEnd( m_log.getIterator() )
    , m_iterator( m_log.getIterator() )
{
}
void TransactionProducer::generateStructure( MPOTransactions& transactions, UnparentedSet& unparented,
                                             MovedObjects& movedObjects )
{
    using RecordType                        = log::Structure::Read;
    log::FileIterator< RecordType > iter    = m_log.begin< RecordType >( m_iterator );
    log::FileIterator< RecordType > iterEnd = m_log.begin< RecordType >( m_iteratorEnd );
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
            {
                transactions[ r.getSource().getMPO() ].push_back( r );
            }
            break;
            case log::Structure::eBreak:
            {
                ASSERT( r.getSource().getMPO() == r.getTarget().getMPO() );
                transactions[ r.getSource().getMPO() ].push_back( r );
                /*if( r.getSource().getRefCount() == 0U )
                {
                    unparented.insert( r.getSource().getObjectAddress() );
                }
                if( r.getTarget().getRefCount() == 0U )
                {
                    unparented.insert( r.getSource().getObjectAddress() );
                }*/
            }
            break;
            case log::Structure::eMove:
            {
                // transactions[ r.getSource().getMPO() ].push_back( r );

                // prevent locally deleting the object
                //unparented.erase( r.getSource().getObjectAddress() );
                movedObjects.insert( { r.getTarget().getMPO(), { r.getSource(), r.getTarget() } } );
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
    using RecordType                        = log::Scheduling::Read;
    log::FileIterator< RecordType > iter    = m_log.begin< RecordType >( m_iterator );
    log::FileIterator< RecordType > iterEnd = m_log.begin< RecordType >( m_iteratorEnd );
    for( ; iter != iterEnd; ++iter )
    {
        const RecordType r = *iter;
        transactions[ r.getRef().getMPO() ].push_back( r );
    }
}
void TransactionProducer::generateMemory( MPOTransactions& transactions )
{
    using RecordType                        = log::Memory::Read;
    log::FileIterator< RecordType > iter    = m_log.begin< RecordType >( m_iterator );
    log::FileIterator< RecordType > iterEnd = m_log.begin< RecordType >( m_iteratorEnd );
    for( ; iter != iterEnd; ++iter )
    {
        const RecordType r = *iter;
        transactions[ r.getRef().getMPO() ].push_back( r );
    }
}

void TransactionProducer::generate( MPOTransactions& transactions, UnparentedSet& unparented,
                                    MovedObjects& movedObjects )
{
    generateStructure( transactions, unparented, movedObjects );
    generateScheduling( transactions );
    generateMemory( transactions );
    m_iterator = m_iteratorEnd;
}

} // namespace mega::network