
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

#include "mega/values/native_types.hpp"
#include "mega/values/compilation/concrete/type_id_instance.hpp"

#include "common/assert_verify.hpp"

namespace mega
{

struct LogicalObject
{
    U64              id;
    concrete::TypeID type;

    inline concrete::TypeID getType() const { return type; }

    struct Hash
    {
        inline U64 operator()( const LogicalObject& ref ) const noexcept
        {
            return ref.id + concrete::TypeID::Hash()( ref.type );
        }
    };

    inline bool operator==( const LogicalObject& cmp ) const { return ( id == cmp.id ) && ( type == cmp.type ); }
};

struct LogicalPointer
{
    U64                      id;
    concrete::TypeIDInstance typeInstance;

    inline concrete::TypeIDInstance getTypeInstance() const { return typeInstance; }

    struct Hash
    {
        inline U64 operator()( const LogicalPointer& ref ) const noexcept
        {
            return ref.id + concrete::TypeIDInstance::Hash()( ref.typeInstance );
        }
    };

    inline bool operator==( const LogicalPointer& cmp ) const
    {
        return ( id == cmp.id ) && ( typeInstance == cmp.typeInstance );
    }

    static inline LogicalPointer make( const LogicalObject&            logicalObject,
                                       const concrete::TypeIDInstance& typeInstance )
    {
        return { logicalObject.id, typeInstance };
    }

    static inline LogicalObject toLogicalObject( const LogicalPointer& )
    {
        THROW_TODO;
        // return { ref.id, TypeID::make_object_from_typeID( ref.typeInstance.type ) };
        UNREACHABLE;
    }
};

} // namespace mega

#endif // GUARD_2023_September_02_logical_reference
