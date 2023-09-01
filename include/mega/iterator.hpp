
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

#include "common/assert_verify.hpp"
#include "common/unreachable.hpp"

#include <optional>
#include <utility>
#include <vector>
#include <variant>
#include <functional>

namespace mega
{

struct TraversalVisitor
{
    virtual TypeID start() const                            = 0;
    virtual void*  read( const TypeInstance& typeInstance ) = 0;

    virtual void                          on_object_start( const TypeInstance& typeInstance ) = 0;
    virtual std::optional< mega::TypeID > on_object_end( const TypeInstance& typeInstance )   = 0;
    virtual void                          on_action_start( const TypeInstance& typeInstance ) = 0;
    virtual void                          on_action_end( const TypeInstance& typeInstance )   = 0;
    virtual void                          on_event_start( const TypeInstance& typeInstance )  = 0;
    virtual void                          on_event_end( const TypeInstance& typeInstance )    = 0;
    virtual void                          on_link_start( const TypeInstance& typeInstance )   = 0;
    virtual std::optional< mega::TypeID > on_link_end( const TypeInstance& typeInstance )     = 0;
    virtual void                          on_interupt( const TypeInstance& typeInstance )     = 0;
    virtual void                          on_function( const TypeInstance& typeInstance )     = 0;
    virtual void                          on_namespace( const TypeInstance& typeInstance )    = 0;
    virtual void                          on_dimension( const TypeInstance& typeInstance )    = 0;
};

struct LogicalObject
{
    U64    id;
    TypeID type;

    struct Hash
    {
        inline U64 operator()( const LogicalObject& ref ) const noexcept
        {
            return ref.id + ref.type.getSymbolID();
        }
    };

    inline bool operator==( const LogicalObject& cmp ) const
    {
        return ( id == cmp.id ) && ( type == cmp.type );
    }
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

    static LogicalReference make( const LogicalObject& logicalObject, const TypeInstance& typeInstance )
    {
        return { logicalObject.id, typeInstance };
    }

    static LogicalObject toLogicalObject( const LogicalReference& ref )
    {
        return { ref.id, TypeID::make_object_from_typeID( ref.typeInstance.type ) };
    }
};
/*
struct LogicalInstantiation
{
    LogicalObject start()
    {
        //
        return LogicalObject{ 0, TypeID{} };
    }

    void* read( const LogicalReference& ref )
    {
        //
        return nullptr;
    }

    U64 linkSize( const LogicalReference& ref )
    {
        //
        return 0;
    }

    LogicalObject linkObject( const LogicalReference& link, U64 index )
    {
        //
        return {};
    }
};

struct LogicalTreeVisitor
{
    void on_object_start( const LogicalReference& ref )
    {
        //
    }
    void on_object_end( const LogicalReference& ref )
    {
        //
    }
    void on_action_start( const LogicalReference& ref )
    {
        //
    }
    void on_action_end( const LogicalReference& ref )
    {
        //
    }
    void on_event_start( const LogicalReference& ref )
    {
        //
    }
    void on_event_end( const LogicalReference& ref )
    {
        //
    }
    void on_link_start( const LogicalReference& ref )
    {
        //
    }
    void on_link_end( const LogicalReference& ref )
    {
        //
    }
    void on_interupt( const LogicalReference& ref )
    {
        //
    }
    void on_function( const LogicalReference& ref )
    {
        //
    }
    void on_namespace( const LogicalReference& ref )
    {
        //
    }
    void on_dimension( const LogicalReference& ref, void* pData )
    {
        //
    }
};
*/

template < typename Instantiation, typename Visitor >
class LogicalTreeTraversal : public TraversalVisitor
{
    struct LinkFrame
    {
        LogicalReference link;
        U64              iterator, size;
    };

    struct ObjectFrame
    {
        LogicalObject object;
    };

    using StackFrame = std::variant< LinkFrame, ObjectFrame >;
    using Stack      = std::vector< StackFrame >;
    Stack          m_stack;
    Instantiation& m_instantiation;
    Visitor&       m_visitor;

    inline LogicalReference getLogicalReference( const TypeInstance& typeInstance ) const
    {
        const auto& objectFrame = std::get< ObjectFrame >( m_stack.back() );
        return LogicalReference::make( objectFrame.object, typeInstance );
    }

public:
    LogicalTreeTraversal( Instantiation& instantiation, Visitor& visitor )
        : m_instantiation( instantiation )
        , m_visitor( visitor )
    {
        // push the initial object frame
        ObjectFrame linkedObject{ m_instantiation.start() };
        m_stack.push_back( linkedObject );
    }

private:
    TypeID start() const override { return std::get< ObjectFrame >( m_stack.back() ).object.type; }

    void* read( const TypeInstance& typeInstance ) override
    {
        ASSERT( !m_stack.empty() );
        const auto& objectFrame = std::get< ObjectFrame >( m_stack.back() );
        return m_instantiation.read( LogicalReference::make( objectFrame.object, typeInstance ) );
    }

    void on_object_start( const TypeInstance& typeInstance ) override
    {
        ASSERT( !m_stack.empty() );
        ASSERT( std::get< ObjectFrame >( m_stack.back() ).object.type == typeInstance.type );
        m_visitor.on_object_start( getLogicalReference( typeInstance ) );
    }

    std::optional< TypeID > on_object_end( const TypeInstance& typeInstance ) override
    {
        ASSERT( !m_stack.empty() );
        ASSERT_MSG( std::get< ObjectFrame >( m_stack.back() ).object.type == typeInstance.type,
                    "Unexpected object frame type: " << std::hex << typeInstance.type << " expected: " << std::hex
                                                     << std::get< ObjectFrame >( m_stack.back() ).object.type );

        m_visitor.on_object_end( getLogicalReference( typeInstance ) );

        // pop the object frame
        m_stack.pop_back();

        if( !m_stack.empty() )
        {
            auto& frame = std::get< LinkFrame >( m_stack.back() );
            return TypeID::make_end_state( frame.link.typeInstance.type );
        }
        else
        {
            return {};
        }
    }

    void on_action_start( const TypeInstance& typeInstance ) override
    {
        m_visitor.on_action_start( getLogicalReference( typeInstance ) );
    }
    void on_action_end( const TypeInstance& typeInstance ) override
    {
        m_visitor.on_action_end( getLogicalReference( typeInstance ) );
    }
    void on_event_start( const TypeInstance& typeInstance ) override
    {
        m_visitor.on_event_start( getLogicalReference( typeInstance ) );
    }
    void on_event_end( const TypeInstance& typeInstance ) override
    {
        m_visitor.on_event_end( getLogicalReference( typeInstance ) );
    }

    void on_link_start( const TypeInstance& typeInstance ) override
    {
        ASSERT( !m_stack.empty() );

        // push the link frame
        const auto&      objectFrame = std::get< ObjectFrame >( m_stack.back() );
        LogicalReference linkRef     = LogicalReference::make( objectFrame.object, typeInstance );
        const U64        linkSize    = m_instantiation.linkSize( linkRef );
        LinkFrame        frame{ linkRef, 0, linkSize };
        m_stack.push_back( frame );
        m_visitor.on_link_start( linkRef );
    }

    std::optional< TypeID > on_link_end( const TypeInstance& typeInstance ) override
    {
        ASSERT( !m_stack.empty() );

        auto& frame = std::get< LinkFrame >( m_stack.back() );

        if( frame.iterator != frame.size )
        {
            // push the object frame
            LogicalObject linkedObject = m_instantiation.linkObject( frame.link, frame.iterator );
            ++frame.iterator;
            ObjectFrame objectFrame{ linkedObject };
            m_stack.push_back( objectFrame );
            return objectFrame.object.type;
        }
        else
        {
            // pop the link frame
            m_stack.pop_back();
            m_visitor.on_link_end( frame.link );
            return {};
        }
    }

    void on_interupt( const TypeInstance& typeInstance ) override
    {
        m_visitor.on_interupt( getLogicalReference( typeInstance ) );
    }

    void on_function( const TypeInstance& typeInstance ) override
    {
        m_visitor.on_function( getLogicalReference( typeInstance ) );
    }

    void on_namespace( const TypeInstance& typeInstance ) override
    {
        m_visitor.on_namespace( getLogicalReference( typeInstance ) );
    }

    void on_dimension( const TypeInstance& typeInstance ) override
    {
        m_visitor.on_dimension( getLogicalReference( typeInstance ), read( typeInstance ) );
    }
};

class Iterator
{
    using TraversalFunction = std::function< void( void* ) >;

    mega::TypeID      m_state; // can be negative for END state
    TraversalFunction m_pTraversalFunction;
    TraversalVisitor& m_visitor;
    bool              m_atEnd = false;

    struct Domain
    {
        bool           isObject;
        mega::Instance iter, total;
        TypeID         successor;
    };
    using DomainStack = std::vector< Domain >;
    DomainStack m_domainStack;

    inline Instance getInstance() const
    {
        Instance result = 0;
        Instance total  = 1;
        for( auto i = m_domainStack.rbegin(); i != m_domainStack.rend(); ++i )
        {
            if( i->isObject )
                break;
            result = result + total * i->iter;
            total  = total * i->total;
        }
        return result;
    }
    inline TypeInstance getTypeInstance( TypeID type ) const { return TypeInstance{ type, getInstance() }; }

public:
    inline Iterator( TraversalFunction pTraversal, TraversalVisitor& visitor )
        : m_state( visitor.start() )
        , m_pTraversalFunction( std::move( pTraversal ) )
        , m_visitor( visitor )
    {
    }

    // pre increment
    inline const Iterator& operator++()
    {
        m_pTraversalFunction( reinterpret_cast< void* >( this ) );
        return *this;
    }

    inline operator void*() const { return m_atEnd ? nullptr : ( void* )this; }

    inline TypeID getState() const { return m_state; }

    inline void object_start( mega::TypeID successor )
    {
        Domain domain{ true, 0, 1, successor };
        m_domainStack.emplace_back( std::move( domain ) );

        m_visitor.on_object_start( getTypeInstance( m_state ) );
        m_state = successor;
    }

    inline void object_end()
    {
        std::optional< mega::TypeID > nextState
            = m_visitor.on_object_end( getTypeInstance( TypeID::make_start_state( m_state ) ) );
        if( nextState.has_value() )
        {
            m_state = nextState.value();
        }
        else
        {
            m_atEnd = true;
        }

        m_domainStack.pop_back();
    }

    inline void action_start( mega::TypeID successor, mega::Instance localDomainSize )
    {
        Domain domain{ false, 0, localDomainSize, successor };
        m_domainStack.emplace_back( std::move( domain ) );
        m_visitor.on_action_start( getTypeInstance( m_state ) );
        m_state = successor;
    }

    inline void action_end( mega::TypeID successor )
    {
        m_visitor.on_action_end( getTypeInstance( m_state ) );
        auto& domain = m_domainStack.back();
        ++domain.iter;
        if( domain.iter == domain.total )
        {
            m_domainStack.pop_back();
            m_state = successor;
        }
        else
        {
            m_visitor.on_action_start( getTypeInstance( TypeID::make_start_state( m_state ) ) );
            m_state = domain.successor;
        }
    }

    inline void event_start( mega::TypeID successor, mega::Instance localDomainSize )
    {
        Domain domain{ false, 0, localDomainSize, successor };
        m_domainStack.emplace_back( std::move( domain ) );
        m_visitor.on_event_start( getTypeInstance( m_state ) );
        m_state = successor;
    }

    inline void event_end( mega::TypeID successor )
    {
        m_visitor.on_event_end( getTypeInstance( m_state ) );
        auto& domain = m_domainStack.back();
        ++domain.iter;
        if( domain.iter == domain.total )
        {
            m_domainStack.pop_back();
            m_state = successor;
        }
        else
        {
            m_visitor.on_event_start( getTypeInstance( TypeID::make_start_state( m_state ) ) );
            m_state = domain.successor;
        }
    }

    inline void link_start( mega::TypeID successor )
    {
        m_visitor.on_link_start( getTypeInstance( m_state ) );
        m_state = successor;
    }

    inline void link_end( mega::TypeID successor )
    {
        std::optional< mega::TypeID > nextState
            = m_visitor.on_link_end( getTypeInstance( TypeID::make_start_state( m_state ) ) );
        if( nextState.has_value() )
        {
            m_state = nextState.value();
        }
        else
        {
            m_state = successor;
        }
    }

    inline void interupt_start( mega::TypeID successor )
    {
        m_visitor.on_interupt( getTypeInstance( m_state ) );
        m_state = successor;
    }
    inline void interupt_end( mega::TypeID successor ) { m_state = successor; }
    inline void function_start( mega::TypeID successor )
    {
        m_visitor.on_function( getTypeInstance( m_state ) );
        m_state = successor;
    }
    inline void function_end( mega::TypeID successor ) { m_state = successor; }
    inline void namespace_start( mega::TypeID successor )
    {
        m_visitor.on_namespace( getTypeInstance( m_state ) );
        m_state = successor;
    }
    inline void namespace_end( mega::TypeID successor ) { m_state = successor; }
    inline void dimension( mega::TypeID successor )
    {
        m_visitor.on_dimension( getTypeInstance( m_state ) );
        m_state = successor;
    }
};

} // namespace mega

#endif // GUARD_2023_August_30_iterator
