
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

#ifndef GUARD_2023_September_01_mpo_visitor
#define GUARD_2023_September_01_mpo_visitor

#include "mega/iterator.hpp"
#include "mega/logical_reference.hpp"
#include "mega/reference.hpp"
#include "mega/any.hpp"

#include "jit/program_functions.hxx"

#include <unordered_map>

namespace mega
{

struct MPORealToLogicalVisitor
{
    mega::reference m_root;

    MPORealToLogicalVisitor( const mega::reference& root )
        : m_root( root )
    {
    }
    MPORealToLogicalVisitor( const MPO& mpo )
        : m_root( mega::reference::make_root( mpo ) )
    {
    }

    using LogicalToReal = std::unordered_map< mega::LogicalObject, mega::reference, mega::LogicalObject::Hash >;
    using RealToLogical = std::unordered_map< mega::reference, mega::LogicalObject, mega::reference::Hash >;
    LogicalToReal m_logicalToReal;
    RealToLogical m_realToLogical;

    inline mega::reference fromLogical( const LogicalReference& logicalRef ) const
    {
        auto iFind = m_logicalToReal.find( LogicalReference::toLogicalObject( logicalRef ) );
        VERIFY_RTE_MSG( iFind != m_logicalToReal.end(), "Failed to locate logical object" );
        return reference::make( iFind->second, logicalRef.typeInstance );
    }

    inline LogicalObject toLogicalObject( const reference& ref )
    {
        const reference objectRef = ref.getObjectAddress();
        auto            iFind     = m_realToLogical.find( objectRef );
        if( iFind != m_realToLogical.end() )
        {
            return iFind->second;
        }
        else
        {
            const LogicalObject logicalObject{ m_realToLogical.size(), objectRef.getType() };

            m_logicalToReal.insert( { logicalObject, objectRef } );
            m_realToLogical.insert( { objectRef, logicalObject } );

            return logicalObject;
        }
    }

    mega::LogicalObject start() { return toLogicalObject( m_root ); }

    U64 linkSize( const LogicalReference& logicalRef, bool bOwning, bool bOwned )
    {
        if( !bOwning )
            return 0U;

        static thread_local mega::runtime::program::LinkSize linkSizeFPtr;
        return linkSizeFPtr( fromLogical( logicalRef ) );
    }

    LogicalObject linkGet( const LogicalReference& logicalRef, U64 index )
    {
        static thread_local mega::runtime::program::LinkGet linkGetFptr;
        return toLogicalObject( linkGetFptr( fromLogical( logicalRef ), index ) );
    }

    Any read( const LogicalReference& logicalRef )
    {
        static thread_local mega::runtime::program::ReadAny readAny;
        return readAny( fromLogical( logicalRef ) );
    }
};

struct MPORealVisitor
{
    mega::reference m_root;

    MPORealVisitor( const mega::reference& root )
        : m_root( root )
    {
    }
    MPORealVisitor( const MPO& mpo )
        : m_root( mega::reference::make_root( mpo ) )
    {
    }

    reference start() { return m_root; }

    U64 linkSize( const reference& ref, bool bOwning, bool bOwned )
    {
        if( !bOwning )
            return 0U;

        static thread_local mega::runtime::program::LinkSize linkSizeFPtr;
        return linkSizeFPtr( ref );
    }

    reference linkGet( const reference& ref, U64 index )
    {
        static thread_local mega::runtime::program::LinkGet linkGetFptr;
        return linkGetFptr( ref, index );
    }

    Any read( const reference& ref )
    {
        static thread_local mega::runtime::program::ReadAny readAny;
        return readAny( ref );
    }
};

template < typename Traversal >
inline void traverse( Traversal& traversal )
{
    static thread_local mega::runtime::program::Traverse programTraverse;

    Iterator iterator( [ &progTraverse = programTraverse ]( void* pIter ) { progTraverse( pIter ); }, traversal );
    while( iterator )
    {
        ++iterator;
    }
}

} // namespace mega

#endif // GUARD_2023_September_01_mpo_visitor
