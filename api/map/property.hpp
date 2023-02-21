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

#ifndef PROPERTY_18_09_2013
#define PROPERTY_18_09_2013

#include "glyphSpec.hpp"
#include "node.hpp"

#include "common/stl.hpp"
#include "common/compose.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/optional.hpp>
#include <boost/chrono.hpp>

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>

namespace map
{

class Factory;

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
class Property : public Node, public boost::enable_shared_from_this< Property >
{
public:
    using Ptr    = boost::shared_ptr< Property >;
    using PtrCst = boost::shared_ptr< const Property >;
    using PtrMap = std::map< std::string, Ptr >;

    static const std::string& TypeName();
    Property( Node::Ptr pParent, const std::string& strName );
    Property( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName );
    virtual Node::PtrCst getPtr() const { return shared_from_this(); }
    virtual Node::Ptr    getPtr() { return shared_from_this(); }
    virtual void         init();
    virtual Node::Ptr    copy( Node::Ptr pParent, const std::string& strName ) const;
    // virtual void load( Loader& loader );
    // virtual void save( Storer& storer ) const;
    virtual std::string getStatement() const;
    void                setStatement( const std::string& strStatement );
    const std::string&  getValue() const { return m_strValue; }

private:
    std::string m_strValue;
};
/*
class RefPtr
{
    struct ReferenceResolver : public boost::static_visitor< Node::Ptr >
    {
        Node::Ptr m_ptr;
        ReferenceResolver( Node::Ptr pThis )
            :   m_ptr( pThis )
        {
        }

        Node::Ptr operator()( const Ed::Identifier& str ) const
        {
            return m_ptr->get< Node >( str );
        }
        Node::Ptr operator()( const Ed::RefActionType& type ) const
        {
            Node::Ptr pResult;
            switch( type )
            {
            case Ed::eRefUp:
                pResult = m_ptr->getParent();
                break;
            default:
                ASSERT( false );
                break;
            }
            return pResult;
        }
        Node::Ptr operator()( const Ed::Ref& ) const
        {
            THROW_RTE( "Invalid reference branch used in map reference" );
            return Node::Ptr();
        }
    };
public:
    RefPtr( Node::Ptr pThis, const Ed::Reference& reference )
        :   m_pThis( pThis ),
            m_reference( reference )
    {

    }

    template< class T >
    boost::shared_ptr< T > get() const
    {
        Node::Ptr pIter = m_pThis.lock();
        for( Ed::Reference::const_iterator
            i = m_reference.begin(),
            iEnd = m_reference.end(); i!=iEnd && pIter; ++i )
        {
            pIter = boost::apply_visitor( ReferenceResolver( pIter ), *i );
        }
        return boost::dynamic_pointer_cast< T >( pIter );
    }

private:
    Node::PtrWeak m_pThis;
    Ed::Reference m_reference;
};
*/
/*
static bool calculateReference( Node::Ptr pSource, Node::Ptr pTarget, Ed::Reference& result )
{
    bool bSuccess = false;

    //first calculate the paths to the root
    typedef std::list< std::string > StringList;
    StringList sourcePath, targetPath;
    for( Node::Ptr pIter = pSource; pIter; pIter = pIter->getParent() )
        sourcePath.push_front( pIter->getName() );
    for( Node::Ptr pIter = pTarget; pIter; pIter = pIter->getParent() )
        targetPath.push_front( pIter->getName() );

    if( !sourcePath.empty() && !targetPath.empty()
        && sourcePath.front() == targetPath.front() )
    {
        bSuccess = true;

        StringList::iterator
            i1 = targetPath.begin(),
            i1End = targetPath.end(),
            i2 = sourcePath.begin(),
            i2End = sourcePath.end();
        for( ; i1 != i1End && i2 != i2End && *i1 == *i2; ++i1, ++i2 )
        {}

        //generate the up part
        if( std::size_t uiUps = std::distance( i2, i2End ) )
        {
            result.resize( uiUps );
            std::fill_n( result.begin(), uiUps, Ed::eRefUp );
        }
        //generate the down part
        std::copy( i1, i1End, std::back_inserter( result ) );
    }

    return bSuccess;
}
*/

class Reference : public Node, public boost::enable_shared_from_this< Reference >
{
public:
    using Ptr    = boost::shared_ptr< Reference >;
    using PtrCst = boost::shared_ptr< const Reference >;
    using PtrMap = std::map< std::string, Ptr >;

    static const std::string& TypeName();
    Reference( Node::Ptr pParent, const std::string& strName );
    Reference( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName );
    virtual Node::PtrCst getPtr() const { return shared_from_this(); }
    virtual Node::Ptr    getPtr() { return shared_from_this(); }
    virtual void         init();
    virtual Node::Ptr    copy( Node::Ptr pParent, const std::string& strName ) const;
    // virtual void load( Loader& loader );
    // virtual void save( Storer& storer ) const;
    virtual std::string getStatement() const;

    // const Ed::Reference& getValue() const;
    // void setValue( const Ed::Reference& ref ) { m_reference = ref; }
private:
    // Ed::Reference m_reference;
};

} // namespace map

#endif // PROPERTY_18_09_2013