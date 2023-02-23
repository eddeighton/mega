
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

#ifndef GUARD_2023_February_23_variant
#define GUARD_2023_February_23_variant

#include "object_info.hpp"
#include "object_loader.hpp"
#include "data_pointer.hpp"

#include <memory>

namespace data
{

template <>
class EGDB_EXPORT Ptr< void >
{
    using ObjectInfoPtr = std::shared_ptr< mega::io::ObjectInfo >;

public:
    Ptr()
        : m_pLoader( nullptr )
        , m_pObjectInfo( std::make_shared< mega::io::ObjectInfo >() )
    {
    }

    Ptr( ObjectPartLoader& loader )
        : m_pLoader( &loader )
        , m_pObjectInfo( std::make_shared< mega::io::ObjectInfo >() )
    {
    }

    Ptr( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        : m_pLoader( &loader )
        , m_pObjectInfo( std::make_shared< mega::io::ObjectInfo >( objectInfo ) )
    {
    }

    Ptr( Ptr& old, const mega::io::ObjectInfo& objectInfo )
        : m_pLoader( old.m_pLoader )
        , m_pObjectInfo( std::make_shared< mega::io::ObjectInfo >( objectInfo ) )
    {
    }

    Ptr( const Ptr& copy, const Ptr& copy2 )
        : m_pLoader( copy.m_pLoader )
        , m_pObjectInfo( copy.m_pObjectInfo )
        , m_pObjectPart( copy.m_pObjectPart )
    {
    }

    Ptr( const Ptr& copy ) = default;

    // allow Ptr< void > to be converted to from anything
    template < typename TOther >
    Ptr( const Ptr< TOther >& copy )
        : m_pLoader( copy.m_pLoader )
        , m_pObjectInfo( copy.m_pObjectInfo )
        , m_pObjectPart( copy.m_pObjectPart )
    {
    }

    Ptr& operator=( const Ptr& copy )
    {
        if( m_pLoader == nullptr )
        {
            m_pLoader = copy.m_pLoader;
        }
        VERIFY_RTE( m_pLoader == copy.m_pLoader );
        if( this != &copy )
        {
            m_pObjectInfo = copy.m_pObjectInfo;
            m_pObjectPart = copy.m_pObjectPart;
        }
        return *this;
    }

    inline mega::io::ObjectInfo::Type getType() const
    {
        ASSERT( m_pObjectInfo );
        return m_pObjectInfo->getType();
    }
    const mega::io::ObjectInfo& getObjectInfo() const { return *m_pObjectInfo; }

    inline bool operator<( const Ptr& cmp ) const { return getObjectInfo() < cmp.getObjectInfo(); }
    inline bool operator==( const Ptr& cmp ) const { return getObjectInfo() == cmp.getObjectInfo(); }

    inline ObjectPartLoader* dontUseThisGetLoader() { return m_pLoader; }

private:
    ObjectPartLoader* m_pLoader;
    ObjectInfoPtr     m_pObjectInfo;
    mutable void*     m_pObjectPart = nullptr;
};

using Variant = Ptr< void >;

template < typename TVariantType, typename TTo, typename TFrom >
struct EGDB_EXPORT UpCast
{
    inline TVariantType operator()( TFrom& from ) const { return from; }
};

inline Variant to_upper( Variant& from );

} // namespace data

#endif // GUARD_2023_February_23_variant
