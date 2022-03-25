//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
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

#include "database/model/invocation.hpp"
#include "database/model/identifiers.hpp"
#include "database/model/name_resolution.hpp"

#include "database/io/generics.hpp"

namespace mega
{
std::ostream& operator<<( std::ostream& os, const InvocationSolution::InvocationID& invocationID )
{
    {
        const InvocationSolution::Context& context = std::get< InvocationSolution::Context >( invocationID );
        if ( context.size() > 1 )
            os << "Var< ";
        for ( const interface::Element* pContextElement : context )
        {
            if ( pContextElement != context.front() )
                os << ", ";
            std::vector< const interface::Element* > path = getPath( pContextElement );
            for ( const interface::Element* pPathElement : path )
            {
                if ( pPathElement != path.front() )
                    os << "::";
                os << pPathElement->getIdentifier();
            }
        }
        if ( context.size() > 1 )
            os << " >";
        else if ( context.empty() )
            os << "no context";
    }
    {
        os << "[";
        const InvocationSolution::TypePath& typePath = std::get< InvocationSolution::TypePath >( invocationID );
        bool                                bFirst = true;
        for ( const std::vector< const interface::Element* >& step : typePath )
        {
            if ( bFirst )
                bFirst = false;
            else
                os << ".";
            if ( !step.empty() )
                os << step.front()->getIdentifier();
            else
                os << "_";
        }
        os << "]";
    }
    {
        os << getOperationString( std::get< OperationID >( invocationID ) );
    }

    return os;
}

void InvocationSolution::load( io::Loader& loader )
{
    loader.loadObjectVector( std::get< Context >( m_invocationID ) );
    loader.loadObjectVectorVector( std::get< TypePath >( m_invocationID ) );
    loader.load( std::get< OperationID >( m_invocationID ) );
    loader.load( m_implicitTypePath );

    {
        std::size_t szSize = 0U;
        loader.load( szSize );
        for ( std::size_t sz = 0U; sz != szSize; ++sz )
        {
            const interface::Element* pInterface = loader.loadObjectRef< interface::Element >();
            const concrete::Element*  pConcrete = loader.loadObjectRef< concrete::Element >();
            m_context.push_back( ElementPair( pInterface, pConcrete ) );
        }
    }

    {
        std::size_t szSize = 0U;
        loader.load( szSize );
        for ( std::size_t sz = 0U; sz != szSize; ++sz )
        {
            ElementPairVector elementVector;
            std::size_t       szSize2 = 0U;
            loader.load( szSize2 );
            for ( std::size_t sz2 = 0U; sz2 != szSize2; ++sz2 )
            {
                const interface::Element* pInterface = loader.loadObjectRef< interface::Element >();
                const concrete::Element*  pConcrete = loader.loadObjectRef< concrete::Element >();
                elementVector.push_back( ElementPair( pInterface, pConcrete ) );
            }
            m_typePath.push_back( elementVector );
        }
    }

    loader.loadObjectVector( m_returnTypes );
    loader.loadObjectVector( m_parameterTypes );
    loader.load( m_bDimensions );
    loader.load( m_bHomogeneousDimensions );
    loader.load( m_explicitOperation );

    {
        ASSERT( !m_pRoot );
        ASTSerialiser astSerialiser;
        m_pRoot = new RootInstruction;
        m_pRoot->load( astSerialiser, loader );
    }
}
void InvocationSolution::store( io::Storer& storer ) const
{
    storer.storeObjectVector( std::get< Context >( m_invocationID ) );
    storer.storeObjectVectorVector( std::get< TypePath >( m_invocationID ) );
    storer.store( std::get< OperationID >( m_invocationID ) );
    storer.store( m_implicitTypePath );

    {
        storer.store( m_context.size() );
        for ( ElementPairVector::const_iterator i = m_context.begin(),
                                                iEnd = m_context.end();
              i != iEnd;
              ++i )
        {
            storer.storeObjectRef( i->first );
            storer.storeObjectRef( i->second );
        }
    }

    {
        storer.store( m_typePath.size() );
        for ( ElementPairVectorVector::const_iterator j = m_typePath.begin(),
                                                      jEnd = m_typePath.end();
              j != jEnd;
              ++j )
        {
            const ElementPairVector& element = *j;
            storer.store( element.size() );
            for ( ElementPairVector::const_iterator i = element.begin(),
                                                    iEnd = element.end();
                  i != iEnd;
                  ++i )
            {
                storer.storeObjectRef( i->first );
                storer.storeObjectRef( i->second );
            }
        }
    }

    storer.storeObjectVector( m_returnTypes );
    storer.storeObjectVector( m_parameterTypes );
    storer.store( m_bDimensions );
    storer.store( m_bHomogeneousDimensions );
    storer.store( m_explicitOperation );

    {
        ASSERT( m_pRoot );
        ASTSerialiser astSerialiser;
        m_pRoot->store( astSerialiser, storer );
    }
}

class GenericOperationVisitor
{
    InvocationSolution&   m_solution;
    const NameResolution& m_resolution;

public:
    GenericOperationVisitor( InvocationSolution& solution, const NameResolution& resolution )
        : m_solution( solution )
        , m_resolution( resolution )
    {
    }

    const concrete::Element* findCommonRoot( const concrete::Element* pLeft, const concrete::Element* pRight )
    {
        if ( pLeft == pRight )
            return pLeft;

        std::list< const concrete::Element* > left, right;
        while ( pLeft )
        {
            left.push_front( pLeft );
            pLeft = pLeft->getParent();
        }
        while ( pRight )
        {
            right.push_front( pRight );
            pRight = pRight->getParent();
        }
        const concrete::Element* pCommonRoot = nullptr;
        for ( std::list< const concrete::Element* >::iterator
                  iLeft
              = left.begin(),
              iRight = right.begin();
              iLeft != left.end() && iRight != right.end() && *iLeft == *iRight;
              ++iLeft, ++iRight )
            pCommonRoot = *iLeft;
        return pCommonRoot;
    }

    void commonRootDerivation( const concrete::Element* pFrom, const concrete::Element* pTo,
                               Instruction*& pInstruction, InstanceVariable*& pVariable )
    {
        if ( pFrom && pFrom != pTo )
        {
            const concrete::Element* pCommon = findCommonRoot( pFrom, pTo );

            while ( pFrom != pCommon )
            {
                const concrete::Action* pParent = dynamic_cast< const concrete::Action* >( pFrom->getParent() );
                ASSERT( pParent );
                // generate parent derivation instruction
                InstanceVariable* pInstanceVariable = new InstanceVariable( pVariable, pParent );

                ParentDerivationInstruction* pParentDerivation = new ParentDerivationInstruction( pVariable, pInstanceVariable );
                pInstruction->append( pParentDerivation );

                pVariable = pInstanceVariable;
                pInstruction = pParentDerivation;
                pFrom = pFrom->getParent();
            }

            std::vector< const concrete::Element* > path;
            while ( pTo != pCommon )
            {
                path.push_back( pTo );
                pTo = pTo->getParent();
            }
            std::reverse( path.begin(), path.end() );

            for ( const concrete::Element* pIter : path )
            {
                if ( const concrete::Action* pAction = dynamic_cast< const concrete::Action* >( pIter ) )
                {
                    if ( pAction->getLocalDomainSize() != 1 )
                    {
                        FailureInstruction* pFailure = new FailureInstruction;
                        pInstruction->append( pFailure );
                        pInstruction = pFailure;
                        return;
                    }

                    InstanceVariable* pInstanceVariable = new InstanceVariable( pVariable, pAction );

                    ChildDerivationInstruction* pChildDerivation = new ChildDerivationInstruction( pVariable, pInstanceVariable );
                    pInstruction->append( pChildDerivation );

                    pVariable = pInstanceVariable;
                    pInstruction = pChildDerivation;
                }
                else if ( const concrete::Dimension_User* pUserDimension = dynamic_cast< const concrete::Dimension_User* >( pIter ) )
                {
                    // just ignor this - always only want the parent instance variable..
                }
                else
                {
                    THROW_RTE( "Unreachable" );
                }
            }
        }
    }

    void buildOperation( const Name& prev, const Name& current,
                         Instruction* pInstruction, InstanceVariable* pInstanceVariable )
    {
        const concrete::Element* pElement = current.getConcrete();
        if ( const concrete::Action* pAction = dynamic_cast< const concrete::Action* >( pElement ) )
        {
            const interface::Context* pInterfaceAction = dynamic_cast< const interface::Context* >( current.getInterface() );
            switch ( m_solution.getOperation() )
            {
            case id_Imp_NoParams:
            case id_Imp_Params:
            {
                // only derive the parent for the starter
                commonRootDerivation( prev.getConcrete(), current.getConcrete()->getParent(), pInstruction, pInstanceVariable );
                CallOperation* pCall = new CallOperation( pInstanceVariable, pInterfaceAction, pAction );
                pInstruction->append( pCall );
            }
            break;
            case id_Start:
            {
                commonRootDerivation( prev.getConcrete(), current.getConcrete()->getParent(), pInstruction, pInstanceVariable );
                StartOperation* pStart = new StartOperation( pInstanceVariable, pInterfaceAction, pAction );
                pInstruction->append( pStart );
            }
            break;
            case id_Stop:
            {
                commonRootDerivation( prev.getConcrete(), current.getConcrete(), pInstruction, pInstanceVariable );
                StopOperation* pStop = new StopOperation( pInstanceVariable, pInterfaceAction, pAction );
                pInstruction->append( pStop );
            }
            break;
            case id_Pause:
            {
                commonRootDerivation( prev.getConcrete(), current.getConcrete(), pInstruction, pInstanceVariable );
                PauseOperation* pPause = new PauseOperation( pInstanceVariable, pInterfaceAction, pAction );
                pInstruction->append( pPause );
            }
            break;
            case id_Resume:
            {
                commonRootDerivation( prev.getConcrete(), current.getConcrete(), pInstruction, pInstanceVariable );
                ResumeOperation* pResume = new ResumeOperation( pInstanceVariable, pInterfaceAction, pAction );
                pInstruction->append( pResume );
            }
            break;
            case id_Wait:
            {
                commonRootDerivation( prev.getConcrete(), current.getConcrete(), pInstruction, pInstanceVariable );
                WaitActionOperation* pWaitAction = new WaitActionOperation( pInstanceVariable, pInterfaceAction, pAction );
                pInstruction->append( pWaitAction );
            }
            break;
            case id_Get:
            {
                commonRootDerivation( prev.getConcrete(), current.getConcrete(), pInstruction, pInstanceVariable );
                GetActionOperation* pGetAction = new GetActionOperation( pInstanceVariable, pInterfaceAction, pAction );
                pInstruction->append( pGetAction );
            }
            break;
            case id_Done:
            {
                commonRootDerivation( prev.getConcrete(), current.getConcrete(), pInstruction, pInstanceVariable );
                DoneOperation* pDone = new DoneOperation( pInstanceVariable, pInterfaceAction, pAction );
                pInstruction->append( pDone );
            }
            break;
            default:
                THROW_RTE( "Unreachable" );
            }
        }
        else if ( const concrete::Dimension_User* pUserDimension = dynamic_cast< const concrete::Dimension_User* >( pElement ) )
        {
            commonRootDerivation( prev.getConcrete(), current.getConcrete(), pInstruction, pInstanceVariable );

            const interface::Dimension* pInterfaceDimension = dynamic_cast< const interface::Dimension* >( current.getInterface() );

            switch ( m_solution.getOperation() )
            {
            case id_Imp_NoParams:
            {
                ReadOperation* pRead = new ReadOperation( pInstanceVariable, pInterfaceDimension, pUserDimension );
                pInstruction->append( pRead );
            }
            break;
            case id_Imp_Params:
            {
                if ( const LinkGroup* pLinkGroup = pUserDimension->getLinkGroup() )
                {
                    WriteLinkOperation* pWrite = new WriteLinkOperation(
                        pInstanceVariable, pInterfaceDimension, pUserDimension, pLinkGroup );
                    pInstruction->append( pWrite );
                }
                else
                {
                    WriteOperation* pWrite = new WriteOperation( pInstanceVariable, pInterfaceDimension, pUserDimension );
                    pInstruction->append( pWrite );
                }
            }
            break;
            case id_Get:
            {
                GetDimensionOperation* pGetAction = new GetDimensionOperation( pInstanceVariable, pInterfaceDimension, pUserDimension );
                pInstruction->append( pGetAction );
            }
            break;
            case id_Wait:
            {
                WaitDimensionOperation* pWaitAction = new WaitDimensionOperation( pInstanceVariable, pInterfaceDimension, pUserDimension );
                pInstruction->append( pWaitAction );
            }
            break;
            case id_Start:
            case id_Stop:
            case id_Pause:
            case id_Resume:
            case id_Done:
                THROW_INVOCATION_EXCEPTION( "Invalid invocation operation on dimension: " << m_solution.getID() );
            default:
                THROW_RTE( "Unreachable" );
            }
        }
        else
        {
            THROW_INVOCATION_EXCEPTION( "Invalid invocation target: " << m_solution.getID() );
        }
    }

    void buildPolymorphicCase( const Name& prev, const Name& current,
                               Instruction* pInstruction, ReferenceVariable* pVariable )
    {
        const concrete::Action* pType = dynamic_cast< const concrete::Action* >( current.getConcrete() );
        ASSERT( pType );

        InstanceVariable*           pInstance = new InstanceVariable( pVariable, pType );
        PolymorphicCaseInstruction* pCase = new PolymorphicCaseInstruction( pVariable, pInstance );
        pInstruction->append( pCase );
        pInstruction = pCase;

        if ( current.isTerminal() )
        {
            buildOperation( prev, current, pInstruction, pInstance );
        }
        else
        {
            if ( current.getChildren().size() > 1U )
            {
                EliminationInstruction* pElim = new EliminationInstruction;
                pInstruction->append( pElim );
                pInstruction = pElim;
            }
            ASSERT( !current.getChildren().empty() );
            for ( std::size_t index : current.getChildren() )
            {
                const Name& next = m_resolution.getNodes()[ index ];
                buildGenerateName( current, next, pInstruction, pInstance );
            }
        }
    }

    void buildMonoDereference( const Name& prev, const Name& current,
                               Instruction* pInstruction, ReferenceVariable* pVariable )
    {
        const concrete::Action* pType = dynamic_cast< const concrete::Action* >( current.getConcrete() );
        ASSERT( pType );

        InstanceVariable* pInstance = new InstanceVariable( pVariable, pType );
        {
            MonomorphicReferenceInstruction* pMono = new MonomorphicReferenceInstruction( pVariable, pInstance );
            pInstruction->append( pMono );
            pInstruction = pMono;
        }

        if ( current.isTerminal() )
        {
            buildOperation( prev, current, pInstruction, pInstance );
        }
        else
        {
            if ( current.getChildren().size() > 1U )
            {
                EliminationInstruction* pElim = new EliminationInstruction;
                pInstruction->append( pElim );
                pInstruction = pElim;
            }
            ASSERT( !current.getChildren().empty() );
            for ( std::size_t index : current.getChildren() )
            {
                const Name& next = m_resolution.getNodes()[ index ];
                buildGenerateName( current, next, pInstruction, pInstance );
            }
        }
    }

    void buildDimensionReference( const Name& prev, const Name& current,
                                  Instruction* pInstruction, InstanceVariable* pVariable )
    {
        std::vector< const concrete::Element* > types;
        for ( std::size_t index : current.getChildren() )
        {
            const Name& next = m_resolution.getNodes()[ index ];
            types.push_back( next.getConcrete() );
        }

        DimensionReferenceVariable* pReferenceVariable = new DimensionReferenceVariable( pVariable, types );
        {
            const concrete::Dimension* pDimension = dynamic_cast< const concrete::Dimension* >( current.getConcrete() );
            ASSERT( pDimension );
            DimensionReferenceReadInstruction* pDimensionRead = new DimensionReferenceReadInstruction(
                pVariable, pReferenceVariable, pDimension );
            pInstruction->append( pDimensionRead );
            pInstruction = pDimensionRead;
        }

        if ( types.size() > 1U )
        {
            PolymorphicReferenceBranchInstruction* pBranch = new PolymorphicReferenceBranchInstruction( pReferenceVariable );
            pInstruction->append( pBranch );

            for ( std::size_t index : current.getChildren() )
            {
                const Name& next = m_resolution.getNodes()[ index ];
                buildPolymorphicCase( current, next, pBranch, pReferenceVariable );
            }
        }
        else
        {
            ASSERT( !types.empty() );
            for ( std::size_t index : current.getChildren() )
            {
                const Name& next = m_resolution.getNodes()[ index ];
                buildMonoDereference( current, next, pInstruction, pReferenceVariable );
            }
        }
    }

    void buildGenerateName( const Name& prev, const Name& current,
                            Instruction* pInstruction, InstanceVariable* pVariable )
    {
        if ( current.isTerminal() )
        {
            buildOperation( prev, current, pInstruction, pVariable );
        }
        else if ( current.isReference() )
        {
            commonRootDerivation( prev.getConcrete(), current.getConcrete(), pInstruction, pVariable );
            buildDimensionReference( prev, current, pInstruction, pVariable );
        }
        else
        {
            commonRootDerivation( prev.getConcrete(), current.getConcrete(), pInstruction, pVariable );

            if ( current.getChildren().size() > 1U )
            {
                EliminationInstruction* pElim = new EliminationInstruction;
                pInstruction->append( pElim );
                pInstruction = pElim;
            }

            for ( std::size_t index : current.getChildren() )
            {
                const Name& next = m_resolution.getNodes()[ index ];
                buildGenerateName( current, next, pInstruction, pVariable );
            }
        }
    }

    void operator()( Instruction* pInstruction, ContextVariable* pVariable )
    {
        const std::vector< const concrete::Element* >& contextTypes = pVariable->getTypes();
        if ( contextTypes.size() > 1U )
        {
            PolymorphicReferenceBranchInstruction* pBranch = new PolymorphicReferenceBranchInstruction( pVariable );
            pInstruction->append( pBranch );

            // start the recursion
            const Name& current = m_resolution.getNodes()[ 0U ];
            for ( std::size_t index : current.getChildren() )
            {
                const Name& next = m_resolution.getNodes()[ index ];
                buildPolymorphicCase( current, next, pBranch, pVariable );
            }
        }
        else
        {
            const Name& current = m_resolution.getNodes()[ 0U ];
            ASSERT( current.getChildren().size() == 1U );
            for ( std::size_t index : current.getChildren() )
            {
                const Name& next = m_resolution.getNodes()[ index ];
                buildMonoDereference( current, next, pInstruction, pVariable );
            }
        }
    }
};

class EnumerationOperationVisitor
{
    InvocationSolution&   m_solution;
    const NameResolution& m_resolution;

public:
    EnumerationOperationVisitor( InvocationSolution& solution, const NameResolution& resolution )
        : m_solution( solution )
        , m_resolution( resolution )
    {
    }

    bool targetDerivation( const concrete::Element* pContext, const concrete::Element* pTarget,
                           Instruction*& pInstruction, InstanceVariable*& pVariable )
    {
        std::vector< const concrete::Element* > path;
        while ( pTarget && ( pTarget != pContext ) )
        {
            path.push_back( pTarget );
            pTarget = pTarget->getParent();
        }

        if ( pTarget == pContext )
        {
            std::reverse( path.begin(), path.end() );
            for ( const concrete::Element* pIter : path )
            {
                if ( const concrete::Action* pAction = dynamic_cast< const concrete::Action* >( pIter ) )
                {
                    InstanceVariable* pInstanceVariable = new InstanceVariable( pVariable, pAction );

                    EnumDerivationInstruction* pEnumDerivation = new EnumDerivationInstruction( pVariable, pInstanceVariable );
                    pInstruction->append( pEnumDerivation );

                    pVariable = pInstanceVariable;
                    pInstruction = pEnumDerivation;
                }
                else
                {
                    // failure
                    FailureInstruction* pFailure = new FailureInstruction;
                    pInstruction->append( pFailure );
                    pInstruction = pFailure;
                    return false;
                }
            }
        }
        else
        {
            // failure
            FailureInstruction* pFailure = new FailureInstruction;
            pInstruction->append( pFailure );
            pInstruction = pFailure;
            return false;
        }
        return true;
    }

    void buildOperation( const Name& prev, const Name& current,
                         Instruction* pInstruction, InstanceVariable* pVariable )
    {
        const concrete::Element* pElement = current.getConcrete();
        if ( const concrete::Action* pAction = dynamic_cast< const concrete::Action* >( pElement ) )
        {
            const interface::Context* pInterfaceAction = dynamic_cast< const interface::Context* >( current.getInterface() );
            switch ( m_solution.getOperation() )
            {
            case id_Raw:
            {
                RangeOperation* pRangeOp = new RangeOperation( pVariable, pInterfaceAction, pAction, RangeOperation::eRaw );
                pInstruction->append( pRangeOp );
            }
            break;
            case id_Range:
            {
                RangeOperation* pRangeOp = new RangeOperation( pVariable, pInterfaceAction, pAction, RangeOperation::eRange );
                pInstruction->append( pRangeOp );
            }
            break;
            default:
                THROW_RTE( "Unreachable" );
            }
        }
        else
        {
            THROW_INVOCATION_EXCEPTION( "Invalid invocation target: " << m_solution.getID() );
        }
    }

    void buildEnum( const Name& prev, const Name& current,
                    Instruction* pInstruction, InstanceVariable* pVariable )
    {
        if ( current.isReference() )
        {
            THROW_INVOCATION_EXCEPTION( "Cannot have reference in range invocation type path: " << m_solution.getID() );
        }

        if ( const concrete::Action* pAction = dynamic_cast< const concrete::Action* >( current.getConcrete() ) )
        {
            if ( targetDerivation( prev.getConcrete(), pAction, pInstruction, pVariable ) )
            {
                if ( current.isTerminal() )
                {
                    buildOperation( prev, current, pInstruction, pVariable );
                }
                else
                {
                    ASSERT( !current.getChildren().empty() );
                    for ( std::size_t index : current.getChildren() )
                    {
                        const Name& next = m_resolution.getNodes()[ index ];
                        buildEnum( current, next, pInstruction, pVariable );
                    }
                }
            }
            else
            {
                THROW_INVOCATION_EXCEPTION( "Enumeration has no derivation: " << m_solution.getID() );
            }
        }
        else
        {
            THROW_INVOCATION_EXCEPTION( "Invalid invocation target: " << m_solution.getID() );
        }
    }

    void buildPolymorphicCase( const Name& prev, const Name& current,
                               Instruction* pInstruction, ReferenceVariable* pVariable )
    {
        const concrete::Action* pType = dynamic_cast< const concrete::Action* >( current.getConcrete() );
        ASSERT( pType );

        InstanceVariable* pInstance = new InstanceVariable( pVariable, pType );
        {
            PolymorphicCaseInstruction* pCase = new PolymorphicCaseInstruction( pVariable, pInstance );
            pInstruction->append( pCase );
            pInstruction = pCase;
        }

        {
            EnumerationInstruction* pEnumeration = new EnumerationInstruction( pInstance );
            pInstruction->append( pEnumeration );
            pInstruction = pEnumeration;
        }

        {
            PruneInstruction* pPrune = new PruneInstruction;
            pInstruction->append( pPrune );
            pInstruction = pPrune;
        }

        ASSERT( !current.getChildren().empty() );
        for ( std::size_t index : current.getChildren() )
        {
            const Name& next = m_resolution.getNodes()[ index ];
            buildEnum( current, next, pInstruction, pInstance );
        }
    }

    void buildMonoDereference( const Name& prev, const Name& current,
                               Instruction* pInstruction, ReferenceVariable* pVariable )
    {
        const concrete::Action* pType = dynamic_cast< const concrete::Action* >( current.getConcrete() );
        ASSERT( pType );

        InstanceVariable* pInstance = new InstanceVariable( pVariable, pType );
        {
            MonomorphicReferenceInstruction* pMono = new MonomorphicReferenceInstruction( pVariable, pInstance );
            pInstruction->append( pMono );
            pInstruction = pMono;
        }

        {
            EnumerationInstruction* pEnumeration = new EnumerationInstruction( pInstance );
            pInstruction->append( pEnumeration );
            pInstruction = pEnumeration;
        }

        {
            PruneInstruction* pPrune = new PruneInstruction;
            pInstruction->append( pPrune );
            pInstruction = pPrune;
        }

        ASSERT( !current.getChildren().empty() );
        for ( std::size_t index : current.getChildren() )
        {
            const Name& next = m_resolution.getNodes()[ index ];
            buildEnum( current, next, pInstruction, pInstance );
        }
    }

    void operator()( Instruction* pInstruction, ContextVariable* pVariable )
    {
        const std::vector< const concrete::Element* >& contextTypes = pVariable->getTypes();
        if ( contextTypes.size() > 1U )
        {
            PolymorphicReferenceBranchInstruction* pBranch = new PolymorphicReferenceBranchInstruction( pVariable );
            pInstruction->append( pBranch );

            // start the recursion
            const Name& current = m_resolution.getNodes()[ 0U ];
            for ( std::size_t index : current.getChildren() )
            {
                const Name& next = m_resolution.getNodes()[ index ];
                buildPolymorphicCase( current, next, pBranch, pVariable );
            }
        }
        else
        {
            const Name& current = m_resolution.getNodes()[ 0U ];
            ASSERT( current.getChildren().size() == 1U );
            for ( std::size_t index : current.getChildren() )
            {
                const Name& next = m_resolution.getNodes()[ index ];
                buildMonoDereference( current, next, pInstruction, pVariable );
            }
        }
    }
};

void InvocationSolution::analyseReturnTypes()
{
    std::vector< const Operation* > operations;
    m_pRoot->getOperations( operations );

    for ( const Operation* pOperation : operations )
    {
        pOperation->getReturnTypes( m_returnTypes );
        pOperation->getParameterTypes( m_parameterTypes );
    }
    m_returnTypes = uniquify_without_reorder( m_returnTypes );
    m_parameterTypes = uniquify_without_reorder( m_parameterTypes );
    ASSERT( std::set< const interface::Element* >( m_returnTypes.begin(), m_returnTypes.end() ).size() == m_returnTypes.size() );
    if ( isOperationEnumeration( getOperation() ) )
    {
        const int iMaxRanges = m_pRoot->setReturnTypes( m_returnTypes );
        m_pRoot->setMaxRanges( iMaxRanges );
    }

    std::vector< const interface::Dimension* > dimensions;
    std::vector< const interface::Context* >   actions;
    for ( const interface::Element* pReturnType : m_returnTypes )
    {
        if ( const interface::Dimension* pDimension = dynamic_cast< const interface::Dimension* >( pReturnType ) )
        {
            dimensions.push_back( pDimension );
        }
        else if ( const interface::Context* pAction = dynamic_cast< const interface::Context* >( pReturnType ) )
        {
            actions.push_back( pAction );
        }
    }

    if ( dimensions.size() && actions.size() )
    {
        THROW_INVOCATION_EXCEPTION( "Mixed dimension and action invocation return types: " << m_invocationID );
    }
    else if ( dimensions.size() )
    {
        m_bDimensions = true;
        m_bHomogeneousDimensions = interface::Dimension::isHomogenous( dimensions );
    }
    else if ( actions.size() )
    {
        m_bDimensions = false;
    }
    else
    {
        THROW_INVOCATION_EXCEPTION( "Invocation could not resolve target types: " << m_invocationID );
    }
}

void InvocationSolution::build( const DerivationAnalysis& analysis, const NameResolution& resolution )
{
    ASSERT( !m_pRoot );

    std::vector< const concrete::Element* > types;
    for ( const ElementPair& element : m_context )
    {
        types.push_back( element.second );
    }
    ContextVariable* pContextVariable = new ContextVariable( types );
    m_pRoot = new RootInstruction( pContextVariable );

    switch ( getOperation() )
    {
    case id_Imp_NoParams:
    case id_Imp_Params:
    case id_Start:
    case id_Stop:
    case id_Pause:
    case id_Resume:
    case id_Wait:
    case id_Get:
    case id_Done:
    {
        GenericOperationVisitor builder( *this, resolution );
        builder( m_pRoot, pContextVariable );
    }
    break;
    case id_Raw:
    case id_Range:
    {
        EnumerationOperationVisitor builder( *this, resolution );
        builder( m_pRoot, pContextVariable );
    }
    break;
    case HIGHEST_OPERATION_TYPE:
        break;
    }

    switch ( m_pRoot->eliminate() )
    {
    case Instruction::eSuccess:
        analyseReturnTypes();
        break;
    case Instruction::eFailure:
        THROW_INVOCATION_EXCEPTION( "No possible derivation for: " << m_invocationID );
    case Instruction::eAmbiguous:
    {
        // get the operations
        std::vector< const Operation* > operations;
        m_pRoot->getOperations( operations );
        Context returnTypes;
        for ( const Operation* pOperation : operations )
        {
            pOperation->getReturnTypes( returnTypes );
        }

        // temp calculate if dimension return types
        std::vector< const interface::Dimension* > dimensions;
        {
            for ( const interface::Element* pReturnType : returnTypes )
            {
                if ( const interface::Dimension* pDimension = dynamic_cast< const interface::Dimension* >( pReturnType ) )
                {
                    dimensions.push_back( pDimension );
                }
            }
        }

        // if starter then accept explicit concrete action type over deriving
        if ( dimensions.empty() )
        // if( ( m_explicitOperation == id_exp_Call ) || ( m_explicitOperation == id_exp_Start ) )
        {
            // determine if there are target elements that are concrete
            // and have a corresponding concrete type
            ASSERT( !m_typePath.empty() );
            const ElementPairVector& last = m_typePath.back();
            ElementPairVector        concreteTargets;
            for ( const ElementPair& element : last )
            {
                const interface::Context* pInterfaceContext = dynamic_cast< const interface::Context* >( element.first );
                if ( pInterfaceContext )
                {
                    const concrete::Action* pConcreteAction = dynamic_cast< const concrete::Action* >( element.second );
                    if ( pConcreteAction->getContext() == pInterfaceContext )
                        concreteTargets.push_back( element );
                }
            }

            // if so use them and eliminate the others
            if ( !concreteTargets.empty() )
            {
                std::vector< const Operation* > candidateOperations;
                for ( const Operation* pOperation : operations )
                {
                    if ( const CallOperation* pCall = dynamic_cast< const CallOperation* >( pOperation ) )
                    {
                        for ( const ElementPair& element : concreteTargets )
                        {
                            if ( ( element.first == pCall->getInterfaceType() ) && ( element.second == pCall->getConcreteType() ) )
                            {
                                candidateOperations.push_back( pCall );
                            }
                        }
                    }
                    else if ( const StartOperation* pStart = dynamic_cast< const StartOperation* >( pOperation ) )
                    {
                        for ( const ElementPair& element : concreteTargets )
                        {
                            if ( ( element.first == pStart->getInterfaceType() ) && ( element.second == pStart->getConcreteType() ) )
                            {
                                candidateOperations.push_back( pStart );
                            }
                        }
                    }
                    else
                    {
                        ASSERT( false );
                    }
                }

                if ( !candidateOperations.empty() )
                {
                    switch ( m_pRoot->secondStageElimination( candidateOperations ) )
                    {
                    case Instruction::eSuccess:
                        analyseReturnTypes();
                        break;
                    case Instruction::eFailure:
                        THROW_INVOCATION_EXCEPTION( "No possible derivation for: " << m_invocationID );
                    case Instruction::eAmbiguous:
                        THROW_INVOCATION_EXCEPTION( "Ambiguous derivation for: " << m_invocationID );
                    }
                }
                else
                {
                    THROW_INVOCATION_EXCEPTION( "Ambiguous derivation for: " << m_invocationID );
                }
            }
            else
            {
                THROW_INVOCATION_EXCEPTION( "Ambiguous derivation for: " << m_invocationID );
            }
        }
        else
        {
            THROW_INVOCATION_EXCEPTION( "Ambiguous derivation for: " << m_invocationID );
        }
    }
    break;
    }

    // set the explicit operation type
    {
        std::vector< const Operation* > operations;
        m_pRoot->getOperations( operations );
        for ( const Operation* pOperation : operations )
        {
            const ExplicitOperationID opType = pOperation->getExplicitOperationType();
            if ( m_explicitOperation == HIGHEST_EXPLICIT_OPERATION_TYPE )
            {
                m_explicitOperation = opType;
            }
            else if ( opType != m_explicitOperation )
            {
                // error
                THROW_INVOCATION_EXCEPTION( "Conflicting operation types: " << m_invocationID );
            }
        }
    }
}

void InvocationSolution::evaluate( RuntimeEvaluator& evaluator, const reference& context ) const
{
    evaluator.initialise();
    evaluator.setVarValue( m_pRoot->getContextVariable(), context );
    m_pRoot->evaluate( evaluator );
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

std::vector< const interface::Element* > fromEGTypeID( const io::Object::Array& objects,
                                                       const Identifiers& identifiers, TypeID typeID, bool bForceDecl )
{
    std::vector< const interface::Element* > result;

    if ( !isOperationType( typeID ) )
    {
        if ( typeID < 0 )
        {
            if ( -typeID < static_cast< int >( objects.size() ) )
            {
                if ( const interface::Element* pElement = dynamic_cast< const interface::Element* >( objects[ -typeID ] ) )
                {
                    return identifiers.getGroup( pElement );
                }
            }
        }
        else if ( bForceDecl )
        {
            if ( typeID < static_cast< int >( objects.size() ) )
            {
                if ( const interface::Element* pElement = dynamic_cast< const interface::Element* >( objects[ typeID ] ) )
                {
                    return identifiers.getGroupBack( pElement );
                }
            }
        }
        else if ( typeID < static_cast< int >( objects.size() ) )
        {
            if ( const interface::Element* pElement = dynamic_cast< const interface::Element* >( objects[ typeID ] ) )
            {
                result.push_back( pElement );
            }
        }
    }
    return result;
}

InvocationSolution::InvocationID InvocationSolution::invocationIDFromTypeIDs(
    const io::Object::Array& objects, const Identifiers& identifiers,
    const std::vector< TypeID >& contextTypes,
    const std::vector< TypeID >& implicitTypePath, OperationID operationType )
{
    std::vector< const interface::Element* > context;
    {
        for ( TypeID typeID : contextTypes )
        {
            std::vector< const interface::Element* > result = fromEGTypeID( objects, identifiers, typeID, false );
            std::copy( result.begin(), result.end(), std::back_inserter( context ) );
        }
        context = uniquify_without_reorder( context );
    }

    std::vector< std::vector< const interface::Element* > > typePath;
    {
        for ( TypeID typeID : implicitTypePath )
        {
            std::vector< const interface::Element* > result = fromEGTypeID( objects, identifiers, typeID, true );
            if ( !result.empty() )
                typePath.push_back( result );
        }
    }

    return InvocationSolution::InvocationID( context, typePath, operationType );
}

InvocationSolution::InvocationID InvocationSolution::invocationIDFromTypeIDs(
    const io::Object::Array& objects, const Identifiers& identifiers,
    const TypeID                 runtimeContextType,
    const std::vector< TypeID >& implicitTypePath, bool bHasParameters )
{
    InvocationSolution::Context context;
    {
        ASSERT( runtimeContextType > 0 );
        const concrete::Action* pContextAction = dynamic_cast< const concrete::Action* >( objects[ runtimeContextType ] );
        ASSERT( pContextAction );
        context.push_back( pContextAction->getContext() );
    }

    OperationID                  operationType = HIGHEST_OPERATION_TYPE;
    InvocationSolution::TypePath typePath;
    {
        for ( TypeID typeID : implicitTypePath )
        {
            ASSERT( typeID < 0 );
            if ( isOperationType( typeID ) )
            {
                operationType = static_cast< OperationID >( typeID );
            }
            else
            {
                ASSERT( -typeID < static_cast< int >( objects.size() ) );

                const interface::Element* pElement = dynamic_cast< const interface::Element* >( objects[ -typeID ] );
                ASSERT( pElement );
                if ( pElement )
                {
                    std::vector< const interface::Element* > elements = identifiers.getGroup( pElement );
                    typePath.push_back( elements );
                }
            }
        }
    }

    if ( operationType == HIGHEST_OPERATION_TYPE )
    {
        // infer the operation type
        if ( bHasParameters )
        {
            operationType = id_Imp_Params;
        }
        else
        {
            operationType = id_Imp_NoParams;
        }
    }

    return InvocationSolution::InvocationID( context, typePath, operationType );
}

const interface::Context* chooseMostDerived( const interface::Context* pConcreteAction,
                                             const interface::Context* pFirst, const interface::Context* pSecond )
{
    ASSERT( pFirst != pSecond );

    if ( pConcreteAction == pFirst )
        return pFirst;
    else if ( pConcreteAction == pSecond )
        return pSecond;
    else
    {
        const interface::Context* pResult = nullptr;
        for ( const interface::Context* pBase : pConcreteAction->getBaseContexts() )
        {
            if ( const interface::Context* p = chooseMostDerived( pBase, pFirst, pSecond ) )
            {
                ASSERT( !pResult );
                pResult = p;
            }
        }
        return pResult;
    }
}

InvocationSolution::ElementPairVector InvocationSolution::getElementVector( const DerivationAnalysis&                       analysis,
                                                                            const std::vector< const interface::Element* >& interfaceElements, bool bIncludeInherited )
{
    ElementPairVector result;

    for ( const interface::Element* pInterfaceElement : interfaceElements )
    {
        std::vector< const concrete::Element* > instances;
        analysis.getInstances( pInterfaceElement, instances, bIncludeInherited );

        for ( const concrete::Element* pConcreteElement : instances )
        {
            // only want each concrete element to occur once in the type path element
            // so scan through existing
            bool bFound = false;
            for ( ElementPairVector::iterator i = result.begin(),
                                              iEnd = result.end();
                  i != iEnd;
                  ++i )
            {
                if ( i->second == pConcreteElement )
                {
                    // choose which interface type to keep - choose the MOST deriving one
                    const concrete::Action* pConcreteAction = dynamic_cast< const concrete::Action* >( pConcreteElement );
                    // the concrete element can only occur be an action if it has occured twice
                    // because dimensions do not use inheritance
                    ASSERT( pConcreteAction );

                    const interface::Context* pFirst = dynamic_cast< const interface::Context* >( i->first );
                    const interface::Context* pSecond = dynamic_cast< const interface::Context* >( pInterfaceElement );

                    i->first = chooseMostDerived( pConcreteAction->getContext(), pFirst, pSecond );
                    ASSERT( i->first );

                    bFound = true;
                }
            }

            if ( !bFound )
            {
                result.push_back( std::make_pair( pInterfaceElement, pConcreteElement ) );
            }
        }
    }

    return result;
}

InvocationSolution::ElementPairVector InvocationSolution::getElementVector( const DerivationAnalysis&                 analysis,
                                                                            const std::vector< interface::Context* >& interfaceElements, bool bIncludeInherited )
{
    std::vector< const interface::Element* > elements;
    for ( interface::Context* pAction : interfaceElements )
        elements.push_back( pAction );
    return getElementVector( analysis, elements, bIncludeInherited );
}

InvocationSolution* InvocationSolutionMap::constructInvocation(
    const InvocationSolution::InvocationID& invocationID, const std::vector< TypeID >& implicitTypePath )
{
    // construct new invocation
    InvocationSolution* pInvocation = m_stage.construct< InvocationSolution >( invocationID, implicitTypePath );

    // calculate the concrete context
    {
        pInvocation->m_context = InvocationSolution::getElementVector(
            m_analysis, std::get< InvocationSolution::Context >( invocationID ), true );
        if ( pInvocation->m_context.empty() )
        {
            THROW_NAMERESOLUTION_EXCEPTION( "Invalid context. " << invocationID );
        }
    }

    const bool bIsImplicit = isOperationImplicit( std::get< OperationID >( invocationID ) );

    {
        const InvocationSolution::TypePath& typePath = std::get< InvocationSolution::TypePath >( invocationID );
        if ( !typePath.empty() )
        {
            for ( InvocationSolution::TypePath::const_iterator
                      i
                  = typePath.begin(),
                  iLast = typePath.end() - 1U,
                  iEnd = typePath.end();
                  i != iEnd;
                  ++i )
            {
                const std::vector< const interface::Element* >& typePathElement = *i;

                const bool bIncludeInherited = true;

                InvocationSolution::ElementPairVector elementVector = InvocationSolution::getElementVector(
                    m_analysis, typePathElement, bIncludeInherited );
                if ( elementVector.empty() )
                {
                    THROW_NAMERESOLUTION_EXCEPTION( "Invalid type path. " << invocationID );
                }
                pInvocation->m_typePath.push_back( elementVector );
            }
        }

        if ( pInvocation->m_typePath.empty() && bIsImplicit )
        {
            THROW_NAMERESOLUTION_EXCEPTION( "Empty type path for implicit invocation. " << invocationID );
        }
    }

    return pInvocation;
}

const InvocationSolution* InvocationSolutionMap::getInvocation(
    const InvocationSolution::InvocationID& invocationID,
    const std::vector< TypeID >&            implicitTypePath )
{
    InvocationSolution::InvocationMap::const_iterator
        iFind
        = m_invocations.find( invocationID );
    if ( iFind != m_invocations.end() )
    {
        return iFind->second;
    }

    InvocationSolution* pInvocation = constructInvocation( invocationID, implicitTypePath );

    NameResolution nameResolution( m_analysis, invocationID,
                                   pInvocation->getContextElements(), pInvocation->getTypePathElements() );

    pInvocation->build( m_analysis, nameResolution );

    m_invocations.insert( std::make_pair( invocationID, pInvocation ) );

    return pInvocation;
}
} // namespace mega