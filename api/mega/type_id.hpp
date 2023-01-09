
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

#ifndef GUARD_2023_January_09_type_id
#define GUARD_2023_January_09_type_id

#include "mega/native_types.hpp"

namespace mega
{

using TypeID = I16;
/*
class TypeID
{
    using Storage = I16;
    union
    {
        struct
        {
            I16 m_object_type_id : 10, m_part_type_id : 6;
        };
        I16 m_storage;
    };

public:
    constexpr operator I16() const { return m_storage; }

    TypeID() = default;

    constexpr explicit TypeID( Storage value )
        : m_storage( value )
    {
    }

    constexpr explicit TypeID( I16 objectTypeID, I16 partTypeID )
        : m_object_type_id( objectTypeID )
        , m_part_type_id( partTypeID )
    {
    }

    constexpr TypeID( const TypeID& cpy )            = default;
    constexpr TypeID( TypeID&& cpy )                 = default;
    constexpr TypeID& operator=( const TypeID& cpy ) = default;
    constexpr TypeID& operator=( Storage value )
    {
        m_storage = value;
        return *this;
    }

    constexpr inline bool operator==( TypeID cmp ) const { return m_storage == cmp.m_storage; }
    constexpr inline bool operator!=( TypeID cmp ) const { return m_storage != cmp.m_storage; }
    constexpr inline bool operator<( TypeID cmp ) const { return m_storage < cmp.m_storage; }

    constexpr inline bool operator==( Storage cmp ) const { return m_storage == cmp; }
    constexpr inline bool operator!=( Storage cmp ) const { return m_storage != cmp; }
    constexpr inline bool operator<( Storage cmp ) const { return m_storage < cmp; }

    constexpr inline I16 getObjectTypeID() const { return m_object_type_id; }
    constexpr inline I16 getPartTypeID() const { return m_part_type_id; }
    constexpr inline I16 get() const { return m_storage; }
};

static constexpr TypeID ROOT_TYPE_ID{ 1, 0 };*/

static constexpr TypeID ROOT_TYPE_ID{ 1 };

} // namespace mega

#endif // GUARD_2023_January_09_type_id
