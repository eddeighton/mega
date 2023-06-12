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

#include "common/assert_verify.hpp"

namespace schematic::format
{
using namespace std::string_literals;

const std::string Node::Feature::Point::TYPE = "Point"s;

void Node::Feature::Point::load( Ed::IShorthandStream& is )
{
    is >> position;
}
void Node::Feature::Point::save( Ed::OShorthandStream& os ) const
{
    os << position;
}

const std::string Node::Feature::Contour::TYPE = "Contour"s;
void              Node::Feature::Contour::load( Ed::IShorthandStream& is )
{
    is >> path;
}
void Node::Feature::Contour::save( Ed::OShorthandStream& os ) const
{
    os << path;
}

const std::string Node::Feature::Pin::TYPE = "Pin"s;
void              Node::Feature::Pin::load( Ed::IShorthandStream& is )
{
    is >> position;
}
void Node::Feature::Pin::save( Ed::OShorthandStream& os ) const
{
    os << position;
}

const std::string Node::Feature::LineSegment::TYPE = "LineSegment"s;
void              Node::Feature::LineSegment::load( Ed::IShorthandStream& is )
{
    is >> start >> end;
}
void Node::Feature::LineSegment::save( Ed::OShorthandStream& os ) const
{
    os << start << end;
}

void Node::Feature::load( Ed::IShorthandStream& is )
{
}
void Node::Feature::save( Ed::OShorthandStream& os )
{
}

const std::string Node::Property::TYPE = "Property"s;
void              Node::Property::load( Ed::IShorthandStream& is )
{
    is >> value;
}
void Node::Property::save( Ed::OShorthandStream& os ) const
{
    os << value;
}

const std::string Node::Site::Space::TYPE = "Space"s;
void              Node::Site::Space::load( Ed::IShorthandStream& is )
{
}
void Node::Site::Space::save( Ed::OShorthandStream& os ) const
{
}

const std::string Node::Site::Wall::TYPE = "Wall"s;
void              Node::Site::Wall::load( Ed::IShorthandStream& is )
{
}
void Node::Site::Wall::save( Ed::OShorthandStream& os ) const
{
}

const std::string Node::Site::Object::TYPE = "Object"s;
void              Node::Site::Object::load( Ed::IShorthandStream& is )
{
}
void Node::Site::Object::save( Ed::OShorthandStream& os ) const
{
}

const std::string Node::Site::Connection::TYPE = "Connection"s;
void              Node::Site::Connection::load( Ed::IShorthandStream& is )
{
}
void Node::Site::Connection::save( Ed::OShorthandStream& os ) const
{
}

const std::string Node::Site::Cut::TYPE = "Cut"s;
void              Node::Site::Cut::load( Ed::IShorthandStream& is )
{
}
void Node::Site::Cut::save( Ed::OShorthandStream& os ) const
{
}

void Node::Site::load( Ed::IShorthandStream& is )
{
    is >> transform;
}
void Node::Site::save( Ed::OShorthandStream& os ) const
{
    os << transform;
}

const std::string Node::File::Schematic::TYPE = "Schematic"s;
void              Node::File::Schematic::load( Ed::IShorthandStream& is )
{
}
void Node::File::Schematic::save( Ed::OShorthandStream& os ) const
{
}

void Node::File::load( Ed::IShorthandStream& is )
{
}
void Node::File::save( Ed::OShorthandStream& os ) const
{
}

void Node::load( Ed::IShorthandStream& is )
{
    is >> name;
}
void Node::save( Ed::OShorthandStream& os ) const
{
    os << name;
}

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
    const std::string type = edNode.statement.getIdentifier();
    {
        auto shorthandOpt = edNode.getShorty();
        VERIFY_RTE_MSG( shorthandOpt.has_value(), "Node: " << type << " is missing value" );

        Ed::IShorthandStream is( shorthandOpt.value() );

        const Ed::ArgumentVariant&               arg = is.read();
        boost::optional< const Ed::Expression& > opt
            = boost::apply_visitor( boost::TypeAccessor< const Ed::Expression >(), arg );
        VERIFY_RTE_MSG( opt.has_value(), "Type name has incorrect value: " << type );
        if( opt.has_value() )
        {
            name = opt.value().string();
        }
    }

    if( type == Node::Feature::Contour::TYPE )
    {
        auto p = mutable_feature()->mutable_contour();
        loadProperty( edNode, ".path", p->path );
    }
    else if( type == Node::Feature::LineSegment::TYPE )
    {
        auto p = mutable_feature()->mutable_lineSegment();
        loadProperty( edNode, ".start", p->start );
        loadProperty( edNode, ".end", p->end );
    }
    else if( type == Node::Feature::Pin::TYPE )
    {
        auto p = mutable_feature()->mutable_pin();
        loadProperty( edNode, ".position", p->position );
    }
    else if( type == Node::Feature::Point::TYPE )
    {
        auto p = mutable_feature()->mutable_point();
        loadProperty( edNode, ".position", p->position );
    }
    else if( type == Node::File::Schematic::TYPE )
    {
        auto p = mutable_file()->mutable_schematic();
    }
    else if( type == Node::Property::TYPE )
    {
        auto           p = mutable_property();
        Ed::Expression exp;
        loadProperty( edNode, ".value", exp );
        p->value = exp.string();
    }
    else if( type == Node::Site::Connection::TYPE )
    {
        auto s = mutable_site();
        loadProperty( edNode, ".transform", s->transform );
        auto p = s->mutable_connection();
    }
    else if( type == Node::Site::Cut::TYPE )
    {
        auto s = mutable_site();
        loadProperty( edNode, ".transform", s->transform );
        auto p = s->mutable_cut();
    }
    else if( type == Node::Site::Object::TYPE )
    {
        auto s = mutable_site();
        loadProperty( edNode, ".transform", s->transform );
        auto p = s->mutable_object();
    }
    else if( type == Node::Site::Space::TYPE )
    {
        auto s = mutable_site();
        loadProperty( edNode, ".transform", s->transform );
        auto p = s->mutable_space();
    }
    else if( type == Node::Site::Wall::TYPE )
    {
        auto s = mutable_site();
        loadProperty( edNode, ".transform", s->transform );
        auto p = s->mutable_wall();
    }
    else
    {
        THROW_RTE( "Encountered unsupported type: " << type );
    }

    int i = 0;
    for( const auto& child : edNode.children )
    {
        const auto& strID = child.statement.getIdentifier();
        if( !strID.empty() && std::isupper( strID.front() ) )
        {
            Node n;
            n.load( child );
            children.insert( { i++, n } );
        }
    }
}

namespace
{
void saveTypeName( Ed::Node& node, const std::string& strType, const std::string& strName )
{
    node.statement = Ed::Statement{ Ed::Declarator{ strType } };
    Ed::OShorthandStream os( node.getShorty() );
    os << Ed::Expression{ strName };
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
            saveTypeName( edNode, Node::Feature::Contour::TYPE, name );
            saveProperty( edNode, "path"s, c.path );
        }
        else if( f.has_lineSegment() )
        {
            const Feature::LineSegment& c = f.lineSegment();
            saveTypeName( edNode, Node::Feature::LineSegment::TYPE, name );
            saveProperty( edNode, "start"s, c.start );
            saveProperty( edNode, "end"s, c.end );
        }
        else if( f.has_pin() )
        {
            const Feature::Pin& c = f.pin();
            saveTypeName( edNode, Node::Feature::Pin::TYPE, name );
            saveProperty( edNode, "position"s, c.position );
        }
        else if( f.has_point() )
        {
            const Feature::Point& c = f.point();
            saveTypeName( edNode, Node::Feature::Point::TYPE, name );
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
            const File::Schematic& s = f.schematic();
            // NOTE - not saving name due to issues with file paths in ed
            saveTypeName( edNode, Node::File::Schematic::TYPE, "base" );
        }
        else
        {
            THROW_RTE( "Node::save not implemeting type: " << f.type.type().name() );
        }
    }
    else if( has_property() )
    {
        const Property& p = property();
        saveTypeName( edNode, Node::Property::TYPE, name );
        saveProperty( edNode, "value"s, Ed::Expression{ p.value } );
    }
    else if( has_site() )
    {
        const Site& s = site();
        if( s.has_connection() )
        {
            const Site::Connection& c = s.connection();
            saveTypeName( edNode, Node::Site::Connection::TYPE, name );
        }
        else if( s.has_cut() )
        {
            const Site::Cut& c = s.cut();
            saveTypeName( edNode, Node::Site::Cut::TYPE, name );
        }
        else if( s.has_object() )
        {
            const Site::Object& o = s.object();
            saveTypeName( edNode, Node::Site::Object::TYPE, name );
        }
        else if( s.has_space() )
        {
            const Site::Space& o = s.space();
            saveTypeName( edNode, Node::Site::Space::TYPE, name );
        }
        else if( s.has_wall() )
        {
            const Site::Wall& o = s.wall();
            saveTypeName( edNode, Node::Site::Wall::TYPE, name );
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
