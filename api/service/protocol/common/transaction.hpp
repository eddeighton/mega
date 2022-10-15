
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

#include "log/record.hpp"

#include <boost/serialization/split_member.hpp>
#include <boost/serialization/array_wrapper.hpp>

#include <vector>
#include <array>
#include <string>
#include <utility>
#include <string_view>
#include <unordered_map>

namespace mega::network
{

class Transaction
{
    using MemoryMap       = std::unordered_map< mega::reference, std::string_view, mega::reference::Hash >;
    using MemoryRecordsIn = std::array< MemoryMap, log::toInt( log::TrackType::TOTAL ) >;

public:
    using SchedulingRecordsIn  = std::vector< log::SchedulerRecordRead >;
    using SchedulingRecordsOut = std::vector< std::pair< reference, log::SchedulerRecord::Type > >;
    using MemoryTrackOut       = std::vector< std::pair< reference, std::string > >;
    using MemoryRecordsOut     = std::array< MemoryTrackOut, log::toInt( log::TrackType::TOTAL ) >;

private:
    SchedulingRecordsIn  m_schedulingRecordsIn;
    SchedulingRecordsOut m_schedulingRecordsOut;

    MemoryRecordsIn  m_memoryRecordsIn;
    MemoryRecordsOut m_memoryRecordsOut;
    bool             m_bLoaded = false;

public:
    const SchedulingRecordsOut& getSchedulingRecords() const { return m_schedulingRecordsOut; }
    const MemoryTrackOut&       getMemoryRecords( log::MemoryTrackType track ) const
    {
        return m_memoryRecordsOut[ log::toInt( log::toTrackType( track ) ) ];
    }

    template < typename Archive >
    void save( Archive& ar, const unsigned int ) const
    {
        // save the input in case this is node creating transaction
        if ( !m_bLoaded )
        {
            ar& m_schedulingRecordsIn.size();
            for ( const auto& s : m_schedulingRecordsIn )
            {
                ar& s.getReference();
                ar& s.getType();
            }

            for ( const auto& map : m_memoryRecordsIn )
            {
                ar& map.size();
                for ( const auto& s : map )
                {
                    ar& s.first;
                    ar& std::string( s.second.begin(), s.second.end() );
                    // ar& boost::serialization::make_array( s.second.data(), s.second.size() );
                }
            }
        }
        else
        {
            // either input or output exists - both serialize to same format for load
            {
                ar& m_schedulingRecordsOut.size();
                for ( const auto& s : m_schedulingRecordsOut )
                {
                    ar& s.first;
                    ar& s.second;
                }
            }
            {
                for ( auto& records : m_memoryRecordsOut )
                {
                    ar& records.size();
                    for ( const auto& s : records )
                    {
                        ar& s.first;
                        ar& s.second;
                    }
                }
            }
        }
    }

    template < typename Archive >
    void load( Archive& ar, const unsigned int )
    {
        m_bLoaded = true;
        {
            U64 size{};
            ar& size;
            m_schedulingRecordsOut.reserve( size );
            m_schedulingRecordsOut.clear();
            for ( U64 i = 0; i != size; ++i )
            {
                std::pair< reference, log::SchedulerRecord::Type > s;
                ar&                                                s.first;
                ar&                                                s.second;
                m_schedulingRecordsOut.push_back( s );
            }
        }
        for ( auto& records : m_memoryRecordsOut )
        {
            U64 size{};
            ar& size;
            records.reserve( size );
            records.clear();
            for ( U64 i = 0; i != size; ++i )
            {
                std::pair< reference, std::string > s;
                ar&                                 s.first;
                ar&                                 s.second;
                records.push_back( s );
            }
        }
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()

    void addSchedulingRecords( const SchedulingRecordsIn& records ) { m_schedulingRecordsIn = records; }

    void addMemoryRecord( const reference& ref, log::MemoryTrackType track, const std::string_view& str_view )
    {
        m_memoryRecordsIn[ log::toInt( log::toTrackType( track ) ) ][ ref ] = str_view;
    }

    void reset()
    {
        m_bLoaded = false;
        m_schedulingRecordsIn.clear();
        m_schedulingRecordsOut.clear();
        for ( auto& m : m_memoryRecordsIn )
        {
            m.clear();
        }
        for ( auto& m : m_memoryRecordsOut )
        {
            m.clear();
        }
    }
};
} // namespace mega::network

#endif // GUARD_2022_October_14_transaction
