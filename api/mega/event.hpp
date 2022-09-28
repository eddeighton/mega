

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
    mega::U64          size;
};

struct events
{
    static mega::event_iterator getIterator();
    static bool                 get( mega::event_iterator& iterator, Event& event );
    static bool                 get( mega::event_iterator& iterator, RawEvent& event );
    static void put( const char* type, mega::AddressStorage address, const void* value, mega::U64 size );
    static bool update(); // just updates the shared memory iterator but also returns if new events
};

#endif // EG_EVENT