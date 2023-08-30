
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

#ifndef GUARD_2023_August_30_iterator
#define GUARD_2023_August_30_iterator

#include "mega/type_id.hpp"
#include "mega/type_instance.hpp"

#include <optional>
#include <vector>

namespace mega
{

struct ReferenceID
{
    U64          objectID;
    TypeInstance typeInstance;
};

using DomainIteration = U64;

struct StackFrame
{
    ReferenceID     id;
    DomainIteration iteration, total;
};

struct IReadVisitor
{
    virtual void*                        getDimension( const ReferenceID& id, const TypeID& dimensionType ) = 0;
    virtual DomainIteration              getDomainSize( const ReferenceID& id )                             = 0;
    virtual std::optional< ReferenceID > onLink( const StackFrame& current, const TypeID& linkType )        = 0;
};

struct IWriteVisitor
{
    virtual void objectStart( const ReferenceID& id ) = 0;
    virtual void actionStart( const ReferenceID& id ) = 0;
};

class Iterator
{
    using Stack             = std::vector< StackFrame >;
    using TraversalFunction = void ( * )( void* pIterator );

    mega::TypeID      m_state; // can be negative for END state
    Stack             m_stack;
    TraversalFunction m_pTraversalFunction;
    IReadVisitor&     m_read;
    IWriteVisitor&    m_write;
    bool              m_atEnd = false;

public:
    inline Iterator( StackFrame&& start, TraversalFunction pTraversal, IReadVisitor& read, IWriteVisitor& write )
        : m_state( start.id.typeInstance.type )
        , m_stack{ std::move( start ) }
        , m_pTraversalFunction( pTraversal )
        , m_read( read )
        , m_write( write )
    {
    }

    // pre increment
    inline const Iterator& operator++()
    {
        m_pTraversalFunction( reinterpret_cast< void* >( this ) );
        return *this;
    }

    inline       operator void*() const { return m_atEnd ? nullptr : ( void* )this; }
    inline void* operator*() const { return m_read.getDimension( m_stack.back().id, m_state ); }

    inline TypeID getTypeID() const { return m_state; }

    inline void object_start( mega::TypeID successor )
    {
        m_write.objectStart( m_stack.back().id );
        m_state = successor;
    }
    inline void object_end()
    {
        m_stack.pop_back();
        if( m_stack.empty() )
        {
            m_atEnd = true;
        }
        else
        {
            m_state = m_stack.back().id.typeInstance.type;
        }
    }
    inline void action_start( mega::TypeID successor )
    {
        // determine the type instance from stack
        TypeInstance typeInstance;

        StackFrame stackFrame{ ReferenceID{ m_stack.back().id.objectID, typeInstance }, 0 };
        m_stack.emplace_back( std::move( stackFrame ) );

        // m_write.actionStart( m_stack.back().id );
        m_state = successor;
    }
    inline void action_end( mega::TypeID successor )
    {
        m_stack.pop_back();

        m_state = successor;
    }
    inline void event_start( mega::TypeID successor ) {}
    inline void event_end( mega::TypeID successor ) {}
    inline void link_start( mega::TypeID successor ) {}
    inline void link_end( mega::TypeID successor ) {}
    inline void interupt_start( mega::TypeID successor ) {}
    inline void interupt_end( mega::TypeID successor ) {}
    inline void function_start( mega::TypeID successor ) {}
    inline void function_end( mega::TypeID successor ) {}
    inline void namespace_start( mega::TypeID successor ) {}
    inline void namespace_end( mega::TypeID successor ) {}
    inline void dimension( mega::TypeID successor ) {}
};

} // namespace mega

#endif // GUARD_2023_August_30_iterator
