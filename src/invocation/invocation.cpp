
#include "invocation/invocation.hpp"

#include "invocation/name_resolution.hpp"
#include "invocation/generic_operation_visitor.hpp"
#include "invocation/elimination.hpp"

#include "database/model/OperationsStage.hxx"
#include "database/types/operation.hpp"

#include "mega/common.hpp"
#include "mega/common_strings.hpp"
#include <optional>

namespace mega
{
namespace invocation
{
using namespace OperationsStage;
using namespace OperationsStage::Operations;

struct SymbolMaps
{
    using SymbolIDMap  = std::map< std::int32_t, OperationsStage::Symbols::Symbol* >;
    using IContextMap  = const std::map< int32_t, Interface::IContext* >;
    using DimensionMap = std::map< int32_t, Interface::DimensionTrait* >;

    SymbolIDMap  m_symbolIDMap;
    IContextMap  m_contextMap;
    DimensionMap m_dimensionMap;

    SymbolMaps( Symbols::SymbolTable& symbolTable )
        : m_symbolIDMap( symbolTable.get_symbol_id_map() )
        , m_contextMap( symbolTable.get_context_type_ids() )
        , m_dimensionMap( symbolTable.get_dimension_type_ids() )
    {
    }
};

InterfaceVariantVector symbolToInterfaceVariantVector( Database& database, Symbols::Symbol* pSymbol )
{
    InterfaceVariantVector result;
    for ( Interface::IContext* pContext : pSymbol->get_contexts() )
    {
        InterfaceVariant* pInterfaceVariant = database.construct< InterfaceVariant >(
            InterfaceVariant::Args{ pContext, std::optional< Interface::DimensionTrait* >() } );
        result.push_back( pInterfaceVariant );
    }

    for ( Interface::DimensionTrait* pDimension : pSymbol->get_dimensions() )
    {
        InterfaceVariant* pInterfaceVariant = database.construct< InterfaceVariant >(
            InterfaceVariant::Args{ std::optional< Interface::IContext* >(), pDimension } );
        result.push_back( pInterfaceVariant );
    }
    return result;
}

InterfaceVariantVector symbolIDToInterfaceVariant( Database& database, const SymbolMaps& symbolMaps,
                                                   mega::SymbolID symbolID )
{
    if ( symbolID < 0 )
    {
        auto iFind = symbolMaps.m_symbolIDMap.find( symbolID );
        VERIFY_RTE( iFind != symbolMaps.m_symbolIDMap.end() );
        return symbolToInterfaceVariantVector( database, iFind->second );
    }
    else
    {
        auto iFind = symbolMaps.m_contextMap.find( symbolID );
        if ( iFind != symbolMaps.m_contextMap.end() )
        {
            InterfaceVariantVector result;
            InterfaceVariant*      pInterfaceVariant = database.construct< InterfaceVariant >(
                InterfaceVariant::Args{ iFind->second, std::optional< Interface::DimensionTrait* >() } );
            result.push_back( pInterfaceVariant );
            return result;
        }
        else
        {
            auto iFind = symbolMaps.m_dimensionMap.find( symbolID );
            if ( iFind != symbolMaps.m_dimensionMap.end() )
            {
                InterfaceVariantVector result;
                InterfaceVariant*      pInterfaceVariant = database.construct< InterfaceVariant >(
                    InterfaceVariant::Args{ std::optional< Interface::IContext* >(), iFind->second } );
                result.push_back( pInterfaceVariant );
                return result;
            }
        }
    }
    THROW_RTE( "Failed to resolve symbol id" );
}

InterfaceVariantVectorVector symbolVectorToInterfaceVariantVector( Database&                              database,
                                                                   const std::vector< Symbols::Symbol* >& symbols )
{
    InterfaceVariantVectorVector result;
    for ( Symbols::Symbol* pSymbol : symbols )
    {
        InterfaceVariantVector interfaceVariantVector = symbolToInterfaceVariantVector( database, pSymbol );
        result.push_back( interfaceVariantVector );
    }
    return result;
}

InterfaceVariantVectorVector
symbolIDVectorToInterfaceVariantVector( Database& database, const SymbolMaps& symbolMaps,
                                        const std::vector< mega::SymbolID >& symbolIDs,
                                        std::optional< mega::OperationID >&  operationIDOpt )
{
    InterfaceVariantVectorVector result;

    for ( mega::SymbolID symbolID : symbolIDs )
    {
        if ( isOperationType( symbolID ) )
        {
            operationIDOpt = static_cast< mega::OperationID >( symbolID );
            continue;
        }
        InterfaceVariantVector interfaceVariantVector = symbolIDToInterfaceVariant( database, symbolMaps, symbolID );
        VERIFY_RTE( !interfaceVariantVector.empty() );
        result.push_back( interfaceVariantVector );
    }

    return result;
}

ElementVector* toElementVector( Database& database, const InterfaceVariantVector& interfaceVariantVector )
{
    std::vector< Element* > elements;

    for ( InterfaceVariant* pInterfaceVariant : interfaceVariantVector )
    {
        if ( pInterfaceVariant->get_context().has_value() )
        {
            Interface::IContext* pContext = pInterfaceVariant->get_context().value();
            for ( Concrete::Context* pConcrete : pContext->get_concrete() )
            {
                InterfaceVariant* pInterfaceVar
                    = database.construct< InterfaceVariant >( InterfaceVariant::Args{ pContext, std::nullopt } );
                ConcreteVariant* pConcreteVar
                    = database.construct< ConcreteVariant >( ConcreteVariant::Args{ pConcrete, std::nullopt } );
                Element* pElement = database.construct< Element >( Element::Args{ pInterfaceVar, pConcreteVar } );
                elements.push_back( pElement );
            }
        }
        else if ( pInterfaceVariant->get_dimension().has_value() )
        {
            Interface::DimensionTrait* pDimension = pInterfaceVariant->get_dimension().value();
            for ( Concrete::Dimensions::User* pConcreteDimension : pDimension->get_concrete() )
            {
                InterfaceVariant* pInterfaceVar
                    = database.construct< InterfaceVariant >( InterfaceVariant::Args{ std::nullopt, pDimension } );
                ConcreteVariant* pConcreteVar = database.construct< ConcreteVariant >(
                    ConcreteVariant::Args{ std::nullopt, pConcreteDimension } );
                Element* pElement = database.construct< Element >( Element::Args{ pInterfaceVar, pConcreteVar } );
                elements.push_back( pElement );
            }
        }
        else
        {
            THROW_RTE( "Missing context in interface variant" );
        }
    }

    VERIFY_RTE( !elements.empty() );

    return database.construct< ElementVector >( ElementVector::Args{ elements } );
}

std::vector< ElementVector* > toElementVector( Database&                           database,
                                               const InterfaceVariantVectorVector& interfaceVariantVectorVector )
{
    std::vector< ElementVector* > result;
    for ( const InterfaceVariantVector& interfaceVariantVector : interfaceVariantVectorVector )
    {
        result.push_back( toElementVector( database, interfaceVariantVector ) );
    }
    return result;
}

void analyseReturnTypes( Database& database, Invocation* pInvocation )
{
    std::vector< OperationsStage::Interface::IContext* >       contextReturnTypes;
    std::vector< OperationsStage::Interface::DimensionTrait* > dimensionReturnTypes;
    bool                                                       bIsWriteOperation = false;
    {
        using OperationsStage::Invocations::Instructions::Instruction;
        using OperationsStage::Invocations::Operations::BasicOperation;
        using OperationsStage::Invocations::Operations::DimensionOperation;
        using OperationsStage::Invocations::Operations::Operation;
        using OperationsStage::Invocations::Operations::Write;
        for ( Operation* pOperation : getOperations( pInvocation->get_root_instruction() ) )
        {
            if ( dynamic_database_cast< Write >( pOperation ) )
                bIsWriteOperation = true;
            for ( auto pReturnType : pOperation->get_return_types() )
            {
                if ( pReturnType->get_context().has_value() )
                    contextReturnTypes.push_back( pReturnType->get_context().value() );
                else if ( pReturnType->get_dimension().has_value() )
                    dimensionReturnTypes.push_back( pReturnType->get_dimension().value() );
            }
        }
    }

    contextReturnTypes   = uniquify_without_reorder( contextReturnTypes );
    dimensionReturnTypes = uniquify_without_reorder( dimensionReturnTypes );

    bool bIsHomogenous = true;
    {
        if ( contextReturnTypes.size() && dimensionReturnTypes.size() )
        {
            if ( !bIsWriteOperation )
            {
                THROW_INVOCATION_EXCEPTION( "Mixed dimension and action invocation return types" );
            }
        }
        if ( contextReturnTypes.size() )
        {
            bIsHomogenous = contextReturnTypes.size() == 1U;
        }
        if ( dimensionReturnTypes.size() )
        {
            std::optional< std::string > typeOpt;
            for ( OperationsStage::Interface::DimensionTrait* pDim : dimensionReturnTypes )
            {
                if ( typeOpt.has_value() )
                {
                    if ( typeOpt.value() != pDim->get_canonical_type() )
                    {
                        bIsHomogenous = false;
                        break;
                    }
                }
                else
                {
                    typeOpt = pDim->get_canonical_type();
                }
            }
        }
    }
    pInvocation->set_return_types_context( contextReturnTypes );
    pInvocation->set_return_types_dimension( dimensionReturnTypes );
    pInvocation->set_homogeneous( bIsHomogenous );
}

void findDuplicate( OperationsStage::Invocations::Instructions::Instruction* pInstruction, const char* pszMsg )
{
    using namespace OperationsStage::Invocations;
    using namespace OperationsStage::Invocations::Instructions;
    using namespace OperationsStage::Invocations::Operations;

    if ( InstructionGroup* pInstructionGroup = dynamic_database_cast< InstructionGroup >( pInstruction ) )
    {
        auto children = pInstructionGroup->get_children();
        VERIFY_RTE_MSG( children.size() < 2U, "Found duplicate: " << pszMsg );
        for ( Instruction* pChildInstruction : children )
        {
            findDuplicate( pChildInstruction, pszMsg );
        }
    }
}

void build( Database& database, Invocation* pInvocation )
{
    switch ( pInvocation->get_operation() )
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
            GenericOperationVisitor visitor{ database, pInvocation };
            visitor();
        }
        break;
        case id_Raw:
        case id_Range:
        {
            // EnumerationOperationVisitor builder( *this, resolution );
            // builder( m_pRoot, pContextVariable );
        }
        break;
        case HIGHEST_OPERATION_TYPE:
            break;
    }

    const auto firstStageResult = firstStageElimination( pInvocation->get_root_instruction() );

    switch ( firstStageResult )
    {
        case eSuccess:
            analyseReturnTypes( database, pInvocation );
            break;
        case eFailure:
            THROW_INVOCATION_EXCEPTION( "No possible derivation" );
        case eAmbiguous:
        {
            using OperationsStage::Invocations::Instructions::Instruction;
            using OperationsStage::Invocations::Operations::BasicOperation;
            using OperationsStage::Invocations::Operations::DimensionOperation;
            using OperationsStage::Invocations::Operations::Operation;

            // get the operations
            const std::vector< Operation* > operations = getOperations( pInvocation->get_root_instruction() );

            std::vector< Concrete::Context* >          contexts;
            std::vector< Concrete::Dimensions::User* > dimensions;
            for ( Operation* pOperation : operations )
            {
                if ( BasicOperation* pBasicOp = dynamic_database_cast< BasicOperation >( pOperation ) )
                {
                    contexts.push_back( pBasicOp->get_concrete_target() );
                }
                else if ( DimensionOperation* pDimensionOp = dynamic_database_cast< DimensionOperation >( pOperation ) )
                {
                    dimensions.push_back( pDimensionOp->get_concrete_dimension() );
                }
                else
                {
                    THROW_RTE( "Unknown operation type" );
                }
            }

            // if starter then accept explicit concrete action type over deriving
            if ( dimensions.empty() )
            // if( ( m_explicitOperation == id_exp_Call ) || ( m_explicitOperation == id_exp_Start ) )
            {
                // determine if there are target elements that are concrete
                // and have a corresponding concrete type
                std::vector< Element* > nonPolyTargets;
                {
                    TypePath* pTypePath = pInvocation->get_type_path();
                    auto      elements  = pTypePath->get_vectors();
                    VERIFY_RTE( !elements.empty() );
                    auto last = elements.back();
                    for ( Element* pElement : last->get_elements() )
                    {
                        auto pInterface = pElement->get_interface()->get_context().value();
                        auto pConcrete  = pElement->get_concrete()->get_context().value();

                        auto t = pInterface->get_concrete();
                        if ( std::find( t.begin(), t.end(), pConcrete ) != t.end() )
                        {
                            nonPolyTargets.push_back( pElement );
                        }
                    }
                }

                // if so use them and eliminate the others
                if ( !nonPolyTargets.empty() )
                {
                    std::vector< Operation* > candidateOperations;
                    for ( Operation* pOperation : operations )
                    {
                        using OperationsStage::Invocations::Operations::Allocate;
                        using OperationsStage::Invocations::Operations::Call;
                        using OperationsStage::Invocations::Operations::Start;

                        if ( Allocate* pAllocate = dynamic_database_cast< Allocate >( pOperation ) )
                        {
                            for ( Element* pElement : nonPolyTargets )
                            {
                                if ( pElement->get_concrete()->get_context().value() == pAllocate->get_concrete_target()
                                     && pElement->get_interface()->get_context().value() == pAllocate->get_interface() )
                                {
                                    candidateOperations.push_back( pAllocate );
                                }
                            }
                        }
                        else if ( Call* pCall = dynamic_database_cast< Call >( pOperation ) )
                        {
                            for ( Element* pElement : nonPolyTargets )
                            {
                                if ( pElement->get_concrete()->get_context().value() == pCall->get_concrete_target()
                                     && pElement->get_interface()->get_context().value() == pCall->get_interface() )
                                {
                                    candidateOperations.push_back( pCall );
                                }
                            }
                        }
                        else if ( Start* pStart = dynamic_database_cast< Start >( pOperation ) )
                        {
                            for ( Element* pElement : nonPolyTargets )
                            {
                                if ( pElement->get_concrete()->get_context().value() == pStart->get_concrete_target()
                                     && pElement->get_interface()->get_context().value() == pStart->get_interface() )
                                {
                                    candidateOperations.push_back( pStart );
                                }
                            }
                        }
                        else
                        {
                            THROW_RTE( "Unexpected operation type" );
                        }
                    }

                    if ( !candidateOperations.empty() )
                    {
                        switch ( secondStageElimination( candidateOperations, pInvocation->get_root_instruction() ) )
                        {
                            case eSuccess:
                                analyseReturnTypes( database, pInvocation );
                                break;
                            case eFailure:
                                THROW_INVOCATION_EXCEPTION( "No possible derivation" );
                            case eAmbiguous:
                                THROW_INVOCATION_EXCEPTION( "Ambiguous derivation" );
                        }
                    }
                    else
                    {
                        THROW_INVOCATION_EXCEPTION( "Ambiguous derivation for" );
                    }
                }
                else
                {
                    THROW_INVOCATION_EXCEPTION( "Ambiguous derivation for" );
                }
            }
            else
            {
                THROW_INVOCATION_EXCEPTION( "Ambiguous derivation for" );
            }
        }
        break;
    }
}

void printIContextFullType( OperationsStage::Interface::IContext* pContext, std::ostream& os )
{
    using IContextVector = std::vector< Interface::IContext* >;
    IContextVector path;
    while ( pContext )
    {
        path.push_back( pContext );
        pContext = dynamic_database_cast< Interface::IContext >( pContext->get_parent() );
    }
    std::reverse( path.begin(), path.end() );
    for ( IContextVector::const_iterator i = path.begin(), iNext = path.begin(), iEnd = path.end(); i != iEnd; ++i )
    {
        ++iNext;
        if ( iNext == iEnd )
        {
            os << ( *i )->get_identifier();
        }
        else
        {
            os << ( *i )->get_identifier() << "::";
        }
    }
}

void printContextType( std::vector< OperationsStage::Interface::IContext* >& contexts, std::ostream& os )
{
    VERIFY_RTE( !contexts.empty() );
    if ( contexts.size() == 1 )
    {
        printIContextFullType( contexts.front(), os );
    }
    else
    {
        os << EG_VARIANT_TYPE << "< ";
        bool bFirst = true;
        for ( Interface::IContext* pContext : contexts )
        {
            if ( bFirst )
                bFirst = false;
            else
                os << ", ";
            printIContextFullType( pContext, os );
        }
        os << " >";
    }
}

void setOrCheck( std::optional< ExplicitOperationID >& resultOpt, ExplicitOperationID value )
{
    if ( !resultOpt.has_value() )
        resultOpt = value;
    else
        VERIFY_RTE_MSG( resultOpt.value() == value, "Conflicting explicit operation type found" );
}
ExplicitOperationID determineExplicitOperationType( Invocation* pInvocation )
{
    std::optional< ExplicitOperationID > resultOpt;

    using namespace OperationsStage::Invocations::Operations;
    for ( auto pOperation : getOperations( pInvocation->get_root_instruction() ) )
    {
        bool bFound = false;
        if ( !bFound )
        {
            if ( Allocate* pOp = dynamic_database_cast< Allocate >( pOperation ) )
            {
                setOrCheck( resultOpt, id_exp_Allocate );
                bFound = true;
            }
        }
        if ( !bFound )
        {
            if ( Call* pOp = dynamic_database_cast< Call >( pOperation ) )
            {
                setOrCheck( resultOpt, id_exp_Call );
                bFound = true;
            }
        }
        if ( !bFound )
        {
            if ( Start* pOp = dynamic_database_cast< Start >( pOperation ) )
            {
                setOrCheck( resultOpt, id_exp_Start );
                bFound = true;
            }
        }
        if ( !bFound )
        {
            if ( Stop* pOp = dynamic_database_cast< Stop >( pOperation ) )
            {
                setOrCheck( resultOpt, id_exp_Stop );
                bFound = true;
            }
        }
        if ( !bFound )
        {
            if ( Pause* pOp = dynamic_database_cast< Pause >( pOperation ) )
            {
                setOrCheck( resultOpt, id_exp_Pause );
                bFound = true;
            }
        }
        if ( !bFound )
        {
            if ( Resume* pOp = dynamic_database_cast< Resume >( pOperation ) )
            {
                setOrCheck( resultOpt, id_exp_Resume );
                bFound = true;
            }
        }
        if ( !bFound )
        {
            if ( Done* pOp = dynamic_database_cast< Done >( pOperation ) )
            {
                setOrCheck( resultOpt, id_exp_Done );
                bFound = true;
            }
        }
        if ( !bFound )
        {
            if ( WaitAction* pOp = dynamic_database_cast< WaitAction >( pOperation ) )
            {
                setOrCheck( resultOpt, id_exp_WaitAction );
                bFound = true;
            }
        }
        if ( !bFound )
        {
            if ( WaitDimension* pOp = dynamic_database_cast< WaitDimension >( pOperation ) )
            {
                setOrCheck( resultOpt, id_exp_WaitDimension );
                bFound = true;
            }
        }
        if ( !bFound )
        {
            if ( GetAction* pOp = dynamic_database_cast< GetAction >( pOperation ) )
            {
                setOrCheck( resultOpt, id_exp_GetAction );
                bFound = true;
            }
        }
        if ( !bFound )
        {
            if ( GetDimension* pOp = dynamic_database_cast< GetDimension >( pOperation ) )
            {
                setOrCheck( resultOpt, id_exp_GetDimension );
                bFound = true;
            }
        }
        if ( !bFound )
        {
            if ( Read* pOp = dynamic_database_cast< Read >( pOperation ) )
            {
                setOrCheck( resultOpt, id_exp_Read );
                bFound = true;
            }
        }
        if ( !bFound )
        {
            if ( Write* pOp = dynamic_database_cast< Write >( pOperation ) )
            {
                setOrCheck( resultOpt, id_exp_Write );
                bFound = true;
            }
        }
        if ( !bFound )
        {
            if ( WriteLink* pOp = dynamic_database_cast< WriteLink >( pOperation ) )
            {
                setOrCheck( resultOpt, id_exp_Write );
                bFound = true;
            }
        }
        if ( !bFound )
        {
            if ( Range* pOp = dynamic_database_cast< Range >( pOperation ) )
            {
                setOrCheck( resultOpt, id_exp_Range );
                bFound = true;
            }
        }
    }

    VERIFY_RTE_MSG( resultOpt.has_value(),
                    "Failed to determine explicit operation type for invocation: " << pInvocation->get_name() );
    return resultOpt.value();
}

OperationsStage::Operations::Invocation* construct( io::Environment& environment, const mega::InvocationID& id,
                                                    Database& database, const mega::io::megaFilePath& sourceFile )
{
    // std::cout << "Found invocation: " << id << std::endl;

    const mega::io::manifestFilePath manifestFile = environment.project_manifest();
    Symbols::SymbolTable*            pSymbolTable = database.one< Symbols::SymbolTable >( manifestFile );

    SymbolMaps symbolMaps( *pSymbolTable );

    std::optional< mega::OperationID > operationIDOpt;

    // 1. Convert from the SymbolIDs to Interface Contexts
    InterfaceVariantVectorVector context
        = symbolIDVectorToInterfaceVariantVector( database, symbolMaps, id.m_context, operationIDOpt );

    InterfaceVariantVectorVector typePath
        = symbolIDVectorToInterfaceVariantVector( database, symbolMaps, id.m_type_path, operationIDOpt );

    if ( operationIDOpt.has_value() )
    {
        THROW_RTE( "Invalid operation specified in invocation" );
    }

    // 2. Convert from Interface Contexts to Interface/Concrete context pair element vectors
    std::vector< ElementVector* > contextElements  = toElementVector( database, context );
    std::vector< ElementVector* > typePathElements = toElementVector( database, typePath );

    Context*  pContext  = database.construct< Context >( Context::Args{ contextElements } );
    TypePath* pTypePath = database.construct< TypePath >( TypePath::Args{ typePathElements } );

    // rebuild the type path string
    std::ostringstream osTypePathStr;
    {
        osTypePathStr << mega::EG_TYPE_PATH << "< ";
        bool bFirst = true;
        for ( mega::SymbolID symbolID : id.m_type_path )
        {
            if ( bFirst )
                bFirst = false;
            else
                osTypePathStr << ", ";
            if ( symbolID < 0 )
            {
                auto iFind = symbolMaps.m_symbolIDMap.find( symbolID );
                VERIFY_RTE( iFind != symbolMaps.m_symbolIDMap.end() );
                osTypePathStr << iFind->second->get_symbol();
            }
            else
            {
                auto iFind = symbolMaps.m_contextMap.find( symbolID );
                if ( iFind != symbolMaps.m_contextMap.end() )
                {
                    printIContextFullType( iFind->second, osTypePathStr );
                }
                else
                {
                    auto iFind = symbolMaps.m_dimensionMap.find( symbolID );
                    VERIFY_RTE( iFind != symbolMaps.m_dimensionMap.end() );
                    printIContextFullType( iFind->second->get_parent(), osTypePathStr );
                    osTypePathStr << "::" << iFind->second->get_id()->get_str();
                }
            }
        }
        osTypePathStr << " >";
    }

    std::ostringstream osName;
    std::ostringstream osContextStr;
    {
        if ( context.size() > 1 )
        {
            osName << EG_VARIANT_TYPE << "< ";
            osContextStr << EG_VARIANT_TYPE << "< ";
        }
        {
            bool bFirst = true;
            for ( InterfaceVariantVector& ivv : context )
            {
                if ( bFirst )
                    bFirst = false;
                else
                {
                    osName << ", ";
                    osContextStr << ", ";
                }
                OperationsStage::Operations::InterfaceVariant* pFirst = ivv.front();
                if ( pFirst->get_context().has_value() )
                {
                    Interface::IContext* pContext = pFirst->get_context().value();
                    osName << pContext->get_identifier();
                    printIContextFullType( pContext, osContextStr );
                }
                else
                {
                    VERIFY_RTE( pFirst->get_dimension().has_value() );
                    Interface::DimensionTrait* pDimension = pFirst->get_dimension().value();
                    osName << pDimension->get_id()->get_str();
                    printIContextFullType( pDimension->get_parent(), osContextStr );
                    osContextStr << "::" << pDimension->get_id()->get_str();
                }
            }
        }
        if ( context.size() > 1 )
        {
            osName << " >";
            osContextStr << " >";
        }
        {
            for ( InterfaceVariantVector& ivv : typePath )
            {
                OperationsStage::Operations::InterfaceVariant* pFirst = ivv.front();
                if ( pFirst->get_context().has_value() )
                {
                    osName << "." << pFirst->get_context().value()->get_identifier();
                }
                else
                {
                    VERIFY_RTE( pFirst->get_dimension().has_value() );
                    osName << "." << pFirst->get_dimension().value()->get_id()->get_str();
                }
            }
        }
        osName << "." << mega::getOperationString( id.m_operation );
    }

    Invocation* pInvocation = database.construct< Invocation >( Invocation::Args{
        pContext, pTypePath, id.m_operation, osName.str(), osContextStr.str(), osTypePathStr.str(), {} } );

    // 3. Compute name resolution
    NameResolution* pNameResolution = resolve( database, pInvocation );
    pInvocation->set_name_resolution( pNameResolution );

    // 4. Build the instructions
    build( database, pInvocation );

    // 5. Analyse result
    const ExplicitOperationID explicitOperationID = determineExplicitOperationType( pInvocation );
    pInvocation->set_explicit_operation( explicitOperationID );

    std::vector< Interface::IContext* >       contexts   = pInvocation->get_return_types_context();
    std::vector< Interface::DimensionTrait* > dimensions = pInvocation->get_return_types_dimension();

    std::optional< std::string >                strFunctionReturnTypeOpt;
    std::optional< std::vector< std::string > > functionParameterTypesOpt;
    {
        bool bNonFunction = false;
        for ( Interface::IContext* pReturnContext : contexts )
        {
            if ( Interface::Function* pFunctionCall = dynamic_database_cast< Interface::Function >( pReturnContext ) )
            {
                if ( strFunctionReturnTypeOpt.has_value() )
                {
                    if ( strFunctionReturnTypeOpt.value()
                         != pFunctionCall->get_return_type_trait()->get_canonical_type() )
                    {
                        THROW_RTE( "Incompatible function return types" );
                    }
                }
                else
                {
                    strFunctionReturnTypeOpt = pFunctionCall->get_return_type_trait()->get_canonical_type();
                }
                if ( functionParameterTypesOpt.has_value() )
                {
                    if ( functionParameterTypesOpt.value()
                         != pFunctionCall->get_arguments_trait()->get_canonical_types() )
                    {
                        THROW_RTE( "Incompatible function parameter types" );
                    }
                }
                else
                {
                    functionParameterTypesOpt = pFunctionCall->get_arguments_trait()->get_canonical_types();
                }
            }
            else
            {
                bNonFunction = true;
            }
        }
        VERIFY_RTE_MSG( !strFunctionReturnTypeOpt.has_value() || !bNonFunction,
                        "Invalid mix of function and non-function operations" );
    }

    pInvocation->set_is_function_call( strFunctionReturnTypeOpt.has_value() );

    std::ostringstream osReturnTypeStr, osRuntimeReturnType;
    {
        switch ( explicitOperationID )
        {
            case mega::id_exp_Read:
            {
                VERIFY_RTE_MSG( !dimensions.empty(), "Read has no dimensions" );
                VERIFY_RTE_MSG( pInvocation->get_homogeneous(), "Non-homogenous dimensions" );

                std::ostringstream osDimensionTrait;
                {
                    printIContextFullType( dimensions.front()->get_parent(), osDimensionTrait );
                    osDimensionTrait << "::" << dimensions.front()->get_id()->get_str();
                }
                osReturnTypeStr << osDimensionTrait.str() << "::Read";
                osRuntimeReturnType << osDimensionTrait.str() << "::Type";
            }
            break;
            case mega::id_exp_Write:
            {
                VERIFY_RTE_MSG( !contexts.empty(), "Write has no result context" );
                VERIFY_RTE_MSG( !dimensions.empty(), "Write has no dimensions" );
                VERIFY_RTE_MSG( pInvocation->get_homogeneous(), "Write operation on non-homogenous dimensions" );

                printContextType( contexts, osReturnTypeStr );
                std::ostringstream osDimensionTrait;
                {
                    printIContextFullType( dimensions.front()->get_parent(), osDimensionTrait );
                    osDimensionTrait << "::" << dimensions.front()->get_id()->get_str();
                }
                osRuntimeReturnType << osDimensionTrait.str() << "::Type";
            }
            break;
            case mega::id_exp_Allocate:
            {
                printContextType( contexts, osReturnTypeStr );
                osRuntimeReturnType << "mega::reference";
            }
            break;
            case mega::id_exp_Call:
            {
                if( !strFunctionReturnTypeOpt.has_value() )
                {
                    osReturnTypeStr << "void";
                }
                else
                {
                    osReturnTypeStr << strFunctionReturnTypeOpt.value();
                }
                // define function pointer type
                osRuntimeReturnType << osReturnTypeStr.str() << "(*)( mega::reference";
                if( functionParameterTypesOpt.has_value() )
                {
                    for ( const std::string& strType : functionParameterTypesOpt.value() )
                        osRuntimeReturnType << "," << strType;
                }
                osRuntimeReturnType << ")";
            }
            break;
            case mega::id_exp_Start:
            case mega::id_exp_Stop:
            case mega::id_exp_Pause:
            case mega::id_exp_Resume:
            case mega::id_exp_WaitAction:
            case mega::id_exp_WaitDimension:
            case mega::id_exp_GetAction:
            case mega::id_exp_GetDimension:
            case mega::id_exp_Done:
            case mega::id_exp_Range:
            case mega::id_exp_Raw:
            {
                osReturnTypeStr << "void";
                osRuntimeReturnType << "void";
                break;
            }

            case mega::HIGHEST_EXPLICIT_OPERATION_TYPE:
            default:
                THROW_RTE( "Unsupported operation type" );
        }
    }

    pInvocation->set_return_type_str( osReturnTypeStr.str() );
    pInvocation->set_runtime_return_type_str( osRuntimeReturnType.str() );

    return pInvocation;
}

} // namespace invocation
} // namespace mega
