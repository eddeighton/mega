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

#include "schematic/node.hpp"
#include "schematic/factory.hpp"
#include "schematic/property.hpp"
#include "schematic/file.hpp"

#include "common/assert_verify.hpp"

#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>

namespace schematic
{

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
Node::Node( Node::Ptr pParent, const std::string& strName )
    : m_pParent( pParent )
    , m_strName( boost::to_lower_copy( strName ) )
    , m_iIndex( 0 )
{
}

Node::Node( Node::PtrCst pOriginal, Node::Ptr pNewParent, const std::string& strName )
    : m_pParent( pNewParent )
    , m_strName( boost::to_lower_copy( strName ) )
    , m_iIndex( pOriginal->m_iIndex )
{
}

Node::~Node()
{
}

std::string Node::generateNewNodeName( const std::string& strPrefix ) const
{
    std::string       strNewKey;
    size_t            iIndex           = m_childrenOrdered.size();
    const std::string strPrefixLowered = boost::to_lower_copy( strPrefix );
    do
    {
        std::ostringstream os;
        os << strPrefixLowered << '_' << std::setfill( '0' ) << std::setw( 4 ) << iIndex++;
        strNewKey = os.str();
    } while( get< Node >( strNewKey ) );
    return strNewKey;
}

std::string Node::generateNewNodeName( Node::Ptr pCopiedNode ) const
{
    return generateNewNodeName( pCopiedNode->getName().substr( 0, 4 ) );
}

void Node::setModified()
{
    m_lastModifiedTick.update();
}
void Node::init()
{
    for_each( []( Ptr pNode ) { pNode->init(); } );
    setModified();
}

void Node::load( const format::Node& node )
{
    // ensure the sites are restored to their original order using
    // the map index
    using NewSite = std::pair< Node::Ptr, const format::Node* >;
    std::vector< NewSite > newNodes( node.children.size() );

    for( const auto& child : node.children )
    {
        int                 szIndex   = child.first;
        const format::Node& childSite = child.second;

        Node::Ptr pNewNode = schematic::construct( getPtr(), childSite );
        pNewNode->load( childSite );

        newNodes[ szIndex ] = std::make_pair( pNewNode, &childSite );
    }

    for( const NewSite& newNode : newNodes )
    {
        add( newNode.first );
        newNode.first->init();
    }
}

void Node::save( format::Node& node ) const
{
    node.name = m_strName;
    node.index = m_iIndex;
    for( Node::Ptr pChildNode : m_childrenOrdered )
    {
        format::Node childNode;
        pChildNode->save( childNode );
        node.children.insert( { childNode.index, childNode } );
    }
}
/*
void Node::load( Node::Ptr pThis, Loader& loader )
{
    //VERIFY_RTE_MSG( node.statement.declarator.identifier, "Node with no identifier" );
    //m_strName = node.statement.declarator.identifier.get();

    THROW_RTE( "TODO" );
    for( Ed::Node::Vector::const_iterator
        i = node.children.begin(), iEnd = node.children.end(); i!=iEnd; ++i )
    {
        VERIFY_RTE_MSG( i->statement.declarator.identifier, "Node with no identifier" );
        if( Node::Ptr pNewNode = factory.load( pThis, *i ) )
        {
            pNewNode->m_iIndex = m_childrenOrdered.size();
            m_childrenOrdered.push_back( pNewNode );
            m_children.insert( std::make_pair( i->statement.declarator.identifier.get(), pNewNode ) );
        }
    }
    setModified();
}

void Node::save( Storer& storer ) const
{
    THROW_RTE( "TODO" );
    node.statement.declarator.identifier = getName();
    for( PtrVector::const_iterator i = m_childrenOrdered.begin(),
        iEnd = m_childrenOrdered.end(); i!=iEnd; ++i )
    {
        Ed::Node n;
        n.statement.declarator.identifier = (*i)->getName();
        (*i)->save( n );
        node.children.push_back( n );
    }

}
*/
bool Node::add( Node::Ptr pNewNode )
{
    bool                   bInserted = false;
    PtrMap::const_iterator iFind     = m_children.find( pNewNode->getName() );
    if( iFind == m_children.end() )
    {
        pNewNode->m_iIndex = m_childrenOrdered.size();
        m_childrenOrdered.push_back( pNewNode );
        m_children.insert( std::make_pair( pNewNode->getName(), pNewNode ) );
        setModified();
        bInserted = true;
    }
    return bInserted;
}
void Node::remove( Node::Ptr pNode )
{
    PtrVector::iterator    iFind    = std::find( m_childrenOrdered.begin(), m_childrenOrdered.end(), pNode );
    PtrMap::const_iterator iFindKey = m_children.find( pNode->getName() );
    VERIFY_RTE( iFind != m_childrenOrdered.end() && iFindKey != m_children.end() );
    m_children.erase( iFindKey );
    // erase and update the later indices
    for( PtrVector::iterator i      = m_childrenOrdered.erase( iFind ),
                             iBegin = m_childrenOrdered.begin(),
                             iEnd   = m_childrenOrdered.end();
         i != iEnd;
         ++i )
        ( *i )->m_iIndex = ( i - iBegin );
    setModified();
}
/*
void Node::removeOptional( Ptr pNode )
{
    PtrVector::iterator iFind =
        std::find( m_childrenOrdered.begin(), m_childrenOrdered.end(), pNode );
    PtrMap::const_iterator iFindKey = m_children.find( pNode->getName() );
    VERIFY_RTE( (iFind != m_childrenOrdered.end() && iFindKey != m_children.end()) ||
        !(iFind != m_childrenOrdered.end() && iFindKey != m_children.end()));
    if( iFind != m_childrenOrdered.end() && iFindKey != m_children.end() )
        remove( pNode );
}*/

boost::optional< std::string > Node::getPropertyString( const std::string& strKey ) const
{
    boost::optional< std::string > result;
    PtrMap::const_iterator         iFind = m_children.find( strKey );
    if( iFind != m_children.end() )
    {
        if( Property::Ptr pProperty = boost::dynamic_pointer_cast< Property >( iFind->second ) )
            result = pProperty->getValue();
    }
    return result;
}

const Timing::UpdateTick& Node::getLastModifiedTickForTree() const
{
    const Timing::UpdateTick* pLastUpdate = &m_lastModifiedTick;

    for( PtrVector::const_iterator i = m_childrenOrdered.begin(), iEnd = m_childrenOrdered.end(); i != iEnd; ++i )
    {
        const Node& child = **i;

        const Timing::UpdateTick& childLatestUpdate = child.getLastModifiedTickForTree();

        if( childLatestUpdate > *pLastUpdate )
            pLastUpdate = &childLatestUpdate;
    }

    return *pLastUpdate;
}

boost::shared_ptr< const File > Node::getRootFile() const
{
    PtrCst pIter = getPtr();
    while( PtrCst pParent = pIter->getParent() )
        pIter = pParent;

    File::PtrCst pFile = boost::dynamic_pointer_cast< const File >( pIter );
    VERIFY_RTE( pFile );
    return pFile;
}

} // namespace schematic