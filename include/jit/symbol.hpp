
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

#include "mega/reference.hpp"
#include "mega/relation_id.hpp"
#include "mega/invocation_id.hpp"

namespace mega::runtime
{
    class Symbol
    {
        std::string m_symbolName;
    public:
        // VStar = void*
        // CVStar = const void*
        // Ref = mega::reference
        // RefCR = const mega::reference&
        // Wo = WriteOperation
        // ID = TypeID
        // U64 = mega::U64
        enum Parameters
        {
            None,
            VStar,              // "Pv"
            VStar_VStar,        // "PvS_"
            Ref,                // "N4mega9referenceE"
            Ref_Ref,            // "N4mega9referenceES0_"
            Ref_VStar,          // "N4mega9referenceEPv"
            Ref_CVStar,         // "N4mega9referenceEPKv"
            Ref_Wo_RefCR,       // "N4mega9referenceE14WriteOperationRKS0_"
            Ref_Wo_CVStar,      // "N4mega9referenceE14WriteOperationRKS0_"
            ID_VStar_VStar,     // "N4mega6TypeIDEPvS1_"
            Ref_VStar_U64,      // "N4mega9referenceEPvm"
            Ref_U64,            // "N4mega9referenceEm"
            Ref_Any             // "N4mega9referenceEm"
        };

        Symbol( const char* prefix, Parameters params );
        Symbol( const char* prefix, RelationID relationID, Parameters params );
        Symbol( const char* prefix, mega::TypeID objectTypeID, Parameters params );
        Symbol( const mega::InvocationID& invocationID, Parameters params );
        Symbol( const char* prefix, mega::TypeID objectTypeID, mega::TypeID objectTypeID2 );
        
        const std::string& get() const { return m_symbolName; }
    };
}

#endif //GUARD_2023_April_26_orc_symbol



