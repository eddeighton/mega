
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

#include "il/backend/backend.hpp"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

#include "common/assert_verify.hpp"

#include <sstream>

namespace mega::il
{
namespace
{
class Printer
{
    llvm::LLVMContext m_context;
    llvm::Module      m_module;
    llvm::IRBuilder<> m_irBuilder;

public:
    Printer()
        : m_module( "Mega JIT", m_context )
        , m_irBuilder( m_context )
    {
    }

    void result() 
    {
        

    }
};

void generate( const ValueType& valueType, Printer& print )
{
    struct Visitor
    {
        Printer& print;
        void     operator()( const Mutable& type ) const { ; }
        void     operator()( const Const& type ) const { ; }
        void     operator()( const Ptr& type ) const { ; }
        void     operator()( const ConstPtr& type ) const { ; }
        void     operator()( const Ref& type ) const { ; }
        void     operator()( const ConstRef& type ) const { ; }

    } visitor{ print };
    std::visit( visitor, valueType );
}

template < typename T >
void generate( const Variable< T >& var, Printer& print )
{
}

void generate( const Variable< Materialiser >& var, const JIT& function, Printer& print )
{
}

void generate( const Expression& exp, Printer& print )
{
    struct Visitor
    {
        Printer& print;

        void operator()( const ReadLiteral& literal ) const {}

        void operator()( const Read& read ) const {}

        void operator()( const Cast& cast ) const
        {
            generate( cast.type, print );
            {
                for( const auto& arg : cast.arguments )
                {
                    generate( arg, print );
                }
            }
        }

        void operator()( const Call& call ) const
        {
            VERIFY_RTE_MSG( !call.function.name.empty(), "Function with no name" );
            VERIFY_RTE_MSG( call.function.parameterTypes.size() == call.arguments.size(),
                            "Function call parameters mismatch for: " << call.function.name );
            for( const auto& arg : call.arguments )
            {
                generate( arg, print );
            }
        }

        void operator()( const CallFunctor& callFunctor ) const
        {
            VERIFY_RTE_MSG( !callFunctor.functor.getName().empty(), "Functor with no name" );
            VERIFY_RTE_MSG( callFunctor.function.parameterTypes.size() == callFunctor.arguments.size(),
                            "Functor call parameters count mismatch function type: "
                                << callFunctor.function.parameterTypes.size()
                                << " arguments: " << callFunctor.arguments.size() );

            for( const auto& arg : callFunctor.arguments )
            {
                generate( arg, print );
            }
        }

        void operator()( const Operator& op ) const
        {
            VERIFY_RTE_MSG( !op.operands.empty(), "Invalid number of operands in operator" );
            switch( op.type )
            {
                case Operator::eEqual:
                    VERIFY_RTE_MSG( op.operands.size() == 2, "Invalid number of operands in operator" );
                    break;
                case Operator::eNotEqual:
                    VERIFY_RTE_MSG( op.operands.size() == 2, "Invalid number of operands in operator" );
                    break;
                case Operator::eAdd:
                    VERIFY_RTE_MSG( op.operands.size() == 2, "Invalid number of operands in operator" );
                    break;
                case Operator::eIndex:
                    VERIFY_RTE_MSG( op.operands.size() == 2, "Invalid number of operands in operator" );
                    break;
                case Operator::eAddressOf:
                    VERIFY_RTE_MSG( op.operands.size() == 1, "Invalid number of operands in operator" );
                    break;
                default:
                    THROW_RTE( "Unknown operator type" );
                    break;
            }
            generate( op.operands.front(), print );
            switch( op.type )
            {
                case Operator::eEqual:
                    break;
                case Operator::eNotEqual:
                    break;
                case Operator::eAdd:
                    break;
                case Operator::eIndex:
                    break;
                case Operator::eAddressOf:
                    break;
                default:
                    THROW_RTE( "Unknown operator type" );
                    break;
            }
            if( op.operands.size() > 1 )
            {
                generate( op.operands.back(), print );
                switch( op.type )
                {
                    case Operator::eEqual:
                        break;
                    case Operator::eNotEqual:
                        break;
                    case Operator::eAdd:
                        break;
                    case Operator::eIndex:
                        break;
                    case Operator::eAddressOf:
                        break;
                    default:
                        THROW_RTE( "Unknown operator type" );
                        break;
                }
            }
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
            generate( expressionStatement.rValue, print );
        }

        void operator()( const MaterialiserStatement& materialiserStatement ) const
        {
            // generate( materialiserStatement.materialiser, materialiserStatement.function, print );
            for( const auto& param : materialiserStatement.arguments )
            {
                generate( param, print );
            }
        }

        void operator()( const Return& returnStatement ) const {}

        void operator()( const Scope& scope ) const
        {
            for( const auto& s : scope.statements )
            {
                generate( s, print );
            }
        }

        void operator()( const If& ifStatement ) const
        {
            VERIFY_RTE_MSG( !ifStatement.cases.empty(), "If statement with no cases" );

            for( const auto& cas : ifStatement.cases )
            {
                // generate( cas.test, print );
                for( const auto& s : cas.statements )
                {
                    generate( s, print );
                }
            }
            if( ifStatement.elseCase.has_value() )
            {
                for( const auto& s : ifStatement.elseCase.value().statements )
                {
                    generate( s, print );
                }
            }
        }

        void operator()( const VariableDeclaration& varDecl ) const
        {
            // generate( varDecl.lValue, print );
            // if( varDecl.rValue.has_value() )
            // {
            //     generate( varDecl.rValue.value(), print );
            // }
        }

        void operator()( const ArrayVariableDeclaration& varDecl ) const
        {
            // generate( varDecl.lValue, print );
            VERIFY_RTE_MSG( !varDecl.initialisations.empty(), "Array variable declaration with no initialisers" );
            // print() << "[ " << varDecl.initialisations.size() << " ] = ";
            for( const auto& init : varDecl.initialisations )
            {
                generate( init, print );
            }
        }

        void operator()( const Assignment& assignment ) const
        {
            // assignment.lValue.getName()
            generate( assignment.rValue, print );
        }

        void operator()( const Switch& switchStatement ) const
        {
            generate( switchStatement.test, print );
            for( const auto& cas : switchStatement.cases )
            {
                // cas.value.getText()
                for( const auto& s : cas.statements )
                {
                    generate( s, print );
                }
            }
        }

        void operator()( const ForLoop& forLoop ) const
        {
            // generate( forLoop.iterator, print );
            // generate( forLoop.initialisation, print );
            // generate( forLoop.test, print );
            // print() << forLoop.iterator.getName() << "=";
            // generate( forLoop.increment, print );

            for( const auto& s : forLoop.statements )
            {
                generate( s, print );
            }
        }

    } visitor{ print };
    std::visit( visitor, statement );
}
} // namespace

std::string generateIR( const FunctionDefinition& functionDef )
{
    VERIFY_RTE_MSG( !functionDef.name.empty(), "Empty function name" );

    Printer print;

    // generate( functionDef.returnType, print );
    // print() << functionDef.name;
    // for( const auto& param : functionDef.parameters )
    // {
    //     generate( param, print );
    // }
    for( const auto& statement : functionDef.statements )
    {
        generate( statement, print );
    }

    return "";
}

} // namespace mega::il