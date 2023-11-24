
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
template < typename T >
class Variable
{
    Type< T >   type;
    std::string name;

public:
    inline Variable( Type< T > type, std::string name )
        : type( std::move( type ) )
        , name( std::move( name ) )
    {
    }
    inline Variable( const Variable& ) = default;

    const Type< T >&   getType() const { return type; }
    const std::string& getName() const { return name; }
};

using MutableVar = Variable< Mutable >;
using ConstVar   = Variable< Const >;

class Literal
{
    Type< Const > type;
    std::string   text;

public:
    inline Literal( Type< Const > type, std::string text )
        : type( std::move( type ) )
        , text( std::move( text ) )
    {
    }
    inline Literal( const Literal& ) = default;

    const Type< Const >& getType() const { return type; }
    const std::string&   getText() const { return text; }
};

/////////////////////////////////////////////////
/////////////////////////////////////////////////
// Expressions
struct Read;
struct ReadLiteral;
struct Call;
struct CallFunctor;
struct Operator;

using Expression = std::variant< Read, ReadLiteral, Call, CallFunctor, Operator >;

struct Read
{
    Variable< Value > variable;
};

struct ReadLiteral
{
    Literal value;
};

struct Call
{
    Type< Function >          function;
    std::vector< Expression > arguments;
};

struct CallFunctor
{
    Variable< Materialiser >  functor;
    Type< Function >          function;
    std::vector< Expression > arguments;
};

struct Operator
{
    std::vector< Expression > operands;
    enum OperandType
    {
        eEqual,
        eNotEqual,
        eAdd
    } type;

    template < typename T >
    static Operator makeIncrement( Variable< T > var )
    {
        return Operator{
            { Read{ var }, ReadLiteral{ Literal{ makeType< Const >( getDataType( *var.getType() ) ), "1" } } },
            Operator::eAdd };
    }
};

/////////////////////////////////////////////////
/////////////////////////////////////////////////
// Statements

struct ExpressionStatement;
struct MaterialiserStatement;
struct Return;
struct VariableDeclaration;
struct Assignment;
struct If;
struct Switch;
struct ForLoop;

using Statement = std::variant< ExpressionStatement, VariableDeclaration, Assignment, MaterialiserStatement, Return, If, Switch, ForLoop >;

struct ExpressionStatement
{
    Expression rValue;
};

struct VariableDeclaration
{
    Variable< Value > lValue;
    std::optional< Expression > rValue;
};

struct Assignment
{
    Variable< Value > lValue;
    Expression        rValue;
};

struct MaterialiserStatement
{
    Variable< Materialiser >  materialiser;
    std::vector< Expression > arguments;
    Type< JIT >               function;
};

struct Return
{
    Variable< Value > rValue;
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
    Variable< Value >        iterator;
    Expression               initialisation;
    Expression               test;
    Expression               increment;
    std::vector< Statement > statements;
};

struct FunctionDefinition
{
    std::string                      name;
    Type< Value >                    returnType;
    std::vector< Variable< Value > > parameters;
    std::vector< Statement >         statements;
};

} // namespace mega::il

#endif // GUARD_2023_November_24_elements
