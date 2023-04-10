
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

#ifndef GUARD_2023_April_10_status_printer
#define GUARD_2023_April_10_status_printer

#include "service/protocol/common/status.hpp"

#include <vector>
#include <ostream>

namespace mega::utilities
{

class StatusPrinter
{
public:
    struct Config
    {
        bool m_bConversations   = false;
        bool m_bMemory          = false;
        bool m_bLocks           = false;
        bool m_bLog             = false;
    };
    StatusPrinter( Config config );

    void print( const network::Status& status, std::ostream& os );

private:
    void printNodeInfo( const network::Status& status, std::ostream& os );
    void printNode( const network::Status& status, std::ostream& os, bool bLast );

    std::ostream& print( std::ostream& os, int iIndent, char c ) const;
    std::ostream& line( std::ostream& os, int iIndent = 0 ) const;
    std::ostream& dash( std::ostream& os, int iIndent = 0 ) const;

    std::vector< int > m_stack;
    std::vector< bool > m_stackLast;
    Config m_config;
};

} // namespace mega::utilities

#endif // GUARD_2023_April_10_status_printer
