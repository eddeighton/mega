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

#ifndef NODE_21_09_2013
#define NODE_21_09_2013

#include "schematic/glyphSpec.hpp"

#include "common/stl.hpp"
#include "common/compose.hpp"
#include "common/assert_verify.hpp"
#include "common/tick.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/optional.hpp>
#include <boost/chrono.hpp>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <iomanip>

namespace schematic
{
class Factory;
class Property;
class File;

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
class Node
{
    Node( const Node& cpy );
    Node& operator=( const Node& cpy );

public:
    using Ptr          = boost::shared_ptr< Node >;
    using WeakPtr      = boost::weak_ptr< Node >;
    using PtrCst       = boost::shared_ptr< const Node >;
    using PtrWeak      = boost::weak_ptr< Node >;
    using PtrCstWeak   = boost::weak_ptr< const Node >;
    using PtrMap       = std::map< std::string, Ptr >;
    using PtrSet       = std::set< Ptr >;
    using PtrCstSet    = std::set< PtrCst >;
    using PtrList      = std::list< Ptr >;
    using PtrVector    = std::vector< Ptr >;
    using PtrCstVector = std::vector< PtrCst >;

    template < class T >
    struct ConvertPtrType
    {
        boost::shared_ptr< T > operator()( Node::Ptr p ) const { return boost::dynamic_pointer_cast< T >( p ); }
    };

    Node( Node::Ptr pParent, const std::string& strName );
    Node( Node::PtrCst pOriginal, Node::Ptr pNewParent, const std::string& strName );
    virtual ~Node();
    virtual Node::PtrCst getPtr() const = 0;
    virtual Node::Ptr    getPtr()       = 0;

protected:
    template < class T, class TParentPtrType >
    inline boost::shared_ptr< T >
    copy( boost::shared_ptr< const T > pThis, TParentPtrType pNewParent, const std::string& strName ) const
    {
        boost::shared_ptr< T > pCopy( new T( pThis, pNewParent, strName ) );
        for( const auto& i : m_childrenOrdered )
        {
            VERIFY_RTE( pCopy->add( i->copy( pCopy, i->getName() ) ) );
        }

        pCopy->init();
        return pCopy;
    }

public:
    const std::string&              getName() const { return m_strName; }
    Ptr                             getParent() const { return m_pParent.lock(); }
    boost::shared_ptr< const File > getRootFile() const;
    const PtrVector&                getChildren() const { return m_childrenOrdered; }
    std::size_t                     size() const { return m_childrenOrdered.size(); }
    const Timing::UpdateTick&       getLastModifiedTick() const { return m_lastModifiedTick; }
    const Timing::UpdateTick&       getLastModifiedTickForTree() const;
    std::size_t                     getIndex() const { return m_iIndex; }
    virtual std::string             getStatement() const = 0;

    void         setModified();
    virtual void init();
    virtual Ptr  copy( Node::Ptr pParent, const std::string& strName ) const = 0;

    virtual void load( const format::Node& node );
    virtual void save( format::Node& node ) const;

    virtual bool add( Ptr pNewNode );
    virtual void remove( Ptr pNode );

    std::string generateNewNodeName( const std::string& strPrefix ) const;
    std::string generateNewNodeName( Node::Ptr pCopiedNode ) const;

    boost::optional< std::string > getPropertyString( const std::string& strKey ) const;

    template < class T >
    boost::shared_ptr< T > get( const std::string& strKey ) const
    {
        boost::shared_ptr< T > pResultPtr;
        auto                   iFind = m_children.find( strKey );
        if( iFind != m_children.end() )
            pResultPtr = boost::dynamic_pointer_cast< T >( iFind->second );
        return pResultPtr;
    }

    template < class TPredicate >
    inline void for_each( const TPredicate& predicate ) const
    {
        generics::for_each_second( m_children, predicate );
    }

    template < class TFunctor, class TPredicateCutOff >
    struct DepthFirstRecursion
    {
        const TFunctor&         m_functor;
        const TPredicateCutOff& m_cutoffPredicate;
        DepthFirstRecursion( const TFunctor& functor, const TPredicateCutOff& cutoffPredicate )
            : m_functor( functor )
            , m_cutoffPredicate( cutoffPredicate )
        {
        }
        inline void operator()( Node::Ptr p ) const
        {
            m_functor( p );
            if( !m_cutoffPredicate( p ) )
                p->for_each( DepthFirstRecursion< TFunctor, TPredicateCutOff >( m_functor, m_cutoffPredicate ) );
        }
    };

    template < class TFunctor, class TPredicateCutOff >
    void for_each_recursive( const TFunctor&         functor,
                             const TPredicateCutOff& cutoffPredicate = generics::_not( generics::all() ) ) const
    {
        generics::for_each_second(
            m_children, DepthFirstRecursion< TFunctor, TPredicateCutOff >( functor, cutoffPredicate ) );
    }

protected:
    PtrWeak m_pParent;

private:
    const std::string  m_strName;
    PtrVector          m_childrenOrdered;
    PtrMap             m_children;
    std::size_t        m_iIndex;
    Timing::UpdateTick m_lastModifiedTick;
};

} // namespace schematic

#endif // NODE_21_09_2013