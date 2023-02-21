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

#include <map>
#include <string>
#include <variant>

#define VARIANT_MEMBER( Type, Name, Index, VariantMember ) \
    const Type& Name() const                               \
    {                                                      \
        return std::get< Index >( VariantMember );         \
    }                                                      \
    bool has_##Name() const                                \
    {                                                      \
        return VariantMember.index() == Index;             \
    }                                                      \
    Type* mutable_##Name()                                 \
    {                                                      \
        if( !has_##Name() )                                \
            VariantMember = Type{};                        \
        return &std::get< Index >( VariantMember );        \
    }

namespace map::format
{
struct F2
{
    double x;
    double y;
};

struct Transform
{
    double m00;
    double m01;
    double m02;
    double m10;
    double m11;
    double m12;
};

struct Path
{
    std::map< int, F2 > points;
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
    };
    struct Wall
    {
        Path contour;
        int  width;
    };
    struct Object
    {
        Path contour;
    };
    struct Connection
    {
        double width;
        double half_height;
    };

    std::variant< Space, Wall, Object, Connection > type;

    VARIANT_MEMBER( Space, space, 0, type );
    VARIANT_MEMBER( Wall, wall, 1, type );
    VARIANT_MEMBER( Object, object, 2, type );
    VARIANT_MEMBER( Connection, connection, 3, type );
};

struct File
{
    struct Schematic
    {
        struct Clip
        {
        };
        struct Ship
        {
        };
        struct Base
        {
        };
        std::variant< Clip, Ship, Base > type;

        VARIANT_MEMBER( Clip, clip, 0, type );
        VARIANT_MEMBER( Ship, ship, 1, type );
        VARIANT_MEMBER( Base, base, 2, type );

        std::map< int, Site > children;
    };

    struct Script
    {
    };

    std::variant< Schematic, Script > type;

    VARIANT_MEMBER( Schematic, schematic, 0, type );
    VARIANT_MEMBER( Script, script, 1, type );
};
} // namespace map

#endif // MAP_FORMAT_21_FEB_2023
