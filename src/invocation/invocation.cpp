
#include "invocation/invocation.hpp"
#include "invocation/name_resolution.hpp"

#include "mega/common.hpp"

namespace mega
{
namespace invocation
{
using namespace OperationsStage;
using namespace OperationsStage::Operations;

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

// 1. Convert from the SymbolIDs to Interface Contexts
InterfaceVariantVector symbolIDToInterfaceVariant( Database& database, const SymbolIDMap& symbolIDMap,
                                                   mega::SymbolID symbolID )
{
    auto iFind = symbolIDMap.find( symbolID );
    VERIFY_RTE( iFind != symbolIDMap.end() );
    return symbolToInterfaceVariantVector( database, iFind->second );
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
symbolIDVectorToInterfaceVariantVector( Database& database, const SymbolIDMap& symbolIDMap,
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
        InterfaceVariantVector interfaceVariantVector = symbolIDToInterfaceVariant( database, symbolIDMap, symbolID );
        VERIFY_RTE( !interfaceVariantVector.empty() );
        result.push_back( interfaceVariantVector );
    }

    return result;
}

// 2. Convert from Interface Contexts to Interface/Concrete context pairs

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
// 3. Compute name resolution

// 4. Solve the invocation instructions

// 5. Determine the return type

OperationsStage::Operations::Invocation* construct( io::Environment& environment, const mega::invocation::ID& id,
                                                    Database& database, const mega::io::megaFilePath& sourceFile )
{
    const mega::io::manifestFilePath manifestFile = environment.project_manifest();
    Symbols::SymbolTable*            pSymbolTable = database.one< Symbols::SymbolTable >( manifestFile );

    const SymbolIDMap symbolIDMap = pSymbolTable->get_symbol_id_map();

    std::optional< mega::OperationID > operationIDOpt;

    InterfaceVariantVectorVector context
        = symbolIDVectorToInterfaceVariantVector( database, symbolIDMap, id.m_context, operationIDOpt );

    InterfaceVariantVectorVector typePath
        = symbolIDVectorToInterfaceVariantVector( database, symbolIDMap, id.m_type_path, operationIDOpt );

    if ( operationIDOpt.has_value() )
    {
        THROW_RTE( "Invalid operation specified in invocation" );
    }

    const Derivation::Mapping* pMapping    = database.one< Derivation::Mapping >( manifestFile );
    const InheritanceMapping   inheritance = pMapping->get_inheritance();

    std::vector< ElementVector* > contextElements  = toElementVector( database, inheritance, context );
    std::vector< ElementVector* > typePathElements = toElementVector( database, inheritance, typePath );

    Context*  pContext  = database.construct< Context >( Context::Args{ contextElements } );
    TypePath* pTypePath = database.construct< TypePath >( TypePath::Args{ typePathElements } );

    Invocation* pInvocation
        = database.construct< Invocation >( Invocation::Args{ pContext, pTypePath, id.m_operation } );

    NameResolution* pNameResolution = resolve( database, pMapping, pSymbolTable, pInvocation );

    pInvocation->set_name_resolution( pNameResolution );

    return pInvocation;
}

} // namespace invocation
} // namespace mega
