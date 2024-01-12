
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
#include "mega/logical_pointer.hpp"
#include "mega/values/runtime/pointer.hpp"
#include "mega/values/runtime/any.hpp"

// #include "jit/program_functions.hxx"

#include <unordered_map>

namespace mega
{
/*
struct MPORealToLogicalVisitor
{
    mega::runtime::Pointer m_root;

    MPORealToLogicalVisitor( const mega::runtime::Pointer& root )
        : m_root( root )
    {
    }
    MPORealToLogicalVisitor( const runtime::MPO& mpo )
        : m_root( mega::runtime::Pointer::make_root( mpo ) )
    {
    }

    using LogicalToReal = std::unordered_map< mega::LogicalObject, mega::runtime::Pointer, mega::LogicalObject::Hash >;
    using RealToLogical = std::unordered_map< mega::runtime::Pointer, mega::LogicalObject, mega::runtime::Pointer::Hash >;
    LogicalToReal m_logicalToReal;
    RealToLogical m_realToLogical;

    inline mega::runtime::Pointer fromLogical( const LogicalPointer& logicalRef ) const
    {
        auto iFind = m_logicalToReal.find( LogicalPointer::toLogicalObject( logicalRef ) );
        VERIFY_RTE_MSG( iFind != m_logicalToReal.end(), "Failed to locate logical object" );
        return Pointer::make( iFind->second, logicalRef.typeInstance );
    }

    inline LogicalObject toLogicalObject( const runtime::Pointer& ref )
    {
        const runtime::Pointer objectRef = ref.getObjectAddress();
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

    U64 linkSize( const LogicalPointer& logicalRef, bool bOwning, bool bOwned )
    {
        if( !bOwning )
            return 0U;

        THROW_TODO;
        // static thread_local mega::runtime::program::LinkSize linkSizeFPtr;
        //return linkSizeFPtr( fromLogical( logicalRef ) );
    }

    LogicalObject linkGet( const LogicalPointer& logicalRef, U64 index )
    {
        THROW_TODO;
        // static thread_local mega::runtime::program::LinkGet linkGetFptr;
        //return toLogicalObject( linkGetFptr( fromLogical( logicalRef ), index ) );
    }

    Any read( const LogicalPointer& logicalRef )
    {
        THROW_TODO;
        // static thread_local mega::runtime::program::ReadAny readAny;
        //return readAny( fromLogical( logicalRef ) );
    }
};

struct MPORealVisitor
{
    mega::Pointer m_root;

    MPORealVisitor( const mega::Pointer& root )
        : m_root( root )
    {
    }
    MPORealVisitor( const MPO& mpo )
        : m_root( mega::Pointer::make_root( mpo ) )
    {
    }

    Pointer start() { return m_root; }

    U64 linkSize( const Pointer& ref, bool bOwning, bool bOwned )
    {
        if( !bOwning )
            return 0U;

        THROW_TODO;
        // static thread_local mega::runtime::program::LinkSize linkSizeFPtr;
        // return linkSizeFPtr( ref );
    }

    Pointer linkGet( const Pointer& ref, U64 index )
    {
        THROW_TODO;
        // static thread_local mega::runtime::program::LinkGet linkGetFptr;
        // return linkGetFptr( ref, index );
    }

    Any read( const Pointer& ref )
    {
        THROW_TODO;
        // static thread_local mega::runtime::program::ReadAny readAny;
        // return readAny( ref );
    }
};

template < typename Traversal >
inline void traverse( Traversal& traversal )
{
    THROW_TODO;
    // static thread_local mega::runtime::program::Traverse programTraverse;

    // Iterator iterator( [ &progTraverse = programTraverse ]( void* pIter ) { progTraverse( pIter ); }, traversal );
    // while( iterator )
    // {
    //     ++iterator;
    // }
}
*/
} // namespace mega

#endif // GUARD_2023_September_01_mpo_visitor
