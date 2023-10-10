
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

#ifndef GUARD_2023_September_07_tree_visitor
#define GUARD_2023_September_07_tree_visitor

#include "mega/any.hpp"

namespace mega
{

template< typename ReferenceType >
struct TreeVisitor
{
    using Reference = ReferenceType;

    void on_object_start( const char* pszType, const Reference& ref )
    {
    }
    void on_object_end( const char* pszType, const Reference& ref )
    {
    }
    void on_component_start( const char* pszType, const Reference& ref )
    {
    }
    void on_component_end( const char* pszType, const Reference& ref )
    {
    }
    void on_action_start( const char* pszType, const Reference& ref )
    {
    }
    void on_action_end( const char* pszType, const Reference& ref )
    {
    }
    void on_state_start( const char* pszType, const Reference& ref )
    {
    }
    void on_state_end( const char* pszType, const Reference& ref )
    {
    }
    void on_event_start( const char* pszType, const Reference& ref )
    {
    }
    void on_event_end( const char* pszType, const Reference& ref )
    {
    }
    void on_link_start( const char* pszType, const Reference& ref, bool bOwning, bool bOwned )
    {
    }
    void on_link_end( const char* pszType, const Reference& ref, bool bOwning, bool bOwned )
    {
    }
    void on_interupt( const char* pszType, const Reference& ref )
    {
    }
    void on_function( const char* pszType, const Reference& ref )
    {
    }
    void on_namespace( const char* pszType, const Reference& ref )
    {
    }
    void on_dimension( const char* pszType, const Reference& ref, const mega::Any& value )
    {
    }
};

}

#endif //GUARD_2023_September_07_tree_visitor
