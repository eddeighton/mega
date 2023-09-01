
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

#ifndef GUARD_2023_September_01_printer
#define GUARD_2023_September_01_printer

#include "mega/iterator.hpp"
#include "mega/reference_io.hpp"

#include <ostream>

namespace mega
{

class LogicalTreePrinter
{
    std::ostream& os;
    std::string indent;

    void push() { indent.push_back(' ');indent.push_back(' '); }
    void pop() { indent.pop_back(); indent.pop_back(); }

public:
    LogicalTreePrinter( std::ostream& os ) : os( os ) {}

    void on_object_start( const LogicalReference& ref )
    {
        using ::operator<<;
        os << indent << "object: " << ref.id << " " << ref.typeInstance << std::endl;
        push();
    }
    void on_object_end( const LogicalReference& ref )
    {
        pop();
    }
    void on_action_start( const LogicalReference& ref )
    {
        using ::operator<<;
        os << indent << "action: " << ref.id << " " << ref.typeInstance << std::endl;
        push();
    }
    void on_action_end( const LogicalReference& ref )
    {
        pop();
    }
    void on_event_start( const LogicalReference& ref )
    {
        using ::operator<<;
        os << indent << "event: " << ref.id << " " << ref.typeInstance << std::endl;
        push();
    }
    void on_event_end( const LogicalReference& ref )
    {
        pop();
    }
    void on_link_start( const LogicalReference& ref )
    {
        using ::operator<<;
        os << indent << "link: " << ref.id << " " << ref.typeInstance << std::endl;
        push();
    }
    void on_link_end( const LogicalReference& ref )
    {
        pop();
    }
    void on_interupt( const LogicalReference& ref )
    {
        using ::operator<<;
        os << indent << "interupt: " << ref.id << " " << ref.typeInstance << std::endl;
    }
    void on_function( const LogicalReference& ref )
    {
        using ::operator<<;
        os << indent << "function: " << ref.id << " " << ref.typeInstance << std::endl;
    }
    void on_namespace( const LogicalReference& ref )
    {
        using ::operator<<;
        os << indent << "namespace: " << ref.id << " " << ref.typeInstance << std::endl;
    }
    void on_dimension( const LogicalReference& ref, void* pData )
    {
        using ::operator<<;
        os << indent << "dimension: " << ref.id << " " << ref.typeInstance << std::endl;
    }
};
}

#endif //GUARD_2023_September_01_printer
