
#include "invocation/invocation.hpp"

#include "invocation/name_resolution.hpp"
#include "invocation/generic_operation_visitor.hpp"
#include "invocation/elimination.hpp"

#include "database/model/OperationsStage.hxx"
#include "database/types/operation.hpp"

#include "mega/common.hpp"
#include "mega/common_strings.hpp"

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

ElementVector* toElementVector( Database& database, const InheritanceMapping& inheritance,
                                const InterfaceVariantVector& interfaceVariantVector )
{
    std::vector< Element* > elements;

    auto addElement = [ &database, &elements ]( Interface::IContext* pContext )
    {
        Interface::IContext* pDerived = pContext;
        VERIFY_RTE( pDerived->get_concrete().has_value() );
        Concrete::Context* pConcrete = pDerived->get_concrete().value();

        InterfaceVariant* pInterfaceVar = database.construct< InterfaceVariant >(
            InterfaceVariant::Args{ pContext, std::optional< Interface::DimensionTrait* >() } );
        ConcreteVariant* pConcreteVar = database.construct< ConcreteVariant >(
            ConcreteVariant::Args{ pConcrete, std::optional< Concrete::Dimension* >() } );

        Element* pElement = database.construct< Element >( Element::Args{ pInterfaceVar, pConcreteVar } );
        elements.push_back( pElement );
    };

    for ( InterfaceVariant* pInterfaceVariant : interfaceVariantVector )
    {
        if ( pInterfaceVariant->get_context().has_value() )
        {
            Interface::IContext* pContext = pInterfaceVariant->get_context().value();

            addElement( pContext );
            for ( InheritanceMapping::const_iterator i    = inheritance.lower_bound( pContext ),
                                                     iEnd = inheritance.upper_bound( pContext );
                  i != iEnd;
                  ++i )
            {
                addElement( i->second );
            }
        }
        else if ( pInterfaceVariant->get_dimension().has_value() )
        {
            Interface::DimensionTrait* pDimension = pInterfaceVariant->get_dimension().value();
            VERIFY_RTE( pDimension->get_concrete().has_value() );
            Concrete::Dimension* pConcreteDimension = pDimension->get_concrete().value();

            InterfaceVariant* pInterfaceVar = database.construct< InterfaceVariant >(
                InterfaceVariant::Args{ std::optional< Interface::IContext* >(), pDimension } );
            ConcreteVariant* pConcreteVar = database.construct< ConcreteVariant >(
                ConcreteVariant::Args{ std::optional< Concrete::Context* >(), pConcreteDimension } );
            Element* pElement = database.construct< Element >( Element::Args{ pInterfaceVar, pConcreteVar } );
            elements.push_back( pElement );
        }
        else
        {
            THROW_RTE( "Missing context in interface variant" );
        }
    }

    VERIFY_RTE( !elements.empty() );

    return database.construct< ElementVector >( ElementVector::Args{ elements } );
}

std::vector< ElementVector* > toElementVector( Database& database, const InheritanceMapping& inheritance,
                                               const InterfaceVariantVectorVector& interfaceVariantVectorVector )
{
    std::vector< ElementVector* > result;
    for ( const InterfaceVariantVector& interfaceVariantVector : interfaceVariantVectorVector )
    {
        result.push_back( toElementVector( database, inheritance, interfaceVariantVector ) );
    }
    return result;
}

void analyseReturnTypes( Database& database, Invocation* pInvocation )
{
    std::vector< OperationsStage::Interface::IContext* >       contextReturnTypes;
    std::vector< OperationsStage::Interface::DimensionTrait* > dimensionReturnTypes;
    {
        using OperationsStage::Invocations::Instructions::Instruction;
        using OperationsStage::Invocations::Operations::BasicOperation;
        using OperationsStage::Invocations::Operations::DimensionOperation;
        using OperationsStage::Invocations::Operations::Operation;
        std::vector< Operation* > operations;
        getOperations( pInvocation->get_root_instruction(), operations );
        for ( Operation* pOperation : operations )
        {
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
            THROW_INVOCATION_EXCEPTION( "Mixed dimension and action invocation return types" );
        }
        else if ( contextReturnTypes.size() )
        {
            bIsHomogenous = contextReturnTypes.size() == 1U;
        }
        else if ( dimensionReturnTypes.size() )
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
        else
        {
            THROW_INVOCATION_EXCEPTION( "Invocation could not resolve target types" );
        }
    }
    pInvocation->set_return_types_context( contextReturnTypes );
    pInvocation->set_return_types_dimension( dimensionReturnTypes );
    pInvocation->set_homogeneous( bIsHomogenous );
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

    switch ( firstStageElimination( pInvocation->get_root_instruction() ) )
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
            std::vector< Operation* > operations;
            getOperations( pInvocation->get_root_instruction(), operations );

            std::vector< Concrete::Context* >   contexts;
            std::vector< Concrete::Dimension* > dimensions;
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

                        if ( pInterface->get_concrete() == pConcrete )
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
                        using OperationsStage::Invocations::Operations::Call;
                        using OperationsStage::Invocations::Operations::Start;

                        if ( Call* pCall = dynamic_database_cast< Call >( pOperation ) )
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

OperationsStage::Operations::Invocation* construct( io::Environment& environment, const mega::invocation::ID& id,
                                                    Database& database, const mega::io::megaFilePath& sourceFile )
{
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
    const Derivation::Mapping* pMapping    = database.one< Derivation::Mapping >( manifestFile );
    const InheritanceMapping   inheritance = pMapping->get_inheritance();

    std::vector< ElementVector* > contextElements  = toElementVector( database, inheritance, context );
    std::vector< ElementVector* > typePathElements = toElementVector( database, inheritance, typePath );

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
        pContext, pTypePath, id.m_operation, osName.str(), osContextStr.str(), osTypePathStr.str() } );

    // 3. Compute name resolution
    NameResolution* pNameResolution = resolve( database, pMapping, pInvocation );
    pInvocation->set_name_resolution( pNameResolution );

    // 4. Build the instructions
    build( database, pInvocation );

    std::vector< Interface::IContext* >       contexts   = pInvocation->get_return_types_context();
    std::vector< Interface::DimensionTrait* > dimensions = pInvocation->get_return_types_dimension();

    std::optional< std::string > strFunctionReturnTypeOpt;
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
            }
            else
            {
                bNonFunction = true;
            }
        }
        VERIFY_RTE_MSG( !strFunctionReturnTypeOpt.has_value() || !bNonFunction, "Invalid function return type" );
    }

    pInvocation->set_is_function_call( strFunctionReturnTypeOpt.has_value() );

    std::ostringstream osReturnTypeStr;
    {
        if ( !contexts.empty() )
        {
            if ( contexts.size() == 1 )
            {
                if ( strFunctionReturnTypeOpt.has_value() )
                {
                    osReturnTypeStr << strFunctionReturnTypeOpt.value();
                }
                else
                {
                    printIContextFullType( contexts.front(), osReturnTypeStr );
                }
            }
            else
            {
                osReturnTypeStr << EG_VARIANT_TYPE << "< ";
                bool bFirst = true;
                for ( Interface::IContext* pContext : contexts )
                {
                    if ( bFirst )
                        bFirst = false;
                    else
                        osReturnTypeStr << ", ";
                    printIContextFullType( pContext, osReturnTypeStr );
                }
                osReturnTypeStr << " >";
            }
        }
        else if ( !dimensions.empty() )
        {
            if ( pInvocation->get_homogeneous() )
            {
                printIContextFullType( dimensions.front()->get_parent(), osReturnTypeStr );
                osReturnTypeStr << "::" << dimensions.front()->get_id()->get_str();
                switch ( id.m_operation )
                {
                    case id_Imp_NoParams:
                        osReturnTypeStr << "::Read";
                        break;
                    case id_Imp_Params:
                        osReturnTypeStr << "::Write";
                        break;
                    case id_Get:
                        osReturnTypeStr << "::Read";
                        break;
                    case id_Start:
                    case id_Stop:
                    case id_Pause:
                    case id_Resume:
                    case id_Wait:
                    case id_Done:
                    case id_Range:
                    case id_Raw:
                    case HIGHEST_OPERATION_TYPE:
                    default:
                        THROW_RTE( "Invalid operation ID" );
                }
            }
            else
            {
                THROW_RTE( "Unsupported" );
            }
        }
        else
        {
            osReturnTypeStr << "void";
        }
    }

    pInvocation->set_return_type_str( osReturnTypeStr.str() );

    return pInvocation;
}

} // namespace invocation
} // namespace mega
