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

struct Site
{
    std::string           name;
    Transform             transform;
    std::map< int, Site > children;

    struct Space
    {
        Path contour;
        int  width;

        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            archive& boost::serialization::make_nvp( "contour", contour );
            archive& boost::serialization::make_nvp( "width", width );
        }
    };
    struct Wall
    {
        Path contour;
        int  width;

        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            archive& boost::serialization::make_nvp( "contour", contour );
            archive& boost::serialization::make_nvp( "width", width );
        }
    };
    struct Object
    {
        Path contour;

        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            archive& boost::serialization::make_nvp( "contour", contour );
        }
    };
    struct Connection
    {
        double width;
        double half_height;

        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            archive& boost::serialization::make_nvp( "width", width );
            archive& boost::serialization::make_nvp( "half_height", half_height );
        }
    };

    boost::variant< Space, Wall, Object, Connection > type;

    VARIANT_MEMBER( Space, space, 0, type );
    VARIANT_MEMBER( Wall, wall, 1, type );
    VARIANT_MEMBER( Object, object, 2, type );
    VARIANT_MEMBER( Connection, connection, 3, type );

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "name", name );
        archive& boost::serialization::make_nvp( "transform", transform );
        archive& boost::serialization::make_nvp( "type", type );
        archive& boost::serialization::make_nvp( "children", children );
    }
};

struct File
{
    struct Schematic
    {
        struct Clip
        {
            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
            }
        };
        struct Ship
        {
            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
            }
        };
        struct Base
        {
            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
            }
        };
        boost::variant< Clip, Ship, Base > type;

        VARIANT_MEMBER( Clip, clip, 0, type );
        VARIANT_MEMBER( Ship, ship, 1, type );
        VARIANT_MEMBER( Base, base, 2, type );

        std::map< int, Site > children;

        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            archive& boost::serialization::make_nvp( "type", type );
            archive& boost::serialization::make_nvp( "children", children );
        }
    };

    struct Script
    {
        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
        }
    };

    boost::variant< Schematic, Script > type;

    VARIANT_MEMBER( Schematic, schematic, 0, type );
    VARIANT_MEMBER( Script, script, 1, type );

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "type", type );
    }
};
} // namespace schematic::format

#endif // MAP_FORMAT_21_FEB_2023
