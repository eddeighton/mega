
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

#ifndef GUARD_2023_September_07_tree_traversal
#define GUARD_2023_September_07_tree_traversal

#include "mega/iterator.hpp"
#include "mega/native_types.hpp"
#include "mega/type_id.hpp"
#include "mega/type_instance.hpp"
#include "mega/any.hpp"

namespace mega
{

/*
struct LogicalInstantiation
{
    LogicalObject start()
    void* read( const LogicalReference& ref )
    U64 linkSize( const LogicalReference& ref, bool bOwning, bool bOwned )
    LogicalObject linkGet( const LogicalReference& link, U64 index )
};
struct LogicalTreeVisitor
{
    void on_object_start( const LogicalReference& ref )
    void on_object_end( const LogicalReference& ref )
    void on_action_start( const LogicalReference& ref )
    void on_action_end( const LogicalReference& ref )
    void on_event_start( const LogicalReference& ref )
    void on_event_end( const LogicalReference& ref )
    void on_link_start( const LogicalReference& ref, bool bOwning, bool bOwned )
    void on_link_end( const LogicalReference& ref, bool bOwning, bool bOwned )
    void on_interupt( const LogicalReference& ref )
    void on_function( const LogicalReference& ref )
    void on_namespace( const LogicalReference& ref )
    void on_dimension( const LogicalReference& ref, void* pData )
};
*/

/*
struct ReferenceTraits
{
    Object
    {
    }
    Reference
    {
        static Reference make( Object, TypeInstance );
    }
};
*/

template < typename ReferenceTraits, typename Instantiation, typename Visitor >
class TreeTraversal : public TraversalVisitor
{
    using Reference = typename ReferenceTraits::Reference;
    using Object    = typename ReferenceTraits::Object;

    struct LinkFrame
    {
        Reference link;
        U64       iterator, size;
    };

    struct ObjectFrame
    {
        Object object;
    };

    using StackFrame = std::variant< LinkFrame, ObjectFrame >;
    using Stack      = std::vector< StackFrame >;
    Stack          m_stack;
    Instantiation& m_instantiation;
    Visitor&       m_visitor;

    inline Reference getReference( const TypeInstance& typeInstance ) const
    {
        const auto& objectFrame = std::get< ObjectFrame >( m_stack.back() );
        return Reference::make( objectFrame.object, typeInstance );
    }

public:
    TreeTraversal( Instantiation& instantiation, Visitor& visitor )
        : m_instantiation( instantiation )
        , m_visitor( visitor )
    {
        // push the initial object frame
        ObjectFrame linkedObject{ m_instantiation.start() };
        m_stack.push_back( linkedObject );
    }

private:
    TypeID start() const override { return std::get< ObjectFrame >( m_stack.back() ).object.getType(); }

    void on_object_start( const TypeInstance& typeInstance ) override
    {
        ASSERT( !m_stack.empty() );
        ASSERT( std::get< ObjectFrame >( m_stack.back() ).object.getType() == typeInstance.type );
        m_visitor.on_object_start( getReference( typeInstance ) );
    }

    std::optional< TypeID > on_object_end( const TypeInstance& typeInstance ) override
    {
        ASSERT( !m_stack.empty() );
        ASSERT_MSG( std::get< ObjectFrame >( m_stack.back() ).object.getType() == typeInstance.type,
                    "Unexpected object frame type: " << std::hex << typeInstance.type << " expected: " << std::hex
                                                     << std::get< ObjectFrame >( m_stack.back() ).object.getType() );

        m_visitor.on_object_end( getReference( typeInstance ) );

        // pop the object frame
        m_stack.pop_back();

        if( !m_stack.empty() )
        {
            auto& frame = std::get< LinkFrame >( m_stack.back() );
            return TypeID::make_end_state( frame.link.getTypeInstance().type );
        }
        else
        {
            return {};
        }
    }

    void on_action_start( const TypeInstance& typeInstance ) override
    {
        m_visitor.on_action_start( getReference( typeInstance ) );
    }
    void on_action_end( const TypeInstance& typeInstance ) override
    {
        m_visitor.on_action_end( getReference( typeInstance ) );
    }
    void on_event_start( const TypeInstance& typeInstance ) override
    {
        m_visitor.on_event_start( getReference( typeInstance ) );
    }
    void on_event_end( const TypeInstance& typeInstance ) override
    {
        m_visitor.on_event_end( getReference( typeInstance ) );
    }

    void on_link_start( const TypeInstance& typeInstance, bool bOwning, bool bOwned ) override
    {
        ASSERT( !m_stack.empty() );

        // push the link frame
        const auto& objectFrame = std::get< ObjectFrame >( m_stack.back() );
        Reference   linkRef     = Reference::make( objectFrame.object, typeInstance );
        const U64   linkSize    = m_instantiation.linkSize( linkRef, bOwning, bOwned );
        LinkFrame   frame{ linkRef, 0, linkSize };
        m_stack.push_back( frame );
        m_visitor.on_link_start( linkRef, bOwning, bOwned );
    }

    std::optional< TypeID > on_link_end( const TypeInstance& typeInstance, bool bOwning, bool bOwned ) override
    {
        ASSERT( !m_stack.empty() );

        auto& frame = std::get< LinkFrame >( m_stack.back() );

        if( frame.iterator != frame.size )
        {
            // push the object frame
            Object linkedObject = m_instantiation.linkGet( frame.link, frame.iterator );
            ++frame.iterator;
            ObjectFrame objectFrame{ linkedObject };
            m_stack.push_back( objectFrame );
            return objectFrame.object.getType();
        }
        else
        {
            // pop the link frame
            m_stack.pop_back();
            m_visitor.on_link_end( frame.link, bOwning, bOwned );
            return {};
        }
    }

    void on_interupt( const TypeInstance& typeInstance ) override
    {
        m_visitor.on_interupt( getReference( typeInstance ) );
    }

    void on_function( const TypeInstance& typeInstance ) override
    {
        m_visitor.on_function( getReference( typeInstance ) );
    }

    void on_namespace( const TypeInstance& typeInstance ) override
    {
        m_visitor.on_namespace( getReference( typeInstance ) );
    }

    void on_dimension( const TypeInstance& typeInstance ) override
    {
        const Reference ref = getReference( typeInstance );
        m_visitor.on_dimension( ref, m_instantiation.read( ref ) );
    }
};
} // namespace mega

#endif // GUARD_2023_September_07_tree_traversal
