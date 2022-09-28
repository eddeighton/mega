


#ifndef IO_GENERICS_MAR_25_2022
#define IO_GENERICS_MAR_25_2022

#include "object.hpp"

#include <array>
#include <iterator>

namespace mega
{
namespace io
{
    template < typename Iterator >
    struct Range
    {
        using iterator_type = Iterator;
        Iterator _begin, _end;
        Range( Iterator _begin, Iterator _end )
            : _begin( _begin )
            , _end( _end )
        {
        }
        Iterator begin() const { return _begin; }
        Iterator end() const { return _end; }

        bool operator=( const Range& cmp ) const
        {
            return _begin == cmp._begin && _end == cmp._end;
        }
    };

} // namespace io
} // namespace mega

#endif // IO_GENERICS_MAR_25_2022
