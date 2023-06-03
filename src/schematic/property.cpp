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

#include "schematic/property.hpp"
#include "schematic/factory.hpp"

#include "common/assert_verify.hpp"

#include <boost/optional.hpp>

namespace schematic
{
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
const std::string& Property::TypeName()
{
    static const std::string strTypeName( "property" );
    return strTypeName;
}
Property::Property( Node::Ptr pParent, const std::string& strName )
    : Node( pParent, strName )
{
}

Property::Property( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName )
    : Node( pOriginal, pParent, strName )
    , m_strValue( pOriginal->m_strValue )
{
}

void Property::init()
{
    Node::init();
}

Node::Ptr Property::copy( Node::Ptr pParent, const std::string& strName ) const
{
    return Node::copy< Property >( shared_from_this(), pParent, strName );
}
void Property::load( const format::Node& node )
{
    Node::load( node );
    VERIFY_RTE( node.has_property() );
    const format::Node::Property& property = node.property();
    m_strValue                             = property.value;
}
void Property::save( format::Node& node ) const
{
    format::Node::Property& property = *node.mutable_property();
    property.value                   = m_strValue;
    Node::save( node );
}

std::string Property::getStatement() const
{
    std::ostringstream os;
    os << m_strValue;
    return os.str();
}

void Property::setStatement( const std::string& strStatement )
{
    if( m_strValue != strStatement )
    {
        m_strValue = strStatement;
        setModified();
    }
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/*
const std::string& Reference::TypeName()
{
    static const std::string strTypeName( "reference" );
    return strTypeName;
}
Reference::Reference( Node::Ptr pParent, const std::string& strName )
    : Node( pParent, strName )
{
}
Reference::Reference( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName )
    : Node( pOriginal, pParent, strName ) //,
                                          // m_reference( pOriginal->m_reference )
{
}
void Reference::init()
{
    Node::init();
}
Node::Ptr Reference::copy( Node::Ptr pParent, const std::string& strName ) const
{
    return Node::copy< Reference >( shared_from_this(), pParent, strName );
}

std::string Reference::getStatement() const
{
    std::ostringstream os;
    {
        THROW_RTE( "TODO" );
    }
    return os.str();
}*/

} // namespace schematic