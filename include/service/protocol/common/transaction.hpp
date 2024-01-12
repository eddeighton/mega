
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

#include "mega/values/runtime/pointer.hpp"

#include "event/file_log.hpp"

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
        std::vector< event::Structure::DataIO >  m_structure;
        std::vector< event::Memory::DataIO >     m_memory;
        std::vector< event::Event::DataIO >      m_event;
        std::vector< event::Transition::DataIO > m_transition;

        template < typename Archive >
        inline void serialize( Archive& ar, const unsigned int )
        {
            ar& m_structure;
            ar& m_memory;
            ar& m_event;
            ar& m_transition;
        }

        inline void push_back( const event::Structure::Read& read )
        {
            THROW_TODO; // DataIO contains PointerHeap - how to convert to PointerNet
            // if DataIO is generated
            event::Structure::DataIO record{ { read.getSource(),
                                               read.getTarget(), read.getRelation(),
                                               read.getType() } };
            m_structure.emplace_back( record );
        }
        inline void push_back( const event::Memory::Read& read )
        {
            event::Memory::DataIO record{ { read.getRef() }, std::string{ read.getData() } };
            m_memory.emplace_back( record );
        }
        inline void push_back( const event::Event::Read& read )
        {
            event::Event::DataIO record{ { read.getRef(), read.getType() } };
            m_event.emplace_back( record );
        }
        inline void push_back( const event::Transition::Read& read )
        {
            event::Transition::DataIO record{ { read.getRef() } };
            m_transition.emplace_back( record );
        }
    };

    std::optional< Out > m_out;

    struct In
    {
        std::vector< event::Structure::DataIO >  m_structure;
        std::vector< event::Memory::DataIO >     m_memory;
        std::vector< event::Event::DataIO >      m_event;
        std::vector< event::Transition::DataIO > m_transition;

        template < typename Archive >
        inline void serialize( Archive& ar, const unsigned int )
        {
            ar& m_structure;
            ar& m_memory;
            ar& m_event;
            ar& m_transition;
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
    using MPOTransactions = std::map< runtime::MPO, Transaction::Out >;
    using UnparentedSet   = std::unordered_set< runtime::PointerHeap, runtime::PointerHeap::Hash >;
    using MovedObjects
        = std::unordered_map< runtime::MPO, std::pair< runtime::PointerHeap, runtime::PointerHeap >, runtime::MPO::Hash >;

    TransactionProducer( mega::event::FileStorage& log );

    void generateStructure( MPOTransactions& transactions, UnparentedSet& unparented, MovedObjects& movedObjects );
    void generateScheduling( MPOTransactions& transactions );
    void generateEvent( MPOTransactions& transactions );
    void generateTransition( MPOTransactions& transactions );
    void generateMemory( MPOTransactions& transactions );
    void generate( MPOTransactions& transactions, UnparentedSet& unparented, MovedObjects& movedObjects );

private:
    mega::event::FileStorage&       m_log;
    const mega::event::IndexRecord& m_iteratorEnd;
    mega::event::IndexRecord        m_iterator;
};

} // namespace mega::network

#endif // GUARD_2022_October_14_transaction
