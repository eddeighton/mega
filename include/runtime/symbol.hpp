
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

#ifndef GUARD_2023_April_26_orc_symbol
#define GUARD_2023_April_26_orc_symbol

#include "mega/values/runtime/pointer.hpp"
#include "mega/values/compilation/interface/relation_id.hpp"
#include "mega/values/compilation/invocation_id.hpp"

namespace mega::runtime
{
class Symbol
{
    std::string m_symbolName;

public:
    // VStar = void*
    // CVStar = const void*
    // Ref = mega::Pointer
    // RefR = mega::Pointer&
    // RefCR = const mega::Pointer&
    // ID = TypeID
    // U64 = mega::U64
    enum Parameters
    {
        None,
        VStar,
        VStar_VStar,
        ID_VStar_VStar,
        
        Mpo,

        Ref,
        Ref_Ref,
        Ref_Ref_U64,
        Ref_VStar,
        Ref_VStar_Bool,
        Ref_CVStar,
        Ref_VStar_U64,
        Ref_U64,
        Ref_Any,

        RefR,
        RefR_U32R,
        RefR_RefR,
        RefR_VStar,
        RefR_CVStar,
        
        RefCR,
        RefCR_U32R
    };

    Symbol( const char* prefix, Parameters params );
    Symbol( const char* prefix, RelationID relationID, Parameters params );
    Symbol( const char* prefix, mega::TypeID objectTypeID, Parameters params );
    Symbol( const mega::InvocationID& invocationID, Parameters params );
    Symbol( const char* prefix, mega::TypeID objectTypeID, mega::TypeID objectTypeID2 );

    const std::string& get() const { return m_symbolName; }
};
} // namespace mega::runtime

#endif // GUARD_2023_April_26_orc_symbol
