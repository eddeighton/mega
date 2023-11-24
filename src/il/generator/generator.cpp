
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

#include "il/generator/generator.hpp"

#include "common/assert_verify.hpp"

#include <sstream>

namespace mega::il
{

class Printer
{
    int                m_indentSize = 2;
    int                m_indent     = 0;
    std::ostringstream m_os;

    void printIndent()
    {
        for( int i = 0; i != m_indent; ++i )
        {
            m_os << ' ';
        }
    }

public:
    std::ostream& line()
    {
        m_os << '\n';
        printIndent();
        return m_os;
    }
    std::ostream& operator()() { return m_os; }

    void indent() { m_indent += m_indentSize; }
    void outdent() { m_indent -= m_indentSize; }

    std::string str() const { return m_os.str(); }
};

void generate( const Type< Value >& valueType, Printer& print )
{
    struct Visitor
    {
        Printer& print;
        void     operator()( const Mutable& type ) const { print() << typeName( type.type ); }
        void     operator()( const Const& type ) const { print() << "const " << typeName( type.type ); }
        void     operator()( const Ptr& type ) const { print() << typeName( type.type ) << "*"; }
        void     operator()( const ConstPtr& type ) const { print() << "const " << typeName( type.type ) << "*"; }
        void     operator()( const Ref& type ) const { print() << typeName( type.type ) << "&"; }
        void     operator()( const ConstRef& type ) const { print() << "const " << typeName( type.type ) << "&"; }

    } visitor{ print };
    std::visit( visitor, *valueType );
}

template < typename T >
void generate( const Variable< T >& var, Printer& print )
{
    generate( var.getType(), print );
    print() << ' ';
    print() << var.getName();
}

void generate( const Expression& exp, Printer& print )
{
    struct Visitor
    {
        Printer& print;

        void operator()( const ReadLiteral& literal ) const { print() << literal.value.getText(); }

        void operator()( const Read& read ) const { print() << read.variable.getName(); }

        void operator()( const Call& call ) const
        {
            VERIFY_RTE_MSG( !call.function->name.empty(), "Function with no name" );
            print() << call.function->name << '(';

            VERIFY_RTE_MSG(
                call.function->parameterTypes.size() == call.arguments.size(), "Function call parameters mismatch" );
            bool bFirst = true;
            for( const auto& arg : call.arguments )
            {
                if( bFirst )
                {
                    bFirst = false;
                }
                else
                {
                    print() << ',';
                }
                generate( arg, print );
            }
            print() << ')';
        }

        void operator()( const Operator& op ) const
        {
            VERIFY_RTE_MSG( op.operands.size() == 2U, "Invalid number of operands in operator" );

            generate( op.operands.front(), print );
            switch( op.type )
            {
                case Operator::eEqual:
                    print() << "==";
                    break;
                case Operator::eNotEqual:
                    print() << "!=";
                    break;
                case Operator::eAdd:
                    print() << "+";
                    break;
                default:
                    THROW_RTE( "Unknown operator type" );
                    break;
            }
            generate( op.operands.back(), print );
        }

    } visitor{ print };
    std::visit( visitor, exp );
}

void generate( const Statement& statement, Printer& print )
{
    struct Visitor
    {
        Printer& print;

        void operator()( const ExpressionStatement& expressionStatement ) const
        {
            print.line();
            generate( expressionStatement.rValue, print );
            print() << ";";
        }

        void operator()( const Return& returnStatement ) const
        {
            print.line() << "return " << returnStatement.rValue.getName() << ";";
        }

        void operator()( const If& ifStatement ) const
        {
            VERIFY_RTE_MSG( !ifStatement.cases.empty(), "If statement with no cases" );

            bool bFirst = true;
            for( const auto& cas : ifStatement.cases )
            {
                if( bFirst )
                {
                    bFirst = false;
                    print.line() << "if";
                }
                else
                {
                    print.line() << "else if";
                }

                print() << "(";
                generate( cas.test, print );
                print() << ")";
                print.line() << "{";
                print.indent();
                for( const auto& s : cas.statements )
                {
                    generate( s, print );
                }
                print.outdent();
                print.line() << "}";
            }
            if( ifStatement.elseCase.has_value() )
            {
                print.line() << "else";
                print.line() << "{";
                print.indent();
                for( const auto& s : ifStatement.elseCase.value().statements )
                {
                    generate( s, print );
                }
                print.outdent();
                print.line() << "}";
            }
        }

        void operator()( const Assignment& assignment ) const
        {
            print.line();
            generate( assignment.lValue, print );
            print() << " = ";
            generate( assignment.rValue, print );
            print() << ";";
        }

        void operator()( const Switch& switchStatement ) const
        {
            print.line() << "switch( ";
            generate( switchStatement.test, print );
            print() << " )";
            print.line() << "{";
            print.indent();
            for( const auto& cas : switchStatement.cases )
            {
                print.line() << "case " << cas.value.getText() << ":";
                print.line() << "{";
                print.indent();
                for( const auto& s : cas.statements )
                {
                    generate( s, print );
                }
                print.outdent();
                print.line() << "}";
                print.line() << "break;";
            }
            print.outdent();
            print.line() << "}";
        }

        void operator()( const ForLoop& forLoop ) const
        {
            print.line() << "for(";
            generate( forLoop.iterator, print );
            print() << " = ";
            generate( forLoop.initialisation, print );
            print() << ";";
            generate( forLoop.test, print );
            print() << ";";
            print() << forLoop.iterator.getName() << "=";
            generate( forLoop.increment, print );
            print() << ")";
            print.line() << "{";
            print.indent();
            for( const auto& s : forLoop.statements )
            {
                generate( s, print );
            }
            print.outdent();
            print.line() << "}";
        }

    } visitor{ print };
    std::visit( visitor, statement );
}

std::string generate( const FunctionDefinition& functionDef )
{
    Printer print;

    generate( functionDef.returnType, print );

    print() << ' ';

    VERIFY_RTE_MSG( !functionDef.name.empty(), "Empty function name" );
    print() << functionDef.name;

    print() << '(';

    bool bFirst = true;
    for( const auto& param : functionDef.parameters )
    {
        if( bFirst )
        {
            bFirst = false;
        }
        else
        {
            print() << ',';
        }
        generate( param, print );
    }

    print() << ')';
    print.line() << '{';
    print.indent();

    for( const auto& statement : functionDef.statements )
    {
        generate( statement, print );
    }

    print.outdent();
    print.line() << '}';

    return print.str();
}

} // namespace mega::il