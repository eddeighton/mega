
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

#ifndef GUARD_2023_November_16_derivation_compiler
#define GUARD_2023_November_16_derivation_compiler
/*
class DerivationBuilder
{
    Instructions::Variables::Stack* make_stack_variable( Instructions::Variables::Variable* pParentVariable,
                                                         Concrete::Graph::Vertex*           pTargetContext )
    {
        auto pStack = m_database.construct< Instructions::Variables::Stack >( Instructions::Variables::Stack::Args{
            Instructions::Variables::Variable::Args{ pParentVariable }, pTargetContext } );
        m_pDerivationCompilation->push_back_variables( pStack );
        return pStack;
    }
    Instructions::Variables::LinkType* make_link_type_variable( Instructions::Variables::Variable* pParentVariable,
                                                                Concrete::Dimensions::LinkType*    pLinkType )
    {
        auto pStack
            = m_database.construct< Instructions::Variables::LinkType >( Instructions::Variables::LinkType::Args{
                Instructions::Variables::Variable::Args{ pParentVariable }, pLinkType } );
        m_pDerivationCompilation->push_back_variables( pStack );
        return pStack;
    }

    Instructions::Variables::Memory* make_memory_variable( Instructions::Variables::Variable* pParentVariable,
                                                           const std::vector< Concrete::Graph::Vertex* >& types )
    {
        auto pMemory
            = m_database.construct< Instructions::Variables::Memory >( Instructions::Variables::Reference::Args{
                Instructions::Variables::Variable::Args{ pParentVariable }, types } );
        m_pDerivationCompilation->push_back_variables( pMemory );
        return pMemory;
    }

    Instructions::Variables::Parameter* make_parameter_variable( const std::vector< Concrete::Graph::Vertex* >& types )
    {
        auto pParameter = m_database.construct< Instructions::Variables::Parameter >(
            Instructions::Variables::Parameter::Args{ Instructions::Variables::Reference::Args{
                Instructions::Variables::Variable::Args{ std::nullopt }, types } } );
        m_pDerivationCompilation->push_back_variables( pParameter );
        return pParameter;
    }

    template < typename TInstruction, typename... ConstructorArgs >
    TInstruction* make_instruction( Instructions::Calculation::InstructionGroup* pParentInstruction,
                                    ConstructorArgs&&... ctorArgs )
    {
        // clang-format off
        TInstruction* pNewInstruction = m_database.construct< TInstruction >
        (
            typename TInstruction::Args
            {
                Instructions::Calculation::InstructionGroup::Args
                {
                    Instructions::Calculation::Instruction::Args{},
                    {}
                },
                ctorArgs...
            }
        );
        // clang-format on
        if( pParentInstruction )
        {
            pParentInstruction->push_back_children( pNewInstruction );
        }

        return pNewInstruction;
    }

private:
    Database&                           m_database;
    Derivation::Node*                   m_pDerivationTreeRoot;
    Operations::DerivationCompilation*  m_pDerivationCompilation;
    Instructions::Variables::Parameter* m_pParameterVariable;
    Instructions::Calculation::Root*    m_pRootInstruction;

public:
    DerivationBuilder( Database& database, Derivation::Node* pDerivationTreeRoot,
                       const std::vector< Concrete::Graph::Vertex* >& contextTypes )
        : m_database( database )
        , m_pDerivationTreeRoot( pDerivationTreeRoot )
        , m_pDerivationCompilation( database.construct< Operations::DerivationCompilation >(
              Operations::DerivationCompilation::Args{ pDerivationTreeRoot, {}, {} } ) )
        , m_pParameterVariable( make_parameter_variable( contextTypes ) )
        , m_pRootInstruction( make_instruction< Instructions::Calculation::Root >( nullptr, m_pParameterVariable ) )
    {
        m_pDerivationCompilation->set_root_instruction( m_pRootInstruction );
    }

    void build()
    {
        auto rootOutEdges       = m_pDerivationTreeRoot->get_edges();
        U64  activeOutEdgeCount = 0U;
        for( auto pEdge : rootOutEdges )
        {
            VERIFY_RTE( !pEdge->get_backtracked() );
            if( !pEdge->get_eliminated() )
            {
                ++activeOutEdgeCount;
            }
        }

        if( activeOutEdgeCount == 1 )
        {
            for( auto pEdge : rootOutEdges )
            {
                if( !pEdge->get_eliminated() )
                {
                    build( m_pRootInstruction, m_pParameterVariable, pEdge );
                    break;
                }
            }
        }
        else if( activeOutEdgeCount > 1 )
        {
            auto pPolyReference
                = make_instruction< Instructions::Calculation::PolyBranch >( m_pRootInstruction, m_pParameterVariable );
            for( auto pEdge : m_pDerivationTreeRoot->get_edges() )
            {
                if( !pEdge->get_eliminated() )
                {
                    auto pInstruction = make_instruction< Instructions::Calculation::PolyCase >(
                        pPolyReference, m_pParameterVariable, pEdge->get_next()->get_vertex() );
                    auto pNext = pEdge->get_next();
                    auto pOR   = db_cast< Derivation::Or >( pNext );
                    VERIFY_RTE( pOR );
                    buildOr( pInstruction, m_pParameterVariable, pOR );
                }
            }
        }
        else
        {
            THROW_RTE( "Derivation tree root has no active out edges" );
        }
    }

private:
    void buildOperation( Instructions::Calculation::InstructionGroup* pInstruction,
                         Instructions::Variables::Variable* pVariable, Concrete::Graph::Vertex* pVertex )
    {
        Instructions::Application::Operation* pOperation
            = m_database.construct< Instructions::Application::Operation >( Instructions::Application::Operation::Args{
                Instructions::Calculation::Instruction::Args{}, pVariable, pVertex } );
        pInstruction->push_back_children( pOperation );
        m_pDerivationCompilation->push_back_operations( pOperation );
    }

    void buildAnd( Instructions::Calculation::InstructionGroup* pInstruction,
                   Instructions::Variables::Variable* pVariable, Derivation::And* pAnd )
    {
        // read the link
        auto pLink = db_cast< Concrete::Dimensions::Link >( pAnd->get_vertex() );

        auto pLinkReference = make_memory_variable( pVariable, {} );
        auto pLinkType      = make_link_type_variable( pVariable, pLink->get_link_type() );

        auto pDereference = make_instruction< Instructions::Calculation::Dereference >(
            pInstruction, pVariable, pLinkReference, pLinkType, pLink );

        // create polymorphic branch
        auto pPolyReference = make_instruction< Instructions::Calculation::LinkBranch >( pDereference, pLinkType );

        bool bFound = false;
        for( auto pEdge : pAnd->get_edges() )
        {
            if( !pEdge->get_eliminated() )
            {
                bFound = true;
                VERIFY_RTE( !pEdge->get_backtracked() );

                pInstruction                                       = pPolyReference;
                Instructions::Variables::Variable* pResultVariable = pLinkReference;

                auto hyperGraphEdges = pEdge->get_edges();
                VERIFY_RTE( !hyperGraphEdges.empty() );

                for( auto hyperGraphEdge : hyperGraphEdges )
                {
                    switch( hyperGraphEdge->get_type().get() )
                    {
                        case ::mega::EdgeType::eMonoSingularMandatory:
                        case ::mega::EdgeType::eMonoSingularOptional:
                        {
                            pInstruction = make_instruction< Instructions::Calculation::PolyCase >(
                                pInstruction, pResultVariable, hyperGraphEdge->get_target() );
                        }
                        break;
                        case ::mega::EdgeType::eMonoNonSingularMandatory:
                        case ::mega::EdgeType::eMonoNonSingularOptional:
                        {
                            THROW_RTE( "Non singular dereference" );
                        }
                        break;
                        case ::mega::EdgeType::ePolySingularMandatory:
                        case ::mega::EdgeType::ePolySingularOptional:
                        {
                            pInstruction = make_instruction< Instructions::Calculation::PolyCase >(
                                pInstruction, pResultVariable, hyperGraphEdge->get_target() );
                        }
                        break;
                        case ::mega::EdgeType::ePolyNonSingularMandatory:
                        case ::mega::EdgeType::ePolyNonSingularOptional:
                        {
                            THROW_RTE( "Non singular dereference" );
                        }
                        break;
                        case ::mega::EdgeType::ePolyParent:
                        {
                            pInstruction = make_instruction< Instructions::Calculation::PolyCase >(
                                pInstruction, pResultVariable, hyperGraphEdge->get_target() );
                        }
                        break;

                        case ::mega::EdgeType::eParent:
                        {
                            auto pTo     = make_stack_variable( pResultVariable, hyperGraphEdge->get_target() );
                            pInstruction = make_instruction< Instructions::Calculation::ParentDerivation >(
                                pInstruction, pResultVariable, pTo );
                            pResultVariable = pTo;
                        }
                        break;

                        default:
                        {
                            THROW_RTE( "Unsupported AND hypergraph edge" );
                        }
                        break;
                    }
                }

                auto pNext = pEdge->get_next();
                auto pOR   = db_cast< Derivation::Or >( pNext );
                VERIFY_RTE( pOR );
                buildOr( pInstruction, pResultVariable, pOR );
            }
        }
        VERIFY_RTE_MSG( bFound, "Failed to find non-eliminated edge in OR step" );
    }

    void buildOr( Instructions::Calculation::InstructionGroup* pInstruction,
                  Instructions::Variables::Variable* pVariable, Derivation::Or* pOr )
    {
        std::vector< Derivation::Edge* > edges;
        for( auto pEdge : pOr->get_edges() )
        {
            if( !pEdge->get_eliminated() && !pEdge->get_backtracked() )
            {
                edges.push_back( pEdge );
            }
        }
        if( edges.empty() )
        {
            // add the operation
            buildOperation( pInstruction, pVariable, pOr->get_vertex() );
        }
        else
        {
            bool bFound = false;
            for( auto pEdge : edges )
            {
                VERIFY_RTE_MSG( !bFound, "Multiple non-eliminated edges in OR derivation step" );
                build( pInstruction, pVariable, pEdge );
                bFound = true;
            }
            VERIFY_RTE_MSG( bFound, "Failed to find non-eliminated edge in OR derivation step" );
        }
    }

    void build( Instructions::Calculation::InstructionGroup* pInstruction, Instructions::Variables::Variable* pVariable,
                Derivation::Edge* pEdge )
    {
        // NOTE pInstruction AND pVariable are OUT parameters taken by Pointer
        buildHyperGraphEdges( pInstruction, pVariable, pEdge );

        auto pNextStep = pEdge->get_next();
        if( auto pAnd = db_cast< Derivation::And >( pNextStep ) )
        {
            // is the AND out edges backtracked for link dimension
            int  iBackTrackedCount = 0;
            auto edges             = pAnd->get_edges();
            for( auto pAndEdge : edges )
            {
                if( pAndEdge->get_backtracked() )
                {
                    ++iBackTrackedCount;
                }
            }
            if( iBackTrackedCount != 0 )
            {
                VERIFY_RTE( iBackTrackedCount == edges.size() );
                buildOperation( pInstruction, pVariable, pAnd->get_vertex() );
            }
            else
            {
                buildAnd( pInstruction, pVariable, pAnd );
            }
        }
        else if( auto pOr = db_cast< Derivation::Or >( pNextStep ) )
        {
            buildOr( pInstruction, pVariable, pOr );
        }
        else
        {
            THROW_RTE( "Unknown derivation step type" );
        }
    }

    void buildHyperGraphEdges( Instructions::Calculation::InstructionGroup*& pInstruction,
                               Instructions::Variables::Variable*& pVariable, Derivation::Edge* pEdge )
    {
        for( auto pGraphEdge : pEdge->get_edges() )
        {
            switch( pGraphEdge->get_type().get() )
            {
                case ::mega::EdgeType::eParent:
                {
                    auto pContext = db_cast< Concrete::Context >( pGraphEdge->get_target() );
                    VERIFY_RTE( pContext );
                    Instructions::Variables::Stack* pTo = make_stack_variable( pVariable, pContext );
                    pInstruction = make_instruction< Instructions::Calculation::ParentDerivation >(
                        pInstruction, pVariable, pTo );
                    pVariable = pTo;
                }
                break;
                case ::mega::EdgeType::eDim:
                case ::mega::EdgeType::eLink:
                case ::mega::EdgeType::eChildSingular:
                {
                    Instructions::Variables::Stack* pTo = make_stack_variable( pVariable, pGraphEdge->get_target() );
                    pInstruction = make_instruction< Instructions::Calculation::ChildDerivation >(
                        pInstruction, pVariable, pTo );
                    pVariable = pTo;
                }
                break;
                case ::mega::EdgeType::eChildNonSingular:
                {
                    THROW_TODO;
                }
                break;
                case ::mega::EdgeType::eMonoSingularMandatory:
                case ::mega::EdgeType::eMonoSingularOptional:
                case ::mega::EdgeType::eMonoNonSingularMandatory:
                case ::mega::EdgeType::eMonoNonSingularOptional:
                case ::mega::EdgeType::ePolySingularMandatory:
                case ::mega::EdgeType::ePolySingularOptional:
                case ::mega::EdgeType::ePolyNonSingularMandatory:
                case ::mega::EdgeType::ePolyNonSingularOptional:
                    THROW_TODO;
                    break;
                case ::mega::EdgeType::ePolyParent:
                {
                    THROW_TODO;
                }
                break;
                default:
                case ::mega::EdgeType::TOTAL_EDGE_TYPES:
                {
                    THROW_RTE( "Unknown hypergraph edge type" );
                }
                break;
            }
        }
    }

public:
    Operations::DerivationCompilation* getDerivationCompilation() const { return m_pDerivationCompilation; }
};
*/
#endif // GUARD_2023_November_16_derivation_compiler
