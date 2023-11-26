
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

namespace il_gen
{
struct ValueType
{
    std::string name;
};

struct Mutable
{
    ValueType valueType;
};
struct Const
{
    ValueType valueType;
};
struct Ref
{
    ValueType valueType;
};
struct Ptr
{
    ValueType valueType;
};
struct ConstRef
{
    ValueType valueType;
};
struct ConstPtr
{
    ValueType valueType;
};
using Type = std::variant< Mutable, Const, Ref, Ptr, ConstRef, ConstPtr >;

struct TypeNameVisitor
{
    std::ostream& os;
    void operator()( const Mutable& v )
    {
        os << v.valueType.name;
    }
    void operator()( const Ptr& v )
    {
        os << v.valueType.name << "*";
    }
    void operator()( const Ref& v )
    {
        os << v.valueType.name << "&";
    }
    void operator()( const Const& v )
    {
        os << "const " << v.valueType.name;
    }
    void operator()( const ConstRef& v )
    {
        os << "const " << v.valueType.name << "&";
    }
    void operator()( const ConstPtr& v )
    {
        os << "const " << v.valueType.name << "*";
    }
};

inline std::ostream& operator<<( std::ostream& os, const Type& type )
{
    TypeNameVisitor visitor{ os };
    std::visit( visitor, type );
    return os;
}

struct Variable
{
    Type        type;
    std::string name;
};

inline std::ostream& operator<<( std::ostream& os, const Variable& var )
{
    return os << var.type << ' ' << var.name;
}

struct Function
{
    std::vector< Variable > arguments;
    Type                    returnType;
    std::string             name;
};

inline std::ostream& operator<<( std::ostream& os, const Function& function )
{
    os << function.returnType << " " << function.name << '(';

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

} // namespace il_gen

#endif // GUARD_2023_November_26_analysis
