
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

#ifndef GUARD_2022_October_14_transaction
#define GUARD_2022_October_14_transaction

#include "mega/reference.hpp"

#include "log/log.hpp"

#include <boost/serialization/split_member.hpp>

#include <vector>
#include <array>
#include <string>
#include <utility>
#include <string_view>
#include <unordered_map>
#include <map>
#include <optional>

namespace mega::network
{

class Transaction
{
public:
    struct Out
    {
        std::vector< log::Structure::Read >  m_structure;
        std::vector< log::Scheduling::Read > m_scheduling;
        std::vector< log::Memory::Read >     m_memory;

        template < typename Archive >
        inline void serialize( Archive& ar, const unsigned int )
        {
            ar& m_structure;
            ar& m_scheduling;
            ar& m_memory;
        }

        void push_back( const log::Structure::Read& read ) { m_structure.push_back( read ); }
        void push_back( const log::Scheduling::Read& read ) { m_scheduling.push_back( read ); }
        void push_back( const log::Memory::Read& read ) { m_memory.push_back( read ); }
    };

    std::optional< Out > m_out;

    struct In
    {
        std::vector< log::Structure::DataIO >  m_structure;
        std::vector< log::Scheduling::DataIO > m_scheduling;
        std::vector< log::Memory::DataIO >     m_memory;

        template < typename Archive >
        inline void serialize( Archive& ar, const unsigned int )
        {
            ar& m_structure;
            ar& m_scheduling;
            ar& m_memory;
        }
    };

    std::optional< In > m_in;

public:
    Transaction()
        : m_in( In{} )
    {
    }
    Transaction( const Out& out )
        : m_out( out )
    {
    }

    template < typename Archive >
    void save( Archive& ar, const unsigned int ) const
    {
        if( m_out.has_value() )
        {
            ar&* m_out;
        }
        else
        {
            VERIFY_RTE( m_in.has_value() );
            ar&* m_in;
        }
    }

    template < typename Archive >
    void load( Archive& ar, const unsigned int )
    {
        VERIFY_RTE( m_in.has_value() );
        ar&* m_in;
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()
};

class TransactionProducer
{
public:
    TransactionProducer( mega::log::Storage& log )
        : m_log( log )
        , m_iteratorEnd( m_log.getIterator() )
        , m_iterator( m_log.getIterator() )
    {
    }

    using MPOTransactions = std::map< MPO, Transaction::Out >;

    void generateStructure( MPOTransactions& transactions )
    {
        using RecordType                    = log::Structure::Read;
        log::Iterator< RecordType > iter    = m_log.begin< RecordType >( m_iterator );
        log::Iterator< RecordType > iterEnd = m_log.begin< RecordType >( m_iteratorEnd );
        for( ; iter != iterEnd; ++iter )
        {
            const RecordType r = *iter;
            ASSERT( r.getSource().getMPO() == r.getTarget().getMPO() );
            transactions[ r.getSource().getMPO() ].push_back( r );
        }
    }
    void generateScheduling( MPOTransactions& transactions )
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
    void generateMemory( MPOTransactions& transactions )
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

    void generate( MPOTransactions& transactions )
    {
        generateStructure( transactions );
        generateScheduling( transactions );
        generateMemory( transactions );
        m_iterator = m_iteratorEnd;
    }

private:
    mega::log::Storage&           m_log;
    const mega::log::IndexRecord& m_iteratorEnd;
    mega::log::IndexRecord        m_iterator;
};

} // namespace mega::network

#endif // GUARD_2022_October_14_transaction