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



#ifndef EG_EVENT
#define EG_EVENT

#include "mega/values/runtime/pointer.hpp"

namespace mega
{
// an event is effectively a type erased Pointer
// it is NOT possible to invoke on an event in c++
struct Event : public mega::Pointer
{
    inline Event() = default;

    inline Event( const Pointer& from ) : mega::Pointer( from ) {}

    template < class T >
    inline Event( const T& from ) : mega::Pointer( from ) {}

    template < class T >
    inline auto operator=( const T& from ) -> Event&
    {
        this->mega::Pointer::operator=( from );
        return *this;
    }

    template < typename TComp >
    inline bool operator==( const TComp& cmp ) const
    {
        return this->mega::Pointer::operator==( cmp );
    }

    template < typename TComp >
    inline bool operator!=( const TComp& cmp ) const
    {
        return !this->mega::Pointer::operator==( cmp );
    }

    template < typename TComp >
    inline bool operator<( const TComp& cmp ) const
    {
        return this->mega::Pointer::operator<( cmp );
    }

    inline operator const void*() const
    {
        return valid() ? this : nullptr;
    }
};
} // namespace mega

using Event = mega::Event;

#endif // EG_EVENT