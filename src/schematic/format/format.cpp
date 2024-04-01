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

#include "schematic/format/format.hpp"

#include "ed/nodeio.hpp"
#include "ed/stlio.hpp"

#include "common/assert_verify.hpp"

namespace schematic::format
{

using namespace std::string_literals;

const std::string Node::Feature::Point::TYPE       = "Point"s;
const std::string Node::Feature::Contour::TYPE     = "Contour"s;
const std::string Node::Feature::Pin::TYPE         = "Pin"s;
const std::string Node::Feature::LineSegment::TYPE = "LineSegment"s;
const std::string Node::Property::TYPE             = "Property"s;
const std::string Node::Site::Space::TYPE          = "Space"s;
const std::string Node::Site::Wall::TYPE           = "Wall"s;
const std::string Node::Site::Object::TYPE         = "Object"s;
const std::string Node::Site::Connection::TYPE     = "Connection"s;
const std::string Node::Site::Cut::TYPE            = "Cut"s;
const std::string Node::File::Schematic::TYPE      = "Schematic"s;

namespace
{

template < typename T >
void loadProperty( const Ed::Node& parentNode, const std::string& strName, T& value )
{
    auto foundNode = Ed::find( parentNode, strName );
    VERIFY_RTE_MSG( foundNode.has_value(),
                    "Failed to locate node: " << strName << " in: " << parentNode.statement.getIdentifier() );
    auto shorthandOpt = foundNode.value().getShorty();
    VERIFY_RTE_MSG( shorthandOpt.has_value(), "Node: " << strName << " is missing value" );
    Ed::IShorthandStream is( shorthandOpt.value() );
    is >> value;
}

} // namespace

void Node::load( const Ed::Node& edNode )
{
    const std::string strType = edNode.statement.getIdentifier();
    {
        auto shorthandOpt = edNode.getShorty();
        VERIFY_RTE_MSG( shorthandOpt.has_value(), "Node: " << strType << " is missing value" );

        Ed::IShorthandStream is( shorthandOpt.value() );

        {
            const Ed::ArgumentVariant&               arg = is.read();
            boost::optional< const Ed::Expression& > opt
                = boost::apply_visitor( boost::TypeAccessor< const Ed::Expression >(), arg );
            VERIFY_RTE_MSG( opt.has_value(), "Type name has incorrect value: " << strType );
            name = opt.value().string();
        }
        {
            const Ed::ArgumentVariant&       arg = is.read();
            boost::optional< const double& > opt = boost::apply_visitor( boost::TypeAccessor< const double >(), arg );
            VERIFY_RTE_MSG( opt.has_value(), "Type name has incorrect index: " << strType );
            index = static_cast< int >( opt.value() );
        }
    }

    if( strType == Node::Feature::Contour::TYPE )
    {
        auto p = mutable_feature()->mutable_contour();
        loadProperty( edNode, ".path", p->path );
    }
    else if( strType == Node::Feature::LineSegment::TYPE )
    {
        auto p = mutable_feature()->mutable_lineSegment();
        loadProperty( edNode, ".start", p->start );
        loadProperty( edNode, ".end", p->end );
    }
    else if( strType == Node::Feature::Pin::TYPE )
    {
        auto p = mutable_feature()->mutable_pin();
        loadProperty( edNode, ".position", p->position );
    }
    else if( strType == Node::Feature::Point::TYPE )
    {
        auto p = mutable_feature()->mutable_point();
        loadProperty( edNode, ".position", p->position );
    }
    else if( strType == Node::File::Schematic::TYPE )
    {
        auto f = mutable_file();
        auto s = f->mutable_schematic();
        (void)s;
    }
    else if( strType == Node::Property::TYPE )
    {
        auto           p = mutable_property();
        Ed::Expression exp;
        loadProperty( edNode, ".value", exp );
        p->value = exp.string();
    }
    else if( strType == Node::Site::Connection::TYPE )
    {
        auto s = mutable_site();
        loadProperty( edNode, ".transform", s->transform );

        auto c = s->mutable_connection();
        (void)c;
    }
    else if( strType == Node::Site::Cut::TYPE )
    {
        auto s = mutable_site();
        loadProperty( edNode, ".transform", s->transform );

        auto c = s->mutable_cut();
        (void)c;
    }
    else if( strType == Node::Site::Object::TYPE )
    {
        auto s = mutable_site();
        loadProperty( edNode, ".transform", s->transform );

        auto o = s->mutable_object();
        (void)o;
    }
    else if( strType == Node::Site::Space::TYPE )
    {
        auto s = mutable_site();
        loadProperty( edNode, ".transform", s->transform );

        auto sp = s->mutable_space();
        (void)sp;
    }
    else if( strType == Node::Site::Wall::TYPE )
    {
        auto s = mutable_site();
        loadProperty( edNode, ".transform", s->transform );

        auto w = s->mutable_wall();
        (void)w;
    }
    else
    {
        THROW_RTE( "Encountered unsupported type: " << strType );
    }

    for( const auto& child : edNode.children )
    {
        const auto& strID = child.statement.getIdentifier();
        if( !strID.empty() && std::isupper( strID.front() ) )
        {
            Node n;
            n.load( child );
            auto ib = children.insert( { n.index, n } );
            VERIFY_RTE_MSG( ib.second, "Node has duplicate index: " << n.name );
        }
    }
}

namespace
{
void saveTypeName( Ed::Node& node, const std::string& strType, const std::string& strName, int index )
{
    node.statement = Ed::Statement{ Ed::Declarator{ strType } };
    node.getShorty().push_back( Ed::Expression{ strName } );
    node.getShorty().push_back( static_cast< double >( index ) );
}

template < typename T >
void saveProperty( Ed::Node& parentNode, const std::string& strName, const T& value )
{
    Ed::Node n{ Ed::Statement{ Ed::Declarator{ strName } } };
    {
        Ed::OShorthandStream os( n.getShorty() );
        os << value;
    }
    parentNode.children.push_back( n );
}
} // namespace

void Node::save( Ed::Node& edNode ) const
{
    if( has_feature() )
    {
        const Feature& f = feature();
        if( f.has_contour() )
        {
            const Feature::Contour& c = f.contour();
            saveTypeName( edNode, Node::Feature::Contour::TYPE, name, index );
            saveProperty( edNode, "path"s, c.path );
        }
        else if( f.has_lineSegment() )
        {
            const Feature::LineSegment& c = f.lineSegment();
            saveTypeName( edNode, Node::Feature::LineSegment::TYPE, name, index );
            saveProperty( edNode, "start"s, c.start );
            saveProperty( edNode, "end"s, c.end );
        }
        else if( f.has_pin() )
        {
            const Feature::Pin& c = f.pin();
            saveTypeName( edNode, Node::Feature::Pin::TYPE, name, index );
            saveProperty( edNode, "position"s, c.position );
        }
        else if( f.has_point() )
        {
            const Feature::Point& c = f.point();
            saveTypeName( edNode, Node::Feature::Point::TYPE, name, index );
            saveProperty( edNode, "position"s, c.position );
        }
        else
        {
            THROW_RTE( "Node::save not implemeting type: " << f.type.type().name() );
        }
    }
    else if( has_file() )
    {
        const File& f = file();
        if( f.has_schematic() )
        {
            // NOTE - not saving name due to issues with file paths in ed
            saveTypeName( edNode, Node::File::Schematic::TYPE, "base", index );
        }
        else
        {
            THROW_RTE( "Node::save not implemeting type: " << f.type.type().name() );
        }
    }
    else if( has_property() )
    {
        const Property& p = property();
        saveTypeName( edNode, Node::Property::TYPE, name, index );
        saveProperty( edNode, "value"s, Ed::Expression{ p.value } );
    }
    else if( has_site() )
    {
        const Site& s = site();
        if( s.has_connection() )
        {
            saveTypeName( edNode, Node::Site::Connection::TYPE, name, index );
        }
        else if( s.has_cut() )
        {
            saveTypeName( edNode, Node::Site::Cut::TYPE, name, index );
        }
        else if( s.has_object() )
        {
            saveTypeName( edNode, Node::Site::Object::TYPE, name, index );
        }
        else if( s.has_space() )
        {
            saveTypeName( edNode, Node::Site::Space::TYPE, name, index );
        }
        else if( s.has_wall() )
        {
            saveTypeName( edNode, Node::Site::Wall::TYPE, name, index );
        }
        else
        {
            THROW_RTE( "Node::save not implemeting type: " << s.type.type().name() );
        }
        saveProperty( edNode, "transform"s, s.transform );
    }
    else
    {
        THROW_RTE( "Node::save not implemeting type: " << type.type().name() );
    }

    for( const auto& child : children )
    {
        Ed::Node childEdNode;
        child.second.save( childEdNode );
        edNode.children.push_back( childEdNode );
    }
}

} // namespace schematic::format
