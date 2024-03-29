
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

#ifndef GUARD_2023_November_24_elements
#define GUARD_2023_November_24_elements

#include <vector>
#include <variant>
#include <string>
#include <optional>

#include "types.hpp"

namespace mega::il
{
template < typename TValueType >
class Variable
{
    TValueType  value;
    std::string name;

public:
    inline Variable( TValueType _value, std::string _name )
        : value( std::move( _value ) )
        , name( std::move( _name ) )
    {
    }

    template < typename TOtherValueType >
    inline Variable( TOtherValueType _value, std::string _name )
        : value( std::move( _value ) )
        , name( std::move( _name ) )
    {
    }

    inline Variable( const Variable& ) = default;

    template < typename TOtherValueType >
    inline Variable( const Variable& cpy )
        : value( cpy.value )
        , name( cpy.name )
    {
    }

    template < typename TOtherValueType >
    inline Variable( Variable&& cpy )
        : value( std::move( cpy.value ) )
        , name( std::move( cpy.name ) )
    {
    }

    const TValueType&  getValueType() const { return value; }
    const std::string& getName() const { return name; }
};

using MutableVar = Variable< Mutable >;
using ConstVar   = Variable< Const >;

class Literal
{
    ConstValueType type;
    std::string    text;

public:
    inline Literal( ConstValueType _type, std::string _text )
        : type( std::move( _type ) )
        , text( std::move( _text ) )
    {
    }
    inline Literal( DataType dataType, std::string _text )
        : type{ Const{ dataType } }
        , text( std::move( _text ) )
    {
    }
    inline Literal( const Literal& ) = default;

    const ConstValueType& getValueType() const { return type; }
    const std::string&    getText() const { return text; }
};

/////////////////////////////////////////////////
/////////////////////////////////////////////////
// Expressions
struct Read;
struct ReadLiteral;
struct Cast;
struct Call;
struct CallFunctor;
struct Operator;

using Expression = std::variant< Read, ReadLiteral, Cast, Call, CallFunctor, Operator >;

struct Read
{
    Variable< ValueType > variable;
};

struct ReadLiteral
{
    Literal value;
};

struct Cast
{
    std::vector< Expression > arguments;
    ValueType                 type;
};

struct Call
{
    Function                  function;
    std::vector< Expression > arguments;
};

struct CallFunctor
{
    Variable< Materialiser >  functor;
    Function                  function;
    std::vector< Expression > arguments;
};

struct Operator
{
    std::vector< Expression > operands;
    enum OperandType
    {
        eEqual,
        eNotEqual,
        eAdd,
        eIndex,
        eAddressOf
    } type;

    template < typename T >
    static Operator makeIncrement( Variable< T > var )
    {
        return Operator{
            { Read{ var }, ReadLiteral{ Literal{ getDataType( var.getValueType() ), "1" } } }, Operator::eAdd };
    }

    template < typename T >
    static Operator makeIncrement( Variable< T > var, std::string amt )
    {
        return Operator{
            { Read{ var }, ReadLiteral{ Literal{ getDataType( var.getValueType() ), amt } } }, Operator::eAdd };
    }
};

/////////////////////////////////////////////////
/////////////////////////////////////////////////
// Statements

struct ExpressionStatement;
struct MaterialiserStatement;
struct Return;
struct VariableDeclaration;
struct ArrayVariableDeclaration;
struct Assignment;
struct Scope;
struct If;
struct Switch;
struct ForLoop;

using Statement = std::variant< ExpressionStatement, VariableDeclaration, ArrayVariableDeclaration, Assignment,
                                MaterialiserStatement, Return, Scope, If, Switch, ForLoop >;

struct ExpressionStatement
{
    Expression rValue;
};

struct VariableDeclaration
{
    Variable< ValueType >       lValue;
    std::optional< Expression > rValue;
};

struct ArrayVariableDeclaration
{
    Variable< ValueType >     lValue;
    std::vector< Expression > initialisations;
};

struct Assignment
{
    Variable< ValueType > lValue;
    Expression            rValue;
};

struct MaterialiserStatement
{
    Variable< Materialiser >  materialiser;
    std::vector< Expression > arguments;
    JIT                       function;
};

struct Return
{
    Variable< ValueType > rValue;
};

struct Scope
{
    std::vector< Statement > statements;
};

struct If
{
    struct Case
    {
        Expression               test;
        std::vector< Statement > statements;
    };
    struct ElseCase
    {
        std::vector< Statement > statements;
    };
    std::vector< Case >       cases;
    std::optional< ElseCase > elseCase;
};

struct Switch
{
    Expression test;
    struct Case
    {
        Literal                  value;
        std::vector< Statement > statements;
    };
    std::vector< Case > cases;
};

struct ForLoop
{
    Variable< ValueType >    iterator;
    Expression               initialisation;
    Expression               test;
    Expression               increment;
    std::vector< Statement > statements;
};

struct FunctionDefinition
{
    std::string                          name;
    ValueType                            returnType;
    std::vector< Variable< ValueType > > parameters;
    std::vector< Statement >             statements;
};

} // namespace mega::il

#endif // GUARD_2023_November_24_elements
