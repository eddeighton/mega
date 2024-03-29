
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

#include "event/file_log.hpp"

namespace mega::network
{

TransactionProducer::TransactionProducer( mega::event::FileStorage& log )
    : m_log( log )
    , m_iteratorEnd( m_log.getIterator() )
    , m_iterator( m_log.getIterator() )
{
}
void TransactionProducer::generateStructure( MPOTransactions& transactions, UnparentedSet&, MovedObjects& movedObjects )
{
    using RecordType                          = event::Structure::Read;
    event::FileIterator< RecordType > iter    = m_log.begin< RecordType >( m_iterator );
    event::FileIterator< RecordType > iterEnd = m_log.begin< RecordType >( m_iteratorEnd );
    for( ; iter != iterEnd; ++iter )
    {
        const RecordType r = *iter;
        switch( r.getType() )
        {
            case event::Structure::eConstruct:
            {
                // do nothing
            }
            break;
            case event::Structure::eDestruct:
            {
                // do nothing
            }
            break;
            case event::Structure::eMake:
            {
                transactions[ r.getSource().getMPO() ].push_back( r );
            }
            break;
            case event::Structure::eBreak:
            {
                // ASSERT( r.getSource().valid() );
                // ASSERT( r.getTarget().valid() );

                ASSERT( r.getSource().getMPO() == r.getTarget().getMPO() );
                transactions[ r.getSource().getMPO() ].push_back( r );
                // Only check the target since owning links always own target
                /*if( r.getSource().getRefCount() == 0U )
                {
                    unparented.insert( r.getSource().getObjectAddress() );
                }*/

                THROW_TODO;
                // if( r.getTarget().getRefCount() == 0U )
                // {
                //     unparented.insert( r.getTarget().getObjectAddress() );
                // }
            }
            break;
            case event::Structure::eMove:
            {
                // transactions[ r.getSource().getMPO() ].push_back( r );

                // prevent locally deleting the object
                // unparented.erase( r.getSource().getObjectAddress() );
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
void TransactionProducer::generateEvent( MPOTransactions& transactions )
{
    using RecordType                          = event::Event::Read;
    event::FileIterator< RecordType > iter    = m_log.begin< RecordType >( m_iterator );
    event::FileIterator< RecordType > iterEnd = m_log.begin< RecordType >( m_iteratorEnd );
    for( ; iter != iterEnd; ++iter )
    {
        const RecordType r = *iter;
        transactions[ r.getRef().getMPO() ].push_back( r );
    }
}
void TransactionProducer::generateTransition( MPOTransactions& transactions )
{
    using RecordType                          = event::Transition::Read;
    event::FileIterator< RecordType > iter    = m_log.begin< RecordType >( m_iterator );
    event::FileIterator< RecordType > iterEnd = m_log.begin< RecordType >( m_iteratorEnd );
    for( ; iter != iterEnd; ++iter )
    {
        const RecordType r = *iter;
        transactions[ r.getRef().getMPO() ].push_back( r );
    }
}
void TransactionProducer::generateMemory( MPOTransactions& transactions )
{
    using RecordType                          = event::Memory::Read;
    event::FileIterator< RecordType > iter    = m_log.begin< RecordType >( m_iterator );
    event::FileIterator< RecordType > iterEnd = m_log.begin< RecordType >( m_iteratorEnd );
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
    generateEvent( transactions );
    generateTransition( transactions );
    generateMemory( transactions );
    m_iterator = m_iteratorEnd;
}

} // namespace mega::network