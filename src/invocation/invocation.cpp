
#include "invocation/invocation.hpp"
#include "database/model/OperationsStage.hxx"
#include "mega/common.hpp"

namespace mega
{
namespace invocation
{
using namespace OperationsStage;
using namespace OperationsStage::Operations;

using SymbolIDMap = std::map< std::int32_t, Symbols::Symbol* >;

// 1. Convert from the SymbolIDs to Interface Contexts
std::vector< InterfaceVariant* > symbolIDToInterfaceVariant( Database& database, const SymbolIDMap& symbolIDMap,
                                                             mega::SymbolID symbolID )
{
    std::vector< InterfaceVariant* > result;

    auto iFind = symbolIDMap.find( symbolID );
    VERIFY_RTE( iFind != symbolIDMap.end() );

    Symbols::Symbol* pSymbol = iFind->second;

    for ( Interface::IContext* pContext : pSymbol->get_contexts() )
    {
        database.construct< InterfaceVariant >( InterfaceVariant::Args{ pContext, nullptr } );
    }

    for ( Interface::DimensionTrait* pDimension : pSymbol->get_dimensions() )
    {
        database.construct< InterfaceVariant >( InterfaceVariant::Args{ nullptr, pDimension } );
    }

    return result;
}

std::vector< std::vector< InterfaceVariant* > >
symbolIDVectorToInterfaceVariantVector( Database& database, const SymbolIDMap& symbolIDMap,
                                        const std::vector< mega::SymbolID >& symbolIDs,
                                        std::optional< mega::OperationID >&  operationIDOpt )
{
    std::vector< std::vector< InterfaceVariant* > > result;

    for ( mega::SymbolID symbolID : symbolIDs )
    {
        if ( isOperationType( symbolID ) )
        {
            operationIDOpt = static_cast< mega::OperationID >( symbolID );
            continue;
        }
        std::vector< InterfaceVariant* > i = symbolIDToInterfaceVariant( database, symbolIDMap, symbolID );
        VERIFY_RTE( !i.empty() );
        result.push_back( i );
    }

    return result;
}

// 2. Convert from Interface Contexts to Interface/Concrete context pairs

ElementVector* toElementVector( Database& database, const std::vector< InterfaceVariant* >& interfaceVariantVector )
{
    ElementVector* pElementVector = nullptr;

    std::vector< Element* > elements;

    for ( InterfaceVariant* pInterfaceVariant : interfaceVariantVector )
    {
        InterfaceVariant* pInterface = nullptr;
        ConcreteVariant*  pConcrete  = nullptr;

        if ( pInterfaceVariant->get_context().has_value() )
        {
            Interface::IContext* pContext = pInterfaceVariant->get_context().value();
            if( pContext->get_concrete().has_value() )
            {
                Concrete::Context* pConcrete = pContext->get_concrete().value();
                //
            }
            else
            {
                THROW_RTE( "Interface context missing concrete instance" );
            }
        }
        else if ( pInterfaceVariant->get_dimension().has_value() )
        {
        }
        else
        {
            THROW_RTE( "Missing context in interface variant" );
        }

        Element* pElement = database.construct< Element >( Element::Args{ pInterface, pConcrete } );
        elements.push_back( pElement );
    }

    pElementVector = database.construct< ElementVector >( ElementVector::Args{ elements } );

    return pElementVector;
}

// 3. Compute name resolution

// 4. Solve the invocation instructions

// 5. Determine the return type

OperationsStage::Operations::Invocation* construct( io::Environment& environment, const mega::invocation::ID& id,
                                                    Database& database, const mega::io::megaFilePath& sourceFile )
{
    Symbols::SymbolTable* pSymbolTable = database.one< Symbols::SymbolTable >( environment.project_manifest() );

    SymbolIDMap symbolIDMap = pSymbolTable->get_symbol_id_map();

    std::vector< Symbols::Symbol* > contextSymbols;
    {
        for ( mega::SymbolID typeID : id.m_context )
        {
            auto iFind = symbolIDMap.find( typeID );
            if ( iFind != symbolIDMap.end() )
            {
                contextSymbols.push_back( iFind->second );
            }
            else
            {
                // pASTContext->getDiagnostics().Report( loc, clang::diag::err_mega_generic_error )
                //     << "Invalid symbol ID: " << typeID;
                // m_bError = true;
                // return false;
            }
        }
    }
    std::vector< Symbols::Symbol* > typePathSymbols;
    {
        for ( mega::SymbolID typeID : id.m_type_path )
        {
            auto iFind = symbolIDMap.find( typeID );
            if ( iFind != symbolIDMap.end() )
            {
                typePathSymbols.push_back( iFind->second );
            }
            else
            {
                // pASTContext->getDiagnostics().Report( loc, clang::diag::err_mega_generic_error )
                //     << "Invalid symbol ID: " << typeID;
                // m_bError = true;
                // return false;
            }
        }
    }

    // Invocation* pInvocation = database.construct< Invocation >( Invocation::Args{ id } );

    // Invocations* pInvocations = database.one< Invocations >( sourceFile );
    // pInvocations->insert_invocations( id, pInvocation );

    return nullptr;
}

} // namespace invocation
} // namespace mega
