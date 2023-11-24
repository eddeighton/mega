
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

#ifndef GUARD_2023_November_24_types
#define GUARD_2023_November_24_types

#include "types.hxx"

#include <vector>
#include <variant>
#include <string>
#include <memory>
#include <map>

namespace mega::il
{

template < typename T >
using Type = std::shared_ptr< T >;

template < typename T, typename... TArgs >
inline auto makeType( TArgs... args )
{
    return std::make_shared< T >( args... );
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
// Value Types
struct Mutable
{
    DataType type;
};

struct Const
{
    DataType type;
};

struct Ptr
{
    DataType type;
};

struct ConstPtr
{
    DataType type;
};

struct Ref
{
    DataType type;
};

struct ConstRef
{
    DataType type;
};

using Value = std::variant< Mutable, Const, Ptr, ConstPtr, Ref, ConstRef >;

template < typename... TArgs >
inline auto makeValueType( TArgs... args )
{
    return std::make_shared< Value >( args... );
}

inline DataType getDataType( const Value& value )
{
    return std::visit( []( const auto& type ) { return type.type; }, value );
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
// Function Types
struct Function
{
    std::string                  name;
    std::vector< Type< Value > > parameterTypes;
    Type< Value >                returnType;
};

struct AOT : Function
{
};

struct JIT : Function
{
};

struct InlineFunction : AOT
{
};

struct ExternFunction : AOT
{
};

struct FunctionTemplate
{
    Type< JIT > function;
};

struct Materialiser : Value
{
    std::vector< Type< Value > >              parameterTypes;
    std::map< std::size_t, FunctionTemplate > functionTemplates;

    inline const auto& getFunctionTemplate( std::size_t id )
    {
        auto iFind = functionTemplates.find( id );
        VERIFY_RTE_MSG( iFind != functionTemplates.end(), "Failed to locate function template with id: " << id );
        return iFind->second;
    }
    
    inline const auto& getFunctionTemplate( const std::string& strName )
    {
        for( const auto& [ _, f ] : functionTemplates )
        {
            if( f.function->name == strName )
            {
                return f;
            }
        }
        THROW_RTE( "Failed to locate function template with name: " << strName );
    }
};

struct FunctionPtr : Ptr
{
    Type< Function > function;
};

} // namespace mega::il

#endif // GUARD_2023_November_24_types
