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

#include "base_task.hpp"

#include "database/ClangCompilationStage.hxx"

#include "compiler/clang_compilation.hpp"

#include "mega/common_strings.hpp"

#include "mega/values/compilation/hyper_graph.hpp"
#include "mega/values/runtime/pointer.hpp"
#include "mega/make_unique_without_reorder.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include <memory>

namespace ClangCompilationStage
{
#include "compiler/interface_printer.hpp"
#include "compiler/interface_visitor.hpp"
#include "compiler/common_ancestor.hpp"

} // namespace ClangCompilationStage

namespace mega::compiler
{

using namespace ClangCompilationStage;

class Task_CPP_Decl : public BaseTask
{
    const mega::io::manifestFilePath m_manifestFilePath;

public:
    Task_CPP_Decl( const TaskArguments& taskArguments )
        : BaseTask( taskArguments )
        , m_manifestFilePath( m_environment.project_manifest() )
    {
    }

    class Printer
    {
        std::ostream& m_os;
        int           m_indentSize = 2;
        int           m_indent     = 0;

        void printIndent()
        {
            for( int i = 0; i != m_indent; ++i )
            {
                m_os << ' ';
            }
        }

    public:
        Printer( std::ostream& os )
            : m_os( os )
        {
        }

        std::ostream& line()
        {
            m_os << '\n';
            printIndent();
            return m_os;
        }
        std::ostream& operator()() { return m_os; }

        void indent() { m_indent += m_indentSize; }
        void outdent() { m_indent -= m_indentSize; }
    };

    void recurse( Database&                                     database,
                  Interface::Node*                              pNode,
                  Printer&                                      printer,
                  const std::unordered_set< Interface::Node* >& includedNodes )
    {
        if( !includedNodes.contains( pNode ) )
            return;

        struct Visitor : Interface::Visitor
        {
            Database& database;
            Printer&  printer;

            Visitor( Database& _database, Printer& _printer )
                : database( _database )
                , printer( _printer )
            {
            }

            virtual ~Visitor() = default;
            virtual bool visit( Interface::UserUsing* pNode ) const
            {
                // only generate the cpp type aliases
                printer.line() << "using " << pNode->get_symbol()->get_token() << " = "
                               << pNode->get_cpp_data_type()->get_type_info()->get_canonical() << ";";
                return true;
            }
            virtual bool visit( Interface::Aggregate* ) const { return true; }
            virtual bool visit( Interface::IContext* ) const { return true; }
        } visitor{ database, printer };

        Interface::visit( visitor, pNode );

        const bool bIsContext  = db_cast< Interface::IContext >( pNode );
        const bool bIsFunction = db_cast< Interface::Function >( pNode );
        if( bIsContext && !bIsFunction )
        {
            printer.line() << "namespace " << pNode->get_symbol()->get_token();
            printer.line() << "{";
            printer.indent();
        }

        for( Interface::Node* pChildNode : pNode->get_children() )
        {
            recurse( database, pChildNode, printer, includedNodes );
        }

        if( bIsContext && !bIsFunction )
        {
            printer.outdent();
            printer.line() << "}";
        }
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::GeneratedHPPSourceFilePath cppDeclsHeader = m_environment.CPPDecls();
        const mega::io::PrecompiledHeaderFile      cppDeclsPCH    = m_environment.CPPDeclsPCH();

        start( taskProgress, "Task_CPP_Decl", m_manifestFilePath.path(), cppDeclsPCH.path() );

        task::DeterminantHash determinant(
            m_toolChain.toolChainHash,
            m_environment.getBuildHashCode( m_environment.InterfaceStage_Tree( m_manifestFilePath ) ),
            m_environment.getBuildHashCode( m_environment.ConcreteStage_Concrete( m_manifestFilePath ) ),
            m_environment.getBuildHashCode( m_environment.HyperGraphAnalysis_Model( m_manifestFilePath ) )

        );

        for( const mega::io::megaFilePath& sourceFilePath : getSortedSourceFiles() )
        {
            determinant ^= m_environment.getBuildHashCode( m_environment.ParserStage_AST( sourceFilePath ) );
        }

        if( m_environment.restore( cppDeclsHeader, determinant ) && m_environment.restore( cppDeclsPCH, determinant ) )
        {
            m_environment.setBuildHashCode( cppDeclsHeader );
            m_environment.setBuildHashCode( cppDeclsPCH );
            cached( taskProgress );
            return;
        }

        Database database( m_environment, m_manifestFilePath );

        std::ostringstream osInterface;
        {
            osInterface << "using namespace mega;\n";
            osInterface << "using namespace mega::interface;\n";

            Printer printer{ osInterface };

            // determine all nodes to include based on occurence of cpp aliases
            std::unordered_set< Interface::Node* > includedNodes;
            {
                for( auto pAlias : database.many< Interface::UserUsing >( m_manifestFilePath ) )
                {
                    for( Interface::Node* pIter = pAlias; pIter;
                         pIter                  = db_cast< Interface::Node >( pIter->get_parent() ) )
                    {
                        includedNodes.insert( pIter );
                    }
                }
            }

            Interface::Root* pRoot = database.one< Interface::Root >( m_manifestFilePath );
            for( Interface::Node* pNode : pRoot->get_children() )
            {
                recurse( database, pNode, printer, includedNodes );
            }

            // generate type info traits
            printer.line() << "namespace " << mega::MEGA_TRAITS;
            printer.line() << "{";

            for( auto pTypeInfo : database.many< Interface::CPP::TypeInfo >( m_manifestFilePath ) )
            {
                printer.line() << "  using " << pTypeInfo->get_trait_name() << " = " << pTypeInfo->get_canonical()
                               << ";";
            }

            printer.line() << "}";
            printer.line();
        }

        if( boost::filesystem::updateFileIfChanged( m_environment.FilePath( cppDeclsHeader ), osInterface.str() ) )
        {
            m_environment.setBuildHashCode( cppDeclsHeader );
            m_environment.stash( cppDeclsHeader, determinant );
        }
        else
        {
            m_environment.setBuildHashCode( cppDeclsHeader );
        }

        Components::Component* pInterfaceComponent = nullptr;
        {
            for( auto pComponent : database.many< Components::Component >( m_environment.project_manifest() ) )
            {
                if( pComponent->get_type() == mega::ComponentType::eInterface )
                {
                    VERIFY_RTE_MSG( !pInterfaceComponent, "Multiple interface components found" );
                    pInterfaceComponent = pComponent;
                }
            }
        }

        const mega::Compilation compilationCMD
            = mega::Compilation::make_cpp_decls_pch( m_environment, m_toolChain, pInterfaceComponent );

        if( EXIT_SUCCESS == run_cmd( taskProgress, compilationCMD.generateCompilationCMD() ) )
        {
            m_environment.setBuildHashCode( cppDeclsPCH );
            m_environment.stash( cppDeclsPCH, determinant );
            succeeded( taskProgress );
        }
        else
        {
            failed( taskProgress );
        }
    }
};

BaseTask::Ptr create_Task_CPP_Decl( const TaskArguments& taskArguments )
{
    return std::make_unique< Task_CPP_Decl >( taskArguments );
}

} // namespace mega::compiler
