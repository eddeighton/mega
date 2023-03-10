
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

#ifndef GUARD_2022_September_30_event_log
#define GUARD_2022_September_30_event_log

#include "record.hpp"

#include <boost/circular_buffer/space_optimized.hpp>

#include <string>
#include <iterator>
#include <vector>

namespace mega
{

class EventLog
{
public:
    using EventQueue = boost::circular_buffer_space_optimized< Record, std::allocator< Record > >;

    class Iterator
    {
    public:
        using UnderlyingIterator = EventQueue::const_iterator;

        //UnderlyingIterator m_iterator;

        operator const Record& () const;

        Iterator& operator++();
        Iterator& operator++(int);
    };

    static Iterator getIterator();
    static Iterator getIterator( TimeStamp timestamp );

    static void log( const std::string& strLogMsg );
    static void warn( const std::string& strLogMsg );
    static void error( const std::string& strLogMsg );
};

} // namespace mega

#endif // GUARD_2022_September_30_event_log
