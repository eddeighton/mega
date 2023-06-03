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

#ifndef MAP_FORMAT_21_FEB_2023
#define MAP_FORMAT_21_FEB_2023

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <boost/serialization/map.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/utility.hpp>

#include <boost/variant.hpp>

#include <map>
#include <string>

#define VARIANT_MEMBER( Type, Name, Index, VariantMember )      \
    const Type& Name() const                                    \
    {                                                           \
        return *boost::get< Type >( &VariantMember );           \
    }                                                           \
    bool has_##Name() const                                     \
    {                                                           \
        return boost::get< Type >( &VariantMember ) != nullptr; \
    }                                                           \
    Type* mutable_##Name()                                      \
    {                                                           \
        if( !has_##Name() )                                     \
            VariantMember = Type{};                             \
        return boost::get< Type >( &VariantMember );            \
    }

namespace schematic::format
{
struct F2
{
    double x;
    double y;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "x", x );
        archive& boost::serialization::make_nvp( "y", y );
    }
};

struct Transform
{
    double m00;
    double m01;
    double m02;
    double m10;
    double m11;
    double m12;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "m00", m00 );
        archive& boost::serialization::make_nvp( "m01", m01 );
        archive& boost::serialization::make_nvp( "m02", m02 );
        archive& boost::serialization::make_nvp( "m10", m10 );
        archive& boost::serialization::make_nvp( "m11", m11 );
        archive& boost::serialization::make_nvp( "m12", m12 );
    }
};

struct Path
{
    std::map< int, F2 > points;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "points", points );
    }
};

struct Node
{
    std::string           name;
    std::size_t           index;
    std::map< int, Node > children;

    struct Feature
    {
        struct Point
        {
            F2 position;

            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
                archive& boost::serialization::make_nvp( "position", position );
            }
        };

        struct Contour
        {
            Path path;

            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
                archive& boost::serialization::make_nvp( "path", path );
            }
        };

        struct Pin
        {
            F2 position;

            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
                archive& boost::serialization::make_nvp( "position", position );
            }
        };

        struct LineSegment
        {
            F2 start;
            F2 end;

            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
                archive& boost::serialization::make_nvp( "start", start );
                archive& boost::serialization::make_nvp( "end", end );
            }
        };

        boost::variant< Point, Contour, Pin, LineSegment > type;

        VARIANT_MEMBER( Point, point, 0, type );
        VARIANT_MEMBER( Contour, contour, 1, type );
        VARIANT_MEMBER( Pin, pin, 2, type );
        VARIANT_MEMBER( LineSegment, lineSegment, 3, type );

        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            archive& boost::serialization::make_nvp( "type", type );
        }
    };

    struct Property
    {
        std::string value;

        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            archive& boost::serialization::make_nvp( "value", value );
        }
    };

    struct Site
    {
        Transform transform;

        struct Space
        {
            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
            }
        };
        struct Wall
        {
            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
            }
        };
        struct Object
        {
            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
            }
        };
        struct Connection
        {
            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
            }
        };
        struct Cut
        {
            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
            }
        };

        boost::variant< Space, Wall, Object, Connection, Cut > type;

        VARIANT_MEMBER( Space, space, 0, type );
        VARIANT_MEMBER( Wall, wall, 1, type );
        VARIANT_MEMBER( Object, object, 2, type );
        VARIANT_MEMBER( Connection, connection, 3, type );
        VARIANT_MEMBER( Cut, cut, 4, type );

        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            archive& boost::serialization::make_nvp( "type", type );
            archive& boost::serialization::make_nvp( "transform", transform );
        }
    };

    struct File
    {
        struct Schematic
        {
            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
            }
        };

        boost::variant< Schematic > type;

        VARIANT_MEMBER( Schematic, schematic, 0, type );

        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            archive& boost::serialization::make_nvp( "type", type );
        }
    };

    boost::variant< Feature, Property, Site, File > type;
    VARIANT_MEMBER( Feature, feature, 0, type );
    VARIANT_MEMBER( Property, property, 1, type );
    VARIANT_MEMBER( Site, site, 2, type );
    VARIANT_MEMBER( File, file, 3, type );

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "type", type );
        archive& boost::serialization::make_nvp( "name", name );
        archive& boost::serialization::make_nvp( "children", children );
    }
};

} // namespace schematic::format

#endif // MAP_FORMAT_21_FEB_2023
