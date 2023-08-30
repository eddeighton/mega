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

#include "mega/reference.hpp"

#include <gtest/gtest.h>

#include <vector>

using TraversalFunction = void ( * )( void* pIterator );

struct ReferenceID
{
    mega::U64    id;
    mega::TypeID type;
};
// using ReferenceIDVector = std::vector< ReferenceID >;

using DomainIteration = mega::U64;

struct StackFrame
{
    ReferenceID id;

    // domain iteration is store in the parent element of the domain
    DomainIteration iteration;
};

using Stack = std::vector< StackFrame >;

struct ReadVisitor
{
    ReferenceID start()
    {
        // get the initial referenceID
        return {};
    }

    void* getDimension( ReferenceID id, const mega::TypeID& typeID )
    {
        void* pTempData = nullptr;

        // use program level JIT function to query for dimension
        mega::reference ref{}; // make ref from current object and typeID;
        // void* pData = getDimension( ref );

        // convert to visitor datatype in temporary memory

        return pTempData;
    }

    std::optional< ReferenceID > onLink( const StackFrame& current, const mega::TypeID& linkType )
    {
        // determine the next object

        ReferenceID result{};

        return result;
    }
};

struct WriteVisitor
{
    void objectBegin( ReferenceID ref )
    {
    }

    void objectEnd( ReferenceID ref )
    {
    }

    void linkBegin( ReferenceID link, ReferenceID object, DomainIteration index )
    {
    }
};

struct Iterator
{
    mega::TypeID      m_typeID;
    Stack&            m_stack;
    TraversalFunction m_pTraversalFunction;
    ReadVisitor*      m_pReadVisitor;
    WriteVisitor*     m_pWriteVisitor;
    bool              m_atEnd = false;

    // pre increment
    const Iterator& operator++()
    {
        m_pTraversalFunction( reinterpret_cast< void* >( this ) );
        return *this;
    }

    inline operator void*() const { return m_atEnd ? nullptr : ( void* )this; }

    void* operator*() const { return m_pReadVisitor->getDimension( m_stack.back().id, m_typeID ); }
};


// JIT compiled traversal function
void traversal( void* pIterator )
{
    Iterator* pIter = reinterpret_cast< Iterator* >( pIterator );

    // pIter->m_pReadVisitor
    switch( pIter->m_typeID )
    {
        // object
        case mega::TypeID::ValueType{ 0x00010000 }:
        {
            pIter->m_pWriteVisitor->objectBegin( pIter->m_stack.back().id );

            // go to next
            pIter->m_typeID = mega::TypeID{ 0x00010002 };
        }
        break;
        // dimension case
        case mega::TypeID::ValueType{ 0x00010001 }:
        {
            // go to next
            pIter->m_typeID = mega::TypeID{ 0x00010002 };
        }
        break;
        // link case
        case mega::TypeID::ValueType{ 0x00010002 }:
        {
            // branch to the object
            std::optional< ReferenceID > linkedObject
                = pIter->m_pReadVisitor->onLink( pIter->m_stack.back(), mega::TypeID{ 0x00010002 } );

            if( !linkedObject.has_value() )
            {
                // transition PAST the link
                pIter->m_typeID = mega::TypeID{ 0x00010003 };
            }
            else
            {
                // increment the domain iteration
                pIter->m_stack.back().iteration++;
                pIter->m_stack.push_back( StackFrame{ linkedObject.value(), 0 } );

                // transition INTO link
                pIter->m_typeID = linkedObject.value().type;
            }
        }
        break;
        // case of being at last element of object
        case mega::TypeID::ValueType{ 0x00010003 }:
        {
            // cannot be empty
            // ASSERT( !pIter->m_stack.empty() );
            pIter->m_pWriteVisitor->objectEnd( pIter->m_stack.back().id );

            // pop the stack
            pIter->m_stack.pop_back();

            if( pIter->m_stack.empty() )
            {
                // reached the end
                pIter->m_atEnd = true;
            }
            else
            {
                pIter->m_typeID = pIter->m_stack.back().id.type;
            }
        }
        break;
        default:
        {
        }
        break;
    }
}

void program_traverse( void* pIterator )
{
    Iterator* pIter = reinterpret_cast< Iterator* >( pIterator );

    if( pIter->m_atEnd )
        return;

    switch( pIter->m_typeID.getObjectID() )
    {
        case 0x0001:
        {
            // get JIT traversal for object
            traversal( pIterator );
        }
        break;
        default:
        {
            //
        }
        break;
    }
}

void visit( ReadVisitor& read, WriteVisitor& write )
{
    Stack stack;

    ReferenceID startRef = read.start();

    StackFrame initial{ startRef, 0 };
    stack.push_back( initial );

    Iterator iterator{ startRef.type, stack, nullptr, &read, &write, false };
    while( iterator )
    {
        ++iterator;
    }
}

TEST( Visitor, Basic )
{
}