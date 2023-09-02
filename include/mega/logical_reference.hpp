
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

#ifndef GUARD_2023_September_02_logical_reference
#define GUARD_2023_September_02_logical_reference

#include "mega/native_types.hpp"
#include "mega/type_instance.hpp"

namespace mega
{

struct LogicalObject
{
    U64    id;
    TypeID type;

    struct Hash
    {
        inline U64 operator()( const LogicalObject& ref ) const noexcept { return ref.id + ref.type.getSymbolID(); }
    };

    inline bool operator==( const LogicalObject& cmp ) const { return ( id == cmp.id ) && ( type == cmp.type ); }
};

struct LogicalReference
{
    U64          id;
    TypeInstance typeInstance;

    struct Hash
    {
        inline U64 operator()( const LogicalReference& ref ) const noexcept
        {
            return ref.id + ref.typeInstance.instance + ref.typeInstance.type.getSymbolID();
        }
    };

    inline bool operator==( const LogicalReference& cmp ) const
    {
        return ( id == cmp.id ) && ( typeInstance == cmp.typeInstance );
    }

    static inline LogicalReference make( const LogicalObject& logicalObject, const TypeInstance& typeInstance )
    {
        return { logicalObject.id, typeInstance };
    }

    static inline LogicalObject toLogicalObject( const LogicalReference& ref )
    {
        return { ref.id, TypeID::make_object_from_typeID( ref.typeInstance.type ) };
    }
};

}

#endif //GUARD_2023_September_02_logical_reference
