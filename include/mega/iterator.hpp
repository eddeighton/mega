
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
    virtual TypeID start() const = 0;

    virtual void                          on_object_start( const TypeInstance& typeInstance ) = 0;
    virtual std::optional< mega::TypeID > on_object_end( const TypeInstance& typeInstance )   = 0;
    virtual void                          on_action_start( const TypeInstance& typeInstance ) = 0;
    virtual void                          on_action_end( const TypeInstance& typeInstance )   = 0;
    virtual void                          on_event_start( const TypeInstance& typeInstance )  = 0;
    virtual void                          on_event_end( const TypeInstance& typeInstance )    = 0;
    virtual void                          on_interupt( const TypeInstance& typeInstance )     = 0;
    virtual void                          on_function( const TypeInstance& typeInstance )     = 0;
    virtual void                          on_namespace( const TypeInstance& typeInstance )    = 0;
    virtual void                          on_dimension( const TypeInstance& typeInstance )    = 0;
    virtual void on_link_start( const TypeInstance& typeInstance, bool bOwning, bool bOwned ) = 0;
    virtual std::optional< mega::TypeID > on_link_end( const TypeInstance& typeInstance, bool bOwning, bool bOwned )
        = 0;
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

    inline void link_start( mega::TypeID successor, bool bOwning, bool bOwned )
    {
        m_visitor.on_link_start( getTypeInstance( m_state ), bOwning, bOwned );
        m_state = successor;
    }

    inline void link_end( mega::TypeID successor, bool bOwning, bool bOwned )
    {
        std::optional< mega::TypeID > nextState
            = m_visitor.on_link_end( getTypeInstance( TypeID::make_start_state( m_state ) ), bOwning, bOwned );
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
