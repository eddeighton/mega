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

#include "ed/node.hpp"
#include "ed/nodeio.hpp"
#include "ed/shorthandio.hpp"
#include "ed/stlio.hpp"
/*
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
*/
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
};

inline Ed::IShorthandStream& operator>>( Ed::IShorthandStream& is, F2& f2 )
{
    return is >> f2.x >> f2.y;
}
inline Ed::OShorthandStream& operator<<( Ed::OShorthandStream& os, const F2& f2 )
{
    return os << f2.x << f2.y;
}

struct Transform
{
    double m00;
    double m01;
    double m02;
    double m10;
    double m11;
    double m12;
};

inline Ed::IShorthandStream& operator>>( Ed::IShorthandStream& is, Transform& data )
{
    // clang-format off
    return is
    >> data.m00
    >> data.m01
    >> data.m02
    >> data.m10
    >> data.m11
    >> data.m12
    ;
    // clang-format on
}
inline Ed::OShorthandStream& operator<<( Ed::OShorthandStream& os, const Transform& data )
{
    // clang-format off
    return os
    << data.m00
    << data.m01
    << data.m02
    << data.m10
    << data.m11
    << data.m12
    ;
    // clang-format on
}

struct Path
{
    std::vector< F2 > points;
};

inline Ed::IShorthandStream& operator>>( Ed::IShorthandStream& is, Path& data )
{
    return Ed::serialiseIn( is, data.points );
}
inline Ed::OShorthandStream& operator<<( Ed::OShorthandStream& os, const Path& data )
{
    return Ed::serialiseOut( os, data.points );
}

struct Node
{
    std::string           name;
    std::size_t           index;
    std::map< int, Node > children;

    struct Feature
    {
        struct Point
        {
            static const std::string TYPE;
            F2                       position;

            void load( Ed::IShorthandStream& is );
            void save( Ed::OShorthandStream& os ) const;
        };

        struct Contour
        {
            static const std::string TYPE;
            Path                     path;

            void load( Ed::IShorthandStream& is );
            void save( Ed::OShorthandStream& os ) const;
        };

        struct Pin
        {
            static const std::string TYPE;
            F2                       position;

            void load( Ed::IShorthandStream& is );
            void save( Ed::OShorthandStream& os ) const;
        };

        struct LineSegment
        {
            static const std::string TYPE;
            F2                       start;
            F2                       end;

            void load( Ed::IShorthandStream& is );
            void save( Ed::OShorthandStream& os ) const;
        };

        boost::variant< Point, Contour, Pin, LineSegment > type;

        VARIANT_MEMBER( Point, point, 0, type );
        VARIANT_MEMBER( Contour, contour, 1, type );
        VARIANT_MEMBER( Pin, pin, 2, type );
        VARIANT_MEMBER( LineSegment, lineSegment, 3, type );

        void load( Ed::IShorthandStream& is );
        void save( Ed::OShorthandStream& os );
    };

    struct Property
    {
        static const std::string TYPE;
        std::string              value;

        void load( Ed::IShorthandStream& is );
        void save( Ed::OShorthandStream& os ) const;
    };

    struct Site
    {
        Transform transform;

        struct Space
        {
            static const std::string TYPE;
            void                     load( Ed::IShorthandStream& is );
            void                     save( Ed::OShorthandStream& os ) const;
        };
        struct Wall
        {
            static const std::string TYPE;
            void                     load( Ed::IShorthandStream& is );
            void                     save( Ed::OShorthandStream& os ) const;
        };
        struct Object
        {
            static const std::string TYPE;
            void                     load( Ed::IShorthandStream& is );
            void                     save( Ed::OShorthandStream& os ) const;
        };
        struct Connection
        {
            static const std::string TYPE;
            void                     load( Ed::IShorthandStream& is );
            void                     save( Ed::OShorthandStream& os ) const;
        };
        struct Cut
        {
            static const std::string TYPE;
            void                     load( Ed::IShorthandStream& is );
            void                     save( Ed::OShorthandStream& os ) const;
        };

        boost::variant< Space, Wall, Object, Connection, Cut > type;

        VARIANT_MEMBER( Space, space, 0, type );
        VARIANT_MEMBER( Wall, wall, 1, type );
        VARIANT_MEMBER( Object, object, 2, type );
        VARIANT_MEMBER( Connection, connection, 3, type );
        VARIANT_MEMBER( Cut, cut, 4, type );

        void load( Ed::IShorthandStream& is );
        void save( Ed::OShorthandStream& os ) const;
    };

    struct File
    {
        struct Schematic
        {
            static const std::string TYPE;
            void                     load( Ed::IShorthandStream& is );
            void                     save( Ed::OShorthandStream& os ) const;
        };

        boost::variant< Schematic > type;

        VARIANT_MEMBER( Schematic, schematic, 0, type );

        void load( Ed::IShorthandStream& is );
        void save( Ed::OShorthandStream& os ) const;
    };

    boost::variant< Feature, Property, Site, File > type;
    VARIANT_MEMBER( Feature, feature, 0, type );
    VARIANT_MEMBER( Property, property, 1, type );
    VARIANT_MEMBER( Site, site, 2, type );
    VARIANT_MEMBER( File, file, 3, type );

    void load( Ed::IShorthandStream& is );
    void save( Ed::OShorthandStream& os ) const;

    void load( const Ed::Node& edNode );
    void save( Ed::Node& edNode ) const;
};

} // namespace schematic::format

#endif // MAP_FORMAT_21_FEB_2023
