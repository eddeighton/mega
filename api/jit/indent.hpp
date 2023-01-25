
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

#ifndef GUARD_2023_January_25_indent
#define GUARD_2023_January_25_indent

#include <ostream>

namespace mega
{
    struct Indent
    {
        int        amt = 1;
        inline int operator++()
        {
            ++amt;
            return amt;
        };
        inline int operator--()
        {
            --amt;
            return amt;
        };
        inline operator int() const { return amt; }
    };

    inline std::ostream& operator<<( std::ostream& os, const Indent& indent )
    {
        static const std::string indentation( "    " );
        for( int i = 0; i != indent; ++i )
            os << indentation;
        return os;
    }
}

#endif //GUARD_2023_January_25_indent
