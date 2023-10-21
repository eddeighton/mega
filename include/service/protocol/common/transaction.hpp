
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

#include "mega/values/runtime/reference.hpp"

#include "log/file_log.hpp"

#include <boost/serialization/split_member.hpp>

#include <vector>
#include <array>
#include <string>
#include <utility>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <optional>

namespace mega::network
{

class Transaction
{
public:
    struct Out
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

        inline void push_back( const log::Structure::Read& read )
        {
            log::Structure::DataIO record{ { read.getSource().getNetworkAddress(), read.getTarget().getNetworkAddress(),
                                             read.getRelation(), read.getType() } };
            m_structure.emplace_back( record );
        }
        inline void push_back( const log::Scheduling::Read& read )
        {
            log::Scheduling::DataIO record{ { read.getRef().getNetworkAddress(), read.getType() } };
            m_scheduling.emplace_back( record );
        }
        inline void push_back( const log::Memory::Read& read )
        {
            log::Memory::DataIO record{ { read.getRef().getNetworkAddress() }, std::string{ read.getData() } };
            m_memory.emplace_back( record );
        }
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
    inline void save( Archive& ar, const unsigned int ) const
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
    inline void load( Archive& ar, const unsigned int )
    {
        VERIFY_RTE( m_in.has_value() );
        ar&* m_in;
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()
};

class TransactionProducer
{
public:
    using MPOTransactions = std::map< MPO, Transaction::Out >;
    using UnparentedSet   = std::unordered_set< reference, reference::Hash >;
    using MovedObjects    = std::unordered_map< MPO, std::pair< reference, reference >, MPO::Hash >;

    TransactionProducer( mega::log::FileStorage& log );

    void generateStructure( MPOTransactions& transactions, UnparentedSet& unparented, MovedObjects& movedObjects );
    void generateScheduling( MPOTransactions& transactions );
    void generateMemory( MPOTransactions& transactions );
    void generate( MPOTransactions& transactions, UnparentedSet& unparented, MovedObjects& movedObjects );

private:
    mega::log::FileStorage&       m_log;
    const mega::log::IndexRecord& m_iteratorEnd;
    mega::log::IndexRecord        m_iterator;
};

} // namespace mega::network

#endif // GUARD_2022_October_14_transaction
