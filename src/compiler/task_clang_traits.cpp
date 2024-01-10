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
#include "compiler/interface.hpp"
#include "compiler/interface_printer.hpp"
#include "compiler/interface_visitor.hpp"
#include "compiler/concrete.hpp"
#include "compiler/concrete_printer.hpp"
#include "compiler/common_ancestor.hpp"

namespace ClangTraits
{
#include "compiler/derivation.hpp"
#include "compiler/disambiguate.hpp"
#include "compiler/derivation_printer.hpp"
} // namespace ClangTraits

} // namespace ClangTraitsStage

namespace mega::compiler
{

using namespace ClangTraitsStage;

namespace
{

using namespace ClangTraitsStage::ClangTraits;

std::string generateCPPType( Database& database, Interface::Node* pNode,
                             std::vector< Parser::Type::Fragment* > fragments )
{
    std::ostringstream os;

    for( auto pFragment : fragments )
    {
        if( auto pPath = db_cast< Parser::Type::Absolute >( pFragment ) )
        {
            os << MEGA_POINTER << "< "
               << "0x" << std::hex << std::setw( 8 ) << std::setfill( '0' )
               << pPath->get_type()->get_interface_id()->get_type_id().getValue() << std::dec << " > ";
        }
        else if( auto pPath = db_cast< Parser::Type::Deriving >( pFragment ) )
        {
            ClangTraits::InterObjectDerivationPolicy       policy{ database };
            ClangTraits::InterObjectDerivationPolicy::Spec spec{ pNode->get_inheritors(), pPath, true };
            std::vector< ClangTraits::Derivation::Or* >    frontier;
            auto pRoot = ClangTraits::solveContextFree( spec, policy, frontier );

            os << MEGA_POINTER << "< ";
            bool                                bFirst = true;
            std::set< mega::interface::TypeID > uniqueInterfaceTypeIDs;
            for( auto pOr : frontier )
            {
                const auto interfaceTypeID = Concrete::getInterfaceTypeID( pOr->get_vertex() );
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
                    os << "0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << interfaceTypeID.getValue()
                       << std::dec;
                    uniqueInterfaceTypeIDs.insert( interfaceTypeID );
                }
            }
            os << " > ";
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

std::vector< ClangTraits::Derivation::Root* >
solveTransitions( Database& database, Parser::TypeDecl::Transitions* pTransitions, Concrete::Node* pCNode )
{
    std::vector< ClangTraits::Derivation::Root* > transitions;

    for( auto pPath : pTransitions->get_type_path_sequence()->get_path_sequence() )
    {
        if( auto pAbsolutePath = db_cast< Parser::Type::Absolute >( pPath ) )
        {
            THROW_RTE( "Invalid use of absolute type in transition" );
        }
        else if( auto pDerivingPath = db_cast< Parser::Type::Deriving >( pPath ) )
        {
            ClangTraits::NonInterObjectDerivationPolicy       policy{ database };
            ClangTraits::NonInterObjectDerivationPolicy::Spec spec{ { pCNode }, pDerivingPath, false };
            std::vector< ClangTraits::Derivation::Or* >       frontier;
            auto pRoot = ClangTraits::solveContextFree( spec, policy, frontier );

            try
            {
                ClangTraits::precedence( pRoot );

                const ClangTraits::Disambiguation result = ClangTraits::disambiguate( pRoot, frontier );
                if( result != ClangTraits::eSuccess )
                {
                    std::ostringstream os;
                    if( result == ClangTraits::eAmbiguous )
                        os << "Derivation disambiguation was ambiguous for: " << Concrete::fullTypeName( pCNode )
                           << "\n";
                    else if( result == ClangTraits::eFailure )
                        os << "Derivation disambiguation failed for: " << Concrete::fullTypeName( pCNode ) << "\n";
                    else
                        THROW_RTE( "Unknown derivation failure type" );
                    THROW_RTE( os.str() );
                }
                else
                {
                    transitions.push_back( pRoot );
                }
            }
            catch( std::exception& ex )
            {
                std::ostringstream os;
                os << "Exception while compiling interupt dispatch for: " << Concrete::fullTypeName( pCNode ) << "\n";
                ClangTraits::printDerivationStep( pRoot, true, os );
                os << "\nError: " << ex.what();
                THROW_RTE( os.str() );
            }
        }
        else
        {
            THROW_RTE( "Unknown type path type" );
        }
    }

    return transitions;
}

struct GraphInfo
{
    // std::map< Interface::UserLink*, HyperGraph::NonOwningRelation* > nonOwning;
    //  std::multimap< Interface::UserLink*, Concrete::OwnershipLink* > owners;
    //  std::multimap< Concrete::OwnershipLink*, Interface::UserLink* > owned;

    GraphInfo( HyperGraph::Graph* pGraph )
    //: nonOwning( pGraph->get_non_owning_relations() )
    // , owners( pGraph->get_owning_relation()->get_owners() )
    // , owned( pGraph->get_owning_relation()->get_owned() )
    {
    }
};

std::vector< Derivation::Dispatch* > buildEventDispatches( Database& database, GraphInfo& graph,
                                                           Derivation::Edge* pEdge )
{
    std::vector< Derivation::Dispatch* > dispatches;

    auto pCurrentStep = pEdge->get_next();

    auto edges = pCurrentStep->get_edges();
    if( edges.empty() )
    {
        auto pEventVertex = pCurrentStep->get_vertex();
        auto pEventState  = db_cast< Concrete::State >( pEventVertex );
        auto pEventEvent  = db_cast< Concrete::Event >( pEventVertex );
        VERIFY_RTE_MSG( pEventState || pEventEvent, "Interupt event does NOT specify an Event or State. Kind is: "
                                                        << Concrete::getKind( pEventVertex ) );

        // create event dispatch
        auto pDispatch = database.construct< Derivation::Dispatch >( Derivation::Dispatch::Args{
            Derivation::Step::Args{ Derivation::Node::Args{ {} }, pCurrentStep->get_vertex() } } );
        dispatches.push_back( pDispatch );
    }
    else
    {
        for( auto p : edges )
        {
            if( !p->get_eliminated() )
            {
                auto result = buildEventDispatches( database, graph, p );
                std::copy( result.begin(), result.end(), std::back_inserter( dispatches ) );
            }
        }
    }

    // the bottom up recursion will start with an empty dispatch.
    // As the stuck unfolds each inverse edge will be added to the dispatch.
    for( Derivation::Dispatch* pDispatch : dispatches )
    {
        // obtain the last step in the event dispatch sequence
        Derivation::Step* pLastStep = pDispatch;
        {
            while( !pLastStep->get_edges().empty() )
            {
                auto edges = pLastStep->get_edges();
                VERIFY_RTE( edges.size() == 1 );
                pLastStep = edges.front()->get_next();
                VERIFY_RTE( pLastStep );
            }
        }

        // solve inverse step from pLastStep->get_vertex() to pEdge->get_from()
        auto pSourceVertex = pLastStep->get_vertex();
        if( auto pTargetStep = db_cast< Derivation::Step >( pEdge->get_from() ) )
        {
            auto pTargetVertex = pTargetStep->get_vertex();
            if( auto pTargetOr = db_cast< Derivation::Or >( pTargetStep ) )
            {
                if( pSourceVertex != pTargetVertex )
                {
                    // common root derivation to or vertex
                    std::vector< Concrete::Edge* > edges;
                    VERIFY_RTE( CommonAncestor::commonRootDerivation( pSourceVertex, pTargetVertex, edges, true ) );

                    // initially only create the steps - which can be refined in second process
                    // to determine if Select or Or
                    auto pNextStep = database.construct< Derivation::Or >(
                        Derivation::Or::Args{ Derivation::Step::Args{ Derivation::Node::Args{ {} }, pTargetVertex } } );

                    auto pDispatchEdge = database.construct< Derivation::Edge >(
                        Derivation::Edge::Args{ pLastStep, pNextStep, false, false, 0, edges } );

                    pLastStep->push_back_edges( pDispatchEdge );
                }
            }
            else if( auto pTargetAnd = db_cast< Derivation::And >( pTargetStep ) )
            {
                Concrete::Node* pGraphInterObjectLinkVertex = nullptr;
                {
                    // previous step MUST be an Or
                    auto pLastOr = db_cast< Derivation::Or >( pLastStep );
                    VERIFY_RTE( pLastOr );

                    // the And MUST be on a link vertex
                    auto pConcreteLink = db_cast< Concrete::Link >( pTargetAnd->get_vertex() );
                    VERIFY_RTE( pConcreteLink );

                    // edge should be single edge to the inter object target type
                    auto graphEdges = pEdge->get_edges();
                    VERIFY_RTE( graphEdges.size() == 1 );
                    auto pGraphInterObjectEdge = db_cast< Concrete::InterObjectEdge >( graphEdges[ 0 ] );
                    VERIFY_RTE( pGraphInterObjectEdge );

                    // which MUST have a source of the target vertex
                    VERIFY_RTE( pGraphInterObjectEdge->get_source() == pTargetVertex );

                    // get the counter part of the inter object edge
                    auto pCounterPart = pGraphInterObjectEdge->get_counterpart();
                    VERIFY_RTE( pCounterPart );

                    Concrete::InterObjectEdge* pCounterEdge = nullptr;
                    {
                        for( auto pCounterPartOutEdge : pCounterPart->get_out_edges() )
                        {
                            if( auto pCounterInterObjectEdge
                                = db_cast< Concrete::InterObjectEdge >( pCounterPartOutEdge ) )
                            {
                                if( pCounterInterObjectEdge->get_counterpart() == pConcreteLink )
                                {
                                    VERIFY_RTE( !pCounterEdge );
                                    pCounterEdge = pCounterInterObjectEdge;
                                }
                            }
                        }
                    }
                    VERIFY_RTE( pCounterEdge );
                    auto pCounterTarget = pCounterEdge->get_target();

                    // create the Selection to perform the inter-object derivation step
                    auto pSelect = database.construct< Derivation::Select >( Derivation::Select::Args{
                        Derivation::Or::Args{ Derivation::Step::Args{ Derivation::Node::Args{ {} }, pCounterPart } },
                        pCounterTarget } );

                    // derive to the counterpart
                    {
                        // solve common root derivation to the link from the current vertex
                        std::vector< Concrete::Edge* > edges;
                        VERIFY_RTE( CommonAncestor::commonRootDerivation( pSourceVertex, pCounterPart, edges, true ) );

                        auto pEdgeToCounterPart = database.construct< Derivation::Edge >(
                            Derivation::Edge::Args{ pLastStep, pSelect, false, false, 0, edges } );
                        pLastStep->push_back_edges( pEdgeToCounterPart );
                    }

                    // can now construct an Or vertex at the target which the Selection derives
                    auto pFinalOrStep = database.construct< Derivation::Or >( Derivation::Or::Args{
                        Derivation::Step::Args{ Derivation::Node::Args{ {} }, pCounterTarget } } );

                    auto pSelectEdge = database.construct< Derivation::Edge >(
                        Derivation::Edge::Args{ pSelect, pFinalOrStep, false, false, 0, { pCounterEdge } } );
                    pSelect->push_back_edges( pSelectEdge );
                }
            }
            else
            {
                THROW_RTE( "Unknown derivation step type" );
            }
        }
        else if( auto pTargetRoot = db_cast< Derivation::Root >( pEdge->get_from() ) )
        {
            // do nothing
        }
        else
        {
            THROW_RTE( "Unknown derivation node type" );
        }
    }

    return dispatches;
}

void buildEventDispatches( Database& database, GraphInfo& graph, Derivation::Root* pDerivationRoot,
                           std::vector< Derivation::Dispatch* >& eventDispatches )
{
    for( auto pEdge : pDerivationRoot->get_edges() )
    {
        if( !pEdge->get_eliminated() )
        {
            auto dispatches = buildEventDispatches( database, graph, pEdge );
            std::copy( dispatches.begin(), dispatches.end(), std::back_inserter( eventDispatches ) );
        }
    }
}

} // namespace

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

    struct Visitor : Interface::Visitor
    {
        Database&  database;
        GraphInfo& graph;
        Printer&   printer;

        Visitor( Database& database, GraphInfo& graph, Printer& printer )
            : database( database )
            , graph( graph )
            , printer( printer )
        {
        }

        virtual ~Visitor() = default;
        virtual bool visit( Interface::UserDimension* pNode ) const
        {
            auto pDataType = pNode->get_dimension()->get_data_type();
            printer.line() << "using " << pNode->get_symbol()->get_token() << " = "
                           << generateCPPType( database, pNode, pDataType->get_cpp_fragments()->get_elements() ) << ";";

            auto pCPPDataType = database.construct< Interface::CPP::DataType >(
                Interface::CPP::DataType::Args{ Interface::CPP::Type::Args{ pNode } } );

            database.construct< Interface::UserDimension >(
                Interface::UserDimension::Args{ pNode, pCPPDataType } );

            return true;
        }
        virtual bool visit( Interface::UserAlias* pNode ) const
        {
            auto pPath = pNode->get_alias()->get_alias_type()->get_type_path();
            if( auto pDeriving = db_cast< Parser::Type::Deriving >( pPath ) )
            {
                THROW_TODO;
                /*ClangTraits::InterObjectDerivationPolicy       policy{ database };
                ClangTraits::InterObjectDerivationPolicy::Spec spec{ pNode->get_inheritors(), pDeriving, true };
                std::vector< ClangTraits::Derivation::Or* >    frontier;
                auto pRoot = ClangTraits::solveContextFree( spec, policy, frontier );*/

                // TODO
            }
            else if( auto pAbsolute = db_cast< Parser::Type::Absolute >( pPath ) )
            {
                // auto pType = pAbsolute->get_type();
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
                           << generateCPPType( database, pNode, pUsingType->get_cpp_fragments()->get_elements() )
                           << ";";

            auto pCPPDataType = database.construct< Interface::CPP::DataType >(
                Interface::CPP::DataType::Args{ Interface::CPP::Type::Args{ pNode } } );

            database.construct< Interface::UserUsing >(
                Interface::UserUsing::Args{ pNode, pCPPDataType } );

            return true;
        }
        virtual bool visit( Interface::UserLink* pNode ) const { return false; }
        virtual bool visit( Interface::Aggregate* pNode ) const { return true; }

        virtual bool visit( Interface::Namespace* pNode ) const { return false; }
        virtual bool visit( Interface::Abstract* pNode ) const { return false; }
        virtual bool visit( Interface::Event* pNode ) const { return false; }
        virtual bool visit( Interface::Object* pNode ) const { return false; }

        virtual bool visit( Interface::Interupt* pNode ) const
        {
            for( auto pCNode : pNode->get_realisers() )
            {
                auto pCInterupt = db_cast< Concrete::Interupt >( pCNode );
                VERIFY_RTE( pCInterupt );

                std::vector< ClangTraits::Derivation::Root* > transitions;
                std::vector< ClangTraits::Derivation::Root* > derivations;
                std::vector< Functions::EventDispatch* >      eventDispatches;

                if( auto transitionOpt = pNode->get_transition_opt() )
                {
                    transitions = solveTransitions( database, transitionOpt.value(), pCNode );
                }

                if( auto eventOpt = pNode->get_events_opt() )
                {
                    Parser::TypeDecl::Events* pEvents = eventOpt.value();

                    for( auto pNamedPath : pEvents->get_type_named_path_sequence()->get_named_path_sequence() )
                    {
                        auto pDerivingPath = db_cast< Parser::Type::Deriving >( pNamedPath->get_path() );
                        VERIFY_RTE( pDerivingPath );

                        ClangTraits::InterObjectDerivationPolicy       policy{ database };
                        ClangTraits::InterObjectDerivationPolicy::Spec spec{ { pCNode }, pDerivingPath, true };
                        std::vector< ClangTraits::Derivation::Or* >    frontier;
                        auto pRoot = ClangTraits::solveContextFree( spec, policy, frontier );
                        derivations.push_back( pRoot );

                        try
                        {
                            // analysis inverse event dispatches ...
                            std::vector< Derivation::Dispatch* > dispatches;
                            buildEventDispatches( database, graph, pRoot, dispatches );

                            for( auto pDispatch : dispatches )
                            {
                                auto pEvent = db_cast< Concrete::Context >( pDispatch->get_vertex() );
                                VERIFY_RTE( pEvent );
                                auto pEventDispatch = database.construct< Functions::EventDispatch >(
                                    Functions::EventDispatch::Args{ pDispatch, pEvent, pCInterupt } );
                                eventDispatches.push_back( pEventDispatch );
                            }
                        }
                        catch( std::exception& ex )
                        {
                            std::ostringstream os;
                            os << "Exception while compiling interupt dispatch for: "
                               << Concrete::fullTypeName( pCInterupt ) << "\n";
                            ClangTraits::printDerivationStep( pRoot, true, os );
                            os << "\nError: " << ex.what();
                            THROW_RTE( os.str() );
                        }
                    }
                }

                database.construct< Concrete::Interupt >(
                    Concrete::Interupt::Args{ pCInterupt, transitions, derivations, eventDispatches } );
            }

            return true;
        }
        virtual bool visit( Interface::Requirement* pNode ) const { return false; }

        virtual bool visit( Interface::Decider* pNode ) const
        {
            for( auto pCNode : pNode->get_realisers() )
            {
                auto pCDecider = db_cast< Concrete::Decider >( pCNode );
                VERIFY_RTE( pCDecider );

                auto pEvents = pNode->get_events();

                std::vector< ClangTraits::Derivation::Root* > events;
                {
                    for( auto pNamedPath : pEvents->get_type_named_path_sequence()->get_named_path_sequence() )
                    {
                        auto pDerivingPath = db_cast< Parser::Type::Deriving >( pNamedPath->get_path() );
                        VERIFY_RTE( pDerivingPath );

                        ClangTraits::NonInterObjectDerivationPolicy       policy{ database };
                        ClangTraits::NonInterObjectDerivationPolicy::Spec spec{ { pCNode }, pDerivingPath, false };
                        std::vector< ClangTraits::Derivation::Or* >       frontier;
                        auto pRoot = ClangTraits::solveContextFree( spec, policy, frontier );
                        events.push_back( pRoot );
                    }
                }
                database.construct< Concrete::Decider >( Concrete::Decider::Args{ pCDecider, events } );
            }

            return true;
        }

        virtual bool visit( Interface::Function* pNode ) const
        {
            printer.line()
                << "using " << pNode->get_symbol()->get_token() << " = "
                << generateCPPType( database, pNode, pNode->get_return_type()->get_cpp_fragments()->get_elements() )
                << "(*)("
                << generateCPPType( database, pNode, pNode->get_arguments()->get_cpp_fragments()->get_elements() )
                << ");";

            auto pCPPDataType = database.construct< Interface::CPP::FunctionType >(
                Interface::CPP::FunctionType::Args{ Interface::CPP::Type::Args{ pNode } } );

            database.construct< Interface::Function >(
                Interface::Function::Args{ pNode, pCPPDataType } );

            return true;
        }

        virtual bool visit( Interface::Action* pNode ) const { return false; }
        virtual bool visit( Interface::Component* pNode ) const { return false; }
        virtual bool visit( Interface::State* pNode ) const
        {
            auto transitionOpt = pNode->get_transition_opt();

            for( auto pCNode : pNode->get_realisers() )
            {
                auto pCState = db_cast< Concrete::State >( pCNode );
                VERIFY_RTE( pCState );

                if( transitionOpt.has_value() )
                {
                    auto transitions = solveTransitions( database, transitionOpt.value(), pCNode );
                    database.construct< Concrete::State >( Concrete::State::Args{ pCState, transitions } );
                }
                else
                {
                    database.construct< Concrete::State >( Concrete::State::Args{ pCState, {} } );
                }
            }
            return true;
        }
        virtual bool visit( Interface::InvocationContext* pNode ) const { return false; }
        virtual bool visit( Interface::IContext* pNode ) const { return true; }
    };

    void recurse( Database& database, GraphInfo& graph, Interface::Node* pNode, Printer& printer )
    {
        Visitor visitor{ database, graph, printer };

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
            recurse( database, graph, pChildNode, printer );
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
            GraphInfo        graph( database.one< HyperGraph::Graph >( m_manifestFilePath ) );

            for( Interface::Node* pNode : pRoot->get_children() )
            {
                recurse( database, graph, pNode, printer );
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
        }
        else
        {
            m_environment.setBuildHashCode( traitsHeader );
        }

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_Clang_Traits_Gen( const TaskArguments& taskArguments )
{
    return std::make_unique< Task_Clang_Traits_Gen >( taskArguments );
}

} // namespace mega::compiler
