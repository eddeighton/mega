//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
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

#ifndef EG_EVENT
#define EG_EVENT

#include "common.hpp"

namespace mega
{
// an event is effectively a type erased reference
// it is NOT possible to invoke on an event in c++
struct Event
{
    inline Event() {}

    inline Event( const reference& from ) { data = from; }

    template < class T >
    inline Event( const T& from )
    {
        data = from.data;
    }

    template < class T >
    inline Event& operator=( const T& from )
    {
        data = from.data;
        return *this;
    }

    template < typename TComp >
    inline bool operator==( const TComp& cmp ) const
    {
        return data == cmp.data;
    }

    template < typename TComp >
    inline bool operator!=( const TComp& cmp ) const
    {
        return !( data == cmp.data );
    }

    template < typename TComp >
    inline bool operator<( const TComp& cmp ) const
    {
        return data < cmp.data;
    }

    inline operator const void*() const
    {
        if ( data.is_valid() )
        {
            return reinterpret_cast< const void* >( &data );
        }
        else
        {
            return nullptr;
        }
    }

    reference data;
};
} // namespace mega

using Event = mega::Event;

struct RawEvent
{
    const char*          type;
    mega::AddressStorage address;
    const void*          value;
    std::size_t          size;
};

struct events
{
    static mega::event_iterator getIterator();
    static bool                 get( mega::event_iterator& iterator, Event& event );
    static bool                 get( mega::event_iterator& iterator, RawEvent& event );
    static void put( const char* type, mega::AddressStorage address, const void* value, std::size_t size );
    static bool update(); // just updates the shared memory iterator but also returns if new events
};

#endif // EG_EVENT