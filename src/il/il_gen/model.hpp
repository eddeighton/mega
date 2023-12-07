
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

#ifndef GUARD_2023_November_26_analysis
#define GUARD_2023_November_26_analysis

#include <vector>
#include <string>
#include <variant>
#include <ostream>
#include <sstream>

namespace il_gen
{
using Namespaces = std::vector< std::string >;

inline std::ostream& operator<<( std::ostream& os, const Namespaces& namespaces )
{
    for( const auto& name : namespaces )
    {
        os << name << "::";
    }
    return os;
}

struct ValueType
{
    std::string name;
    inline bool operator==( const ValueType& cmp ) const = default;
};

struct Mutable
{
    ValueType   valueType;
    inline bool operator==( const Mutable& cmp ) const = default;
};
struct Const
{
    ValueType   valueType;
    inline bool operator==( const Const& cmp ) const = default;
};
struct Ref
{
    ValueType   valueType;
    inline bool operator==( const Ref& cmp ) const = default;
};
struct Ptr
{
    ValueType   valueType;
    inline bool operator==( const Ptr& cmp ) const = default;
};
struct ConstRef
{
    ValueType   valueType;
    inline bool operator==( const ConstRef& cmp ) const = default;
};
struct ConstPtr
{
    ValueType   valueType;
    inline bool operator==( const ConstPtr& cmp ) const = default;
};
using Type = std::variant< Mutable, Const, Ref, Ptr, ConstRef, ConstPtr >;

inline std::ostream& operator<<( std::ostream& os, const Type& type )
{
    struct TypeNameVisitor
    {
        std::ostream& os;
        void          operator()( const Mutable& v ) { os << v.valueType.name; }
        void          operator()( const Ptr& v ) { os << v.valueType.name << "*"; }
        void          operator()( const Ref& v ) { os << v.valueType.name << "&"; }
        void          operator()( const Const& v ) { os << "const " << v.valueType.name; }
        void          operator()( const ConstRef& v ) { os << "const " << v.valueType.name << "&"; }
        void          operator()( const ConstPtr& v ) { os << "const " << v.valueType.name << "*"; }
    } visitor{ os };
    std::visit( visitor, type );
    return os;
}

inline std::string getVariableType( const Type& type )
{
    std::ostringstream os;
    struct VariableTypeVisitor
    {
        // see mega/src/include/il/elements/types.hpp
        std::ostream& os;
        void          operator()( const Mutable& v ) { os << "Mutable"; }
        void          operator()( const Ptr& v ) { os << "Ptr"; }
        void          operator()( const Ref& v ) { os << "Ref"; }
        void          operator()( const Const& v ) { os << "Const"; }
        void          operator()( const ConstRef& v ) { os << "ConstRef"; }
        void          operator()( const ConstPtr& v ) { os << "ConstPtr"; }
    } visitor{ os };
    std::visit( visitor, type );
    return os.str();
}

inline const ValueType& getDataType( const Type& type )
{
    struct VariableTypeVisitor
    {
        const ValueType& operator()( const Mutable& v ) { return v.valueType; }
        const ValueType& operator()( const Ptr& v ) { return v.valueType; }
        const ValueType& operator()( const Ref& v ) { return v.valueType; }
        const ValueType& operator()( const Const& v ) { return v.valueType; }
        const ValueType& operator()( const ConstRef& v ) { return v.valueType; }
        const ValueType& operator()( const ConstPtr& v ) { return v.valueType; }
    } visitor;
    return std::visit( visitor, type );
}

struct NativeType
{
    Type type;
};

struct InlineType
{
    Type type;
};

struct SlicedType
{
    Type type;
    Type sliceType;
};

struct BoxedType
{
    Type type;
};

using AdaptedType = std::variant< NativeType, InlineType, SlicedType, BoxedType >;

inline std::ostream& operator<<( std::ostream& os, const AdaptedType& adaptedType )
{
    struct TypeNameVisitor
    {
        std::ostream& os;
        void          operator()( const NativeType& v ) { os << v.type; }
        void          operator()( const InlineType& v ) { os << v.type; }
        void          operator()( const SlicedType& v ) { os << v.type; }
        void          operator()( const BoxedType& v ) { os << v.type; }
    } visitor{ os };
    std::visit( visitor, adaptedType );
    return os;
}

inline std::string getCPPType( const AdaptedType& type )
{
    std::ostringstream os;
    os << type;
    return os.str();
}

inline std::string getCPPErasedType( const AdaptedType& type )
{
    std::ostringstream os;
    os << type;
    return os.str();
}

inline std::string getVariableType( const AdaptedType& type )
{
    struct VariableTypeVisitor
    {
        // see mega/src/include/il/elements/types.hpp
        std::string operator()( const NativeType& v ) { return getVariableType( v.type ); }
        std::string operator()( const InlineType& v ) { return getVariableType( v.type ); }
        std::string operator()( const SlicedType& v ) { return getVariableType( v.type ); }
        std::string operator()( const BoxedType& v ) { return getVariableType( v.type ); }
    } visitor;
    return std::visit( visitor, type );
}

inline const ValueType& getDataType( const AdaptedType& type )
{
    struct VariableTypeVisitor
    {
        const ValueType& operator()( const NativeType& v ) { return getDataType( v.type ); }
        const ValueType& operator()( const InlineType& v ) { return getDataType( v.type ); }
        const ValueType& operator()( const SlicedType& v ) { return getDataType( v.type ); }
        const ValueType& operator()( const BoxedType& v ) { return getDataType( v.type ); }
    } visitor;
    return std::visit( visitor, type );
}

struct Variable
{
    AdaptedType type;
    std::string name;
};

inline std::ostream& operator<<( std::ostream& os, const Variable& var )
{
    return os << var.type << ' ' << var.name;
}

struct Function
{
    std::vector< Variable > arguments;
    AdaptedType             returnType;
    std::string             name;
    Namespaces              namespaces;
};

inline std::ostream& operator<<( std::ostream& os, const Function& function )
{
    os << function.returnType << " " << function.namespaces << function.name << '(';

    bool bFirst = true;
    for( const auto& arg : function.arguments )
    {
        if( bFirst )
        {
            bFirst = false;
        }
        else
        {
            os << ',';
        }
        os << arg;
    }

    os << ')';
    return os;
}

struct Materialiser
{
    std::string name;
    std::vector< Variable > arguments;
    std::vector< Function > functions;
};

struct Model
{
    std::vector< Type > nativeTypes;
    std::vector< Type > inlineTypes;
    std::vector< Type > externTypes;

    std::vector< Function >     inlineFunctions;
    std::vector< Function >     externFunctions;
    std::vector< Materialiser > materialisers;

    inline bool isNativeType( const Type& type ) const
    {
        const auto dataType = getDataType( type );
        for( const auto& t : nativeTypes )
        {
            if( getDataType( t ) == dataType )
            {
                return true;
            }
        }
        return false;
    }

    inline bool isInlineType( const Type& type ) const
    {
        const auto dataType = getDataType( type );
        for( const auto& t : inlineTypes )
        {
            if( getDataType( t ) == dataType )
            {
                return true;
            }
        }
        return false;
    }

    inline bool isExternType( const Type& type ) const
    {
        const auto dataType = getDataType( type );
        for( const auto& t : externTypes )
        {
            if( getDataType( t ) == dataType )
            {
                return true;
            }
        }
        return false;
    }
};

} // namespace il_gen

#endif // GUARD_2023_November_26_analysis
