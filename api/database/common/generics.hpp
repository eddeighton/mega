
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
