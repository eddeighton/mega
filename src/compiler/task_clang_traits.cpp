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

#include "database/ClangTraitsStage.hxx"

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

namespace ClangTraitsStage
{
#include "compiler/interface_printer.hpp"
#include "compiler/interface_visitor.hpp"
#include "compiler/common_ancestor.hpp"

namespace ClangTraits
{
#include "compiler/derivation.hpp"
}

} // namespace ClangTraitsStage

namespace mega::compiler
{

using namespace ClangTraitsStage;

class Task_Clang_Traits_Gen : public BaseTask
{
    const mega::io::manifestFilePath m_manifestFilePath;

public:
    Task_Clang_Traits_Gen( const TaskArguments& taskArguments )
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

    void recurse( Database& database, Interface::Node* pNode, Printer& printer )
    {
        struct Visitor : Interface::Visitor
        {
            Database& database;
            Printer&  printer;

            Visitor( Database& database, Printer& printer )
                : database( database )
                , printer( printer )
            {
            }

            std::string
            generateCPPType( Interface::Node* pNode, std::vector< Parser::Type::Fragment* > fragments ) const
            {
                std::ostringstream os;

                for( auto pFragment : fragments )
                {
                    if( auto pPath = db_cast< Parser::Type::Absolute >( pFragment ) )
                    {
                        os << "TypedPtr< " << pPath->get_type()->get_interface_id()->get_type_id() << " >";
                    }
                    else if( auto pPath = db_cast< Parser::Type::Deriving >( pFragment ) )
                    {
                        ClangTraits::InterObjectDerivationPolicy       policy{ database };
                        ClangTraits::InterObjectDerivationPolicy::Spec spec{ pNode->get_inheritors(), pPath, true };
                        std::vector< ClangTraits::Derivation::Or* >    frontier;
                        auto pRoot = ClangTraits::solveContextFree( spec, policy, frontier );

                        os << "TypedPtr< ";
                        bool                                bFirst = true;
                        std::set< mega::interface::TypeID > uniqueInterfaceTypeIDs;
                        for( auto pOr : frontier )
                        {
                            const auto interfaceTypeID
                                = pOr->get_vertex()->get_node()->get_interface_id()->get_type_id();
                            if( !uniqueInterfaceTypeIDs.contains( interfaceTypeID ) )
                            {
                                if( bFirst )
                                {
                                    bFirst = false;
                                }
                                else
                                {
                                    os << ", ";
                                }
                                os << interfaceTypeID;
                                uniqueInterfaceTypeIDs.insert( interfaceTypeID );
                            }
                        }
                        os << " >";
                    }
                    else if( auto pOpaque = db_cast< Parser::Type::CPPOpaque >( pFragment ) )
                    {
                        os << pOpaque->get_str();
                    }
                    else
                    {
                        THROW_RTE( "Unknown fragment type" );
                    }
                }

                return os.str();
            }

            virtual ~Visitor() = default;
            virtual bool visit( Interface::UserDimension* pNode ) const
            {
                auto pDataType = pNode->get_dimension()->get_data_type();
                printer.line() << "using " << pNode->get_symbol()->get_token() << " = "
                               << generateCPPType( pNode, pDataType->get_cpp_fragments()->get_elements() ) << ";";
                return true;
            }
            virtual bool visit( Interface::UserAlias* pNode ) const
            {
                auto pPath = pNode->get_alias()->get_alias_type()->get_type_path();
                if( auto pDeriving = db_cast< Parser::Type::Deriving >( pPath ) )
                {
                    ClangTraits::InterObjectDerivationPolicy       policy{ database };
                    ClangTraits::InterObjectDerivationPolicy::Spec spec{ pNode->get_inheritors(), pDeriving, true };
                    std::vector< ClangTraits::Derivation::Or* >    frontier;
                    auto pRoot = ClangTraits::solveContextFree( spec, policy, frontier );
                }
                else if( auto pAbsolute = db_cast< Parser::Type::Absolute >( pPath ) )
                {
                    auto pType = pAbsolute->get_type();
                }
                else
                {
                    THROW_RTE( "Unknown type path type" );
                }
                return true;
            }
            virtual bool visit( Interface::UserUsing* pNode ) const
            {
                auto pUsingType = pNode->get_cpp_using()->get_using_type();
                printer.line() << "using " << pNode->get_symbol()->get_token() << " = "
                               << generateCPPType( pNode, pUsingType->get_cpp_fragments()->get_elements() ) << ";";

                return true;
            }
            virtual bool visit( Interface::UserLink* pNode ) const { return false; }
            virtual bool visit( Interface::ParsedAggregate* pNode ) const { return false; }
            virtual bool visit( Interface::OwnershipLink* pNode ) const { return false; }
            virtual bool visit( Interface::ActivationBitSet* pNode ) const { return false; }
            virtual bool visit( Interface::GeneratedAggregate* pNode ) const { return false; }
            virtual bool visit( Interface::Aggregate* pNode ) const { return true; }

            virtual bool visit( Interface::Namespace* pNode ) const { return false; }
            virtual bool visit( Interface::Abstract* pNode ) const { return false; }
            virtual bool visit( Interface::Event* pNode ) const { return false; }
            virtual bool visit( Interface::Object* pNode ) const { return false; }
            virtual bool visit( Interface::Interupt* pNode ) const { return false; }
            virtual bool visit( Interface::Requirement* pNode ) const { return false; }
            virtual bool visit( Interface::Decider* pNode ) const { return false; }
            virtual bool visit( Interface::Function* pNode ) const
            {
                printer.line() << "using " << pNode->get_symbol()->get_token() << " = "
                               << generateCPPType(
                                      pNode, pNode->get_return_type()->get_cpp_fragments()->get_elements() )
                               << "(*)("
                               << generateCPPType( pNode, pNode->get_arguments()->get_cpp_fragments()->get_elements() )
                               << ");";
                return true;
            }
            virtual bool visit( Interface::Action* pNode ) const { return false; }
            virtual bool visit( Interface::Component* pNode ) const { return false; }
            virtual bool visit( Interface::State* pNode ) const { return false; }
            virtual bool visit( Interface::InvocationContext* pNode ) const { return false; }
            virtual bool visit( Interface::IContext* pNode ) const { return true; }
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
            recurse( database, pChildNode, printer );
        }

        if( bIsContext && !bIsFunction )
        {
            printer.outdent();
            printer.line() << "}";
        }
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::GeneratedHPPSourceFilePath traitsHeader = m_environment.ClangTraits();
        const mega::io::CompilationFilePath traitsDBFile = m_environment.ClangTraitsStage_Traits( m_manifestFilePath );
        start( taskProgress, "Task_Clang_Traits_Gen", m_manifestFilePath.path(), traitsHeader.path() );

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

        if( m_environment.restore( traitsHeader, determinant ) && m_environment.restore( traitsDBFile, determinant ) )
        {
            m_environment.setBuildHashCode( traitsHeader );
            m_environment.setBuildHashCode( traitsDBFile );
            cached( taskProgress );
            return;
        }

        std::ostringstream osInterface;
        {
            Database database( m_environment, m_manifestFilePath );

            osInterface << "using namespace mega;\n";
            osInterface << "using namespace mega::interface;\n";

            Printer printer{ osInterface };

            Interface::Root* pRoot = database.one< Interface::Root >( m_manifestFilePath );
            for( Interface::Node* pNode : pRoot->get_children() )
            {
                recurse( database, pNode, printer );
            }

            auto fileHashCode = database.save_Traits_to_temp();
            m_environment.setBuildHashCode( traitsDBFile, fileHashCode );
            m_environment.temp_to_real( traitsDBFile );
            m_environment.stash( traitsDBFile, determinant );
        }

        if( boost::filesystem::updateFileIfChanged( m_environment.FilePath( traitsHeader ), osInterface.str() ) )
        {
            m_environment.setBuildHashCode( traitsHeader );
            m_environment.stash( traitsHeader, determinant );
            succeeded( taskProgress );
        }
        else
        {
            m_environment.setBuildHashCode( traitsHeader );
            cached( taskProgress );
        }
    }
};

BaseTask::Ptr create_Task_Clang_Traits_Gen( const TaskArguments& taskArguments )
{
    return std::make_unique< Task_Clang_Traits_Gen >( taskArguments );
}

} // namespace mega::compiler
