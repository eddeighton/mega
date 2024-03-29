
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
#include "mega/logical_tree.hpp"
#include "mega/values/runtime/any.hpp"
#include "mega/values/runtime/any_io.hpp"

#include <boost/algorithm/string.hpp>

#include <ostream>
#include <iomanip>

namespace mega
{

class LogicalTreePrinter
{
    std::ostream& os;
    std::string   indent;

    void push()
    {
        indent.push_back( ' ' );
        indent.push_back( ' ' );
    }
    void pop()
    {
        indent.pop_back();
        indent.pop_back();
    }

    inline std::string lastType( const char* pszType )
    {
        std::string s( pszType );
        auto        iFind = std::find( s.rbegin(), s.rend(), ':' );
        return std::string( iFind.base(), s.end() );
    }

    void line( const char* pszMetaType, const char* pszType, const LogicalPointer& ref )
    {
        os << ref.typeInstance << indent << pszMetaType << ": " << lastType( pszType );
    }

public:
    LogicalTreePrinter( std::ostream& os_ )
        : os( os_ )
    {
    }

    void on_object_start( const char* pszType, const LogicalPointer& ref )
    {
        line( "obj", pszType, ref );
        os << "\n";
        push();
    }
    void on_object_end( const char*, const LogicalPointer& ) { pop(); }
    void on_component_start( const char* pszType, const LogicalPointer& ref )
    {
        line( "com", pszType, ref );
        os << "\n";
        push();
    }
    void on_component_end( const char*, const LogicalPointer& ) { pop(); }
    void on_action_start( const char* pszType, const LogicalPointer& ref )
    {
        line( "act", pszType, ref );
        os << "\n";
        push();
    }
    void on_action_end( const char*, const LogicalPointer& ) { pop(); }
    void on_state_start( const char* pszType, const LogicalPointer& ref )
    {
        line( "sta", pszType, ref );
        os << "\n";
        push();
    }
    void on_state_end( const char*, const LogicalPointer& ) { pop(); }
    void on_event_start( const char* pszType, const LogicalPointer& ref )
    {
        line( "eve", pszType, ref );
        os << "\n";
        push();
    }
    void on_event_end( const char*, const LogicalPointer& ) { pop(); }
    void on_link_start( const char* pszType, const LogicalPointer& ref, bool, bool )
    {
        line( "lin", pszType, ref );
        os << "\n";
        push();
    }
    void on_link_end( const char*, const LogicalPointer&, bool, bool ) { pop(); }
    void on_interupt( const char* pszType, const LogicalPointer& ref )
    {
        line( "int", pszType, ref );
        os << "\n";
    }
    void on_function( const char* pszType, const LogicalPointer& ref )
    {
        line( "fun", pszType, ref );
        os << "\n";
    }
    void on_decider( const char* pszType, const LogicalPointer& ref )
    {
        line( "dec", pszType, ref );
        os << "\n";
    }
    void on_namespace( const char* pszType, const LogicalPointer& ref )
    {
        line( "nam", pszType, ref );
        os << "\n";
    }
    void on_dimension( const char* pszType, const LogicalPointer& ref, const mega::runtime::Any& value )
    {
        line( "dim", pszType, ref );
        os << " " << value << "\n";
    }
};
} // namespace mega

#endif // GUARD_2023_September_01_printer
