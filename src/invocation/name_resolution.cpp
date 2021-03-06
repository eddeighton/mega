
#include "invocation/name_resolution.hpp"
#include "invocation/invocation.hpp"

#include "database/model/OperationsStage.hxx"

namespace mega
{
namespace invocation
{

using namespace OperationsStage;

namespace
{

void expandReferences( OperationsStage::Database& database, const OperationsStage::Derivation::Mapping* pMapping, std::vector< Operations::Name* >& names )
{
    using namespace OperationsStage::Operations;

    bool bContinue = true;
    while ( bContinue )
    {
        bContinue = false;

        // names
        while ( bContinue )
        {
            bContinue = false;
            for ( Name* pName : names )
            {
                std::vector< Name* > children = pName->get_children();
                if ( children.empty() )
                {
                    ConcreteVariant* pConVar = pName->get_element()->get_concrete();
                    if ( pConVar->get_dimension().has_value() )
                    {
                        // if the dimension is to a mega type then add child nodes
                        bool bIsToMegaType = false;

                        Concrete::Dimension*       pConcreteDimension  = pConVar->get_dimension().value();
                        Interface::DimensionTrait* pInterfaceDimension = pConcreteDimension->get_interface_dimension();

                        if ( bIsToMegaType )
                        {
                            bContinue = true;
                            pName->set_is_reference( true );

                            std::vector< Symbols::Symbol* > symbols = pInterfaceDimension->get_symbols();

                            InterfaceVariantVectorVector interfaceVariantVectorVector
                                = symbolVectorToInterfaceVariantVector( database, symbols );

                            std::vector< Operations::ElementVector* > elementVector = toElementVector(
                                database, pMapping->get_inheritance(), interfaceVariantVectorVector );

                            for ( Operations::ElementVector* pElementVector : elementVector )
                            {
                                for ( Element* pElement : pElementVector->get_elements() )
                                {
                                    Name* pChildName = database.construct< Name >(
                                        Name::Args{ NameRoot::Args{ {} }, pElement, false, false } );
                                    names.push_back( pChildName );
                                    pName->push_back_children( pChildName );
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void addType( OperationsStage::Database&        database,
              std::vector< Operations::Name* >& names,
              Operations::ElementVector*        pElementVector )
{
    using namespace OperationsStage::Operations;

    std::vector< Operations::Name* > namesCopy = names;
    for ( Name* pName : namesCopy )
    {
        std::vector< Name* > children = pName->get_children();
        if ( children.empty() )
        {
            ConcreteVariant* pConVar = pName->get_element()->get_concrete();
            if ( !pConVar->get_context().has_value() )
            {
                THROW_RTE( "Cannot resolve non-context element in invocation" );
            }
            Concrete::Context* pContext = pConVar->get_context().value();

            std::vector< Element* > memberElements;
            for ( Element* pElement : pElementVector->get_elements() )
            {
                bool bFoundType = false;

                for ( Concrete::Context* pChildContext : pContext->get_children() )
                {
                    if ( pChildContext == pElement->get_concrete()->get_context() )
                    {
                        memberElements.push_back( pElement );
                        break;
                    }
                }

                // determine if pElement is member of pContext
                {
                    if ( Concrete::Namespace* pNamespace = dynamic_database_cast< Concrete::Namespace >( pContext ) )
                    {
                        bFoundType = true;
                        for ( Concrete::Dimension* pDimension : pNamespace->get_dimensions() )
                        {
                            if ( pDimension == pElement->get_concrete()->get_dimension() )
                            {
                                memberElements.push_back( pElement );
                                break;
                            }
                        }
                    }
                }
                {
                    if ( Concrete::Action* pAction = dynamic_database_cast< Concrete::Action >( pContext ) )
                    {
                        bFoundType = true;
                        for ( Concrete::Dimension* pDimension : pAction->get_dimensions() )
                        {
                            if ( pDimension == pElement->get_concrete()->get_dimension() )
                            {
                                memberElements.push_back( pElement );
                                break;
                            }
                        }
                    }
                }
                {
                    if ( Concrete::Event* pEvent = dynamic_database_cast< Concrete::Event >( pContext ) )
                    {
                        bFoundType = true;
                        for ( Concrete::Dimension* pDimension : pEvent->get_dimensions() )
                        {
                            if ( pDimension == pElement->get_concrete()->get_dimension() )
                            {
                                memberElements.push_back( pElement );
                                break;
                            }
                        }
                    }
                }
                {
                    if ( Concrete::Function* pFunction = dynamic_database_cast< Concrete::Function >( pContext ) )
                    {
                        bFoundType = true;
                    }
                }
                {
                    if ( Concrete::Object* pObject = dynamic_database_cast< Concrete::Object >( pContext ) )
                    {
                        bFoundType = true;
                        for ( Concrete::Dimension* pDimension : pObject->get_dimensions() )
                        {
                            if ( pDimension == pElement->get_concrete()->get_dimension() )
                            {
                                memberElements.push_back( pElement );
                                break;
                            }
                        }
                    }
                }
                {
                    if ( Concrete::Link* pLink = dynamic_database_cast< Concrete::Link >( pContext ) )
                    {
                        bFoundType = true;
                    }
                }
                {
                    if ( Concrete::Table* pTable = dynamic_database_cast< Concrete::Table >( pContext ) )
                    {
                        bFoundType = true;
                    }
                }

                VERIFY_RTE( bFoundType );
            }

            if ( !memberElements.empty() )
            {
                // add member matches
                for ( Element* pElement : memberElements )
                {
                    Name* pMemberName
                        = database.construct< Name >( Name::Args{ NameRoot::Args{ {} }, pElement, true, false } );
                    names.push_back( pMemberName );
                    pName->push_back_children( pMemberName );
                }
            }
            else
            {
                // add all
                for ( Element* pElement : pElementVector->get_elements() )
                {
                    Name* pChildName
                        = database.construct< Name >( Name::Args{ NameRoot::Args{ {} }, pElement, false, false } );
                    names.push_back( pChildName );
                    pName->push_back_children( pChildName );
                }
            }
        }
    }
}

void pruneBranches( OperationsStage::Operations::Name* pName )
{
    using namespace OperationsStage::Operations;

    auto children = pName->get_children();

    if ( !children.empty() )
    {
        if ( pName->get_is_reference() )
        {
            if ( !pName->get_is_member() )
            {
                for ( Name* pChild : children )
                {
                    pruneBranches( pChild );
                    if ( pChild->get_is_member() )
                    {
                        pName->set_is_member( true );
                    }
                }
            }
        }
        else
        {
            std::vector< Name* > best;
            bool                 bRemovedChild = false;
            for ( Name* pChild : children )
            {
                pruneBranches( pChild );
                if ( pChild->get_is_member() )
                    best.push_back( pChild );
                else
                    bRemovedChild = true;
            }
            if ( !best.empty() )
            {
                pName->set_is_member( true );
                if ( bRemovedChild )
                    pName->set_children( best );
            }
        }
    }
}

} // namespace

OperationsStage::Operations::NameResolution* resolve( OperationsStage::Database&                  database,
                                                      const OperationsStage::Derivation::Mapping* pMapping,
                                                      OperationsStage::Operations::Invocation*    pInvocation )
{
    bool bExpandFinalReferences = false;
    switch ( pInvocation->get_operation() )
    {
        case id_Imp_NoParams:
        case id_Imp_Params:
            break;
        case id_Start:
        case id_Stop:
        case id_Pause:
        case id_Resume:
        case id_Wait:
            break;
        case id_Get:
            break;
        case id_Done:
            bExpandFinalReferences = true;
            break;
        case id_Range:
            break;
        case id_Raw:
            break;
        case HIGHEST_OPERATION_TYPE:
        default:
            THROW_RTE( "Invalid operation type" );
    }

    using namespace OperationsStage::Operations;

    Operations::Context* pContext = pInvocation->get_context();

    Operations::TypePath* pTypePath = pInvocation->get_type_path();

    std::vector< Name* > names;
    NameRoot*            pNameRoot = database.construct< NameRoot >( NameRoot::Args{ {} } );

    for ( ElementVector* pElementVector : pContext->get_vectors() )
    {
        for ( Element* pElement : pElementVector->get_elements() )
        {
            Name* pName = database.construct< Name >( Name::Args{ NameRoot::Args{ {} }, pElement, false, false } );
            names.push_back( pName );
            pNameRoot->push_back_children( pName );
        }
    }

    for ( ElementVector* pElementVector : pTypePath->get_vectors() )
    {
        expandReferences( database, pMapping, names );
        addType( database, names, pElementVector );

        for ( Name* pName : pNameRoot->get_children() )
        {
            pruneBranches( pName );
        }
    }

    if ( bExpandFinalReferences )
    {
        expandReferences( database, pMapping, names );
        for ( Name* pName : pNameRoot->get_children() )
        {
            pruneBranches( pName );
        }
    }

    return database.construct< NameResolution >( NameResolution::Args{ pNameRoot } );
}

} // namespace invocation
} // namespace mega
